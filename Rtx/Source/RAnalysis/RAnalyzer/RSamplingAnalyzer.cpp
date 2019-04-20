#include "RSamplingAnalyzer.h"

#include <QDateTime>
#include <QCoreApplication>

#include "RMainWindow.h"
#include "RDomain.h"
#include "RFunction.h"
#include "RFunction.h"
#include "RRandomVariable.h"
#include "RTransformer.h"
#include "RAccumulator.h"
#include "RRandomNumberGenerator.h"
#include "RFailureProbabilityAccumulator.h"
#include "RIndependentNormalRandomNumberGenerator.h"

#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_cdf.h>
#include <gsl/gsl_linalg.h>

#include <qmutex.h>
#include <QMutexLocker>

#include "RParallelProcessor.h"


RSamplingAnalyzer::RSamplingAnalyzer(QObject *parent, QString name)
	: RAnalyzer(parent, name), RParallelComputerInterface()
{
	/* //$$ 
	theRandomNumberGenerator = 0;
	theTransformer = 0;
	theAccumulator = 0;
	*/

	analysisParallelComputer = new RParallelProcessor(this);

	theComputeRandomVariableSensitivities = false;
	theNeglectCorrelationInSensitivity = false;
}

RSamplingAnalyzer::~RSamplingAnalyzer()
{
	delete analysisParallelComputer;
}

QObject * RSamplingAnalyzer::getRandomNumberGenerator() const	
{	
	return theRandomNumberGenerator.data();
}	
	
void RSamplingAnalyzer::setRandomNumberGenerator(QObject *value)	
{	
	theRandomNumberGenerator = qobject_cast<RRandomNumberGenerator *>(value);
}	
	
QObject * RSamplingAnalyzer::getTransformer() const	
{	
	return theTransformer.data();
}	
	
void RSamplingAnalyzer::setTransformer(QObject *value)	
{	
	theTransformer = qobject_cast<RTransformer *>(value);
}	
	
QObject * RSamplingAnalyzer::getAccumulator() const	
{	
	return theAccumulator.data();
}	
	
void RSamplingAnalyzer::setAccumulator(QObject *value)	
{	
	theAccumulator = qobject_cast<RAccumulator *>(value);
}	
	
QString RSamplingAnalyzer::getFunctionList()	
{	
	return functionListToString(theFunctionList);
}	
	
void RSamplingAnalyzer::setFunctionList(QString value)	
{	
	theFunctionList = stringToFunctionList(value);
}


QString RSamplingAnalyzer::getSensitivityRandomVariableList()	
{	
	QList<RPointer<RParameter> > paramList;
	for (int i = 0; i < theSensitivityRandomVariableList.size(); i++) {
        RPointer<RParameter> param = qobject_cast<RParameter *>(theSensitivityRandomVariableList[i].data()); //salarifard
		paramList << param;
	}

	return parameterListToString(paramList);
}

void RSamplingAnalyzer::setSensitivityRandomVariableList(QString value)	
{	
	theSensitivityRandomVariableList.clear();
	QList<RPointer<RParameter> > paramList = stringToParameterList(value);
	for (int i = 0; i < paramList.size(); i++) {
		RRandomVariable *rv = qobject_cast<RRandomVariable *>(paramList[i].data());
		if (rv) {
			theSensitivityRandomVariableList << rv;
		} else {
			// Issue an error if there is no such random variable
			rCritical() << "Error in" << objectName() << ": The variable" << paramList[i].data()->objectName() << "is not an random variable.";
			return;
		}
	}
}

bool RSamplingAnalyzer::getComputeRandomVariableSensitivities()
{
	return theComputeRandomVariableSensitivities;
}	

void RSamplingAnalyzer::setComputeRandomVariableSensitivities(bool value)
{
	theComputeRandomVariableSensitivities = value;
}

bool RSamplingAnalyzer::getNeglectCorrelationInSensitivity()
{
	return theNeglectCorrelationInSensitivity;
}	

void RSamplingAnalyzer::setNeglectCorrelationInSensitivity(bool value)
{
	theNeglectCorrelationInSensitivity = value;
}

QString RSamplingAnalyzer::getSensitivityFunctionList()	
{	
	return functionListToString(theSensitivityFunctionList);
}	
	
void RSamplingAnalyzer::setSensitivityFunctionList(QString value)	
{	
	theSensitivityFunctionList = stringToFunctionList(value);
}


