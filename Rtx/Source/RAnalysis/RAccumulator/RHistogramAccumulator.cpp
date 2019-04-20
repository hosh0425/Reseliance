// Last modified by Mojtaba on March 19, 2014

#include "RHistogramAccumulator.h"

#include "RFunction.h"
#include "RMainWindow.h"
#include "RPlotWidget.h"
#include "RRandomVariable.h"
#include "RContinuousRandomVariable.h"

#include <QFile>
#include <QFileInfo>
#include <QApplication>
#include <QString>



#include <gsl/gsl_blas.h>
#include <gsl/gsl_cdf.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_histogram.h>
#include <gsl/gsl_math.h>

#include <qreadwritelock.h>

RHistogramAccumulator::RHistogramAccumulator(QObject *parent, QString name)
	: RAccumulator(parent, name)
{
	// Initialize data members
	theNumberOfBins = 100;
	outputQFile = 0;
	theOutputFile = "";
	thePDFFile = "";
	theCDFFile = "";
    tempString = "";

	bool ok = connect(this, SIGNAL(signalUpdatePlot(bool)), this, SLOT(updatePlot(bool)), Qt::BlockingQueuedConnection);

	// Sensitivity implementations
	theDisplayMeanSensitvityDiagrams = false;
	theDisplayStandardDeviationSensitvityDiagrams = false;
	theCCDFSensitivityFile = "";
	theJointPDFSensitivityFile = "";
	theSaveOutputPerSample = false;
	ok = connect(this, SIGNAL(signalInitiateSensitivityPlots()), this, SLOT(initiateSensitivityPlotsSlot()), Qt::BlockingQueuedConnection);

	theSensitivityPlotsHaveBeenInitiated = false;

	// instantiating lock
	theNumberOfBinsLock = new QReadWriteLock();
}

RHistogramAccumulator::~RHistogramAccumulator()
{
	if (outputQFile) {
		delete outputQFile;
		outputQFile = 0;
	}

	delete theNumberOfBinsLock;
}

void RHistogramAccumulator::reset()
{
	// Reset data members
	setTheContinueFlagValue(true);
	theFunctionList.clear();
	theIterationIndex = 0;

	maxVector.clear();
	minVector.clear();
	sumVector.clear();
	squaredSumVector.clear();
	theFunctionValueVectorList.clear();


	// Sensitivity implementations

	sumMeanVector.clear();
	squaredSumMeanVector.clear();
	sumMeanBetaVector.clear();
	sumStdvVector.clear();
	squaredSumStdvVector.clear();
	sumStdvBetaVector.clear();

	theSensitivityMeanValueVectorList.clear();
	theSensitivityStdvValueVectorList.clear();

	sensitivityFirstMomentList.clear();

	theSensitivityFunctionFlagList.clear();

	theSRVSize = 0;
	theComputeRandomVariableSensitivities = false;

	theSensitivityPlotsHaveBeenInitiated = false;
}

int RHistogramAccumulator::getNumberOfBins() const
{
	theNumberOfBinsLock->lockForRead();
	int val = theNumberOfBins;
	theNumberOfBinsLock->unlock();

	return val;
}

void RHistogramAccumulator::setNumberOfBins(int value)
{
	theNumberOfBinsLock->lockForWrite();
	theNumberOfBins = value;
	theNumberOfBinsLock->unlock();

	//	for (int i = 0; i < theFunctionList.count(); i++) {
	//		if (thePlotList[i]) {
	//			updatePlot(false);
	//		}
	//	}
	updatePlot(false);

	// Save the PDF in a text file, if the user has asked to
	if (!thePDFFile.isEmpty()) {
		savePDFtoTextFile();
	}
}

QString RHistogramAccumulator::getOutputFile() const
{
	return theOutputFile;
}

void RHistogramAccumulator::setOutputFile(QString value)
{
	theOutputFile = QFileInfo(value).filePath();
}

QString RHistogramAccumulator::getPDFFile() const
{
	return thePDFFile;
}

void RHistogramAccumulator::setPDFFile(QString value)
{
	thePDFFile = QFileInfo(value).filePath();
}

QString RHistogramAccumulator::getCDFFile() const
{
	return theCDFFile;
}

void RHistogramAccumulator::setCDFFile(QString value)
{
	theCDFFile = QFileInfo(value).filePath();
}


bool RHistogramAccumulator::getDisplayMeanSensitvityDiagrams() const
{
	return theDisplayMeanSensitvityDiagrams;
}

void RHistogramAccumulator::setDisplayMeanSensitvityDiagrams(bool value)
{
	theDisplayMeanSensitvityDiagrams = value;
}

bool RHistogramAccumulator::getSaveOutputPerSample() const
{
	return theSaveOutputPerSample;
}

void RHistogramAccumulator::setSaveOutputPerSample(bool value)
{
	theSaveOutputPerSample = value;
}

bool RHistogramAccumulator::getDisplayStandardDeviationSensitvityDiagrams() const
{
	return theDisplayStandardDeviationSensitvityDiagrams;
}

void RHistogramAccumulator::setDisplayStandardDeviationSensitvityDiagrams(bool value)
{
	theDisplayStandardDeviationSensitvityDiagrams = value;
}

QString RHistogramAccumulator::getCCDFSensitivityFile() const
{
	return theCCDFSensitivityFile;
}

void RHistogramAccumulator::setCCDFSensitivityFile(QString value)
{
	theCCDFSensitivityFile = QFileInfo(value).filePath();
}

QString RHistogramAccumulator::getJointPDFSensitivityFile() const
{
	return theJointPDFSensitivityFile;
}

void RHistogramAccumulator::setJointPDFSensitivityFile(QString value)
{
	theJointPDFSensitivityFile = QFileInfo(value).filePath();
}


