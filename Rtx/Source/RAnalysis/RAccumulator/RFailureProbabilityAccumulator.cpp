// Last modified by Mojtaba on March 19, 2014

#include "RFailureProbabilityAccumulator.h"

#include <QFile>
#include <QApplication>

#include "RRandomNumberGenerator.h"
#include "RFunction.h"
#include "RMainWindow.h"
#include "RPlotWidget.h"

#include <gsl/gsl_blas.h>
#include <gsl/gsl_cdf.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_vector.h>

//#include <qwt_plot_curve.h>

#include <math.h>

#include <qmutex.h>

#include "RContinuousRandomVariable.h"

#include <qreadwritelock.h>


RFailureProbabilityAccumulator::RFailureProbabilityAccumulator(QObject *parent, QString name)
	: RAccumulator(parent, name)
{
	// Initialize data members
	//$$ theRandomNumberGenerator = 0;
	theTargetCoefficientOfVariation = 0.02;

	governingCOV = 999.0;
	failureHasOccured = false;

	// thePlot = 0;

	bool ok = connect(this, SIGNAL(signalUpdatePlot()), this, SLOT(updatePlot()), Qt::BlockingQueuedConnection);

	theTargetCoefficientOfVariationLock = new QReadWriteLock();
}

RFailureProbabilityAccumulator::~RFailureProbabilityAccumulator()
{
	delete theTargetCoefficientOfVariationLock;
}

double RFailureProbabilityAccumulator::getTargetCoefficientOfVariation() const
{
	theTargetCoefficientOfVariationLock->lockForRead();
	double val = theTargetCoefficientOfVariation;
	theTargetCoefficientOfVariationLock->unlock();

	return val;
}

void RFailureProbabilityAccumulator::setTargetCoefficientOfVariation(double value)
{
	theTargetCoefficientOfVariationLock->lockForWrite();
	theTargetCoefficientOfVariation = value;
	theTargetCoefficientOfVariationLock->unlock();
}

long RFailureProbabilityAccumulator::getMaximumIterations()
{
	return RAccumulator::getMaximumIterations();
}

void RFailureProbabilityAccumulator::setMaximumIterations(long value)
{
	RAccumulator::setMaximumIterations(value);
}

QObject * RFailureProbabilityAccumulator::getRandomNumberGenerator() const
{
	return theRandomNumberGenerator.data();
}

void RFailureProbabilityAccumulator::setRandomNumberGenerator(QObject *value)
{
	theRandomNumberGenerator = qobject_cast<RRandomNumberGenerator *>(value);
}

void RFailureProbabilityAccumulator::reset()
{
	// Reset data members
	setTheContinueFlagValue(true);
	theFunctionList.clear();
	theIterationIndex = 0;
	qSum.clear();
	qSquaredSum.clear();
	qBar.clear();
	qBarVariance.clear();
	qBarCOV.clear();
	governingCOV = 999.0;
	failureHasOccured = false;

	thePfCovVector.clear();
	theIndexVector.clear();

	// thePlot = 0;

	// Sensitivity implementations
	qMeanSensitivity.clear();
	qStdvSensitivity.clear();
	qBarMeanSensitivity.clear();
	qBarStdvSensitivity.clear();
	qSquareMeanSensitivity.clear();
	qSquareStdvSensitivity.clear();
	qCOVMeanSensitivity.clear();
	qCOVStdvSensitivity.clear();
	theSensitivityFunctionFlagList.clear();
	theSRVSize = 0;
	theComputeRandomVariableSensitivities = false;
}

void RFailureProbabilityAccumulator::setSensitivityFunctionFlagList(QList<bool> flaglist)
{
	theSensitivityFunctionFlagList = flaglist;
}


