#include "RSimpleBuildingRepairCostModel.h"

#include "RParameter.h"
#include "RResponse.h"

RSimpleBuildingRepairCostModel::RSimpleBuildingRepairCostModel(QObject *parent, QString name)
	: RModel(parent, name)
{
	// Initializing the podoubleer data members
	//$$ theDamageFactor = 0;
	theFootprintArea = 100;
	//$$ theUnitAreaCost = 0;
	theNumberOfStories = 1;
	//$$ theEpsilon = 0;
	
	// Instantiating the output response theRepairCostResponse
	theDomain->createObject(objectName() + "Response", "RGenericResponse");
	// Setting theRepairCostResponse podoubleer
	theRepairCostResponse = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	// Setting the "model" property of theRepairCostResponse to be "this" object
	theRepairCostResponse->setModel(this);
}

RSimpleBuildingRepairCostModel::~RSimpleBuildingRepairCostModel()
{

}

QObject * RSimpleBuildingRepairCostModel::getDamageFactor() const	
{	
	return theDamageFactor.data();
}	

void RSimpleBuildingRepairCostModel::setDamageFactor(QObject *value)	
{	
	theDamageFactor = qobject_cast<RParameter *>(value);
}	

double RSimpleBuildingRepairCostModel::getFootprintArea() const	
{	
	return theFootprintArea;
}	

void RSimpleBuildingRepairCostModel::setFootprintArea(double value)	
{	
	theFootprintArea = value;
}	

QObject *RSimpleBuildingRepairCostModel::getUnitAreaCost() const	
{	
	return theUnitAreaCost.data();
}	

void RSimpleBuildingRepairCostModel::setUnitAreaCost(QObject *value)	
{	
	theUnitAreaCost = qobject_cast<RParameter *>(value);
}	

double RSimpleBuildingRepairCostModel::getNumberOfStories() const	
{	
	return theNumberOfStories;
}	

void RSimpleBuildingRepairCostModel::setNumberOfStories(double value)	
{	
	theNumberOfStories = value;
}	

QObject * RSimpleBuildingRepairCostModel::getEpsilon() const	
{	
	return theEpsilon.data();
}	

void RSimpleBuildingRepairCostModel::setEpsilon(QObject *value)	
{	
	theEpsilon = qobject_cast<RParameter *>(value);
}	

QObject * RSimpleBuildingRepairCostModel::getRepairStatus() const
{
	return theRepairStatus.data();
}

void RSimpleBuildingRepairCostModel::setRepairStatus(QObject *value)
{
	theRepairStatus = qobject_cast<RParameter *>(value);
}

int RSimpleBuildingRepairCostModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
{	
	// This is where the code for the model is written!
	double damageFactor = theDamageFactor.data()->getCurrentValue(threadID);
	double unitAreaCost = theUnitAreaCost.data()->getCurrentValue(threadID);
	double Epsilon = theEpsilon.data()->getCurrentValue(threadID);

	double repairCost = damageFactor * theNumberOfStories * theFootprintArea * unitAreaCost + Epsilon;
	if (repairCost < 0) {
		repairCost = 0;
		rDebug(threadID) << "Warning: The repair cost of the model" << objectName() << "is less than 0. Rt is setting it to 0.";
	}

	//If the repairStatus is zero, i.e., the repair opeation has not been done yet, the reapir cost is zero. Once the reapir operation is done the cost is added. 
	double repairStatus = theRepairStatus.data()->getCurrentValue(threadID);
	repairCost = repairCost * repairStatus;

	// Updating the response current value
	theRepairCostResponse->setCurrentValue(repairCost,threadID);

	if (evaluateDDM) {
		double dRepairCostResponse_dDamageFactor = theNumberOfStories * theFootprintArea * unitAreaCost;
		double dRepairCostResponse_dUnitAreaCost = theNumberOfStories * theFootprintArea * damageFactor;
		double dRepairCostResponse_dEpsilon = 1.0;

		QMap<RParameter *, double> theRepairCostGradientMap;
		theRepairCostGradientMap.insertMulti(theDamageFactor.data(), dRepairCostResponse_dDamageFactor);
		theRepairCostGradientMap.insertMulti(theUnitAreaCost.data(), dRepairCostResponse_dUnitAreaCost);
		theRepairCostGradientMap.insertMulti(theEpsilon.data(), dRepairCostResponse_dEpsilon);

		theDDMMap.insertMulti(theRepairCostResponse, theRepairCostGradientMap);
	}

	return 1;
}

bool RSimpleBuildingRepairCostModel::canBeRunParallelly(){
	return true;
}