#include "RWaterStationRepairTimeModel.h"	

#include "RParameter.h"	
#include "RResponse.h"	
#include "RWaterStationInformationModel.h"

#include <math.h>	

RWaterStationRepairTimeModel::RWaterStationRepairTimeModel(QObject *parent, QString name)
	: RModel(parent, name)
{
	// Initializing the data members
	//theWaterStationInformationModel = 0;
	//theDamageRatio = 0;
	//theRepairTimeUncertainty = 0;
	//theWaterStationFunctionality = 0;

	// Instantiating the response object
	theDomain->createObject(objectName() + "Response", "RGenericResponse");
	theRepairTime = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theRepairTime->setModel(this);
}

RWaterStationRepairTimeModel::~RWaterStationRepairTimeModel()
{

}

QObject * RWaterStationRepairTimeModel::getWaterStationInformationModel() const
{
	return theWaterStationInformationModel.data();
}

void RWaterStationRepairTimeModel::setWaterStationInformationModel(QObject *value)
{
	theWaterStationInformationModel = qobject_cast<RModel *>(value);
}

QObject * RWaterStationRepairTimeModel::getDamageRatio() const
{
	return theDamageRatio.data();
}

void RWaterStationRepairTimeModel::setDamageRatio(QObject *value)
{
	theDamageRatio = qobject_cast<RParameter *>(value);
}

QObject * RWaterStationRepairTimeModel::getRepairTimeUncertainty() const
{
	return theRepairTimeUncertainty.data();
}

void RWaterStationRepairTimeModel::setRepairTimeUncertainty(QObject *value)
{
	theRepairTimeUncertainty = qobject_cast<RParameter *>(value);
}

int RWaterStationRepairTimeModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
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

	double type = theWSIM->getType();
	double components = theWSIM->getComponents();

	//Computing the repair time based on the FEMA
	//Mean Damage Ratio Table 15.25 FEMA
	QList<double> damageRatioList;
	damageRatioList << 0 << 0.05 << 0.38 << 0.8 << 1.0;
	
	double DamageRatio = theDamageRatio.data()->getCurrentValue(threadID);
	if (DamageRatio > 1)  DamageRatio = 1;
	if (DamageRatio < 0)  DamageRatio = 0;

	//Get the input standard normal random variable
	double randNum1 = theRepairTimeUncertainty.data()->getCurrentValue(threadID);

	//Computing the repair time based on the FEMA

	//Lists of mean repair time and stdev
	QList<double> meanTimeList;
	QList<double> stdevTimeList;

	//Parameters of restoration time per Table 8.1a of FEMA
	meanTimeList << 0 << 0.9 << 3.1 << 13.5 << 35;
	stdevTimeList << 0 << 0.3 << 2.7 << 10 << 18 ;

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

bool RWaterStationRepairTimeModel::canBeRunParallelly() {
	return true;
}