#include "RWaterStationInspectionTimeModel.h"	

#include "RParameter.h"	
#include "RResponse.h"	
#include "RWaterStationInformationModel.h"
#include "RWaterStationServiceabilityModel.h"	
#include <math.h>	

RWaterStationInspectionTimeModel::RWaterStationInspectionTimeModel(QObject *parent, QString name)
	: RModel(parent, name)
{
	// Initializing the data members
	//theWaterStationInformationModel = 0;
	//theInspectionTimeParameter = 0;
	theMonitoring = RWaterStationInspectionTimeModel::Visual;

	// Instantiating the response object
	theDomain->createObject(objectName() + "TimeResponse", "RGenericResponse");
	theInspectionTime = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theInspectionTime->setModel(this);

	theDomain->createObject(objectName() + "StatusResponse", "RGenericResponse");
	theInspectionStatus = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theInspectionStatus->setModel(this);
	theInspectionStatus->setCurrentValue(1);
}

RWaterStationInspectionTimeModel::~RWaterStationInspectionTimeModel()
{

}

QObject * RWaterStationInspectionTimeModel::getWaterStationInformationModel() const
{
	return theWaterStationInformationModel.data();
}

void RWaterStationInspectionTimeModel::setWaterStationInformationModel(QObject *value)
{
	theWaterStationInformationModel = qobject_cast<RModel *>(value);
}

RWaterStationInspectionTimeModel::RMonitoring RWaterStationInspectionTimeModel::getMonitoring() const
{
	return theMonitoring;
}

void RWaterStationInspectionTimeModel::setMonitoring(RWaterStationInspectionTimeModel::RMonitoring value)
{
	theMonitoring = value;
}

QObject * RWaterStationInspectionTimeModel::getInspectionTimeParameter() const
{
	return theInspectionTimeParameter.data();
}

void RWaterStationInspectionTimeModel::setInspectionTimeParameter(QObject *value)
{
	theInspectionTimeParameter = qobject_cast<RParameter *>(value);
}

int RWaterStationInspectionTimeModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
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


	// Getting the information from the WSIM
	RWaterStationInformationModel *theWSIM = qobject_cast<RWaterStationInformationModel *>(theWaterStationInformationModel.data());
	if (!theWSIM) {
		qCritical() << "Error in model" << objectName() << ":" << theWaterStationInformationModel.data()->objectName() << "is not a Water Station information model.";
		return -1;
	}


	//Get the inspection Time
	double inspectionTime = theInspectionTimeParameter.data()->getCurrentValue(threadID);
	//inspection time less than 0.1 day can be assumed zero
	if (inspectionTime <0.1) {
		inspectionTime = 0.0;
	}

	if ((inspectionTime == 0) || (theMonitoring == RWaterStationInspectionTimeModel::Sensor)) {
		theInspectionTime->setCurrentValue(0.0);
		theInspectionStatus->setCurrentValue(1.0);
	}
	else if (theMonitoring == RWaterStationInspectionTimeModel::Drone) {
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

bool RWaterStationInspectionTimeModel::canBeRunParallelly() {
	return true;
}