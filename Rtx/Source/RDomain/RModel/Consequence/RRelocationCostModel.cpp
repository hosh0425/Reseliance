#include "RRelocationCostModel.h"	

#include "RParameter.h"	
#include "RResponse.h"	
#include "RTime.h"

#include <gsl/gsl_math.h>

#include <math.h>	

RRelocationCostModel::RRelocationCostModel(QObject *parent, QString name)
	: RModel(parent, name)
{
	int arraySizeForParallelizedObject = getArraySizeForParallelizedObject();

	currentTime = new double[arraySizeForParallelizedObject];
	previousTime = new double[arraySizeForParallelizedObject];
	currentDemand = new double[arraySizeForParallelizedObject];
	previousDemand = new double[arraySizeForParallelizedObject];
	cumulativeRelocationCost = new double[arraySizeForParallelizedObject];

	// Instantiating the response object
	theDomain->createObject(objectName() + "DemandResponse", "RGenericResponse");
	theRelocationDemand = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theRelocationDemand->setModel(this);

	theDomain->createObject(objectName() + "CostResponse", "RGenericResponse");
	theRelocationCost = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theRelocationCost->setModel(this);
}
RRelocationCostModel::~RRelocationCostModel()
{
	delete[] currentTime;
	delete[] previousTime;
	delete[] currentDemand;
	delete[] previousDemand;
	delete[] cumulativeRelocationCost;

}

QObject * RRelocationCostModel::getTime() const
{
	return theTime.data();
}

void RRelocationCostModel::setTime(QObject *value)
{
	theTime = qobject_cast<RParameter *>(value);
}

QString RRelocationCostModel::getPopulationList()
{
	//Check the population parameters
	QList<RPointer<RParameter> > maxPopList;
	for (int i = 0; i< thePopulationList.count(); i++) {
		RParameter *parameter = qobject_cast<RParameter *>(thePopulationList[i].data());
		if (parameter) {
			maxPopList << parameter;
		}
		else {
			// Issue an error if the parameter is not a parameter
			rCritical() << "Error in" << objectName() << ": The parameter %1 is not a parameter. Please correct and try again";

		}
	}

	return parameterListToString(maxPopList);
}

void RRelocationCostModel::setPopulationList(QString value)
{
	thePopulationList = stringToParameterList(value);
}

QString RRelocationCostModel::getRepairOrRestorationStatusList()
{
	//Check the repair status parameters
	QList<RPointer<RParameter> > statusList;
	for (int i = 0; i< theRepairOrRestorationStatusList.count(); i++) {
		RParameter *parameter = qobject_cast<RParameter *>(theRepairOrRestorationStatusList[i].data());
		if (parameter) {
			statusList << parameter;
		}
		else {
			// Issue an error if the parameter is not a parameter
			rCritical() << "Error in" << objectName() << ": The parameter %1 is not a parameter. Please correct and try again";

		}
	}

	return parameterListToString(statusList);
}

void RRelocationCostModel::setRepairOrRestorationStatusList(QString value)
{
	theRepairOrRestorationStatusList = stringToParameterList(value);
}

QObject * RRelocationCostModel::getRelocationCostPerDayPerPerson() const
{
	return theRelocationCostPerDayPerPerson.data();
}

void RRelocationCostModel::setRelocationCostPerDayPerPerson(QObject *value)
{
	theRelocationCostPerDayPerPerson = qobject_cast<RParameter *>(value);
}

int RRelocationCostModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
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

	if (thePopulationList.count() != theRepairOrRestorationStatusList.count()) {
		rCritical(threadID) << "Error in" << this->objectName() << ":"
			<< "Please enter as many population list as the number of repair or restoration status list";
		return -1;
	}

	currentTime[threadID] = theTime.data()->getCurrentValue(threadID);
	double unitRelocationCost = theRelocationCostPerDayPerPerson.data()->getCurrentValue(threadID);

	double demand = 0;
	for (int i = 0; i < thePopulationList.count(); i++) {
		double pop = thePopulationList[i].data()->getCurrentValue(threadID);
		double status = theRepairOrRestorationStatusList[i].data()->getCurrentValue(threadID);
		demand = demand + pop * (1 - status);
	}
	currentDemand[threadID] = demand;

	cumulativeRelocationCost[threadID] += unitRelocationCost * (previousDemand[threadID]) * (currentTime[threadID] - previousTime[threadID]);

	theRelocationDemand->setCurrentValue(demand, threadID);
	theRelocationCost->setCurrentValue(cumulativeRelocationCost[threadID], threadID);


	//After evaluating the response the previous state should be set to the current state
	previousDemand[threadID] = currentDemand[threadID];
	previousTime[threadID] = currentTime[threadID];


	return 0;
}

void RRelocationCostModel::reset()
{
	theInitialTime = theTime.data()->getCurrentValue();
}

void RRelocationCostModel::resetHistoryVariables(int threadID)
{
	cumulativeRelocationCost[threadID] = 0;

	currentTime[threadID] = theInitialTime;
	previousTime[threadID] = theInitialTime;

	currentDemand[threadID] = 0;
	previousDemand[threadID] = 0;
}

bool RRelocationCostModel::canBeRunParallelly() {
	return true;
}