int RHistogramAccumulator::accumulate(QVector<double> functionValueVector, gsl_vector * standardNormalRandomNumbers, QVector<double> sensitivityResultMeanVector, QVector<double> sensitivityResultStdvVector, int threadID)
{

	// Incrementing theIterationIndex
	theIterationIndex++;

	//QString tempString;

	if ((!theOutputFile.isEmpty()) && (!outputQFile)) {
		outputQFile = new QFile(theOutputFile);
		if (outputQFile->exists()) {
			outputQFile->remove();
		}

		// The follwoing lines insert the function names at the top of the file if there is more than one function
		if (theFunctionList.count() > 1) {
			for (int i = 0; i < theFunctionList.count(); i++) {
					tempString += QString("%1\t").arg(theFunctionList[i]->objectName());
			}


			/*// TEMPORARY to add the realization of random variables after function values
			QList<RRandomVariable *> rvList = (*theFunctionList[0]->getRandomVariableList());
			for (int i=0; i < rvList.count(); i++) {
			if (i == rvList.count() - 1) {
			tempString += QString("%1").arg(rvList[i]->objectName());
			} else {
			tempString += QString("%1\t").arg(rvList[i]->objectName());
			}
			}
			// END OF TEMPORARY PART*/
			tempString += "\n";
		}

		if (!outputQFile->open(QIODevice::Append | QIODevice::Text)) {
			rCritical(threadID) << "Error: Could not create the file" << theOutputFile << ".";
			return -1;
		}
	}

	// Print the iteration number to the console
	if (getOutputDisplayLevel() > RObject::None) {
		rDebug(threadID) << "Sample" << theIterationIndex;
	}

	cntrSensFunc = -1;
	// Iterating on functions
	for (int i = 0; i < theFunctionList.count() && sensitivityResultStdvVector.size() == sensitivityResultMeanVector.size(); i++) {
		if (!theOutputFile.isEmpty()) {
				tempString += QString("%1\t").arg(functionValueVector[i], 0, 'g', 12);
		}

		// Display the results of this sample in output pane
		if (getOutputDisplayLevel() >= RObject::Moderate) {
			rDebug(threadID) << "Function" << theFunctionList[i]->objectName() << "\t=" << functionValueVector[i];
		}

				//if (theDisplayDiagrams && qobject_cast<QApplication*>(QCoreApplication::instance()) != 0) {
				//	// Only plot at plotting frequency, or at the last sample
				//	if (thePlotList[i] == 0) {
				//		
				//	}
				//}

		theFunctionValueVectorList[i].append(functionValueVector[i]);

		if (maxVector[i] < functionValueVector[i]) {
			maxVector[i] = functionValueVector[i];
		}
		if (minVector[i] > functionValueVector[i]) {
			minVector[i] = functionValueVector[i];
		}
		sumVector[i] += functionValueVector[i];
		squaredSumVector[i] += functionValueVector[i] * functionValueVector[i];

		// Sensitivity implementations
		if (theComputeRandomVariableSensitivities && theSensitivityFunctionFlagList[i]) {
			cntrSensFunc += 1;
			for (int k = 0; k < theSRVSize; k++) {
					theSensitivityMeanValueVectorList[k + theSRVSize * cntrSensFunc].append(sensitivityResultMeanVector[k]);
					theSensitivityStdvValueVectorList[k + theSRVSize * cntrSensFunc].append(sensitivityResultStdvVector[k]);		
			}
		}

		/*// TEMPORARY CODE to evaluate moments on a regular interval, June, 2012
		QString expectationStr;
		for (int j = 1; j <= 5; j++) {
		double aa = (double)j / 1.0;
		double C_aa_Sum = 0.0;

		for (int k = 0; k < theIterationIndex; k++) {
		C_aa_Sum += pow(theFunctionValueVectorList[0][k], aa);
		}
		double C_aa_Expectation = C_aa_Sum / (double)theIterationIndex;
		expectationStr += QString("%1\t").arg(C_aa_Expectation, 0, 'g', 10);
		}
		rDebug() << qPrintable(expectationStr);
		// END OF THE TEMPORARY CODE*/
		tempString += "\n";
	}

	// sensitivity implementation
	if (!theSensitivityPlotsHaveBeenInitiated) {
		if (getDisplayDiagrams() && qobject_cast<QApplication*>(QCoreApplication::instance()) != 0) {
			if (threadID == 0)
				initiateSensitivityPlotsSlot();
			else
				emit signalInitiateSensitivityPlots();
		}

		theSensitivityPlotsHaveBeenInitiated = true;
	}

	// updating plots
	if ((((theIterationIndex % getPlottingInterval() == 0) || (theIterationIndex == getMaximumIterations())) && (theIterationIndex > 1)))
		if (threadID == 0)
			updatePlot(true);
		else
			emit signalUpdatePlot(true);


	if (!theOutputFile.isEmpty()) {
		/*// TEMPORARY to add the realization of random variables after function values
		QList<RRandomVariable *> rvList = (*theFunctionList[0]->getRandomVariableList());
		for (int i=0; i < rvList.count(); i++) {
		if (i == rvList.count() - 1) {
		tempString += QString("%1").arg(rvList[i]->getCurrentValue(), 0, 'g', 10);
		} else {
		tempString += QString("%1,").arg(rvList[i]->getCurrentValue(), 0, 'g', 10);
		}
		}
		// END OF TEMPORARY PART*/
			

		QByteArray byteArray;
		
		if (theSaveOutputPerSample) {
			byteArray.append(tempString);
			tempString = "";
			outputQFile->write(byteArray);
			outputQFile->flush();
		}
		else if (sensitivityResultStdvVector.size() != sensitivityResultMeanVector.size()){
			byteArray.append(tempString);
			tempString = "";
			outputQFile->write(byteArray);
			outputQFile->flush();
		}
	}

	// Update theContinueFlag
	if (theIterationIndex >= getMaximumIterations()) {
		setTheContinueFlagValue(false);
	}

	return 0;
}

