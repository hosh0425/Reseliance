#include "RLifeQualityReductionModel.h"	

#include "RParameter.h"	
#include "RResponse.h"	
#include "RTime.h"

#include <gsl/gsl_math.h>

#include <math.h>	

RLifeQualityReductionModel::RLifeQualityReductionModel(QObject *parent, QString name)
	: RModel(parent, name)
{
	int arraySizeForParallelizedObject = getArraySizeForParallelizedObject();

	currentTime = new double[arraySizeForParallelizedObject];
	previousTime = new double[arraySizeForParallelizedObject];


	currentPopWithoutHouse = new double[arraySizeForParallelizedObject];
	previousPopWithoutHouse = new double[arraySizeForParallelizedObject];

	currentPopWithoutPower = new double[arraySizeForParallelizedObject];
	previousPopWithoutPower = new double[arraySizeForParallelizedObject];

	currentPopWithoutWater = new double[arraySizeForParallelizedObject];
	previousPopWithoutWater = new double[arraySizeForParallelizedObject];

	currentPopWithoutPowerAndWater = new double[arraySizeForParallelizedObject];
	previousPopWithoutPowerAndWater = new double[arraySizeForParallelizedObject];


	cumulativeLifeQualityReduction = new double[arraySizeForParallelizedObject];

	theDomain->createObject(objectName() + "PopulationWithoutHouseResponse", "RGenericResponse");
	PopulationWithoutHouse = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	PopulationWithoutHouse->setModel(this);

	theDomain->createObject(objectName() + "PopulationWithoutOnlyPowerResponse", "RGenericResponse");
	PopulationWithoutOnlyPower = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	PopulationWithoutOnlyPower->setModel(this);

	theDomain->createObject(objectName() + "PopulationWithoutOnlyWaterResponse", "RGenericResponse");
	PopulationWithoutOnlyWater = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	PopulationWithoutOnlyWater->setModel(this);

	theDomain->createObject(objectName() + "PopulationWithoutPowerAndWaterResponse", "RGenericResponse");
	PopulationWithoutPowerAndWater = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	PopulationWithoutPowerAndWater->setModel(this);

	theDomain->createObject(objectName() + "CostResponse", "RGenericResponse");
	LifeQualityReductionCost = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	LifeQualityReductionCost->setModel(this);
}
RLifeQualityReductionModel::~RLifeQualityReductionModel()
{
	delete[] currentTime;
	delete[] previousTime;


	delete[] currentPopWithoutHouse;
	delete[] previousPopWithoutHouse;

	delete[] currentPopWithoutPower;
	delete[] previousPopWithoutPower;

	delete[] currentPopWithoutWater;
	delete[] previousPopWithoutWater;

	delete[] currentPopWithoutPowerAndWater;
	delete[] previousPopWithoutPowerAndWater;


	delete[] cumulativeLifeQualityReduction;

}

QObject * RLifeQualityReductionModel::getTime() const
{
	return theTime.data();
}

void RLifeQualityReductionModel::setTime(QObject *value)
{
	theTime = qobject_cast<RParameter *>(value);
}

QString RLifeQualityReductionModel::getPopulationList()
{
	return parameterListToString(thePopulationList);
}

void RLifeQualityReductionModel::setPopulationList(QString value)
{
	thePopulationList = stringToParameterList(value);
}

QString RLifeQualityReductionModel::getRepairOrRestorationStatusList()
{
	return parameterListToString(theRepairOrRestorationStatusList);
}

void RLifeQualityReductionModel::setRepairOrRestorationStatusList(QString value)
{
	theRepairOrRestorationStatusList = stringToParameterList(value);
}

QString RLifeQualityReductionModel::getPowerServiceabilityList()
{
	return parameterListToString(thePowerServiceabilityList);
}

void RLifeQualityReductionModel::setPowerServiceabilityList(QString value)
{
	thePowerServiceabilityList = stringToParameterList(value);
}

QString RLifeQualityReductionModel::getWaterServiceabilityList()
{
	return parameterListToString(theWaterServiceabilityList);
}

void RLifeQualityReductionModel::setWaterServiceabilityList(QString value)
{
	theWaterServiceabilityList = stringToParameterList(value);
}

QObject * RLifeQualityReductionModel::getUnitCostOfHavingNoHouse() const
{
	return theUnitCostOfHavingNoHouse.data();
}

void RLifeQualityReductionModel::setUnitCostOfHavingNoHouse(QObject *value)
{
	theUnitCostOfHavingNoHouse = qobject_cast<RParameter *>(value);
}

QObject * RLifeQualityReductionModel::getUnitCostOfHavingHouseWithoutPower() const
{
	return theUnitCostOfHavingHouseWithoutPower.data();
}

