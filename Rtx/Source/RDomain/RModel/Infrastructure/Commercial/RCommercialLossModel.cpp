#include "RCommercialLossModel.h"	

#include "RParameter.h"	
#include "RResponse.h"	
#include "RCommercialInformationModel.h"
#include <gsl/gsl_cdf.h>
#include "RTime.h"

#include <math.h>	

RCommercialLossModel::RCommercialLossModel(QObject *parent, QString name)
	: RModel(parent, name)
{
	int arraySizeForParallelizedObject = getArraySizeForParallelizedObject();

	//theOperationCapacity = new int[arraySizeForParallelizedObject];
	currentTime = new double[arraySizeForParallelizedObject];
	previousTime = new double[arraySizeForParallelizedObject];
	currentRevenue = new double[arraySizeForParallelizedObject];
	previousPotentialRevenue = new double[arraySizeForParallelizedObject];
	cumulativeCommercialLoss = new double[arraySizeForParallelizedObject];

	// Instantiating the response object
	theDomain->createObject(objectName() + "LossResponse", "RGenericResponse");
	theCommercialLoss = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theCommercialLoss->setModel(this);

	// Instantiating the response object
	theDomain->createObject(objectName() + "PotentialRevenueResponse", "RGenericResponse");
	theCurrentPotentialRevenue = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theCurrentPotentialRevenue->setModel(this);
	;
	theDomain->createObject(objectName() + "AbsoluteRevenueResponse", "RGenericResponse");
	theCurrentAbsoluteRevenue = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theCurrentAbsoluteRevenue->setModel(this);

}

RCommercialLossModel::~RCommercialLossModel()
{
	delete[] currentTime;
	delete[] previousTime;
	delete[] currentRevenue;
	delete[] previousPotentialRevenue;
	delete[] cumulativeCommercialLoss;

}

QObject * RCommercialLossModel::getCommercialInformationModel() const
{
	return theCommercialInformationModel.data();
}

void RCommercialLossModel::setCommercialInformationModel(QObject *value)
{
	theCommercialInformationModel = qobject_cast<RModel *>(value);
}

QObject * RCommercialLossModel::getNormalDailyRevenue() const
{
	return theNormalDailyRevenue.data();
}

void RCommercialLossModel::setNormalDailyRevenue(QObject *value)
{
	theNormalDailyRevenue = qobject_cast<RParameter *>(value);
}

QObject * RCommercialLossModel::getRecaptureFactor() const
{
	return theRecaptureFactor.data();
}

void RCommercialLossModel::setRecaptureFactor(QObject *value)
{
	theRecaptureFactor = qobject_cast<RParameter *>(value);
}

QObject * RCommercialLossModel::getStructuralDamageRatio() const
{
	return theStructuralDamageRatio.data();
}

void RCommercialLossModel::setStructuralDamageRatio(QObject *value)
{
	theStructuralDamageRatio = qobject_cast<RParameter *>(value);
}

QObject * RCommercialLossModel::getBuildingRepairStatus() const
{
	return theBuildingRepairStatus.data();
}

void RCommercialLossModel::setBuildingRepairStatus(QObject *value)
{
	theBuildingRepairStatus = qobject_cast<RParameter *>(value);
}

QObject * RCommercialLossModel::getBuildingRestorationStatus() const
{
	return theBuildingRestorationStatus.data();
}

void RCommercialLossModel::setBuildingRestorationStatus(QObject *value)
{
	theBuildingRestorationStatus = qobject_cast<RParameter *>(value);
}


QObject * RCommercialLossModel::getPowerServiceability() const
{
	return thePowerServiceability.data();
}

void RCommercialLossModel::setPowerServiceability(QObject *value)
{
	thePowerServiceability = qobject_cast<RParameter *>(value);
}

QObject * RCommercialLossModel::getWaterServiceability() const
{
	return theWaterServiceability.data();
}

void RCommercialLossModel::setWaterServiceability(QObject *value)
{
	theWaterServiceability = qobject_cast<RParameter *>(value);
}

QObject * RCommercialLossModel::getBuildingDependency() const
{
	return theBuildingDependency.data();
}

void RCommercialLossModel::setBuildingDependency(QObject *value)
{
	theBuildingDependency = qobject_cast<RParameter *>(value);
}

QObject * RCommercialLossModel::getPowerDependency() const
{
	return thePowerDependency.data();
}

void RCommercialLossModel::setPowerDependency(QObject *value)
{
	thePowerDependency = qobject_cast<RParameter *>(value);
}

QObject * RCommercialLossModel::getWaterDependency() const
{
	return theWaterDependency.data();
}

void RCommercialLossModel::setWaterDependency(QObject *value)
{
	theWaterDependency = qobject_cast<RParameter *>(value);
}

QObject * RCommercialLossModel::getTimeParameter() const
{
	return theTimeParameter.data();
}
void RCommercialLossModel::setTimeParameter(QObject *value)
{
	theTimeParameter = qobject_cast<RTime *>(value);
}

int RCommercialLossModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
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


	// Getting the information from the CIM
	RCommercialInformationModel *theCIM = qobject_cast<RCommercialInformationModel *>(theCommercialInformationModel.data());
	if (!theCIM) {
		rCritical(threadID) << "Error in model" << objectName() << ":" << theCommercialInformationModel.data()->objectName() << "is not a Commercial information model.";
		return -1;
	}
	
	double normalDailyRevenue = theNormalDailyRevenue.data()->getCurrentValue(threadID);
	double recaptureFactor = theRecaptureFactor.data()->getCurrentValue(threadID);
	double structalDamageRatio = theStructuralDamageRatio.data()->getCurrentValue(threadID);

	double BuildingRepairStatus = theBuildingRepairStatus.data()->getCurrentValue(threadID);
	double BuildingRestorationStatus = theBuildingRestorationStatus.data()->getCurrentValue(threadID);
	double powerServiceability = thePowerServiceability.data()->getCurrentValue(threadID);
	double waterServiceability = theWaterServiceability.data()->getCurrentValue(threadID);

	double buildingDependency = theBuildingDependency.data()->getCurrentValue(threadID);
	double powerDependency = thePowerDependency.data()->getCurrentValue(threadID);
	double waterDependency = theWaterDependency.data()->getCurrentValue(threadID);

	//The dependency values must be between 0 and 1
	if (buildingDependency < 0) {
		buildingDependency = 0;
	}
	else if (buildingDependency > 1) {
		buildingDependency = 1;
	}

	if (powerDependency < 0) {
		powerDependency = 0;
	}
	else if (powerDependency > 1) {
		powerDependency = 1;
	}

	if (waterDependency < 0) {
		waterDependency = 0;
	}
	else if (waterDependency > 1) {
		waterDependency = 1;
	}

	//Check if the RecaptureFactor is over 1 or below 0
	if (recaptureFactor < 0) {
		recaptureFactor = 0;
	}
	else if (recaptureFactor > 1) {
		recaptureFactor = 1;
	}
	

	//A variable that shows the reduction factor due to the building, power, water unserviceability
	double buildingLossFactor = 1;
	double powerLossFactor = 1;
	double waterLossFactor = 1;

	//Extended from FEMA 2012 formula 15-15
	if ((BuildingRepairStatus != 1) && (BuildingRestorationStatus != 1)) {
		//buildingLossFactor = (1 - BuildingRepairStatus)*(1 - buildingDependency)*(2 - structalDamageRatio)/2;
		buildingLossFactor = recaptureFactor*(1 - buildingDependency);

	} else if ((BuildingRepairStatus != 1) && (BuildingRestorationStatus == 1)) {
		buildingLossFactor = recaptureFactor;
	} else {
		if (powerServiceability != 1) {
			powerLossFactor = (1 - powerServiceability)*(1 - powerDependency);
		}

		if (waterServiceability != 1) {
			waterLossFactor = (1 - waterServiceability)*(1 - waterDependency);
		}
	}

	//Evaluating the current state
	currentRevenue[threadID] = normalDailyRevenue*buildingLossFactor*powerLossFactor*waterLossFactor;
	currentTime[threadID] = theTimeParameter.data()->getCurrentValue(threadID);
	
	//The absolute revenue is the one that is directly computed based on the only current condition 
	//The Potential revenue is the one that is computed based on the current condition and previous condition - the potential revenue at one stage should be more than its previous stage as the recovey process is under completion

	double currentAbsoluteRevenue = currentRevenue[threadID];

	//As the should be positively ascending after the hazard, it can not descend and get values less than its previous state
	if (currentRevenue[threadID] < previousPotentialRevenue[threadID]) 
		currentRevenue[threadID] = previousPotentialRevenue[threadID];
	
	cumulativeCommercialLoss[threadID] += (normalDailyRevenue - previousPotentialRevenue[threadID])*(currentTime[threadID] - previousTime[threadID]);

	theCommercialLoss->setCurrentValue(cumulativeCommercialLoss[threadID], threadID);
	theCurrentPotentialRevenue->setCurrentValue(currentRevenue[threadID], threadID);
	theCurrentAbsoluteRevenue->setCurrentValue(currentAbsoluteRevenue, threadID);


	//After evaluating the response the previous state should be set to the current state
	previousPotentialRevenue[threadID] = currentRevenue[threadID];
	previousTime[threadID] = currentTime[threadID];

	
	return 0;
}

bool RCommercialLossModel::canBeRunParallelly() {
	return true;
}
void RCommercialLossModel::resetHistoryVariables(int threadID)
{
	cumulativeCommercialLoss[threadID] = 0;
	
	currentTime[threadID] = theInitialTime;
	previousTime[threadID] = theInitialTime;

	currentRevenue[threadID] = theNormalDailyRevenue.data()->getCurrentValue(threadID);
	previousPotentialRevenue[threadID] = 0.0; /*theNormalDailyRevenue.data()->getCurrentValue(threadID);*/
}
void RCommercialLossModel::reset()
{
	theInitialTime = theTimeParameter.data()->getCurrentValue();
	theCurrentPotentialRevenue->setCurrentValue(theNormalDailyRevenue.data()->getCurrentValue());
}