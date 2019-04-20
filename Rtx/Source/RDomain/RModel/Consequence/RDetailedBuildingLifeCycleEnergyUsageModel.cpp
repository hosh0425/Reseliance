#include "RDetailedBuildingLifeCycleEnergyUsageModel.h"

#include "RParameter.h"
#include "RResponse.h"

RDetailedBuildingLifeCycleEnergyUsageModel::RDetailedBuildingLifeCycleEnergyUsageModel(QObject *parent, QString &name)
	: RModel(parent, name)
{
	// Initializing the pointer data members

/* //$$ 
	theConcreteQuantity = 0;
	theConcreteIntensity = 0;
	theConcreteTransportationDistance = 0;
	theConcreteTransportationIntensity = 0;
	theSteelQuantity = 0;
	theSteelIntensity = 0;
	theSteelTransportationDistance = 0;
	theSteelTransportationIntensity = 0;
	theWoodQuantity = 0;
	theWoodIntensity = 0;
	theWoodTransportationDistance = 0;
	theWoodTransportationIntensity = 0;
	theGypsumBoardQuantity = 0;
	theGypsumBoardIntensity = 0;
	theGypsumBoardTransportationDistance = 0;
	theGypsumBoardTransportationIntensity = 0;
	theVapourBarrierQuantity = 0;
	theVapourBarrierIntensity = 0;
	theVapourBarrierTransportationDistance = 0;
	theVapourBarrierTransportationIntensity = 0;
	theInsulationQuantity = 0;
	theInsulationIntensity = 0;
	theInsulationTransportationDistance = 0;
	theInsulationTransportationIntensity = 0;
	theGlassQuantity = 0;
	theGlassIntensity = 0;
	theGlassTransportationDistance = 0;
	theGlassTransportationIntensity = 0;
	theOtherMaterialQuantity = 0;
	theOtherMaterialIntensity = 0;
	theOtherMaterialTransportationDistance = 0;
	theOtherMaterialTransportationIntensity = 0;
	theRatioWorkerHours = 0;
	theTotalWorkerHours = 0;
	theWorkerShiftLength = 0;
	theDistanceWorkerTravel = 0;
	theHeavyMachineIntensity = 0;
	theManualLaborIntensity = 0;
	theMeanNumberWorkers = 0;
	theWorkerTransportationIntensity = 0;
	theDesignLife = 0;
	theAnnualEnergy = 0;
	theMaterial1Life = 0;
	theMaterial1ReplacementIntensity = 0;
	theMaterial2Life = 0;
	theMaterial2ReplacementIntensity = 0;
	theEOLIntensity = 0;
*/

	// Instantiating the output response
	theDomain->createObject(objectName() + "TotalResponse", "RGenericResponse");
	// Setting theResponse pointer
	theEnergyUsageResponse = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	// Setting the "model" property of theResponse to be "this" object
	theEnergyUsageResponse->setModel(this);


	// Instantiating the output response
	theDomain->createObject(objectName() + "PreUseReponse", "RGenericResponse");
	// Setting theResponse pointer
	thePreUseResponse = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	// Setting the "model" property of theResponse to be "this" object
	thePreUseResponse->setModel(this);


	// Instantiating the output response
	theDomain->createObject(objectName() + "PostUseResponse", "RGenericResponse");
	// Setting theResponse pointer
	thePostUseResponse = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	// Setting the "model" property of theResponse to be "this" object
	thePostUseResponse->setModel(this);


}



RDetailedBuildingLifeCycleEnergyUsageModel::~RDetailedBuildingLifeCycleEnergyUsageModel()
{

}

QObject * RDetailedBuildingLifeCycleEnergyUsageModel::getConcreteQuantity() const
{
	return theConcreteQuantity.data();
}

