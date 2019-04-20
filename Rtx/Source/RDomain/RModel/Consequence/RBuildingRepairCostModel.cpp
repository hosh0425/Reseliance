#include "RBuildingRepairCostModel.h"

#include "RParameter.h"
#include "RResponse.h"
#include "RBuildingInformationModel.h"

#include <math.h>

RBuildingRepairCostModel::RBuildingRepairCostModel(QObject *parent, QString name)
	: RModel(parent, name)
{
	//$$ theBuildingInformationModel = 0;
	//$$ theStructuralDamageRatio = 0;
	//$$ theNonstructuralDriftDamageRatio = 0;
	//$$ theNonstructuralAccelerationDamageRatio = 0;
	//$$ theContentDamageRatio = 0;
	theStructuralUnitAreaCost = 0;
	theNonstructuralDriftUnitAreaCost = 0;
	theNonstructuralAccelerationUnitAreaCost = 0;
	theContentUnitAreaCost = 0;
	//$$ theTime = 0;
	//$$ theAnnualInterestRate = 0;
	//$$ theEpsilon = 0;
	theRepairabilityStructuralDamageRatio = 1.0;

	theDomain->createObject(objectName() + "Response", "RGenericResponse");
	theRepairCost = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theRepairCost->setModel(this);
}

RBuildingRepairCostModel::~RBuildingRepairCostModel()
{

}

QObject * RBuildingRepairCostModel::getBuildingInformationModel() const	
{	
	return theBuildingInformationModel.data();
}	
	
void RBuildingRepairCostModel::setBuildingInformationModel(QObject *value)	
{	
	theBuildingInformationModel = qobject_cast<RModel *>(value);	// Getting the information from the BIM
}	

QObject * RBuildingRepairCostModel::getStructuralDamageRatio() const	
{	
	return theStructuralDamageRatio.data();
}	
	
void RBuildingRepairCostModel::setStructuralDamageRatio(QObject *value)	
{	
	theStructuralDamageRatio = qobject_cast<RParameter *>(value);
}	
	
QObject * RBuildingRepairCostModel::getNonstructuralDriftDamageRatio() const	
{	
	return theNonstructuralDriftDamageRatio.data();
}	
	
void RBuildingRepairCostModel::setNonstructuralDriftDamageRatio(QObject *value)	
{	
	theNonstructuralDriftDamageRatio = qobject_cast<RParameter *>(value);
}	
	
QObject * RBuildingRepairCostModel::getNonstructuralAccelerationDamageRatio() const	
{	
	return theNonstructuralAccelerationDamageRatio.data();
}	
	
void RBuildingRepairCostModel::setNonstructuralAccelerationDamageRatio(QObject *value)	
{	
	theNonstructuralAccelerationDamageRatio = qobject_cast<RParameter *>(value);
}	
	
QObject * RBuildingRepairCostModel::getContentDamageRatio() const	
{	
	return theContentDamageRatio.data();
}	
	
void RBuildingRepairCostModel::setContentDamageRatio(QObject *value)	
{	
	theContentDamageRatio = qobject_cast<RParameter *>(value);
}

double RBuildingRepairCostModel::getStructuralUnitAreaCost() const	
{	
	return theStructuralUnitAreaCost;
}	
	
void RBuildingRepairCostModel::setStructuralUnitAreaCost(double value)	
{	
	theStructuralUnitAreaCost = value;
}	
	
double RBuildingRepairCostModel::getNonstructuralDriftUnitAreaCost() const	
{	
	return theNonstructuralDriftUnitAreaCost;
}	
	
void RBuildingRepairCostModel::setNonstructuralDriftUnitAreaCost(double value)	
{	
	theNonstructuralDriftUnitAreaCost = value;
}	
	
double RBuildingRepairCostModel::getNonstructuralAccelerationUnitAreaCost() const	
{	
	return theNonstructuralAccelerationUnitAreaCost;
}	
	
void RBuildingRepairCostModel::setNonstructuralAccelerationUnitAreaCost(double value)	
{	
	theNonstructuralAccelerationUnitAreaCost = value;
}	
	
double RBuildingRepairCostModel::getContentUnitAreaCost() const	
{	
	return theContentUnitAreaCost;
}	
	
void RBuildingRepairCostModel::setContentUnitAreaCost(double value)	
{	
	theContentUnitAreaCost = value;
}	