int RFailureProbabilityAccumulator::accumulate(QVector<double> functionValueVector, gsl_vector * standardNormalRandomNumbers, QVector<double> sensitivityResultMeanVector, QVector<double> sensitivityResultStdvVector, int threadID)
{
	gsl_vector * u = standardNormalRandomNumbers;

	// Incrementing theIterationIndex
	theIterationIndex++;

	if (theIterationIndex == 1) {
		// Checking if all pointer properties are filled out
		int status = checkProperties();
		if (status < 0) {
			return -1;
		}
	}

	// Print the iteration number to the console
	if (getOutputDisplayLevel() > RObject::None) {
		rDebug(threadID) << "Sample" << theIterationIndex;
	}

	// Get the vector of start point from the RRandomNumberGenerator
	gsl_vector *startPoint = theRandomNumberGenerator.data()->getStartingPoint();

	// Get the sampling standard deviation from the RRandomNumberGenerator
	double samplingStandardDeviation = theRandomNumberGenerator.data()->getStandardDeviation();

	// Compute u^2 using GSL norm
	double u2 = gsl_pow_2(gsl_blas_dnrm2(u));

	// Calculating some constants
	double count = u->size;
	//double covarianceMatrixDet = pow(samplingStandardDeviation, count);
	//double factor1 = 1.0 / (pow(2 * M_PI, 0.5 * count));
	//double factor2 = factor1 / sqrt(covarianceMatrixDet);

	// Compute values of joint distributions at the u-point
	//double phi = factor1 * exp(-0.5 * u2);

	// Compute uMinusStartPoint using BLAS:  uMinusStartPoint = u - startPoint
	gsl_vector *uMinusStartPoint = gsl_vector_calloc(count);
	gsl_vector_memcpy(uMinusStartPoint, u);
	gsl_vector_sub(uMinusStartPoint, startPoint);

	// Compute uMinusStartPoint^2 using GSL norm
	double uMinusStartPoint2 = gsl_pow_2(gsl_blas_dnrm2(uMinusStartPoint));
	gsl_vector_free(uMinusStartPoint);
	uMinusStartPoint = 0;

	//double h = factor2 * exp(-0.5 * uMinusStartPoint2 / (samplingStandardDeviation * samplingStandardDeviation));

	double phi_h = exp(count / 2.0 * log(samplingStandardDeviation) + 0.5 *(uMinusStartPoint2 / (samplingStandardDeviation * samplingStandardDeviation) - u2));
	double I;

	int functionCount = theFunctionList.count();

	theIndexVector.append(theIterationIndex);


	// Sensitivity implementations
	if (theComputeRandomVariableSensitivities) {
		// Number of Sensitivity Random Variables
		theSRVSize = sensitivityResultMeanVector.size();

		// Resizing the vectors stores sensitivity results
		qMeanSensitivity.resize(theSRVSize * functionCount);
		qStdvSensitivity.resize(theSRVSize * functionCount);
		qBarMeanSensitivity.resize(theSRVSize * functionCount);
		qBarStdvSensitivity.resize(theSRVSize * functionCount);

		qSquareMeanSensitivity.resize(theSRVSize * functionCount);
		qSquareStdvSensitivity.resize(theSRVSize * functionCount);
		qCOVMeanSensitivity.resize(theSRVSize * functionCount);
		qCOVStdvSensitivity.resize(theSRVSize * functionCount);
	}


	// Iterating on functions
	for (int i = 0; i < functionCount; i++) {

		// Collect result of sampling
		if (functionValueVector[i] < 0.0) {
			I = 1;
			failureHasOccured = true;
		}
		else {
			I = 0;
		}

		// Update sums
		double q = I * phi_h;
		qSum[i] += q;
		qSquaredSum[i] += q * q;


		// Sensitivity implementations
		// TO DO: IMPLEMENT SAMPLING SENSITIVITY USING THE FORMULATION PROPOSED BY Sørensen ON PAGE 101 : NP, I do it. Sign:Hesam :)
		// Update sensitivity sums
		if (theComputeRandomVariableSensitivities && theSensitivityFunctionFlagList[i]) {
			for (int k = 0; k < theSRVSize; k++) {
				qMeanSensitivity[k + i * theSRVSize] += sensitivityResultMeanVector[k] * q;
				qStdvSensitivity[k + i * theSRVSize] += sensitivityResultStdvVector[k] * q;

				qBarMeanSensitivity[k + i * theSRVSize] = qMeanSensitivity[k + i * theSRVSize] / theIterationIndex;
				qBarStdvSensitivity[k + i * theSRVSize] = qStdvSensitivity[k + i * theSRVSize] / theIterationIndex;

				qSquareMeanSensitivity[k + i * theSRVSize] += gsl_pow_2(sensitivityResultMeanVector[k] * q);
				qSquareStdvSensitivity[k + i * theSRVSize] += gsl_pow_2(sensitivityResultStdvVector[k] * q);

				qCOVMeanSensitivity[k + i * theSRVSize] = qAbs(sqrt((qSquareMeanSensitivity[k + i * theSRVSize] / theIterationIndex - gsl_pow_2(qBarMeanSensitivity[k + i * theSRVSize])) / (theIterationIndex - 1)) / qBarMeanSensitivity[k + i * theSRVSize]);
				qCOVStdvSensitivity[k + i * theSRVSize] = qAbs(sqrt((qSquareStdvSensitivity[k + i * theSRVSize] / theIterationIndex - gsl_pow_2(qBarStdvSensitivity[k + i * theSRVSize])) / (theIterationIndex - 1)) / qBarStdvSensitivity[k + i * theSRVSize]);
			}
		}


		if (qSum[i] > 0.0) {
			// Compute coefficient of variation (of pf)
			qBar[i] = qSum[i] / theIterationIndex;
			qBarVariance[i] = (qSquaredSum[i] / theIterationIndex - gsl_pow_2(qBar[i])) / theIterationIndex;
			if (qBarVariance[i] < 0.0) {
				qBarVariance[i] = 0.0;
			}
			qBarCOV[i] = sqrt(qBarVariance[i]) / qBar[i];
		}

		thePfCovVector[i].append(qBarCOV[i]);

		// Display the results of this sample in output pane
		if (getOutputDisplayLevel() >= RObject::Moderate) {
			rDebug(threadID) << "Function" << theFunctionList[i]->objectName() << ":\t" << "pf Mean =" << qBar[i] << " , \tpf CoV =" << qBarCOV[i];
		}

		// Sensitivity implementations
		if (getOutputDisplayLevel() >= RObject::Maximum) {
			if (theComputeRandomVariableSensitivities && theSensitivityFunctionFlagList[i]) {
				rDebug(threadID) << "";
				rDebug(threadID) << "Pf Sensitivity result :";
				for (int k = 0; k < theSRVSize; k++) {
					rDebug(threadID) << theSensitivityRandomVariableList[k].data()->objectName() << " Mean: " << qBarMeanSensitivity[k + i * theSRVSize] << " (CoV:" << qCOVMeanSensitivity[k + i * theSRVSize] << ")" << " Stdv: " << qBarStdvSensitivity[k + i * theSRVSize] << " (CoV:" << qCOVStdvSensitivity[k + i * theSRVSize] << ")";
				}
			}
		}
	}

	if (getOutputDisplayLevel() >= RObject::Moderate) {
		rDebug(threadID) << "";
	}
	// Now all the functions have been looped over

	// Compute governing coefficient of variation
	if (!failureHasOccured) {
		governingCOV = 999.0;
	}
	else {
		governingCOV = 0.0;
		for (int i = 0; i < functionCount; i++) {
			if (qBarCOV[i] > governingCOV) {
				governingCOV = qBarCOV[i];
			}
		}
	}

	for (int i = 0; i < functionCount; i++) {
		if (qBarCOV[i] == 0.0) {
			governingCOV = 999.0;
		}
	}

	// Make sure the COV isn't exactly zero; that could be the case if only failures occur in cases where the 'q' remains 1
	if (governingCOV == 0.0) {
		governingCOV = 999.0;
	}

	// Update theContinueFlag
	if (((theIterationIndex >= getMaximumIterations()) || (governingCOV < getTargetCoefficientOfVariation())) && (theIterationIndex >= 10)) { //??????????????????????????????? Minimum number of samples is considered to be 10 rather than 2.
		setTheContinueFlagValue(false);
	}

	// emitting a signal to update plots if it is supposed to plot
	if (getDisplayDiagrams() && ((qobject_cast<QApplication*>(QCoreApplication::instance()) != 0)))
		if ((theIterationIndex % getPlottingInterval() == 0) || (getTheContinueFlagValue() == false))
			if (threadID > 0)
				emit signalUpdatePlot();
			else
				updatePlot();

	return 0;
}

