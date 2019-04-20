//Developed by Hossein Nasr
#include "RDiscreteEventSamplingAnalyzer.h"

#include <QMap>
#include <qvector.h>
#include <qlist.h>
#include <QCoreApplication>
#include <QApplication>
#include <QVector>
//#include <QPointer>
#include <QFile>
#include <QFileInfo>
#include <QString>

#include <gsl/gsl_vector.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

#include "RMainWindow.h"
#include "RDomain.h"
#include "RFunction.h"
#include "RModel.h"
#include "RConstant.h"
#include "RDecisionVariable.h"
#include "RRandomVariable.h"
#include "RTime.h"
#include "RTransformer.h"
#include "RAccumulator.h"
#include "RHistogramAccumulator.h"
#include "RRandomNumberGenerator.h"
#include "RFailureProbabilityAccumulator.h"
#include "RIndependentNormalRandomNumberGenerator.h"
#include "ROccurrenceModel.h"
#include "REventModel.h"
#include "RResponse.h"
#include "RPlotWidget.h"
#include "RPoissonPulseProcessModel.h"
#include "qstring.h"

#include <qmutex.h>
#include <QMutexLocker>
#include <qthread.h>
#include "RParallelProcessor.h"


#include <gsl/gsl_blas.h>
#include <gsl/gsl_cdf.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_histogram.h>
#include <gsl/gsl_math.h>
RDiscreteEventSamplingAnalyzer::RDiscreteEventSamplingAnalyzer(QObject *parent, QString name)
	: RAnalyzer(parent, name), RParallelComputerInterface()
{
	theFunctionEstimationInterval = 1;
	//theTimeSpan = 1826;

	int arraySizeForParallelizedObject = getArraySizeForParallelizedObject();
	multiThreaded = arraySizeForParallelizedObject - 1;

	sampleCountLock = new QMutex();
	theIterationIndexLock = new QMutex();
	plottingIsEnabledLock = new QMutex();
	theContinueFlagLock = new QMutex();
	theMaxTimeSizeLock = new QMutex();

	accumulatorLock = new QMutex();
	modifyTimeSizeLock = new QMutex();

	updateSamplePlotLock = new QMutex();
	evaluateDependencyLock = new QMutex();


	analysisParallelComputer = new RParallelProcessor(this);
}

RDiscreteEventSamplingAnalyzer::~RDiscreteEventSamplingAnalyzer()
{
	delete sampleCountLock;

	delete theIterationIndexLock;
	delete plottingIsEnabledLock;
	delete theContinueFlagLock;

	delete accumulatorLock;
	delete modifyTimeSizeLock;
	delete updateSamplePlotLock;
	delete evaluateDependencyLock;

	delete analysisParallelComputer;
	//// if the auxiliary transformer is not null, deleting it
	//if (thePhysicalRandomVariablesTransformer) {
	//	delete thePhysicalRandomVariablesTransformer.data();
	//}
}

bool RDiscreteEventSamplingAnalyzer::getDisplayDiagrams()
{
	return theDisplayDiagrams;
}

void RDiscreteEventSamplingAnalyzer::setDisplayDiagrams(bool value)
{
	theDisplayDiagrams = value;
}

double RDiscreteEventSamplingAnalyzer::getFunctionEstimationInterval() const
{
	return theFunctionEstimationInterval;
}

void RDiscreteEventSamplingAnalyzer::setFunctionEstimationInterval(double value)
{
	theFunctionEstimationInterval = value;
}

QObject * RDiscreteEventSamplingAnalyzer::getRandomNumberGenerator() const
{
	return theRandomNumberGenerator.data();
}

void RDiscreteEventSamplingAnalyzer::setRandomNumberGenerator(QObject *value)
{
	theRandomNumberGenerator = qobject_cast<RRandomNumberGenerator *>(value);
}

QObject * RDiscreteEventSamplingAnalyzer::getTransformer() const
{
	return theTransformer.data();
}

void RDiscreteEventSamplingAnalyzer::setTransformer(QObject *value)
{
	//// if the auxiliary transformer is not null, deleting it
	//if (thePhysicalRandomVariablesTransformer) {
	//	delete thePhysicalRandomVariablesTransformer.data();
	//}
	theTransformer = qobject_cast<RTransformer *>(value);
}

QObject * RDiscreteEventSamplingAnalyzer::getAccumulator() const
{
	return theAccumulator.data();
}

void RDiscreteEventSamplingAnalyzer::setAccumulator(QObject *value)
{
	theAccumulator = qobject_cast<RAccumulator *>(value);
}

QString RDiscreteEventSamplingAnalyzer::getFunctionList()
{
	return functionListToString(theFunctionList);
}

void RDiscreteEventSamplingAnalyzer::setFunctionList(QString value)
{
	theFunctionList = stringToFunctionList(value);
}

QString RDiscreteEventSamplingAnalyzer::getFunctionAccumulationTypeList()
{
	return intVectorToString(theFunctionAccumulationTypeList);
}

void RDiscreteEventSamplingAnalyzer::setFunctionAccumulationTypeList(QString value)
{
	theFunctionAccumulationTypeList = stringToIntVector(value);
}

QString RDiscreteEventSamplingAnalyzer::getOccurrenceModelList()
{
	return modelListToString(theOccurrenceModelList);
}

void RDiscreteEventSamplingAnalyzer::setOccurrenceModelList(QString value)
{
	theOccurrenceModelList = stringToModelList(value);
}

QString RDiscreteEventSamplingAnalyzer::getEventModelList()
{
	return modelListToString(theEventModelList);
}

void RDiscreteEventSamplingAnalyzer::setEventModelList(QString value)
{
	theEventModelList = stringToModelList(value);
}

//double RDiscreteEventSamplingAnalyzer::getTimespan() const
//{
//	return theTimeSpan;
//}
//
//void RDiscreteEventSamplingAnalyzer::setTimespan(double value)
//{
//	theTimeSpan = value;
//}

QObject * RDiscreteEventSamplingAnalyzer::getTimeParameter() const
{
	return theTimeParameter.data();
}

void RDiscreteEventSamplingAnalyzer::setTimeParameter(QObject *value)
{
	theTimeParameter = qobject_cast<RTime *>(value);
}

