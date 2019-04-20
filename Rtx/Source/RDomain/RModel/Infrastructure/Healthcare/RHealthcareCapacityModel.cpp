#include "RHealthcareCapacityModel.h"	

#include "RParameter.h"	
#include "RResponse.h"	
#include "RHealthcareInformationModel.h"
#include <gsl/gsl_cdf.h>
#include "RTime.h"

#include <math.h>	

RHealthcareCapacityModel::RHealthcareCapacityModel(QObject *parent, QString name)
	: RModel(parent, name)
{

	int arraySizeForParallelizedObject = getArraySizeForParallelizedObject();

	previousPotentialCapacity = new int[arraySizeForParallelizedObject];

	// Instantiating the response object
	theDomain->createObject(objectName() + "PotentialResponse", "RGenericResponse");
	theHealthcarePotentialCapacity = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theHealthcarePotentialCapacity->setModel(this);
	
	theDomain->createObject(objectName() + "AvailableResponse", "RGenericResponse");
	theHealthcareAvailableCapacity = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theHealthcareAvailableCapacity->setModel(this);

	theDomain->createObject(objectName() + "AbsoluteResponse", "RGenericResponse");
	theHealthcareAbsoluteCapacity = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theHealthcareAbsoluteCapacity->setModel(this);

}

RHealthcareCapacityModel::~RHealthcareCapacityModel()
{
	delete[] previousPotentialCapacity;
}

QObject * RHealthcareCapacityModel::getHealthcareInformationModel() const
{
	return theHealthcareInformationModel.data();
}

void RHealthcareCapacityModel::setHealthcareInformationModel(QObject *value)
{
	theHealthcareInformationModel = qobject_cast<RModel *>(value);
}

QObject * RHealthcareCapacityModel::getStructuralDamageRatio() const
{
	return theStructuralDamageRatio.data();
}

void RHealthcareCapacityModel::setStructuralDamageRatio(QObject *value)
{
	theStructuralDamageRatio = qobject_cast<RParameter *>(value);
}

QObject * RHealthcareCapacityModel::getNonstructuralDriftDamageRatio() const
{
	return theNonstructuralDriftDamageRatio.data();
}

void RHealthcareCapacityModel::setNonstructuralDriftDamageRatio(QObject *value)
{
	theNonstructuralDriftDamageRatio = qobject_cast<RParameter *>(value);
}

QObject * RHealthcareCapacityModel::getNonstructuralAccelerationDamageRatio() const
{
	return theNonstructuralAccelerationDamageRatio.data();
}

void RHealthcareCapacityModel::setNonstructuralAccelerationDamageRatio(QObject *value)
{
	theNonstructuralAccelerationDamageRatio = qobject_cast<RParameter *>(value);
}

QObject * RHealthcareCapacityModel::getContentDamageRatio() const
{
	return theContentDamageRatio.data();
}

void RHealthcareCapacityModel::setContentDamageRatio(QObject *value)
{
	theContentDamageRatio = qobject_cast<RParameter *>(value);
}

QObject * RHealthcareCapacityModel::getNormalDemand() const
{
	return theNormalDemand.data();
}

void RHealthcareCapacityModel::setNormalDemand(QObject *value)
{
	theNormalDemand = qobject_cast<RParameter *>(value);
}

QObject * RHealthcareCapacityModel::getRepairStatus() const
{
	return theRepairStatus.data();
}

void RHealthcareCapacityModel::setRepairStatus(QObject *value)
{
	theRepairStatus = qobject_cast<RParameter *>(value);
}


QObject * RHealthcareCapacityModel::getRestorationStatus() const
{
	return theRestorationStatus.data();
}

void RHealthcareCapacityModel::setRestorationStatus(QObject *value)
{
	theRestorationStatus = qobject_cast<RParameter *>(value);
}

QObject * RHealthcareCapacityModel::getRecaptureFactor() const
{
	return theRecaptureFactor.data();
}

void RHealthcareCapacityModel::setRecaptureFactor(QObject *value)
{
	theRecaptureFactor = qobject_cast<RParameter *>(value);
}

QObject * RHealthcareCapacityModel::getPowerServiceability() const
{
	return thePowerServiceability.data();
}

void RHealthcareCapacityModel::setPowerServiceability(QObject *value)
{
	thePowerServiceability = qobject_cast<RParameter *>(value);
}

QObject * RHealthcareCapacityModel::getWaterServiceability() const
{
	return theWaterServiceability.data();
}

void RHealthcareCapacityModel::setWaterServiceability(QObject *value)
{
	theWaterServiceability = qobject_cast<RParameter *>(value);
}

