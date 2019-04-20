#include "RVariableInferenceDialog.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QDateTime>
#include <QtDebug>

#include "RMainWindow.h"
#include "RDomain.h"
#include "RContinuousRandomVariable.h"
#include "math.h"
#include "RPlotWidget.h"



#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_sort_vector.h>
#include <gsl/gsl_cdf.h>
#include <gsl/gsl_math.h>

RVariableInferenceDialog::RVariableInferenceDialog(QWidget *parent, RDomain *passedDomain)
: QDialog(parent)
{
	setupUi(this);

	theDomain = passedDomain;
	X = 0;

	connect(okButton, SIGNAL(clicked()), this, SLOT(onOkButtonClicked()));
	connect(browseButton, SIGNAL(clicked()), this, SLOT(browse()));
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
}

RVariableInferenceDialog::~RVariableInferenceDialog()
{
	deleteVectors();
}

void RVariableInferenceDialog::browse()
{
	// Get the file name from the user using QFileDialog
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open Input file"), QDir::currentPath(), "All Files (*.*)");
	if (fileName.isEmpty()) {
		return;
	}
	lineEdit->setText(fileName);
}

void RVariableInferenceDialog::setDomain(RDomain *passedDomain)
{
	theDomain = passedDomain;
}

void RVariableInferenceDialog::onOkButtonClicked()
{	
	QString filePath = lineEdit->text();
	filePath.remove('"');

	if (filePath.isEmpty()) {
		QMessageBox::critical(this, tr("Error"), tr("Please choose enter a file name and try again."), QMessageBox::Ok | QMessageBox::Default);
		return;
	}
	RMainWindow *mainWindow = qobject_cast<RMainWindow *>(this->parent());

	accept();

	mainWindow->raiseDock("Output");


	// Read the file here
	QFile file(filePath);
	if (!file.open(QFile::ReadOnly /*| QFile::Text*/)) {
		// Error
		RObject::rCritical() << "Error: Could not open the file" << filePath << ".";
		return;
	}

	// Read the file content into a QString
	QTextStream stream(&file);
	stream.seek(0);
	//QString fileContentString = in.readAll();

	// Tokenize the QString of file contents by "\n" and create a string list of input file lines
	QString verticalRegExpString = QString("[n\n\v\r%1%2]").arg(QChar(QChar::ParagraphSeparator)).arg(QChar(QChar::LineSeparator));
	QRegExp verticalRegExp = QRegExp(verticalRegExpString);
	//QStringList fileContentStringList = fileContentString.split(verticalRegExp, QString::SkipEmptyParts);

	QStringList fileContentStringList;
	QString line;
	QStringList lineList;
	stream.setAutoDetectUnicode(true);
	do {
		line = stream.readLine();
		if (!line.isEmpty()) {
			lineList = line.split(verticalRegExp, QString::SkipEmptyParts);
			fileContentStringList.append(lineList);
		}
		//QCoreApplication::processEvents();
	} while (!line.isNull());

	int size = fileContentStringList.count();

	QRegExp regExp = QRegExp("[ ,;\t]");

	int columnCount = 0;


	for (int i = 0; i < fileContentStringList.count(); i++) {


		QStringList valueList = fileContentStringList[i].split(regExp, QString::SkipEmptyParts);

		if (i == 0) {
			columnCount = valueList.count();

			X = gsl_matrix_calloc(size, columnCount);
		} else {
			if (columnCount != valueList.count()) {
				RObject::rCritical() << "Error: The number of data at the line" << i + 1 << "is different than the line 1 in the database file.";
				deleteVectors();
				return;
			}
		}

		for (int j = 0; j < valueList.count(); j++) {
			QString tempStr = valueList[j].trimmed();
			bool ok;
			double tempValue = tempStr.toDouble(&ok);
			if (!ok) {
				RObject::rCritical() << "Error: Could not read the value number" << j + 1 << "at the line" << i + 1 << "in the database file.";
				deleteVectors();
				return;
			}

			// The first value in the line is the output

			gsl_matrix_set(X, i, j, tempValue);
		}
	}

	file.close();


	// Execute the 'analyze' method
	int result = this->analyze();
	if (result < 0) {
		RObject::rCritical() << "Error: The variable inference analysis did not converge because of errors.";
		return;
	}
	mainWindow->updateTree("Domain");
}

