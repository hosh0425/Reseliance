#include "RParametricAnalysisDialog.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QDateTime>

#include "RMainWindow.h"
#include "RDomain.h"
#include "RFunction.h"
#include "RParameter.h"
#include "RPlotWidget.h"
#include "RAnalyzer.h"

#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

RParametricAnalysisDialog::RParametricAnalysisDialog(QWidget *parent, RDomain *passedDomain)
	: QDialog(parent)
{
	setupUi(this);

	theDomain = passedDomain;

	QList<RFunction *> functionList = theDomain->findChildren<RFunction *>();
	QString functionNames;
	for (int i = 0; i < functionList.count(); i++) {
		functionNames += functionList[i]->objectName() + "; ";
	}
	functionsLineEdit->setText(functionNames);

	QList<RParameter *> parameterList = theDomain->findChildren<RParameter *>();
	QString parameterNames;
	QString parameterMinimums;
	QString parameterMaximums;
	QString parameterCounts;
	for (int i = 0; i < parameterList.count(); i++) {
		if (parameterList[i]->inherits("RResponse") || parameterList[i]->inherits("RLocation") || parameterList[i]->inherits("RCorrelation")) {
			continue;
		}
		parameterNames += parameterList[i]->objectName() + "; ";
		parameterMinimums += QString("%1; ").arg(parameterList[i]->getCurrentValue() / 2.0);
		parameterMaximums += QString("%1; ").arg(parameterList[i]->getCurrentValue() * 1.5);
		parameterCounts += QString("%1; ").arg(5);
	}
	parametersLineEdit->setText(parameterNames);
	minLineEdit->setText(parameterMinimums);
	maxLineEdit->setText(parameterMaximums);
	countLineEdit->setText(parameterCounts);

	QList<RAnalyzer *> analyzerList = theDomain->findChildren<RAnalyzer *>();
	QStringList analyzerNameList;
	for (int i = 0; i < analyzerList.count(); i++) {
		analyzerNameList << analyzerList[i]->objectName();
	}
	analyzerComboBox->addItems(analyzerNameList);

	bool ok = connect(okButton, SIGNAL(clicked()), this, SLOT(onOkButtonClicked()));
	ok = connect(browseButton, SIGNAL(clicked()), this, SLOT(browse()));
	ok = connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
	ok = connect(functionGroupBox, SIGNAL(clicked(bool)), this, SLOT(onFunctionGroupBoxClicked(bool)));
	ok = connect(analyzerGroupBox, SIGNAL(clicked(bool)), this, SLOT(onAnalyzerGroupBoxClicked(bool)));
	ok = connect(parent, SIGNAL(subWindowAdded(QWidget *)), this, SLOT(onSubWindowAdded(QWidget *)));
}

RParametricAnalysisDialog::~RParametricAnalysisDialog()
{

}

void RParametricAnalysisDialog::setDomain(RDomain *passedDomain)
{
	theDomain = passedDomain;
}

void RParametricAnalysisDialog::browse()
{
	// Get the file name from the user using QFileDialog
	QString fileName = QFileDialog::getSaveFileName(this, tr("Save output File"), QDir::currentPath(), "Test Files (*.txt)");
	if (fileName.isEmpty()) {
		return;
	}
	outputFileLineEdit->setText(fileName);
}