void RHistogramAccumulator::terminateAccumulation() {

	if (!theOutputFile.isEmpty()) {

		outputQFile->close();
		delete outputQFile;
		outputQFile = 0;
	}


	/*// TEMPORARY CODE to store the histogram data in a text file
	for (int i = 0; i < theFunctionList.count(); i++) {
	// Storing the histogram data in an output file
	QFile *output = new QFile(QString("%1_Histogram.txt").arg(theFunctionList[i]->objectName()));
	if (output->exists()) {
	output->remove();
	}
	if (!output->open(QIODevice::Append | QIODevice::Text)) {
	rCritical() << "Warning: Could not create the file" << theOutputFile << ".";
	}
	QwtData &data = thePDFCurveList[i]->data();
	int dataCount = data.size();
	double CDF = 0;
	double binWidth = 0;
	if (dataCount >= 2) {
	binWidth = data.x(1) - data.x(0);
	} else {
	continue;
	}
	for (int j = 0; j < dataCount; j++) {
	double x = data.x(j);
	double y = data.y(j);
	CDF += y * binWidth;

	QString tempString = QString("%1\t%2\t%3\n").arg(x, 0, 'g', 8).arg(y, 0, 'g', 8).arg(CDF, 0, 'g', 8);
	QByteArray byteArray;
	byteArray.append(tempString);
	output->write(byteArray);
	output->flush();
	}
	output->close();
	delete output;
	}
	// End of the TEMPORARY part*/

	rDebug() << "Histogram sampling analysis is complete.";
	rDebug() << "";

	// Iterating on functions
	for (int i = 0; i < theFunctionList.count(); i++) {
		double mean = sumVector[i] / theIterationIndex;
		double stdv = sqrt(1.0 / (theIterationIndex - 1) * (squaredSumVector[i] - theIterationIndex * mean * mean));

		rDebug() << "";
		rDebug() << "Function" << theFunctionList[i]->objectName() << ":" << "\tMean =" << mean << "\tStandard Deviation =" << stdv;


		/*// TEMPORARY code to evaluate and print the a-th moment of the samples, used in the sensitivity paper, June, 2012
		rDebug() << "";
		rDebug() << "Moments:";
		rDebug() << "";
		for (int j = 0; j <= 100; j++) {
		double aa = (double)j / 10.0;
		double C_aa_Sum = 0.0;

		for (int k = 0; k < theIterationIndex; k++) {
		C_aa_Sum += pow(theFunctionValueVectorList[i][k], aa);
		}

		double C_aa_Expectation = C_aa_Sum / (double)theIterationIndex;

		rDebug() << qPrintable(QString("%1\t%2").arg(aa).arg(C_aa_Expectation));

		// Store the second moment in a text file named "SecondMoment.txt"
		if (aa == 2.0) {
		QString secondMomentFileName = QString("SecondMoment.txt");
		QFile *output = new QFile(secondMomentFileName);
		if (output->exists()) {
		output->remove();
		}
		if (!output->open(QIODevice::WriteOnly | QIODevice::Text)) {
		rCritical() << "Warning: Could not create the file" << secondMomentFileName;
		}

		QString tempString = QString("%1").arg(C_aa_Expectation, 0, 'g', 14);
		QByteArray byteArray;
		byteArray.append(tempString);
		output->write(byteArray);
		output->flush();

		output->close();
		delete output;
		}
		}
		rDebug() << "";
		rDebug() << "";


		// TEMPORARY code to evaluate and print the expectation of the exp(bb*function)-1
		rDebug() << "Exponential Expectations:";
		for (int j = 0; j <= 100; j++) {

		double bb = 2e-9 + 4e-9 * (double)j / 100.0;
		double exp_C_bb_Sum = 0.0;

		for (int k = 0; k < theIterationIndex; k++) {
		exp_C_bb_Sum += exp(bb * theFunctionValueVectorList[i][k]) - 1;
		}

		double exp_C_bb_Expectation = exp_C_bb_Sum / (double)theIterationIndex;

		rDebug() << qPrintable(QString("%1\t%2").arg(bb).arg(exp_C_bb_Expectation));
		}
		rDebug() << "";
		rDebug() << "";
		// End of the TEMPORARY code*/
	}

	// Sensitivity implementations
	if (theComputeRandomVariableSensitivities) {

		saveSensitvitytoTextFile(theSensitivityRandomVariableList);

		QString headerStr = "Variable";
		QStringList sensitivityStringList;
		for (int i = 0; i < theSRVSize; i++) {
			sensitivityStringList << theSensitivityRandomVariableList[i].data()->objectName();
		}

		cntrSensFunc = -1;

		for (int i = 0; i < theFunctionList.count(); i++) {
			if (theSensitivityFunctionFlagList[i]) {
				cntrSensFunc += 1;
				headerStr += qPrintable(QString("\tdE[%1]/dMean\tdE[%2]/dStdv").arg(theFunctionList[i]->objectName()).arg(theFunctionList[i]->objectName()));
				for (int k = 0; k < theSRVSize; k++) {
					double dE_dMean = sensitivityFirstMomentList[k + cntrSensFunc*theSRVSize].value(0);
					double dE_dStdv = sensitivityFirstMomentList[k + cntrSensFunc*theSRVSize].value(1);
					sensitivityStringList[k] += QString("\t%1").arg(dE_dMean, 0, 'g', 10) + QString("\t%1").arg(dE_dStdv, 0, 'g', 10);
				}
			}
		}
		rDebug() << "";
		rDebug() << "";
		rDebug() << "Sensitivity results";
		rDebug() << "";
		rDebug() << qPrintable(headerStr);

		for (int k = 0; k < theSRVSize; k++) {
			rDebug() << qPrintable(sensitivityStringList[k]);
		}
	}

	rDebug() << "";
	rDebug() << "The total number of samples =" << theIterationIndex;
	rDebug() << "";

	// Save the CDF and CoV in a text file, if the user has aksed to
	if (!theCDFFile.isEmpty()) {
		saveCDFtoTextFile();
	}

	// Save the PDF in a text file, if the user has aksed to
	if (!thePDFFile.isEmpty()) {
		savePDFtoTextFile();
	}

	// Sensitivity implementations
	// Save the sensitvity results in text files, if the user has aksed to
}

