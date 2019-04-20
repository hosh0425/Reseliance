#include "RBuildingRestorationModel.h"	

#include "RParameter.h"	
#include "RResponse.h"	
#include "RBuildingInformationModel.h"
#include <math.h>	

RBuildingRestorationModel::RBuildingRestorationModel(QObject *parent, QString name)
	: RModel(parent, name)
{

	// Instantiating the response object
	theDomain->createObject(objectName() + "TimeResponse", "RGenericResponse");
	theRestorationTime = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theRestorationTime->setModel(this);

	theDomain->createObject(objectName() + "StatusResponse", "RGenericResponse");
	theRestorationStatus = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theRestorationStatus->setModel(this);
	theRestorationStatus->setCurrentValue(1);
}

RBuildingRestorationModel::~RBuildingRestorationModel()
{

}

QObject * RBuildingRestorationModel::getBuildingInformationModel() const
{
	return theBuildingInformationModel.data();
}

void RBuildingRestorationModel::setBuildingInformationModel(QObject *value)
{
	theBuildingInformationModel = qobject_cast<RModel *>(value);
}

QObject * RBuildingRestorationModel::getStructuralDamageRatio() const
{
	return theStructuralDamageRatio.data();
}

void RBuildingRestorationModel::setStructuralDamageRatio(QObject *value)
{
	theStructuralDamageRatio = qobject_cast<RParameter *>(value);
}

QObject * RBuildingRestorationModel::getRepairTime() const
{
	return theRepairTime.data();
}

void RBuildingRestorationModel::setRepairTime(QObject *value)
{
	theRepairTime = qobject_cast<RParameter *>(value);
}

QObject * RBuildingRestorationModel::getMobilizationTime() const
{
	return theMobilizationTime.data();
}

void RBuildingRestorationModel::setMobilizationTime(QObject *value)
{
	theMobilizationTime = qobject_cast<RParameter *>(value);
}

int RBuildingRestorationModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
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

	QList<double> structDamageRatioList;
	structDamageRatioList << 0 << 0.02 << 0.1 << 0.5 << 1.0;

	//Based on FEMA table 15.11 
	QList<double> restorationTimeMultiplierList;
	switch (theBIM->getOccupancyType()) {

	case (RBuildingInformationModel::SingleResidence):
		restorationTimeMultiplierList << 0 << 0 << 0.5 << 1 << 1;
		break;
	case (RBuildingInformationModel::MultiResidence):
		restorationTimeMultiplierList << 0 << 0 << 0.5 << 1 << 1;
		break;
	case (RBuildingInformationModel::InstitutionalResidence):
		restorationTimeMultiplierList << 0 << 0 << 0.5 << 1 << 1;
		break;
	case (RBuildingInformationModel::Trade):
		restorationTimeMultiplierList << 0.5 << 0.1 << 0.15 << 0.3 << 0.4;
		break;
	case (RBuildingInformationModel::Services):
		restorationTimeMultiplierList << 0.5 << 0.1 << 0.15 << 0.25 << 0.35;
		break;
	case (RBuildingInformationModel::Medical):
		restorationTimeMultiplierList << 0.5 << 0.1 << 0.5 << 0.5 << 0.5;
		break;
	case (RBuildingInformationModel::Recreation):
		restorationTimeMultiplierList << 0.5 << 0.1 << 1 << 1 << 1;
		break;
	case (RBuildingInformationModel::Parking):
		restorationTimeMultiplierList << 0.1 << 0.1 << 1 << 1 << 1;
		break;
	case (RBuildingInformationModel::School):
		restorationTimeMultiplierList << 0.5 << 0.1 << 0.02 << 0.05 << 0.05;
		break;
	case (RBuildingInformationModel::University):
		restorationTimeMultiplierList << 0.5 << 0.1 << 0.02 << 0.03 << 0.03;
		break;
	case (RBuildingInformationModel::Government):
		restorationTimeMultiplierList << 0.5 << 0.1 << 0.02 << 0.03 << 0.03;
		break;
	case (RBuildingInformationModel::Industry):
		restorationTimeMultiplierList << 0.5 << 0.2 << 0.2 << 0.3 << 0.4;
		break;
	case (RBuildingInformationModel::Agriculture):
		restorationTimeMultiplierList << 0 << 0 << 0.05 << 0.1 << 0.2;
		break;
	}

	double structuralDamageRatio = theStructuralDamageRatio.data()->getCurrentValue(threadID);
	if (structuralDamageRatio > 1)  structuralDamageRatio = 1;
	if (structuralDamageRatio < 0)  structuralDamageRatio = 0;

	int index = 1;
	for (int i = 1; i < structDamageRatioList.count(); i++) {
		if ((structuralDamageRatio <= structDamageRatioList[i]) && (structuralDamageRatio >= structDamageRatioList[i-1])) {
			index = i;
			break;
		}
	}

	double restorationMultiplier = (restorationTimeMultiplierList[index] * (structuralDamageRatio - structDamageRatioList[index - 1]) + restorationTimeMultiplierList[index - 1] * (structDamageRatioList[index] - structuralDamageRatio)) / (structDamageRatioList[index] - structDamageRatioList[index - 1]);


	double repairTime = theRepairTime.data()->getCurrentValue(threadID);
	double mobilizationTime = theMobilizationTime.data()->getCurrentValue(threadID);
	double recoveryTime = mobilizationTime + repairTime;

	double RestorationTime = restorationMultiplier * repairTime;
	if (region == RBuildingInformationModel::Iran) RestorationTime = 1.2 * RestorationTime;
	
	//restoration time less than one day can be conceived as zero
	if (RestorationTime < 1) RestorationTime = 0.0;

	double RestorationStatus = 0.0;

	//Damage ratio 0.05 is equivalent to Damage state moderate and more - Expert opinion
	if (structuralDamageRatio < 0.05) {
		RestorationStatus = 1.0;
	}
	//if (RestorationTime == 0) {
	//	RestorationStatus = 1;
	//}
	theRestorationTime->setCurrentValue(RestorationTime, threadID);
	theRestorationStatus->setCurrentValue(RestorationStatus, threadID);


	return 0;
}

bool RBuildingRestorationModel::canBeRunParallelly() {
	return true;
}