int RDiscreteEventSamplingAnalyzer::analyze()
{
	// Inform the user that the sampling analysis has started
	QDateTime startTime = QDateTime::currentDateTime(); //salarifard

	QString showstarttime = startTime.toString();
	QString years = showstarttime.section(' ', 4, 4);
	QString days = showstarttime.section(' ', 2, 2);
	QString hours = showstarttime.section(' ', 3, 3);
	QString months = showstarttime.section(' ', 1, 1);



	if (showstarttime.contains("Jan", Qt::CaseInsensitive)) {
		months = "JANUARY";

	}
	else if (showstarttime.contains("Feb", Qt::CaseInsensitive)) {
		months = "FEBRUARY";

	}
	else if (showstarttime.contains("Mar", Qt::CaseInsensitive)) {
		months = "MARCH";

	}
	else if (showstarttime.contains("Apr", Qt::CaseInsensitive)) {
		months = "APRIL";

	}
	else if (showstarttime.contains("May", Qt::CaseInsensitive)) {
		months = "MAY";

	}
	else if (showstarttime.contains("Jun", Qt::CaseInsensitive)) {
		months = "JUNE";

	}
	else if (showstarttime.contains("Jul", Qt::CaseInsensitive)) {
		months = "JULY";

	}
	else if (showstarttime.contains("Aug", Qt::CaseInsensitive)) {
		months = "AUGUST";

	}
	else if (showstarttime.contains("Sep", Qt::CaseInsensitive)) {
		months = "SEPTEMBER";

	}
	else if (showstarttime.contains("Oct", Qt::CaseInsensitive)) {
		months = "OCTOBER";

	}
	else if (showstarttime.contains("Nov", Qt::CaseInsensitive)) {
		months = "NOVEMBER";

	}
	else if (showstarttime.contains("Dec", Qt::CaseInsensitive)) {
		months = "DECEMBER";

	}



	rDebug() << "##### DISCRETE EVENT SAMPLING ANALYSIS STARTED ON " + months + " " + days + ", " + years + " AT " + hours; //salarifard


	
	rDebug() << "";


	//variable to check each method work correctly
	int result;

	// Issue an error if the recovery function list is empty
	if (theFunctionList.isEmpty()) {
		rCritical() << "Error in" << objectName() << ": Select at least one recovery function and try again";
		return -1;
	}

	//Create the default accumulation type list
	if (theFunctionAccumulationTypeList.count() > theFunctionList.count()) {
		int diff = theFunctionAccumulationTypeList.count() - theFunctionList.count();
		for (int i = theFunctionAccumulationTypeList.count() - 1; i < theFunctionAccumulationTypeList.count() + diff - 1; i++) {
			theFunctionAccumulationTypeList.removeAt(i);
		}

	}

	if (theFunctionAccumulationTypeList.count() < theFunctionList.count()) {
		int diff = -theFunctionAccumulationTypeList.count() + theFunctionList.count();
		for (int i = 0; i < diff; i++) {
			int last = theFunctionAccumulationTypeList.last();
			theFunctionAccumulationTypeList << last;
		}
	}

	if (!theFunctionAccumulationTypeList.isEmpty()) {
		if (theFunctionAccumulationTypeList.count() != theFunctionList.count()) {
			rCritical() << "Error: Please enter as many accumulation type as the number of functions";
			return -1;
		}
	}
	for (int i = 0; i < theFunctionAccumulationTypeList.count(); i++) {
		if ((theFunctionAccumulationTypeList[i] < 0) || (theFunctionAccumulationTypeList[i] > 3)) {
			rCritical() << "Error in" << objectName() << ": The function accumulation type should be either 0, 1, 2, or 3. Please correct and try again";
			return -1;
		}
	}

	// Iterating on the passed functions to parse them
	for (int i = 0; i < theFunctionList.count(); i++) {
		result = theFunctionList[i].data()->populateLists();
		if (result < 0) {
			rCritical() << "Error: Could not parse the function" << theFunctionList[i].data()->objectName();
			return -1;
		}
	}


	doAccumulate = false;
	bool zeroAccumulationTypeList = true;
	if (theFunctionAccumulationTypeList.isEmpty()) {
		for (int i = 0; i < theFunctionList.count(); i++) {
			theFunctionAccumulationTypeList << 0;
		}
	}

	for (int f = 0; f < theFunctionList.count(); f++) {
		if (theFunctionAccumulationTypeList[f] != 0)
			zeroAccumulationTypeList = false;
	}

	doAccumulate = (multiThreaded && !zeroAccumulationTypeList) || (!multiThreaded && (theDisplayDiagrams || !zeroAccumulationTypeList));

	hazardModelList.clear();
	hazardOccurenceRateList.clear();
	for (int i = 0; i < theOccurrenceModelList.count(); i++) {
		ROccurrenceModel *hazardModel = qobject_cast<ROccurrenceModel *>(theOccurrenceModelList[i].data());
		if (hazardModel) {
			//// ATTENTION: This analysis for now only support Poisson Pulse Processes as occurrence model.
			//RPoissonPulseProcessModel *poissonModel = qobject_cast<RPoissonPulseProcessModel *>(hazardModel);
			//if (!poissonModel) {
			//	rCritical() << "Error in" << objectName() << ": Currently, The Discrete Event Sampling Analysis only supports Poisson Pulse Processes as occurrence models";
			//	return -1;
			//}

			//hazardModelList << poissonModel;
			//hazardOccurenceRateList << poissonModel->getNonzeroOccurrenceRate().toDouble();

			hazardModelList << hazardModel;
			hazardOccurenceRateList << hazardModel->getNonzeroOccurrenceRateValue();
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

	// Create a list of Event models

	eventModelList.clear();
	for (int i = 0; i < theEventModelList.count(); i++) {
		REventModel *eventModel = qobject_cast<REventModel *>(theEventModelList[i].data());
		if (eventModel) {
			eventModelList << eventModel;
		}
		else {
			// Issue an error if the model is not an occurrence model
			rCritical() << "Error in" << objectName() << ": The model %1 is not an event model. Please correct and try again";
			return -1;
		}
	}

	// Issue an error if the nothing is entered in the event model list
	if (eventModelList.isEmpty()) {
		rCritical() << "Error in" << objectName() << ": Enter at least one event model and try again";
		return -1;
	}

	//if (theTimeSpan <= 0) {
	//	rCritical() << "Error in" << objectName() << ": Enter a timespan greater than zero and try again";;
	//	return -1;
	//}

	if (!theTimeParameter) {
		rCritical() << "Error in" << objectName() << ": Enter a Time object and try again";
		return -1;
	}

	//// Checking if all analysis tools are provided - - - - it should not be in comment mode
	//result = checkProperties();
	//if (result < 0) {
	//	rCritical() << "Error in" << objectName() << ": Could find one or more necessary tools";
	//	return -1;
	//}

	if (!theRandomNumberGenerator) {
		rCritical() << "Error: please enter one Random Number Generator";
		return -1;
	}

	if (!theTransformer) {
		rCritical() << "Error: please enter one Transformer";
		return -1;
	}

	if (!theAccumulator) {
		rCritical() << "Error: please enter one Accumulator";
		return -1;
	}


	result = theRandomNumberGenerator.data()->checkProperties();
	if (result < 0) {
		rCritical() << "Error: Could find one or more necessary tools in the object" << theRandomNumberGenerator.data()->objectName();
		return -1;
	}
	if ((theRandomNumberGenerator.data()->getStartPoint() == RRandomNumberGenerator::CurrentValue)) {
		rCritical() << "WARNING: the start point of the Random Number Generator should be set to" << "Origin";
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

	/*if ((theRandomNumberGenerator->getStartPoint() == RRandomNumberGenerator::CurrentValue) && (theAccumulator->inherits("RHistorgramAccumulator"))) {
	rCritical() << "WARNING: A histogram sampling around a point other than origin is not recommended.";
	}*/
	theHistogramAccumulator = qobject_cast<RHistogramAccumulator *> (theAccumulator.data());
	if (!theHistogramAccumulator) {
		rCritical() << "ERROR: The accumulator should be a Histogram Accumulator";
		return -1;
	}

	//*** Create the function that evalualtes the initial status of the system
	allInitialStateFunction = new RFunction(theDomain, "allInitialStateFunction");
	QString expression = "0";
	for (int i = 0; i < theFunctionList.count(); i++) {
		//QList<RResponse *> initialStateResponseList = theFunctionList[i].data()->getResponseList();
		//for (int j = 0; j < initialStateResponseList.count(); j++) {
		//	expression = expression + "+" + initialStateResponseList[j]->objectName();
		//}
		expression = expression + "+" + theFunctionList[i].data()->getExpression();
	}
	
	bool ok;
	for (int i = 0; i < eventModelList.count(); i++) {
		QList<RResponse *> eventResponseList = eventModelList[i]->getResponseList();
		for (int j = 0; j < eventResponseList.count(); j++) {
			//if (!theAllParameterList.contains(eventResponseList[j])) {
			//	theAllParameterList << eventResponseList[j];
			//}
			expression = expression + "+" + eventResponseList[j]->objectName();
		}

		QList<RRandomVariable *> eventRVList = eventModelList[i]->getRandomVariableList();
		for (int j = 0; j < eventRVList.count(); j++) {
			////if (!theAllParameterList.contains(eventConstantList[j])) {
			//theAllParameterList << eventRVList[j];
			////}
			expression = expression + "+" + eventRVList[j]->objectName();
		}
		
		QList<RConstant *> eventConstantList = eventModelList[i]->getConstantList();
		for (int j = 0; j < eventConstantList.count(); j++) {
			////if (!theAllParameterList.contains(eventConstantList[j])) {
			//	theAllParameterList << eventConstantList[j];
			////}
			expression = expression + "+" + eventConstantList[j]->objectName();
		}

		QList<RDecisionVariable *> eventDecisionVariableList = eventModelList[i]->getDecisionVariableList();
		for (int j = 0; j < eventDecisionVariableList.count(); j++) {
			////if (!theAllParameterList.contains(eventDecisionVariableList[j])) {
			//	theAllParameterList << eventDecisionVariableList[j];
			////}
			expression = expression + "+" + eventDecisionVariableList[j]->objectName();
		}

		QList<RParameter *> eventPhysicalParameterList = eventModelList[i]->getPhysicalParameterList();
		for (int j = 0; j < eventPhysicalParameterList.count(); j++) {
			////if (!theAllParameterList.contains(eventPhysicalParameterList[j])) {
			//	theAllParameterList << eventPhysicalParameterList[j];
			////}
			expression = expression + "+" + eventPhysicalParameterList[j]->objectName();
		}
	}

	allInitialStateFunction->setExpression(expression);
	result = allInitialStateFunction->populateLists();
	if (result < 0) {
		rCritical() << "Error: Could not parse the initial state function";
		return -1;
	}



	////To test the Evaluate depedndency method- - - - - tested - - - - - OK
	//evaluateDependencies(theExampleParameter);
	//for (int i=0; i < theDependencyHash[theExampleParameter].count(); i++) {
	//	rDebug() << theDependencyHash[theExampleParameter][i]->objectName() ;
	//}

	bool displayDiagrams = theDisplayDiagrams;
	//bool convergence = true;
	//bool continueCondition = true;
	int sampleCount = 0;

	theIterationIndex = 0;
	theMaxTimeSize = 0;

	//clear data members before the analysis
	theAllRandomVariableList.clear();
	//physicalRandomVariableList.clear();
	backupAllRandomVariablesValueList.clear();
	//allParameterBackupList.clear();
	hazardNormalizedOccurenceRateList.clear();
	unusedEventModelList.clear();
	unusedOccurrenceModelList.clear();
	unusedAssociatedModelList.clear();
	unusedMagnitudeModelList.clear();
	allFunctionResponseSet.clear();
	maxFixedIntervalVector.clear();

	double hazardSumOfOccurenceRate;

	// We need a time object to be given to the dialog box that at each moment shows the current time and at the beginning, shows the date and time that corresponds to the begininng of the T
	initialTime = theTimeParameter.data()->getCurrentValue();
	//double timeSpan = theTimeSpan;

	plottingIsEnabled = (!multiThreaded) && (theDisplayDiagrams) && (qobject_cast<QApplication*>(QCoreApplication::instance()) != 0);
	thePlottingInterval = theAccumulator.data()->getPlottingInterval();

	// ***** START OF INITIALIZATION BEFORE START OF SIMULATION *****

	//Create the list of normalized occurence rate
	//hazardNormalizedOccurenceRateList.clear();
	hazardSumOfOccurenceRate = 0;

	for (int i = 0; i < hazardOccurenceRateList.count(); i++) {
		hazardSumOfOccurenceRate += hazardOccurenceRateList[i];
	}

	for (int i = 0; i < hazardOccurenceRateList.count(); i++) {
		hazardNormalizedOccurenceRateList << (hazardOccurenceRateList[i] / hazardSumOfOccurenceRate);
	}

	//All model list contains all the models that the functions contain plus the event models
	theAllModelList = allInitialStateFunction->getModelList();

	for (int i = 0; i < eventModelList.count(); i++) {
		theAllModelList << (eventModelList[i]);
	}
	backupAllModelList = theAllModelList;

	for (int i = 0; i < theFunctionList.count(); i++) {
		RFunction *recoveryFunction = theFunctionList[i].data();
		QList<RResponse *> tempResponseList = recoveryFunction->getResponseList();

		for (int j = 0; j < tempResponseList.count(); j++) {
			allFunctionResponseSet.insert(tempResponseList[j]);
		}


	}


	// Create a list of the "unused" hazard models in the domain (the ones that are in the domain, but are not given in the dialog box of the orchestrating algorithm)
	unusedOccurrenceModelList = theDomain->findChildren<ROccurrenceModel *>();
	for (int i = 0; i < hazardModelList.count(); i++) {
		unusedOccurrenceModelList.removeOne(hazardModelList[i]);
	}

	// Building a list of the models associated with unusedOccurrenceModelList and taking away those which do not affect the functions (This list will be used to forcibly turn off the hazards that affect the function, but are not given as a hazard to the dialog box of the orchestrating algorithm)	
	for (int i = 0; i < unusedOccurrenceModelList.count(); i++) {
		RModel *tempModel = qobject_cast<RModel *>(unusedOccurrenceModelList[i]->getMagnitudeModel());
		if ((!unusedMagnitudeModelList.contains(tempModel)) && (theAllModelList.contains(tempModel))) {
			unusedMagnitudeModelList << tempModel;
		}
		QList<RModel *> tempModelList = unusedOccurrenceModelList[i]->getAssociatedModels();
		for (int j = 0; j < tempModelList.count(); j++) {
			if (!unusedAssociatedModelList.contains(tempModelList[j]) && theAllModelList.contains(tempModelList[j])) {
				unusedAssociatedModelList << tempModelList[j];
			}
		}
	}

	// Create a list of the "unused" event models in the domain (the ones that are in the domain, but are not given in the dialog box of the orchestrating algorithm)
	unusedEventModelList = theDomain->findChildren<REventModel *>();
	for (int i = 0; i < eventModelList.count(); i++) {
		unusedEventModelList.removeOne(eventModelList[i]);
	}

	//foreach(RModel *modelFromSet, theAllModelList) {
	//	modelFromSet->reset();
	//}

	for (int i = 0; i < theAllModelList.count(); i++) {
		RModel *tempModel = theAllModelList[i];
		tempModel->reset();
	}
	
	// Reset the accumulator and the random generator
	theAccumulator.data()->reset();
	theRandomNumberGenerator.data()->reset();
	this->resetAccumulator();

	//Clear the dependency hash
	theDependencyHash.clear();

	// Set the functionList in the accumulator - the recovery function list should be set in the accumulator
	QList<RFunction *> AccumulatorFunctionList;
	for (int i = 0; i < theFunctionList.count(); i++) {
		AccumulatorFunctionList << theFunctionList[i].data();
	}

	theAccumulator.data()->setFunctionList(AccumulatorFunctionList);
	this->initializeAccumulator();

	// Create the mereged list of random variables used in all of the passed functions (and eliminating the repeated ones)
	QList<RRandomVariable *> *tempRandomVariableList = allInitialStateFunction->getRandomVariableList();
	for (int j = 0; j < tempRandomVariableList->count(); j++) {
		RRandomVariable *tempRandomVariable = tempRandomVariableList->value(j);
		//if (!theAllRandomVariableList.contains(tempRandomVariable)) {
			theAllRandomVariableList << tempRandomVariable;
		//}
	}

	//// Create the mereged list of all paramter used in all of the passed functions (and eliminating the repeated ones)
	//for (int j = 0; j < theInitialModelList.count(); j++) {
	//	bool ok;
	//	QList<RDecisionVariable *> tempDVList = theInitialModelList[j]->getDecisionVariableList();
	//	for (int k = 0; k < tempDVList.count(); k++) {
	//		RParameter *tempParam = tempDVList[k];
	//		//if (!theAllParameterList.contains(tempParam)) {
	//			theAllParameterList << tempParam;
	//		//}
	//	}

	//	QList<RConstant *> tempConstList = theInitialModelList[j]->getConstantList();
	//	for (int k = 0; k < tempConstList.count(); k++) {
	//		RParameter *tempParam = tempConstList[k];
	//		//if (!theAllParameterList.contains(tempParam)) {
	//			theAllParameterList << tempParam;
	//		//}
	//	}

	//	QList<RParameter *> tempPhysicalParameterList = theInitialModelList[j]->getPhysicalParameterList();
	//	for (int k = 0; k < tempPhysicalParameterList.count(); k++) {
	//		RParameter *tempParam = tempPhysicalParameterList[k];
	//		//if (!theAllParameterList.contains(tempParam)) {
	//			theAllParameterList << tempParam;
	//		//}
	//	}
	//}

	//for (int i = 0; i < hazardModelList.count(); i++) {
	//	QList<RParameter *> hazardParameterList = hazardModelList[i]->getAllParametersList(&ok);
	//	for (int j = 0; j < hazardParameterList.count(); j++) {
	//		RParameter *tempParam = hazardParameterList[j];
	//		if (!theAllParameterList.contains(tempParam)) {
	//			theAllParameterList << tempParam;
	//		}
	//	}
	//}
	

	//Create upstream response list for all models
	for (int i = 0; i < theAllModelList.count(); i++) {
		RModel *tempModel = theAllModelList[i];
		tempModel->createUpstreamResponseList();
	}


	// Soring the theRandomVariableList according to alphabetical order
	theDomain->sort(&theAllRandomVariableList);
	allRandomVariableCount = theAllRandomVariableList.count();

	//// create a list of random variables from physical parameters of all models of theModelSet

	//for (int i = 0; i < theAllModelList.count(); i++) {
	//	RModel *modelFromSet = theAllModelList[i];

	//	QList<RParameter *> paramList = modelFromSet->getPhysicalParameterList();
	//	for (int l = 0; l < paramList.count(); l++) {
	//		RRandomVariable *rv = qobject_cast<RRandomVariable *>(paramList[l]);
	//		if (rv) {
	//			if (!physicalRandomVariableList.contains(rv)) {
	//				physicalRandomVariableList << rv;
	//			}
	//		}
	//	}
	//}
	//// Soring the the list according to alphabetical order
	//physicalRandomVariableCount = physicalRandomVariableList.count();
	//theDomain->sort(&physicalRandomVariableList);
	////// This is almost never the case except if the user is dumb enough :) to give all constants to hazards
	////if (physicalRandomVariableCount == 0) {
	////	rCritical() << "No physical parameters to sample. There should be at least one random variable to be sampled for each hazard.";
	////	return -1;
	////}

	// Set the random variable list in theTransformer, which also intilizes the transfomer for a new analysis
	result = theTransformer.data()->setRandomVariableList(&theAllRandomVariableList);
	if (result < 0) {
		rCritical() << "Error: Could not set the list of random variables to the transformer.";
		return -1;
	}

	// Setting the list of random variables to the RandomNumberGenerator (In RandomNumberGenerator types which need a start point, this line also established the start point vector)
	result = theRandomNumberGenerator.data()->setRandomVariableList(&theAllRandomVariableList);
	if (result < 0) {
		rCritical() << "Error: Could not set the list of random variables to the random number generator.";
		return -1;
	}

	//gsl_rng *randomGenerator = theRandomNumberGenerator->getRandomGeneratorEngine();

	//// switch transformer's data sets and set newRandomVariableList as the new data set of the transformer
	//if (!physicalRandomVariableList.isEmpty()) {
	//	thePhysicalRandomVariablesTransformer = theTransformer.data()->createHelperTransformer();
	//	result = thePhysicalRandomVariablesTransformer.data()->setRandomVariableList(&physicalRandomVariableList);
	//	if (result < 0) {
	//		rCritical() << "Error: Could not set the list of random variables to the transformer.";
	//		return -1;
	//	}
	//}
	/*if (!physicalRandomVariableList.isEmpty()) {
	theTransformer.data()->switchOldAndNewDataSets();
	result = theTransformer.data()->setRandomVariableList(&physicalRandomVariableList);
	if (result < 0) {
	rCritical() << "Error: Could not set the list of random variables to the transformer.";
	return -1;
	}
	}*/

	// Backing up the currentValue of the random variables (now that theStartPoint vector is established in theRandomNumberGenerator, we can do this; otherwise, we would miss the value of storedValue by backing up.
	for (int i = 0; i < allRandomVariableCount; i++) {
		backupAllRandomVariablesValueList.append(theAllRandomVariableList[i]->getCurrentValue());
	}

	//// Backing up the currentValue of the all parameter list
	//for (int i = 0; i < theAllParameterList.count(); i++) {
	//	allParameterBackupList.append(theAllParameterList[i]->getCurrentValue());
	//}

	// Clear Plotting data members
	theTimePlotList.clear();
	theTimePlot.clear();

	theCumulativeTimePlotList.clear();
	theCumulativeTimePlot.clear();

	theCurveList.clear();
	theCumulativeCurveList.clear();

#ifndef NO_QCP
	// Plotting incremental
	if (plottingIsEnabled) {
		for (int i = 0; i < theFunctionList.count(); i++) {
			// Instatiating a time plot
			//RMainWindow *mainWindow = qobject_cast<RMainWindow *>(parent()->parent());
			theTimePlot = new RPlotWidget(theMainWindow);
			theMainWindow->addSubWindow(theTimePlot, "Discrete Event Sampling Incremental Time Plot", true);

			QList<QCPAxis*> axisrect;
			axisrect << theTimePlot.data()->xAxis << theTimePlot.data()->yAxis;

			theTimePlot.data()->axisRect()->setRangeZoomAxes(axisrect);
			theTimePlot->axisRect()->setRangeDragAxes(axisrect);
			theTimePlot->setInteraction(QCP::iRangeDrag, true); // Enable dragging
			theTimePlot->setInteraction(QCP::iRangeZoom, true); // Enable zoom by scrolling

			theTimePlot.data()->xAxis->setLabel("Time (Days)");
			theTimePlot.data()->yAxis->setLabel(theFunctionList[i].data()->objectName());

			theTimePlot.data()->legend->setVisible(true);

			theTimePlot.data()->addGraph(theTimePlot.data()->xAxis, theTimePlot.data()->yAxis);
			theTimePlot.data()->graph(0)->setAntialiased(true);
			theTimePlot.data()->graph(0)->setPen(QPen(Qt::GlobalColor(7 + i)));
			theCurveList << theTimePlot.data()->graph(0);
			theTimePlot.data()->graph(0)->setName(theFunctionList[i].data()->objectName());

			theTimePlotList.append(theTimePlot);
		}
	}

	// Plotting Cumulative
	if (plottingIsEnabled) {
		for (int i = 0; i < theFunctionList.count(); i++) {
			// Instatiating a time plot
			//RMainWindow *mainWindow = qobject_cast<RMainWindow *>(parent()->parent());
			theCumulativeTimePlot = new RPlotWidget(theMainWindow);
			theMainWindow->addSubWindow(theCumulativeTimePlot, "Discrete Event Sampling Cumulative Time Plot", true);

			QList<QCPAxis*> axisrect;
			axisrect << theCumulativeTimePlot.data()->xAxis << theCumulativeTimePlot.data()->yAxis;

			theCumulativeTimePlot.data()->axisRect()->setRangeZoomAxes(axisrect);
			theCumulativeTimePlot->axisRect()->setRangeDragAxes(axisrect);
			theCumulativeTimePlot->setInteraction(QCP::iRangeDrag, true); // Enable dragging
			theCumulativeTimePlot->setInteraction(QCP::iRangeZoom, true); // Enable zoom by scrolling

			theCumulativeTimePlot.data()->xAxis->setLabel("Time (Days)");
			theCumulativeTimePlot.data()->yAxis->setLabel(theFunctionList[i].data()->objectName());

			theCumulativeTimePlot.data()->legend->setVisible(true);

			theCumulativeTimePlot.data()->addGraph(theCumulativeTimePlot.data()->xAxis, theCumulativeTimePlot.data()->yAxis);
			theCumulativeTimePlot.data()->graph(0)->setAntialiased(true);
			theCumulativeTimePlot.data()->graph(0)->setPen(QPen(Qt::GlobalColor(7 + i)));
			theCumulativeCurveList << theCumulativeTimePlot.data()->graph(0);
			theCumulativeTimePlot.data()->graph(0)->setName(theFunctionList[i].data()->objectName());

			theCumulativeTimePlotList.append(theCumulativeTimePlot);
		}
	}


#endif // NO_QCP


	// doing main analysis
	void * inputArgs[] = { (void *)(&sampleCount) };
	int status = analysisParallelComputer->run(&RDiscreteEventSamplingAnalyzer::doAnalysisWrapper, this, inputArgs);

	QCoreApplication::processEvents();

	// checking error occurance
	if (status < 0)
		return status;

	theAccumulator.data()->terminateAccumulation();
	this->terminateAccumulation();




	QDateTime finishTime = QDateTime::currentDateTime();
	double numDays = startTime.daysTo(finishTime);
	double numMilliSecs = startTime.time().msecsTo(finishTime.time());
	double numSecs = numDays * 86400.0 + numMilliSecs / 1000.0;

	//test the recovery time
	/*for (int i = 0; i < recoveryTimeVector.count(); i++) {
	rDebug() << recoveryTimeVector[i];
	}*/
	QString showfinishtime = finishTime.toString();
	QString yearf = showfinishtime.section(' ',4,4);
	QString dayf = showfinishtime.section(' ',2,2);
	QString hourf = showfinishtime.section(' ', 3, 3);
	QString monthf = showfinishtime.section(' ', 1, 1);

	

	if (showfinishtime.contains("Jan", Qt::CaseInsensitive)) {
		monthf = "JANUARY";
		
	}
	else if (showfinishtime.contains("Feb", Qt::CaseInsensitive)) {
		monthf = "FEBRUARY";
		
	}
	else if (showfinishtime.contains("Mar", Qt::CaseInsensitive)) {
		monthf = "MARCH";
		
	}
	else if (showfinishtime.contains("Apr", Qt::CaseInsensitive)) {
		monthf = "APRIL";
		
	}
	else if (showfinishtime.contains("May", Qt::CaseInsensitive)) {
		monthf = "MAY";
		
	}
	else if (showfinishtime.contains("Jun", Qt::CaseInsensitive)) {
		monthf = "JUNE";
		
	}
	else if (showfinishtime.contains("Jul", Qt::CaseInsensitive)) {
		monthf = "JULY";
		
	}
	else if (showfinishtime.contains("Aug", Qt::CaseInsensitive)) {
		monthf = "AUGUST";
		
	}
	else if (showfinishtime.contains("Sep", Qt::CaseInsensitive)) {
		monthf = "SEPTEMBER";
		
	}
	else if (showfinishtime.contains("Oct", Qt::CaseInsensitive)) {
		monthf = "OCTOBER";
		
	}
	else if (showfinishtime.contains("Nov", Qt::CaseInsensitive)) {
		monthf = "NOVEMBER";
		
	}
	else if (showfinishtime.contains("Dec", Qt::CaseInsensitive)) {
		monthf = "DECEMBER";
		
	}
	


	rDebug() << "##### DISCRETE EVENT SAMPLING ANALYSIS DONE ON "+ monthf+" "+dayf+", "+yearf+" AT "+hourf; //salarifard
	rDebug() << "##### DISCRETE EVENT SAMPLING ANALYSIS DONE IN" << numSecs << "SECONDS.";
	rDebug() << "";
	rDebug() << "";

	// Restoring the currentValue of the random variables from the storedValue
	for (int i = 0; i < allRandomVariableCount; i++) {
		theAllRandomVariableList[i]->setCurrentValue(backupAllRandomVariablesValueList[i]);
	}

	//// Restoring the currentValue of the all parameter list
	//for (int i = 0; i < allParameterBackupList.count(); i++) {
	//	theAllParameterList[i]->setCurrentValue(allParameterBackupList[i]);
	//}

	theDomain->deleteObject("allInitialStateFunction");
	//theDomain->deleteObject("recoveryTimeFunction");

	// Restoring the time object to the initial time
	theTimeParameter.data()->setCurrentValue(initialTime);

	theCurveList.clear();
	theCumulativeCurveList.clear();


	return 0;
}

int RDiscreteEventSamplingAnalyzer::doAnalysisWrapper(void * theObject, void * inputsArgs[], int threadID) {
	// casting object



	RDiscreteEventSamplingAnalyzer * thisSampling = (RDiscreteEventSamplingAnalyzer *)(theObject);

	// casting input arguments
	int * generalSampleCount = (int *)inputsArgs[0];




	// calling the method
	return thisSampling->doAnalysis(generalSampleCount, threadID);

	return 0;
}

int RDiscreteEventSamplingAnalyzer::doAnalysis(int * generalSampleCount, int threadID) {

	int sampleCount;

	// Declaring vector u (Attention: Vector u should not be allocated here, it just gets the memory address of the genereted random numbers
	gsl_vector *u = 0;
	// Declaring vector x (Attention: Vector u SHOULD be allocated in this method, somewhere below)
	gsl_vector *x = 0;

	double hazardSumOfOccurenceRate;

	// We need a time object to be given to the dialog box that at each moment shows the current time and at the beginning, shows the date and time that corresponds to the begininng of the T

	double currentTime = initialTime;
	double previousTime = initialTime;


	int result;
	bool continueConditionInThread = true;

	gsl_rng *randomGenerator = theRandomNumberGenerator.data()->getRandomGeneratorEngine(threadID);

	theContinueFlagLock->lock();
	theContinueFlag = analysisParallelComputer->getContinueCondition() && continueConditionInThread;
	theContinueFlagLock->unlock();


	//******* START OF SIMULATION *******
	while (theContinueFlag) {

		//***** DATA MEMBERS FOR SIMULATION *****

		bool convergence = true;
		bool continueCondition = true;
		QMap <double, QVariantList> allEventMap;
		QMap <double, QVariantList> newEventMap;

		// data member that stores the data that should be given to the accumulator
		QVector<double> accumulatorSampleResultVector;

		QVector <double> recoveryTimeVector;

		QList<RPointer<RParameter> > allEventUpdatedParameterList;
		QList<RResponse *> allEventUpdatedResponseList;
		//QList<double> backupAllEventUpdatedParameterValueList;
		//QList<RResponse *> allEventUpstreamResponseList;

		// Vector that stores the finish times in each sample
		QVector<double> sampleFixedIntervalVector;

		// Vector list that stores sample results
		QVector<QList<double> > sampleResultFixedIntervalListVector;
		QVector<QList<double> > sampleCumulativeResultFixedIntervalListVector;

		QVector<QList<double> > sampleIncrementalResultListVector;
		QVector<QList<double> > sampleCumulativeResultListVector;


		QList<QVector<double> > sampleResultVectorList;
		QList<QVector<double> > sampleCumulativeResultVectorList;
		QList<QVector<double> > sampleResultFixedIntervalVectorList;
		QList<QVector<double> > sampleCumulativeResultFixedIntervalVectorList;

		sampleCountLock->lock();
		sampleCount = ++(*generalSampleCount);
		sampleCountLock->unlock();


		if (RDiscreteEventSamplingAnalyzer::getOutputDisplayLevel() != RDiscreteEventSamplingAnalyzer::None) {
			rDebug(threadID) << "SAMPLE" << sampleCount << "*********************************************************";
		}

		theTimeParameter.data()->setCurrentValue(initialTime, threadID);

		//foreach(RModel *modelFromSet, theAllModelList) {
		//	modelFromSet->resetHistoryVariables(threadID);
		//}

		//This method is called at the start of each sample. For some models, it has specific implementation
		for (int i = 0; i < theAllModelList.count(); i++) {
			RModel *tempModel = theAllModelList[i];
			tempModel->resetHistoryVariables(threadID);
		}

		//allEventMap.clear();
		//allEventUpstreamResponseList.clear();
		//sampleIncrementalResultListVector.clear();
		//sampleCumulativeResultListVector.clear();
		//sampleResultFixedIntervalListVector.clear();
		//sampleCumulativeResultFixedIntervalListVector.clear();
		//sampleFixedIntervalVector.clear();
		//currentTime = 0;
		//accumulatorSampleResultVector.clear();

		//****** START OF EACH SAMPLE ******

		//***** START OF INITIAL SYSTEM EVALUATION PROCESS *****

		// *** START OF DETERMINING WHICH HAZARD SHOULD BE "ON" ***

		//Generate a random number between 0 and 1 to determine which hazard is on, based on their rate of occurence
		double hazardRandomNumber;

		hazardRandomNumber = gsl_ran_flat(randomGenerator, 0, 1);

		// Creating a list of booleans that tracks which hazards are on and which are off
		QList<bool> onList;
		for (int i = 0; i < hazardModelList.count(); i++) {
			onList << false;
		}


		//hazardNormalizedOccurenceRateList << 0.2 << 0.4 << 0.1 << 0.3;
		QList<double> hazardCumulativeNormalizedOccurenceRateList;
		hazardCumulativeNormalizedOccurenceRateList = hazardNormalizedOccurenceRateList;

		for (int i = 1; i < hazardNormalizedOccurenceRateList.count(); i++) {
			hazardCumulativeNormalizedOccurenceRateList[i] += hazardCumulativeNormalizedOccurenceRateList[i - 1];
		}
		hazardCumulativeNormalizedOccurenceRateList.prepend(0);

		int onHazardIndex;
		for (int i = 0; i < hazardNormalizedOccurenceRateList.count(); i++) {
			if ((hazardCumulativeNormalizedOccurenceRateList[i]<hazardRandomNumber) && (hazardCumulativeNormalizedOccurenceRateList[i + 1] >= hazardRandomNumber)) {
				onHazardIndex = i;
				break;
			}
		}

		onList[onHazardIndex] = true;
		//rDebug(threadID) << "";
		//rDebug(threadID) << hazardModelList[onHazardIndex]->objectName();
		// *** END OF DETERMINING WHICH HAZARD SHOULD BE "ON" ***


		QList<RRandomVariable *> currentRandomVariableList = theAllRandomVariableList;
		int currentRandomVariableCount = allRandomVariableCount;

		result = theRandomNumberGenerator.data()->generateNumbers(currentRandomVariableCount, threadID);
		if (result < 0) {
			rCritical(threadID) << "Error: Could not generate random numbers for sampling analysis.";
			return -1;
		}

		// Getting the vector of generated random numbers
		u = theRandomNumberGenerator.data()->getGeneratedNumbers(threadID);

		// Syntax to keep the GUI responsive
		if (threadID == 0) {
			QCoreApplication::processEvents();
		}

		// Allocating vector x
		if (x) {
			gsl_vector_free(x);
			x = 0;
		}
		x = gsl_vector_calloc(currentRandomVariableCount);

		// Transform starting point into original space
		result = theTransformer.data()->transformPointToOriginalSpace(u, x, threadID);
		if (result < 0) {
			rCritical(threadID) << "Error: Could not transform from standard-normal space to original space.";
			return -1;
		}

		// Update the current value of random variables to the new realization
		for (int j = 0; j < currentRandomVariableCount; j++) {

			double valu = gsl_vector_get(u, j);

			double valx = gsl_vector_get(x, j);
			currentRandomVariableList[j]->setCurrentValue(valx, threadID);
		}

		allInitialStateFunction->updateAnalysisStatus(threadID);


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
				}
			}

			//// Setting the isAnalyzed flag of the models associated with the hazards other than those that has just become on to true, so that they are not run
			//if (!swichedOnHazardsIndexVector.contains(j)) {
			//	// ATTENTION: THIS IMPLEMENTATION ASSUMES THAT NONE OF THESE "ASSOCIATED" MODELS ARE SHARED BY TWO OR MORE HAZARDS
			//	QList<RModel *> modelList = hazardModelList[j]->getAssociatedModels();
			//	for (int k = 0; k < modelList.count(); k++) {
			//		modelList[k]->setResponseAnalyzed(true);
			//	}
			//}
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
			QList<RResponse *> responseList = unusedAssociatedModelList[j]->findChildren<RResponse *>();
			for (int k = 0; k < responseList.count(); k++) {
				responseList[k]->setCurrentValue(0, threadID);
			}
		}

		// *** evaluate the initial function ***

		result = allInitialStateFunction->evaluateFunction(false, RObject::RandomVariable, threadID);

		//Issue an error if an event model has not been input in the analyzer while it is needed to model the recovery process ---- e.g. we have power stations in the recovery functions but we do not have powerEventModel in the event models
		for (int n = 0; n < unusedEventModelList.count(); n++) {
			if (!unusedEventModelList[n]->isAnalyzed(threadID)) {
				rCritical(threadID) << "Error:" << unusedEventModelList[n]->objectName() << "should be input as an event model in the analyzer";
				return -1;
			}
		}



		//*** CREATE THE ALL EVENT MAP AFTER EVALUATING THE INITIAL SYSTEM EVALUATION ***
		//loop on the all event models and call the method scheduleEvents for them
		for (int n = 0; n < eventModelList.count(); n++) {
			//if (!eventModelList[n]->isAnalyzed()) {
			result = eventModelList[n]->ScheduleEvents(currentTime, threadID);
			if (result < 0) {
				rCritical(threadID) << "Error:" << eventModelList[n]->objectName() << "Can not schedule events";
				return -1;
			}
			//}
		}

		//loop on the all event models and get the list events they have made and insert them in the allEventMap
		for (int m = 0; m < eventModelList.count(); m++) {
			//if (!eventModelList[m]->isAnalyzed()) {
			QList< QList<double> > thisModelEventTimeListList = eventModelList[m]->getNewEventTimeListList(threadID);
			for (int n = 0; n < thisModelEventTimeListList.count(); n++) {
				double thisModelFinishTime = thisModelEventTimeListList[n][0];
				double thisModelStartTime = thisModelEventTimeListList[n][1];
				QVariantList thisModelVariantList;

				thisModelVariantList << (thisModelStartTime);
				thisModelVariantList << (QVariant::fromValue<QObject *>(eventModelList[m]));

				allEventMap.insertMulti(thisModelFinishTime, thisModelVariantList);
			}
			//}
		}



		//***** END OF INITIAL SYSTEM EVALUATION PROCESS *****

		QList<RResponse *> copyAllFunctionResponseList = allFunctionResponseSet.toList();
		//for (int j = 0; j < copyAllFunctionResponseList.count(); j++) {
		//	copyAllFunctionResponseList[j]->backup(threadID);
		//}


		//***** START OF RECOVERY PROCESS *****

		//After determining the post disaster status of the system, it is now to simulate the recovery process using recovery functions

		//QVector is a better choice in cases that do lots of appending or inserting, since QVector allocates memory for its items in a single heap allocation.
		QVector<double> sampleUniqueFinishTimeVector = allEventMap.uniqueKeys().toVector();
		if (!sampleUniqueFinishTimeVector.isEmpty()) {
			if (sampleUniqueFinishTimeVector[0] != initialTime) {
				sampleUniqueFinishTimeVector.prepend(initialTime);
			}
		}

		//Evaluate the post disaster status for each recovery function
		QList<double> postDisasterFunctionValueList;
		for (int f = 0; f < theFunctionList.count(); f++) {
			RFunction *recoveryFunction = theFunctionList[f].data();

			result = recoveryFunction->evaluateFunction(false, RObject::RandomVariable, threadID);
			double funValue = 0;
			if (result < 0) {
				rCritical(threadID) << "WARNING: Could not evaluate the function" << recoveryFunction->objectName();
				if (result < -1) {
					// This means that for non-convergence, the function could not be evaluated. So it is assumed that the system has failed. Thus, the function value is set to some number less than Zero, e.g. -1.0.
					funValue = -1.0;
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
					return -1;
				}
			}

			funValue = recoveryFunction->getFunctionValue(threadID);
			postDisasterFunctionValueList << funValue;
		}

		QList<RPointer<RParameter> > currentEventUpdatedParameterList;
		QList<RResponse *> currentEventUpdatedResponseList;

		QList<RModel *> currentEventUpstreamModelList;
		QList<RModel *> currentEventDownstreamModelList;

		QList<RResponse *> currentEventUpstreamAndOutputResponseList;
		QList<RResponse *> currentEventDownstreamResponseList;
		QList<RResponse *> currentEventAllResponseList;

		QList<QVariantList> currentEventVariantListList;
		QList<double> currentEventRecoveryFunctionValueList;
		QList<double> currentEventStartTimeList;

		QList<RResponse *> futureResponseList = copyAllFunctionResponseList;
		QList<RResponse *> currentAndPastEventsUpstreamAndOutputResponseList;
		QList<RResponse *> currentAndPastEventsDownstreamResponseList;
		QList<RResponse *> currentAndPastEventsAllResponseList;
		QList<RModel *> currentAndPastEventsDownstreamModelList;

		// the objects that are needed for output
		QList<QString> currentEventOutputList;


		//**** START OF ITERATING ON FINISH TIMES ****
		for (int i = 0; i < sampleUniqueFinishTimeVector.count(); i++) {

			// Syntax to keep the GUI responsive
			if (threadID == 0) {
				QCoreApplication::processEvents();
			}

			currentTime = sampleUniqueFinishTimeVector[i];
			theTimeParameter.data()->setCurrentValue(currentTime, threadID);
			if (i != 0) {
				previousTime = sampleUniqueFinishTimeVector[i - 1];
			}


			//Clear the list at each step of finish time
			newEventMap.clear();

			currentEventVariantListList.clear();
			currentEventUpdatedParameterList.clear();
			currentEventUpdatedResponseList.clear();
			currentEventVariantListList = allEventMap.values(currentTime);

			currentEventUpstreamModelList.clear();
			currentEventDownstreamModelList.clear();

			currentEventUpstreamAndOutputResponseList.clear();
			currentEventDownstreamResponseList.clear();

			currentEventRecoveryFunctionValueList.clear();
			currentEventStartTimeList.clear();

			currentEventOutputList.clear();

			//Creating the output text
			QString outputText4 = "Functions Cumulative Values: ";

			//Get the updated parameter list from the event models which have scheduled events at current time
			for (int j = 0; j < currentEventVariantListList.count(); j++) {
				//int a = currentEventVariantListList.count();
				QVariantList currentEventVariantList = currentEventVariantListList[j];
				double currentEventStartTime = currentEventVariantList[0].toDouble();

				if (!currentEventStartTimeList.contains(currentEventStartTime)) {
					currentEventStartTimeList << currentEventStartTime;
				}
				QObject *currentEventModelObject = qvariant_cast<QObject *>(currentEventVariantList[1]);
				REventModel *currentEventModel = qobject_cast<REventModel *>(currentEventModelObject);

				currentEventModel->UpdateOperationStatus(currentTime, threadID);
				QList<RPointer<RParameter> > updatedParameterList = currentEventModel->getUpdatedParamterList(currentTime, threadID);

				for (int k = 0; k < updatedParameterList.count(); k++) {
					if (!currentEventUpdatedParameterList.contains(updatedParameterList[k])) {
						currentEventUpdatedParameterList << updatedParameterList[k];
					}
				}
			}

			//Strings to manage the output text
			QString outputText1 /*= "Finish Time: "*/;
			QString outputText2 = "Start Times:";
			QString outputText3 = "Updated Parameters: ";

			double txt = currentTime - initialTime;
			QString str = QString("Finish Time: %1").arg(txt, 0, 'f', 2);
			outputText1.append(str);
			//outputText1.append(QString::number(currentTime - initialTime));
			outputText1.append("| ");

			for (int k = 0; k < currentEventStartTimeList.count(); k++) {
				double startTxt = currentEventStartTimeList[k] - initialTime;
				str = QString(" %1").arg(startTxt, 0, 'f', 2);
				outputText2.append(str);

				if (k != currentEventStartTimeList.count() - 1) {
					outputText2.append(", ");
				}
			}

			if (currentEventStartTimeList.count() == 0) {
				double startTxt = 0.0;
				str = QString(" %1").arg(startTxt, 0, 'f', 2);
				outputText2.append(str);
			}

			outputText2.append("| ");

			for (int k = 0; k < currentEventUpdatedParameterList.count(); k++) {
				outputText3.append(currentEventUpdatedParameterList[k].data()->objectName());
				if (k != currentEventUpdatedParameterList.count() - 1) {
					outputText3.append(", ");
				}
				else {
				}
			}
			outputText3.append("| ");

			switch (RDiscreteEventSamplingAnalyzer::getOutputDisplayLevel())
			{
			case (RDiscreteEventSamplingAnalyzer::Maximum):
				outputText1.append(outputText2);
				outputText1.append(outputText3);
				break;
			case (RDiscreteEventSamplingAnalyzer::Moderate):
				outputText1.append(outputText3);
				break;
			case(RDiscreteEventSamplingAnalyzer::Minimum):
				break;
			}

			currentEventOutputList << outputText1;



			allEventUpdatedParameterList.append(currentEventUpdatedParameterList);

			/*backupAllEventUpdatedParameterValueList.clear();
			for (int t = 0; t < allEventUpdatedParameterList.count(); t++) {
			backupAllEventUpdatedParameterValueList << allEventUpdatedParameterList[t]->getCurrentValue();
			}*/


			////test the scheduling - - - -  - - - - OK
			//if (!currentEventUpdatedParameterList.isEmpty()) {
			//	rDebug() << currentTime << currentEventUpdatedParameterList[0]->objectName();
			//}

			for (int l = 0; l < currentEventUpdatedParameterList.count(); l++) {
				RParameter *currentParam = currentEventUpdatedParameterList[l].data();
				if (qobject_cast <RResponse *> (currentParam)) {
					RResponse *currentResponse = qobject_cast <RResponse *> (currentParam);
					currentEventUpdatedResponseList << currentResponse;
				}
			}

			allEventUpdatedResponseList.append(currentEventUpdatedResponseList);


			////test RModel::getResponseList() - - - - CHECKED - - - - - it only return the immediate respone list
			//QObject *obj = futureResponseList[0]->getModel();
			//RModel *model = qobject_cast<RModel *> (obj);
			//QList<RResponse *> testResponseList = model->getResponseList();
			//for (int t = 0; t < testResponseList.count(); t++) {
			//	QString a = testResponseList[0]->objectName();
			//}


			////Test RFunction::getResponseList() - -  - - -  CHECKED - - - -  It works and returns all of the upstream models
			//RResponse *res = futureResponseList[0];
			//RFunction *testFun = new RFunction(theDomain, "testFun");
			//testFun->setExpression(res->objectName());
			//testFun->populateLists();
			//QList<RResponse *> testResponseList = testFun->getResponseList();
			//for (int t = 0; t < testResponseList.count(); t++) {
			//	QString a = testResponseList[0]->objectName();
			//}
			//theDomain->deleteObject("testFun");


			//*** For past and current event setAnalyzed to true and for future events setCurrentValue to 0 ***

			//** Create the upstream response list of the recently updated responses due the occurrence of current event **
			for (int l = 0; l < currentEventUpdatedResponseList.count(); l++) {
				RResponse *currentResponse = currentEventUpdatedResponseList[l];

				QObject *modelObject = currentResponse->getModel();
				RModel *currentModel = qobject_cast<RModel *> (modelObject);

				QList<RResponse *> upstreamResponseList = currentModel->getUpstreamResponseList();
				QList<RResponse *> outputResponseList = currentModel->findChildren<RResponse *>();

				currentEventUpstreamAndOutputResponseList << upstreamResponseList << outputResponseList;
			}
			//Delete the duplicate responses
			QSet<RResponse *> currentEventUpstreamAndOutputResponseSet = currentEventUpstreamAndOutputResponseList.toSet();
			currentEventUpstreamAndOutputResponseList = currentEventUpstreamAndOutputResponseSet.toList();
			currentAndPastEventsUpstreamAndOutputResponseList << currentEventUpstreamAndOutputResponseList;

			
			////////////////////// This part of code was used when we wanted to track  values other than costs, for example damage ratios, repair times, ...
			////Create the down stream model list of the recently updated responses
			//QList<RModel *> dependentModelList;
			//for (int l = 0; l < currentEventUpdatedParameterList.count(); l++) {
			//	RParameter *currentParam = currentEventUpdatedParameterList[l].data();
			//	if (!theDependencyHash.contains(currentParam)) {
			//		evaluateDependencyLock->lock();
			//		evaluateDependencies(currentParam);
			//		evaluateDependencyLock->unlock();
			//	}
			//	QList <RModel *> modelList = theDependencyHash[currentParam];
			//	for (int r = 0; r < modelList.count(); r++) {
			//		dependentModelList << modelList[r];
			//	}
			//}

			//for (int k = 0; k < dependentModelList.count(); k++) {
			//	REventModel *eventModel = qobject_cast<REventModel *> (dependentModelList[k]);
			//	if (!eventModel) {
			//		currentEventDownstreamModelList << dependentModelList[k];
			//	}
			//}
			//currentAndPastEventsDownstreamModelList << currentEventDownstreamModelList;

			////** Create the down stream response list of the recently updated responses **
			//for (int k = 0; k < currentEventDownstreamModelList.count(); k++) {
			//	RModel *currentModel = currentEventDownstreamModelList[k];
			//	QList<RResponse *> outputResponseList = currentModel->findChildren<RResponse *>();

			//	currentEventDownstreamResponseList << outputResponseList;
			//}
			//currentAndPastEventsDownstreamResponseList << currentEventDownstreamResponseList;

			//currentEventAllResponseList = currentEventDownstreamResponseList + currentEventUpstreamAndOutputResponseList;
			//currentAndPastEventsAllResponseList = currentAndPastEventsDownstreamResponseList + currentAndPastEventsUpstreamAndOutputResponseList;
			//////////////////


			for (int t = 0; t < currentEventUpstreamAndOutputResponseList.count(); t++) {
				currentEventUpstreamAndOutputResponseList[t]->setAnalyzed(true, threadID);
			}

			/*for (int k = 0; k < currentEventUpstreamAndOutputResponseList.count(); k++) {
			RResponse *response = currentEventUpstreamAndOutputResponseList[k];
			futureResponseList.removeOne(response);
			}*/

			/*for (int t = 0; t < currentAndPastEventsUpstreamAndOutputResponseList.count(); t++) {
			currentAndPastEventsUpstreamAndOutputResponseList[t]->setAnalyzed(true);
			}*/

			////////////////////// This part of code was used when we wanted to track  values other than costs, for example damage ratios, repair times, ...
			//for (int k = 0; k < currentEventAllResponseList.count(); k++) {
			//	futureResponseList.removeOne(currentEventAllResponseList[k]);
			//}
			//for (int k = 0; k < futureResponseList.count(); k++) {
			//	futureResponseList[k]->setAnalyzed(true, threadID);
			//	futureResponseList[k]->setCurrentValue(0, threadID);
			//}
			//////////////////////


			//*** Turn on all the models and responses that are related to this event ***
			updateAnalysisStatus(currentEventUpdatedParameterList, threadID);

			////Since some responses may not be in the function but should be evaluated, it is needed that we evaluate the all initial state function - Not Needed anymore because of new way of function evaluation (only for costs)
			//allInitialStateFunction->evaluateFunction(false, RObject::RandomVariable, threadID);

			// *** Start of Iterating on recovery functions to evaluate them ***	
			for (int j = 0; j < theFunctionList.count(); j++) {
				RFunction *recoveryFunction = theFunctionList[j].data();
				result = recoveryFunction->evaluateFunction(false, RObject::RandomVariable, threadID);

				double recoveryFunctionValue;

				recoveryFunctionValue = recoveryFunction->getFunctionValue(threadID);

				currentEventRecoveryFunctionValueList << recoveryFunctionValue;

				//Output text
				outputText4.append(theFunctionList[j].data()->objectName());
				outputText4.append("= ");
				outputText4.append(QString::number(recoveryFunctionValue));

				if (j != theFunctionList.count() - 1) {
					outputText4.append(", ");
				}
			}

			//Output text
			for (int q = 0; q < currentEventOutputList.count(); q++) {
				currentEventOutputList[q].append(outputText4);
			}
			if (RDiscreteEventSamplingAnalyzer::getOutputDisplayLevel() != RDiscreteEventSamplingAnalyzer::None) {
				for (int r = 0; r < currentEventOutputList.count(); r++) {
					rDebug(threadID) << currentEventOutputList[r];
				}
			}


			// *** End of Iterating on recovery functions to evaluate them ***

			////////////////////// This part of code was used when we wanted to track  values other than costs, for example damage ratios, repair times, ...
			//for (int k = 0; k < currentEventAllResponseList.count(); k++) {
			//	int index = copyAllFunctionResponseList.indexOf(currentEventAllResponseList[k]);
			//	copyAllFunctionResponseList.removeAt(index);
			//}
			//for (int l = 0; l < copyAllFunctionResponseList.count(); l++) {
			//	copyAllFunctionResponseList[l]->restore(threadID);
			//}
			//////////////////////

			//Create the sampleCumulativeResultListVector for this event after evaluation of all recovery functions
			sampleCumulativeResultListVector.append(currentEventRecoveryFunctionValueList);

			QVector<double> zero(theFunctionList.count());
			sampleIncrementalResultListVector << zero.toList();

			/*if (i!=0) {
			for (int k = 0; k < theFunctionList.count(); k++) {
			sampleCumulativeResultListVector[i][k] = sampleCumulativeResultListVector[i-1][k] + sampleIncrementalResultListVector[i][k];
			}
			}*/

			if (i == 0) {
				for (int k = 0; k < theFunctionList.count(); k++) {
					sampleIncrementalResultListVector[0][k] = sampleCumulativeResultListVector[0][k];
				}
			}
			else {
				for (int k = 0; k < theFunctionList.count(); k++) {
					sampleIncrementalResultListVector[i][k] = sampleCumulativeResultListVector[i][k] - sampleCumulativeResultListVector[i - 1][k];
				}
			}

			//loop on the recently become false event models and call the method scheduleEvents for them
			for (int n = 0; n < eventModelList.count(); n++) {
				if (!eventModelList[n]->isAnalyzed(threadID)) {
					result = eventModelList[n]->ScheduleEvents(currentTime, threadID);
					if (result < 0) {
						rCritical(threadID) << "Error:" << eventModelList[n]->objectName() << "Can not schedule events";
						return -1;
					}
				}
			}

			//loop on the recently become false event models and get the list events they have made and insert them in the allEventMap
			for (int m = 0; m < eventModelList.count(); m++) {
				if (!eventModelList[m]->isAnalyzed(threadID)) {
					QList< QList<double> > thisModelEventTimeListList = eventModelList[m]->getNewEventTimeListList(threadID);
					for (int n = 0; n < thisModelEventTimeListList.count(); n++) {
						double thisModelFinishTime = thisModelEventTimeListList[n][0];
						double thisModelStartTime = thisModelEventTimeListList[n][1];
						QVariantList thisModelVariantList;
						thisModelVariantList << (thisModelStartTime);
						thisModelVariantList << (QVariant::fromValue<QObject *>(eventModelList[m]));
						//allEventMap.contains()
						allEventMap.insertMulti(thisModelFinishTime, thisModelVariantList);
						newEventMap.insertMulti(thisModelFinishTime, thisModelVariantList);
					}
				}
			}

			//Update the unique finish time vector based on the new events which have been just made by the event models
			sampleUniqueFinishTimeVector = allEventMap.uniqueKeys().toVector();
			if (!sampleUniqueFinishTimeVector.isEmpty()) {
				if (sampleUniqueFinishTimeVector[0] != initialTime) {
					sampleUniqueFinishTimeVector.prepend(initialTime);
				}
			}
		}
		//**** END OF ITERATING ON FINISH TIMES ****

		//Update the plot after the recovery process has finished
		if (threadID == 0)
			updateSamplePlot(sampleCount, sampleUniqueFinishTimeVector, sampleIncrementalResultListVector, sampleCumulativeResultListVector);


		//*** Creating the result map based on the cost estimation interval which the user has provided ***
		double FunctionEstimationInterval = theFunctionEstimationInterval;
		double sampleRecoveryTime = 0;

		//QList<double> a = sampleIncrementalResultListVector[0];


		if (!sampleUniqueFinishTimeVector.isEmpty()) {
			sampleRecoveryTime = sampleUniqueFinishTimeVector.last() - initialTime;
			recoveryTimeVector << sampleRecoveryTime;
		}
		//Determine the index for the last time based on the cost estimation interval
		double lastInterval = ceil(sampleRecoveryTime / FunctionEstimationInterval);

		for (int j = 0; j <= lastInterval; j++) {
			sampleFixedIntervalVector << (initialTime + j * FunctionEstimationInterval);
		}

		//Initiate the sampleResultFixedIntervalListVector
		QVector <double> zeroVector(theFunctionList.count());

		for (int j = 0; j <= lastInterval; j++) {
			sampleResultFixedIntervalListVector << zeroVector.toList();
			sampleCumulativeResultFixedIntervalListVector << zeroVector.toList();
		}

		//Create the sampleResultFixedIntervalListVector based on the cost estimation interval
		for (int j = 0; j < sampleUniqueFinishTimeVector.count(); j++) {
			int nextInterval = ceil((sampleUniqueFinishTimeVector[j] - initialTime) / FunctionEstimationInterval);
			for (int k = 0; k < theFunctionList.count(); k++) {
				sampleResultFixedIntervalListVector[nextInterval][k] += sampleIncrementalResultListVector[j][k];
			}
		}

		//Create the sampleCumulativeResultFixedIntervalListVector based on the cost estimation interval
		for (int j = 0; j < theFunctionList.count(); j++) {
			sampleCumulativeResultFixedIntervalListVector[0][j] = sampleResultFixedIntervalListVector[0][j];
		}

		for (int i = 1; i < sampleFixedIntervalVector.count(); i++) {
			for (int j = 0; j < theFunctionList.count(); j++) {
				sampleCumulativeResultFixedIntervalListVector[i][j] = sampleCumulativeResultFixedIntervalListVector[i - 1][j] + sampleResultFixedIntervalListVector[i][j];
			}
		}

		accumulatorSampleResultVector = sampleCumulativeResultFixedIntervalListVector.last().toVector();
		//accumulatorSampleResultVector << sampleRecoveryTime;
		//*** End of Creating the result map based on the cost estimation interval which the user has provided ***


		//***** END OF RECOVERY PROCESS *****

		// *** Accumulate the sampleCumulativeResultFixedIntervalListVector.last()

		QVector<double> sampleResult(theFunctionList.count());

		if (convergence) {
			result = theAccumulator.data()->accumulate(accumulatorSampleResultVector, u, sampleResult, sampleResult, threadID);
			//result = theHistogramAccumulator->accumulate(accumulatorSampleResultVector, u, sampleResult, sampleResult, threadID);
			if (result < 0) {
				rCritical(threadID) << "Error: Could not accumulate the results of this sample.";
				return -1;
			}
			theMaximumIterations = theAccumulator.data()->getMaximumIterations();
			// Check for continuing condition from the accumulator
			continueConditionInThread = theAccumulator.data()->getContinueFlag();

		}


		//initialize the inside accumulator in the first sample (similar to setFunctionList()  method in acumulator)
		accumulatorLock->lock();
		if (doAccumulate) {

			if (theIterationIndex == 0) {
				//theMaxTimeSizeLock->lock();
				theMaxTimeSize = sampleFixedIntervalVector.count();
				//theMaxTimeSizeLock->unlock();

				for (int k = 0; k < sampleFixedIntervalVector.count(); k++) {
					maxFixedIntervalVector << sampleFixedIntervalVector[k] - initialTime;
				}
				this->initializeAccumulator();
			}

			//If the time size of this event is less or more that the max time size of all previous events, this method normalize the size
			//modifyTimeSizeLock->lock();
			if (theMaxTimeSize != sampleFixedIntervalVector.count()) {
				this->modifyTimeSize(sampleFixedIntervalVector, &sampleResultFixedIntervalListVector, &sampleCumulativeResultFixedIntervalListVector);
			}
			//modifyTimeSizeLock->unlock();

			// Incrementing theIterationIndex
			//theIterationIndexLock->lock();
			theIterationIndex++;
			//theIterationIndexLock->unlock();

			this->accumulate(sampleResultFixedIntervalListVector, sampleCumulativeResultFixedIntervalListVector, threadID);

		}
		accumulatorLock->unlock();

		if (threadID == 0) {
			QCoreApplication::processEvents();
		}

		//****** END OF EACH SAMPLE ******
		if (RDiscreteEventSamplingAnalyzer::getOutputDisplayLevel() != RDiscreteEventSamplingAnalyzer::None) {
			rDebug(threadID) << "";
		}

		theContinueFlagLock->lock();
		theContinueFlag = analysisParallelComputer->getContinueCondition() && continueConditionInThread;
		theContinueFlagLock->unlock();

	}
	//******* END OF SIMULATION *******

	return endAnalysis(x/*, dfdMean_1_f, dfdStdv_1_f*/, 0);
	//return 0;
}

