#ifndef RDetailedBuildingLifeCycleEnergyUsageModel_H
#define RDetailedBuildingLifeCycleEnergyUsageModel_H

#include "RModel.h"

class RParameter;
class RResponse;

class RDetailedBuildingLifeCycleEnergyUsageModel : public RModel
{
	Q_OBJECT

	// Define a QProperty for each input (parameter) data member. The type of the QProperty is "QObject *"
	Q_PROPERTY(QObject *ConcreteQuantity READ getConcreteQuantity WRITE setConcreteQuantity)
	Q_PROPERTY(QObject *ConcreteIntensity READ getConcreteIntensity WRITE setConcreteIntensity)
	Q_PROPERTY(QObject *ConcreteTransportationDistance READ getConcreteTransportationDistance WRITE setConcreteTransportationDistance)
	Q_PROPERTY(QObject *ConcreteTransportationIntensity READ getConcreteTransportationIntensity WRITE setConcreteTransportationIntensity)
	Q_PROPERTY(QObject *SteelQuantity READ getSteelQuantity WRITE setSteelQuantity)
	Q_PROPERTY(QObject *SteelIntensity READ getSteelIntensity WRITE setSteelIntensity)
	Q_PROPERTY(QObject *SteelTransportationDistance READ getSteelTransportationDistance WRITE setSteelTransportationDistance)
	Q_PROPERTY(QObject *SteelTransportationIntensity READ getSteelTransportationIntensity WRITE setSteelTransportationIntensity)
	Q_PROPERTY(QObject *WoodQuantity READ getWoodQuantity WRITE setWoodQuantity)
	Q_PROPERTY(QObject *WoodIntensity READ getWoodIntensity WRITE setWoodIntensity)
	Q_PROPERTY(QObject *WoodTransportationDistance READ getWoodTransportationDistance WRITE setWoodTransportationDistance)
	Q_PROPERTY(QObject *WoodTransportationIntensity READ getWoodTransportationIntensity WRITE setWoodTransportationIntensity)
	Q_PROPERTY(QObject *GypsumBoardQuantity READ getGypsumBoardQuantity WRITE setGypsumBoardQuantity)
	Q_PROPERTY(QObject *GypsumBoardIntensity READ getGypsumBoardIntensity WRITE setGypsumBoardIntensity)
	Q_PROPERTY(QObject *GypsumBoardTransportationDistance READ getGypsumBoardTransportationDistance WRITE setGypsumBoardTransportationDistance)
	Q_PROPERTY(QObject *GypsumBoardTransportationIntensity READ getGypsumBoardTransportationIntensity WRITE setGypsumBoardTransportationIntensity)
	Q_PROPERTY(QObject *VapourBarrierQuantity READ getVapourBarrierQuantity WRITE setVapourBarrierQuantity)
	Q_PROPERTY(QObject *VapourBarrierIntensity READ getVapourBarrierIntensity WRITE setVapourBarrierIntensity)
	Q_PROPERTY(QObject *VapourBarrierTransportationDistance READ getVapourBarrierTransportationDistance WRITE setVapourBarrierTransportationDistance)
	Q_PROPERTY(QObject *VapourBarrierTransportationIntensity READ getVapourBarrierTransportationIntensity WRITE setVapourBarrierTransportationIntensity)
	Q_PROPERTY(QObject *InsulationQuantity READ getInsulationQuantity WRITE setInsulationQuantity)
	Q_PROPERTY(QObject *InsulationIntensity READ getInsulationIntensity WRITE setInsulationIntensity)
	Q_PROPERTY(QObject *InsulationTransportationDistance READ getInsulationTransportationDistance WRITE setInsulationTransportationDistance)
	Q_PROPERTY(QObject *InsulationTransportationIntensity READ getInsulationTransportationIntensity WRITE setInsulationTransportationIntensity)
	Q_PROPERTY(QObject *GlassQuantity READ getGlassQuantity WRITE setGlassQuantity)
	Q_PROPERTY(QObject *GlassIntensity READ getGlassIntensity WRITE setGlassIntensity)
	Q_PROPERTY(QObject *GlassTransportationDistance READ getGlassTransportationDistance WRITE setGlassTransportationDistance)
	Q_PROPERTY(QObject *GlassTransportationIntensity READ getGlassTransportationIntensity WRITE setGlassTransportationIntensity)
	Q_PROPERTY(QObject *OtherMaterialQuantity READ getOtherMaterialQuantity WRITE setOtherMaterialQuantity)
	Q_PROPERTY(QObject *OtherMaterialIntensity READ getOtherMaterialIntensity WRITE setOtherMaterialIntensity)
	Q_PROPERTY(QObject *OtherMaterialTransportationDistance READ getOtherMaterialTransportationDistance WRITE setOtherMaterialTransportationDistance)
	Q_PROPERTY(QObject *OtherMaterialTransportationIntensity READ getOtherMaterialTransportationIntensity WRITE setOtherMaterialTransportationIntensity)


