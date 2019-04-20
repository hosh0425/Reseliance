#ifndef RDetailedBuildingLifeCycleGWPModel_H
#define RDetailedBuildingLifeCycleGWPModel_H

#include "RModel.h"

class RParameter;
class RResponse;

class RDetailedBuildingLifeCycleGWPModel : public RModel
{
	Q_OBJECT

	// Define a QProperty for each input (parameter) data member. The type of the QProperty is "QObject *"
	Q_PROPERTY(QObject *ConcreteQuantity READ getConcreteQuantity WRITE setConcreteQuantity)
	Q_PROPERTY(QObject *ConcreteGWPIntensity READ getConcreteGWPIntensity WRITE setConcreteGWPIntensity)
	Q_PROPERTY(QObject *ConcreteTransportationDistance READ getConcreteTransportationDistance WRITE setConcreteTransportationDistance)
	Q_PROPERTY(QObject *ConcreteTransportationGWPIntensity READ getConcreteTransportationGWPIntensity WRITE setConcreteTransportationGWPIntensity)
	Q_PROPERTY(QObject *SteelQuantity READ getSteelQuantity WRITE setSteelQuantity)
	Q_PROPERTY(QObject *SteelGWPIntensity READ getSteelGWPIntensity WRITE setSteelGWPIntensity)
	Q_PROPERTY(QObject *SteelTransportationDistance READ getSteelTransportationDistance WRITE setSteelTransportationDistance)
	Q_PROPERTY(QObject *SteelTransportationGWPIntensity READ getSteelTransportationGWPIntensity WRITE setSteelTransportationGWPIntensity)
	Q_PROPERTY(QObject *WoodQuantity READ getWoodQuantity WRITE setWoodQuantity)
	Q_PROPERTY(QObject *WoodGWPIntensity READ getWoodGWPIntensity WRITE setWoodGWPIntensity)
	Q_PROPERTY(QObject *WoodTransportationDistance READ getWoodTransportationDistance WRITE setWoodTransportationDistance)
	Q_PROPERTY(QObject *WoodTransportationGWPIntensity READ getWoodTransportationGWPIntensity WRITE setWoodTransportationGWPIntensity)
	Q_PROPERTY(QObject *GypsumBoardQuantity READ getGypsumBoardQuantity WRITE setGypsumBoardQuantity)
	Q_PROPERTY(QObject *GypsumBoardGWPIntensity READ getGypsumBoardGWPIntensity WRITE setGypsumBoardGWPIntensity)
	Q_PROPERTY(QObject *GypsumBoardTransportationDistance READ getGypsumBoardTransportationDistance WRITE setGypsumBoardTransportationDistance)
	Q_PROPERTY(QObject *GypsumBoardTransportationGWPIntensity READ getGypsumBoardTransportationGWPIntensity WRITE setGypsumBoardTransportationGWPIntensity)
	Q_PROPERTY(QObject *VapourBarrierQuantity READ getVapourBarrierQuantity WRITE setVapourBarrierQuantity)
	Q_PROPERTY(QObject *VapourBarrierGWPIntensity READ getVapourBarrierGWPIntensity WRITE setVapourBarrierGWPIntensity)
	Q_PROPERTY(QObject *VapourBarrierTransportationDistance READ getVapourBarrierTransportationDistance WRITE setVapourBarrierTransportationDistance)
	Q_PROPERTY(QObject *VapourBarrierTransportationGWPIntensity READ getVapourBarrierTransportationGWPIntensity WRITE setVapourBarrierTransportationGWPIntensity)
	Q_PROPERTY(QObject *InsulationQuantity READ getInsulationQuantity WRITE setInsulationQuantity)
	Q_PROPERTY(QObject *InsulationGWPIntensity READ getInsulationGWPIntensity WRITE setInsulationGWPIntensity)
	Q_PROPERTY(QObject *InsulationTransportationDistance READ getInsulationTransportationDistance WRITE setInsulationTransportationDistance)
	Q_PROPERTY(QObject *InsulationTransportationGWPIntensity READ getInsulationTransportationGWPIntensity WRITE setInsulationTransportationGWPIntensity)
	Q_PROPERTY(QObject *GlassQuantity READ getGlassQuantity WRITE setGlassQuantity)
	Q_PROPERTY(QObject *GlassGWPIntensity READ getGlassGWPIntensity WRITE setGlassGWPIntensity)
	Q_PROPERTY(QObject *GlassTransportationDistance READ getGlassTransportationDistance WRITE setGlassTransportationDistance)
	Q_PROPERTY(QObject *GlassTransportationGWPIntensity READ getGlassTransportationGWPIntensity WRITE setGlassTransportationGWPIntensity)
	Q_PROPERTY(QObject *OtherMaterialQuantity READ getOtherMaterialQuantity WRITE setOtherMaterialQuantity)
	Q_PROPERTY(QObject *OtherMaterialGWPIntensity READ getOtherMaterialGWPIntensity WRITE setOtherMaterialGWPIntensity)
	Q_PROPERTY(QObject *OtherMaterialTransportationDistance READ getOtherMaterialTransportationDistance WRITE setOtherMaterialTransportationDistance)
	Q_PROPERTY(QObject *OtherMaterialTransportationGWPIntensity READ getOtherMaterialTransportationGWPIntensity WRITE setOtherMaterialTransportationGWPIntensity)




