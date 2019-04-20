#include "RBuildingInspectionTimeModel.h"	

#include "RParameter.h"	
#include "RResponse.h"	
#include "RBuildingInformationModel.h"
#include <math.h>	

RBuildingInspectionTimeModel::RBuildingInspectionTimeModel(QObject *parent, QString name)
	: RModel(parent, name)
{
	// Initializing the data members
	//theBuildingInformationModel = 0;
	//theInspectionTimeParameter = 0;
	theMonitoring = RBuildingInspectionTimeModel::Visual;

	// Instantiating the response object
	theDomain->createObject(objectName() + "TimeResponse", "RGenericResponse");
	theInspectionTime = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theInspectionTime->setModel(this);

	theDomain->createObject(objectName() + "StatusResponse", "RGenericResponse");
	theInspectionStatus = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theInspectionStatus->setModel(this);
	theInspectionStatus->setCurrentValue(1);
}

RBuildingInspectionTimeModel::~RBuildingInspectionTimeModel()
{

}

QObject * RBuildingInspectionTimeModel::getBuildingInformationModel() const
{
	return theBuildingInformationModel.data();
}

void RBuildingInspectionTimeModel::setBuildingInformationModel(QObject *value)
{
	theBuildingInformationModel = qobject_cast<RModel *>(value);
}

RBuildingInspectionTimeModel::RMonitoring RBuildingInspectionTimeModel::getMonitoring() const
{
	return theMonitoring;
}

void RBuildingInspectionTimeModel::setMonitoring(RBuildingInspectionTimeModel::RMonitoring value)
{
	theMonitoring = value;
}



QObject * RBuildingInspectionTimeModel::getInspectionTimeParameter() const
{
	return theInspectionTimeParameter.data();
}

void RBuildingInspectionTimeModel::setInspectionTimeParameter(QObject *value)
{
	theInspectionTimeParameter = qobject_cast<RParameter *>(value);
}

int RBuildingInspectionTimeModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
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


	// Getting the information from the BIM
	RBuildingInformationModel *theBIM = qobject_cast<RBuildingInformationModel *>(theBuildingInformationModel.data());
	if (!theBIM) {
		qCritical() << "Error in model" << objectName() << ":" << theBuildingInformationModel.data()->objectName() << "is not a Building information model.";
		return -1;
	}


	//Get the inspection Time
	double inspectionTime = theInspectionTimeParameter.data()->getCurrentValue(threadID);
	//inspection Time less than 0.1 day can be assumed to be zero
	if (inspectionTime < 0.1) {
		inspectionTime = 0.0;
	}

	if ((inspectionTime == 0) || (theMonitoring == RBuildingInspectionTimeModel::Sensor)) {
		theInspectionTime->setCurrentValue(0.0);
		theInspectionStatus->setCurrentValue(1.0);
	}
	else if (theMonitoring == RBuildingInspectionTimeModel::Drone) {
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

bool RBuildingInspectionTimeModel::canBeRunParallelly() {
	return true;
}