	Q_PROPERTY(QObject *RatioWorkerHours READ getRatioWorkerHours WRITE setRatioWorkerHours)
	Q_PROPERTY(QObject *TotalWorkerHours READ getTotalWorkerHours WRITE setTotalWorkerHours)
	Q_PROPERTY(QObject *WorkerShiftLength READ getWorkerShiftLength WRITE setWorkerShiftLength)
	Q_PROPERTY(QObject *DistanceWorkerTravel READ getDistanceWorkerTravel WRITE setDistanceWorkerTravel)
	Q_PROPERTY(QObject *HeavyMachineIntensity READ getHeavyMachineIntensity WRITE setHeavyMachineIntensity)
	Q_PROPERTY(QObject *ManualLaborIntensity READ getManualLaborIntensity WRITE setManualLaborIntensity)
	Q_PROPERTY(QObject *MeanNumberWorkers READ getMeanNumberWorkers WRITE setMeanNumberWorkers)
	Q_PROPERTY(QObject *WorkerTransportationIntensity READ getWorkerTransportationIntensity WRITE setWorkerTransportationIntensity)
	Q_PROPERTY(QObject *DesignLife READ getDesignLife WRITE setDesignLife)
	Q_PROPERTY(QObject *AnnualEnergy READ getAnnualEnergy WRITE setAnnualEnergy)
	Q_PROPERTY(QObject *Material1Life READ getMaterial1Life WRITE setMaterial1Life)
	Q_PROPERTY(QObject *Material1ReplacementIntensity READ getMaterial1ReplacementIntensity WRITE setMaterial1ReplacementIntensity)
	Q_PROPERTY(QObject *Material2Life READ getMaterial2Life WRITE setMaterial2Life)
	Q_PROPERTY(QObject *Material2ReplacementIntensity READ getMaterial2ReplacementIntensity WRITE setMaterial2ReplacementIntensity)
	Q_PROPERTY(QObject *EOLIntensity READ getEOLIntensity WRITE setEOLIntensity)


public:
	// Define the constructor and destructor
	RDetailedBuildingLifeCycleEnergyUsageModel(QObject *parent, QString &name);
	~RDetailedBuildingLifeCycleEnergyUsageModel();

	// Define the mandatory read/write methods for all QProperties
	QObject *getConcreteQuantity() const;
	void setConcreteQuantity(QObject *value);

	QObject *getConcreteIntensity() const;
	void setConcreteIntensity(QObject *value);

	QObject *getConcreteTransportationDistance() const;
	void setConcreteTransportationDistance(QObject *value);

	QObject *getConcreteTransportationIntensity() const;
	void setConcreteTransportationIntensity(QObject *value);

	QObject *getSteelQuantity() const;
	void setSteelQuantity(QObject *value);

	QObject *getSteelIntensity() const;
	void setSteelIntensity(QObject *value);

	QObject *getSteelTransportationDistance() const;
	void setSteelTransportationDistance(QObject *value);

	QObject *getSteelTransportationIntensity() const;
	void setSteelTransportationIntensity(QObject *value);

	QObject *getWoodQuantity() const;
	void setWoodQuantity(QObject *value);

	QObject *getWoodIntensity() const;
	void setWoodIntensity(QObject *value);

	QObject *getWoodTransportationDistance() const;
	void setWoodTransportationDistance(QObject *value);

	QObject *getWoodTransportationIntensity() const;
	void setWoodTransportationIntensity(QObject *value);

	QObject *getGypsumBoardQuantity() const;
	void setGypsumBoardQuantity(QObject *value);

	QObject *getGypsumBoardIntensity() const;
	void setGypsumBoardIntensity(QObject *value);

	QObject *getGypsumBoardTransportationDistance() const;
	void setGypsumBoardTransportationDistance(QObject *value);

	QObject *getGypsumBoardTransportationIntensity() const;
	void setGypsumBoardTransportationIntensity(QObject *value);

	QObject *getVapourBarrierQuantity() const;
	void setVapourBarrierQuantity(QObject *value);

	QObject *getVapourBarrierIntensity() const;
	void setVapourBarrierIntensity(QObject *value);

	QObject *getVapourBarrierTransportationDistance() const;
	void setVapourBarrierTransportationDistance(QObject *value);

	QObject *getVapourBarrierTransportationIntensity() const;
	void setVapourBarrierTransportationIntensity(QObject *value);

	QObject *getInsulationQuantity() const;
	void setInsulationQuantity(QObject *value);

	QObject *getInsulationIntensity() const;
	void setInsulationIntensity(QObject *value);

	QObject *getInsulationTransportationDistance() const;
	void setInsulationTransportationDistance(QObject *value);

	QObject *getInsulationTransportationIntensity() const;
	void setInsulationTransportationIntensity(QObject *value);

	QObject *getGlassQuantity() const;
	void setGlassQuantity(QObject *value);

	QObject *getGlassIntensity() const;
	void setGlassIntensity(QObject *value);

	QObject *getGlassTransportationDistance() const;
	void setGlassTransportationDistance(QObject *value);

	QObject *getGlassTransportationIntensity() const;
	void setGlassTransportationIntensity(QObject *value);

	QObject *getOtherMaterialQuantity() const;
	void setOtherMaterialQuantity(QObject *value);

