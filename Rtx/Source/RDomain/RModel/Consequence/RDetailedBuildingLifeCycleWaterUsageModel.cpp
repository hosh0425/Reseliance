#include "RDetailedBuildingLifeCycleWaterUsageModel.h"

#include "RParameter.h"
#include "RResponse.h"

RDetailedBuildingLifeCycleWaterUsageModel::RDetailedBuildingLifeCycleWaterUsageModel(QObject *parent, QString &name)
	: RModel(parent, name)
{
	// Initializing the pointer data members
/* //$$ 
	theConcreteQuantity = 0;
	theConcreteWIntensity = 0;
	theConcreteTransportationDistance = 0;
	theConcreteTransportationWIntensity = 0;
	theSteelQuantity = 0;
	theSteelWIntensity = 0;
	theSteelTransportationDistance = 0;
	theSteelTransportationWIntensity = 0;
	theWoodQuantity = 0;
	theWoodWIntensity = 0;
	theWoodTransportationDistance = 0;
	theWoodTransportationWIntensity = 0;
	theGypsumBoardQuantity = 0;
	theGypsumBoardWIntensity = 0;
	theGypsumBoardTransportationDistance = 0;
	theGypsumBoardTransportationWIntensity = 0;
	theVapourBarrierQuantity = 0;
	theVapourBarrierWIntensity = 0;
	theVapourBarrierTransportationDistance = 0;
	theVapourBarrierTransportationWIntensity = 0;
	theInsulationQuantity = 0;
	theInsulationWIntensity = 0;
	theInsulationTransportationDistance = 0;
	theInsulationTransportationWIntensity = 0;
	theGlassQuantity = 0;
	theGlassWIntensity = 0;
	theGlassTransportationDistance = 0;
	theGlassTransportationWIntensity = 0;
	theOtherMaterialQuantity = 0;
	theOtherMaterialWIntensity = 0;
	theOtherMaterialTransportationDistance = 0;
	theOtherMaterialTransportationWIntensity = 0;
	theWorkerHourWIntensity = 0;
	theTotalWorkerHours = 0;
	theDaysYearlyOperation = 0;
	theTotalBuildingOccupants = 0;
	theFlowFixture1WDemand = 0;
	theFlushMaleWDemand = 0;
	theFlushFemaleWDemand = 0;
	theRatioMaleOccupants = 0;
	theDesignLife = 0;    
*/
	// Instantiating the output response
	theDomain->createObject(objectName() + "Response", "RGenericResponse");
	// Setting theResponse pointer
	theWaterUsageFineResponse = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	// Setting the "model" property of theResponse to be "this" object
	theWaterUsageFineResponse->setModel(this);
}

RDetailedBuildingLifeCycleWaterUsageModel::~RDetailedBuildingLifeCycleWaterUsageModel()
{

}

QObject * RDetailedBuildingLifeCycleWaterUsageModel::getConcreteQuantity() const
{
	return theConcreteQuantity.data();
}