int RDiscreteEventSamplingAnalyzer::endAnalysis(gsl_vector * x, int errorCode) {

	if (x != 0)
		gsl_vector_free(x);

	return errorCode;
}

void RDiscreteEventSamplingAnalyzer::initializeForParallelComputation(int threadID) {

	// initializing transformer
	theTransformer.data()->initializeForParallelComputation(threadID);
	//if (thePhysicalRandomVariablesTransformer) {
	//	thePhysicalRandomVariablesTransformer.data()->initializeForParallelComputation(threadID);
	//}
	// initializing the allInitialStateFunction that contains all of the parameters and responses 
	allInitialStateFunction->initializeForParallelComputation(threadID);

}

void RDiscreteEventSamplingAnalyzer::resetAfterParallelComputation() {

	// initializing all the contributed functions
	//foreach(RPointer<RFunction> contributedFunction, theFunctionList) {
	//	contributedFunction.data()->resetAfterParallelComputation();
	//}

	for (int i = 0; i < theFunctionList.count(); i++) {
		RFunction *contributedFunction = theFunctionList[i].data();
		contributedFunction->resetAfterParallelComputation();
	}

}

bool RDiscreteEventSamplingAnalyzer::canBeRunParallelly() {

	// checking properties of type QObject*
	if (!canAllQObjectStarPropertiesBeRunParallelly()) {
		rDebug() << "Hence, the Discrete Event Sampling Analyzer cannot be run using parallel processing.";
		return false;
	}

	// checking contributed functions
	//foreach(RPointer<RFunction> contributedFunction, theFunctionList) {
	//	if (!contributedFunction.data()->canBeRunParallelly()) {
	//		return false;
	//	}
	//}

	for (int i = 0; i < theFunctionList.count(); i++) {
		RFunction *contributedFunction = theFunctionList[i].data();
		if (!contributedFunction->canBeRunParallelly()) {
			rDebug() << "Hence, the Discrete Event Sampling Analyzer cannot be run using parallel processing.";
			return false;
		}
	}

	return true;
}

