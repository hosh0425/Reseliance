#include "RBuildingRepairMobilizationCostModel.h"	

#include "RParameter.h"	
#include "RResponse.h"	
#include "RBuildingInformationModel.h"
#include <math.h>	

RBuildingRepairMobilizationCostModel::RBuildingRepairMobilizationCostModel(QObject *parent, QString name)
	: RModel(parent, name)
{
	// Initializing the data members
	//theBuildingInformationModel = 0;
	//theMobilizationCostParameter = 0;

	// Instantiating the response object
	theDomain->createObject(objectName() + "Response", "RGenericResponse");
	theMobilizationCost = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theMobilizationCost->setModel(this);

}

RBuildingRepairMobilizationCostModel::~RBuildingRepairMobilizationCostModel()
{

}

QObject * RBuildingRepairMobilizationCostModel::getBuildingInformationModel() const
{
	return theBuildingInformationModel.data();
}

void RBuildingRepairMobilizationCostModel::setBuildingInformationModel(QObject *value)
{
	theBuildingInformationModel = qobject_cast<RModel *>(value);
}

QObject * RBuildingRepairMobilizationCostModel::getStructuralDamageRatio() const
{
	return theStructuralDamageRatio.data();
}

void RBuildingRepairMobilizationCostModel::setStructuralDamageRatio(QObject *value)
{
	theStructuralDamageRatio = qobject_cast<RParameter *>(value);
}

QObject * RBuildingRepairMobilizationCostModel::getMobilizationCostParameter() const
{
	return theMobilizationCostParameter.data();
}

void RBuildingRepairMobilizationCostModel::setMobilizationCostParameter(QObject *value)
{
	theMobilizationCostParameter = qobject_cast<RParameter *>(value);
}

QObject * RBuildingRepairMobilizationCostModel::getMobilizationStatus() const
{
	return theMobilizationStatus.data();
}

void RBuildingRepairMobilizationCostModel::setMobilizationStatus(QObject *value)
{
	theMobilizationStatus = qobject_cast<RParameter *>(value);
}

int RBuildingRepairMobilizationCostModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
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

	double mobilizationCost = 0;
	double structuralDamageRatio = theStructuralDamageRatio.data()->getCurrentValue(threadID);
	if (structuralDamageRatio > 1)  structuralDamageRatio = 1;
	if (structuralDamageRatio < 0)  structuralDamageRatio = 0;

	mobilizationCost = theMobilizationCostParameter.data()->getCurrentValue(threadID) * structuralDamageRatio;
	double mobilizationStatus = theMobilizationStatus.data()->getCurrentValue(threadID);


	//THE ALGORITHM
	//Compute the Mobilization cost
	//If the mobilizationStatus is zero, i.e., the mobilization opeation has not been done yet, the repair cost is zero. Once the mobilization operation is done the cost is added. 
	mobilizationCost = mobilizationCost * mobilizationStatus;

	theMobilizationCost->setCurrentValue(mobilizationCost, threadID);

	return 0;
}

bool RBuildingRepairMobilizationCostModel::canBeRunParallelly() {
	return true;
}