QObject * RBuildingRepairCostModel::getTime() const	
{	
	return theTime.data();
}	
	
void RBuildingRepairCostModel::setTime(QObject *value)	
{	
	theTime = qobject_cast<RParameter *>(value);
}	

QObject * RBuildingRepairCostModel::getAnnualInterestRate() const	
{	
	return theAnnualInterestRate.data();
}	
	
void RBuildingRepairCostModel::setAnnualInterestRate(QObject *value)	
{	
	theAnnualInterestRate = qobject_cast<RParameter *>(value);
}	
	
QObject * RBuildingRepairCostModel::getEpsilon() const	
{	
	return theEpsilon.data();
}	
	
void RBuildingRepairCostModel::setEpsilon(QObject *value)	
{	
	theEpsilon = qobject_cast<RParameter *>(value);
}

QObject * RBuildingRepairCostModel::getRepairStatus() const
{
	return theRepairStatus.data();
}

void RBuildingRepairCostModel::setRepairStatus(QObject *value)
{
	theRepairStatus = qobject_cast<RParameter *>(value);
}

double RBuildingRepairCostModel::getRepairabilityStructuralDamageRatio() const
{
	return theRepairabilityStructuralDamageRatio;
}

void RBuildingRepairCostModel::setRepairabilityStructuralDamageRatio(double value)
{
	theRepairabilityStructuralDamageRatio = value;
}

int RBuildingRepairCostModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
{	
	// Looking up structural damage model parameters
	double DFs = theStructuralDamageRatio.data()->getCurrentValue(threadID);
	double DFnd = theNonstructuralDriftDamageRatio.data()->getCurrentValue(threadID);
	double DFna = theNonstructuralAccelerationDamageRatio.data()->getCurrentValue(threadID);
	double DFc = theContentDamageRatio.data()->getCurrentValue(threadID);
	double currentTime = theTime.data()->getCurrentValue(threadID);
	double interestRate = theAnnualInterestRate.data()->getCurrentValue(threadID);
	double epsilon = theEpsilon.data()->getCurrentValue(threadID);
	double repairStatus = theRepairStatus.data()->getCurrentValue(threadID);

	//since damage ratios must be positive, they cannot be under zero
	if (DFs < 0.0) DFs = 0.0;
	if (DFnd < 0.0) DFnd = 0.0;
	if (DFna < 0.0) DFna = 0.0;
	if (DFc < 0.0) DFc = 0.0;

	//modify the damage ratios based on the repairability structural damage ratio
	if (DFs > theRepairabilityStructuralDamageRatio) {
		//since the damage ratios can be over 1, for those values the damage ratios would not need to be changed
		if (DFs < 1.0) DFs = 1.0;
		if (DFnd < 1.0) DFnd = 1.0;
		if (DFna < 1.0) DFna = 1.0;
		if (DFc < 1.0) DFc = 1.0;
	}


	// Getting the information from the BIM
	RBuildingInformationModel *theBIM = qobject_cast<RBuildingInformationModel *>(theBuildingInformationModel.data());
	if (!theBIM) {
		rCritical(threadID) << "Error in model" << objectName() << ":" << theBuildingInformationModel.data()->objectName() << "is not a building information model.";
		return -1;
	}
	double totalArea = theBIM->getTotalArea();

	double repairCost = (DFs * theStructuralUnitAreaCost + DFnd * theNonstructuralDriftUnitAreaCost + DFna * theNonstructuralAccelerationUnitAreaCost + DFc * theContentUnitAreaCost) * totalArea * epsilon;

	double futureDiscountFactor = 1.0;
	if (currentTime > theInitialTime) {
		//futureDiscountFactor = pow(1 + interestRate, -(currentTime - theInitialTime) / 365.0);
		futureDiscountFactor = exp(-interestRate * (currentTime - theInitialTime) / 365.0);
	}

	repairCost *= futureDiscountFactor;
	//If the repairStatus is zero, i.e., the repair opeation has not been done yet, the repair cost is zero. Once the repair operation is done the cost is added. 
	repairCost = repairCost * repairStatus;
	theRepairCost->setCurrentValue(repairCost,threadID);

	if (evaluateDDM) {
		// IMPLEMENT
	}

	return 0;
}

void RBuildingRepairCostModel::reset()
{
	theInitialTime = theTime.data()->getCurrentValue();
}

bool RBuildingRepairCostModel::canBeRunParallelly(){
	return true;
}