void RDiscreteEventSamplingAnalyzer::evaluateDependencies(RParameter *passedParameter)
{

	QList<RModel *> dependentModelList;

	// Estabishing a hash of RModels called "remainingModelList" and anserting all the models of theBackupModelList to it. The reason why backup lists are used here is explained where the backup lists are declated in the .h file.
	// The backup list is used, for example, to create QMaps for efficient finite difefrence analysis. The reason that the original lists are not used is that those lists may be modified in some of the analysis types, e.g., load coincidence. So, the backup lists should be used.
	QHash<RModel*, bool> remainingModelHash;
	int remainingModelsCount = backupAllModelList.size();

	//foreach(RModel *model, backupAllModelList) {
	//	remainingModelHash.insert(model, true);
	//}

	for (int i = 0; i < backupAllModelList.count(); i++) {
		RModel *model = theAllModelList[i];
		remainingModelHash.insert(model, true);
	}


	QHash<RModel *, bool>::Iterator nextRemainingModelIterator;

	// Searching for next model among the models in the "remainingModelList" by iterating on them
	for (nextRemainingModelIterator = remainingModelHash.begin(); nextRemainingModelIterator != remainingModelHash.end(); ++nextRemainingModelIterator) {

		// A line to keep the GUI responsive
		QCoreApplication::processEvents();

		// if model has been proven to be dependant, skip checking it
		if (!nextRemainingModelIterator.value()) {
			continue;
		}

		// Getting the pointer to this model. The pointer is named "modelObject".
		RModel *modelObject = nextRemainingModelIterator.key();

		// If the "modelObject" CONTAINS the "passedParameter" (that is, if the "passedParameter" is an input to "modelObject"), then this model and all of its outputs (reponses) along with the models downstream are affrected by the change in current value of this parameter.
		if (modelObject->contains(passedParameter)) {

			// setting the flag of the model in remainingModelHash to false, as it is proven to be dependant
			remainingModelHash[modelObject] = false;
			remainingModelsCount--;

			//// Setting the isAnalyzed flag of all the output responses of modelObject to false
			//modelObject->setResponseAnalyzed(false);
			dependentModelList.append(modelObject);

			// If the remainingModelList is empty, then the search is done:
			if (remainingModelsCount == 0) {
				break;
			}

			// Adding the output RResponses of the "modelObject" (which are its children) to the "searchList", so that they will be considered for flag updating of the models for which they are input.
			// Getting the list of the output RResponses on the "modelObject"
			QList<RResponse *> tempResponseList = modelObject->findChildren<RResponse *>();
			// Iterrating on the list
			for (int k = 0; k < tempResponseList.count(); k++) {
				// If the dependency map of the tempResponseList[k] is not establish, then estabish its dependency map first (recursive call)
				//evaluateDependencyLock->lock();
				if (!theDependencyHash.contains(tempResponseList[k])) {
					evaluateDependencies(tempResponseList[k]);
				}
				//evaluateDependencyLock->unlock();

				// Get the list of modelsthat are affected by tempResponseList[k]
				QList<RModel *> tempModelList = theDependencyHash[tempResponseList[k]];
				for (int l = 0; l < tempModelList.count(); l++) {

					// setting the flag of the model in remainingModelHash to false if it is true (it hasn't been added to theDependantModelList)
					if (remainingModelHash[tempModelList[l]]) {
						remainingModelHash[tempModelList[l]] = false;
						remainingModelsCount--;

						// Appned the tempModelList to the dependentModelList of the parameter under consideration
						dependentModelList.append(tempModelList[l]);

						// If the remainingModelList is empty, then the search is done:
						if (remainingModelsCount == 0) {
							break;
						}
					}
				}
			}
		}
	}
	//Change the dependentModelList to set to remove duplicate models. Then change it back to list
	dependentModelList.toSet().toList();
	theDependencyHash[passedParameter] = dependentModelList;
}