void RHistogramAccumulator::setFunctionList(QList<RFunction *> passedFunctionList)
{
	theFunctionList = passedFunctionList;

	// free lists

	//qDeleteAll(thePlotList); deleteall is not possible on the list of RPointers, so it is deleted through a for loop

#ifndef NO_QCP
	//for (int i = 0; i < thePlotList.count(); ++i) {
	//	delete thePlotList[i].data();
	//}

	// free lists
	//qDeleteAll(thePlotList);
	thePlotList.clear();
	thePDFCurveList.clear();
	theCDFCurveList.clear();
	theCoVCurveList.clear();
#endif // NO_QCP




	RMainWindow *mainWindow = qobject_cast<RMainWindow *>(parent()->parent());

	for (int i = 0; i < theFunctionList.count(); i++) {

		maxVector.append(-1e308);
		minVector.append(1e308);

		sumVector.append(0.0);
		squaredSumVector.append(0.0);

		QVector<double> vector;
		theFunctionValueVectorList.append(vector);

#ifndef No_QCP

		if (qobject_cast<QApplication*>(QCoreApplication::instance()) != 0) { //salarifard : in GUI mode

		// Initiating thePlot
			QCustomPlot *thePlot = new RPlotWidget(mainWindow);


			if (getDisplayDiagrams() && qobject_cast<QApplication*>(QCoreApplication::instance()) != 0)
			{
				mainWindow->addSubWindow(thePlot, objectName() + " Plot of the Function " + theFunctionList[i]->objectName(), true);

				// Assigning the autorange axes
				QList<QCPAxis*> axisrect;
				axisrect << thePlot->xAxis << thePlot->yAxis << thePlot->yAxis2;
				thePlot->axisRect()->setRangeZoomAxes(axisrect);
				thePlot->axisRect()->setRangeDragAxes(axisrect);
				thePlot->setInteraction(QCP::iRangeDrag, true); // Enable dragging
				thePlot->setInteraction(QCP::iRangeZoom, true); // Enable zoom by scrolling

																// Setting the Axis labels
				thePlot->xAxis->setLabel("Function Value");
				thePlot->yAxis->setLabel("PDF");
				thePlot->yAxis2->setLabel("CDF or CoV");

				thePlot->yAxis2->setTickLabels(true); // Shows the values for the tickmarks
				thePlot->yAxis2->setVisible(true);

				thePlotList << thePlot;

				thePlot->addGraph(thePlot->xAxis, thePlot->yAxis2); // adds the CoV curve attached to xAxis and yAxis2
				thePlot->graph(0)->setAntialiased(true);
				thePlot->graph(0)->setPen(QPen(Qt::black));
				theCoVCurveList << thePlot->graph(0);
				thePlot->graph(0)->setName("CoV");

				thePlot->addGraph(thePlot->xAxis, thePlot->yAxis); // adds PDF curve attached to xAxis and yAxis
				thePlot->graph(1)->setAntialiased(true);
				thePlot->graph(1)->setPen(QPen(Qt::red));
				thePDFCurveList << thePlot->graph(1);
				thePlot->graph(1)->setName("PDF");

				thePlot->addGraph(thePlot->xAxis, thePlot->yAxis2); // adds CDF curve attached to xAxis and yAxis2
				thePlot->graph(2)->setAntialiased(true);
				thePlot->graph(2)->setPen(QPen(Qt::blue));
				theCDFCurveList << thePlot->graph(2);
				thePlot->graph(2)->setName("CDF");

				thePlot->legend->setVisible(true);
			}

#endif


		}
	}
}

bool RHistogramAccumulator::getContinueFlag()
{
	return getTheContinueFlagValue();
}

void RHistogramAccumulator::setSensitivityFunctionFlagList(QList<bool> flaglist)
{
	theSensitivityFunctionFlagList = flaglist;

	for (int i = 0; i < theFunctionList.count(); i++) {
		if (theSensitivityFunctionFlagList[i]) {
			for (int k = 0; k < theSRVSize; k++) {
				sumMeanVector.append(0.0);
				squaredSumMeanVector.append(0.0);
				sumMeanBetaVector.append(0.0);

				QVector<double> vectorMean;
				theSensitivityMeanValueVectorList.append(vectorMean);

				sumStdvVector.append(0.0);
				squaredSumStdvVector.append(0.0);
				sumStdvBetaVector.append(0.0);

				QVector<double> vectorStdv;
				theSensitivityStdvValueVectorList.append(vectorStdv);
			}
		}
	}
}

void RHistogramAccumulator::evaluateHistogram(int functionIndex, QVector<double> *xVector, QVector<double> *PDFVector)
{
	int i = functionIndex;
	QVector<double> functioniValueVector = theFunctionValueVectorList[i];

	// PDF curve
	// Allocate memory for a histogram
	gsl_histogram *histogram = gsl_histogram_alloc(getNumberOfBins());

	// Reset the histogram
	gsl_histogram_set_ranges_uniform(histogram, minVector[i], maxVector[i]);

	// Loop over the samples to generate the histogram
	for (int j = 0; j < theIterationIndex; j++) {
		// Add values to create histogram
		gsl_histogram_increment(histogram, functioniValueVector[j]);
	}

	// Normalize to get relative frequency diagram (“PDF”)
	gsl_histogram_scale(histogram, 1.0 / ((maxVector[i] - minVector[i]) / getNumberOfBins() * theIterationIndex));

	// Compute the histogram
	for (int j = 0; j < getNumberOfBins(); j++) {
		double binContent = gsl_histogram_get(histogram, j);
		*PDFVector << binContent;

		double upper;
		double lower;
		gsl_histogram_get_range(histogram, j, &lower, &upper);
		double midPoint = (upper + lower) / 2.0;
		*xVector << midPoint;
	}

	// Release the histogram
	gsl_histogram_free(histogram);
}

void RHistogramAccumulator::evaluateSensitivity(QVector<double> SortedSensitivityVector, QVector<double> *SensitvityMu, QVector<double> *SensitvityCoV)
{
	double sum = 0.0;
	double sumSquare = 0.0;
	double var = 0.0;
	double mu = 0.0;
	double cov = 0.0;

	double totalSum = 0;
	for (int i = 0; i < theIterationIndex; i++) {
		totalSum += SortedSensitivityVector[i];
	}

	for (int i = 0; i < theIterationIndex; i++) {
		sum += SortedSensitivityVector[i];
		mu = (totalSum - sum) / (double)theIterationIndex;
		*SensitvityMu << mu;
	}

	double sumCoV = 0;
	double sumSquareCoV = 0;
	double sumCoV1 = 0;
	double sumSquareCoV1 = 0;
	int firstTime = 1;

	for (int i = 0; i < theIterationIndex; i++) {
		sumSquare += gsl_pow_2(SortedSensitivityVector[i]);
	}

	for (int i = 0; i < theIterationIndex; i++) {

		if (i < theIterationIndex / 2) {
			sumCoV += SortedSensitivityVector[i];
			sumSquareCoV += gsl_pow_2(SortedSensitivityVector[i]);
			mu = sumCoV / (double)theIterationIndex;
			var = (sumSquareCoV / theIterationIndex - mu*mu) / (theIterationIndex - 1.0);
		}
		else {
			if (firstTime == 1) {
				sumCoV1 = sum - sumCoV;
				sumSquareCoV1 = sumSquare - sumSquareCoV;
				firstTime = 0;
			}

			sumCoV1 -= SortedSensitivityVector[i];
			sumSquareCoV1 -= SortedSensitivityVector[i] * SortedSensitivityVector[i];
			mu = sumCoV1 / (double)theIterationIndex;
			var = (sumSquareCoV1 / theIterationIndex - mu*mu) / (theIterationIndex - 1.0);
		}

		cov = qAbs(sqrt(var) / mu);

		if (i == theIterationIndex - 1)
			cov = 1.0;

		if (cov > 10.0)
			cov = 10.0;

		*SensitvityCoV << cov;
	}
}