QObject * RHealthcareCapacityModel::getPowerDependency() const
{
	return thePowerDependency.data();
}

void RHealthcareCapacityModel::setPowerDependency(QObject *value)
{
	thePowerDependency = qobject_cast<RParameter *>(value);
}

QObject * RHealthcareCapacityModel::getWaterDependency() const
{
	return theWaterDependency.data();
}

void RHealthcareCapacityModel::setWaterDependency(QObject *value)
{
	theWaterDependency = qobject_cast<RParameter *>(value);
}

int RHealthcareCapacityModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
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


	// Getting the information from the HIM
	RHealthcareInformationModel *theHIM = qobject_cast<RHealthcareInformationModel *>(theHealthcareInformationModel.data());
	if (!theHIM) {
		rCritical(threadID) << "Error in model" << objectName() << ":" << theHealthcareInformationModel.data()->objectName() << "is not a Healthcare information model.";
		return -1;
	}

	double maxCapacity = theHIM->getNumberOfBeds();
	double NormalDemand = theNormalDemand.data()->getCurrentValue(threadID);

	double DFs = theStructuralDamageRatio.data()->getCurrentValue(threadID);
	double DFnd = theNonstructuralDriftDamageRatio.data()->getCurrentValue(threadID);
	double DFna = theNonstructuralAccelerationDamageRatio.data()->getCurrentValue(threadID);
	double DFc = theContentDamageRatio.data()->getCurrentValue(threadID);

	if (DFs > 1)  DFs = 1;
	if (DFs < 0)  DFs = 0;

	if (DFnd > 1)  DFnd = 1;
	if (DFnd < 0)  DFnd = 0;

	if (DFna > 1)  DFna = 1;
	if (DFna < 0)  DFna = 0;

	if (DFc > 1)  DFc = 1;
	if (DFc < 0)  DFc = 0;

	double repairStatus = theRepairStatus.data()->getCurrentValue(threadID);
	double restorationStatus = theRestorationStatus.data()->getCurrentValue(threadID);;
	double recaptureFactor = theRecaptureFactor.data()->getCurrentValue(threadID);
	double powerServiceability = thePowerServiceability.data()->getCurrentValue(threadID);
	double waterServiceability = theWaterServiceability.data()->getCurrentValue(threadID);

	double powerDependency = thePowerDependency.data()->getCurrentValue(threadID);
	double waterDependency = theWaterDependency.data()->getCurrentValue(threadID);
	
	double buildingLossFactor = 1;
	double powerLossFactor = 1;
	double waterLossFactor = 1;

	

	int currentCapacity = maxCapacity - NormalDemand;

	if ((repairStatus != 1) && (restorationStatus!=1)) {
		//buildingLossFactor = (1 - DFs)*(3 - DFnd - DFna - DFc)/3;
		buildingLossFactor = recaptureFactor / 2;
	}
	else if ((repairStatus != 1) && (restorationStatus == 1)) {
		//buildingLossFactor = (3 - DFnd - DFna - DFc) / 3;
		buildingLossFactor = recaptureFactor;
	}
	else {
		if (powerServiceability != 1) {
			powerLossFactor = (1 - powerServiceability)*(1 - powerDependency);
		}

		if (waterServiceability != 1) {
			waterLossFactor = (1 - waterServiceability)*(1 - waterDependency);
		}
	}


	currentCapacity = maxCapacity*buildingLossFactor*powerLossFactor*waterLossFactor - NormalDemand;

	if (currentCapacity < 0) currentCapacity = 0;

	int absoluteCapacity = currentCapacity;

	if (currentCapacity < previousPotentialCapacity[threadID]) currentCapacity = previousPotentialCapacity[threadID];

	int preAvailCap = theHealthcareAvailableCapacity->getCurrentValue(threadID);
	int availableCapacity = currentCapacity - previousPotentialCapacity[threadID] + preAvailCap;

	theHealthcarePotentialCapacity->setCurrentValue(currentCapacity, threadID);
	theHealthcareAvailableCapacity->setCurrentValue(availableCapacity, threadID);
	theHealthcareAbsoluteCapacity->setCurrentValue(absoluteCapacity, threadID);

	previousPotentialCapacity[threadID] = currentCapacity;

	return 0;
}

void RHealthcareCapacityModel::resetHistoryVariables(int threadID)
{
	previousPotentialCapacity[threadID] = 0;
	theHealthcareAvailableCapacity->setCurrentValue(0, threadID);

}

bool RHealthcareCapacityModel::canBeRunParallelly() {
	return true;
}