int RSamplingAnalyzer::analyze()
{

	int result = this->analyze(true);

	if (result < 0) {
		return result;
	}
}

int RSamplingAnalyzer::analyze(bool populateListsInFunction)
{
	// Inform the user that the sampling analysis has started
	rDebug() << "##### STARTING SAMPLING ANALYSIS ...";
	rDebug() << "";

	QDateTime startTime = QDateTime::currentDateTime();


	// Issue an error if the function list is empty
	if (theFunctionList.isEmpty()) {
		rCritical() << "Error in" << objectName() << ": Select at least one function and try again";
		return -1;
	}

	// Checking if all analysis tools are provided
	int result = checkProperties();
	if (result < 0) {
		rCritical() << "Error in" << objectName() << ": Could find one or more necessary tools";
		return -1;
	}

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

	theAccumulator.data()->setComputeRandomVariableSensitivities(theComputeRandomVariableSensitivities);
	theAccumulator.data()->setFunctionList(functionList);


	// Sensitivity implementations
	if (theComputeRandomVariableSensitivities) {
		for (int i = 0; i < theSensitivityFunctionList.count(); i++) {
			if (!theFunctionList.contains(theSensitivityFunctionList[i])) {
				rCritical() << "Error: The function" << theSensitivityFunctionList[i].data()->objectName() << "that appears in the sensitivity function list of the sampling analyzer must also be included in the function list.";
				return -2;
			}
		}
		QList<bool> sensitivityFunctionFlagList;
		for (int i = 0; i < theFunctionList.count(); i++) {
			if (theSensitivityFunctionList.contains(theFunctionList[i])) {
				sensitivityFunctionFlagList << true;
			} else {
				sensitivityFunctionFlagList << false;
			}
		}
		theAccumulator.data()->setSensitivityRandomVariableList(theSensitivityRandomVariableList);
		theAccumulator.data()->setSensitivityFunctionFlagList(sensitivityFunctionFlagList);

		// Issue an error if theSensitivityFunctionList is empty 
		if (theSensitivityFunctionList.isEmpty()) {
			rCritical() << "Error in" << objectName() << ": At least one function is needed for the sensitivity analysis.";
			return -1;
		}

		// Issue an error if theSensitivityRandomVariableList is empty 
		if (theSensitivityRandomVariableList.isEmpty()) {
			rCritical() << "Error in" << objectName() << ": At least one random variable is needed for the sensitivity analysis.";
			return -1;
		}
	}


	int status = theRandomNumberGenerator.data()->checkProperties();
	if (status < 0) {
		rCritical() << "Error: Could find one or more necessary tools in the object" << theRandomNumberGenerator.data()->objectName();
		return -1;
	}
	
	status = theTransformer.data()->checkProperties();
	if (status < 0) {
		rCritical() << "Error: Could find one or more necessary tools in the object" << theTransformer.data()->objectName();
		return -1;
	}
	
	status = theAccumulator.data()->checkProperties();
	if (status < 0) {
		rCritical() << "Error: Could find one or more necessary tools in the object" << theAccumulator.data()->objectName();
		return -1;
	}

	// Iterating on the passed functions to parse them
	if (populateListsInFunction) {
		for (int i = 0; i < theFunctionList.count(); i++) {
			result = theFunctionList[i].data()->populateLists();
			if (result < 0) {
				rCritical() << "Error: Could not parse the function" << theFunctionList[i].data()->objectName();
				return -1;
			}
		}
	}
	
	// Create the mereged list of random variables used in all of the passed functions (and eliminating the repeated ones
	theRandomVariableList.clear();
	for (int i = 0; i < theFunctionList.count(); i++) {
		QList<RRandomVariable *> *tempRandomVariableList = theFunctionList[i].data()->getRandomVariableList();
		for (int j = 0; j < tempRandomVariableList->count(); j++) {
			RRandomVariable *tempRandomVariable = tempRandomVariableList->value(j);
			if (!theRandomVariableList.contains(tempRandomVariable)) {
				theRandomVariableList << tempRandomVariable;
			}
		}
	}
	
	// Soring the theRandomVariableList according to alphabetical order
	theDomain->sort(&theRandomVariableList);

	theNumberOfRandomVariables = theRandomVariableList.count();

	// Backing up the currentValue of the random variables
	QVector<double> randomVariablesBackup;
	for (int i = 0; i < theNumberOfRandomVariables; i++) {
		randomVariablesBackup.append(theRandomVariableList[i]->getCurrentValue());
	}

	// Setting the list of random variables to the transformer
	result = theTransformer.data()->setRandomVariableList(&theRandomVariableList);
	if (result < 0) {
		rCritical() << "Error: Could not set the list of random variables to the transformer.";
		return -1;
	}

	// Setting the list of random variables to the RandomNumberGenerator (In RandomNumberGenerator types which need a start point, this line also establishes the start point vector)
	result = theRandomNumberGenerator.data()->setRandomVariableList(&theRandomVariableList);
	if (result < 0) {
		rCritical() << "Error: Could not set the list of random variables to the random number generator.";
		return -1;
	}

	bool isFirstSimulation = true;
	//double seed = 1;


	// Sensitivity implementations
	int SRVSize = 0;
	if  (theComputeRandomVariableSensitivities) {	
		// Set number of sensitivity random variables
		SRVSize = theSensitivityRandomVariableList.size();

		for (int i = 0; i < SRVSize; i++) {
			if (!theRandomVariableList.contains(theSensitivityRandomVariableList[i].data())) {
				rCritical() << "Error: The list of random variables that affect the functions enterd for sampling analysis does not include" << theSensitivityRandomVariableList[i].data()->objectName();
				return -2;
			}
		}
	}


	// doing main analysis
	void * inputArgs[] = {(void *) (&SRVSize)};
	status = analysisParallelComputer->run(&RSamplingAnalyzer::doAnalysisWrapper, this, inputArgs);

	QCoreApplication::processEvents();

	// checking error occurance
	if (status < 0)
		return status;

	theAccumulator.data()->terminateAccumulation();


	QDateTime finishTime = QDateTime::currentDateTime();
	double numDays = startTime.daysTo(finishTime);
	double numMilliSecs = startTime.time().msecsTo(finishTime.time());
	double numSecs = numDays * 86400.0 + numMilliSecs / 1000.0;

	rDebug() << "##### SAMPLING ANALYSIS DONE IN" << numSecs << "SECONDS.";
	rDebug() << "";
	rDebug() << "";

	// Restoring the currentValue of the random variables from the storedValue
	for (int i = 0; i < theNumberOfRandomVariables; i++) {
		theRandomVariableList[i]->setCurrentValue(randomVariablesBackup[i]);
	}

	for (int i = 0 ; i < theFunctionList.count(); i++)
		theFunctionList[i].data()->resetAfterParallelComputation();

	return 0;
}

