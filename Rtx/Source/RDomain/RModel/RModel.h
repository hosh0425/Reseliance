// Last modified by Mojtaba on June 30, 2012

#ifndef RModel_H
#define RModel_H

#include "RObject.h"
#include "RDomain.h"
#include "RPointer.h" //salarifard

class RParameter;
class RRandomVariable;
class RDecisionVariable;
class RConstant;
class RResponse;
class RLocation;

class QReadWriteLock;
class QMutex;

class RModel : public RObject
{
	Q_OBJECT

	// Indicates whether or not the output for the model should be desplayed
	Q_PROPERTY(bool DisplayOutput READ getDisplayOutput WRITE setDisplayOutput USER true)

public:
	RModel(QObject *parent, QString name);
	virtual ~RModel();

	// getter/setter methods for meta-properties
	bool getDisplayOutput();
	void setDisplayOutput(bool value);

	// Runs the model analysis in the given threadID, by first calling the runAnalysis for upstream responses ($MTh$)
	int runAnalysis(bool evaluateDDM, RObject::RParameterType parameterType, int threadID);

	// Evaluates the responses that flow upstream in the given threadID, i.e., enter the upstream model ($MTh$)
	virtual int setResponsesThatFlowUpstream(int threadID);

	// Specific implementation of the setObjectName, which sets the object name for the model and its auto-generated responses
	void setObjectName(const QString &name);

	// Indicates if the passed parameter is an input to the model
	bool contains(RParameter *value);
	


	// Returns a list of random variables that are input to the model
	QList<RRandomVariable *> getRandomVariableList();

	// Returns a list of decision variables that are input to the model
	QList<RDecisionVariable *> getDecisionVariableList();

	// Returns a list of constants that are input to the model
	QList<RConstant *> getConstantList();

	// Returns a list of responses from upstream models that are input to the model
	QList<RResponse *> getResponseList();

	// create a list of responses from ALL upstream models that are input to the model 
	void createUpstreamResponseList();

	// Returns a list of responses from ALL upstream models that are input to the model
	QList<RResponse *> getUpstreamResponseList();

	// Sets the isAnalyzed flag of all output responses of the model to false, in the given threadID ($MTh$)
	virtual void setResponseAnalyzed(bool analyzed, int threadID);

	// Clears the NULL parameters from the passed parameter list
	//void clearNullPointers(QList<RPointer<RParameter> > *passdedParameterList);

	// Invokes the operations that must be run before a new analysis, i.e., a new FORM analysis, starts
	virtual void reset();

	// Resets the history variables at the start of a new sample (scenario) in a scenario sampling analysis, in the given threadID ($MTh$)
	virtual void resetHistoryVariables(int threadID);

	// Updates the history variables at the end of each function evaluation within a scenario, in the given threadID ($MTh$)
	virtual void updateHistoryVariables(int threadID);

	// Compiles and returns a list of parameters that are input to the model
	virtual QList<RParameter *> getAllParametersList(bool *ok);
	
	// This method is only implemented in some hazard models, such as magnitude and location models; It returns the list of parameters (essentially random variables) that should be sampled every time the hazard model becomes on (in a scenario sampling analysis)
	virtual QList<RParameter *> getPhysicalParameterList();

	// Clears the DDM QMap objects
	void clearDDMMaps();

	// Returns the coefficients a, b, c, and d in the cubic polynomial ax^3+bx^2+cx+d that is used to smooth out the kinks in the model form
	int cubicSmooth(double x0, double epsilon, double y1, double y2, double dy1, double dy2, double *a, double *b, double *c, double *d);

	// Overloaded method that returns the smoothed function value (y) at point x
	int cubicSmooth(double x0, double epsilon, double y1, double y2, double dy1, double dy2, double x, double *y);

protected:
	// Evaluates the model in the given threadID ($MTh$) ; Return values:
	// -1~-9: The model evaluation has failed
	// -10: The DDM evaluation has failed
	// 1: The model HAS the implementation of DDM and both DDM and model have been evaluated successfully
	// 0: The model does NOT have the implementation of DDM and the model has been evaluated successfully
	virtual int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID) = 0;

	// Evaluates gradient of the model response wrt. input parameters by finite difference ; used when the function is trying to compute the gradeint by DDMs but the model does not have DDM implementations
	int evaluateModelGradient(RObject::RParameterType parameterType);

	// Calls the method on all of the input parameters to make sure they've all been initialized (specially location and time)
	virtual void initializeForParallelComputation(int threadID);

	// Pointer to the domain object
	RDomain *theDomain;

	// List of all input parameters
	QList<RParameter *> theAllParametersList;

	// List of all upstream responses
	QList<RResponse *> theUpstreamResponseList;

	// a hash of all input parameters
	QHash<RParameter*, bool> theAllParametersHash;

	// QMap object that relates the computed DDM sensitivities to input parameters for each output response
	QMap<RResponse *, QMap<RParameter *, double> > theDDMMap;

	// QMap object that stores the explicit and implicit dependencies of this model to parameters
	QMap<RParameter *, int> theDependenceMap;

private:
	
	// Data members for meta-properties
	// IMPORTANT : DO NOT USE THIS PROPERTY OUT OF SAFE AREA OF ITS LOCK... YOU CAN USE ITS GETTER
	bool theDisplayOutput;

	QReadWriteLock * theDisplayOutputLock;

	// for preventing more than one thread extracting the all parameter list
	QMutex * theAllParameterListLock;
};

#endif // RModel_H