void RParametricAnalysisDialog::onOkButtonClicked()
{
	RMainWindow *mainWindow = qobject_cast<RMainWindow *>(this->parent());

	QRegExp regExp = QRegExp(QString("[ ,;:\t\n\v\r%1%2]").arg(QChar(QChar::ParagraphSeparator)).arg(QChar(QChar::LineSeparator)));

	// PARAMETERS
	QList<RParameter *> parameterList;
	QString parametersString = parametersLineEdit->text();
	QStringList parameterStringList = parametersString.split(regExp, QString::SkipEmptyParts);
	// Issue an error if the nothing is entered in the parametersLineEdit
	if (parameterStringList.isEmpty()) {
		QMessageBox::critical(this, tr("Error"), tr("Select at least one parameter and try again."), QMessageBox::Ok | QMessageBox::Default);
		return;
	}
	// Create a list of parameters from parameterLineEdit
	for (int i = 0; i < parameterStringList.count(); i++) {
		RParameter *parameter = theDomain->findChild<RParameter *>(parameterStringList[i]);
		if (parameter) {
			parameterList << parameter;
		} else {
			// Issue an error if the parameter name does not exist in the domain
			QMessageBox::critical(this, tr("Error"), QString("The parameter %1 does not exist. Please correct the name and try again.").arg(parameterStringList[i]), QMessageBox::Ok | QMessageBox::Default);
			return;
		}
	}

	// MINIMUM
	QVector<double> minimumVector;
	QString minimumsString = minLineEdit->text();
	QStringList minimumStringList = minimumsString.split(regExp, QString::SkipEmptyParts);
	// Issue an error if the nothing is entered in the minimumsLineEdit
	if (minimumStringList.count() != parameterStringList.count()) {
		QMessageBox::critical(this, tr("Error"), tr("Enter as many minimum values as the number of parameters."), QMessageBox::Ok | QMessageBox::Default);
		return;
	}
	// Create a list of minimums from minimumLineEdit
	for (int i = 0; i < minimumStringList.count(); i++) {
		bool ok;
		double minimum = minimumStringList[i].toDouble(&ok);
		if (ok) {
			minimumVector << minimum;
		} else {
			// Issue an error if the minimum name does not exist in the domain
			QMessageBox::critical(this, tr("Error"), QString("The entry %1 in the list of minimum values is not a number. Please try again.").arg(minimumStringList[i]), QMessageBox::Ok | QMessageBox::Default);
			return;
		}
	}

	// MAXIMUM
	QVector<double> maximumVector;
	QString maximumsString = maxLineEdit->text();
	QStringList maximumStringList = maximumsString.split(regExp, QString::SkipEmptyParts);
	// Issue an error if the nothing is entered in the maximumsLineEdit
	if (maximumStringList.count() != parameterStringList.count()) {
		QMessageBox::critical(this, tr("Error"), tr("Enter as many maximum values as the number of parameters."), QMessageBox::Ok | QMessageBox::Default);
		return;
	}
	// Create a list of maximums from maximumLineEdit
	for (int i = 0; i < maximumStringList.count(); i++) {
		bool ok;
		double maximum = maximumStringList[i].toDouble(&ok);
		if (ok) {
			maximumVector << maximum;
		} else {
			// Issue an error if the maximum name does not exist in the domain
			QMessageBox::critical(this, tr("Error"), QString("The entry %1 in the list of maximum values is not a number. Please try again.").arg(maximumStringList[i]), QMessageBox::Ok | QMessageBox::Default);
			return;
		}
	}

	// COUNT
	QVector<int> countVector;
	QString countsString = countLineEdit->text();
	QStringList countStringList = countsString.split(regExp, QString::SkipEmptyParts);
	// Issue an error if the nothing is entered in the countsLineEdit
	if (countStringList.count() != parameterStringList.count()) {
		QMessageBox::critical(this, tr("Error"), tr("Enter as many values as the number of parameters in the list of the number of values."), QMessageBox::Ok | QMessageBox::Default);
		return;
	}
	// Create a list of counts from countLineEdit
	for (int i = 0; i < countStringList.count(); i++) {
		bool ok;
		int count = countStringList[i].toInt(&ok);
		if (ok) {
			countVector << count;
		} else {
			// Issue an error if the count name does not exist in the domain
			QMessageBox::critical(this, tr("Error"), QString("The entry %1 in the list of the number of values is not a number. Please try again.").arg(countStringList[i]), QMessageBox::Ok | QMessageBox::Default);
			return;
		}
	}

	// FUNCTIONS OR ANALYZERS
	QList<RFunction *> functionList;
	RAnalyzer *analyzer;
	if (functionGroupBox->isChecked()) {
		QString functionsString = functionsLineEdit->text();
		QStringList functionStringList = functionsString.split(regExp, QString::SkipEmptyParts);
		// Issue an error if the nothing is entered in the functionLineEdit
		if (functionStringList.isEmpty()) {
			QMessageBox::critical(this, tr("Error"), tr("Select at least one function and try again."), QMessageBox::Ok | QMessageBox::Default);
			return;
		}
		// Create a list of functions from functionsLineEdit
		for (int i = 0; i < functionStringList.count(); i++) {
			RFunction *function = theDomain->findChild<RFunction *>(functionStringList[i]);
			if (function) {
				functionList << function;
			} else {
				// Issue an error if the function name does not exist in the domain
				QMessageBox::critical(this, tr("Error"), QString("The function %1 does not exist. Please correct the name and try again.").arg(functionStringList[i]), QMessageBox::Ok | QMessageBox::Default);
				return;
			}
		}
	} else if (analyzerGroupBox->isChecked()) {
		if (analyzerComboBox->currentIndex() > -1) {
			analyzer = theDomain->findChild<RAnalyzer *>(analyzerComboBox->currentText());
		} else {
			QMessageBox::critical(this, tr("Error"), tr("Please choose an analyzer and try again."), QMessageBox::Ok | QMessageBox::Default);
			return;
		}
	}


	accept();
	
	mainWindow->raiseDock("Output");

	// Getting a backup from the value of the parameters
	QVector<double> backupVector;
	for (int i = 0; i < parameterList.count(); i++) {
		backupVector << parameterList[i]->getCurrentValue();
	}

	// Run thefunction evaluation analysis
	int result = this->analyze(parameterList, minimumVector, maximumVector, countVector, functionList, analyzer);
	if (result < 0) {		
		RObject::rCritical() << "Error: The data generation is stopped because of errors.";
		return;
	}

	// Reverting the parameter values back to the backup values
	for (int i = 0; i < parameterList.count(); i++) {
		parameterList[i]->setCurrentValue(backupVector[i]);
	}
}

