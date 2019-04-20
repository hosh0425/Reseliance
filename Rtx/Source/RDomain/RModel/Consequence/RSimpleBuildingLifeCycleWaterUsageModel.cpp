#include "RSimpleBuildingLifeCycleWaterUsageModel.h"

#include "RParameter.h"
#include "RResponse.h"

RSimpleBuildingLifeCycleWaterUsageModel::RSimpleBuildingLifeCycleWaterUsageModel(QObject *parent, QString &name)
	: RModel(parent, name)
{
	// Initializing the pointer data members
	/* //$$ 
	thePreUseWIntensity = 0;
	theNumberStoreys = 0;
	theFootprintArea = 0;
	theAnnualWIntensity = 0;
	theDesignLife = 0;
	*/

	// Instantiating the output response
	theDomain->createObject(objectName() + "Response", "RGenericResponse");
	// Setting theResponse pointer
	theWaterUsageCoarseResponse = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	// Setting the "model" property of theResponse to be "this" object
	theWaterUsageCoarseResponse->setModel(this);
}

RSimpleBuildingLifeCycleWaterUsageModel::~RSimpleBuildingLifeCycleWaterUsageModel()
{

}

QObject * RSimpleBuildingLifeCycleWaterUsageModel::getPreUseWIntensity() const
{
	return thePreUseWIntensity.data();
}

void RSimpleBuildingLifeCycleWaterUsageModel::setPreUseWIntensity(QObject *value)
{
	thePreUseWIntensity = qobject_cast<RParameter *>(value);
}

QObject * RSimpleBuildingLifeCycleWaterUsageModel::getNumberStoreys() const
{
	return theNumberStoreys.data();
}

void RSimpleBuildingLifeCycleWaterUsageModel::setNumberStoreys(QObject *value)
{
	theNumberStoreys = qobject_cast<RParameter *>(value);
}

QObject * RSimpleBuildingLifeCycleWaterUsageModel::getFootprintArea() const
{
	return theFootprintArea.data();
}

void RSimpleBuildingLifeCycleWaterUsageModel::setFootprintArea(QObject *value)
{
	theFootprintArea = qobject_cast<RParameter *>(value);
}

QObject * RSimpleBuildingLifeCycleWaterUsageModel::getAnnualWIntensity() const
{
	return theAnnualWIntensity.data();
}

void RSimpleBuildingLifeCycleWaterUsageModel::setAnnualWIntensity(QObject *value)
{
	theAnnualWIntensity = qobject_cast<RParameter *>(value);
}

QObject * RSimpleBuildingLifeCycleWaterUsageModel::getDesignLife() const
{
	return theDesignLife.data();
}

void RSimpleBuildingLifeCycleWaterUsageModel::setDesignLife(QObject *value)
{
	theDesignLife = qobject_cast<RParameter *>(value);
}






int RSimpleBuildingLifeCycleWaterUsageModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
{	
	// Extract the input parameter values
	double wpud = thePreUseWIntensity.data()->getCurrentValue(threadID); // i.e. 20 [MPa]
	double ns = theNumberStoreys.data()->getCurrentValue(threadID); // i.e. 200 [m^3]
	double af = theFootprintArea.data()->getCurrentValue(threadID);
	double wd = theAnnualWIntensity.data()->getCurrentValue(threadID);
	double td = theDesignLife.data()->getCurrentValue(threadID); // i.e. 3000 [kg/year]


	double response = ns*af*(wpud+td*wd);

	// Updating the response current value
	theWaterUsageCoarseResponse->setCurrentValue(response,threadID);

	return 0;
}

bool RSimpleBuildingLifeCycleWaterUsageModel::canBeRunParallelly(){
	return true;
}