void RDetailedBuildingLifeCycleEnergyUsageModel::setConcreteQuantity(QObject *value)
{
	theConcreteQuantity = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleEnergyUsageModel::getConcreteIntensity() const
{
	return theConcreteIntensity.data();
}

void RDetailedBuildingLifeCycleEnergyUsageModel::setConcreteIntensity(QObject *value)
{
	theConcreteIntensity = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleEnergyUsageModel::getConcreteTransportationDistance() const
{
	return theConcreteTransportationDistance.data();
}

void RDetailedBuildingLifeCycleEnergyUsageModel::setConcreteTransportationDistance(QObject *value)
{
	theConcreteTransportationDistance = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleEnergyUsageModel::getConcreteTransportationIntensity() const
{
	return theConcreteTransportationIntensity.data();
}

void RDetailedBuildingLifeCycleEnergyUsageModel::setConcreteTransportationIntensity(QObject *value)
{
	theConcreteTransportationIntensity = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleEnergyUsageModel::getSteelQuantity() const
{
	return theSteelQuantity.data();
}

void RDetailedBuildingLifeCycleEnergyUsageModel::setSteelQuantity(QObject *value)
{
	theSteelQuantity = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleEnergyUsageModel::getSteelIntensity() const
{
	return theSteelIntensity.data();
}

void RDetailedBuildingLifeCycleEnergyUsageModel::setSteelIntensity(QObject *value)
{
	theSteelIntensity = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleEnergyUsageModel::getSteelTransportationDistance() const
{
	return theSteelTransportationDistance.data();
}

void RDetailedBuildingLifeCycleEnergyUsageModel::setSteelTransportationDistance(QObject *value)
{
	theSteelTransportationDistance = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleEnergyUsageModel::getSteelTransportationIntensity() const
{
	return theSteelTransportationIntensity.data();
}

void RDetailedBuildingLifeCycleEnergyUsageModel::setSteelTransportationIntensity(QObject *value)
{
	theSteelTransportationIntensity = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleEnergyUsageModel::getWoodQuantity() const
{
	return theWoodQuantity.data();
}

void RDetailedBuildingLifeCycleEnergyUsageModel::setWoodQuantity(QObject *value)
{
	theWoodQuantity = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleEnergyUsageModel::getWoodIntensity() const
{
	return theWoodIntensity.data();
}

void RDetailedBuildingLifeCycleEnergyUsageModel::setWoodIntensity(QObject *value)
{
	theWoodIntensity = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleEnergyUsageModel::getWoodTransportationDistance() const
{
	return theWoodTransportationDistance.data();
}

void RDetailedBuildingLifeCycleEnergyUsageModel::setWoodTransportationDistance(QObject *value)
{
	theWoodTransportationDistance = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleEnergyUsageModel::getWoodTransportationIntensity() const
{
	return theWoodTransportationIntensity.data();
}

void RDetailedBuildingLifeCycleEnergyUsageModel::setWoodTransportationIntensity(QObject *value)
{
	theWoodTransportationIntensity = qobject_cast<RParameter *>(value);
}


QObject * RDetailedBuildingLifeCycleEnergyUsageModel::getGypsumBoardQuantity() const
{
	return theGypsumBoardQuantity.data();
}

void RDetailedBuildingLifeCycleEnergyUsageModel::setGypsumBoardQuantity(QObject *value)
{
	theGypsumBoardQuantity = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleEnergyUsageModel::getGypsumBoardIntensity() const
{
	return theGypsumBoardIntensity.data();
}

void RDetailedBuildingLifeCycleEnergyUsageModel::setGypsumBoardIntensity(QObject *value)
{
	theGypsumBoardIntensity = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleEnergyUsageModel::getGypsumBoardTransportationDistance() const
{
	return theGypsumBoardTransportationDistance.data();
}

void RDetailedBuildingLifeCycleEnergyUsageModel::setGypsumBoardTransportationDistance(QObject *value)
{
	theGypsumBoardTransportationDistance = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleEnergyUsageModel::getGypsumBoardTransportationIntensity() const
{
	return theGypsumBoardTransportationIntensity.data();
}

void RDetailedBuildingLifeCycleEnergyUsageModel::setGypsumBoardTransportationIntensity(QObject *value)
{
	theGypsumBoardTransportationIntensity = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleEnergyUsageModel::getVapourBarrierQuantity() const
{
	return theVapourBarrierQuantity.data();
}

void RDetailedBuildingLifeCycleEnergyUsageModel::setVapourBarrierQuantity(QObject *value)
{
	theVapourBarrierQuantity = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleEnergyUsageModel::getVapourBarrierIntensity() const
{
	return theVapourBarrierIntensity.data();
}

void RDetailedBuildingLifeCycleEnergyUsageModel::setVapourBarrierIntensity(QObject *value)
{
	theVapourBarrierIntensity = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleEnergyUsageModel::getVapourBarrierTransportationDistance() const
{
	return theVapourBarrierTransportationDistance.data();
}

void RDetailedBuildingLifeCycleEnergyUsageModel::setVapourBarrierTransportationDistance(QObject *value)
{
	theVapourBarrierTransportationDistance = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleEnergyUsageModel::getVapourBarrierTransportationIntensity() const
{
	return theVapourBarrierTransportationIntensity.data();
}

void RDetailedBuildingLifeCycleEnergyUsageModel::setVapourBarrierTransportationIntensity(QObject *value)
{
	theVapourBarrierTransportationIntensity = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleEnergyUsageModel::getInsulationQuantity() const
{
	return theInsulationQuantity.data();
}

void RDetailedBuildingLifeCycleEnergyUsageModel::setInsulationQuantity(QObject *value)
{
	theInsulationQuantity = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleEnergyUsageModel::getInsulationIntensity() const
{
	return theInsulationIntensity.data();
}

void RDetailedBuildingLifeCycleEnergyUsageModel::setInsulationIntensity(QObject *value)
{
	theInsulationIntensity = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleEnergyUsageModel::getInsulationTransportationDistance() const
{
	return theInsulationTransportationDistance.data();
}

void RDetailedBuildingLifeCycleEnergyUsageModel::setInsulationTransportationDistance(QObject *value)
{
	theInsulationTransportationDistance = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleEnergyUsageModel::getInsulationTransportationIntensity() const
{
	return theInsulationTransportationIntensity.data();
}

void RDetailedBuildingLifeCycleEnergyUsageModel::setInsulationTransportationIntensity(QObject *value)
{
	theInsulationTransportationIntensity = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleEnergyUsageModel::getGlassQuantity() const
{
	return theGlassQuantity.data();
}

void RDetailedBuildingLifeCycleEnergyUsageModel::setGlassQuantity(QObject *value)
{
	theGlassQuantity = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleEnergyUsageModel::getGlassIntensity() const
{
	return theGlassIntensity.data();
}

void RDetailedBuildingLifeCycleEnergyUsageModel::setGlassIntensity(QObject *value)
{
	theGlassIntensity = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleEnergyUsageModel::getGlassTransportationDistance() const
{
	return theGlassTransportationDistance.data();
}

void RDetailedBuildingLifeCycleEnergyUsageModel::setGlassTransportationDistance(QObject *value)
{
	theGlassTransportationDistance = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleEnergyUsageModel::getGlassTransportationIntensity() const
{
	return theGlassTransportationIntensity.data();
}

void RDetailedBuildingLifeCycleEnergyUsageModel::setGlassTransportationIntensity(QObject *value)
{
	theGlassTransportationIntensity = qobject_cast<RParameter *>(value);
}


QObject * RDetailedBuildingLifeCycleEnergyUsageModel::getOtherMaterialQuantity() const
{
	return theOtherMaterialQuantity.data();
}

void RDetailedBuildingLifeCycleEnergyUsageModel::setOtherMaterialQuantity(QObject *value)
{
	theOtherMaterialQuantity = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleEnergyUsageModel::getOtherMaterialIntensity() const
{
	return theOtherMaterialIntensity.data();
}

void RDetailedBuildingLifeCycleEnergyUsageModel::setOtherMaterialIntensity(QObject *value)
{
	theOtherMaterialIntensity = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleEnergyUsageModel::getOtherMaterialTransportationDistance() const
{
	return theOtherMaterialTransportationDistance.data();
}

void RDetailedBuildingLifeCycleEnergyUsageModel::setOtherMaterialTransportationDistance(QObject *value)
{
	theOtherMaterialTransportationDistance = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleEnergyUsageModel::getOtherMaterialTransportationIntensity() const
{
	return theOtherMaterialTransportationIntensity.data();
}

void RDetailedBuildingLifeCycleEnergyUsageModel::setOtherMaterialTransportationIntensity(QObject *value)
{
	theOtherMaterialTransportationIntensity = qobject_cast<RParameter *>(value);
}







QObject * RDetailedBuildingLifeCycleEnergyUsageModel::getRatioWorkerHours() const
{
	return theRatioWorkerHours.data();
}

void RDetailedBuildingLifeCycleEnergyUsageModel::setRatioWorkerHours(QObject *value)
{
	theRatioWorkerHours = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleEnergyUsageModel::getTotalWorkerHours() const
{
	return theTotalWorkerHours.data();
}

void RDetailedBuildingLifeCycleEnergyUsageModel::setTotalWorkerHours(QObject *value)
{
	theTotalWorkerHours = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleEnergyUsageModel::getWorkerShiftLength() const
{
	return theWorkerShiftLength.data();
}

void RDetailedBuildingLifeCycleEnergyUsageModel::setWorkerShiftLength(QObject *value)
{
	theWorkerShiftLength = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleEnergyUsageModel::getDistanceWorkerTravel() const
{
	return theDistanceWorkerTravel.data();
}

void RDetailedBuildingLifeCycleEnergyUsageModel::setDistanceWorkerTravel(QObject *value)
{
	theDistanceWorkerTravel = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleEnergyUsageModel::getHeavyMachineIntensity() const
{
	return theHeavyMachineIntensity.data();
}

void RDetailedBuildingLifeCycleEnergyUsageModel::setHeavyMachineIntensity(QObject *value)
{
	theHeavyMachineIntensity = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleEnergyUsageModel::getManualLaborIntensity() const
{
	return theManualLaborIntensity.data();
}

void RDetailedBuildingLifeCycleEnergyUsageModel::setManualLaborIntensity(QObject *value)
{
	theManualLaborIntensity = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleEnergyUsageModel::getMeanNumberWorkers() const
{
	return theMeanNumberWorkers.data();
}

void RDetailedBuildingLifeCycleEnergyUsageModel::setMeanNumberWorkers(QObject *value)
{
	theMeanNumberWorkers = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleEnergyUsageModel::getWorkerTransportationIntensity() const
{
	return theWorkerTransportationIntensity.data();
}

void RDetailedBuildingLifeCycleEnergyUsageModel::setWorkerTransportationIntensity(QObject *value)
{
	theWorkerTransportationIntensity = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleEnergyUsageModel::getDesignLife() const
{
	return theDesignLife.data();
}

void RDetailedBuildingLifeCycleEnergyUsageModel::setDesignLife(QObject *value)
{
	theDesignLife = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleEnergyUsageModel::getAnnualEnergy() const
{
	return theAnnualEnergy.data();
}

void RDetailedBuildingLifeCycleEnergyUsageModel::setAnnualEnergy(QObject *value)
{
	theAnnualEnergy = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleEnergyUsageModel::getMaterial1Life() const
{
	return theMaterial1Life.data();
}

void RDetailedBuildingLifeCycleEnergyUsageModel::setMaterial1Life(QObject *value)
{
	theMaterial1Life = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleEnergyUsageModel::getMaterial1ReplacementIntensity() const
{
	return theMaterial1ReplacementIntensity.data();
}

void RDetailedBuildingLifeCycleEnergyUsageModel::setMaterial1ReplacementIntensity(QObject *value)
{
	theMaterial1ReplacementIntensity = qobject_cast<RParameter *>(value);
}


QObject * RDetailedBuildingLifeCycleEnergyUsageModel::getMaterial2Life() const
{
	return theMaterial2Life.data();
}

void RDetailedBuildingLifeCycleEnergyUsageModel::setMaterial2Life(QObject *value)
{
	theMaterial2Life = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleEnergyUsageModel::getMaterial2ReplacementIntensity() const
{
	return theMaterial2ReplacementIntensity.data();
}

void RDetailedBuildingLifeCycleEnergyUsageModel::setMaterial2ReplacementIntensity(QObject *value)
{
	theMaterial2ReplacementIntensity = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleEnergyUsageModel::getEOLIntensity() const
{
	return theEOLIntensity.data();
}

void RDetailedBuildingLifeCycleEnergyUsageModel::setEOLIntensity(QObject *value)
{
	theEOLIntensity = qobject_cast<RParameter *>(value);
}








int RDetailedBuildingLifeCycleEnergyUsageModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
{	
	// Extract the input parameter values
	double c_q = theConcreteQuantity.data()->getCurrentValue(threadID); 
	double c_i = theConcreteIntensity.data()->getCurrentValue(threadID);
	double c_td = theConcreteTransportationDistance.data()->getCurrentValue(threadID);
	double c_ti = theConcreteTransportationIntensity.data()->getCurrentValue(threadID);
	double s_q = theSteelQuantity.data()->getCurrentValue(threadID);
	double s_i = theSteelIntensity.data()->getCurrentValue(threadID);
	double s_td = theSteelTransportationDistance.data()->getCurrentValue(threadID);
	double s_ti = theSteelTransportationIntensity.data()->getCurrentValue(threadID);
	double w_q = theWoodQuantity.data()->getCurrentValue(threadID);
	double w_i = theWoodIntensity.data()->getCurrentValue(threadID);
	double w_td = theWoodTransportationDistance.data()->getCurrentValue(threadID);
	double w_ti = theWoodTransportationIntensity.data()->getCurrentValue(threadID);
	double gb_q = theGypsumBoardQuantity.data()->getCurrentValue(threadID);
	double gb_i = theGypsumBoardIntensity.data()->getCurrentValue(threadID);
	double gb_td = theGypsumBoardTransportationDistance.data()->getCurrentValue(threadID);
	double gb_ti = theGypsumBoardTransportationIntensity.data()->getCurrentValue(threadID);
	double v_q = theVapourBarrierQuantity.data()->getCurrentValue(threadID);
	double v_i = theVapourBarrierIntensity.data()->getCurrentValue(threadID);
	double v_td = theVapourBarrierTransportationDistance.data()->getCurrentValue(threadID);
	double v_ti = theVapourBarrierTransportationIntensity.data()->getCurrentValue(threadID);
	double i_q = theInsulationQuantity.data()->getCurrentValue(threadID); 
	double i_i = theInsulationIntensity.data()->getCurrentValue(threadID);
	double i_td = theInsulationTransportationDistance.data()->getCurrentValue(threadID);
	double i_ti = theInsulationTransportationIntensity.data()->getCurrentValue(threadID); 
	double g_q = theGlassQuantity.data()->getCurrentValue(threadID);
	double g_i = theGlassIntensity.data()->getCurrentValue(threadID);
	double g_td = theGlassTransportationDistance.data()->getCurrentValue(threadID); 
	double g_ti = theGlassTransportationIntensity.data()->getCurrentValue(threadID); 
	double o_q = theOtherMaterialQuantity.data()->getCurrentValue(threadID);
	double o_i = theOtherMaterialIntensity.data()->getCurrentValue(threadID);
	double o_td = theOtherMaterialTransportationDistance.data()->getCurrentValue(threadID);
	double o_ti = theOtherMaterialTransportationIntensity.data()->getCurrentValue(threadID);

	double rh = theRatioWorkerHours.data()->getCurrentValue(threadID);
	double twh = theTotalWorkerHours.data()->getCurrentValue(threadID);
	double ts = theWorkerShiftLength.data()->getCurrentValue(threadID);
	double dwt = theDistanceWorkerTravel.data()->getCurrentValue(threadID);
	double ihm = theHeavyMachineIntensity.data()->getCurrentValue(threadID);
	double im = theManualLaborIntensity.data()->getCurrentValue(threadID);
	double nw = theMeanNumberWorkers.data()->getCurrentValue(threadID);
	double iwt = theWorkerTransportationIntensity.data()->getCurrentValue(threadID);
	double tdes = theDesignLife.data()->getCurrentValue(threadID);
	double ea = theAnnualEnergy.data()->getCurrentValue(threadID);
	double tmat1 = theMaterial1Life.data()->getCurrentValue(threadID);
	double irmat1 = theMaterial1ReplacementIntensity.data()->getCurrentValue(threadID);
	double tmat2 = theMaterial2Life.data()->getCurrentValue(threadID);
	double irmat2 = theMaterial2ReplacementIntensity.data()->getCurrentValue(threadID);
	double ieol = theEOLIntensity.data()->getCurrentValue(threadID);


	double em = 0; // extraction & manufacture
	double oc = 0; // on-site construction
	double op = 0; // operation
	double maint = 0; // maintenance
	double eol = 0; // end of life (demolition)
	double qtot = c_q+s_q+w_q+gb_q+i_q+g_q+o_q+v_q;
	double pre = em+oc;
	double post = op+maint+eol;

	em = c_q*(c_i+c_td*c_ti)+s_q*(s_i+s_td*s_ti)+v_q*(v_i+v_td*v_ti)+w_q*(w_i+w_td*w_ti)+gb_q*(gb_i+gb_td*gb_ti)+i_q*(i_i+i_td*i_ti)+g_q*(g_i+g_td*g_ti)+o_q*(o_i+o_td*o_ti);

	oc = rh*twh*ihm+(1-rh)*im*twh+twh*iwt*dwt*nw/ts;

	op = tdes * ea;

	maint = (tdes/tmat1 -1)*irmat1 + (tdes/tmat2-1)*irmat2;

	eol = qtot *ieol;


	// Add all life cycle stages
	double response = em+oc+op+maint+eol;

	// Updating the response current value
	theEnergyUsageResponse->setCurrentValue(response,threadID);
	thePreUseResponse->setCurrentValue(pre,threadID);
	thePostUseResponse->setCurrentValue(post,threadID);

	return 0;
}

bool RDetailedBuildingLifeCycleEnergyUsageModel::canBeRunParallelly(){
	return true;
}