void RHistogramAccumulator::updatePlot(bool checkMinMax)
{

#ifndef NO_QCP

	if (getDisplayDiagrams() && (qobject_cast<QApplication*>(QCoreApplication::instance()) != 0))
	{
		cntrSensFunc = -1;
		for (int i = 0; i < theFunctionList.count(); i++) {
			if (thePlotList[i] && ((minVector[i] != maxVector[i]) || (!checkMinMax))) {
	
				QVector<double> functioniValueVector = theFunctionValueVectorList[i];
	
				// Update PDF
				QVector<double> xVector;
				QVector<double> PDFVector;
				evaluateHistogram(i, &xVector, &PDFVector);
				if (getDisplayDiagrams() && (qobject_cast<QApplication*>(QCoreApplication::instance()) != 0)) {
					thePDFCurveList[i]->setData(xVector, PDFVector);
				}
	
				// Update CDF
				QVector<double> CDFVector;
				QVector<double> CoVVector;
				qSort(functioniValueVector.begin(), functioniValueVector.end());
				computeCDFandCoV(&CDFVector, &CoVVector);
				if (getDisplayDiagrams() && (qobject_cast<QApplication*>(QCoreApplication::instance()) != 0)) {
					theCDFCurveList[i]->setData(functioniValueVector, CDFVector);
					theCoVCurveList[i]->setData(functioniValueVector, CoVVector);
	
					////QPlotList[i]->removeAllSeries();
					//series.clear();
					//
					//for (int i = 0; i < functioniValueVector.count(); i++)
					//{
					//	series << QPointF(functioniValueVector[i], CDFVector[i]);
					//}
	
					//QPlotList[i]->addSeries(&series);
	
					thePlotList[i]->xAxis->rescale(true);
					thePlotList[i]->yAxis->rescale(true);
					thePlotList[i]->yAxis2->rescale(true);
	
					thePlotList[i]->xAxis->scaleRange(1.1);
					thePlotList[i]->yAxis->scaleRange(1.1);
					thePlotList[i]->yAxis2->scaleRange(1.1);
					//thePlotList[i]->graph(0)->rescaleAxes(true);
					//thePlotList[i]->graph(1)->rescaleAxes(true);
					//thePlotList[i]->graph(2)->rescaleAxes(true);
	
					// Replot
					thePlotList[i]->replot();
				}
	
				// Sensitivity implementations
				if (theComputeRandomVariableSensitivities  && theSensitivityFunctionFlagList[i]) {
	
					// Sort sensitivty vectors based on function values
					QVector<double> functioniValueSortedList;
					QList<QVector<double> > theSensitivityMeanValueSortedList;
					QList<QVector<double> > theSensitivityStdvValueSortedList;
	
					for (int k = 0; k < theSRVSize; k++) {
	
						functioniValueSortedList = theFunctionValueVectorList[i];
						theSensitivityMeanValueSortedList.append(theSensitivityMeanValueVectorList[k + cntrSensFunc * theSRVSize]);
						theSensitivityStdvValueSortedList.append(theSensitivityStdvValueVectorList[k + cntrSensFunc * theSRVSize]);
	
						QMap<double, double> sortMapMean;
						QMap<double, double> sortMapStdv;
	
						for (int j = 0; j < functioniValueSortedList.size(); ++j)
						{
							// Syntax to keep the GUI responsive
							QCoreApplication::processEvents();
	
							sortMapMean.insertMulti(functioniValueSortedList.at(j), theSensitivityMeanValueSortedList[k].at(j));
							sortMapStdv.insertMulti(functioniValueSortedList.at(j), theSensitivityStdvValueSortedList[k].at(j));
						}
						functioniValueSortedList = sortMapMean.keys().toVector();
						theSensitivityMeanValueSortedList[k] = sortMapMean.values().toVector();
	
						functioniValueSortedList = sortMapStdv.keys().toVector();
						theSensitivityStdvValueSortedList[k] = sortMapStdv.values().toVector();
					}
	
					// Update sensitivity plots
					for (int k = 0; k < theSRVSize; k++) {
						// Syntax to keep the GUI responsive
						QCoreApplication::processEvents();
	
						QVector<double> MeanSensitivityVectorMu;     // dPf/dMean Mean value Plot
						QVector<double> MeanSensitivityVectorCoV;    // dPf/dMean CoV Plot
						QVector<double> MeanBetaSensitivityVectorMu; // dBeta/dMean Mean value Plot
						QVector<double> StdvSensitivityVectorMu;
						QVector<double> StdvSensitivityVectorCoV;
						QVector<double> StdvBetaSensitivityVectorMu;
	
						// reset sum vectors for new sample
						sumMeanVector[k + cntrSensFunc * theSRVSize] = 0;
						squaredSumMeanVector[k + cntrSensFunc * theSRVSize] = 0;
						sumMeanBetaVector[k + cntrSensFunc * theSRVSize] = 0;
						sumStdvVector[k + cntrSensFunc * theSRVSize] = 0;
						squaredSumStdvVector[k + cntrSensFunc * theSRVSize] = 0;
						sumStdvBetaVector[k + cntrSensFunc * theSRVSize] = 0;
	
	
						evaluateSensitivity(theSensitivityMeanValueSortedList[k], &MeanSensitivityVectorMu, &MeanSensitivityVectorCoV);
	
	
						// Calculate Beta sensitivity
						/*for (int p = 0; p < MeanSensitivityVectorMu.count(); p++) {
						MeanBetaSensitivityVectorMu.append( MeanSensitivityVectorMu[p] * (-1 / gsl_ran_ugaussian_pdf(gsl_cdf_ugaussian_Pinv(1-theCDFCurveList[i]->y(p)))) );
						}*/
	
						if (theDisplayMeanSensitvityDiagrams) {
							if (getDisplayDiagrams() && qobject_cast<QApplication*>(QCoreApplication::instance()) != 0) {
								theMeanMuCurveList[k + cntrSensFunc * theSRVSize]->setData(functioniValueSortedList, MeanSensitivityVectorMu);
								theMeanCoVCurveList[k + cntrSensFunc * theSRVSize]->setData(functioniValueSortedList, MeanSensitivityVectorCoV);
							}
						}
	
	
						evaluateSensitivity(theSensitivityStdvValueSortedList[k], &StdvSensitivityVectorMu, &StdvSensitivityVectorCoV);
	
	
	
						// Calculate Beta sensitivity
						/*for (int p = 0; p < StdvSensitivityVectorMu.count(); p++)	{
						StdvBetaSensitivityVectorMu.append( StdvSensitivityVectorMu[p] * (-1 / gsl_ran_ugaussian_pdf(gsl_cdf_ugaussian_Pinv(1-theCDFCurveList[i]->y(p)))) );
						}*/
	
						if (theDisplayStandardDeviationSensitvityDiagrams) {
							if (getDisplayDiagrams() && (qobject_cast<QApplication*>(QCoreApplication::instance()) != 0)) {
								theStdvMuCurveList[k + cntrSensFunc * theSRVSize]->setData(functioniValueSortedList, StdvSensitivityVectorMu);
								theStdvCoVCurveList[k + cntrSensFunc * theSRVSize]->setData(functioniValueSortedList, StdvSensitivityVectorCoV);
							}
						}
	
	
						if ((theDisplayStandardDeviationSensitvityDiagrams || theDisplayMeanSensitvityDiagrams) && getDisplayDiagrams() && (qobject_cast<QApplication*>(QCoreApplication::instance()) != 0)) {
							// Replot
							theSensitivityPlotList[k + cntrSensFunc * theSRVSize]->replot();
						}
	
	
						// Compute sum and sumSquare for final sensitivity vector
						for (int j = 0; j < MeanSensitivityVectorMu.count(); j++) {
							sumMeanVector[k + cntrSensFunc * theSRVSize] += MeanSensitivityVectorMu[j];
							squaredSumMeanVector[k + cntrSensFunc * theSRVSize] += MeanSensitivityVectorMu[j] * MeanSensitivityVectorMu[j];
							//sumMeanBetaVector[k + cntrSensFunc * theSRVSize] += MeanBetaSensitivityVectorMu[j];
							sumStdvVector[k + cntrSensFunc * theSRVSize] += StdvSensitivityVectorMu[j];
							squaredSumStdvVector[k + cntrSensFunc * theSRVSize] += StdvSensitivityVectorMu[j] * StdvSensitivityVectorMu[j];
							//sumStdvBetaVector[k + cntrSensFunc * theSRVSize] += StdvBetaSensitivityVectorMu[j];
						}
					}
				}
			}
		}
	}
#endif // NO_QCP


}