void RLifeQualityReductionModel::setUnitCostOfHavingHouseWithoutPower(QObject *value)
{
	theUnitCostOfHavingHouseWithoutPower = qobject_cast<RParameter *>(value);
}
QObject * RLifeQualityReductionModel::getUnitCostOfHavingHouseWithoutWater() const
{
	return theUnitCostOfHavingHouseWithoutWater.data();
}

void RLifeQualityReductionModel::setUnitCostOfHavingHouseWithoutWater(QObject *value)
{
	theUnitCostOfHavingHouseWithoutWater = qobject_cast<RParameter *>(value);
}

QObject * RLifeQualityReductionModel::getUnitCostOfHavingHouseWithoutPowerAndWater() const
{
	return theUnitCostOfHavingHouseWithoutPowerAndWater.data();
}

void RLifeQualityReductionModel::setUnitCostOfHavingHouseWithoutPowerAndWater(QObject *value)
{
	theUnitCostOfHavingHouseWithoutPowerAndWater = qobject_cast<RParameter *>(value);
}
int RLifeQualityReductionModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
{
	// Checking if all pointer properties are given by the user
	int status = this->checkProperties();
	// Error handling
	if (status < 0) {
		// Printing an error message to the output pane
		rCritical(threadID) << "Error in" << this->objectName() << ":"
			<< "Could not find all the passed objects";
		// Returning the error code
		return -1;
	}

	//Check the population parameters
	QList<RParameter *> popList;
	for (int i = 0; i< thePopulationList.count(); i++) {
		RParameter *parameter = qobject_cast<RParameter *>(thePopulationList[i].data());
		if (parameter) {
			popList << parameter;
		}
		else {
			// Issue an error if the parameter is not a parameter
			rCritical() << "Error in" << objectName() << ": The parameter %1 is not a parameter. Please correct and try again";
			return -1;
		}
	}



	//Check the repair status parameters
	QList<RParameter *> repairStatusList;
	for (int i = 0; i< theRepairOrRestorationStatusList.count(); i++) {
		RParameter *parameter = qobject_cast<RParameter *>(theRepairOrRestorationStatusList[i].data());
		if (parameter) {
			repairStatusList << parameter;
		}
		else {
			// Issue an error if the parameter is not a parameter
			rCritical() << "Error in" << objectName() << ": The parameter %1 is not a parameter. Please correct and try again";
			return -1;
		}
	}

	//Check the power status parameters
	QList<RParameter *> powerStatusList;
	for (int i = 0; i< thePowerServiceabilityList.count(); i++) {
		RParameter *parameter = qobject_cast<RParameter *>(thePowerServiceabilityList[i].data());
		if (parameter) {
			powerStatusList << parameter;
		}
		else {
			// Issue an error if the parameter is not a parameter
			rCritical() << "Error in" << objectName() << ": The parameter %1 is not a parameter. Please correct and try again";
			return -1;
		}
	}

	//Check the water status parameters
	QList<RParameter *> waterStatusList;
	for (int i = 0; i< theWaterServiceabilityList.count(); i++) {
		RParameter *parameter = qobject_cast<RParameter *>(theWaterServiceabilityList[i].data());
		if (parameter) {
			waterStatusList << parameter;
		}
		else {
			// Issue an error if the parameter is not a parameter
			rCritical() << "Error in" << objectName() << ": The parameter %1 is not a parameter. Please correct and try again";
			return -1;
		}
	}
	
	//Check the number of input in all lists
	if (popList.count() != repairStatusList.count()) {
		rCritical(threadID) << "Error in" << this->objectName() << ":"
			<< "Please enter as many repair or restoration parameter parameters as the number of population parameters";
		return -1;
	}
	if (popList.count() != powerStatusList.count()) {
		rCritical(threadID) << "Error in" << this->objectName() << ":"
			<< "Please enter as many power serviceability parameters as the number of population parameters";
		return -1;
	}
	if (popList.count() != waterStatusList.count()) {
		rCritical(threadID) << "Error in" << this->objectName() << ":"
			<< "Please enter as many water serviceability parameter as the number of population parameters";
		return -1;
	}
	
	//Algorithm
	double unitCostNoHouse = theUnitCostOfHavingNoHouse.data()->getCurrentValue(threadID);
	double unitCostNoPower = theUnitCostOfHavingHouseWithoutPower.data()->getCurrentValue(threadID);
	double unitCostNoWater = theUnitCostOfHavingHouseWithoutWater.data()->getCurrentValue(threadID);
	double unitCostNoPowerNoWater = theUnitCostOfHavingHouseWithoutPowerAndWater.data()->getCurrentValue(threadID);

	currentTime[threadID] = theTime.data()->getCurrentValue(threadID);

	//Calculate the number of people without house
	double numNoHouse = 0;
	for (int i = 0; i < popList.count(); i++) {
		double pop = popList[i]->getCurrentValue(threadID);
		double repairStatus = repairStatusList[i]->getCurrentValue(threadID);

		numNoHouse = numNoHouse + pop * (1 - repairStatus);
	}
	currentPopWithoutHouse[threadID] = numNoHouse;

	//Calculate the number of people  with house and Water but without power 
	double numNoPower = 0;
	for (int i = 0; i < popList.count(); i++) {
		double pop = popList[i]->getCurrentValue(threadID);
		double repairStatus = repairStatusList[i]->getCurrentValue(threadID);
		double powerStatus = powerStatusList[i]->getCurrentValue(threadID);
		double waterStatus = waterStatusList[i]->getCurrentValue(threadID);

		numNoPower = numNoPower + pop * repairStatus * (1 - powerStatus) * waterStatus;
	}
	currentPopWithoutPower[threadID] = numNoPower;
	
	//Calculate the number of people  with house and power but without water
	double numNoWater = 0;
	for (int i = 0; i < popList.count(); i++) {
		double pop = popList[i]->getCurrentValue(threadID);
		double repairStatus = repairStatusList[i]->getCurrentValue(threadID);
		double powerStatus = powerStatusList[i]->getCurrentValue(threadID);
		double waterStatus = waterStatusList[i]->getCurrentValue(threadID);

		numNoWater = numNoWater + pop * repairStatus * powerStatus * (1 - waterStatus);
	}
	currentPopWithoutWater[threadID] = numNoWater;

	//Calculate the number of people with house but without power and water
	double numNoPowerNoWater = 0;
	for (int i = 0; i < popList.count(); i++) {
		double pop = popList[i]->getCurrentValue(threadID);
		double repairStatus = repairStatusList[i]->getCurrentValue(threadID);
		double powerStatus = powerStatusList[i]->getCurrentValue(threadID);
		double waterStatus = waterStatusList[i]->getCurrentValue(threadID);

		numNoPowerNoWater = numNoPowerNoWater + pop * repairStatus * (1 - powerStatus) * (1 - waterStatus);
	}
	currentPopWithoutPowerAndWater[threadID] = numNoPowerNoWater;

	//Calculate the cost for each category of people
	double costNoHouse = unitCostNoHouse * previousPopWithoutHouse[threadID] * (currentTime[threadID] - previousTime[threadID]);
	double costNoPower = unitCostNoPower * previousPopWithoutPower[threadID] * (currentTime[threadID] - previousTime[threadID]);
	double costNoWater = unitCostNoWater * previousPopWithoutWater[threadID] * (currentTime[threadID] - previousTime[threadID]);
	double costNoPowerNoWater = unitCostNoPowerNoWater * previousPopWithoutPowerAndWater[threadID] * (currentTime[threadID] - previousTime[threadID]);


	cumulativeLifeQualityReduction[threadID] += costNoHouse + costNoPower + costNoWater + costNoPowerNoWater;

	LifeQualityReductionCost->setCurrentValue(cumulativeLifeQualityReduction[threadID], threadID);

	PopulationWithoutHouse->setCurrentValue(currentPopWithoutHouse[threadID], threadID);
	PopulationWithoutOnlyPower->setCurrentValue(currentPopWithoutPower[threadID], threadID);
	PopulationWithoutOnlyWater->setCurrentValue(currentPopWithoutWater[threadID], threadID);
	PopulationWithoutPowerAndWater->setCurrentValue(currentPopWithoutPowerAndWater[threadID], threadID);
	
	//After evaluating the response the previous state should be set to the current state
	previousPopWithoutHouse[threadID] = currentPopWithoutHouse[threadID];
	previousPopWithoutPower[threadID] = currentPopWithoutPower[threadID];
	previousPopWithoutWater[threadID] = currentPopWithoutWater[threadID];
	previousPopWithoutPowerAndWater[threadID] = currentPopWithoutPowerAndWater[threadID];

	previousTime[threadID] = currentTime[threadID];


	return 0;
}

void RLifeQualityReductionModel::reset()
{
	theInitialTime = theTime.data()->getCurrentValue();
}

void RLifeQualityReductionModel::resetHistoryVariables(int threadID)
{
	currentTime[threadID] = theInitialTime;
	previousTime[threadID] = theInitialTime;

	currentPopWithoutHouse[threadID] = 0;
	previousPopWithoutHouse[threadID] = 0;

	currentPopWithoutPower[threadID] = 0;
	previousPopWithoutPower[threadID] = 0;

	currentPopWithoutWater[threadID] = 0;
	previousPopWithoutWater[threadID] = 0;

	currentPopWithoutPowerAndWater[threadID] = 0;
	previousPopWithoutPowerAndWater[threadID] = 0;

	cumulativeLifeQualityReduction[threadID] = 0;

}

bool RLifeQualityReductionModel::canBeRunParallelly() {
	return true;
}