void RFailureProbabilityAccumulator::terminateAccumulation() {

	rDebug() << "Sampling analysis is complete.";
	rDebug() << "";

	if (!failureHasOccured) {
		rDebug() << "Warning: Failure did not occur for any of the functions.";
	}

	int functionCount = theFunctionList.size();

	// Iterating on functions
	for (int i = 0; i < functionCount; i++) {
		double pf = qBar[i];
		double beta = -gsl_cdf_gaussian_Pinv(pf, 1.0);
		double COV = qBarCOV[i];

		if (thePlot)
		{
			thePlot->replot();
		}

		rDebug() << "";

		rDebug() << "Sampling analysis results for the function " << theFunctionList[i]->objectName() << ":";
		rDebug() << "\tThe reliability index:";
		rDebug() << "\tBeta =" << beta;
		rDebug() << "";
		rDebug() << "\tThe probability of failure:";
		rDebug() << "\tpf =" << pf;
		rDebug() << "";
		rDebug() << "\tThe coefficient of variation of sampling";
		rDebug() << "\tCoV =" << COV;
		rDebug() << "";


		// Sensitivity implementations
		if (theComputeRandomVariableSensitivities && theSensitivityFunctionFlagList[i]) {

			// Compute Beta sensitivities
			QVector<double> betaMeanSensitivity;
			QVector<double> betaStdvSensitivity;

			for (int k = 0; k < theSRVSize; k++) {
				double betaMeanSens = qBarMeanSensitivity[k + i * theSRVSize] * (-1 / gsl_ran_ugaussian_pdf(gsl_cdf_ugaussian_Pinv(pf)));
				double betaStdvSens = qBarStdvSensitivity[k + i * theSRVSize] * (-1 / gsl_ran_ugaussian_pdf(gsl_cdf_ugaussian_Pinv(pf)));
				betaMeanSensitivity << betaMeanSens;
				betaStdvSensitivity << betaStdvSens;
			}

			// Printing in Output pane
			rDebug() << "Sensitivty Results:";
			rDebug() << "";
			rDebug() << "Variable" << "\tdPf/dMean" << "\tCoV[dPf/dMean]" << "\tdBeta/dMean" << "\tdPf/dStdv" << "\tCoV[dPf/dStdv]" << "\tdBeta/dStdv";

			for (int k = 0; k < theSRVSize; k++) {
				rDebug() << theSensitivityRandomVariableList[k].data()->objectName() << "\t" << qBarMeanSensitivity[k + i * theSRVSize] << "\t" << qCOVMeanSensitivity[k + i * theSRVSize] << "\t" << betaMeanSensitivity[k] << "\t" << qBarStdvSensitivity[k + i * theSRVSize] << "\t" << qCOVStdvSensitivity[k + i * theSRVSize] << "\t" << betaStdvSensitivity[k];
			}

			rDebug() << "";

			// Set the sensitivity values to properties of each random variable
			for (int k = 0; k < theSRVSize; k++) {
				RContinuousRandomVariable *RV = qobject_cast<RContinuousRandomVariable *>(theSensitivityRandomVariableList[k].data());
				RV->setBetaSensitivityWrtMean(betaMeanSensitivity[k]);
				RV->setBetaSensitivityWrtStandardDeviation(betaStdvSensitivity[k]);

				double delta = betaMeanSensitivity[k] * RV->getStandardDeviationValue(0);
				double eta = betaStdvSensitivity[k] * RV->getStandardDeviationValue(0);

				RV->setDeltaSensitivityMeasure(delta);
				RV->setEtaSensitivityMeasure(eta);
			}

		}


		/*// TEMPORARY code to store pf in a text file name "pf.txt"
		QString fileName = QString("pf.txt");
		QFile *output = new QFile(fileName);
		if (output->exists()) {
		output->remove();
		}
		if (!output->open(QIODevice::WriteOnly | QIODevice::Text)) {
		rCritical() << "Warning: Could not create the file" << fileName;
		}

		QString tempString = QString("%1").arg(pf, 0, 'g', 14);
		QByteArray byteArray;
		byteArray.append(tempString);
		output->write(byteArray);
		output->flush();
		output->close();
		delete output;
		// END of the temporary code*/
	}

	rDebug() << "The total number of samples =" << theIterationIndex;
	rDebug() << "";
}