int RVariableInferenceDialog::analyze()
{
	// Inform the user that the sampling analysis has started
	RObject::rDebug() << "##### STARTING VARIABLE INFERENCE ANALYSIS ...";
	RObject::rDebug() << "";


	// Start recording the time
	QDateTime startTime = QDateTime::currentDateTime();


	// Get a hold of the main window, for plotting purposes
	RMainWindow *mainWindow = qobject_cast<RMainWindow *>(parent());



	if (!X) {
		qCritical() << "Error: The matrix of regressors and/or the vector of regressand have not been read from the text file successfully. Please revise the file and try again.";
		return -1;
	}


	// Get number of random variables and observations
	int N = X->size1;
	int numRVs = X->size2;

	// Allocate a vector that will be used in the single-variable inference
	gsl_vector *variableVector = gsl_vector_calloc(N);

	// Allocate vectors to store means and standard deviations
	gsl_vector *means = gsl_vector_calloc(numRVs);
	gsl_vector *stdvs = gsl_vector_calloc(numRVs);

	// Allocate vectors to store min and max values
	gsl_vector *mins = gsl_vector_calloc(numRVs);
	gsl_vector *maxs = gsl_vector_calloc(numRVs);


	for (int j=0; j<numRVs; j++) {

		// Collect the variable vector from X
		for (int i=0; i<N; i++) {
			gsl_vector_set(variableVector,i,gsl_matrix_get(X,i,j));
		}

		// Get max and min values for later plots
		gsl_vector_set(mins,j,(gsl_vector_min(variableVector)));
		gsl_vector_set(maxs,j,(gsl_vector_max(variableVector)));


#ifndef NO_QCP

		int plotNum = 0;

		// Create a plot
		QPointer<RPlotWidget> thePlot = new RPlotWidget(mainWindow);
		QString str = QString("Cumulative Frequency Diagram for Column %1").arg(j + 1);
		mainWindow->addSubWindow(thePlot, str, true);
		thePlot->xAxis->setLabel("Random Variable");
		thePlot->yAxis->setLabel("Cumulative Frequency");


		// Create the x and y vectors and the curve to be plotted
		QVector<double> xValues;
		QVector<double> yValues;
		thePlot->addGraph();
		thePlot->graph(0)->setName("Data");
		thePlot->graph(0)->setPen(QPen(Qt::black));
		QCPGraph *theCDFcurve = thePlot->graph(0);


		//  Set properties of the line and attach it to the plot
		/*QPen thePen;
		thePen.setColor(QColor(Qt::black));
		theCDFcurve->setPen(thePen);
		theCDFcurve->setRenderHint(QwtPlotItem::RenderAntialiased, true);
		theCDFcurve->attach(thePlot);*/
#endif // NO_QCP




		// Sort the elements of the vector into ascending order
		gsl_sort_vector(variableVector);


		// Compute the sum, the sum squared, and send data points to the plot curve
		double value;
		double vectorSum = 0.0;
		double vectorSumSquared = 0.0;
		for (int i=0; i<N; i++) {
			value = gsl_vector_get(variableVector,i);
			vectorSum += value;
			vectorSumSquared += value*value;
			xValues << value;
			yValues << (i+1)/(double)N;
		}
#ifndef NO_QCP
theCDFcurve->setData(xValues, yValues);
#endif // NO_QCP


		// Compute the mean (and create a QString with its value)
		double mean = vectorSum/(double)N;
		gsl_vector_set(means, j, mean);


		// Compute the variance
		double variance = 1.0/(N-1.0) * (vectorSumSquared - N * gsl_pow_2(mean));


		// Compute the standard deviation
		double stdv = sqrt(variance);
		gsl_vector_set(stdvs, j, stdv);


		// Print results to the screen
		RObject::rDebug() << "";
		RObject::rDebug() << "The mean of the data in column" << (j+1) << "is:" << mean;
		RObject::rDebug() << "The standard deviation of the data in column" << (j+1) << "is:" << stdv;
		RObject::rDebug() << "Consequently, the coefficient of variation (in percent) of the data in column" << (j+1) << "is:" << 100.0*stdv/mean;


		// For the next steps, get the max and min of the vector
		double vectorMin = gsl_vector_min(variableVector);
		double vectorMax = gsl_vector_max(variableVector);
		int numPoints = 30;


		// Declare a random variable to be used in the following plotting
		RContinuousRandomVariable theRV(0, "theRandomVariable");
		theRV.setMean(QString("%1").arg(mean, 0, 'g', 14));
		theRV.setStandardDeviation(QString("%1").arg(stdv, 0, 'g', 14));
		QVector<double> xDistrValues;
		QVector<double> yDistrValues;
		for (int i=0; i<numPoints+1; i++) {
			value = vectorMin + (vectorMax-vectorMin)*i/(double)numPoints;
			xDistrValues << value;
		}

#ifndef NO_QCP

		// Add a NORMAL line to the plot
		if (NormalPlotCheckBox->isChecked()) {
			plotNum++;
			yDistrValues.clear();
			theRV.setDistributionType("Normal (mean, stdv)");

			thePlot->addGraph();
			thePlot->graph(plotNum)->setName("Normal");
			thePlot->graph(plotNum)->setPen(QPen(Qt::red));
			QCPGraph *theNormalCDFcurve = thePlot->graph(plotNum);

			for (int i = 0; i < numPoints + 1; i++) {
				yDistrValues << theRV.getCDFValue(xDistrValues[i], 0);
			}
			theNormalCDFcurve->setData(xDistrValues, yDistrValues);
		}


		// Add a LOGNORMAL line to the plot
		if (LognormalPlotCheckBox->isChecked()) {
			plotNum++;
			yDistrValues.clear();
			theRV.setDistributionType("Lognormal (zeta, sigma)");
			thePlot->addGraph();
			thePlot->graph(plotNum)->setName("Lognormal");
			thePlot->graph(plotNum)->setPen(QPen(Qt::blue));
			QCPGraph *theLognormalCDFcurve = thePlot->graph(plotNum);

			for (int i = 0; i < numPoints + 1; i++) {
				yDistrValues << theRV.getCDFValue(xDistrValues[i], 0);
			}
			theLognormalCDFcurve->setData(xDistrValues, yDistrValues);
		}


		// Add a EXPONENTIAL line to the plot
		if (ExponentialPlotCheckBox->isChecked()) {
			plotNum++;
			yDistrValues.clear();
			theRV.setDistributionType("Exponential (mu, xo)");
			thePlot->addGraph();
			thePlot->graph(plotNum)->setName("Exponential");
			thePlot->graph(plotNum)->setPen(QPen(Qt::darkRed));
			QCPGraph *theExponentialCDFcurve = thePlot->graph(plotNum);

			for (int i = 0; i < numPoints + 1; i++) {
				yDistrValues << theRV.getCDFValue(xDistrValues[i], 0);
			}
			theExponentialCDFcurve->setData(xDistrValues, yDistrValues);
		}


		// Add a UNIFORM line to the plot
		if (UniformPlotCheckBox->isChecked()) {
			plotNum++;
			yDistrValues.clear();
			theRV.setDistributionType("Uniform (a, b)");
			thePlot->addGraph();
			thePlot->graph(plotNum)->setName("Uniform");
			thePlot->graph(plotNum)->setPen(QPen(Qt::green));
			QCPGraph *theUniformCDFcurve = thePlot->graph(plotNum);

			for (int i = 0; i < numPoints + 1; i++) {
				yDistrValues << theRV.getCDFValue(xDistrValues[i], 0);
			}
			theUniformCDFcurve->setData(xDistrValues, yDistrValues);
		}

		// Add a LAPLACE line to the plot
		if (LaplacePlotCheckBox->isChecked()) {
			plotNum++;
			yDistrValues.clear();
			theRV.setDistributionType("Laplace (a, xo)");
			thePlot->addGraph();
			thePlot->graph(plotNum)->setName("Laplace");
			thePlot->graph(plotNum)->setPen(QPen(Qt::darkGreen));
			QCPGraph *theLaplaceCDFcurve = thePlot->graph(plotNum);

			for (int i = 0; i < numPoints + 1; i++) {
				yDistrValues << theRV.getCDFValue(xDistrValues[i], 0);
			}
			theLaplaceCDFcurve->setData(xDistrValues, yDistrValues);
		}


		// Add a RAYLEIGH line to the plot
		if (RayleighPlotCheckBox->isChecked()) {
			plotNum++;
			yDistrValues.clear();
			theRV.setDistributionType("Rayleigh (sigma, xo)");
			thePlot->addGraph();
			thePlot->graph(plotNum)->setName("Rayleigh");
			thePlot->graph(plotNum)->setPen(QPen(Qt::darkBlue));
			QCPGraph *theRayleighCDFcurve = thePlot->graph(plotNum);

			for (int i = 0; i < numPoints + 1; i++) {
				yDistrValues << theRV.getCDFValue(xDistrValues[i], 0);
			}
			theRayleighCDFcurve->setData(xDistrValues, yDistrValues);
		}


		// Add a GAMMA line to the plot
		if (GammaPlotCheckBox->isChecked()) {
			plotNum++;
			yDistrValues.clear();
			theRV.setDistributionType("Gamma (a, b)");
			thePlot->addGraph();
			thePlot->graph(plotNum)->setName("Gamma");
			thePlot->graph(plotNum)->setPen(QPen(Qt::cyan));
			QCPGraph *theGammaCDFcurve = thePlot->graph(plotNum);

			for (int i = 0; i < numPoints + 1; i++) {
				yDistrValues << theRV.getCDFValue(xDistrValues[i], 0);
			}
			theGammaCDFcurve->setData(xDistrValues, yDistrValues);
		}


		// Add a CHI SQUARE line to the plot
		if (ChiSquarePlotCheckBox->isChecked()) {
			plotNum++;
			yDistrValues.clear();
			theRV.setDistributionType("Chi-square (nu, xo)");
			thePlot->addGraph();
			thePlot->graph(plotNum)->setName("Chi-square");
			thePlot->graph(plotNum)->setPen(QPen(Qt::magenta));
			QCPGraph *theChiSquaredCDFcurve = thePlot->graph(plotNum);

			for (int i = 0; i < numPoints + 1; i++) {
				yDistrValues << theRV.getCDFValue(xDistrValues[i], 0);
			}
			theChiSquaredCDFcurve->setData(xDistrValues, yDistrValues);
		}


		// Add a LOGISTIC line to the plot
		if (LogisticPlotCheckBox->isChecked()) {
			plotNum++;
			yDistrValues.clear();
			theRV.setDistributionType("Logistic (a, xo)");
			thePlot->addGraph();
			thePlot->graph(plotNum)->setName("Logistic");
			thePlot->graph(plotNum)->setPen(QPen(Qt::darkMagenta));
			QCPGraph *theLogisticCDFcurve = thePlot->graph(plotNum);

			for (int i = 0; i < numPoints + 1; i++) {
				yDistrValues << theRV.getCDFValue(xDistrValues[i], 0);
			}
			theLogisticCDFcurve->setData(xDistrValues, yDistrValues);
		}


		// Add a GUMBEL line to the plot
		if (GumbelPlotCheckBox->isChecked()) {
			plotNum++;
			yDistrValues.clear();
			theRV.setDistributionType("Gumbel (a, b)");
			thePlot->addGraph();
			thePlot->graph(plotNum)->setName("Gumbel");
			thePlot->graph(plotNum)->setPen(QPen(Qt::gray));
			QCPGraph *theGumbelCDFcurve = thePlot->graph(plotNum);

			for (int i = 0; i < numPoints + 1; i++) {
				yDistrValues << theRV.getCDFValue(xDistrValues[i], 0);
			}
			theGumbelCDFcurve->setData(xDistrValues, yDistrValues);
		}


		// Display the plot

		thePlot->xAxis->rescale(true);
		thePlot->yAxis->rescale(true);
		thePlot->xAxis->scaleRange(1.1);
		thePlot->yAxis->scaleRange(1.1);
		thePlot->replot();
#endif // NO_QCP

	}


	// Compute correlation matrix
	if (numRVs>1) {
		double productSum;
		double value;
		double mini, maxi, minj, maxj;
		gsl_matrix *R = gsl_matrix_calloc(numRVs, numRVs);
		for (int i=0; i<numRVs; i++) {
			gsl_matrix_set(R,i,i, 1.0);
		}
		for (int i=0; i<numRVs; i++) {
			for (int j=0; j<i; j++) {


	#ifndef NO_QCP

				// Create scatter plots
				QPointer<RPlotWidget> thePlot = new RPlotWidget(mainWindow);
				QString str = QString("Scatter Plot for Columns %1 and %2").arg(j + 1).arg(i + 1);
				mainWindow->addSubWindow(thePlot, str, true);
				thePlot->xAxis->setLabel(QString("Data in Column %1").arg(j + 1));
				thePlot->yAxis->setLabel(QString("Data in Column %1").arg(i + 1));

				for (int k = 0; k < N; k++) {

					thePlot->addGraph();
					thePlot->graph(k)->setLineStyle(QCPGraph::lsNone);
					thePlot->graph(k)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 4));

					QCPGraph *theMarker = thePlot->graph(k);
					theMarker->addData(gsl_matrix_get(X, k, j), gsl_matrix_get(X, k, i));
				}

				mini = gsl_vector_get(mins, i);
				maxi = gsl_vector_get(maxs, i);
				minj = gsl_vector_get(mins, j);
				maxj = gsl_vector_get(maxs, j);

				thePlot->yAxis->setRange(mini - 0.1*(maxi - mini), maxi + 0.1*(maxi - mini));
				thePlot->xAxis->setRange(minj - 0.1*(maxj - minj), maxj + 0.1*(maxj - minj));
	#endif // NO_QCP





				// Compute the correlation coefficient
				productSum = 0.0;
				for (int k=0; k<N; k++) {
					productSum += gsl_matrix_get(X,k,i) * gsl_matrix_get(X,k,j);
				}
				value = 1.0/(N-1.0) * (productSum - N * gsl_vector_get(means,i) * gsl_vector_get(means,j)  ) / (gsl_vector_get(stdvs,i) * gsl_vector_get(stdvs,j));
				gsl_matrix_set(R,i,j, value);
				gsl_matrix_set(R,j,i, value);
			}
		}
		RObject::rDebug() << "";
		RObject::rDebug() << "The correlation matrix is:";
		theDomain->print(R, 0);
		gsl_matrix_free(R);
	}


	// Free vectors/matrices
	gsl_vector_free(mins);
	gsl_vector_free(maxs);
	gsl_vector_free(variableVector);
	gsl_vector_free(means);
	gsl_vector_free(stdvs);


	QDateTime finishTime = QDateTime::currentDateTime();
	double numDays = startTime.daysTo(finishTime);
	double numMilliSecs = startTime.time().msecsTo(finishTime.time());
	double numSecs = numDays * 86400.0 + numMilliSecs / 1000.0;

	RObject::rDebug() << "";
	RObject::rDebug() << "##### VARIABLE INFERENCE ANALYSIS DONE IN" << numSecs << "SECONDS.";
	RObject::rDebug() << "";
	RObject::rDebug() << "";

	return 0;
}

void RVariableInferenceDialog::deleteVectors()
{
	if (X) {
		gsl_matrix_free(X);
		X = 0;
	}
}
