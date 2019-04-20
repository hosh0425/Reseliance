#include "RPowerStationRepairCostModel.h"	

#include "RParameter.h"	
#include "RResponse.h"	
#include "RPowerStationInformationModel.h"

#include <math.h>	

RPowerStationRepairCostModel::RPowerStationRepairCostModel(QObject *parent, QString name)
	: RModel(parent, name)
{
	// The default replacement values are in Table 15.17 of FEMA

	// Instantiating the response object
	theDomain->createObject(objectName() + "Response", "RGenericResponse");
	theRepairCost = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theRepairCost->setModel(this);
}

RPowerStationRepairCostModel::~RPowerStationRepairCostModel()
{

}

QObject * RPowerStationRepairCostModel::getPowerStationInformationModel() const
{
	return thePowerStationInformationModel.data();
}

void RPowerStationRepairCostModel::setPowerStationInformationModel(QObject *value)
{
	thePowerStationInformationModel = qobject_cast<RModel *>(value);
}

QObject * RPowerStationRepairCostModel::getDamageRatio() const
{
	return theDamageRatio.data();
}

void RPowerStationRepairCostModel::setDamageRatio(QObject *value)
{
	theDamageRatio = qobject_cast<RParameter *>(value);
}

QObject * RPowerStationRepairCostModel::getPowerStationServiceability() const
{
	return thePowerStationServiceability.data();
}

void RPowerStationRepairCostModel::setPowerStationServiceability(QObject *value)
{
	thePowerStationServiceability = qobject_cast<RParameter *>(value);
}

QObject *  RPowerStationRepairCostModel::getReplacementValue() const
{
	return theReplacementValue.data();
}

void RPowerStationRepairCostModel::setReplacementValue(QObject *value)
{
	theReplacementValue = qobject_cast<RParameter *>(value);
}

int RPowerStationRepairCostModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
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
	RPowerStationInformationModel *thePSIM = qobject_cast<RPowerStationInformationModel *>(thePowerStationInformationModel.data());
	if (!thePSIM) {
		qCritical() << "Error in model" << objectName() << ":" << thePowerStationInformationModel.data()->objectName() << "is not a Power Station information model.";
		return -1;
	}

	double ReplacementValue = theReplacementValue.data()->getCurrentValue(threadID);
	double damageRatio = theDamageRatio.data()->getCurrentValue(threadID);
	double powerStationServiceability = thePowerStationServiceability.data()->getCurrentValue(threadID);


	//THE ALGORITHM
	//Compute the repair cost
	double repairCost = 0.0;
	repairCost = (damageRatio*ReplacementValue);

	//If the serviceability is zero, i.e., the repair opeation has not been done yet, the repair cost is zero. Once the serviceability is changed to 1, the cost is added. 
	repairCost = repairCost * powerStationServiceability;

	theRepairCost->setCurrentValue(repairCost, threadID);

	//qCritical() << repairCost;


	return 0;
}

bool RPowerStationRepairCostModel::canBeRunParallelly() {
	return true;
}