void RFailureProbabilityAccumulator::setFunctionList(QList<RFunction *> passedFunctionList)
{
	theFunctionList = passedFunctionList;
	int count = passedFunctionList.count();

	// Resizing the vectors qSum, qSquaredSum, qBar, qBarVariance, and qBarCOV and setting all its elements to Zero
	zeroQVector(&qSum, count);
	zeroQVector(&qSquaredSum, count);
	zeroQVector(&qBar, count);
	zeroQVector(&qBarVariance, count);
	zeroQVector(&qBarCOV, count);

	for (int i = 0; i < count; i++) {
		QVector<double> vector;
		thePfCovVector.append(vector);
	}



	if (getDisplayDiagrams() && ((qobject_cast<QApplication*>(QCoreApplication::instance()) != 0))) {

#ifndef NO_QCP

		theCurveList.clear();

		// Instatiating thePlot
		RMainWindow *mainWindow = qobject_cast<RMainWindow *>(parent()->parent());
		thePlot = new RPlotWidget(mainWindow);
		mainWindow->addSubWindow(thePlot, objectName() + " Plot", true);

		QList<QCPAxis*> axisrect;
		axisrect << thePlot->xAxis << thePlot->yAxis;
		thePlot->axisRect()->setRangeZoomAxes(axisrect);
		thePlot->axisRect()->setRangeDragAxes(axisrect);
		thePlot->setInteraction(QCP::iRangeDrag, true); // Enable dragging
		thePlot->setInteraction(QCP::iRangeZoom, true); // Enable zoom by scrolling

		thePlot->xAxis->setLabel("Number of Samples");
		thePlot->yAxis->setLabel("CoV of the Probability");

		thePlot->legend->setVisible(true);

		// Instantiating one curve for each function
		for (int i = 0; i < count; i++) {

			thePlot->addGraph(thePlot->xAxis, thePlot->yAxis);
			thePlot->graph(i)->setAntialiased(true);
			thePlot->graph(i)->setPen(QPen(Qt::GlobalColor(7 + i)));
			theCurveList << thePlot->graph(i);
			thePlot->graph(i)->setName(theFunctionList[i]->objectName());
		}
#endif // NO_QCP



	}
}

