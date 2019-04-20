// Last modified by Mojtaba on March 19, 2014

#include "RScenarioSamplingAnalyzer.h"

#include <QMap>
#include <QCoreApplication>
#include <QApplication>

#include <gsl/gsl_vector.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

#include "RMainWindow.h"
#include "RDomain.h"
#include "RFunction.h"
#include "RModel.h"
#include "RRandomVariable.h"
#include "RTime.h"
#include "RTransformer.h"
#include "RAccumulator.h"
#include "RRandomNumberGenerator.h"
#include "RFailureProbabilityAccumulator.h"
#include "RIndependentNormalRandomNumberGenerator.h"
#include "ROccurrenceModel.h"
#include "RResponse.h"
#include "RPlotWidget.h"

////#include <qwt_plot_curve.h>

#include <qmutex.h>
#include <QMutexLocker>

#include "RParallelProcessor.h"

RScenarioSamplingAnalyzer::RScenarioSamplingAnalyzer(QObject *parent, QString name)
	: RAnalyzer(parent, name), RParallelComputerInterface()
{
	/* //$$
	theRandomNumberGenerator = 0;
	theTransformer = 0;
	theAccumulator = 0;
	theTimespan = 18262;
	theTimeParameter = 0;

	thePhysicalRandomVariablesTransformer = 0;
	*/

	theComputeRandomVariableSensitivities = false;
	theNeglectCorrelationInSensitivity = false;

	sampleCountLock = new QMutex();

	analysisParallelComputer = new RParallelProcessor(this);

	theDisplayDiagramsLock = new QReadWriteLock();
}

RScenarioSamplingAnalyzer::~RScenarioSamplingAnalyzer()
{
	delete sampleCountLock;
	delete analysisParallelComputer;

	// if the auxiliary transformer is not null, deleting it
	if (thePhysicalRandomVariablesTransformer) {
		delete thePhysicalRandomVariablesTransformer.data();
	}

	delete theDisplayDiagramsLock;
}

QObject * RScenarioSamplingAnalyzer::getRandomNumberGenerator() const
{
	return theRandomNumberGenerator.data();
}

void RScenarioSamplingAnalyzer::setRandomNumberGenerator(QObject *value)
{
	theRandomNumberGenerator = qobject_cast<RRandomNumberGenerator *>(value);
}

QObject * RScenarioSamplingAnalyzer::getTransformer() const
{
	return theTransformer.data();
}

void RScenarioSamplingAnalyzer::setTransformer(QObject *value)
{
	// if the auxiliary transformer is not null, deleting it
	if (thePhysicalRandomVariablesTransformer) {
		delete thePhysicalRandomVariablesTransformer.data();
	}

	theTransformer = qobject_cast<RTransformer *>(value);
}

QObject * RScenarioSamplingAnalyzer::getAccumulator() const
{
	return theAccumulator.data();
}

void RScenarioSamplingAnalyzer::setAccumulator(QObject *value)
{
	theAccumulator = qobject_cast<RAccumulator *>(value);
}

QString RScenarioSamplingAnalyzer::getFunctionList()
{
	return functionListToString(theFunctionList);
}

void RScenarioSamplingAnalyzer::setFunctionList(QString value)
{
	theFunctionList = stringToFunctionList(value);
}

QString RScenarioSamplingAnalyzer::getFunctionAccumulationTypeList()
{
	return intVectorToString(theFunctionAccumulationTypeList);
}

void RScenarioSamplingAnalyzer::setFunctionAccumulationTypeList(QString value)
{
	theFunctionAccumulationTypeList = stringToIntVector(value);
}

QString RScenarioSamplingAnalyzer::getThresholdList()
{
	return doubleVectorToString(theThresholdList);
}

void RScenarioSamplingAnalyzer::setThresholdList(QString value)
{
	theThresholdList = stringToDoubleVector(value);
}

QString RScenarioSamplingAnalyzer::getOccurrenceModelList()
{
	return modelListToString(theOccurrenceModelList);
}

void RScenarioSamplingAnalyzer::setOccurrenceModelList(QString value)
{
	theOccurrenceModelList = stringToModelList(value);
}

void RScenarioSamplingAnalyzer::setDisplayDiagrams(bool value)
{
	theDisplayDiagramsLock->lockForWrite();
	theDisplayDiagrams = value;
	theDisplayDiagramsLock->unlock();
}

bool RScenarioSamplingAnalyzer::getDisplayDiagrams()
{
	theDisplayDiagramsLock->lockForRead();
	bool val = theDisplayDiagrams;
	theDisplayDiagramsLock->unlock();

	return val;
}

double RScenarioSamplingAnalyzer::getTimespan() const
{
	return theTimespan;
}

void RScenarioSamplingAnalyzer::setTimespan(double value)
{
	theTimespan = value;
}

QObject * RScenarioSamplingAnalyzer::getTimeParameter() const
{
	return theTimeParameter.data();
}

void RScenarioSamplingAnalyzer::setTimeParameter(QObject *value)
{
	theTimeParameter = qobject_cast<RTime *>(value);
}


QString RScenarioSamplingAnalyzer::getSensitivityRandomVariableList()
{
	QList<RPointer<RParameter> > paramList;
	for (int i = 0; i < theSensitivityRandomVariableList.size(); i++) {
        RPointer<RParameter> param = qobject_cast<RParameter *>(theSensitivityRandomVariableList[i].data()); //salarifard
		paramList << param;
	}

	return parameterListToString(paramList);
}

void RScenarioSamplingAnalyzer::setSensitivityRandomVariableList(QString value)
{
	theSensitivityRandomVariableList.clear();
	QList<RPointer<RParameter> > paramList = stringToParameterList(value);
	for (int i = 0; i < paramList.size(); i++) {
		RRandomVariable *rv = qobject_cast<RRandomVariable *>(paramList[i].data());
		if (rv) {
			theSensitivityRandomVariableList << rv;
		}
		else {
			// Issue an error if there is no such random variable
			rCritical() << "Error in" << objectName() << ": The variable" << paramList[i].data()->objectName() << "is not a random variable.";
			return;
		}
	}
}

bool RScenarioSamplingAnalyzer::getComputeRandomVariableSensitivities()
{
	return theComputeRandomVariableSensitivities;
}

