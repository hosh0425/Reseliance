#include "RWaterStationInspectionCostModel.h"	

#include "RParameter.h"	
#include "RResponse.h"	
#include "RWaterStationInformationModel.h"

#include <math.h>	

RWaterStationInspectionCostModel::RWaterStationInspectionCostModel(QObject *parent, QString name)
	: RModel(parent, name)
{
	// Initializing the data members
	//theWaterStationInformationModel = 0;
	//theInspectionCostParameter = 0;
	//theInspectionStatus = 0;

	// Instantiating the response object
	theDomain->createObject(objectName() + "Response", "RGenericResponse");
	theInspectionCost = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theInspectionCost->setModel(this);
}

RWaterStationInspectionCostModel::~RWaterStationInspectionCostModel()
{

}

QObject * RWaterStationInspectionCostModel::getWaterStationInformationModel() const
{
	return theWaterStationInformationModel.data();
}

void RWaterStationInspectionCostModel::setWaterStationInformationModel(QObject *value)
{
	theWaterStationInformationModel = qobject_cast<RModel *>(value);
}

QObject * RWaterStationInspectionCostModel::getInspectionCostParameter() const
{
	return theInspectionCostParameter.data();
}

void RWaterStationInspectionCostModel::setInspectionCostParameter(QObject *value)
{
	theInspectionCostParameter = qobject_cast<RParameter *>(value);
}

QObject * RWaterStationInspectionCostModel::getInspectionStatus() const
{
	return theInspectionStatus.data();
}

void RWaterStationInspectionCostModel::setInspectionStatus(QObject *value)
{
	theInspectionStatus = qobject_cast<RParameter *>(value);
}

int RWaterStationInspectionCostModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
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

	double inspectionCost = 0.0;

	inspectionCost = theInspectionCostParameter.data()->getCurrentValue(threadID);
	double inspectionStatus = theInspectionStatus.data()->getCurrentValue(threadID);


	//THE ALGORITHM
	//Compute the inspection cost
	//If the InspectionStatus is zero, i.e., the Inspection opeation has not been done yet, the repair cost is zero. Once the inspection operation is done the cost is added. 
	inspectionCost = inspectionCost * inspectionStatus;
	theInspectionCost->setCurrentValue(inspectionCost, threadID);



	return 0;
}

bool RWaterStationInspectionCostModel::canBeRunParallelly() {
	return true;
}