bool RFailureProbabilityAccumulator::getContinueFlag()
{
	return getTheContinueFlagValue();
}

void RFailureProbabilityAccumulator::zeroQVector(QVector<double> *passedVector, double count)
{
	// Resizing the vector to "count" and setting all its elements to Zero
	passedVector->resize(count);
	for (int i = 0; i < count; i++) {
		passedVector->replace(i, 0.0);
	}
}

double RFailureProbabilityAccumulator::getFailureProbability(RFunction *passedFunction)
{
	return qBar[theFunctionList.indexOf(passedFunction)];
}

void RFailureProbabilityAccumulator::updatePlot() {


	int functionCount = theFunctionList.count();

#ifndef NO_QCP
	if (thePlot)
	{
		for (int i = 0; i < functionCount; i++) {
			theCurveList[i]->setData(theIndexVector, thePfCovVector[i]);

		}

		thePlot->xAxis->rescale(true);
		thePlot->xAxis->setRangeUpper(1.05*thePlot->xAxis->range().upper);

		thePlot->yAxis->setRange(0.0, 1.1);

		thePlot->replot();
	}
#endif // NO_QCP

}

bool RFailureProbabilityAccumulator::canBeRunParallelly() {
	return true;
}

bool RFailureProbabilityAccumulator::doesNeedStandardNormalRandomNumbers() {
	return true;
}