#include "RDetailedBuildingLifeCycleGWPModel.h"

#include "RParameter.h"
#include "RResponse.h"

RDetailedBuildingLifeCycleGWPModel::RDetailedBuildingLifeCycleGWPModel(QObject *parent, QString &name)
	: RModel(parent, name)
{
	// Initializing the pointer data members
/* //$$ 
	theConcreteQuantity = 0;
	theConcreteGWPIntensity = 0;
	theConcreteTransportationDistance = 0;
	theConcreteTransportationGWPIntensity = 0;
	theSteelQuantity = 0;
	theSteelGWPIntensity = 0;
	theSteelTransportationDistance = 0;
	theSteelTransportationGWPIntensity = 0;
	theWoodQuantity = 0;
	theWoodGWPIntensity = 0;
	theWoodTransportationDistance = 0;
	theWoodTransportationGWPIntensity = 0;
	theGypsumBoardQuantity = 0;
	theGypsumBoardGWPIntensity = 0;
	theGypsumBoardTransportationDistance = 0;
	theGypsumBoardTransportationGWPIntensity = 0;
	theVapourBarrierQuantity = 0;
	theVapourBarrierGWPIntensity = 0;
	theVapourBarrierTransportationDistance = 0;
	theVapourBarrierTransportationGWPIntensity = 0;
	theInsulationQuantity = 0;
	theInsulationGWPIntensity = 0;
	theInsulationTransportationDistance = 0;
	theInsulationTransportationGWPIntensity = 0;
	theGlassQuantity = 0;
	theGlassGWPIntensity = 0;
	theGlassTransportationDistance = 0;
	theGlassTransportationGWPIntensity = 0;
	theOtherMaterialQuantity = 0;
	theOtherMaterialGWPIntensity = 0;
	theOtherMaterialTransportationDistance = 0;
	theOtherMaterialTransportationGWPIntensity = 0;
	theEnergyConstruction = 0;
	theGWPIntensityEnergyConstruction = 0;
	theAnnualEnergyOperation = 0;
	theDesignLife = 0;
	theGWPIntensityOperation = 0;
	theEOLEnergy = 0;
	theGWPIntensityEOL = 0;
*/

	// Instantiating the output response
	theDomain->createObject(objectName() + "Response", "RGenericResponse");
	// Setting theResponse pointer
	theGWPFineResponse = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	// Setting the "model" property of theResponse to be "this" object
	theGWPFineResponse->setModel(this);
}

RDetailedBuildingLifeCycleGWPModel::~RDetailedBuildingLifeCycleGWPModel()
{

}

QObject * RDetailedBuildingLifeCycleGWPModel::getConcreteQuantity() const
{
	return theConcreteQuantity.data();
}

