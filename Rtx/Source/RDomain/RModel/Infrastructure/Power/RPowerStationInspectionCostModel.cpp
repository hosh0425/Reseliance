#include "RPowerStationInspectionCostModel.h"	

#include "RParameter.h"	
#include "RResponse.h"	
#include "RPowerStationInformationModel.h"

#include <math.h>	

RPowerStationInspectionCostModel::RPowerStationInspectionCostModel(QObject *parent, QString name)
	: RModel(parent, name)
{
	// Initializing the data members
	//thePowerStationInformationModel = 0;
	//theInspectionCostParameter = 0;
	//theInspectionStatus = 0;

	// Instantiating the response object
	theDomain->createObject(objectName() + "Response", "RGenericResponse");
	theInspectionCost = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theInspectionCost->setModel(this);
}

RPowerStationInspectionCostModel::~RPowerStationInspectionCostModel()
{

}

QObject * RPowerStationInspectionCostModel::getPowerStationInformationModel() const
{
	return thePowerStationInformationModel.data();
}

void RPowerStationInspectionCostModel::setPowerStationInformationModel(QObject *value)
{
	thePowerStationInformationModel = qobject_cast<RModel *>(value);
}

QObject * RPowerStationInspectionCostModel::getInspectionCostParameter() const
{
	return theInspectionCostParameter.data();
}

void RPowerStationInspectionCostModel::setInspectionCostParameter(QObject *value)
{
	theInspectionCostParameter = qobject_cast<RParameter *>(value);
}

QObject * RPowerStationInspectionCostModel::getInspectionStatus() const
{
	return theInspectionStatus.data();
}

void RPowerStationInspectionCostModel::setInspectionStatus(QObject *value)
{
	theInspectionStatus = qobject_cast<RParameter *>(value);
}

int RPowerStationInspectionCostModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
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


	double inspectionCost = 0.0;

	double replacementCost = thePSIM->getReplacementCost();
	inspectionCost = theInspectionCostParameter.data()->getCurrentValue(threadID);
	double inspectionStatus = theInspectionStatus.data()->getCurrentValue(threadID);


	//THE ALGORITHM
	//Compute the inspection cost
	//If the InspectionStatus is zero, i.e., the Inspection operation has not been done yet, the repair cost is zero. Once the inspection operation is done the cost is added. 
	inspectionCost = inspectionCost * inspectionStatus;
	theInspectionCost->setCurrentValue(inspectionCost, threadID);

	return 0;
}

bool RPowerStationInspectionCostModel::canBeRunParallelly() {
	return true;
}