void RDiscreteEventSamplingAnalyzer::updateAnalysisStatus(RParameter *passedParameter, int threadID)
{
	QList<RModel *> dependentModelList;

	evaluateDependencyLock->lock();
	if (!theDependencyHash.contains(passedParameter)) {
		evaluateDependencies(passedParameter);
	}
	evaluateDependencyLock->unlock();

	dependentModelList = theDependencyHash[passedParameter];

	for (int j = 0; j < dependentModelList.count(); j++) {
		if (theAllModelList.contains(dependentModelList[j])) {
			dependentModelList[j]->setResponseAnalyzed(false, threadID);
		}
	}
}

void RDiscreteEventSamplingAnalyzer::updateAnalysisStatus(QList<RPointer<RParameter> > passedParameterList, int threadID)
{
	for (int i = 0; i < passedParameterList.count(); i++) {
		updateAnalysisStatus(passedParameterList[i].data(), threadID);
	}
}

void RDiscreteEventSamplingAnalyzer::resetAccumulator()
{
	squaredSumIncrementalListVector.clear();
	sumIncrementalListVector.clear();
	maxIncrementalListVector.clear();
	minIncrementalListVector.clear();

	squaredSumCumulativeListVector.clear();
	sumCumulativeListVector.clear();
	maxCumulativeListVector.clear();
	minCumulativeListVector.clear();

	theFunctionIncrementalValueListVectorVector.clear();
	theFunctionCumulativeValueListVectorVector.clear();


	theIncrementalPlotList.clear();
	theIncrementalMeanCurveList.clear();
	theIncrementalPlusCurveList.clear();
	theIncrementalMinusCurveList.clear();


	theCumulativePlotList.clear();
	theCumulativeMeanCurveList.clear();
	theCumulativePlusCurveList.clear();
	theCumulativeMinusCurveList.clear();
}

