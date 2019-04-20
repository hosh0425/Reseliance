#include "RWaterStationRepairCostModel.h"	

#include "RParameter.h"	
#include "RResponse.h"	
#include "RWaterStationInformationModel.h"

#include <math.h>	

RWaterStationRepairCostModel::RWaterStationRepairCostModel(QObject *parent, QString name)
	: RModel(parent, name)
{
	// The default replacement values are in Table 15.17 of FEMA
	
	// Instantiating the response object
	theDomain->createObject(objectName() + "Response", "RGenericResponse");
	theRepairCost = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theRepairCost->setModel(this);
}

RWaterStationRepairCostModel::~RWaterStationRepairCostModel()
{

}

QObject * RWaterStationRepairCostModel::getWaterStationInformationModel() const
{
	return theWaterStationInformationModel.data();
}

void RWaterStationRepairCostModel::setWaterStationInformationModel(QObject *value)
{
	theWaterStationInformationModel = qobject_cast<RModel *>(value);
}

QObject * RWaterStationRepairCostModel::getDamageRatio() const
{
	return theDamageRatio.data();
}

void RWaterStationRepairCostModel::setDamageRatio(QObject *value)
{
	theDamageRatio = qobject_cast<RParameter *>(value);
}

QObject * RWaterStationRepairCostModel::getWaterStationFunctionality() const
{
	return theWaterStationFunctionality.data();
}

void RWaterStationRepairCostModel::setWaterStationFunctionality(QObject *value)
{
	theWaterStationFunctionality = qobject_cast<RParameter *>(value);
}

QObject *  RWaterStationRepairCostModel::getReplacementValue() const
{
	return theReplacementValue.data();
}

void RWaterStationRepairCostModel::setReplacementValue(QObject *value)
{
	theReplacementValue = qobject_cast<RParameter *>(value);
}

int RWaterStationRepairCostModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
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

	double ReplacementValue = theReplacementValue.data()->getCurrentValue(threadID);
	double damageRatio = theDamageRatio.data()->getCurrentValue(threadID);
	double WaterStationFunctionality = theWaterStationFunctionality.data()->getCurrentValue(threadID);


	//THE ALGORITHM
	//Compute the repair cost
	double repairCost = 0.0;
	repairCost = (damageRatio*ReplacementValue);

	//If the functionality is zero, i.e., the repair opeation has not been done yet, the repair cost is zero. Once the functionality is changed to 1, the cost is added. 
	repairCost = repairCost * WaterStationFunctionality;
	theRepairCost->setCurrentValue(repairCost, threadID);

	//qCritical() << repairCost;


	return 0;
}

bool RWaterStationRepairCostModel::canBeRunParallelly() {
	return true;
}