void RScenarioSamplingAnalyzer::setComputeRandomVariableSensitivities(bool value)
{
	theComputeRandomVariableSensitivities = value;
}

bool RScenarioSamplingAnalyzer::getNeglectCorrelationInSensitivity()
{
	return theNeglectCorrelationInSensitivity;
}

void RScenarioSamplingAnalyzer::setNeglectCorrelationInSensitivity(bool value)
{
	theNeglectCorrelationInSensitivity = value;
}

QString RScenarioSamplingAnalyzer::getSensitivityFunctionList()
{
	return functionListToString(theSensitivityFunctionList);
}

void RScenarioSamplingAnalyzer::setSensitivityFunctionList(QString value)
{
	theSensitivityFunctionList = stringToFunctionList(value);
}


int RScenarioSamplingAnalyzer::analyze()
{
	// Inform the user that the sampling analysis has started
	rDebug() << "##### STARTING SCENARIO SAMPLING ANALYSIS ...";
	rDebug() << "";

	QDateTime startTime = QDateTime::currentDateTime();



	// Issue an error if the function list is empty
	if (theFunctionList.isEmpty()) {
		rCritical() << "Error in" << objectName() << ": Select at least one function and try again";
		return -1;
	}

	// Issue an error if the number of functions does not match the number of accumulation types
	if (theFunctionList.count() != theFunctionAccumulationTypeList.count()) {
		rCritical() << "Error in" << objectName() << ": Enter as many function accumulation types as the number of functions";
		return -1;
	}
	// Issue an error if the functionFlag is not a number between 0 to 2
	for (int i = 0; i < theFunctionAccumulationTypeList.count(); i++) {
		if ((theFunctionAccumulationTypeList[i] < 0) || (theFunctionAccumulationTypeList[i] > 3)) {
			rCritical() << "Error in" << objectName() << ": The function accumulation type should be either 0, 1, 2, or 3. Please correct and try again";
			return -1;
		}
	}

	if ((!theThresholdList.isEmpty()) && (theFunctionList.count() != theThresholdList.count())) {
		rCritical() << "Error in" << objectName() << ": Enter as many thresholds as the number of functions";
		return -1;
	}



	// Create a list of occurrence models
	hazardModelList.clear();
	for (int i = 0; i < theOccurrenceModelList.count(); i++) {
		ROccurrenceModel *model = qobject_cast<ROccurrenceModel *>(theOccurrenceModelList[i].data());
		if (model) {
			hazardModelList << model;
		}
		else {
			// Issue an error if the model is not an occurrence model
			rCritical() << "Error in" << objectName() << ": The model %1 is not an occurrence model. Please correct and try again";
			return -1;
		}
	}

	// Issue an error if the nothing is entered in the hazardLineEdit
	if (hazardModelList.isEmpty()) {
		rCritical() << "Error in" << objectName() << ": Enter at least one occurrence model and try again";
		return -1;
	}

	// Issue an error if T <=0
	if (theTimespan <= 0) {
		rCritical() << "Error in" << objectName() << ": Enter a timespan greater than zero and try again";
		return -1;
	}

	// Issue an error if the time object is not input
	if (!theTimeParameter) {
		rCritical() << "Error in" << objectName() << ": Enter a Time object and try again";
		return -1;
	}

	// Checking if all analysis tools are provided
	int result = checkProperties();
	if (result < 0) {
		rCritical() << "Error in" << objectName() << ": Could find one or more necessary tools";
		return -1;
	}

	T = theTimespan;

	// Reset the accumulator and the random generator
	theAccumulator.data()->reset();
	theRandomNumberGenerator.data()->reset();

	if ((theRandomNumberGenerator.data()->getStartPoint() == RRandomNumberGenerator::CurrentValue) && (theAccumulator.data()->inherits("RHistorgramAccumulator"))) {
		rCritical() << "WARNING: A histogram sampling around a point other than origin is not recommended.";
	}

	// Set the functionList in the accumulator
	QList<RFunction *> functionList;
	for (int i = 0; i < theFunctionList.count(); i++) {
		functionList << theFunctionList[i].data();
	}
	theAccumulator.data()->setFunctionList(functionList);

	// Sensitivity implementations
	QList<bool> sensitivityFunctionFlagList;
	if (theComputeRandomVariableSensitivities) {
		for (int i = 0; i < theSensitivityFunctionList.count(); i++) {
			if (!theFunctionList.contains(theSensitivityFunctionList[i])) {
				rCritical() << "Error: The function" << theSensitivityFunctionList[i].data()->objectName() << "that appears in the sensitivity function list of the scenario sampling analyzer must also be included in the function list.";
				return -2;
			}
		}
		for (int i = 0; i < theFunctionList.count(); i++) {
			if (theSensitivityFunctionList.contains(theFunctionList[i])) {
				sensitivityFunctionFlagList << true;
			}
			else {
				sensitivityFunctionFlagList << false;
			}
		}
		theAccumulator.data()->setSensitivityRandomVariableList(theSensitivityRandomVariableList);
		theAccumulator.data()->setSensitivityFunctionFlagList(sensitivityFunctionFlagList);
		theAccumulator.data()->setComputeRandomVariableSensitivities(true);

		// Issue an error if theSensitivityFunctionList is empty 
		if (theSensitivityFunctionList.isEmpty()) {
			rCritical() << "Error in" << objectName() << ": At least one function is needed for the sensitivity analysis.";
			return -2;
		}

		// Issue an error if theSensitivityRandomVariableList is empty 
		if (theSensitivityRandomVariableList.isEmpty()) {
			rCritical() << "Error in" << objectName() << ": At least one random variable is needed for the sensitivity analysis.";
			return -2;
		}
	}


	result = theRandomNumberGenerator.data()->checkProperties();
	if (result < 0) {
		rCritical() << "Error: Could find one or more necessary tools in the object" << theRandomNumberGenerator.data()->objectName();
		return -1;
	}

	result = theTransformer.data()->checkProperties();
	if (result < 0) {
		rCritical() << "Error: Could find one or more necessary tools in the object" << theTransformer.data()->objectName();
		return -1;
	}

	result = theAccumulator.data()->checkProperties();
	if (result < 0) {
		rCritical() << "Error: Could find one or more necessary tools in the object" << theAccumulator.data()->objectName();
		return -1;
	}

	// Iterating on the passed functions to parse them
	for (int i = 0; i < theFunctionList.count(); i++) {
		result = theFunctionList[i].data()->populateLists();
		if (result < 0) {
			rCritical() << "Error: Could not parse the function" << theFunctionList[i].data()->objectName();
			return -1;
		}
	}


	theRandomVariableList.clear();
	theModelSet.clear();

	for (int i = 0; i < theFunctionList.count(); i++) {
		// Create the mereged list of random variables used in all of the passed functions (and eliminating the repeated ones)
		QList<RRandomVariable *> *tempRandomVariableList = theFunctionList[i].data()->getRandomVariableList();
		for (int j = 0; j < tempRandomVariableList->count(); j++) {
			RRandomVariable *tempRandomVariable = tempRandomVariableList->value(j);
			if (!theRandomVariableList.contains(tempRandomVariable)) {
				theRandomVariableList << tempRandomVariable;
			}
		}

		// Create a merged "set" of models affecting the functions
		QSet<RModel *> tempModelSet = theFunctionList[i].data()->getModelList().toSet();
		theModelSet.unite(tempModelSet);
	}


	// Sorting the theRandomVariableList according to alphabetical order
	theDomain->sort(&theRandomVariableList);
	randomVariablesCount = theRandomVariableList.count();


	// create a list of random variables from physical parameters of all models of theModelSet
	physicalRandomVariableList.clear();

	QSetIterator<RModel *> i(theModelSet);
	while (i.hasNext()) {

		RModel * modelFromSet = i.next();

		QList<RParameter *> paramList = modelFromSet->getPhysicalParameterList();
		for (int l = 0; l < paramList.count(); l++) {
			RRandomVariable *rv = qobject_cast<RRandomVariable *>(paramList[l]);
			if (rv) {
				if (!physicalRandomVariableList.contains(rv)) {
					physicalRandomVariableList << rv;
				}
			}
		}
	}

	// Sorting the the list according to alphabetical order
	theDomain->sort(&physicalRandomVariableList);
	physicalRandomVariablesCount = physicalRandomVariableList.count();

	// This is almost never the case except if the user is dumb enough :) to give all constants to hazards
	if (physicalRandomVariablesCount == 0) {
		/*QString errorStr;
		for (int ii = 1; i < swichedOnHazardsIndexVector.count(); i++) {
		errorStr += hazardModelList[swichedOnHazardsIndexVector[i]]->objectName() + ", ";
		}
		errorStr = errorStr.remove(errorStr.size()-2, 2) + ".";
		rCritical() << "No random variables to sample at the time instant t =" << currentTime << ". There should be at least one random variable to be sampled when a hazard occurs. Check the Magnitude Models of the following hazards:" << qPrintable(errorStr);*/
		rCritical() << "No physical parameters to sample. There should be at least one random variable to be sampled for each hazard.";
		return -1;
	}

	// Create a list of the "unused" hazard occurrence models in the domain (the ones that are in the domain, but are not given in the dialog box of the orchestrating algorithm)
	unusedOccurrenceModelList.clear();
	unusedOccurrenceModelList = theDomain->findChildren<ROccurrenceModel *>();
	for (int i = 0; i < hazardModelList.count(); i++) {
		unusedOccurrenceModelList.removeOne(hazardModelList[i]);
	}

	// Building a list of the models asssociated with unusedOccurrenceModelList and taking away those which do not affect the functions (This list will be used to forcibly turn off the hazards that affect the function, but are not given as a hazard to the dialog box of the orchestrating algorithm)	
	unusedAssociatedModelList.clear();
	unusedMagnitudeModelList.clear();
	for (int i = 0; i < unusedOccurrenceModelList.count(); i++) {
		RModel *tempModel = qobject_cast<RModel *>(unusedOccurrenceModelList[i]->getMagnitudeModel());
		if ((!unusedMagnitudeModelList.contains(tempModel)) && (theModelSet.contains(tempModel))) {
			unusedMagnitudeModelList << tempModel;
		}
		QList<RModel *> tempModelList = unusedOccurrenceModelList[i]->getAssociatedModels();
		for (int j = 0; j < tempModelList.count(); j++) {
			if (!unusedAssociatedModelList.contains(tempModelList[j]) && theModelSet.contains(tempModelList[j])) {
				unusedAssociatedModelList << tempModelList[j];
			}
		}
	}

	// Set the random variable list in theTransformer, which also intilizes the transfomer for a new analysis
	result = theTransformer.data()->setRandomVariableList(&theRandomVariableList);
	if (result < 0) {
		rCritical() << "Error: Could not set the list of random variables to the transformer.";
		return -1;
	}

	// Setting the list of random variables to the RandomNumberGenerator (In RandomNumberGenerator types which need a start point, this line also established the start point vector)
	result = theRandomNumberGenerator.data()->setRandomVariableList(&theRandomVariableList);
	if (result < 0) {
		rCritical() << "Error: Could not set the list of random variables to the random number generator.";
		return -1;
	}

	// setting the list of physical random variables as the random variable list of the auxiliary transformer
	thePhysicalRandomVariablesTransformer = theTransformer.data()->createHelperTransformer();
	result = thePhysicalRandomVariablesTransformer.data()->setRandomVariableList(&physicalRandomVariableList);
	if (result < 0) {
		rCritical() << "Error: Could not set the list of random variables to the transformer.";
		return -1;
	}

	// Backing up the currentValue of the random variables (now that theStartPoint vector is established in theRandomNumberGenerator, we can do this; otherwise, we would miss the value of storedValue by backing up.
	QVector<double> randomVariablesBackup;
	for (int i = 0; i < randomVariablesCount; i++) {
		randomVariablesBackup.append(theRandomVariableList[i]->getCurrentValue());
	}

	// Initializing a boolean for stop condition
	bool continueCondition = true;

	bool isFirstSimulation = true;
	//double seed = 1;


	//// Create a list of start and end time objects
	//QList<RTime *> startTimeList;
	//QList<RTime *> endTimeList;
	//for (int i = 0; i < hazardModelList.count(); i++) {
	//	RTime *startTime = qobject_cast<RTime *>(hazardModelList[i]->getStartTime());
	//	if (startTime) {
	//		startTimeList << startTime;
	//	} else {
	//		rCritical() << "Error: The start time given to" << hazardModelList[i]->objectName() << "is not a time object. Please correct and try again.";
	//		return -1;
	//	}
	//	RTime *endTime = qobject_cast<RTime *>(hazardModelList[i]->getEndTime());
	//	if (endTime) {
	//		endTimeList << endTime;
	//	} else {
	//		rCritical() << "Error: The end time given to" << hazardModelList[i]->objectName() << "is not a time object. Please correct and try again.";
	//		return -1;
	//	}
	//}


	// We need a time object to be given to the dialog box that at each moment shows the current time and at the beginning, shows the date and time that corresponds to the begininng of the T
	initialTime = theTimeParameter.data()->getCurrentValue();




#ifndef NO_QCP


	if ((qobject_cast<QApplication*>(QCoreApplication::instance()) != 0) && (theDisplayDiagrams = !0)) {
		

		// Plotting

		theTimePlot.clear();
		theCurveList.clear();
		currentTimeCurve = 0;
		timePlotHeight = 0.9;

		// Instatiating a time plot
		RMainWindow *mainWindow = qobject_cast<RMainWindow *>(parent()->parent());
		theTimePlot = new RPlotWidget(theMainWindow);
		theMainWindow->addSubWindow(theTimePlot.data(), "Scenario Sampling Time Plot", true);;

		QList<QCPAxis*> axisrect;
		axisrect << theTimePlot.data()->xAxis << theTimePlot.data()->yAxis;
		theTimePlot.data()->axisRect()->setRangeZoomAxes(axisrect);
		theTimePlot->axisRect()->setRangeDragAxes(axisrect);
		theTimePlot->setInteraction(QCP::iRangeDrag, true); // Enable dragging
		theTimePlot->setInteraction(QCP::iRangeZoom, true); // Enable zoom by scrolling

		theTimePlot.data()->xAxis->setLabel("Time (Days)");
		theTimePlot.data()->yAxis->setLabel("Ocuurrence");

		theTimePlot.data()->legend->setVisible(true);

		// Instantiating one curve for each hazard
		for (int i = 0; i < hazardModelList.count(); i++) {

			theTimePlot.data()->addGraph(theTimePlot.data()->xAxis, theTimePlot.data()->yAxis);
			theTimePlot.data()->graph(i)->setAntialiased(true);
			theTimePlot.data()->graph(i)->setPen(QPen(Qt::GlobalColor(7 + i)));
			theCurveList << theTimePlot.data()->graph(i);
			theTimePlot.data()->graph(i)->setName(hazardModelList[i]->objectName());
		}
		//theTimePlot.data()->enableAxis(QwtPlot::yLeft, false);

		// Instantiating one curve for the vertical line of the current time
		int count = hazardModelList.count();
		theTimePlot.data()->addGraph(theTimePlot.data()->xAxis, theTimePlot.data()->yAxis);
		theTimePlot.data()->graph(count)->setAntialiased(true);
		theTimePlot.data()->graph(count)->setPen(QPen(Qt::black));
		currentTimeCurve = theTimePlot.data()->graph(count);
		theTimePlot.data()->graph(count)->setName("Current Time");
	}

#endif // NO_QCP


	// a counter for ongoing or collected samples
	int sampleCount = 0;

	// Sensitivity implementations
	int SRVSize = 0;
	if (theComputeRandomVariableSensitivities) {
		// Set number of sensitivity random variables
		SRVSize = theSensitivityRandomVariableList.size();

		for (int i = 0; i < SRVSize; i++) {
			if (!theRandomVariableList.contains(theSensitivityRandomVariableList[i].data())) {
				rCritical() << "Error: The list of random variables that affect the functions enterd for sampling analysis does not include" << theSensitivityRandomVariableList[i].data()->objectName();
				return -2;
			}
		}

		// Declare that the sensitvity due to accumulation type = addition is not available
		for (int j = 0; j < theFunctionList.count(); j++) {
			if (theFunctionAccumulationTypeList[j] == 0 && sensitivityFunctionFlagList[j]) {
				rCritical() << "The Sensitivity analysis is not available for accumulation type = addition (0). Please use other accumulation types for function" << theFunctionList[j].data()->objectName();
				return -2;
			}
		}
	}


	// doing main analysis
	void * inputArgs[] = { (void *)(&isFirstSimulation), (void *)(&sampleCount) };
	int status = analysisParallelComputer->run(&RScenarioSamplingAnalyzer::doAnalysisWrapper, this, inputArgs);

	QCoreApplication::processEvents();

	// checking error occurance
	if (status < 0)
		return status;

	theAccumulator.data()->terminateAccumulation();

	QDateTime finishTime = QDateTime::currentDateTime();
	double numDays = startTime.daysTo(finishTime);
	double numMilliSecs = startTime.time().msecsTo(finishTime.time());
	double numSecs = numDays * 86400.0 + numMilliSecs / 1000.0;

	rDebug() << "##### SCENARIO SAMPLING ANALYSIS DONE IN" << numSecs << "SECONDS.";
	rDebug() << "";
	rDebug() << "";

	// Restoring the currentValue of the random variables from the storedValue
	for (int i = 0; i < randomVariablesCount; i++) {
		theRandomVariableList[i]->setCurrentValue(randomVariablesBackup[i]);
	}

	// Restoring the time object to the initial time
	theTimeParameter.data()->setCurrentValue(initialTime);

	// clearing the list of plot curves
	theCurveList.clear();

	return 0;
}

