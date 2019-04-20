#include "RPowerStationInspectionTimeModel.h"	

#include "RParameter.h"	
#include "RResponse.h"	
#include "RPowerStationInformationModel.h"
#include <math.h>	

RPowerStationInspectionTimeModel::RPowerStationInspectionTimeModel(QObject *parent, QString name)
	: RModel(parent, name)
{
	// Initializing the data members
	//thePowerStationInformationModel = 0;
	//theInspectionTimeParameter = 0;
	theMonitoring = RPowerStationInspectionTimeModel::Visual;

	// Instantiating the response object
	theDomain->createObject(objectName() + "TimeResponse", "RGenericResponse");
	theInspectionTime = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theInspectionTime->setModel(this);

	theDomain->createObject(objectName() + "StatusResponse", "RGenericResponse");
	theInspectionStatus = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theInspectionStatus->setModel(this);
	theInspectionStatus->setCurrentValue(1.0);
}

RPowerStationInspectionTimeModel::~RPowerStationInspectionTimeModel()
{

}

QObject * RPowerStationInspectionTimeModel::getPowerStationInformationModel() const
{
	return thePowerStationInformationModel.data();
}

void RPowerStationInspectionTimeModel::setPowerStationInformationModel(QObject *value)
{
	thePowerStationInformationModel = qobject_cast<RModel *>(value);
}

RPowerStationInspectionTimeModel::RMonitoring RPowerStationInspectionTimeModel::getMonitoring() const
{
	return theMonitoring;
}

void RPowerStationInspectionTimeModel::setMonitoring(RPowerStationInspectionTimeModel::RMonitoring value)
{
	theMonitoring = value;
}



QObject * RPowerStationInspectionTimeModel::getInspectionTimeParameter() const
{
	return theInspectionTimeParameter.data();
}

void RPowerStationInspectionTimeModel::setInspectionTimeParameter(QObject *value)
{
	theInspectionTimeParameter = qobject_cast<RParameter *>(value);
}

int RPowerStationInspectionTimeModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
{

	// Checking if all pointer properties are given by the user
	int status = this->checkProperties();
	// Error handling
	if (status < 0) {
		// Printing an error message to the output pane
		qCritical() << "Error in" << this->objectName() << ":"
			<< "Could not find all the passed objects";
		// Returning the error code
		return -1;
	}


	// Getting the information from the PSIM
	RPowerStationInformationModel *thePSIM = qobject_cast<RPowerStationInformationModel *>(thePowerStationInformationModel.data());
	if (!thePSIM) {
		qCritical() << "Error in model" << objectName() << ":" << thePowerStationInformationModel.data()->objectName() << "is not a Power Station information model.";
		return -1;
	}


	//Get the inspection Time
	double inspectionTime = theInspectionTimeParameter.data()->getCurrentValue(threadID);
	//inspection time less than 0.1 day can be assumed zero
	if (inspectionTime < 0.1) {
		inspectionTime = 0.0;
	}

	if ((inspectionTime == 0) || (theMonitoring == RPowerStationInspectionTimeModel::Sensor)) {
		theInspectionTime->setCurrentValue(0.0);
		theInspectionStatus->setCurrentValue(1.0);
	}
	else if (theMonitoring == RPowerStationInspectionTimeModel::Drone) {
		//It is assumed that the inspection Time with drone is half the time with nothing
		theInspectionTime->setCurrentValue(inspectionTime / 2, threadID);
		theInspectionStatus->setCurrentValue(0.0, threadID);
	}
	else {
		theInspectionTime->setCurrentValue(inspectionTime, threadID);
		theInspectionStatus->setCurrentValue(0.0, threadID);
	}

	return 0;
}

bool RPowerStationInspectionTimeModel::canBeRunParallelly() {
	return true;
}