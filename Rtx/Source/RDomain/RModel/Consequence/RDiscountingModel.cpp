#include "RDiscountingModel.h"

#include "RParameter.h"
#include "RResponse.h"

#include <math.h>

RDiscountingModel::RDiscountingModel(QObject *parent, QString name)
	: RModel(parent, name)
{
/* //$$ 
	theFutureValue = 0;
	theTime = 0;
	theAnnualInterestRate = 0;
*/

	theDomain->createObject(objectName() + "Response", "RGenericResponse");
	thePresentValue = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	thePresentValue->setModel(this);
}

RDiscountingModel::~RDiscountingModel()
{

}

QObject * RDiscountingModel::getFutureValue() const	
{	
	return theFutureValue.data();
}	
	
void RDiscountingModel::setFutureValue(QObject *value)	
{	
	theFutureValue = qobject_cast<RParameter *>(value);
}	

QObject * RDiscountingModel::getTime() const	
{	
	return theTime.data();
}	
	
void RDiscountingModel::setTime(QObject *value)	
{	
	theTime = qobject_cast<RParameter *>(value);
}	

QObject * RDiscountingModel::getAnnualInterestRate() const	
{	
	return theAnnualInterestRate.data();
}	
	
void RDiscountingModel::setAnnualInterestRate(QObject *value)	
{	
	theAnnualInterestRate = qobject_cast<RParameter *>(value);
}	

int RDiscountingModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
{
	double futureValue = theFutureValue.data()->getCurrentValue(threadID);
	double currentTime = theTime.data()->getCurrentValue(threadID);
	double interestRate = theAnnualInterestRate.data()->getCurrentValue(threadID);

	double futureDiscountFactor = 1.0;
	if (currentTime > theInitialTime) {
		//futureDiscountFactor = pow(1 + interestRate, -(currentTime - theInitialTime) / 365.0);
		futureDiscountFactor = exp(-interestRate * (currentTime - theInitialTime) / 365.0);
	}

	double presentValue = futureValue * futureDiscountFactor;

	thePresentValue->setCurrentValue(presentValue, threadID);

	if (evaluateDDM) {
		// IMPLEMENT
	}

	return 0;
}

void RDiscountingModel::reset()
{
	theInitialTime = theTime.data()->getCurrentValue();
}

bool RDiscountingModel::canBeRunParallelly(){
	return true;
}
