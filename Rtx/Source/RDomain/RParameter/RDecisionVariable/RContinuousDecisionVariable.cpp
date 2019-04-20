#include "RContinuousDecisionVariable.h"

RContinuousDecisionVariable::RContinuousDecisionVariable(QObject *parent, QString name)
	: RDecisionVariable(parent, name)
{
/* //$$ 
	theInitialValueParameter = 0;
	theUpperBoundParameter = 0;
	theLowerBoundParameter = 0;
	theIncrementalCostParameter = 0;
*/

	theBetaSensitivityWrtCost = 0;
	thePfSensitivityWrtCost = 0;

	// defining array data members
	int arraySizeForParallelizedObject = getArraySizeForParallelizedObject();

	theInitialValue = new double[arraySizeForParallelizedObject];
	theUpperBound = new double[arraySizeForParallelizedObject];
	theLowerBound = new double[arraySizeForParallelizedObject];
	theIncrementalCost = new double[arraySizeForParallelizedObject];

	// setting default values for index=0
	theInitialValue[0] = 0;
	theUpperBound[0] = 0;
	theLowerBound[0] = 0;
	theIncrementalCost[0] = 1;
}

RContinuousDecisionVariable::~RContinuousDecisionVariable()
{
	delete [] theInitialValue;
	delete [] theUpperBound;
	delete [] theLowerBound;
	delete [] theIncrementalCost;
}

QString RContinuousDecisionVariable::getInitialValue()
{
	return getStringParameter(&theInitialValue[0], theInitialValueParameter.data());
}

void RContinuousDecisionVariable::setInitialValue(QString value)
{
	setStringParameter(value, &theInitialValue[0], &theInitialValueParameter);
	theCurrentValue[0] = getInitialValueValue(0);
}

QString RContinuousDecisionVariable::getUpperBound()
{
	return getStringParameter(&theUpperBound[0], theUpperBoundParameter.data());
}

void RContinuousDecisionVariable::setUpperBound(QString value)
{
	setStringParameter(value, &theUpperBound[0], &theUpperBoundParameter);
}

QString RContinuousDecisionVariable::getLowerBound()
{
	return getStringParameter(&theLowerBound[0], theLowerBoundParameter.data());
}

void RContinuousDecisionVariable::setLowerBound(QString value)
{
	setStringParameter(value, &theLowerBound[0], &theLowerBoundParameter);
}

QString RContinuousDecisionVariable::getIncrementalCost()
{
	return getStringParameter(&theIncrementalCost[0], theIncrementalCostParameter.data());
}

void RContinuousDecisionVariable::setIncrementalCost(QString value)
{
	setStringParameter(value, &theIncrementalCost[0], &theIncrementalCostParameter);
}


double RContinuousDecisionVariable::getInitialValueValue(int threadID)
{
	return getDoubleParameter(&theInitialValue[threadID], theInitialValueParameter.data());
}

double RContinuousDecisionVariable::getUpperBoundValue(int threadID)
{
	return getDoubleParameter(&theUpperBound[threadID], theUpperBoundParameter.data());
}

double RContinuousDecisionVariable::getLowerBoundValue(int threadID)
{
	return getDoubleParameter(&theLowerBound[threadID], theLowerBoundParameter.data());
}

double RContinuousDecisionVariable::getIncrementalCostValue(int threadID)
{
	return getDoubleParameter(&theIncrementalCost[threadID], theIncrementalCostParameter.data());
}

void RContinuousDecisionVariable::setInitialValue(double value,int threadID){

	setDoubleParameter(value,&theInitialValue[threadID], theInitialValueParameter.data(),threadID);
}

void RContinuousDecisionVariable::setUpperBound(double value,int threadID){

	setDoubleParameter(value,&theUpperBound[threadID], theUpperBoundParameter.data(),threadID);
}

void RContinuousDecisionVariable::setLowerBound(double value, int threadID){

	setDoubleParameter(value,&theLowerBound[threadID], theLowerBoundParameter.data(),threadID);
}

void RContinuousDecisionVariable::setIncrementalCost(double value,int threadID){

	setDoubleParameter(value,&theIncrementalCost[threadID], theIncrementalCostParameter.data(),threadID);
}


double RContinuousDecisionVariable::getBetaSensitivityWrtCost()
{
	return theBetaSensitivityWrtCost;
}

void RContinuousDecisionVariable::setBetaSensitivityWrtCost(double value)
{
	theBetaSensitivityWrtCost = value;
}

double RContinuousDecisionVariable::getPfSensitivityWrtCost()
{
	return thePfSensitivityWrtCost;
}

void RContinuousDecisionVariable::setPfSensitivityWrtCost(double value)
{
	thePfSensitivityWrtCost = value;
}


QList<RParameter *> RContinuousDecisionVariable::getParameterList()
{
	QList<RParameter *> theParameterList;
	if (theInitialValueParameter) {
		theParameterList << theInitialValueParameter.data();
	}
	if (theUpperBoundParameter) {
		theParameterList << theUpperBoundParameter.data();
	}
	if (theLowerBoundParameter) {
		theParameterList << theLowerBoundParameter.data();
	}
	if (theIncrementalCostParameter) {
		theParameterList << theIncrementalCostParameter.data();
	}
	return theParameterList;
}

void RContinuousDecisionVariable::initializeForParallelComputation(int threadID){

	// cal the method in superclass
	RDecisionVariable::initializeForParallelComputation(threadID);

	theInitialValue[threadID] = theInitialValue[0];
	theUpperBound[threadID] = theUpperBound[0];
	theLowerBound[threadID] = theLowerBound[0];
	theIncrementalCost[threadID] = theIncrementalCost[0];
}

bool RContinuousDecisionVariable::canBeRunParallelly(){
	return true;
}