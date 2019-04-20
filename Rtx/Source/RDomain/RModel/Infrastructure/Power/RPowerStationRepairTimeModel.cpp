#include "RPowerStationRepairTimeModel.h"	

#include "RParameter.h"	
#include "RResponse.h"	
#include "RPowerStationInformationModel.h"
#include <math.h>	

RPowerStationRepairTimeModel::RPowerStationRepairTimeModel(QObject *parent, QString name)
	: RModel(parent, name)
{
	// Initializing the data members
	//thePowerStationInformationModel = 0;
	//theDamageRatio = 0;
	//theRepairTimeUncertainty = 0;
	//thePowerStationServiceability = 0;

	// Instantiating the response object
	theDomain->createObject(objectName() + "Response", "RGenericResponse");
	theRepairTime = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theRepairTime->setModel(this);
}

RPowerStationRepairTimeModel::~RPowerStationRepairTimeModel()
{

}

QObject * RPowerStationRepairTimeModel::getPowerStationInformationModel() const
{
	return thePowerStationInformationModel.data();
}

void RPowerStationRepairTimeModel::setPowerStationInformationModel(QObject *value)
{
	thePowerStationInformationModel = qobject_cast<RModel *>(value);
}

QObject * RPowerStationRepairTimeModel::getDamageRatio() const
{
	return theDamageRatio.data();
}

void RPowerStationRepairTimeModel::setDamageRatio(QObject *value)
{
	theDamageRatio = qobject_cast<RParameter *>(value);
}

QObject * RPowerStationRepairTimeModel::getRepairTimeUncertainty() const
{
	return theRepairTimeUncertainty.data();
}

void RPowerStationRepairTimeModel::setRepairTimeUncertainty(QObject *value)
{
	theRepairTimeUncertainty = qobject_cast<RParameter *>(value);
}

int RPowerStationRepairTimeModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
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

	double type = thePSIM->getType();
	double components = thePSIM->getComponents();

	//Computing the repair time based on the FEMA
	//Mean Damage Ratio Table 15.28 FEMA
	QList<double> damageRatioList;
	damageRatioList << 0 << 0.05 << 0.11 << 0.55 << 1.0;


	double DamageRatio = theDamageRatio.data()->getCurrentValue(threadID);
	if (DamageRatio > 1)  DamageRatio = 1.0;
	if (DamageRatio < 0)  DamageRatio = 0.0;

	//Get the input standard normal random variable
	double randNum1 = theRepairTimeUncertainty.data()->getCurrentValue(threadID);

	//Computing the repair time based on the FEMA

	//Vectors of mean repair time and stdev
	QList<double> meanTimeList;
	QList<double> stdevTimeList;

	//Parameters of restoration time per Table 8.22a of FEMA
	meanTimeList << 0 << 1 << 3 << 7 << 30;
	stdevTimeList << 0 << 0.5 << 1.5 << 3.5 << 15;

	//THE ALGORITHM

	//Compute the repair time
	double repairTime = 0.0;

	int index = 1;
	for (int i = 1; i < damageRatioList.count(); i++) {
		if ((DamageRatio <= damageRatioList[i]) && (DamageRatio >= damageRatioList[i - 1])) {
			index = i;
			break;
		}
	}
	double meanRepairTime = (meanTimeList[index] * (DamageRatio - damageRatioList[index - 1]) + meanTimeList[index - 1] * (damageRatioList[index] - DamageRatio)) / (damageRatioList[index] - damageRatioList[index - 1]);
	double stdevRepairTime = (stdevTimeList[index] * (DamageRatio - damageRatioList[index - 1]) + stdevTimeList[index - 1] * (damageRatioList[index] - DamageRatio)) / (damageRatioList[index] - damageRatioList[index - 1]);

	repairTime = randNum1 * stdevRepairTime + meanRepairTime;

	//if the repair time is less than one day it can be considered to be zero since it is not a really full task of repairing
	if (repairTime < 1) {
		repairTime = 0.0;
	}

	double repairStatus = 0.0;
	if (repairTime == 0) {
		repairStatus = 1.0;
	}

	theRepairTime->setCurrentValue(repairTime, threadID);


	return 0;
}

bool RPowerStationRepairTimeModel::canBeRunParallelly() {
	return true;
}