void RHistogramAccumulator::computeCDFandCoV(QVector<double> *CDFVector, QVector<double> *CoVVector)
{
	for (int i = 1; i <= theIterationIndex; i++) {
		double CDF = (double)i / (double)theIterationIndex;
		*CDFVector << CDF;

		double prob = 0;
		if (i < theIterationIndex / 2.0) {
			prob = CDF;
		}
		else {
			prob = 1.0 - CDF;
		}
		double variance = (prob - gsl_pow_2(prob)) / theIterationIndex;
		if (variance < 0.0) {
			variance = 0.0;
		}
		double CoV = sqrt(variance) / prob;
		if (i == theIterationIndex) {
			CoV = 1.0;
		}
		*CoVVector << CoV;
	}
}

void RHistogramAccumulator::saveCDFtoTextFile()
{
	QList<QVector<double> > valueVectorList;
	QVector<double> CDFVector;
	QVector<double> CoVVector;
	for (int i = 0; i < theFunctionList.count(); i++) {
		QVector<double> functioniValueVector = theFunctionValueVectorList[i];
		qSort(functioniValueVector.begin(), functioniValueVector.end());
		valueVectorList << functioniValueVector;

		if (i == 0) {
			computeCDFandCoV(&CDFVector, &CoVVector);
		}
	}
	valueVectorList.insert(0, CoVVector);
	valueVectorList.insert(0, CDFVector);

	QFile *file = new QFile(theCDFFile);
	if (file->exists()) {
		file->remove();
	}

	// The follwoing lines insert the headers (function names, etc.) at the top of the file 
	QString tempStr = "CDF\tCoV\t";
	for (int i = 0; i < theFunctionList.count(); i++) {
		tempStr += QString("%1\t").arg(theFunctionList[i]->objectName());
	}
	tempStr += "\n";

	if (!file->open(QIODevice::WriteOnly | QIODevice::Text)) {
		rCritical() << "Error: Could not create the file" << theCDFFile;
		return;
	}

	QByteArray byteArray;
	byteArray.append(tempStr);
	tempStr.clear();

	for (int i = 0; i < theIterationIndex; i++) {
		for (int j = 0; j < valueVectorList.count(); j++) {
			tempStr += QString("%1\t").arg(valueVectorList[j][i], 0, 'g', 12);
		}
		tempStr += "\n";
		byteArray.append(tempStr);
		tempStr.clear();
	}

	file->write(byteArray);
	file->flush();
	file->close();
	delete file;
	file = 0;
}

void RHistogramAccumulator::savePDFtoTextFile()
{
	QList<QVector<double> > xVectorList;
	QList<QVector<double> > PDFVectorList;
	for (int i = 0; i < theFunctionList.count(); i++) {
		QVector<double> xVector;
		QVector<double> PDFVector;
		evaluateHistogram(i, &xVector, &PDFVector);
		xVectorList << xVector;
		PDFVectorList << PDFVector;
	}

	QFile *file = new QFile(thePDFFile);
	if (file->exists()) {
		file->remove();
	}

	// The follwoing lines insert the headers (function names, etc.) at the top of the file 
	QString tempStr;
	for (int i = 0; i < theFunctionList.count(); i++) {
		tempStr += QString("%1\tPDF(%1)\t").arg(theFunctionList[i]->objectName());
	}
	tempStr += "\n";

	if (!file->open(QIODevice::WriteOnly | QIODevice::Text)) {
		rCritical() << "Error: Could not create the file" << thePDFFile;
		return;
	}

	QByteArray byteArray;
	byteArray.append(tempStr);
	tempStr.clear();

	for (int i = 0; i < getNumberOfBins(); i++) {
		for (int j = 0; j < xVectorList.count(); j++) {
			tempStr += QString("%1\t%2\t").arg(xVectorList[j][i], 0, 'g', 12).arg(PDFVectorList[j][i], 0, 'g', 12);
		}
		tempStr += "\n";
		byteArray.append(tempStr);
		tempStr.clear();
	}

	file->write(byteArray);
	file->flush();
	file->close();
	delete file;
	file = 0;
}

