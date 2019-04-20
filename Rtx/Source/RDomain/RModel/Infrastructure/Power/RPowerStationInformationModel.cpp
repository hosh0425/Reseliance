#include "RPowerStationInformationModel.h"

#include "RParameter.h"
#include "RResponse.h"
#include "RTime.h"

RPowerStationInformationModel::RPowerStationInformationModel(QObject *parent, QString name)
	: RModel(parent, name)
{
	//theLocation = 0;
	//theTimeOfConstruction = 0;

	// The default replacement values are in Table 15.17 of FEMA
	theReplacementCost = 10000;
	theType = RPowerStationInformationModel::LowVoltage;
	theComponents = RPowerStationInformationModel::Anchored;

}
RPowerStationInformationModel::~RPowerStationInformationModel()
{

}

QObject * RPowerStationInformationModel::getLocation() const
{
	return theLocation.data();
}

void RPowerStationInformationModel::setLocation(QObject *value)
{
	theLocation = qobject_cast<RParameter *>(value);

	// Verification that the user has given an RLocationResponse or an RLocation
	parameterToLocation(theLocation.data());
}

QObject * RPowerStationInformationModel::getTimeOfConstruction() const
{
	return theTimeOfConstruction.data();
}

void RPowerStationInformationModel::setTimeOfConstruction(QObject *value)
{
	theTimeOfConstruction = qobject_cast<RParameter *>(value);
}

double RPowerStationInformationModel::getReplacementCost() const
{
	return theReplacementCost;
}

void RPowerStationInformationModel::setReplacementCost(double value)
{
	theReplacementCost = value;
}

RPowerStationInformationModel::RType RPowerStationInformationModel::getType() const
{
	return theType;
}

void RPowerStationInformationModel::setType(RPowerStationInformationModel::RType value)
{
	theType = value;
}

RPowerStationInformationModel::RComponents RPowerStationInformationModel::getComponents() const
{
	return theComponents;
}

void RPowerStationInformationModel::setComponents(RPowerStationInformationModel::RComponents value)
{
	theComponents = value;
}

int RPowerStationInformationModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
{
	return 0;
}

bool RPowerStationInformationModel::canBeRunParallelly() {
	return true;
}