int RParametricAnalysisDialog::analyze(QList<RParameter *> passedParameterList, QVector<double> passedMinimumVector, QVector<double> passedMaximumVector, QVector<int> passedCountVector, QList<RFunction *> passedFunctionList, RAnalyzer *passedAnalyzer)
{
	RObject::rDebug() << "##### STARTING PARAMETRIC ANALYSIS ...";
	RObject::rDebug() << "";

	QDateTime startTime = QDateTime::currentDateTime();

	
	int varCount = passedParameterList.count();
	int funCount = passedFunctionList.count();
	QVector<int> currentIndexVector;	
	QVector<int> numVector = passedCountVector;
	QVector<double> maxVector = passedMaximumVector;
	QVector<double> minVector = passedMinimumVector;
	for (int i = 0; i < varCount; i++) {
		currentIndexVector << -1;
	}

	QList<QVector<double> > dataVectorList;
	QList<QVector<double> > resultVectorList;

	QStringList outputStringList;

	if (functionGroupBox->isChecked() && printCheckBox->isChecked()) {
		QString titleStr;
		for (int i = 0; i < funCount; i++) {
			titleStr += passedFunctionList[i]->objectName() + " ";
		}
		titleStr += "; ";

		for (int i = 0; i < varCount; i++) {
			titleStr += passedParameterList[i]->objectName() + " ";
		}
		RObject::rDebug() << qPrintable(titleStr);
	}

	// Create a random number generator
	const gsl_rng_type *randomGeneratorType;
	gsl_rng *randomGenerator = 0;
	if (randomRadioButton->isChecked() || randomGridRadioButton->isChecked()) {
		gsl_rng_env_setup();
		randomGeneratorType = gsl_rng_default;
		randomGenerator = gsl_rng_alloc(randomGeneratorType);
	}

	// Start the parametric analysis
	for (int i = 0; i < varCount; i++) {
		currentIndexVector[i]++;
		if (currentIndexVector[i] == numVector[i]) {
			currentIndexVector[i] = -1;
			i--;
			i--;
			if (i == -2) {
				break;
			}
			continue;
		}

		if (i == varCount - 1) {
			QString str;
			QVector<double> dataVector;
			for (int j = 0; j < varCount; j++) {
				double value;
				if (gridRadioButton->isChecked()) {
					value = minVector[j] + (maxVector[j] - minVector[j]) * currentIndexVector[j] / (numVector[j] - 1);
				} else if (randomGridRadioButton->isChecked()) {
					double minGridValue = minVector[j] + (maxVector[j] - minVector[j]) * currentIndexVector[j] / numVector[j];
					double maxGridValue = minVector[j] + (maxVector[j] - minVector[j]) * (currentIndexVector[j] + 1) / numVector[j];
					value = gsl_ran_flat(randomGenerator, minGridValue, maxGridValue);
				} else if (randomRadioButton->isChecked()) {
					value = gsl_ran_flat(randomGenerator, minVector[j], maxVector[j]);
				}
				dataVector << value;
				passedParameterList[j]->setCurrentValue(value);
				str += QString("%1 ").arg(value, 0, 'g', 12);
			}
			dataVectorList << dataVector;

			// Do the following if the user has selected function evaluation
			if (functionGroupBox->isChecked()) {
				QString functionStr;
				QVector<double> resultVector;			

				for (int j = 0; j < funCount; j++) {
					passedFunctionList[j]->updateAnalysisStatus(0);
				}

				for (int j = 0; j < funCount; j++) {
					passedFunctionList[j]->evaluateFunction(false, RObject::RandomVariable,0);
					double functionValue = passedFunctionList[j]->getFunctionValue();
					resultVector << functionValue;
					QCoreApplication::processEvents();

					functionStr += QString("%1 ").arg(functionValue, 0, 'g', 12);
				}
				resultVectorList << resultVector;

				str = QString("%1; ").arg(functionStr) + str;
				
				outputStringList << str;
				if (printCheckBox->isChecked()) {
					RObject::rDebug() << qPrintable(str);
				}
			} 
			
			// Do the following if the user has selected other types of analysis
			else if (analyzerGroupBox->isChecked()) {

				QString parameterValuesString = "Analysis for ";
				for (int j = 0; j < varCount; j++) {
					parameterValuesString += QString("%1=%2").arg(passedParameterList[j]->objectName()).arg(passedParameterList[j]->getCurrentValue(), 0, 'g', 12);
					theCurrentParameterValuesStr = parameterValuesString;
					if (j < varCount - 1) {
						parameterValuesString += ", ";
						theCurrentParameterValuesStr += ", ";
					} else {
						parameterValuesString += ":";
					}
				}
				RObject::rDebug() << qPrintable(parameterValuesString);


				int result = passedAnalyzer->startAnalyzing();
				if (result < 0) {
					RObject::rCritical() << "Warning: The analysis at the current realization of parameter(s) did not run because of errors.";
				}
				RObject::rDebug() << "";
				RObject::rDebug() << "";
				RObject::rDebug() << "";
				RObject::rDebug() << "";
			}


			if (currentIndexVector[i] < numVector[i]) {
				i--;
			}
		}
	}

	if (randomGenerator) {
		gsl_rng_free(randomGenerator);
	}

	// Write the generated data to an output file if the user has asked for it
	if ((!outputFileLineEdit->text().isEmpty()) && (functionGroupBox->isChecked())) {
		QString fileName = outputFileLineEdit->text();
		if (QFileInfo(fileName).suffix().toLower() != QString("txt").toLower()) {
			fileName += ".txt";
		}

		 // Open the file
		QFile file(fileName);
		if (!file.open(QFile::WriteOnly | QFile::Text)) {
			// Error
			QMessageBox::critical(this, tr("Error"), QString("Could not create the file %1.").arg(fileName), QMessageBox::Ok | QMessageBox::Default);
			return 0;
		}

		for (int i = 0; i < outputStringList.count(); i++) {
			QByteArray byteArray;
			byteArray.append(outputStringList[i] + "\n");

			file.write(byteArray);
			file.flush();
		}

		// Close the file
		file.close();
	}

	QDateTime finishTime = QDateTime::currentDateTime();
	double numDays = startTime.daysTo(finishTime);
	double numMilliSecs = startTime.time().msecsTo(finishTime.time());
	double numSecs = numDays * 86400.0 + numMilliSecs / 1000.0;

	RObject::rDebug() << "";
	RObject::rDebug() << "##### PARAMETRIC ANALYSIS DONE IN" << numSecs << "SECONDS.";
	RObject::rDebug() << "";
	RObject::rDebug() << "";


#ifndef NO_QCP

	if (plot2DCheckBox->isChecked() && functionGroupBox->isChecked()) {
		if (varCount > 1) {
			RObject::rCritical() << "Warning: 2D plotting for the generated data is performed only when 1 parameter is selected.";
		}
		else if (varCount == 1) {
			RMainWindow *mainWindow = qobject_cast<RMainWindow *>(theDomain->parent());
			QVector<double> dataVector;
			for (int i = 0; i < dataVectorList.count(); i++) {
				dataVector << dataVectorList[i][0];
			}
			for (int i = 0; i < passedFunctionList.count(); i++) {
				QVector<double> resultVector;
				double minResult = resultVectorList[0][i];
				double maxResult = resultVectorList[0][i];
				for (int j = 0; j < resultVectorList.count(); j++) {
					double tempResult = resultVectorList[j][i];
					resultVector << tempResult;
					minResult = qMin(minResult, tempResult);
					maxResult = qMax(maxResult, tempResult);
				}

				QCustomPlot *thePlot = new RPlotWidget(mainWindow);
				thePlot->xAxis->setLabel(passedParameterList[0]->objectName());
				thePlot->yAxis->setLabel(passedFunctionList[i]->objectName());
				mainWindow->addSubWindow(thePlot, passedFunctionList[i]->objectName() + " Vs. " + passedParameterList[0]->objectName(), true);
				int check = 0;
				if (gridRadioButton->isChecked()) {
					check++;
					// Instantiating curve
					thePlot->addGraph();
					thePlot->graph(0)->setName(passedFunctionList[i]->objectName());
					QCPGraph *theCurve = thePlot->graph(0);
					theCurve->setPen(QColor(Qt::red));
					//theCurve->setRenderHint(QwtPlotItem::RenderAntialiased, true);
					theCurve->setData(dataVector, resultVector);
				}
				else {
					// Take away the legends
					thePlot->legend->setVisible(false);

					// Set the axes scale
					double maxVar = maxVector[0];
					double minVar = minVector[0];
					double deltaResult = maxResult - minResult;
					if (deltaResult == 0) {
						deltaResult += minResult / 10.0;
					}
					thePlot->xAxis->setRange(minVar - 0.1 * (maxVar - minVar), maxVar + 0.1 * (maxVar - minVar));
					thePlot->yAxis->setRange(minResult - 0.1 * deltaResult, maxResult + 0.1 * deltaResult);


					// Plot the dots
					for (int j = 0; j < dataVector.count(); j++) {
						thePlot->addGraph();
						thePlot->graph(check+j)->setLineStyle(QCPGraph::lsNone);
						thePlot->graph(check+j)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 4));

						QCPGraph *theMarker = thePlot->graph(check + j);
						theMarker->addData(dataVector[j], resultVector[j]);
					}
				}
				thePlot->xAxis->rescale(true);
				thePlot->yAxis->rescale(true);
				thePlot->xAxis->scaleRange(1.1);
				thePlot->yAxis->scaleRange(1.1);
				thePlot->replot();
			}
		}
	}

#endif // NO_QCP

	
	return 0;
}

void RParametricAnalysisDialog::onSubWindowAdded(QWidget *subWindow)
{
	RPlotWidget *plot = qobject_cast<RPlotWidget *>(subWindow);
	if (plot) {
		QString title  = plot->windowTitle() + " for " + theCurrentParameterValuesStr;
		plot->setWindowTitle(title);
	}
}

void RParametricAnalysisDialog::onFunctionGroupBoxClicked(bool checked)
{
	analyzerGroupBox->setChecked(!checked);
}

void RParametricAnalysisDialog::onAnalyzerGroupBoxClicked(bool checked)
{
	functionGroupBox->setChecked(!checked);
}
