// Last modified by Mojtaba on June 30, 2012

#ifndef RContinuousDecisionVariable_H
#define RContinuousDecisionVariable_H

#include "RDecisionVariable.h"
#include "RPointer.h"//salarifard

class RContinuousDecisionVariable : public RDecisionVariable
{
	Q_OBJECT

	// Sets the initial value as a parameter object or a double
	Q_PROPERTY(QString InitialValue READ getInitialValue WRITE setInitialValue)

	// Sets the upper bound as a parameter object or a double
	Q_PROPERTY(QString UpperBound READ getUpperBound WRITE setUpperBound)

	// Sets the lower bound as a parameter object or a double
	Q_PROPERTY(QString LowerBound READ getLowerBound WRITE setLowerBound)

	// Sets the cost of increasing the decision variable by one unit as a parameter object or a double
	Q_PROPERTY(QString IncrementalCost READ getIncrementalCost WRITE setIncrementalCost)

	// Read-only meta-property that shows the derivative of the realiability index wrt. the cost associated with the decision variable
	Q_PROPERTY(double BetaSensitivityWrtCost READ getBetaSensitivityWrtCost);

	// Read-only meta-property that shows the derivative of the failure probability wrt. the cost associated with the decision variable
	Q_PROPERTY(double PfSensitivityWrtCost READ getPfSensitivityWrtCost);

public:
	RContinuousDecisionVariable(QObject *parent, QString name);
	~RContinuousDecisionVariable();

	// getter/setter methods for meta-properties ,DO NOT USE THESE METHODS IN PARALLEL COMPUTATION, use the versions with threadID instead
	QString getInitialValue();
	void setInitialValue(QString value);
	QString getUpperBound();
	void setUpperBound(QString value);
	QString getLowerBound();
	void setLowerBound(QString value);
	QString getIncrementalCost();
	void setIncrementalCost(QString value);
	
	double getBetaSensitivityWrtCost();
	void setBetaSensitivityWrtCost(double value);
	double getPfSensitivityWrtCost();
	void setPfSensitivityWrtCost(double value);

	// thread specific setters and getters
	double getInitialValueValue(int threadID);
	double getUpperBoundValue(int threadID); 
	double getLowerBoundValue(int threadID);
	double getIncrementalCostValue(int threadID);
	void setInitialValue(double value, int threadID);
	void setUpperBound(double value,int threadID);
	void setLowerBound(double value,int threadID);
	void setIncrementalCost(double value,int threadID);

	// Returns a list of parameters that are input to the decision variable as lower bound, upper bound, etc.
	QList<RParameter *> getParameterList();

	// Copies the value at index 0 to the index of the given threadID for data members that are changed to array due to parallelization. Each subclass must also call the method of their superclass
	void initializeForParallelComputation(int threadID);

	// returns true as this parameter is parallelized
	bool canBeRunParallelly();

private:
	// Data members for meta-properties
	// array of double holds the double value in each threadID ($MTh$)
	RPointer<RParameter> theInitialValueParameter;
	double * theInitialValue;
	RPointer<RParameter> theUpperBoundParameter;
	double * theUpperBound;
	RPointer<RParameter> theLowerBoundParameter;
	double * theLowerBound;
	RPointer<RParameter> theIncrementalCostParameter;
	double * theIncrementalCost;

	// Data members for sensitivities
	double theBetaSensitivityWrtCost;
	double thePfSensitivityWrtCost;

};

#endif // RContinuousDecisionVariable_H
