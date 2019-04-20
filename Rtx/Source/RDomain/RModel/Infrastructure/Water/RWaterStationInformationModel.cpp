#include "RWaterStationInformationModel.h"

#include "RParameter.h"
#include "RResponse.h"
#include "RTime.h"

RWaterStationInformationModel::RWaterStationInformationModel(QObject *parent, QString name)
	: RModel(parent, name)
{
	//theLocation = 0;
	//theTimeOfConstruction = 0;


	theType = RWaterStationInformationModel::Small;
	theComponents = RWaterStationInformationModel::Anchored;

}
RWaterStationInformationModel::~RWaterStationInformationModel()
{

}

QObject * RWaterStationInformationModel::getLocation() const
{
	return theLocation.data();
}

void RWaterStationInformationModel::setLocation(QObject *value)
{
	theLocation = qobject_cast<RParameter *>(value);

	// Verification that the user has given an RLocationResponse or an RLocation
	parameterToLocation(theLocation.data());
}

QObject * RWaterStationInformationModel::getTimeOfConstruction() const
{
	return theTimeOfConstruction.data();
}

void RWaterStationInformationModel::setTimeOfConstruction(QObject *value)
{
	theTimeOfConstruction = qobject_cast<RParameter *>(value);
}

RWaterStationInformationModel::RType RWaterStationInformationModel::getType() const
{
	return theType;
}

void RWaterStationInformationModel::setType(RWaterStationInformationModel::RType value)
{
	theType = value;
}

RWaterStationInformationModel::RComponents RWaterStationInformationModel::getComponents() const
{
	return theComponents;
}

void RWaterStationInformationModel::setComponents(RWaterStationInformationModel::RComponents value)
{
	theComponents = value;
}

int RWaterStationInformationModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
{
	return 0;
}

bool RWaterStationInformationModel::canBeRunParallelly() {
	return true;
}