void RDetailedBuildingLifeCycleWaterUsageModel::setConcreteQuantity(QObject *value)
{
	theConcreteQuantity = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleWaterUsageModel::getConcreteWIntensity() const
{
	return theConcreteWIntensity.data();
}

void RDetailedBuildingLifeCycleWaterUsageModel::setConcreteWIntensity(QObject *value)
{
	theConcreteWIntensity = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleWaterUsageModel::getConcreteTransportationDistance() const
{
	return theConcreteTransportationDistance.data();
}

void RDetailedBuildingLifeCycleWaterUsageModel::setConcreteTransportationDistance(QObject *value)
{
	theConcreteTransportationDistance = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleWaterUsageModel::getConcreteTransportationWIntensity() const
{
	return theConcreteTransportationWIntensity.data();
}

void RDetailedBuildingLifeCycleWaterUsageModel::setConcreteTransportationWIntensity(QObject *value)
{
	theConcreteTransportationWIntensity = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleWaterUsageModel::getSteelQuantity() const
{
	return theSteelQuantity.data();
}

void RDetailedBuildingLifeCycleWaterUsageModel::setSteelQuantity(QObject *value)
{
	theSteelQuantity = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleWaterUsageModel::getSteelWIntensity() const
{
	return theSteelWIntensity.data();
}

void RDetailedBuildingLifeCycleWaterUsageModel::setSteelWIntensity(QObject *value)
{
	theSteelWIntensity = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleWaterUsageModel::getSteelTransportationDistance() const
{
	return theSteelTransportationDistance.data();
}

void RDetailedBuildingLifeCycleWaterUsageModel::setSteelTransportationDistance(QObject *value)
{
	theSteelTransportationDistance = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleWaterUsageModel::getSteelTransportationWIntensity() const
{
	return theSteelTransportationWIntensity.data();
}

void RDetailedBuildingLifeCycleWaterUsageModel::setSteelTransportationWIntensity(QObject *value)
{
	theSteelTransportationWIntensity = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleWaterUsageModel::getWoodQuantity() const
{
	return theWoodQuantity.data();
}

void RDetailedBuildingLifeCycleWaterUsageModel::setWoodQuantity(QObject *value)
{
	theWoodQuantity = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleWaterUsageModel::getWoodWIntensity() const
{
	return theWoodWIntensity.data();
}

void RDetailedBuildingLifeCycleWaterUsageModel::setWoodWIntensity(QObject *value)
{
	theWoodWIntensity = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleWaterUsageModel::getWoodTransportationDistance() const
{
	return theWoodTransportationDistance.data();
}

void RDetailedBuildingLifeCycleWaterUsageModel::setWoodTransportationDistance(QObject *value)
{
	theWoodTransportationDistance = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleWaterUsageModel::getWoodTransportationWIntensity() const
{
	return theWoodTransportationWIntensity.data();
}

void RDetailedBuildingLifeCycleWaterUsageModel::setWoodTransportationWIntensity(QObject *value)
{
	theWoodTransportationWIntensity = qobject_cast<RParameter *>(value);
}


QObject * RDetailedBuildingLifeCycleWaterUsageModel::getGypsumBoardQuantity() const
{
	return theGypsumBoardQuantity.data();
}

void RDetailedBuildingLifeCycleWaterUsageModel::setGypsumBoardQuantity(QObject *value)
{
	theGypsumBoardQuantity = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleWaterUsageModel::getGypsumBoardWIntensity() const
{
	return theGypsumBoardWIntensity.data();
}

void RDetailedBuildingLifeCycleWaterUsageModel::setGypsumBoardWIntensity(QObject *value)
{
	theGypsumBoardWIntensity = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleWaterUsageModel::getGypsumBoardTransportationDistance() const
{
	return theGypsumBoardTransportationDistance.data();
}

void RDetailedBuildingLifeCycleWaterUsageModel::setGypsumBoardTransportationDistance(QObject *value)
{
	theGypsumBoardTransportationDistance = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleWaterUsageModel::getGypsumBoardTransportationWIntensity() const
{
	return theGypsumBoardTransportationWIntensity.data();
}

void RDetailedBuildingLifeCycleWaterUsageModel::setGypsumBoardTransportationWIntensity(QObject *value)
{
	theGypsumBoardTransportationWIntensity = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleWaterUsageModel::getVapourBarrierQuantity() const
{
	return theVapourBarrierQuantity.data();
}

void RDetailedBuildingLifeCycleWaterUsageModel::setVapourBarrierQuantity(QObject *value)
{
	theVapourBarrierQuantity = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleWaterUsageModel::getVapourBarrierWIntensity() const
{
	return theVapourBarrierWIntensity.data();
}

void RDetailedBuildingLifeCycleWaterUsageModel::setVapourBarrierWIntensity(QObject *value)
{
	theVapourBarrierWIntensity = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleWaterUsageModel::getVapourBarrierTransportationDistance() const
{
	return theVapourBarrierTransportationDistance.data();
}

void RDetailedBuildingLifeCycleWaterUsageModel::setVapourBarrierTransportationDistance(QObject *value)
{
	theVapourBarrierTransportationDistance = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleWaterUsageModel::getVapourBarrierTransportationWIntensity() const
{
	return theVapourBarrierTransportationWIntensity.data();
}

void RDetailedBuildingLifeCycleWaterUsageModel::setVapourBarrierTransportationWIntensity(QObject *value)
{
	theVapourBarrierTransportationWIntensity = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleWaterUsageModel::getInsulationQuantity() const
{
	return theInsulationQuantity.data();
}

void RDetailedBuildingLifeCycleWaterUsageModel::setInsulationQuantity(QObject *value)
{
	theInsulationQuantity = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleWaterUsageModel::getInsulationWIntensity() const
{
	return theInsulationWIntensity.data();
}

void RDetailedBuildingLifeCycleWaterUsageModel::setInsulationWIntensity(QObject *value)
{
	theInsulationWIntensity = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleWaterUsageModel::getInsulationTransportationDistance() const
{
	return theInsulationTransportationDistance.data();
}

void RDetailedBuildingLifeCycleWaterUsageModel::setInsulationTransportationDistance(QObject *value)
{
	theInsulationTransportationDistance = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleWaterUsageModel::getInsulationTransportationWIntensity() const
{
	return theInsulationTransportationWIntensity.data();
}

void RDetailedBuildingLifeCycleWaterUsageModel::setInsulationTransportationWIntensity(QObject *value)
{
	theInsulationTransportationWIntensity = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleWaterUsageModel::getGlassQuantity() const
{
	return theGlassQuantity.data();
}

void RDetailedBuildingLifeCycleWaterUsageModel::setGlassQuantity(QObject *value)
{
	theGlassQuantity = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleWaterUsageModel::getGlassWIntensity() const
{
	return theGlassWIntensity.data();
}

void RDetailedBuildingLifeCycleWaterUsageModel::setGlassWIntensity(QObject *value)
{
	theGlassWIntensity = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleWaterUsageModel::getGlassTransportationDistance() const
{
	return theGlassTransportationDistance.data();
}

void RDetailedBuildingLifeCycleWaterUsageModel::setGlassTransportationDistance(QObject *value)
{
	theGlassTransportationDistance = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleWaterUsageModel::getGlassTransportationWIntensity() const
{
	return theGlassTransportationWIntensity.data();
}

void RDetailedBuildingLifeCycleWaterUsageModel::setGlassTransportationWIntensity(QObject *value)
{
	theGlassTransportationWIntensity = qobject_cast<RParameter *>(value);
}


QObject * RDetailedBuildingLifeCycleWaterUsageModel::getOtherMaterialQuantity() const
{
	return theOtherMaterialQuantity.data();
}

void RDetailedBuildingLifeCycleWaterUsageModel::setOtherMaterialQuantity(QObject *value)
{
	theOtherMaterialQuantity = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleWaterUsageModel::getOtherMaterialWIntensity() const
{
	return theOtherMaterialWIntensity.data();
}

void RDetailedBuildingLifeCycleWaterUsageModel::setOtherMaterialWIntensity(QObject *value)
{
	theOtherMaterialWIntensity = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleWaterUsageModel::getOtherMaterialTransportationDistance() const
{
	return theOtherMaterialTransportationDistance.data();
}

void RDetailedBuildingLifeCycleWaterUsageModel::setOtherMaterialTransportationDistance(QObject *value)
{
	theOtherMaterialTransportationDistance = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleWaterUsageModel::getOtherMaterialTransportationWIntensity() const
{
	return theOtherMaterialTransportationWIntensity.data();
}

void RDetailedBuildingLifeCycleWaterUsageModel::setOtherMaterialTransportationWIntensity(QObject *value)
{
	theOtherMaterialTransportationWIntensity = qobject_cast<RParameter *>(value);
}







QObject * RDetailedBuildingLifeCycleWaterUsageModel::getWorkerHourWIntensity() const
{
	return theWorkerHourWIntensity.data();
}

void RDetailedBuildingLifeCycleWaterUsageModel::setWorkerHourWIntensity(QObject *value)
{
	theWorkerHourWIntensity = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleWaterUsageModel::getTotalWorkerHours() const
{
	return theTotalWorkerHours.data();
}

void RDetailedBuildingLifeCycleWaterUsageModel::setTotalWorkerHours(QObject *value)
{
	theTotalWorkerHours = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleWaterUsageModel::getDaysYearlyOperation() const
{
	return theDaysYearlyOperation.data();
}

void RDetailedBuildingLifeCycleWaterUsageModel::setDaysYearlyOperation(QObject *value)
{
	theDaysYearlyOperation = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleWaterUsageModel::getTotalBuildingOccupants() const
{
	return theTotalBuildingOccupants.data();
}

void RDetailedBuildingLifeCycleWaterUsageModel::setTotalBuildingOccupants(QObject *value)
{
	theTotalBuildingOccupants = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleWaterUsageModel::getFlowFixture1WDemand() const
{
	return theFlowFixture1WDemand.data();
}

void RDetailedBuildingLifeCycleWaterUsageModel::setFlowFixture1WDemand(QObject *value)
{
	theFlowFixture1WDemand = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleWaterUsageModel::getFlushMaleWDemand() const
{
	return theFlushMaleWDemand.data();
}

void RDetailedBuildingLifeCycleWaterUsageModel::setFlushMaleWDemand(QObject *value)
{
	theFlushMaleWDemand = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleWaterUsageModel::getFlushFemaleWDemand() const
{
	return theFlushFemaleWDemand.data();
}

void RDetailedBuildingLifeCycleWaterUsageModel::setFlushFemaleWDemand(QObject *value)
{
	theFlushFemaleWDemand = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleWaterUsageModel::getRatioMaleOccupants() const
{
	return theRatioMaleOccupants.data();
}

void RDetailedBuildingLifeCycleWaterUsageModel::setRatioMaleOccupants(QObject *value)
{
	theRatioMaleOccupants = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleWaterUsageModel::getDesignLife() const
{
	return theDesignLife.data();
}

void RDetailedBuildingLifeCycleWaterUsageModel::setDesignLife(QObject *value)
{
	theDesignLife = qobject_cast<RParameter *>(value);
}





int RDetailedBuildingLifeCycleWaterUsageModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
{	
	// Extract the input parameter values
	double c_q = theConcreteQuantity.data()->getCurrentValue(threadID); 
	double c_i = theConcreteWIntensity.data()->getCurrentValue(threadID);
	double c_td = theConcreteTransportationDistance.data()->getCurrentValue(threadID);
	double c_ti = theConcreteTransportationWIntensity.data()->getCurrentValue(threadID);
	double s_q = theSteelQuantity.data()->getCurrentValue(threadID); 
	double s_i = theSteelWIntensity.data()->getCurrentValue(threadID);
	double s_td = theSteelTransportationDistance.data()->getCurrentValue(threadID);
	double s_ti = theSteelTransportationWIntensity.data()->getCurrentValue(threadID); 
	double w_q = theWoodQuantity.data()->getCurrentValue(threadID);
	double w_i = theWoodWIntensity.data()->getCurrentValue(threadID);
	double w_td = theWoodTransportationDistance.data()->getCurrentValue(threadID); 
	double w_ti = theWoodTransportationWIntensity.data()->getCurrentValue(threadID); 
	double gb_q = theGypsumBoardQuantity.data()->getCurrentValue(threadID);
	double gb_i = theGypsumBoardWIntensity.data()->getCurrentValue(threadID);
	double gb_td = theGypsumBoardTransportationDistance.data()->getCurrentValue(threadID);
	double gb_ti = theGypsumBoardTransportationWIntensity.data()->getCurrentValue(threadID);
	double v_q = theVapourBarrierQuantity.data()->getCurrentValue(threadID); 
	double v_i = theVapourBarrierWIntensity.data()->getCurrentValue(threadID);
	double v_td = theVapourBarrierTransportationDistance.data()->getCurrentValue(threadID);
	double v_ti = theVapourBarrierTransportationWIntensity.data()->getCurrentValue(threadID);
	double i_q = theInsulationQuantity.data()->getCurrentValue(threadID); 
	double i_i = theInsulationWIntensity.data()->getCurrentValue(threadID);
	double i_td = theInsulationTransportationDistance.data()->getCurrentValue(threadID);
	double i_ti = theInsulationTransportationWIntensity.data()->getCurrentValue(threadID); 
	double g_q = theGlassQuantity.data()->getCurrentValue(threadID);
	double g_i = theGlassWIntensity.data()->getCurrentValue(threadID);
	double g_td = theGlassTransportationDistance.data()->getCurrentValue(threadID); 
	double g_ti = theGlassTransportationWIntensity.data()->getCurrentValue(threadID); 
	double o_q = theOtherMaterialQuantity.data()->getCurrentValue(threadID);
	double o_i = theOtherMaterialWIntensity.data()->getCurrentValue(threadID);
	double o_td = theOtherMaterialTransportationDistance.data()->getCurrentValue(threadID);
	double o_ti = theOtherMaterialTransportationWIntensity.data()->getCurrentValue(threadID);
	double w_wh = theWorkerHourWIntensity.data()->getCurrentValue(threadID);
	double twh = theTotalWorkerHours.data()->getCurrentValue(threadID);
	double dyo = theDaysYearlyOperation.data()->getCurrentValue(threadID);
	double oe = theTotalBuildingOccupants.data()->getCurrentValue(threadID);
	double ifl = theFlowFixture1WDemand.data()->getCurrentValue(threadID);
	double imf = theFlushMaleWDemand.data()->getCurrentValue(threadID);
	double iff = theFlushFemaleWDemand.data()->getCurrentValue(threadID);
	double rmf = theRatioMaleOccupants.data()->getCurrentValue(threadID);
	double td = theDesignLife.data()->getCurrentValue(threadID);


	double em = 0; // extraction & manufacture
	double oc = 0; // on-site construction
	double op = 0; // operation



	em = c_q*(c_i+c_td*c_ti)+s_q*(s_i+s_td*s_ti)+v_q*(v_i+v_td*v_ti)+w_q*(w_i+w_td*w_ti)+gb_q*(gb_i+gb_td*gb_ti)+i_q*(i_i+i_td*i_ti)+g_q*(g_i+g_td*g_ti)+o_q*(o_i+o_td*o_ti);

	oc = twh * w_wh;

	op = td*dyo*oe*(ifl+imf*rmf+iff*(1-rmf));


	// Add all life cycle stages
	double response = em+oc+op;

	// Updating the response current value
	theWaterUsageFineResponse->setCurrentValue(response,threadID);

	return 0;
}

bool RDetailedBuildingLifeCycleWaterUsageModel::canBeRunParallelly(){
	return true;
}