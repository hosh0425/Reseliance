// Last modified by Mojtaba on June 30, 2012

#ifndef RFunction_H
#define RFunction_H

#include "RObject.h"
#include "RParallelComputerInterface.h"

#include "muParser.h"

#include <gsl/gsl_matrix.h>

class RDomain;
class RModel;
class RRandomVariable;
class RDecisionVariable;
class RConstant;
class RResponse;
class RParameter;

class QMutex;
class QReadWriteLock;
class RParallelProcessor;

class RFunction : public RObject, public RParallelComputerInterface
{
	Q_OBJECT

	// Sets the mathematical expression of the function
	Q_PROPERTY(QString Expression READ getExpression WRITE setExpression)

	// Sets the method for computing the gradient of function; options are FiniteDifference and DDM
	Q_PROPERTY(RGradientAnalysisType GradientAnalysisType READ getGradientAnalysisType WRITE setGradientAnalysisType)	

	// Read-only meta-property that shows the current value of the function
	Q_PROPERTY(double FunctionValue READ getFunctionValue)

	// Enum for the method of gradient computation
	Q_ENUMS(RGradientAnalysisType)

public:
	RFunction(QObject *parent, QString name);
	virtual ~RFunction();

	// Initializes all of the contributed models and parameters 
	void initializeForParallelComputation(int threadID);

	// Resets all of the contributed models and parameters 
	void resetAfterParallelComputation();

	// Returns true if all contributed models and parameters are parallelized
	bool canBeRunParallelly();

	// Declaring the RGradientAnalysisType enum values
	enum RGradientAnalysisType{FiniteDifference, DDM};

	// getter/setter methods for meta-properties
	QString getExpression() const;
	void setExpression(QString value);
	RGradientAnalysisType getGradientAnalysisType() const;
	void setGradientAnalysisType(RGradientAnalysisType value);

	// Returns the function value
	double getFunctionValue() const;

	// returns the function value in the given threadID ($MTh$)
	double getFunctionValue(int threadID);

	// Returns the gradient of the function
	gsl_vector *getGradient() const;

	// Returns the a list of random variables that implicitly and explicitly affect the function
	QList<RRandomVariable *> *getRandomVariableList();

	// Returns the a list of models that affect the function
	QList<RModel *> getModelList();

	// Returns the a list of parameters that explicitly appear in the function expression
	QList<RParameter *> getExplicitParameterList();

	// Returns the a list of decision variables that implicitly and explicitly affect the function
	QList<RDecisionVariable *> getDecisionVariableList();

	// Returns the a list of constants that implicitly and explicitly affect the function
	QList<RConstant *> getConstantList();

	// Returns the a list of responses that implicitly and explicitly affect the function
	QList<RResponse *> getResponseList();

	// Resets the function by clearing all lists of parameters and deleting the muParser
	void reset();

	// Clears the DDM QMap objects
	void clearDDMMaps();

	// Parses (tokenuzes) the function and populates the lists of parameters
	int populateLists();

	// Evaluates the function in the given threadID ($MTh$)
	int evaluateFunction(bool evaluateGradientWithFunction, RObject::RParameterType parameterType, int threadID);

	// Evaluates the gradient of the function
	int evaluateGradient(RObject::RParameterType parameterType);

	// Establishes a list of models that are affected by the passedParameter
	void evaluateDependencies(RParameter *passedParameter);

	// Updates the isAnalyzed flag of responses of models that are affected by the passedParameter, in the given threadID ($MTh$)
	void updateAnalysisStatus(RParameter *passedParameter, int threadID);

	// Overloaded method that updates the isAnalyzed flag of responses of models that are affected by the passedParameterList, in the given threadID ($MTh$)
	void updateAnalysisStatus(QList<RParameter *> passedParameterList, int threadID);

	// Overloaded method that updates the isAnalyzed flag of responses of models that are affected by the passedRandomVariableList, in the given threadID ($MTh$)
	void updateAnalysisStatus(QList<RRandomVariable *> passedRandomVariableList, int threadID);

	// Overloaded method that sets the isAnalyzed flag of responses of ALL models that affect this function, , in the given threadID ($MTh$)
	void updateAnalysisStatus(int threadID);

	// Replaces the list of responses that affect this function by the newResponseList; this method is currently used by RLoadCoincidenceAnalyzer to modify lists for different hazard
	void setResponseList(QList<RResponse *> newResponseList);
	
	// Replaces the list of parameters that explicitly appear in this function by the newParameterList; this method is currently used by RLoadCoincidenceAnalyzer to modify lists for different hazard
	void setExplicitParameterList(QList<RParameter *> newParameterList);

	// Replaces the list of models that affect this function by the newModelList; this method is currently used by RLoadCoincidenceAnalyzer to modify lists for different hazard
	void setModelList(QList<RModel *> newModelList);

private:

	// a wrapper around finite difference method to give to the parallel computer
	int static finiteDifferenceWrapper(void * theObject, void * inputArgs[], int threadID);
	
	// Auxiliary method that carries out finite difference, , in the given threadID ($MTh$)
	void finiteDifference(RParameter *parameter, RObject::RParameterType parameterType, double perturbationFactor, double originalFunctionValue, double *derivative, int *status, int threadID);

	// Auxiliary method that is carried out in each thread and calculates function's gradient to as many parameters as possible using finite difference (If possible parallelly), generalParameterIndex is a reference to a shared variable between threads that specifies next parameter to calculate gradient with respect to it
	int calculateFunctionGradientWithFD(QList<RParameter *> * parameterList, int * generalParameterIndex, RObject::RParameterType parameterType, double perturbationFactor, double originalFunctionValue, int threadID = 0);

	// Returns a list containing all the models and parameters that effect the function
	QLinkedList<RObject *> getTheListOfAllContributingObjects();

	// Data members for meta-properties
	QString theExpression;
	RGradientAnalysisType theGradientAnalysisType;

	bool theDisplayGradient;

	// Pointer to the domain object
	RDomain *theDomain;

	// an array of muParser data member that parses (tokenizes) the function expression in each threadID ($MTh$)
	mu::Parser **theParser;

	// Data member that stores the function value in each threadID ($MTh$)
	double * theValue;

	// Data member that stores the gradient of the function
	gsl_vector *theGradient;

	// Data members for parameter and model lists
	QList<RModel *> theModelList;
	QList<RParameter *> theExplicitParameterList;
	QList<RRandomVariable *> theRandomVariableList;
	QList<RDecisionVariable *> theDecisionVariableList;
	QList<RConstant *> theConstantList;
	QList<RResponse *> theResponseList;	

	// Backup of the list of models that affect this function 
	QList<RModel *> backupModelList;	

	// Data member that relates each parameter with a list of models it affects
	QMap<RParameter *, QList<RModel *> > theDependencyHash;

	// DDM QMap for responses that explicitly appear in the function expression
	QMap<RParameter *, int> theExplicitResponseDependenceMap;

	// a lock for preventing execution of populateList in all threads ($MTh$)
	QMutex * populateListLock;

	// a hash of pointer to RParameter to a unique lock for preventing calculation of dependencies of a single parameter in all threads ($MTh$)
	QMap<RParameter *, QReadWriteLock * > theDependencyCalculationLockHash;

	// a lock for safely read and increase parameter index in different threads while calculating function gradient using finite difference
	QMutex * finiteDifferenceParameterIndexLock;

	// a reference to parallelComputer class to handle most of the stuff about parallelism
	RParallelProcessor * finiteDifferenceParallelComputer;
};

#endif // RFunction_H