void RDetailedBuildingLifeCycleGWPModel::setConcreteQuantity(QObject *value)
{
	theConcreteQuantity = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleGWPModel::getConcreteGWPIntensity() const
{
	return theConcreteGWPIntensity.data();
}

void RDetailedBuildingLifeCycleGWPModel::setConcreteGWPIntensity(QObject *value)
{
	theConcreteGWPIntensity = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleGWPModel::getConcreteTransportationDistance() const
{
	return theConcreteTransportationDistance.data();
}

void RDetailedBuildingLifeCycleGWPModel::setConcreteTransportationDistance(QObject *value)
{
	theConcreteTransportationDistance = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleGWPModel::getConcreteTransportationGWPIntensity() const
{
	return theConcreteTransportationGWPIntensity.data();
}

void RDetailedBuildingLifeCycleGWPModel::setConcreteTransportationGWPIntensity(QObject *value)
{
	theConcreteTransportationGWPIntensity = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleGWPModel::getSteelQuantity() const
{
	return theSteelQuantity.data();
}

void RDetailedBuildingLifeCycleGWPModel::setSteelQuantity(QObject *value)
{
	theSteelQuantity = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleGWPModel::getSteelGWPIntensity() const
{
	return theSteelGWPIntensity.data();
}

void RDetailedBuildingLifeCycleGWPModel::setSteelGWPIntensity(QObject *value)
{
	theSteelGWPIntensity = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleGWPModel::getSteelTransportationDistance() const
{
	return theSteelTransportationDistance.data();
}

void RDetailedBuildingLifeCycleGWPModel::setSteelTransportationDistance(QObject *value)
{
	theSteelTransportationDistance = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleGWPModel::getSteelTransportationGWPIntensity() const
{
	return theSteelTransportationGWPIntensity.data();
}

void RDetailedBuildingLifeCycleGWPModel::setSteelTransportationGWPIntensity(QObject *value)
{
	theSteelTransportationGWPIntensity = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleGWPModel::getWoodQuantity() const
{
	return theWoodQuantity.data();
}

void RDetailedBuildingLifeCycleGWPModel::setWoodQuantity(QObject *value)
{
	theWoodQuantity = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleGWPModel::getWoodGWPIntensity() const
{
	return theWoodGWPIntensity.data();
}

void RDetailedBuildingLifeCycleGWPModel::setWoodGWPIntensity(QObject *value)
{
	theWoodGWPIntensity = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleGWPModel::getWoodTransportationDistance() const
{
	return theWoodTransportationDistance.data();
}

void RDetailedBuildingLifeCycleGWPModel::setWoodTransportationDistance(QObject *value)
{
	theWoodTransportationDistance = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleGWPModel::getWoodTransportationGWPIntensity() const
{
	return theWoodTransportationGWPIntensity.data();
}

void RDetailedBuildingLifeCycleGWPModel::setWoodTransportationGWPIntensity(QObject *value)
{
	theWoodTransportationGWPIntensity = qobject_cast<RParameter *>(value);
}


QObject * RDetailedBuildingLifeCycleGWPModel::getGypsumBoardQuantity() const
{
	return theGypsumBoardQuantity.data();
}

void RDetailedBuildingLifeCycleGWPModel::setGypsumBoardQuantity(QObject *value)
{
	theGypsumBoardQuantity = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleGWPModel::getGypsumBoardGWPIntensity() const
{
	return theGypsumBoardGWPIntensity.data();
}

void RDetailedBuildingLifeCycleGWPModel::setGypsumBoardGWPIntensity(QObject *value)
{
	theGypsumBoardGWPIntensity = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleGWPModel::getGypsumBoardTransportationDistance() const
{
	return theGypsumBoardTransportationDistance.data();
}

void RDetailedBuildingLifeCycleGWPModel::setGypsumBoardTransportationDistance(QObject *value)
{
	theGypsumBoardTransportationDistance = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleGWPModel::getGypsumBoardTransportationGWPIntensity() const
{
	return theGypsumBoardTransportationGWPIntensity.data();
}

void RDetailedBuildingLifeCycleGWPModel::setGypsumBoardTransportationGWPIntensity(QObject *value)
{
	theGypsumBoardTransportationGWPIntensity = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleGWPModel::getVapourBarrierQuantity() const
{
	return theVapourBarrierQuantity.data();
}

void RDetailedBuildingLifeCycleGWPModel::setVapourBarrierQuantity(QObject *value)
{
	theVapourBarrierQuantity = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleGWPModel::getVapourBarrierGWPIntensity() const
{
	return theVapourBarrierGWPIntensity.data();
}

void RDetailedBuildingLifeCycleGWPModel::setVapourBarrierGWPIntensity(QObject *value)
{
	theVapourBarrierGWPIntensity = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleGWPModel::getVapourBarrierTransportationDistance() const
{
	return theVapourBarrierTransportationDistance.data();
}

void RDetailedBuildingLifeCycleGWPModel::setVapourBarrierTransportationDistance(QObject *value)
{
	theVapourBarrierTransportationDistance = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleGWPModel::getVapourBarrierTransportationGWPIntensity() const
{
	return theVapourBarrierTransportationGWPIntensity.data();
}

void RDetailedBuildingLifeCycleGWPModel::setVapourBarrierTransportationGWPIntensity(QObject *value)
{
	theVapourBarrierTransportationGWPIntensity = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleGWPModel::getInsulationQuantity() const
{
	return theInsulationQuantity.data();
}

void RDetailedBuildingLifeCycleGWPModel::setInsulationQuantity(QObject *value)
{
	theInsulationQuantity = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleGWPModel::getInsulationGWPIntensity() const
{
	return theInsulationGWPIntensity.data();
}

void RDetailedBuildingLifeCycleGWPModel::setInsulationGWPIntensity(QObject *value)
{
	theInsulationGWPIntensity = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleGWPModel::getInsulationTransportationDistance() const
{
	return theInsulationTransportationDistance.data();
}

void RDetailedBuildingLifeCycleGWPModel::setInsulationTransportationDistance(QObject *value)
{
	theInsulationTransportationDistance = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleGWPModel::getInsulationTransportationGWPIntensity() const
{
	return theInsulationTransportationGWPIntensity.data();
}

void RDetailedBuildingLifeCycleGWPModel::setInsulationTransportationGWPIntensity(QObject *value)
{
	theInsulationTransportationGWPIntensity = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleGWPModel::getGlassQuantity() const
{
	return theGlassQuantity.data();
}

void RDetailedBuildingLifeCycleGWPModel::setGlassQuantity(QObject *value)
{
	theGlassQuantity = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleGWPModel::getGlassGWPIntensity() const
{
	return theGlassGWPIntensity.data();
}

void RDetailedBuildingLifeCycleGWPModel::setGlassGWPIntensity(QObject *value)
{
	theGlassGWPIntensity = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleGWPModel::getGlassTransportationDistance() const
{
	return theGlassTransportationDistance.data();
}

void RDetailedBuildingLifeCycleGWPModel::setGlassTransportationDistance(QObject *value)
{
	theGlassTransportationDistance = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleGWPModel::getGlassTransportationGWPIntensity() const
{
	return theGlassTransportationGWPIntensity.data();
}

void RDetailedBuildingLifeCycleGWPModel::setGlassTransportationGWPIntensity(QObject *value)
{
	theGlassTransportationGWPIntensity = qobject_cast<RParameter *>(value);
}


QObject * RDetailedBuildingLifeCycleGWPModel::getOtherMaterialQuantity() const
{
	return theOtherMaterialQuantity.data();
}

void RDetailedBuildingLifeCycleGWPModel::setOtherMaterialQuantity(QObject *value)
{
	theOtherMaterialQuantity = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleGWPModel::getOtherMaterialGWPIntensity() const
{
	return theOtherMaterialGWPIntensity.data();
}

void RDetailedBuildingLifeCycleGWPModel::setOtherMaterialGWPIntensity(QObject *value)
{
	theOtherMaterialGWPIntensity = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleGWPModel::getOtherMaterialTransportationDistance() const
{
	return theOtherMaterialTransportationDistance.data();
}

void RDetailedBuildingLifeCycleGWPModel::setOtherMaterialTransportationDistance(QObject *value)
{
	theOtherMaterialTransportationDistance = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleGWPModel::getOtherMaterialTransportationGWPIntensity() const
{
	return theOtherMaterialTransportationGWPIntensity.data();
}

void RDetailedBuildingLifeCycleGWPModel::setOtherMaterialTransportationGWPIntensity(QObject *value)
{
	theOtherMaterialTransportationGWPIntensity = qobject_cast<RParameter *>(value);
}







QObject * RDetailedBuildingLifeCycleGWPModel::getEnergyConstruction() const
{
	return theEnergyConstruction.data();
}

void RDetailedBuildingLifeCycleGWPModel::setEnergyConstruction(QObject *value)
{
	theEnergyConstruction = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleGWPModel::getGWPIntensityEnergyConstruction() const
{
	return theGWPIntensityEnergyConstruction.data();
}

void RDetailedBuildingLifeCycleGWPModel::setGWPIntensityEnergyConstruction(QObject *value)
{
	theGWPIntensityEnergyConstruction = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleGWPModel::getAnnualEnergyOperation() const
{
	return theAnnualEnergyOperation.data();
}

void RDetailedBuildingLifeCycleGWPModel::setAnnualEnergyOperation(QObject *value)
{
	theAnnualEnergyOperation = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleGWPModel::getDesignLife() const
{
	return theDesignLife.data();
}

void RDetailedBuildingLifeCycleGWPModel::setDesignLife(QObject *value)
{
	theDesignLife = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleGWPModel::getGWPIntensityOperation() const
{
	return theGWPIntensityOperation.data();
}

void RDetailedBuildingLifeCycleGWPModel::setGWPIntensityOperation(QObject *value)
{
	theGWPIntensityOperation = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleGWPModel::getEOLEnergy() const
{
	return theEOLEnergy.data();
}

void RDetailedBuildingLifeCycleGWPModel::setEOLEnergy(QObject *value)
{
	theEOLEnergy = qobject_cast<RParameter *>(value);
}

QObject * RDetailedBuildingLifeCycleGWPModel::getGWPIntensityEOL() const
{
	return theGWPIntensityEOL.data();
}

void RDetailedBuildingLifeCycleGWPModel::setGWPIntensityEOL(QObject *value)
{
	theGWPIntensityEOL = qobject_cast<RParameter *>(value);
}






int RDetailedBuildingLifeCycleGWPModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
{	
	// Extract the input parameter values
	double c_q = theConcreteQuantity.data()->getCurrentValue(threadID); 
	double c_i = theConcreteGWPIntensity.data()->getCurrentValue(threadID);
	double c_td = theConcreteTransportationDistance.data()->getCurrentValue(threadID);
	double c_ti = theConcreteTransportationGWPIntensity.data()->getCurrentValue(threadID);
	double s_q = theSteelQuantity.data()->getCurrentValue(threadID); 
	double s_i = theSteelGWPIntensity.data()->getCurrentValue(threadID);
	double s_td = theSteelTransportationDistance.data()->getCurrentValue(threadID);
	double s_ti = theSteelTransportationGWPIntensity.data()->getCurrentValue(threadID); 
	double w_q = theWoodQuantity.data()->getCurrentValue(threadID);
	double w_i = theWoodGWPIntensity.data()->getCurrentValue(threadID);
	double w_td = theWoodTransportationDistance.data()->getCurrentValue(threadID); 
	double w_ti = theWoodTransportationGWPIntensity.data()->getCurrentValue(threadID); 
	double gb_q = theGypsumBoardQuantity.data()->getCurrentValue(threadID);
	double gb_i = theGypsumBoardGWPIntensity.data()->getCurrentValue(threadID);
	double gb_td = theGypsumBoardTransportationDistance.data()->getCurrentValue(threadID);
	double gb_ti = theGypsumBoardTransportationGWPIntensity.data()->getCurrentValue(threadID);
	double v_q = theVapourBarrierQuantity.data()->getCurrentValue(threadID); 
	double v_i = theVapourBarrierGWPIntensity.data()->getCurrentValue(threadID);
	double v_td = theVapourBarrierTransportationDistance.data()->getCurrentValue(threadID);
	double v_ti = theVapourBarrierTransportationGWPIntensity.data()->getCurrentValue(threadID);
	double i_q = theInsulationQuantity.data()->getCurrentValue(threadID); 
	double i_i = theInsulationGWPIntensity.data()->getCurrentValue(threadID);
	double i_td = theInsulationTransportationDistance.data()->getCurrentValue(threadID);
	double i_ti = theInsulationTransportationGWPIntensity.data()->getCurrentValue(threadID); 
	double g_q = theGlassQuantity.data()->getCurrentValue(threadID);
	double g_i = theGlassGWPIntensity.data()->getCurrentValue(threadID);
	double g_td = theGlassTransportationDistance.data()->getCurrentValue(threadID); 
	double g_ti = theGlassTransportationGWPIntensity.data()->getCurrentValue(threadID); 
	double o_q = theOtherMaterialQuantity.data()->getCurrentValue(threadID);
	double o_i = theOtherMaterialGWPIntensity.data()->getCurrentValue(threadID);
	double o_td = theOtherMaterialTransportationDistance.data()->getCurrentValue(threadID);
	double o_ti = theOtherMaterialTransportationGWPIntensity.data()->getCurrentValue(threadID);

	double ec = theEnergyConstruction.data()->getCurrentValue(threadID);
	double i_ec = theGWPIntensityEnergyConstruction.data()->getCurrentValue(threadID);
	double aeo = theAnnualEnergyOperation.data()->getCurrentValue(threadID);
	double td = theDesignLife.data()->getCurrentValue(threadID);
	double i_aeo = theGWPIntensityOperation.data()->getCurrentValue(threadID);
	double el = theEOLEnergy.data()->getCurrentValue(threadID);
	double i_el = theGWPIntensityEOL.data()->getCurrentValue(threadID);


	double em = 0; // extraction & manufacture
	double oc = 0; // on-site construction
	double op = 0; // operation
	double eol = 0; // end of life (demolition)
	//double qtot = c_q+s_q+w_q+gb_q+i_q+g_q+o_q+v_q;

	em = c_q*(c_i+c_td*c_ti)+s_q*(s_i+s_td*s_ti)+v_q*(v_i+v_td*v_ti)+w_q*(w_i+w_td*w_ti)+gb_q*(gb_i+gb_td*gb_ti)+i_q*(i_i+i_td*i_ti)+g_q*(g_i+g_td*g_ti)+o_q*(o_i+o_td*o_ti);

	oc = ec*i_ec;

	op = aeo*td*i_aeo;

	eol = el*i_el;


	// Add all life cycle stages
	double response = em+oc+op+eol;

	// Updating the response current value
	theGWPFineResponse->setCurrentValue(response,threadID);

	return 0;
}

bool RDetailedBuildingLifeCycleGWPModel::canBeRunParallelly(){
	return true;
}