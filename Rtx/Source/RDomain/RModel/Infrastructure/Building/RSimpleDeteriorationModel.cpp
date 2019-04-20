#include "RSimpleDeteriorationModel.h"

#include "RParameter.h"
#include "RResponse.h"
#include "RBuildingInformationModel.h"

#include <math.h>

RSimpleDeteriorationModel::RSimpleDeteriorationModel(QObject *parent, QString name)
	: RModel(parent, name)
{
	//theIntactValue = 0;
	//$$ theTime = 0;
	//$$ theDeteriorationRate = 0;
	
	theDomain->createObject(objectName() + "Response", "RGenericResponse");
	theDeteriorationFactor = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theDeteriorationFactor->setModel(this);
}

RSimpleDeteriorationModel::~RSimpleDeteriorationModel()
{

}

/*QObject * RSimpleDeteriorationModel::getIntactValue() const	
{	
	return theIntactValue;
}	
	
void RSimpleDeteriorationModel::setIntactValue(QObject *value)	
{	
	theIntactValue = qobject_cast<RParameter *>(value);
}*/

QObject * RSimpleDeteriorationModel::getTime() const	
{	
	return theTime.data();
}	
	
void RSimpleDeteriorationModel::setTime(QObject *value)	
{	
	theTime = qobject_cast<RParameter *>(value);
}	

QObject * RSimpleDeteriorationModel::getDeteriorationRate() const	
{	
	return theDeteriorationRate.data();
}	
	
void RSimpleDeteriorationModel::setDeteriorationRate(QObject *value)	
{	
	theDeteriorationRate = qobject_cast<RParameter *>(value);
}	

int RSimpleDeteriorationModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
{
	//double intactValue = theIntactValue->getCurrentValue();
	double currentTime = theTime.data()->getCurrentValue(threadID);
	double deteriorationRate = theDeteriorationRate.data()->getCurrentValue(threadID);

	double factor = 1.0;
	if (currentTime > theInitialTime) {
		//factor = pow(1 + deteriorationRate, -(currentTime - theInitialTime) / 365.0);
		factor = exp(-deteriorationRate * (currentTime - theInitialTime) / 365.0);
	}


	theDeteriorationFactor->setCurrentValue(factor, threadID);

	if (evaluateDDM) {
		// IMPLEMENT
	}

	return 0;
}

void RSimpleDeteriorationModel::reset()
{
	theInitialTime = theTime.data()->getCurrentValue();
}

bool RSimpleDeteriorationModel::canBeRunParallelly(){
	return true;
}