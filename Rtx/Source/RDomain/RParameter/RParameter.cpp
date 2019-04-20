#include "RParameter.h"
#include "RDomain.h"

RParameter::RParameter(QObject *parent, QString name)
	: RObject(parent, name)
{

	/* $MTh$ initializing */

	int arraySizeForParallelizedObject = getArraySizeForParallelizedObject();
	theCurrentValue = new double[arraySizeForParallelizedObject];
	
	theCurrentValue[0] = 0;
}

RParameter::~RParameter()
{
	delete [] theCurrentValue;
}

double RParameter::getCurrentValue(int threadID) const
{
	return theCurrentValue[threadID];
}

void RParameter::setCurrentValue(double value)
{
	theCurrentValue[0] = value;
}

void RParameter::setCurrentValue(double value, int threadID)
{
	theCurrentValue[threadID] = value;
}

double * RParameter::getCurrentValuePointer(int threadID)
{
	return &theCurrentValue[threadID];
}

void RParameter::initializeForParallelComputation(int threadID){

	// initialize in superclass
	RObject::initializeForParallelComputation(threadID);

	theCurrentValue[threadID] = theCurrentValue[0];
}