int RScenarioSamplingAnalyzer::doAnalysisWrapper(void * theObject, void * inputArgs[], int threadID) {

	// casting object
	RScenarioSamplingAnalyzer * thisSampling = (RScenarioSamplingAnalyzer *)(theObject);

	// casting input arguments
	bool firstSimulation = *((bool *)inputArgs[0]);
	int * generalSampleCount = (int *)inputArgs[1];

	// calling the method
	return thisSampling->doAnalysis(firstSimulation, generalSampleCount, threadID);
}

int RScenarioSamplingAnalyzer::doAnalysis(bool isFirstSimulation, int * generalSampleCount, int threadID) {

	// ATTENTION : general error messages (error messages related to all threads) will be printed only in the first simulation

	int result;

	// Declaring vector u (Attention: Vector u should not be allocated here, it just gets the memory address of the genereted random numbers
	gsl_vector *u = 0;
	// Declaring vector x (Attention: Vector x SHOULD be allocated in this method, somewhere below)
	gsl_vector *x = 0;

	// Sensitivity implementations
	// Allocating the vectors to store sensitivity results in each sample
	gsl_vector *dfdMean_1_f = 0;
	gsl_vector *dfdStdv_1_f = 0;

	int SRVSize = 0;

	if (theComputeRandomVariableSensitivities) {
		// Set number of sensitivity random variables
		SRVSize = theSensitivityRandomVariableList.size();

		dfdMean_1_f = gsl_vector_calloc(SRVSize);
		dfdStdv_1_f = gsl_vector_calloc(SRVSize);
	}

	long plottingFrequency = theAccumulator.data()->getPlottingInterval();
	bool plottingIsEnabled = (qobject_cast<QApplication*>(QCoreApplication::instance()) != 0) && (theDisplayDiagrams = !0) && (threadID == 0);

	// getting the random generator engine
	gsl_rng * randomGenerator = theRandomNumberGenerator.data()->getRandomGeneratorEngine(threadID);

	bool continueConditionInThread = true;

	// start running samples till continue condition flag becomes false
	while (analysisParallelComputer->getContinueCondition() && continueConditionInThread) {

		int sampleCount;

		sampleCountLock->lock();
		sampleCount = ++(*generalSampleCount);
		sampleCountLock->unlock();

		// Creating a map of the start time of hazards and their corresponding index
		QMap<double, int> startTimeMap;
		QList<QVector<double> > startTimeVectorList;
		QList<QVector<double> > endTimeVectorList;

		for (int i = 0; i < hazardModelList.count(); i++) {
			// Establishing the time axis for this hazard
			ROccurrenceModel *model = hazardModelList[i];

			result = model->establishTimeAxis(T, randomGenerator, threadID);

			if (result < 0) {

				if (isFirstSimulation) {
					rCritical(threadID) << "Error: Could not establish the time axis for the hazard" << model->objectName();
				}

				return endAnalysis(x, dfdMean_1_f, dfdStdv_1_f, -1);
			}

			// Adding the start time vector and end time vector to a big list and also adding the start times to the map
			QVector<double> startVector = model->getStartTimeVector(threadID);
			QVector<double> endVector = model->getEndTimeVector(threadID);
			for (int j = 0; j < startVector.count(); j++) {
				startTimeMap.insertMulti(startVector[j], i);
			}
			startTimeVectorList << startVector;
			endTimeVectorList << endVector;

#ifndef NO_QCP

			if (plottingIsEnabled) {
				// Plotting the time axes
				// Clearing theCurveList
				if ((theTimePlot) && (sampleCount % plottingFrequency == 0)) {
					// Creating x and y vectors
					QVector<double> xVector;
					QVector<double> yVector;

					//if (startVector[0]!=0)
					//{
					//	xVector << 0.0;
					//	yVector << i;
					//}

					xVector << 0.0;
					yVector << i;

					for (int j = 0; j < startVector.count(); j++) {
						xVector << startVector[j] << startVector[j];
						xVector << endVector[j] << endVector[j];
						yVector << i << i + timePlotHeight;
						yVector << i + timePlotHeight << i;
					}
					xVector << T << T;
					yVector << i << i;
					theCurveList[i]->setData(xVector, yVector);

					// Send the current time vectical line in the time plot to Zero
					QVector<double> xCurrentVector;
					QVector<double> yCurrentVector;
					xCurrentVector << 0.0 << 0.0;
					yCurrentVector << 0.0 << hazardModelList.count();
					currentTimeCurve->setData(xCurrentVector, yCurrentVector);
				}
			}

#endif // NO_QCP

		}

#ifndef NO_QCP

		// Update the plot
		if (plottingIsEnabled) {
			if ((theTimePlot) && (sampleCount % plottingFrequency == 0)) {

				theTimePlot->xAxis->rescale(true);
				theTimePlot->yAxis->rescale(true);
				theTimePlot->yAxis2->rescale(true);


				theTimePlot->xAxis->setRangeUpper(1.05*theTimePlot->xAxis->range().upper);
				theTimePlot->yAxis->scaleRange(1.1);
				theTimePlot->yAxis2->scaleRange(1.1);

				theTimePlot.data()->replot();
			}
		}

#endif // NO_QCP



		// Build a vector of unique start times
		QVector<double> uniqueStartTimeList = startTimeMap.uniqueKeys().toVector();

		// Creating a list of booleans that tracks which hazards are on and which are off
		QList<bool> onList;
		for (int i = 0; i < hazardModelList.count(); i++) {
			onList << false;
		}

		// Declaring a QVector of double to store the results of this sample for all function
		QVector<double> sampleResult;
		// Declaring a QVector of double to store the local maximum of the result (used for function with "Add" as their accumulation type. The local maximum function value is stored until it is time for it to be added.
		QVector<double> sampleLocalMaximum;
		for (int i = 0; i < theFunctionList.count(); i++) {
			sampleResult << 0;
			sampleLocalMaximum << 0;
		}

		double currentTime = 0;
		double previousTime = 0;

		bool convergence = true;

		bool shouldAddFunction = false;
		bool oldShouldAddFunction = true;

		// Resetting the history variables
		QSetIterator<RModel *> modelSetIterator(theModelSet);
		while (modelSetIterator.hasNext()) {
			modelSetIterator.next()->resetHistoryVariables(threadID);
		}


		// Sensitivity implementations
		// Allocating the vectors to store final sensitivity results
		QVector<double> sensitivityResultMeanVector;
		QVector<double> sensitivityResultStdvVector;
		sensitivityResultMeanVector.resize(SRVSize);
		sensitivityResultStdvVector.resize(SRVSize);


		for (int i = 0; i < uniqueStartTimeList.count(); i++) {
			previousTime = currentTime;
			currentTime = uniqueStartTimeList[i];
			theTimeParameter.data()->setCurrentValue(initialTime + currentTime, threadID);

#ifndef NO_QCP
			if (plottingIsEnabled) {
				// Update the current time vectical line in the time plot
				if ((theTimePlot) && (sampleCount % plottingFrequency == 0)) {
					QVector<double> xVector;
					QVector<double> yVector;
					xVector << currentTime << currentTime;
					yVector << 0.0 << hazardModelList.count();
					currentTimeCurve->setData(xVector, yVector);

					theTimePlot->xAxis->rescale(true);
					theTimePlot->yAxis->rescale(true);
					theTimePlot->yAxis2->rescale(true);


					theTimePlot->xAxis->setRangeUpper(1.05 *theTimePlot->xAxis->range().upper);
					theTimePlot->yAxis->scaleRange(1.1);
					theTimePlot->yAxis2->scaleRange(1.1);

					theTimePlot.data()->replot();
				}
			}
#endif // NO_QCP


			// Modifying the onList for the hazards that has become off since the last time	
			for (int j = 0; j < onList.count(); j++) {
				if (onList[j]) {
					QVector<double> startTimeVector = startTimeVectorList[j];
					QVector<double> endTimeVector = endTimeVectorList[j];
					if (endTimeVector[0] <= currentTime) { // Meaning that the hazard has become off
						onList[j] = false;
						endTimeVector.remove(0);
						endTimeVectorList[j] = endTimeVector;
						startTimeVector.remove(0);
						startTimeVectorList[j] = startTimeVector;

						//// Setting the start and end time objects of the hazard that has become off to the currentTime (?)
						//startTimeList[j]->setCurrentValue(initialTime + currentTime);
						//endTimeList[j]->setCurrentValue(initialTime + currentTime);
					}
				}
			}


			// Modifying the onList for the hazards that have become on since the last time
			QVector<int> swichedOnHazardsIndexVector = startTimeMap.values(currentTime).toVector();
			for (int j = 0; j < swichedOnHazardsIndexVector.count(); j++) {
				int index = swichedOnHazardsIndexVector[j];
				onList[index] = true;

				//double startTime = startTimeVectorList[index][0];
				//double endTime = endTimeVectorList[index][0];
				//startTimeList[index]->setCurrentValue(initialTime + startTime);
				//endTimeList[index]->setCurrentValue(initialTime + endTime);

				// ATTENTION: THIS IMPLEMENTATION ASSUMES THAT NONE OF THESE "ASSOCIATED" MODELS ARE SHARED BY TWO OR MORE HAZARDS"ON" 
				// Old implementation, which only assumes that the magnitude model and the associated model list may have physical parameters
				/*QList<RModel *> modelList = hazardModelList[index]->getAssociatedModels();
				RModel *model = qobject_cast<RModel *>(hazardModelList[index]->getMagnitudeModel());
				modelList << model;
				for (int k = 0; k < modelList.count(); k++) {
				QList<RParameter *> paramList = modelList[k]->getPhysicalParameterList();
				for (int l = 0; l < paramList.count(); l++) {
				RRandomVariable *rv = qobject_cast<RRandomVariable *>(paramList[l]);
				if (rv) {
				newRandomVariableList << rv;
				}
				}
				}*/
			}

			// Syntax to keep the GUI responsive
			if (threadID == 0) {
				QCoreApplication::processEvents();
			}

			int currentCount;
			QList<RRandomVariable *> currentRandomVariableList;

			if (i == 0) {
				currentCount = randomVariablesCount;
				currentRandomVariableList = theRandomVariableList;
			}
			else {
				currentCount = physicalRandomVariablesCount;
				currentRandomVariableList = physicalRandomVariableList;
			}

			// Allocating vector x
			if (x) {
				gsl_vector_free(x);
				x = 0;
			}
			x = gsl_vector_calloc(currentCount);

			// Generating random numbers in standard-normal space
			if (isFirstSimulation) {
				result = theRandomNumberGenerator.data()->generateNumbers(currentCount/*, seed*/, threadID);
			}
			else {
				result = theRandomNumberGenerator.data()->generateNumbers(currentCount, threadID);
			}
			//seed = theRandomNumberGenerator->getSeed();
			if (result < 0) {
				rCritical(threadID) << "Error: Could not generate random numbers for sampling analysis.";
				return endAnalysis(x, dfdMean_1_f, dfdStdv_1_f, -1);
			}

			// Getting the vector of generated random numbers
			u = theRandomNumberGenerator.data()->getGeneratedNumbers(threadID);

			// creating a safe area for transforming (as different threads may use different data sets)

			// if it is the first time, the main transformer should be used
			if (i == 0) {
				// Transform starting point into original space
				result = theTransformer.data()->transformPointToOriginalSpace(u, x, threadID);
			}
			else {
				// Transform starting point into original space
				result = thePhysicalRandomVariablesTransformer.data()->transformPointToOriginalSpace(u, x, threadID);
			}

			if (result < 0) {
				rCritical(threadID) << "Error: Could not transform from standard-normal space to original space.";
				return endAnalysis(x, dfdMean_1_f, dfdStdv_1_f, -1);
			}

			// Update the current value of random variables to the new realization
			for (int j = 0; j < currentCount; j++) {
				currentRandomVariableList[j]->setCurrentValue(gsl_vector_get(x, j), threadID);
			}

			// Iterating on the passed functions and calling the method to update the isAnalyzed flags of RResponses due to the new realization of random variables for each function
			for (int j = 0; j < theFunctionList.count(); j++) {
				RFunction *function = theFunctionList[j].data();
				function->updateAnalysisStatus(threadID);
			}


			for (int j = 0; j < hazardModelList.count(); j++) {
				// Setting the value of the magnitude model and associated model list responses for "off" hazards to Zero (plus setting their isAnalyzed flag to true, so that they are not run)
				if (!onList[j]) {
					RModel *model = qobject_cast<RModel *>(hazardModelList[j]->getMagnitudeModel());
					model->setResponseAnalyzed(true, threadID);
					QList<RResponse *> responseList = model->findChildren<RResponse *>();
					for (int k = 0; k < responseList.count(); k++) {
						responseList[k]->setCurrentValue(0, threadID);
					}

					QList<RModel *> modelList = hazardModelList[j]->getAssociatedModels();
					for (int k = 0; k < modelList.count(); k++) {
						modelList[k]->setResponseAnalyzed(true, threadID);

						// Setting to zero the responses of the models associated with the hazards other than those that has just become on to true 						
						QList<RResponse *> responseList = modelList[k]->findChildren<RResponse *>();
						for (int k = 0; k < responseList.count(); k++) {
							responseList[k]->setCurrentValue(0, threadID);
						}

						QList<RModel *> modelList = hazardModelList[j]->getAssociatedModels();
						for (int k = 0; k < modelList.count(); k++) {
							modelList[k]->setResponseAnalyzed(true, threadID);

							// Setting to zero the responses of the models associated with the hazards other than those that has just become on to true 						
							QList<RResponse *> responseList = modelList[k]->findChildren<RResponse *>();
							for (int k = 0; k < responseList.count(); k++) {
								responseList[k]->setCurrentValue(0, threadID);
							}
						}
					}
				}

				// Setting the isAnalyzed flag of the associated models and the magniude model of the hazards other than those that has just become on to true, so that they are not run
				if (!swichedOnHazardsIndexVector.contains(j)) {
					RModel *model = qobject_cast<RModel *>(hazardModelList[j]->getMagnitudeModel());
					model->setResponseAnalyzed(true, threadID);

					// ATTENTION: THIS IMPLEMENTATION ASSUMES THAT NONE OF THESE "ASSOCIATED" MODELS ARE SHARED BY TWO OR MORE HAZARDS
					QList<RModel *> modelList = hazardModelList[j]->getAssociatedModels();
					for (int k = 0; k < modelList.count(); k++) {
						modelList[k]->setResponseAnalyzed(true, threadID);
					}
				}
			}

			// Setting the value of the magnitude model responses associated with "unused" hazards (explained above) to Zero (plus setting their isAnalyzed flag to true, so that they are not run)
			for (int j = 0; j < unusedMagnitudeModelList.count(); j++) {
				unusedMagnitudeModelList[j]->setResponseAnalyzed(true, threadID);
				QList<RResponse *> responseList = unusedMagnitudeModelList[j]->findChildren<RResponse *>();
				for (int k = 0; k < responseList.count(); k++) {
					responseList[k]->setCurrentValue(0, threadID);
				}
			}

			// Setting the isAnalyzed flag of the models associated with the "unused" hazards (explained above) to true, so that they are not run
			for (int j = 0; j < unusedAssociatedModelList.count(); j++) {
				unusedAssociatedModelList[j]->setResponseAnalyzed(true, threadID);
			}





			// When no hazard is "on", the functions that have the "Add" accumulation type are added. Otherwise (when some hazards are on), the maximum is found until all hazards are off and then that maximum is added.
			// Determining if at least one hazard remains on till the next analysis (which means that the function should not be added)
			shouldAddFunction = true;
			if (i < uniqueStartTimeList.count() - 1) { // In the last analysis, the function should be added anyway.
				for (int j = 0; j < onList.count(); j++) {
					if (onList[j]) {
						QVector<double> startTimeVector = startTimeVectorList[j];
						QVector<double> endTimeVector = endTimeVectorList[j];
						if (endTimeVectorList[j][0] > uniqueStartTimeList[i + 1]) { // Meaning that the hazard will not become "off" before the next analysis
							shouldAddFunction = false;
							break;
						}
					}
				}
				// If a rare hazard, e.g., an earthquake, comes on, then the "Add" accumulation type functions are accumulated. 
				for (int j = 0; j < hazardModelList.count(); j++) {
					if (hazardModelList[j]->isRareHazard()) {
						if (onList[j]) {
							shouldAddFunction = true;
							break;
						}
					}
				}
			}


			// Sensitivity implementations
			bool computeJointPDFSensitivity = false;

			// Iterating on the passed functions
			for (int j = 0; j < theFunctionList.count(); j++) {
				RFunction *function = theFunctionList[j].data();

				// Evaluate the function
				result = function->evaluateFunction(false, RObject::RandomVariable, threadID);
				double functionValue = function->getFunctionValue(threadID);

				// result = 0 mean function is evaluated without error;
				// result = -1 means function is not evaluated due to errors other than non-convergence;
				// result < -1 means function is not evaluated due to non-convergence.
				if (result < 0) {
					rCritical(threadID) << "WARNING: Could not evaluate the function" << function->objectName();
					if (result < -1) {
						// This means that for non-convergence, the function could not be evaluated. So it is assumed that the system has failed. Thus, the function value is set to some number less than Zero, e.g. -1.0.
						functionValue = -1.0;
						if (theAccumulator.data()->inherits("RHistogramAccumulator")) {
							rCritical(threadID) << "WARNING: Non-convergence in this sample. The sample is discarded.";
							convergence = false;
							break;
						}
						else {
							rCritical(threadID) << "WARNING: Assuming failure in this sample due to non-convergence.";
						}
					}
					else {
						// This means that for some reason other than non-convergence, the function could not be evaluated. So the sampling is terminated.
						rCritical(threadID) << "Please correct the error and try again.";
						return endAnalysis(x, dfdMean_1_f, dfdStdv_1_f, -1);
					}
				}
				//rDebug() << "";


				// Updating the result corresponding to this function based on the accumulation type of the function
				if (theFunctionAccumulationTypeList[j] == 0) {			// Add
					if (oldShouldAddFunction) {
						sampleLocalMaximum[j] = functionValue;
					}
					else if (sampleLocalMaximum[j] < functionValue) {
						sampleLocalMaximum[j] = functionValue;
					}
					if (shouldAddFunction) {
						sampleResult[j] += sampleLocalMaximum[j];
					}
					oldShouldAddFunction = shouldAddFunction;
				}
				else if (theFunctionAccumulationTypeList[j] == 1) {	// Max
					if (i == 0) {
						sampleResult[j] = functionValue;
						computeJointPDFSensitivity = true;
					}
					else if (sampleResult[j] < functionValue) {
						sampleResult[j] = functionValue;
						computeJointPDFSensitivity = true;
					}
				}
				else if (theFunctionAccumulationTypeList[j] == 2) {	// Min
					if (i == 0) {
						sampleResult[j] = functionValue;
						computeJointPDFSensitivity = true;
					}
					else if (sampleResult[j] > functionValue) {
						sampleResult[j] = functionValue;
						computeJointPDFSensitivity = true;
					}
				}
				else if (theFunctionAccumulationTypeList[j] == 3) {	// Last
					if (i == uniqueStartTimeList.count() - 1) {
						sampleResult[j] = functionValue;
						computeJointPDFSensitivity = true;
					}
				}
			}

			// Sensitivity implementations
			// Compute sensitivity wrt to Mean or Stdv for current event
			if (theComputeRandomVariableSensitivities && computeJointPDFSensitivity) {
				// Allocating the vectors to gets sensitivity result from Transformer in each sample
				gsl_vector_set_zero(dfdMean_1_f);
				gsl_vector_set_zero(dfdStdv_1_f);

				gsl_vector *xAll = gsl_vector_calloc(randomVariablesCount);
				gsl_vector *uAll = gsl_vector_calloc(randomVariablesCount);
				for (int k = 0; k < randomVariablesCount; k++) {
					gsl_vector_set(xAll, k, theRandomVariableList[k]->getCurrentValue(threadID));
				}

				int status = theTransformer.data()->transformPointToStandardNormalSpace(xAll, uAll, threadID);
				if (status < 0) {
					rCritical(threadID) << "Error: Could not transform point to the standard normal space for sensitivty analysis";
					return endAnalysis(x, dfdMean_1_f, dfdStdv_1_f, -2);
				}

				status = theTransformer.data()->computeJointPDFSensitivityToDistributionParameters(xAll, uAll, theSensitivityRandomVariableList, dfdMean_1_f, dfdStdv_1_f, theNeglectCorrelationInSensitivity, threadID);
				if (status) {
					rCritical(threadID) << "Error: Could not compute sensitivity wrt to Mean or Stdv.";
					return endAnalysis(x, dfdMean_1_f, dfdStdv_1_f, -2);
				}

				if (xAll) {
					gsl_vector_free(xAll);
					xAll = 0;
				}
				if (uAll) {
					gsl_vector_free(uAll);
					uAll = 0;
				}

				for (int k = 0; k < SRVSize; k++) {
					sensitivityResultMeanVector[k] = gsl_vector_get(dfdMean_1_f, k);
					sensitivityResultStdvVector[k] = gsl_vector_get(dfdStdv_1_f, k);
				}
			}


			// Updating the history variables
			QSetIterator<RModel *> modelSetIterator(theModelSet);
			while (modelSetIterator.hasNext()) {
				modelSetIterator.next()->updateHistoryVariables(threadID);
			}

			// Just a delay//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			//for (int x = 0; x < 1000000000; x++) { }
		}

		// APPLYING THE FUNCTION THRESHOLDS
		if (!theThresholdList.isEmpty()) {
			for (int i = 0; i < sampleResult.count(); i++) {
				sampleResult[i] = theThresholdList[i] - sampleResult[i];
			}
		}

#ifndef NO_QCP

		if (plottingIsEnabled) {
			// Update the current time vectical line in the time plot
			if ((theTimePlot) && (sampleCount % plottingFrequency == 0)) {
				QVector<double> xCurrentVector;
				QVector<double> yCurrentVector;
				xCurrentVector << T << T;
				yCurrentVector << 0.0 << hazardModelList.count();
				currentTimeCurve->setData(xCurrentVector, yCurrentVector);

				theTimePlot->xAxis->rescale(true);
				theTimePlot->yAxis->rescale(true);
				theTimePlot->yAxis2->rescale(true);

				theTimePlot->xAxis->setRangeUpper(1.05*theTimePlot->xAxis->range().upper);
				theTimePlot->yAxis->scaleRange(1.1);
				theTimePlot->yAxis2->scaleRange(1.1);

				theTimePlot.data()->replot();
			}
		}

#endif // NO_QCP


		// Accumulate the result of this sample
		if (convergence) {

			result = theAccumulator.data()->accumulate(sampleResult, u, sensitivityResultMeanVector, sensitivityResultStdvVector, threadID);

			if (result < 0) {
				rCritical(threadID) << "Error: Could not accumulate the results of this sample.";
				return endAnalysis(x, dfdMean_1_f, dfdStdv_1_f, -1);
			}

			// Check for continuing condition from the accumulator
			continueConditionInThread = theAccumulator.data()->getContinueFlag();
		}

		if (threadID == 0) {
			QCoreApplication::processEvents();
		}
	}

	return endAnalysis(x, dfdMean_1_f, dfdStdv_1_f, 0);
}

