#include "RBuildingInspectionCostModel.h"	

#include "RParameter.h"	
#include "RResponse.h"	
#include "RBuildingInformationModel.h"
#include <math.h>	

RBuildingInspectionCostModel::RBuildingInspectionCostModel(QObject *parent, QString name)
	: RModel(parent, name)
{
	// Initializing the data members
	//theBuildingInformationModel = 0;
	//theInspectionCostParameter = 0;

	// Instantiating the response object
	theDomain->createObject(objectName() + "Response", "RGenericResponse");
	theInspectionCost = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theInspectionCost->setModel(this);

}

RBuildingInspectionCostModel::~RBuildingInspectionCostModel()
{

}

QObject * RBuildingInspectionCostModel::getBuildingInformationModel() const
{
	return theBuildingInformationModel.data();
}

void RBuildingInspectionCostModel::setBuildingInformationModel(QObject *value)
{
	theBuildingInformationModel = qobject_cast<RModel *>(value);
}

QObject * RBuildingInspectionCostModel::getInspectionCostParameter() const
{
	return theInspectionCostParameter.data();
}

void RBuildingInspectionCostModel::setInspectionCostParameter(QObject *value)
{
	theInspectionCostParameter = qobject_cast<RParameter *>(value);
}

QObject * RBuildingInspectionCostModel::getInspectionStatus() const
{
	return theInspectionStatus.data();
}

void RBuildingInspectionCostModel::setInspectionStatus(QObject *value)
{
	theInspectionStatus = qobject_cast<RParameter *>(value);
}

int RBuildingInspectionCostModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
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

	double inspectionCost = 0;
	inspectionCost = theInspectionCostParameter.data()->getCurrentValue(threadID);
	double InspectionStatus = theInspectionStatus.data()->getCurrentValue(threadID);


	//THE ALGORITHM
	//Compute the inspection cost
	//If the InspectionStatus is zero, i.e., the Inspection opeation has not been done yet, the repair cost is zero. Once the inspection operation is done the cost is added. 
	inspectionCost = inspectionCost * InspectionStatus;
	theInspectionCost->setCurrentValue(inspectionCost, threadID);

	return 0;
}

bool RBuildingInspectionCostModel::canBeRunParallelly() {
	return true;
}