	Q_PROPERTY(QObject *EnergyConstruction READ getEnergyConstruction WRITE setEnergyConstruction)
	Q_PROPERTY(QObject *GWPIntensityEnergyConstruction READ getGWPIntensityEnergyConstruction WRITE setGWPIntensityEnergyConstruction)
	Q_PROPERTY(QObject *AnnualEnergyOperation READ getAnnualEnergyOperation WRITE setAnnualEnergyOperation)
	Q_PROPERTY(QObject *DesignLife READ getDesignLife WRITE setDesignLife)
	Q_PROPERTY(QObject *GWPIntensityOperation READ getGWPIntensityOperation WRITE setGWPIntensityOperation)
	Q_PROPERTY(QObject *EOLEnergy READ getEOLEnergy WRITE setEOLEnergy)
	Q_PROPERTY(QObject *GWPIntensityEOL READ getGWPIntensityEOL WRITE setGWPIntensityEOL)


public:
	// Define the constructor and destructor
	RDetailedBuildingLifeCycleGWPModel(QObject *parent, QString &name);
	~RDetailedBuildingLifeCycleGWPModel();

	// Define the mandatory read/write methods for all QProperties
	QObject *getConcreteQuantity() const;
	void setConcreteQuantity(QObject *value);

	QObject *getConcreteGWPIntensity() const;
	void setConcreteGWPIntensity(QObject *value);

	QObject *getConcreteTransportationDistance() const;
	void setConcreteTransportationDistance(QObject *value);

	QObject *getConcreteTransportationGWPIntensity() const;
	void setConcreteTransportationGWPIntensity(QObject *value);

	QObject *getSteelQuantity() const;
	void setSteelQuantity(QObject *value);

	QObject *getSteelGWPIntensity() const;
	void setSteelGWPIntensity(QObject *value);

	QObject *getSteelTransportationDistance() const;
	void setSteelTransportationDistance(QObject *value);

	QObject *getSteelTransportationGWPIntensity() const;
	void setSteelTransportationGWPIntensity(QObject *value);

	QObject *getWoodQuantity() const;
	void setWoodQuantity(QObject *value);

	QObject *getWoodGWPIntensity() const;
	void setWoodGWPIntensity(QObject *value);

	QObject *getWoodTransportationDistance() const;
	void setWoodTransportationDistance(QObject *value);

	QObject *getWoodTransportationGWPIntensity() const;
	void setWoodTransportationGWPIntensity(QObject *value);

	QObject *getGypsumBoardQuantity() const;
	void setGypsumBoardQuantity(QObject *value);

	QObject *getGypsumBoardGWPIntensity() const;
	void setGypsumBoardGWPIntensity(QObject *value);

	QObject *getGypsumBoardTransportationDistance() const;
	void setGypsumBoardTransportationDistance(QObject *value);

	QObject *getGypsumBoardTransportationGWPIntensity() const;
	void setGypsumBoardTransportationGWPIntensity(QObject *value);

	QObject *getVapourBarrierQuantity() const;
	void setVapourBarrierQuantity(QObject *value);

	QObject *getVapourBarrierGWPIntensity() const;
	void setVapourBarrierGWPIntensity(QObject *value);

	QObject *getVapourBarrierTransportationDistance() const;
	void setVapourBarrierTransportationDistance(QObject *value);

	QObject *getVapourBarrierTransportationGWPIntensity() const;
	void setVapourBarrierTransportationGWPIntensity(QObject *value);

	QObject *getInsulationQuantity() const;
	void setInsulationQuantity(QObject *value);

	QObject *getInsulationGWPIntensity() const;
	void setInsulationGWPIntensity(QObject *value);

	QObject *getInsulationTransportationDistance() const;
	void setInsulationTransportationDistance(QObject *value);