int RScenarioSamplingAnalyzer::endAnalysis(gsl_vector * x, gsl_vector * dfdMean_1_f, gsl_vector * dfdStdv_1_f, int errorCode) {

	if (x != 0)
		gsl_vector_free(x);

	if (dfdMean_1_f != 0)
		gsl_vector_free(dfdMean_1_f);

	if (dfdStdv_1_f != 0)
		gsl_vector_free(dfdStdv_1_f);

	return errorCode;
}


void RScenarioSamplingAnalyzer::initializeForParallelComputation(int threadID) {

	// initializing transformer
	theTransformer.data()->initializeForParallelComputation(threadID);
	thePhysicalRandomVariablesTransformer.data()->initializeForParallelComputation(threadID);

	// initializing all the contributed functions
	for (int i = 0; i < theFunctionList.count(); i++) {
		theFunctionList[i].data()->initializeForParallelComputation(threadID);
	}
}

void RScenarioSamplingAnalyzer::resetAfterParallelComputation() {

	// initializing all the contributed functions
	for (int i = 0; i < theFunctionList.count(); ++i){
		theFunctionList[i].data()->resetAfterParallelComputation();
	}
}

bool RScenarioSamplingAnalyzer::canBeRunParallelly() {

	// checking properties of type QObject*
	if (!canAllQObjectStarPropertiesBeRunParallelly()) {
		rDebug() << "Hence, the Scenario Sampling Analyzer cannot be run using parallel processing. ";
		return false;
	}

	// checking contributed functions
	for (int i = 0; i < theFunctionList.count(); ++i){
		if (!theFunctionList[i].data()->canBeRunParallelly()) {
			rDebug() << "Hence, the Scenario Sampling Analyzer cannot be run using parallel processing. ";
			return false;
		}
	}

	return true;
}