void RHistogramAccumulator::saveSensitvitytoTextFile(QList<RPointer<RRandomVariable> > sensitivityRandomVariableList)
{
	QList<QVector<double> > xVectorList;
	QList<QVector<double> > SensitivityVectorList;
	QList<QVector<double> > dfdMeanVectorList;

	QString tempStrHdr = QString("");
	QString tempStr = QString("");
	QByteArray byteArray;
	QString tempStrHdr2 = QString("");
	QString tempStr2 = QString("");
	QByteArray byteArray2;

	cntrSensFunc = -1;

	for (int i = 0; i < theFunctionList.count(); i++) {

		QList<QVector<double> > tempMeanSensitivityCurve;
		QList<QVector<double> > tempMeanSensitivityCurveCoV;
		QList<QVector<double> > tempStdvSensitivityCurve;
		QList<QVector<double> > tempStdvSensitivityCurveCoV;

		if (theSensitivityFunctionFlagList[i]) {
			cntrSensFunc += 1;

			// Sort sensitivty vectors based on function values
			QVector<double> functioniValueSortedList;
			QList<QVector<double> > theSensitivityMeanValueSortedList;
			QList<QVector<double> > theSensitivityStdvValueSortedList;
			for (int k = 0; k < theSRVSize; k++) {
				functioniValueSortedList = theFunctionValueVectorList[i];
				theSensitivityMeanValueSortedList.append(theSensitivityMeanValueVectorList[k + cntrSensFunc * theSRVSize]);
				theSensitivityStdvValueSortedList.append(theSensitivityStdvValueVectorList[k + cntrSensFunc * theSRVSize]);
				QMap<double, double> sortMapMean;
				QMap<double, double> sortMapStdv;
				for (int j = 0; j < functioniValueSortedList.size(); ++j) {
					sortMapMean.insertMulti(functioniValueSortedList.at(j), theSensitivityMeanValueSortedList[k].at(j));
					sortMapStdv.insertMulti(functioniValueSortedList.at(j), theSensitivityStdvValueSortedList[k].at(j));
				}
				functioniValueSortedList = sortMapMean.keys().toVector();
				theSensitivityMeanValueSortedList[k] = sortMapMean.values().toVector();

				functioniValueSortedList = sortMapStdv.keys().toVector();
				theSensitivityStdvValueSortedList[k] = sortMapStdv.values().toVector();
			}

			for (int k = 0; k < theSRVSize; k++) {
				QVector<double> MeanSensitivityVectorMu;     // dPf/dMean Mean value Plot
				QVector<double> MeanSensitivityVectorCoV;    // dPf/dMean CoV Plot
				QVector<double> StdvSensitivityVectorMu;	 // dPf/dStdv Mean value Plot
				QVector<double> StdvSensitivityVectorCoV;	 // dPf/dStdv CoV Plot

															 // Compute Sensitivities for all thresholds
				evaluateSensitivity(theSensitivityMeanValueSortedList[k], &MeanSensitivityVectorMu, &MeanSensitivityVectorCoV);
				evaluateSensitivity(theSensitivityStdvValueSortedList[k], &StdvSensitivityVectorMu, &StdvSensitivityVectorCoV);

				tempMeanSensitivityCurve << MeanSensitivityVectorMu;
				tempMeanSensitivityCurveCoV << MeanSensitivityVectorCoV;
				tempStdvSensitivityCurve << StdvSensitivityVectorMu;
				tempStdvSensitivityCurveCoV << StdvSensitivityVectorCoV;

				// Compute dE/dMean and dE/dStdv 
				QVector<double> tempFirstMomentVector;
				tempFirstMomentVector.append(trapezoidalIntegration(functioniValueSortedList, MeanSensitivityVectorMu));
				tempFirstMomentVector.append(trapezoidalIntegration(functioniValueSortedList, StdvSensitivityVectorMu));
				sensitivityFirstMomentList << tempFirstMomentVector;
			}
			// Prepare text files to print to 2 output files
			if (!theJointPDFSensitivityFile.isEmpty()) {

				// The follwoing lines insert the headers (function names, etc.) and sensitivity values to the text for the file theJointPDFSensitivityFile
				tempStrHdr += QString("%1\t").arg(theFunctionList[i]->objectName());

				for (int p = 0; p < functioniValueSortedList.count(); p++) {
					tempStr += QString("%1\t").arg(theFunctionValueVectorList[i][p], 0, 'g', 12);

					for (int k = 0; k < theSRVSize; k++) {
						RContinuousRandomVariable *RV = qobject_cast<RContinuousRandomVariable *>(sensitivityRandomVariableList[k].data());
						if (p == 0) {
							tempStrHdr += QString("df/dMean[%1]/(1/f)\tdf/dStdv[%1]/(1/f)\t").arg(RV->objectName());
						}
						tempStr += QString("%1\t%2\t").arg(theSensitivityMeanValueSortedList[k][p], 0, 'g', 12).arg(theSensitivityStdvValueSortedList[k][p], 0, 'g', 12);
					}
					tempStr += "\n";
				}

				tempStrHdr += "\n";
				tempStr += "\n\n\n";

				byteArray.append(tempStrHdr);
				tempStrHdr.clear();

				byteArray.append(tempStr);
				tempStr.clear();

			}
			if (!theCCDFSensitivityFile.isEmpty()) {

				// The follwoing lines insert the headers (function names, etc.) and CCDF sensitvity values the text for the file theCCDFSensitivityFile
				tempStrHdr2 += QString("%1\t").arg(theFunctionList[i]->objectName());

				for (int p = 0; p < functioniValueSortedList.count(); p++) {
					tempStr2 += QString("%1\t").arg(theFunctionValueVectorList[i][p], 0, 'g', 12);
					for (int k = 0; k < theSRVSize; k++) {
						RContinuousRandomVariable *RV = qobject_cast<RContinuousRandomVariable *>(sensitivityRandomVariableList[k].data());
						if (p == 0) {
							tempStrHdr2 += QString("dCCDF[%1]/dMean[%2]\tCoV(dCCDF[%1]/dMean[%2])\tdCCDF[%1]/dStdv[%2]\tCoV(dCCDF[%1]/dStdv[%2])\t").arg(theFunctionList[i]->objectName()).arg(RV->objectName());
						}
						tempStr2 += QString("%1\t%2\t%3\t%4\t").arg(tempMeanSensitivityCurve[k][p], 0, 'g', 12).arg(tempMeanSensitivityCurveCoV[k][p], 0, 'g', 12).arg(tempStdvSensitivityCurve[k][p], 0, 'g', 12).arg(tempStdvSensitivityCurveCoV[k][p], 0, 'g', 12);
					}
					tempStr2 += "\n";
				}
				tempStrHdr2 += "\n";
				tempStr2 += "\n\n\n";

				byteArray2.append(tempStrHdr2);
				tempStrHdr2.clear();

				byteArray2.append(tempStr2);
				tempStr2.clear();
			}
		}
	}

	// Print sensitvity values to 2 output files
	if (!theJointPDFSensitivityFile.isEmpty()) {

		QString theFileName = QString(theJointPDFSensitivityFile);
		QFile *file = new QFile(theFileName);
		if (file->exists()) {
			file->remove();
		}

		if (!file->open(QIODevice::WriteOnly | QIODevice::Text)) {
			rCritical() << "Error: Could not create the file" << theFileName;
			return;
		}

		file->write(byteArray);
		file->flush();
		file->close();
		delete file;
		file = 0;

	}

	if (!theCCDFSensitivityFile.isEmpty()) {

		QString theFileName2 = QString(theCCDFSensitivityFile);
		QFile *file2 = new QFile(theFileName2);
		if (file2->exists()) {
			file2->remove();
		}

		if (!file2->open(QIODevice::WriteOnly | QIODevice::Text)) {
			rCritical() << "Error: Could not create the file" << theFileName2;
			return;
		}

		file2->write(byteArray2);
		file2->flush();
		file2->close();
		delete file2;
		file2 = 0;
	}
}


