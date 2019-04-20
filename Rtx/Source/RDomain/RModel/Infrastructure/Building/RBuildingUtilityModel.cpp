#include "RBuildingUtilityModel.h"	

#include "RParameter.h"	
#include "RResponse.h"	
#include "RBuildingInformationModel.h"

#include <math.h>	

RBuildingUtilityModel::RBuildingUtilityModel(QObject *parent, QString name)
	: RModel(parent, name)
{
	// Instantiating the response object
	theDomain->createObject(objectName() + "PowerResponse", "RGenericResponse");
	theBuildingPowerUtility = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theBuildingPowerUtility->setModel(this);
	theBuildingPowerUtility->setCurrentValue(1.0);

	// Instantiating the response object
	theDomain->createObject(objectName() + "WaterResponse", "RGenericResponse");
	theBuildingWaterUtility = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theBuildingWaterUtility->setModel(this);
	theBuildingWaterUtility->setCurrentValue(1.0);
}

RBuildingUtilityModel::~RBuildingUtilityModel()
{

}

QObject * RBuildingUtilityModel::getBuildingInformationModel() const
{
	return theBuildingInformationModel.data();
}

void RBuildingUtilityModel::setBuildingInformationModel(QObject *value)
{
	theBuildingInformationModel = qobject_cast<RModel *>(value);
}

QObject * RBuildingUtilityModel::getPowerServiceability() const
{
	return thePowerServiceability.data();
}

void RBuildingUtilityModel::setPowerServiceability(QObject *value)
{
	thePowerServiceability = qobject_cast<RParameter *>(value);
}

QObject * RBuildingUtilityModel::getWaterServiceability() const
{
	return theWaterServiceability.data();
}

void RBuildingUtilityModel::setWaterServiceability(QObject *value)
{
	theWaterServiceability = qobject_cast<RParameter *>(value);
}

int RBuildingUtilityModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
{

	// Checking if all pointer properties are given by the user
	int status = this->checkProperties();
	// Error handling
	if (status < 0) {
		// Printing an error message to the output pane
		rCritical(threadID) << "Error in" << this->objectName() << ":"
			<< "Could not find all the passed objects";
		// Returning the error code
		return -1;
	}

	double powerUtility = thePowerServiceability.data()->getCurrentValue(threadID);
	double waterUtility = theWaterServiceability.data()->getCurrentValue(threadID);

	theBuildingPowerUtility->setCurrentValue(powerUtility, threadID);
	theBuildingWaterUtility->setCurrentValue(waterUtility, threadID);

	return 0;
}

bool RBuildingUtilityModel::canBeRunParallelly() {
	return true;
}