// a wrapper around doAnalysis method to give to the parallel computer
int RSamplingAnalyzer::doAnalysisWrapper(void * theObject, void * inputArgs[], int threadID){

	// casting object
	RSamplingAnalyzer * thisSampling = (RSamplingAnalyzer *) (theObject);

	// casting input arguments
	int SRVSize = * ((int *) inputArgs[0]); 

	// calling the method
	return thisSampling->doAnalysis(SRVSize, threadID);
}

int RSamplingAnalyzer::doAnalysis(int SRVSize, int threadID){

	// ATTENTION : in multithreaded mode we will have first simulation for each thread!
	bool isFirstSimulation = true;

	// ATTENTION : general error messages (error messages related to all threads) will be printed only in the first simulation of each thread

	// Declaring vector u (Attention: Vector u should not be allocated here, it just gets the memory address of the genereted random numbers
	gsl_vector * u = 0;
	
	// Allocating vectors x
	gsl_vector * x = gsl_vector_calloc(theNumberOfRandomVariables);

	// Sensitivity implementations
	// Allocating the vectors to store sensitivity results in each sample
	gsl_vector * dfdMean_1_f = 0;
	gsl_vector * dfdStdv_1_f = 0;
	// allocating dfdMean_1_f and dfdStdv_1_f
	if (theComputeRandomVariableSensitivities){
		dfdMean_1_f = gsl_vector_calloc(SRVSize);
		dfdStdv_1_f = gsl_vector_calloc(SRVSize);
	}

	int result;
	bool continueConditionInThread = true;

	// start running samples till continue condition flag becomes false
	while (analysisParallelComputer->getContinueCondition() && continueConditionInThread){

		// Generating random numbers in standard-normal space
		if (isFirstSimulation) {
			result = theRandomNumberGenerator.data()->generateNumbers(theNumberOfRandomVariables/*, seed*/, threadID);
		} else {
			result = theRandomNumberGenerator.data()->generateNumbers(theNumberOfRandomVariables, threadID);
		}

		if (result < 0) {

			rCritical(threadID) << "Error: Could not generate random numbers for sampling analysis.";
			return endAnalysis(x, dfdMean_1_f, dfdStdv_1_f, -1);
		}

		// Getting the vector of generated random numbers
		u = theRandomNumberGenerator.data()->getGeneratedNumbers(threadID);

		// Transform starting point into original space
		result = theTransformer.data()->transformPointToOriginalSpace(u, x, threadID);

		if (result < 0) {
			rCritical(threadID) << "Error: Could not transform from standard-normal space to original space.";
			return endAnalysis(x, dfdMean_1_f, dfdStdv_1_f, -1);
		}

		// Update the current value of random variables to the new realization
		for (int i = 0; i < theNumberOfRandomVariables; i++) {
			theRandomVariableList[i]->setCurrentValue(gsl_vector_get(x, i), threadID);
		}

		// Iterating on the passed functions and calling the method to update the isAnalyzed flags of RResponses due to the new realization of random variables for each function
		for (int i = 0; i < theFunctionList.count(); i++) {
			theFunctionList[i].data()->updateAnalysisStatus(threadID);
		}
		
		// Declaring a QVector of double to store the results of this sample for all function
		QVector<double> sampleResult;
		
		bool convergence = true;

		// Iterating on the passed functions
		for (int i = 0; i < theFunctionList.count(); i++) {
			RFunction *function = theFunctionList[i].data();
			
			// Evaluate the function
			result = function->evaluateFunction(false, RObject::RandomVariable, threadID);
			double functionValue = function->getFunctionValue(threadID);
			
			// result = 0 mean function is evaluated without error;
			// result = -1 means function is not evaluated due to errors other than non-convergence;
			// result < -1 means function is not evaluated due to non-convergence.
			if (result < 0) {

				rCritical(threadID) << "Warning: Could not evaluate the function" << function->objectName() << ".";

				if (result < -1) {					
					// This means that for non-convergence, the function could not be evaluated. So it is assumed that the system has failed. Thus, the function value is set to some number less than Zero, e.g. -1.0.
					functionValue = -1.0;
					if (theAccumulator.data()->inherits("RHistogramAccumulator")) {			
						rCritical(threadID) << "WARNING: Non-convergence in this sample. The sample is discarded.";
						convergence = false;
						return endAnalysis(x, dfdMean_1_f, dfdStdv_1_f, -1);
					} else {
						rCritical(threadID) << "WARNING: Assuming failure in this sample due to non-convergence.";
					}
				} else {
					// This means that for some reason other than non-convergence, the function could not be evaluated. So the sampling is terminated.
					rCritical(threadID) << "Please correct the error and try again.";
					return endAnalysis(x, dfdMean_1_f, dfdStdv_1_f, -1);
				}	
			}
			
			// Appending the sample result for this function to the list 
			sampleResult << functionValue;

			/*// For generation of random database
			QString str;
			for (int i = 0; i < theRandomVariableList.count(); i++) {
				str += QString("%1").arg(theRandomVariableList[i]->getCurrentValue(), 0, 'g', 10) + ",";
			}
			str += QString("%1").arg(functionValue, 0, 'g', 10);

			rDebug() << qPrintable(str);*/
		}


		// Sensitivity implementations
		// Compute sensitivity wrt to Mean or Stdv if demanded
		if (theComputeRandomVariableSensitivities && convergence) {
			int status = theTransformer.data()->computeJointPDFSensitivityToDistributionParameters(x, u, theSensitivityRandomVariableList, dfdMean_1_f, dfdStdv_1_f, theNeglectCorrelationInSensitivity, threadID);
			if (status) {
				rCritical(threadID) << "Error: Could not compute sensitivity of the joint PDF with respect to the mean or the standard deviation.";
				return endAnalysis(x, dfdMean_1_f, dfdStdv_1_f, -2);
			}
		}


		// Accumulate the result of this sample (if the continueCondition flag is still true)

		if (convergence) {


			// Sensitivity implementations
			// Allocating the vectors to store final sensitivity results
			QVector<double> sensitivityResultMeanVector;
			QVector<double> sensitivityResultStdvVector;
			sensitivityResultMeanVector.resize(SRVSize);
			sensitivityResultStdvVector.resize(SRVSize);

			if  (theComputeRandomVariableSensitivities) {
				for (int k = 0; k < SRVSize; k++) {
					sensitivityResultMeanVector[k] = gsl_vector_get(dfdMean_1_f, k);
					sensitivityResultStdvVector[k] = gsl_vector_get(dfdStdv_1_f, k);
				}
			}

			
			result = theAccumulator.data()->accumulate(sampleResult, u, sensitivityResultMeanVector, sensitivityResultStdvVector, threadID);

			if (result < 0) {

				rCritical(threadID) << "Error: Could not accumulate the results of this sample.";
				return endAnalysis(x, dfdMean_1_f, dfdStdv_1_f, -1);
			}

			// Check for continuing condition from the accumulator
			continueConditionInThread = theAccumulator.data()->getContinueFlag();
		}

		if (isFirstSimulation)
			isFirstSimulation = false;


		// Sensitivity implementations
		/*////Temprory: To comapare calculated df/dMean with perturbation result
		RContinuousRandomVariable *RV = qobject_cast<RContinuousRandomVariable *>(theRandomVariableList[0]);
		double fdd = theTransformer->fiii;
		double df = gsl_vector_get(dfdMean_1_f, 0) * fdd;
		double oMean = RV->getMeanValue();
		double oStdv = RV->getStandardDeviationValue();
		double h = oStdv/100.0;

		RV->setMeanValue(oMean+h);
	
		//result = theTransformer->transformPointToOriginalSpace(u, x);
		status = theTransformer -> computeJointPDFSensitivityToDistributionParameters(x, u, theRandomVariableList, theRandomVariableList, dfdMean_1_f, dfdStdv_1_f, theNeglectCorrelationInSensitivity);

		double fddp=theTransformer->fiii;
		double dfp = (fddp-fdd)/h;

		double differ = (  df - dfp)/dfp*100;

		RV->setMeanValue(oMean);
		/////////////////////////////////////////////////////////////////////////////*/


		if (threadID == 0){
			QCoreApplication::processEvents();
		}
	}
	QVector<double> sampleResult1;
	QVector<double> sensitivityResultMeanVector1;
	QVector<double> sensitivityResultStdvVector1;
	sampleResult1.resize(0);
	sensitivityResultMeanVector1.resize(3);
	sensitivityResultStdvVector1.resize(0);
	 theAccumulator.data()->accumulate(sampleResult1, u, sensitivityResultMeanVector1, sensitivityResultStdvVector1, threadID);
	// end of sampling in one thread

	return endAnalysis(x, dfdMean_1_f, dfdStdv_1_f, 0);
}