double RHistogramAccumulator::trapezoidalIntegration(QVector<double> x, QVector<double> y)
{
	double integral = 0.0;
	for (int i = 0; i < x.count() - 1; i++) {
		integral += 0.5 * (x[i + 1] - x[i]) * (y[i] + y[i + 1]);
	}
	return integral;
}

void RHistogramAccumulator::initiateSensitivityPlotsSlot() {


#ifndef NO_QCP
	for (int i = 0; i < theFunctionList.count(); i++) {

		// Sensitivity implementations
		if (theComputeRandomVariableSensitivities && theSensitivityFunctionFlagList[i]) {

			for (int k = 0; k < theSRVSize; k++) {

				if (getDisplayDiagrams() && (qobject_cast<QApplication*>(QCoreApplication::instance()) != 0)) {



					// Only plot at plotting frequency, or at the last sample
					if (theSensitivityPlotList[k + cntrSensFunc * theSRVSize] == 0 && (theDisplayMeanSensitvityDiagrams || theDisplayStandardDeviationSensitvityDiagrams)) {

						RMainWindow *mainWindow = qobject_cast<RMainWindow *>(parent()->parent());

						QCustomPlot *theSensitivityPlot = new RPlotWidget(mainWindow);

						theSensitivityPlot->setInteraction(QCP::iRangeDrag, true);
						theSensitivityPlot->setInteraction(QCP::iRangeZoom);
						QList<QCPAxis*> axisrect;
						axisrect << theSensitivityPlot->xAxis << theSensitivityPlot->yAxis << theSensitivityPlot->yAxis2;
						theSensitivityPlot->axisRect()->setRangeZoomAxes(axisrect);
						theSensitivityPlot->xAxis->setLabel("Function Value");
						theSensitivityPlot->yAxis->setLabel("Sensitivity Value");
						theSensitivityPlot->yAxis2->setLabel("CoV");
						theSensitivityPlot->yAxis2->setTickLabels(true);
						theSensitivityPlot->yAxis2->setVisible(true);
						mainWindow->addSubWindow(theSensitivityPlot, objectName() + " Sensitvity Plot, Function " + theFunctionList[i]->objectName() + ", variable " + theSensitivityRandomVariableList[k].data()->objectName(), true);

						theSensitivityPlot->legend->setVisible(true);
						theSensitivityPlotList[k + cntrSensFunc * theSRVSize] = theSensitivityPlot;

						if (theDisplayMeanSensitvityDiagrams) {
							// The Mean Sensitivity Plot
							// Instantiating Mean curve
							theSensitivityPlot->graph(0)->setAntialiased(true);
							theSensitivityPlot->graph(0)->setPen(QPen(Qt::red));
							theSensitivityPlot->graph(0)->setName("dCCDF/dMean");
							theMeanMuCurveList[k + cntrSensFunc * theSRVSize] = theSensitivityPlot->graph(0);

							// Instantiating COV curve
							theSensitivityPlot->graph(1)->setAntialiased(true);
							theSensitivityPlot->graph(1)->setPen(QPen(Qt::black));
							theSensitivityPlot->graph(1)->setName("CoV[dCCDF/dMean]");
							theMeanCoVCurveList[k + cntrSensFunc * theSRVSize] = theSensitivityPlot->graph(1);;
						}

						if (theDisplayStandardDeviationSensitvityDiagrams) {
							// The Stdv Sensitivity Plot
							// Instantiating Mean curve
							theSensitivityPlot->graph(2)->setAntialiased(true);
							theSensitivityPlot->graph(2)->setPen(QPen(Qt::blue));
							theSensitivityPlot->graph(2)->setName("dCCDF/dStdv");
							theStdvMuCurveList[k + cntrSensFunc * theSRVSize] = theSensitivityPlot->graph(2);

							// Instantiating COV curve
							theSensitivityPlot->addGraph(theSensitivityPlot->xAxis, theSensitivityPlot->yAxis2);
							theSensitivityPlot->graph(3)->setAntialiased(true);
							theSensitivityPlot->graph(3)->setPen(QPen(Qt::green));
							theSensitivityPlot->graph(3)->setName("CoV[dCCDF/dStdv]");
							theStdvCoVCurveList[k + cntrSensFunc * theSRVSize] = theSensitivityPlot->graph(3);
						}
					}
				}
			}
		}
	}
#endif // NO_QCP


}

bool RHistogramAccumulator::canBeRunParallelly() {
	return true;
}

bool RHistogramAccumulator::doesNeedStandardNormalRandomNumbers() {
	return true;
}