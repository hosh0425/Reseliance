// Last modified by Mojtaba on June 30, 2012

#ifndef RParameter_H
#define RParameter_H

#include "RObject.h"
#include "RPointer.h" //salarifard

class RParameter : public RObject
{
	Q_OBJECT

public:
	RParameter(QObject *parent, QString name);
	virtual ~RParameter();
	
	// Returns the current realization of the parameter (DO NOT USE THIS METHOD WITHOUT THREADID INPUT)
	virtual double getCurrentValue(int threadID = 0) const;

	// Sets the current realization of the parameter to the passed value (DO NOT USE THIS METHOD, use the overloaded version with threadID input instead)
	virtual void setCurrentValue(double value);

	// Sets the current realization of the parameter to the passed value, in the given threadID ($MTh$)
	virtual void setCurrentValue(double value, int threadID);

	// Returns a pointer to the current value double in the given threadID, used by muParser ($MTh$)
	double *getCurrentValuePointer(int threadID);

	// Copies the value at index 0 to the index of the given threadID for data members that are changed to array due to parallelization. Each subclass must also call the method of their superclass
	virtual void initializeForParallelComputation(int threadID);

protected:
	
	// Data member that store the current realization of the parameter, in each threadID ($MTh$)
	double * theCurrentValue;

private:
	
};

//Q_DECLARE_METATYPE(RParameter *);

#endif // RParameter_H