void RDiscreteEventSamplingAnalyzer::initializeAccumulator()
{
	for (int i = 0; i < theMaxTimeSize; i++) {

		QList<double> maxList;
		QList<double> minList;
		QList<double> sumList;
		QList<double> squaredSumList;

		//QVector<QList<double> > functionIncrementalValueListVector;
		//QVector<QList<double> > functionCumulativeValueListVector;

		for (int j = 0; j < theFunctionList.count(); j++) {

			maxList.append(-1e308);
			minList.append(1e308);

			sumList.append(0.0);
			squaredSumList.append(0.0);

			//QList<double> list;
			//functionIncrementalValueListVector.append(list);
			//functionCumulativeValueListVector.append(list);
		}

		squaredSumIncrementalListVector.append(squaredSumList);
		sumIncrementalListVector.append(sumList);
		maxIncrementalListVector.append(maxList);
		minIncrementalListVector.append(minList);


		squaredSumCumulativeListVector.append(squaredSumList);
		sumCumulativeListVector.append(sumList);
		maxCumulativeListVector.append(maxList);
		minCumulativeListVector.append(minList);

	}

	if (plottingIsEnabled) {
		if ((theIncrementalPlotList.isEmpty()) && (theCumulativePlotList.isEmpty())) {
			for (int j = 0; j < theFunctionList.count(); j++) {
				QPointer<RPlotWidget> IncrementalPlot = 0;
				theIncrementalPlotList << IncrementalPlot;

				QPointer<RPlotWidget> cumPlot = 0;
				theCumulativePlotList << cumPlot;

#ifndef NO_QCP
				QCPGraph *MeanCurve = 0;
				theIncrementalMeanCurveList << MeanCurve;

				QCPGraph *PlusCurve = 0;
				theIncrementalPlusCurveList << PlusCurve;

				QCPGraph *MinusCurve = 0;
				theIncrementalMinusCurveList << MinusCurve;

				QCPGraph *cumMeanCurve = 0;
				theCumulativeMeanCurveList << cumMeanCurve;

				QCPGraph *cumPlusCurve = 0;
				theCumulativePlusCurveList << cumPlusCurve;

				QCPGraph *cumMinusCurve = 0;
				theCumulativeMinusCurveList << cumMinusCurve;
#endif // NO_QCP


			}
		}
		for (int i = 0; i < theFunctionList.count(); i++) {

			if (plottingIsEnabled) {
				if (theIncrementalPlotList[i] == 0) {
					// Initiating thePlot

					//RMainWindow *mainWindow = qobject_cast<RMainWindow *>(parent()->parent());
					QPointer<RPlotWidget> thePlot = new RPlotWidget(theMainWindow);

					theMainWindow->addSubWindow(thePlot, objectName() + " Plot of the Incremental Values of Function " + theFunctionList[i].data()->objectName(), true);

					// Assigning the autorange axes
					QList<QCPAxis*> axisrect;
					axisrect << thePlot->xAxis << thePlot->yAxis /*<< thePlot->yAxis2*/;
					thePlot->axisRect()->setRangeZoomAxes(axisrect);
					thePlot->axisRect()->setRangeDragAxes(axisrect);
					thePlot->setInteraction(QCP::iRangeDrag, true); // Enable dragging
					thePlot->setInteraction(QCP::iRangeZoom, true); // Enable zoom by scrolling

																	// Setting the Axis labels
					thePlot->xAxis->setLabel("Time (Days)");
					thePlot->yAxis->setLabel("Function Incremental Value");
					/*thePlot->yAxis2->setLabel("CDF or CoV");*/

					//thePlot->yAxis2->setTickLabels(true); // Shows the values for the tickmarks
					//thePlot->yAxis2->setVisible(true);

					theIncrementalPlotList[i] = thePlot;

					thePlot->addGraph(thePlot->xAxis, thePlot->yAxis); // adds the CoV curve attached to xAxis and yAxis2
					thePlot->graph(0)->setAntialiased(true);
					thePlot->graph(0)->setPen(QPen(Qt::black));
					theIncrementalMeanCurveList[i] = thePlot->graph(0);
					thePlot->graph(0)->setName("Mean Values");

					thePlot->addGraph(thePlot->xAxis, thePlot->yAxis); // adds PDF curve attached to xAxis and yAxis
					thePlot->graph(1)->setAntialiased(true);
					thePlot->graph(1)->setPen(QPen(Qt::red));
					theIncrementalPlusCurveList[i] = thePlot->graph(1);
					thePlot->graph(1)->setName("Mean plus one standard deviation Values");

					thePlot->addGraph(thePlot->xAxis, thePlot->yAxis); // adds CDF curve attached to xAxis and yAxis2
					thePlot->graph(2)->setAntialiased(true);
					thePlot->graph(2)->setPen(QPen(Qt::blue));
					theIncrementalMinusCurveList[i] = thePlot->graph(2);
					thePlot->graph(2)->setName("Mean minus one standard deviation Values");

					thePlot->legend->setVisible(true);
				}

				if (theCumulativePlotList[i] == 0) {
					// Initiating thePkot
					//RMainWindow *mainWindow = qobject_cast<RMainWindow *>(parent()->parent());
					QPointer<RPlotWidget> thePlot = new RPlotWidget(theMainWindow);

					theMainWindow->addSubWindow(thePlot, objectName() + " Plot of the Cumulative Values of Function " + theFunctionList[i].data()->objectName(), true);

					// Assigning the autorange axes
					QList<QCPAxis*> axisrect;
					axisrect << thePlot->xAxis << thePlot->yAxis /*<< thePlot->yAxis2*/;
					thePlot->axisRect()->setRangeZoomAxes(axisrect);
					thePlot->axisRect()->setRangeDragAxes(axisrect);
					thePlot->setInteraction(QCP::iRangeDrag, true); // Enable dragging
					thePlot->setInteraction(QCP::iRangeZoom, true); // Enable zoom by scrolling

																	// Setting the Axis labels
					thePlot->xAxis->setLabel("Time (Days)");
					thePlot->yAxis->setLabel("Function Cumulative Value");
					/*thePlot->yAxis2->setLabel("CDF or CoV");*/

					//thePlot->yAxis2->setTickLabels(true); // Shows the values for the tickmarks
					//thePlot->yAxis2->setVisible(true);

					theCumulativePlotList[i] = thePlot;

					thePlot->addGraph(thePlot->xAxis, thePlot->yAxis); // adds the CoV curve attached to xAxis and yAxis2
					thePlot->graph(0)->setAntialiased(true);
					thePlot->graph(0)->setPen(QPen(Qt::black));
					theCumulativeMeanCurveList[i] = thePlot->graph(0);
					thePlot->graph(0)->setName("Mean Values");

					thePlot->addGraph(thePlot->xAxis, thePlot->yAxis); // adds PDF curve attached to xAxis and yAxis
					thePlot->graph(1)->setAntialiased(true);
					thePlot->graph(1)->setPen(QPen(Qt::red));
					theCumulativePlusCurveList[i] = thePlot->graph(1);
					thePlot->graph(1)->setName("Mean plus one standard deviation Values");

					thePlot->addGraph(thePlot->xAxis, thePlot->yAxis); // adds CDF curve attached to xAxis and yAxis2
					thePlot->graph(2)->setAntialiased(true);
					thePlot->graph(2)->setPen(QPen(Qt::blue));
					theCumulativeMinusCurveList[i] = thePlot->graph(2);
					thePlot->graph(2)->setName("Mean minus one standard deviation Values");

					thePlot->legend->setVisible(true);
				}
			}
		}
	}
}

