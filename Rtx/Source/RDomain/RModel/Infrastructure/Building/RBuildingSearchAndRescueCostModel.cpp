#include "RBuildingSearchAndRescueCostModel.h"	

#include "RParameter.h"	
#include "RResponse.h"	
#include "RBuildingInformationModel.h"
#include <math.h>	

RBuildingSearchAndRescueCostModel::RBuildingSearchAndRescueCostModel(QObject *parent, QString name)
	: RModel(parent, name)
{
	// Initializing the data members
	//theBuildingInformationModel = 0;
	//theStructuralDamageRatio = 0;

	// Instantiating the response object
	theDomain->createObject(objectName() + "Response", "RGenericResponse");
	theSearchAndRescueCost = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theSearchAndRescueCost->setModel(this);

}

RBuildingSearchAndRescueCostModel::~RBuildingSearchAndRescueCostModel()
{

}

QObject *RBuildingSearchAndRescueCostModel::getBuildingInformationModel() const
{
	return theBuildingInformationModel.data();
}

void RBuildingSearchAndRescueCostModel::setBuildingInformationModel(QObject *value)
{
	theBuildingInformationModel = qobject_cast<RModel *>(value);
}

QObject *RBuildingSearchAndRescueCostModel::getStructuralDamageRatio() const
{
	return theStructuralDamageRatio.data();
}

void RBuildingSearchAndRescueCostModel::setStructuralDamageRatio(QObject *value)
{
	theStructuralDamageRatio = qobject_cast<RParameter *>(value);
}

QObject *RBuildingSearchAndRescueCostModel::getSearchAndRescueStatus() const
{
	return theSearchAndRescueStatus.data();
}

void RBuildingSearchAndRescueCostModel::setSearchAndRescueStatus(QObject *value)
{
	theSearchAndRescueStatus = qobject_cast<RParameter *>(value);
}

QObject *RBuildingSearchAndRescueCostModel::getSearchAndRescueTime() const
{
	return theSearchAndRescueTime.data();
}

void RBuildingSearchAndRescueCostModel::setSearchAndRescueTime(QObject *value)
{
	theSearchAndRescueTime = qobject_cast<RParameter *>(value);
}

QObject *RBuildingSearchAndRescueCostModel::getSearchAndRescueCostRate() const
{
	return theSearchAndRescueCostRate.data();
}

void RBuildingSearchAndRescueCostModel::setSearchAndRescueCostRate(QObject *value)
{
	theSearchAndRescueCostRate = qobject_cast<RParameter *>(value);
}

int RBuildingSearchAndRescueCostModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
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

	double region = theBIM->getRegion();
	double totalArea = theBIM->getTotalArea();
	double height = theBIM->getHeight();
	double material = theBIM->getMaterial();
	double bearingSystem = theBIM->getLoadBearingSystem();
	
	double structuralDamageRatio = theStructuralDamageRatio.data()->getCurrentValue(threadID);
	if (structuralDamageRatio > 1)  structuralDamageRatio = 1;
	if (structuralDamageRatio < 0)  structuralDamageRatio = 0;

	double rescueTime = theSearchAndRescueTime.data()->getCurrentValue(threadID);
	double rescueCostPerDay = theSearchAndRescueCostRate.data()->getCurrentValue(threadID);
	double rescueCost = rescueTime * rescueCostPerDay;

	//If the SearchAndRescueStatus is zero, i.e., the SearchAndRescue opeation has not been done yet, the repair cost is zero. Once the SearchAndRescue operation is done the cost is added. 
	double SearchAndRescueStatus = theSearchAndRescueStatus.data()->getCurrentValue(threadID);
	rescueCost = rescueCost * SearchAndRescueStatus;
	theSearchAndRescueCost->setCurrentValue(rescueCost, threadID);

	return 0;
}

bool RBuildingSearchAndRescueCostModel::canBeRunParallelly() {
	return true;
}