	QObject *getOtherMaterialIntensity() const;
	void setOtherMaterialIntensity(QObject *value);

	QObject *getOtherMaterialTransportationDistance() const;
	void setOtherMaterialTransportationDistance(QObject *value);

	QObject *getOtherMaterialTransportationIntensity() const;
	void setOtherMaterialTransportationIntensity(QObject *value);



	QObject *getRatioWorkerHours() const;
	void setRatioWorkerHours(QObject *value);

	QObject *getTotalWorkerHours() const;
	void setTotalWorkerHours(QObject *value);

	QObject *getWorkerShiftLength() const;
	void setWorkerShiftLength(QObject *value);

	QObject *getDistanceWorkerTravel() const;
	void setDistanceWorkerTravel(QObject *value);

	QObject *getHeavyMachineIntensity() const;
	void setHeavyMachineIntensity(QObject *value);

	QObject *getManualLaborIntensity() const;
	void setManualLaborIntensity(QObject *value);

	QObject *getMeanNumberWorkers() const;
	void setMeanNumberWorkers(QObject *value);

	QObject *getWorkerTransportationIntensity() const;
	void setWorkerTransportationIntensity(QObject *value);

	QObject *getDesignLife() const;
	void setDesignLife(QObject *value);

	QObject *getAnnualEnergy() const;
	void setAnnualEnergy(QObject *value);

	QObject *getMaterial1Life() const;
	void setMaterial1Life(QObject *value);

	QObject *getMaterial1ReplacementIntensity() const;
	void setMaterial1ReplacementIntensity(QObject *value);

	QObject *getMaterial2Life() const;
	void setMaterial2Life(QObject *value);

	QObject *getMaterial2ReplacementIntensity() const;
	void setMaterial2ReplacementIntensity(QObject *value);

	QObject *getEOLIntensity() const;
	void setEOLIntensity(QObject *value);




	// Define the method where the actual model is coded
	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID);

	// returns true as this model is parallelized
	bool canBeRunParallelly();

private:
	// Declare input data members
	RPointer<RParameter> theConcreteQuantity;
	RPointer<RParameter> theConcreteIntensity;
	RPointer<RParameter> theConcreteTransportationDistance;
	RPointer<RParameter> theConcreteTransportationIntensity;
	RPointer<RParameter> theSteelQuantity;
	RPointer<RParameter> theSteelIntensity;
	RPointer<RParameter> theSteelTransportationDistance;
	RPointer<RParameter> theSteelTransportationIntensity;
	RPointer<RParameter> theWoodQuantity;
	RPointer<RParameter> theWoodIntensity;
	RPointer<RParameter> theWoodTransportationDistance;
	RPointer<RParameter> theWoodTransportationIntensity;
	RPointer<RParameter> theGypsumBoardQuantity;
	RPointer<RParameter> theGypsumBoardIntensity;
	RPointer<RParameter> theGypsumBoardTransportationDistance;
	RPointer<RParameter> theGypsumBoardTransportationIntensity;
	RPointer<RParameter> theVapourBarrierQuantity;
	RPointer<RParameter> theVapourBarrierIntensity;
	RPointer<RParameter> theVapourBarrierTransportationDistance;
	RPointer<RParameter> theVapourBarrierTransportationIntensity;
	RPointer<RParameter> theInsulationQuantity;
	RPointer<RParameter> theInsulationIntensity;
	RPointer<RParameter> theInsulationTransportationDistance;
	RPointer<RParameter> theInsulationTransportationIntensity;
	RPointer<RParameter> theGlassQuantity;
	RPointer<RParameter> theGlassIntensity;
	RPointer<RParameter> theGlassTransportationDistance;
	RPointer<RParameter> theGlassTransportationIntensity;
	RPointer<RParameter> theOtherMaterialQuantity;
	RPointer<RParameter> theOtherMaterialIntensity;
	RPointer<RParameter> theOtherMaterialTransportationDistance;
	RPointer<RParameter> theOtherMaterialTransportationIntensity;

	RPointer<RParameter> theRatioWorkerHours;
	RPointer<RParameter> theTotalWorkerHours;
	RPointer<RParameter> theWorkerShiftLength;
	RPointer<RParameter> theDistanceWorkerTravel;
	RPointer<RParameter> theHeavyMachineIntensity;
	RPointer<RParameter> theManualLaborIntensity;
	RPointer<RParameter> theMeanNumberWorkers;
	RPointer<RParameter> theWorkerTransportationIntensity;
	RPointer<RParameter> theDesignLife;
	RPointer<RParameter> theAnnualEnergy;
	RPointer<RParameter> theMaterial1Life;
	RPointer<RParameter> theMaterial1ReplacementIntensity;
	RPointer<RParameter> theMaterial2Life;
	RPointer<RParameter> theMaterial2ReplacementIntensity;
	RPointer<RParameter> theEOLIntensity;



	// Declare responses
	RResponse *theEnergyUsageResponse;
	RResponse *thePreUseResponse;
	RResponse *thePostUseResponse;


};

#endif // RDetailedBuildingLifeCycleEnergyUsageModel_H