void RDiscreteEventSamplingAnalyzer::modifyTimeSize(QVector<double> sampleFixedIntervalVector, QVector<QList<double> > *sampleResultFixedIntervalListVector, QVector<QList<double> > *sampleCumulativeResultFixedIntervalListVector)
{
	QList<double> zeroList;
	QList<double> maxList;
	QList<double> minList;
	for (int i = 0; i < theFunctionList.count(); i++) {
		zeroList << 0;
		maxList << -1e308;
		minList << 1e308;
	}

	//if this event time size is less than max time size it adds zero list to the end of this event sample result
	if (theMaxTimeSize > sampleFixedIntervalVector.count()) {
		int sizeDifference = theMaxTimeSize - sampleFixedIntervalVector.count();
		QVector<QList<double> > tempListVector = *sampleCumulativeResultFixedIntervalListVector;
		QList<double> lastValueList = tempListVector.last();
		for (int i = 0; i < sizeDifference; i++) {
			*sampleResultFixedIntervalListVector << zeroList;
			*sampleCumulativeResultFixedIntervalListVector << lastValueList;
		}

	}
	else {
		//if this event time size is more than max time size it modifies list of all previous events sample result
		int sizeDifference = sampleFixedIntervalVector.count() - theMaxTimeSize;

		for (int i = 0; i < theFunctionIncrementalValueListVectorVector.count(); i++) {
			QList<double> lastValueList = theFunctionCumulativeValueListVectorVector[i].last();
			for (int j = 0; j < sizeDifference; j++) {
				theFunctionIncrementalValueListVectorVector[i] << zeroList;
				theFunctionCumulativeValueListVectorVector[i] << lastValueList;
			}
		}

		for (int j = 0; j < sizeDifference; j++) {

			minIncrementalListVector.append(minList);
			maxIncrementalListVector.append(maxList);
			if (theDisplayDiagrams) {
				sumIncrementalListVector.append(zeroList);
				squaredSumIncrementalListVector.append(zeroList);
			}
		}
		for (int j = 0; j < sizeDifference; j++) {

			minCumulativeListVector.append(minCumulativeListVector.last());
			maxCumulativeListVector.append(maxCumulativeListVector.last());
			if (theDisplayDiagrams) {
				sumCumulativeListVector.append(sumCumulativeListVector.last());
				squaredSumCumulativeListVector.append(squaredSumCumulativeListVector.last());
			}
		}
		maxFixedIntervalVector.clear();
		for (int k = 0; k < sampleFixedIntervalVector.count(); k++) {
			maxFixedIntervalVector << sampleFixedIntervalVector[k] - initialTime;
		}

		//theMaxTimeSizeLock->lock();
		theMaxTimeSize = sampleFixedIntervalVector.count();
		//theMaxTimeSizeLock->unlock();
	}
}

int RDiscreteEventSamplingAnalyzer::accumulate(QVector<QList<double> > functionIncrementalValueListVector, QVector<QList<double> > functionCumulativeValueListVector, int threadID)
{

	QString tempString;

	//Clear the vector lists that stores the mean, plus, and minus Incremental values
	theFunctionIncrementalMeanValueVectorList.clear();
	theFunctionIncrementalStdevValueVectorList.clear();

	theFunctionIncrementalPlusValueVectorList.clear();
	theFunctionIncrementalMinusValueVectorList.clear();

	//Clear the vector lists that stores the mean, plus, and minus cumulative values
	theFunctionCumulativeMeanValueVectorList.clear();
	theFunctionCumulativeStdevValueVectorList.clear();

	theFunctionCumulativePlusValueVectorList.clear();
	theFunctionCumulativeMinusValueVectorList.clear();

	// Iterating on functions
	for (int i = 0; i < theFunctionList.count(); i++) {

		for (int j = 0; j < theMaxTimeSize; j++) {
			if (maxIncrementalListVector[j][i] < functionIncrementalValueListVector[j][i]) {
				maxIncrementalListVector[j][i] = functionIncrementalValueListVector[j][i];
			}
			if (minIncrementalListVector[j][i] > functionIncrementalValueListVector[j][i]) {
				minIncrementalListVector[j][i] = functionIncrementalValueListVector[j][i];
			}
			if (theDisplayDiagrams) {
				sumIncrementalListVector[j][i] += functionIncrementalValueListVector[j][i];
				squaredSumIncrementalListVector[j][i] += functionIncrementalValueListVector[j][i] * functionIncrementalValueListVector[j][i];
			}
		}
		for (int j = 0; j < theMaxTimeSize; j++) {
			if (maxCumulativeListVector[j][i] < functionCumulativeValueListVector[j][i]) {
				maxCumulativeListVector[j][i] = functionCumulativeValueListVector[j][i];
			}
			if (minCumulativeListVector[j][i] > functionCumulativeValueListVector[j][i]) {
				minCumulativeListVector[j][i] = functionCumulativeValueListVector[j][i];
			}
			if (theDisplayDiagrams) {
				sumCumulativeListVector[j][i] += functionCumulativeValueListVector[j][i];
				squaredSumCumulativeListVector[j][i] += functionCumulativeValueListVector[j][i] * functionCumulativeValueListVector[j][i];
			}
		}
	}

	theFunctionIncrementalValueListVectorVector.append(functionIncrementalValueListVector);
	theFunctionCumulativeValueListVectorVector.append(functionCumulativeValueListVector);

	///////////////// if we want to update the plot after each sample
	if ((((theIterationIndex % theAccumulator.data()->getPlottingInterval() == 0) || (theIterationIndex == theAccumulator.data()->getMaximumIterations())) && (theIterationIndex > 1)) && (theDisplayDiagrams)) {
		for (int i = 0; i < theFunctionList.count(); i++) {
			//Calculate the mean and stdev of Incremental values of each function for each time interval
			QVector<double> meanIncrementalValuesVector;
			QVector<double> stdevIncrementalValuesVector;
			QVector<double> plusIncrementalValuesVector;
			QVector<double> minusIncrementalValuesVector;

			for (int j = 0; j < sumIncrementalListVector.count(); j++) {
				double mean = sumIncrementalListVector[j][i] / theIterationIndex;
				meanIncrementalValuesVector << mean;

				double stdev;
				if (theIterationIndex > 1) {
					stdev = sqrt(1.0 / (theIterationIndex - 1) * (squaredSumIncrementalListVector[j][i] - theIterationIndex * mean * mean));

				}
				else {
					stdev = 0;
				}

				stdevIncrementalValuesVector << stdev;

				double plus = mean + stdev;
				plusIncrementalValuesVector << plus;

				double minus = mean - stdev;
				minusIncrementalValuesVector << minus;
			}

			theFunctionIncrementalMeanValueVectorList.append(meanIncrementalValuesVector);
			theFunctionIncrementalStdevValueVectorList.append(stdevIncrementalValuesVector);
			theFunctionIncrementalPlusValueVectorList.append(plusIncrementalValuesVector);
			theFunctionIncrementalMinusValueVectorList.append(minusIncrementalValuesVector);


			//Calculate the mean and stdev of cumulative values of each function for each time interval
			QVector<double> meanCumulativeValuesVector;
			QVector<double> stdevCumulativeValuesVector;
			QVector<double> plusCumulativeValuesVector;
			QVector<double> minusCumulativeValuesVector;

			for (int j = 0; j < sumCumulativeListVector.count(); j++) {
				double mean = sumCumulativeListVector[j][i] / theIterationIndex;
				meanCumulativeValuesVector << mean;

				double stdev;
				if (theIterationIndex > 1) {
					stdev = sqrt(1.0 / (theIterationIndex - 1) * (squaredSumCumulativeListVector[j][i] - theIterationIndex * mean * mean));
				}
				else {
					stdev = 0;
				}
				stdevCumulativeValuesVector << stdev;
				double plus = mean + stdev;
				plusCumulativeValuesVector << plus;

				double minus = mean - stdev;
				minusCumulativeValuesVector << minus;
			}

			theFunctionCumulativeMeanValueVectorList.append(meanCumulativeValuesVector);
			theFunctionCumulativeStdevValueVectorList.append(stdevCumulativeValuesVector);
			theFunctionCumulativePlusValueVectorList.append(plusCumulativeValuesVector);
			theFunctionCumulativeMinusValueVectorList.append(minusCumulativeValuesVector);

		}
	}
	/////////////////

	if ((((theIterationIndex % theAccumulator.data()->getPlottingInterval() == 0) || (theIterationIndex == theAccumulator.data()->getMaximumIterations())) && (theIterationIndex > 1)) && (theDisplayDiagrams)) {
		if (threadID == 0)
			updatePlot();
	}

	return 0;

}

