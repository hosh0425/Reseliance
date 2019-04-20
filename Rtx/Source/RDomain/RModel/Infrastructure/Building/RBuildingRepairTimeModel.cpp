#include "RBuildingRepairTimeModel.h"	

#include "RParameter.h"	
#include "RResponse.h"	
#include "RBuildingInformationModel.h"
#include <math.h>	

RBuildingRepairTimeModel::RBuildingRepairTimeModel(QObject *parent, QString name)
	: RModel(parent, name)
{

	// Instantiating the response object
	theDomain->createObject(objectName() + "TimeResponse", "RGenericResponse");
	theRepairTime = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theRepairTime->setModel(this);

	theDomain->createObject(objectName() + "StatusResponse", "RGenericResponse");
	theRepairStatus = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theRepairStatus->setModel(this);
	theRepairStatus->setCurrentValue(1);
}

RBuildingRepairTimeModel::~RBuildingRepairTimeModel()
{

}

QObject * RBuildingRepairTimeModel::getBuildingInformationModel() const
{
	return theBuildingInformationModel.data();
}

void RBuildingRepairTimeModel::setBuildingInformationModel(QObject *value)
{
	theBuildingInformationModel = qobject_cast<RModel *>(value);
}

QObject * RBuildingRepairTimeModel::getStructuralDamageRatio() const
{
	return theStructuralDamageRatio.data();
}

void RBuildingRepairTimeModel::setStructuralDamageRatio(QObject *value)
{
	theStructuralDamageRatio = qobject_cast<RParameter *>(value);
}

int RBuildingRepairTimeModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
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

	QList<double> repairTimeList;
	//Based on FEMA table 15.9 
	switch (theBIM->getOccupancyType()) {

	case (RBuildingInformationModel::SingleResidence):
		repairTimeList << 0 << 2 << 30 << 90 << 180;
		break;
	case (RBuildingInformationModel::MultiResidence):
		repairTimeList << 0 << 5 << 30 << 120 << 240;
		break;
	case (RBuildingInformationModel::InstitutionalResidence):
		repairTimeList << 0 << 5 << 30 << 120 << 240;
		break;
	case (RBuildingInformationModel::Trade):
		repairTimeList << 0 << 5 << 30 << 90 << 180;
		break;
	case (RBuildingInformationModel::Services):
		repairTimeList << 0 << 5 << 30 << 120 << 240;
		break;
	case (RBuildingInformationModel::Medical):
		repairTimeList << 0 << 10 << 45 << 180 << 360;
		break;
	case (RBuildingInformationModel::Recreation):
		repairTimeList << 0 << 5 << 30 << 90 << 180;
		break;
	case (RBuildingInformationModel::Parking):
		repairTimeList << 0 << 2 << 20 << 80 << 160;
		break;
	case (RBuildingInformationModel::School):
		repairTimeList << 0 << 10 << 30 << 120 << 240;
		break;
	case (RBuildingInformationModel::University):
		repairTimeList << 0 << 10 << 45 << 180 << 360;
		break;
	case (RBuildingInformationModel::Government):
		repairTimeList << 0 << 10 << 30 << 120 << 240;
		break;
	case (RBuildingInformationModel::Industry):
		repairTimeList << 0 << 10 << 30 << 120 << 240;
		break;
	case (RBuildingInformationModel::Agriculture):
		repairTimeList << 0 << 2 << 10 << 30 << 60;
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

	double repairTime = (repairTimeList[index] * (structuralDamageRatio - structDamageRatioList[index - 1]) + repairTimeList[index - 1] * (structDamageRatioList[index] - structuralDamageRatio)) / (structDamageRatioList[index] - structDamageRatioList[index - 1]);

	//If the building is located in Iran, the repair time derived from HAZUS is increased by 20 percent
	if (region == RBuildingInformationModel::Iran) repairTime = 1.2 * repairTime;

	//repair time less than one day can be assumed zero
	if (repairTime < 1) repairTime = 0.0;

	double repairStatus = 0.0;

	//Here the RepairStatus is eguivalent to Serviceability of the building
	//Damage ratio 0.05 is equivalent to Damage state moderate and more - Expert opinion
	if (structuralDamageRatio < 0.05) {
		repairStatus = 1.0;
	}
	//if (repairTime == 0) {
	//	repairStatus = 1;
	//}
	theRepairTime->setCurrentValue(repairTime, threadID);
	theRepairStatus->setCurrentValue(repairStatus, threadID);


	return 0;
}

bool RBuildingRepairTimeModel::canBeRunParallelly() {
	return true;
}