int RSamplingAnalyzer::endAnalysis(gsl_vector* x, gsl_vector* dfdMean_1_f, gsl_vector* dfdStdv_1_f, int errorCode){

	gsl_vector_free(x);

	if (dfdMean_1_f)
		gsl_vector_free(dfdMean_1_f);
	
	if (dfdStdv_1_f)
		gsl_vector_free(dfdStdv_1_f);

	return errorCode;
}

void RSamplingAnalyzer::initializeForParallelComputation(int threadID){

	// initializing transformer
	theTransformer.data()->initializeForParallelComputation(threadID);

	for (int i = 0; i < theFunctionList.count(); ++i) {
		theFunctionList[i].data()->initializeForParallelComputation(threadID);
	}
}

void RSamplingAnalyzer::resetAfterParallelComputation(){

	// initializing all the contributed functions
	for (int i = 0; i < theFunctionList.count(); ++i) {
		theFunctionList[i].data()->resetAfterParallelComputation();
	}
}

bool RSamplingAnalyzer::canBeRunParallelly(){

	// checking properties of type QObject*
	if (!(canAllQObjectStarPropertiesBeRunParallelly())){
		rDebug() << "Hence, the Sampling Analyzer cannot be run using parallel processing.";
		return false;
	}

	// checking contributed functions
	for (int i = 0; i < theFunctionList.count(); ++i) {
		if (!theFunctionList[i].data()->canBeRunParallelly()){
			rDebug() << "Hence, the Sampling Analyzer cannot be run using parallel processing.";
			return false;
		}
	}

	return true;
}