void RDiscreteEventSamplingAnalyzer::updatePlot()
{
	for (int i = 0; i < theFunctionList.count(); i++) {

#ifndef NO_QCP
		if (plottingIsEnabled && (theIncrementalPlotList[i])) {
			theIncrementalMeanCurveList[i]->setData(maxFixedIntervalVector, theFunctionIncrementalMeanValueVectorList[i]);
			theIncrementalPlusCurveList[i]->setData(maxFixedIntervalVector, theFunctionIncrementalPlusValueVectorList[i]);
			theIncrementalMinusCurveList[i]->setData(maxFixedIntervalVector, theFunctionIncrementalMinusValueVectorList[i]);

			theIncrementalPlotList[i]->xAxis->rescale(true);
			theIncrementalPlotList[i]->yAxis->rescale(true);
			theIncrementalPlotList[i]->yAxis2->rescale(true);

			theIncrementalPlotList[i]->xAxis->setRangeUpper(1.05*theIncrementalPlotList[i]->xAxis->range().upper);
			theIncrementalPlotList[i]->yAxis->scaleRange(1.1);
			theIncrementalPlotList[i]->yAxis2->scaleRange(1.1);

			// Replot
			theIncrementalPlotList[i]->replot();
		}
		if (plottingIsEnabled && (theCumulativePlotList[i])) {

			theCumulativeMeanCurveList[i]->setData(maxFixedIntervalVector, theFunctionCumulativeMeanValueVectorList[i]);
			theCumulativePlusCurveList[i]->setData(maxFixedIntervalVector, theFunctionCumulativePlusValueVectorList[i]);
			theCumulativeMinusCurveList[i]->setData(maxFixedIntervalVector, theFunctionCumulativeMinusValueVectorList[i]);

			theCumulativePlotList[i]->xAxis->rescale(true);
			theCumulativePlotList[i]->yAxis->rescale(true);
			theCumulativePlotList[i]->yAxis2->rescale(true);

			theCumulativePlotList[i]->xAxis->setRangeUpper(1.05*theCumulativePlotList[i]->xAxis->range().upper);
			theCumulativePlotList[i]->yAxis->scaleRange(1.1);
			theCumulativePlotList[i]->yAxis2->scaleRange(1.1);

			// Replot
			theCumulativePlotList[i]->replot();
		}
#endif // NO_QCP



	}

}

void RDiscreteEventSamplingAnalyzer::savePDFtoTextFile(int functionIndex, bool incremental)
{
	int i = functionIndex;
	QString fileName;
	QString PDFType = "Incremental";
	if (!incremental) PDFType = "Cumulative";

	fileName = theFunctionList[i].data()->objectName() + " " + PDFType + " Values PDF.txt";

	QList<QVector<double> > xVectorList;
	QList<QVector<double> > PDFVectorList;
	for (int t = 0; t < theMaxTimeSize; t++) {
		QVector<double> xVector;
		QVector<double> PDFVector;
		evaluateHistogram(i, t, incremental, &xVector, &PDFVector);
		xVectorList << xVector;
		PDFVectorList << PDFVector;
	}

	QFile *file = new QFile(fileName);
	if (file->exists()) {
		file->remove();
	}

	// The follwoing lines insert the headers (function names, etc.) at the top of the file 
	QString tempStr;
	for (int i = 0; i < theMaxTimeSize; i++) {
		tempStr += QString("%1\tPDF(%1)\t").arg(maxFixedIntervalVector[i]);
	}
	tempStr += "\n";

	if (!file->open(QIODevice::WriteOnly | QIODevice::Text)) {
		qCritical() << "Error: Could not create the file" << fileName;
		return;
	}

	QByteArray byteArray;
	byteArray.append(tempStr);
	tempStr.clear();

	for (int i = 0; i < theNumberOfBins; i++) {
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

void RDiscreteEventSamplingAnalyzer::saveCDFtoTextFile(int functionIndex, bool incremental)
{
	int i = functionIndex;
	QString fileName;
	QString CDFType = "Incremental";
	if (!incremental) CDFType = "Cumulative";

	fileName = theFunctionList[i].data()->objectName() + " " + CDFType + " Values CDF.txt";

	QList<QList<double> > sortedValueListList;
	QList<double> CDFList;
	QList<double> CoVList;

	QList<double> functionTimeValueList;
	QList<QList<double> > functionTimeValueListList;
	if (incremental) {
		for (int t = 0; t < theMaxTimeSize; t++) {
			functionTimeValueList.clear();
			for (int j = 0; j < theFunctionIncrementalValueListVectorVector.count(); j++) {
				functionTimeValueList << theFunctionIncrementalValueListVectorVector[j][t][i];
			}
			functionTimeValueListList << functionTimeValueList;
		}
	}
	else {
		for (int t = 0; t < theMaxTimeSize; t++) {
			functionTimeValueList.clear();
			for (int j = 0; j < theFunctionCumulativeValueListVectorVector.count(); j++) {
				functionTimeValueList << theFunctionCumulativeValueListVectorVector[j][t][i];
			}
			functionTimeValueListList << functionTimeValueList;
		}
	}


	for (int t = 0; t < theMaxTimeSize; t++) {
		functionTimeValueList = functionTimeValueListList[t];
		qSort(functionTimeValueList.begin(), functionTimeValueList.end());
		sortedValueListList << functionTimeValueList;

		if (t == 0) {
			computeCDFandCoV(&CDFList, &CoVList);
		}
	}
	sortedValueListList.insert(0, CoVList);
	sortedValueListList.insert(0, CDFList);

	QFile *file = new QFile(fileName);
	if (file->exists()) {
		file->remove();
	}

	// The follwoing lines insert the headers (function names, etc.) at the top of the file 
	QString tempStr = "CDF\tCoV\t";
	for (int t = 0; t < theMaxTimeSize; t++) {
		tempStr += QString("%1\t").arg(maxFixedIntervalVector[t]);
	}
	tempStr += "\n";

	if (!file->open(QIODevice::WriteOnly | QIODevice::Text)) {
		qCritical() << "Error: Could not create the file" << fileName;
		return;
	}

	QByteArray byteArray;
	byteArray.append(tempStr);
	tempStr.clear();

	for (int i = 0; i < theIterationIndex; i++) {
		for (int j = 0; j < sortedValueListList.count(); j++) {
			tempStr += QString("%1\t").arg(sortedValueListList[j][i], 0, 'g', 12);
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

void RDiscreteEventSamplingAnalyzer::computeCDFandCoV(QList<double> *CDFList, QList<double> *CoVList)
{
	for (int i = 1; i <= theIterationIndex; i++) {
		double CDF = (double)i / (double)theIterationIndex;
		*CDFList << CDF;

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
		*CoVList << CoV;
	}
}

void RDiscreteEventSamplingAnalyzer::evaluateHistogram(int functionIndex, int timeIndex, bool incremental, QVector<double> *xVector, QVector<double> *PDFVector)
{
	theNumberOfBins = theHistogramAccumulator->getNumberOfBins();

	int i = functionIndex;
	int t = timeIndex;
	QList<double> functionTimeValueList;

	if (incremental) {

		for (int j = 0; j < theFunctionIncrementalValueListVectorVector.count(); j++) {
			functionTimeValueList << theFunctionIncrementalValueListVectorVector[j][t][i];
		}

		//this if is for the case that all of the numbers are the same-in other words there is no histogram or pdf
		if (minIncrementalListVector[t][i] != maxIncrementalListVector[t][i]) {
			// PDF curve
			// Allocate memory for a histogram
			gsl_histogram *histogram = gsl_histogram_alloc(theNumberOfBins);

			// Reset the histogram
			gsl_histogram_set_ranges_uniform(histogram, minIncrementalListVector[t][i], maxIncrementalListVector[t][i]);

			// Loop over the samples to generate the histogram
			for (int j = 0; j < theIterationIndex; j++) {
				// Add values to create histogram
				gsl_histogram_increment(histogram, functionTimeValueList[j]);
			}

			// Normalize to get relative frequency diagram (“PDF”)
			gsl_histogram_scale(histogram, 1.0 / ((maxIncrementalListVector[t][i] - minIncrementalListVector[t][i]) / theNumberOfBins * theIterationIndex));

			// Compute the histogram
			for (int j = 0; j < theNumberOfBins; j++) {
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
		else {
			for (int j = 0; j < theNumberOfBins; j++) {
				*xVector << minIncrementalListVector[t][i];
				*PDFVector << 1 / (maxIncrementalListVector[t][i] - minIncrementalListVector[t][i]);
			}
		}


	}
	else {
		for (int j = 0; j < theFunctionCumulativeValueListVectorVector.count(); j++) {
			functionTimeValueList << theFunctionCumulativeValueListVectorVector[j][t][i];
		}

		//this if is for the case that all of the numbers are the same-in other words there is no histogram or pdf
		if (minCumulativeListVector[t][i] != maxCumulativeListVector[t][i]) {
			// CDF curve
			// Allocate memory for a histogram
			gsl_histogram *histogram = gsl_histogram_alloc(theNumberOfBins);

			// Reset the histogram
			gsl_histogram_set_ranges_uniform(histogram, minCumulativeListVector[t][i], maxCumulativeListVector[t][i]);

			// Loop over the samples to generate the histogram
			for (int j = 0; j < theIterationIndex; j++) {
				// Add values to create histogram
				gsl_histogram_increment(histogram, functionTimeValueList[j]);
			}

			// Normalize to get relative frequency diagram (“CDF”)
			gsl_histogram_scale(histogram, 1.0 / ((maxCumulativeListVector[t][i] - minCumulativeListVector[t][i]) / theNumberOfBins * theIterationIndex));

			// Compute the histogram
			for (int j = 0; j < theNumberOfBins; j++) {
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
		else {
			for (int j = 0; j < theNumberOfBins; j++) {
				*xVector << minCumulativeListVector[t][i];
				*PDFVector << 1 / (maxCumulativeListVector[t][i] - minCumulativeListVector[t][i]);
			}
		}

	}

}

void RDiscreteEventSamplingAnalyzer::terminateAccumulation() {

	bool incremental = true;
	bool cumulative = false;

	//// Save the CDF and PDF in a text file, if the user has aksed to
	for (int i = 0; i < theFunctionList.count(); i++) {
		if (theFunctionAccumulationTypeList[i] == 1) {
			savePDFtoTextFile(i, incremental);
			saveCDFtoTextFile(i, incremental);
		}
		else if (theFunctionAccumulationTypeList[i] == 2) {
			savePDFtoTextFile(i, cumulative);
			saveCDFtoTextFile(i, cumulative);
		}
		else if (theFunctionAccumulationTypeList[i] == 3) {
			savePDFtoTextFile(i, incremental);
			saveCDFtoTextFile(i, incremental);
			savePDFtoTextFile(i, cumulative);
			saveCDFtoTextFile(i, cumulative);
		}
	}
}

void RDiscreteEventSamplingAnalyzer::updateSamplePlot(int sampleCount, QVector<double> sampleUniqueFinishTimeVector, QVector<QList<double> > sampleIncrementalResultListVector, QVector<QList<double> > sampleCumulativeResultListVector) {
#ifndef NO_QCP
	if (plottingIsEnabled) {
		for (int i = 0; i < theFunctionList.count(); i++) {
			// Plotting the time axes
			// Clearing theCurveList
			if ((theTimePlotList[i]) && (sampleCount % theAccumulator.data()->getPlottingInterval() == 0)) {
				// Creating x and y vectors
				QVector<double> xVector;
				QVector<double> yVector;
				//xVector << initialTime;
				//yVector << 0;
				for (int j = 0; j < sampleUniqueFinishTimeVector.count(); j++) {
					xVector << (sampleUniqueFinishTimeVector[j] - initialTime) << (sampleUniqueFinishTimeVector[j] - initialTime);
					xVector << (sampleUniqueFinishTimeVector[j] - initialTime) << (sampleUniqueFinishTimeVector[j] - initialTime);
					yVector << 0 << sampleIncrementalResultListVector[j][i];
					yVector << sampleIncrementalResultListVector[j][i] << 0;
				}

				theCurveList[i]->setData(xVector, yVector);

			}
			// Update the plot
			if ((theTimePlotList[i]) && (sampleCount % theAccumulator.data()->getPlottingInterval() == 0)) {
				theTimePlot->xAxis->rescale(true);
				theTimePlot->yAxis->rescale(true);
				theTimePlot->yAxis2->rescale(true);

				theTimePlot->xAxis->setRangeUpper(1.05*theTimePlot->xAxis->range().upper);
				theTimePlot->yAxis->scaleRange(1.1);
				theTimePlot->yAxis2->scaleRange(1.1);
				theTimePlotList[i]->replot();
			}
		}
	}

	//Update the Cumulative plot after the recovery process has finished
	if (plottingIsEnabled) {
		for (int i = 0; i < theFunctionList.count(); i++) {
			// Plotting the time axes
			// Clearing theCurveList
			if ((theCumulativeTimePlotList[i]) && (sampleCount % theAccumulator.data()->getPlottingInterval() == 0)) {
				// Creating x and y vectors
				QVector<double> xVector;
				QVector<double> yVector;
				//xVector << initialTime;
				//yVector << 0;
				for (int j = 0; j < sampleUniqueFinishTimeVector.count(); j++) {
					xVector << (sampleUniqueFinishTimeVector[j] - initialTime);
					//xVector << (sampleUniqueFinishTimeVector[j]-initialTime) << (sampleUniqueFinishTimeVector[j]-initialTime);
					yVector << sampleCumulativeResultListVector[j][i];
					//yVector << sampleIncrementalResultListVector[j][i] << 0;
				}

				theCumulativeCurveList[i]->setData(xVector, yVector);
			}
			// Update the plot
			if ((theCumulativeTimePlotList[i]) && (sampleCount % theAccumulator.data()->getPlottingInterval() == 0)) {
				theCumulativeTimePlot->xAxis->rescale(true);
				theCumulativeTimePlot->yAxis->rescale(true);
				theCumulativeTimePlot->yAxis2->rescale(true);

				theCumulativeTimePlot->xAxis->setRangeUpper(1.05*theCumulativeTimePlot->xAxis->range().upper);
				theCumulativeTimePlot->yAxis->scaleRange(1.1);
				theCumulativeTimePlot->yAxis2->scaleRange(1.1);
				theCumulativeTimePlotList[i]->replot();
			}
		}
	}
#endif // NO_QCP


}