	QObject *getInsulationTransportationGWPIntensity() const;
	void setInsulationTransportationGWPIntensity(QObject *value);

	QObject *getGlassQuantity() const;
	void setGlassQuantity(QObject *value);

	QObject *getGlassGWPIntensity() const;
	void setGlassGWPIntensity(QObject *value);

	QObject *getGlassTransportationDistance() const;
	void setGlassTransportationDistance(QObject *value);

	QObject *getGlassTransportationGWPIntensity() const;
	void setGlassTransportationGWPIntensity(QObject *value);

	QObject *getOtherMaterialQuantity() const;
	void setOtherMaterialQuantity(QObject *value);

	QObject *getOtherMaterialGWPIntensity() const;
	void setOtherMaterialGWPIntensity(QObject *value);

	QObject *getOtherMaterialTransportationDistance() const;
	void setOtherMaterialTransportationDistance(QObject *value);

	QObject *getOtherMaterialTransportationGWPIntensity() const;
	void setOtherMaterialTransportationGWPIntensity(QObject *value);




	QObject *getEnergyConstruction() const;
	void setEnergyConstruction(QObject *value);

	QObject *getGWPIntensityEnergyConstruction() const;
	void setGWPIntensityEnergyConstruction(QObject *value);

	QObject *getAnnualEnergyOperation() const;
	void setAnnualEnergyOperation(QObject *value);

	QObject *getDesignLife() const;
	void setDesignLife(QObject *value);

	QObject *getGWPIntensityOperation() const;
	void setGWPIntensityOperation(QObject *value);

	QObject *getEOLEnergy() const;
	void setEOLEnergy(QObject *value);

	QObject *getGWPIntensityEOL() const;
	void setGWPIntensityEOL(QObject *value);



	// Define the method where the actual model is coded
	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID);

	// returns true as this model is parallelized
	bool canBeRunParallelly();

private:
	// Declare input data members
	RPointer<RParameter> theConcreteQuantity;
	RPointer<RParameter> theConcreteGWPIntensity;
	RPointer<RParameter> theConcreteTransportationDistance;
	RPointer<RParameter> theConcreteTransportationGWPIntensity;
	RPointer<RParameter> theSteelQuantity;
	RPointer<RParameter> theSteelGWPIntensity;
	RPointer<RParameter> theSteelTransportationDistance;
	RPointer<RParameter> theSteelTransportationGWPIntensity;
	RPointer<RParameter> theWoodQuantity;
	RPointer<RParameter> theWoodGWPIntensity;
	RPointer<RParameter> theWoodTransportationDistance;
	RPointer<RParameter> theWoodTransportationGWPIntensity;
	RPointer<RParameter> theGypsumBoardQuantity;
	RPointer<RParameter> theGypsumBoardGWPIntensity;
	RPointer<RParameter> theGypsumBoardTransportationDistance;
	RPointer<RParameter> theGypsumBoardTransportationGWPIntensity;
	RPointer<RParameter> theVapourBarrierQuantity;
	RPointer<RParameter> theVapourBarrierGWPIntensity;
	RPointer<RParameter> theVapourBarrierTransportationDistance;
	RPointer<RParameter> theVapourBarrierTransportationGWPIntensity;
	RPointer<RParameter> theInsulationQuantity;
	RPointer<RParameter> theInsulationGWPIntensity;
	RPointer<RParameter> theInsulationTransportationDistance;
	RPointer<RParameter> theInsulationTransportationGWPIntensity;
	RPointer<RParameter> theGlassQuantity;
	RPointer<RParameter> theGlassGWPIntensity;
	RPointer<RParameter> theGlassTransportationDistance;
	RPointer<RParameter> theGlassTransportationGWPIntensity;
	RPointer<RParameter> theOtherMaterialQuantity;
	RPointer<RParameter> theOtherMaterialGWPIntensity;
	RPointer<RParameter> theOtherMaterialTransportationDistance;
	RPointer<RParameter> theOtherMaterialTransportationGWPIntensity;

	RPointer<RParameter> theEnergyConstruction;
	RPointer<RParameter> theGWPIntensityEnergyConstruction;
	RPointer<RParameter> theAnnualEnergyOperation;
	RPointer<RParameter> theDesignLife;
	RPointer<RParameter> theGWPIntensityOperation;
	RPointer<RParameter> theEOLEnergy;
	RPointer<RParameter> theGWPIntensityEOL;



	// Declare responses
	RResponse *theGWPFineResponse;

};

#endif // RDetailedBuildingLifeCycleGWPModel_H
