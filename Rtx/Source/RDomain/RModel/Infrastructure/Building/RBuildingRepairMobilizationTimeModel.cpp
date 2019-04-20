#include "RBuildingRepairMobilizationTimeModel.h"	

#include "RParameter.h"	
#include "RResponse.h"	
#include "RBuildingInformationModel.h"
#include <math.h>	

RBuildingRepairMobilizationTimeModel::RBuildingRepairMobilizationTimeModel(QObject *parent, QString name)
	: RModel(parent, name)
{

	// Instantiating the response object
	theDomain->createObject(objectName() + "TimeResponse", "RGenericResponse");
	theMobilizationTime = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theMobilizationTime->setModel(this);

	theDomain->createObject(objectName() + "StatusResponse", "RGenericResponse");
	theMobilizationStatus = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theMobilizationStatus->setModel(this);
	theMobilizationStatus->setCurrentValue(1);
}

RBuildingRepairMobilizationTimeModel::~RBuildingRepairMobilizationTimeModel()
{

}

QObject * RBuildingRepairMobilizationTimeModel::getBuildingInformationModel() const
{
	return theBuildingInformationModel.data();
}

void RBuildingRepairMobilizationTimeModel::setBuildingInformationModel(QObject *value)
{
	theBuildingInformationModel = qobject_cast<RModel *>(value);
}

QObject * RBuildingRepairMobilizationTimeModel::getStructuralDamageRatio() const
{
	return theStructuralDamageRatio.data();
}

void RBuildingRepairMobilizationTimeModel::setStructuralDamageRatio(QObject *value)
{
	theStructuralDamageRatio = qobject_cast<RParameter *>(value);
}

//QObject * RBuildingRepairMobilizationTimeModel::getRepairTime() const
//{
//	return theRepairTime.data();
//}
//
//void RBuildingRepairMobilizationTimeModel::setRepairTime(QObject *value)
//{
//	theRepairTime = qobject_cast<RParameter *>(value);
//}

int RBuildingRepairMobilizationTimeModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
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


	QList<double> structDamageRatioList;
	structDamageRatioList << 0 << 0.02 << 0.1 << 0.5 << 1.0;

	//Based on FEMA table 15.10
	QList<double> MobilizationTimeList;
	switch (theBIM->getOccupancyType()) {

	case (RBuildingInformationModel::SingleResidence):
		MobilizationTimeList << 0 << 3 << 90 << 270 << 540;
		break;
	case (RBuildingInformationModel::MultiResidence):
		MobilizationTimeList << 0 << 5 << 90 << 360 << 720;
		break;
	case (RBuildingInformationModel::InstitutionalResidence):
		MobilizationTimeList << 0 << 5 << 60 << 240 << 240;
		break;
	case (RBuildingInformationModel::Trade):
		MobilizationTimeList << 0 << 5 << 60 << 180 << 180;
		break;
	case (RBuildingInformationModel::Services):
		MobilizationTimeList << 0 << 10 << 60 << 210 << 210;
		break;
	case (RBuildingInformationModel::Medical):
		MobilizationTimeList << 0 << 10 << 90 << 225 << 330;
		break;
	case (RBuildingInformationModel::Recreation):
		MobilizationTimeList << 0 << 15 << 60 << 90 << 180;
		break;
	case (RBuildingInformationModel::Parking):
		MobilizationTimeList << 0 << 3 << 40 << 100 << 200;
		break;
	case (RBuildingInformationModel::School):
		MobilizationTimeList << 0 << 0 << 60 << 240 << 240;
		break;
	case (RBuildingInformationModel::University):
		MobilizationTimeList << 0 << 0 << 75 << 300 << 600;
		break;
	case (RBuildingInformationModel::Government):
		MobilizationTimeList << 0 << 0 << 60 << 240 << 240;
		break;
	case (RBuildingInformationModel::Industry):
		MobilizationTimeList << 0 << 0 << 60 << 120 << 120;
		break;
	case (RBuildingInformationModel::Agriculture):
		MobilizationTimeList << 0 << 0 << 10 << 30 << 60;
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
	
	double MobilizationTime = (MobilizationTimeList[index]*(structuralDamageRatio- structDamageRatioList[index-1]) + MobilizationTimeList[index-1] * (structDamageRatioList[index] - structuralDamageRatio))/(structDamageRatioList[index] - structDamageRatioList[index - 1]);
	 
	//If the building is located in Iran, the repair time derived from HAZUS is increased by 20 percent
	double region = theBIM->getRegion();
	if (region == RBuildingInformationModel::Iran) MobilizationTime = 1.2 * MobilizationTime;

	//Mobilization Time less than one day can be assumed to be zero
	if (MobilizationTime < 1) MobilizationTime = 0.0;

	double MobilizationStatus = 0.0;

	if (MobilizationTime == 0.0) {
		MobilizationStatus = 1.0;
	}
	theMobilizationTime->setCurrentValue(MobilizationTime, threadID);
	theMobilizationStatus->setCurrentValue(MobilizationStatus, threadID);
	

	return 0;
}

bool RBuildingRepairMobilizationTimeModel::canBeRunParallelly() {
	return true;
}