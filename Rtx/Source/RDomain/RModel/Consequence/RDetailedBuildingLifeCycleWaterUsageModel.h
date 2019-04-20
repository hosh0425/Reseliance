#ifndef RDetailedBuildingLifeCycleWaterUsageModel_H
#define RDetailedBuildingLifeCycleWaterUsageModel_H

#include "RModel.h"

class RParameter;
class RResponse;

class RDetailedBuildingLifeCycleWaterUsageModel : public RModel
{
	Q_OBJECT

	// Define a QProperty for each input (parameter) data member. The type of the QProperty is "QObject *"
	Q_PROPERTY(QObject *ConcreteQuantity READ getConcreteQuantity WRITE setConcreteQuantity)
	Q_PROPERTY(QObject *ConcreteWIntensity READ getConcreteWIntensity WRITE setConcreteWIntensity)
	Q_PROPERTY(QObject *ConcreteTransportationDistance READ getConcreteTransportationDistance WRITE setConcreteTransportationDistance)
	Q_PROPERTY(QObject *ConcreteTransportationWIntensity READ getConcreteTransportationWIntensity WRITE setConcreteTransportationWIntensity)
	Q_PROPERTY(QObject *SteelQuantity READ getSteelQuantity WRITE setSteelQuantity)
	Q_PROPERTY(QObject *SteelWIntensity READ getSteelWIntensity WRITE setSteelWIntensity)
	Q_PROPERTY(QObject *SteelTransportationDistance READ getSteelTransportationDistance WRITE setSteelTransportationDistance)
	Q_PROPERTY(QObject *SteelTransportationWIntensity READ getSteelTransportationWIntensity WRITE setSteelTransportationWIntensity)
	Q_PROPERTY(QObject *WoodQuantity READ getWoodQuantity WRITE setWoodQuantity)
	Q_PROPERTY(QObject *WoodWIntensity READ getWoodWIntensity WRITE setWoodWIntensity)
	Q_PROPERTY(QObject *WoodTransportationDistance READ getWoodTransportationDistance WRITE setWoodTransportationDistance)
	Q_PROPERTY(QObject *WoodTransportationWIntensity READ getWoodTransportationWIntensity WRITE setWoodTransportationWIntensity)
	Q_PROPERTY(QObject *GypsumBoardQuantity READ getGypsumBoardQuantity WRITE setGypsumBoardQuantity)
	Q_PROPERTY(QObject *GypsumBoardWIntensity READ getGypsumBoardWIntensity WRITE setGypsumBoardWIntensity)
	Q_PROPERTY(QObject *GypsumBoardTransportationDistance READ getGypsumBoardTransportationDistance WRITE setGypsumBoardTransportationDistance)
	Q_PROPERTY(QObject *GypsumBoardTransportationWIntensity READ getGypsumBoardTransportationWIntensity WRITE setGypsumBoardTransportationWIntensity)
	Q_PROPERTY(QObject *VapourBarrierQuantity READ getVapourBarrierQuantity WRITE setVapourBarrierQuantity)
	Q_PROPERTY(QObject *VapourBarrierWIntensity READ getVapourBarrierWIntensity WRITE setVapourBarrierWIntensity)
	Q_PROPERTY(QObject *VapourBarrierTransportationDistance READ getVapourBarrierTransportationDistance WRITE setVapourBarrierTransportationDistance)
	Q_PROPERTY(QObject *VapourBarrierTransportationWIntensity READ getVapourBarrierTransportationWIntensity WRITE setVapourBarrierTransportationWIntensity)
	Q_PROPERTY(QObject *InsulationQuantity READ getInsulationQuantity WRITE setInsulationQuantity)
	Q_PROPERTY(QObject *InsulationWIntensity READ getInsulationWIntensity WRITE setInsulationWIntensity)
	Q_PROPERTY(QObject *InsulationTransportationDistance READ getInsulationTransportationDistance WRITE setInsulationTransportationDistance)
	Q_PROPERTY(QObject *InsulationTransportationWIntensity READ getInsulationTransportationWIntensity WRITE setInsulationTransportationWIntensity)
	Q_PROPERTY(QObject *GlassQuantity READ getGlassQuantity WRITE setGlassQuantity)
	Q_PROPERTY(QObject *GlassWIntensity READ getGlassWIntensity WRITE setGlassWIntensity)
	Q_PROPERTY(QObject *GlassTransportationDistance READ getGlassTransportationDistance WRITE setGlassTransportationDistance)
	Q_PROPERTY(QObject *GlassTransportationWIntensity READ getGlassTransportationWIntensity WRITE setGlassTransportationWIntensity)
	Q_PROPERTY(QObject *OtherMaterialQuantity READ getOtherMaterialQuantity WRITE setOtherMaterialQuantity)
	Q_PROPERTY(QObject *OtherMaterialWIntensity READ getOtherMaterialWIntensity WRITE setOtherMaterialWIntensity)
	Q_PROPERTY(QObject *OtherMaterialTransportationDistance READ getOtherMaterialTransportationDistance WRITE setOtherMaterialTransportationDistance)
	Q_PROPERTY(QObject *OtherMaterialTransportationWIntensity READ getOtherMaterialTransportationWIntensity WRITE setOtherMaterialTransportationWIntensity)

	Q_PROPERTY(QObject *WorkerHourWIntensity READ getWorkerHourWIntensity WRITE setWorkerHourWIntensity)
	Q_PROPERTY(QObject *TotalWorkerHours READ getTotalWorkerHours WRITE setTotalWorkerHours)
	Q_PROPERTY(QObject *DaysYearlyOperation READ getDaysYearlyOperation WRITE setDaysYearlyOperation)
	Q_PROPERTY(QObject *TotalBuildingOccupants READ getTotalBuildingOccupants WRITE setTotalBuildingOccupants)
	Q_PROPERTY(QObject *FlowFixture1WDemand READ getFlowFixture1WDemand WRITE setFlowFixture1WDemand)
	Q_PROPERTY(QObject *FlushMaleWDemand READ getFlushMaleWDemand WRITE setFlushMaleWDemand)
	Q_PROPERTY(QObject *FlushFemaleWDemand READ getFlushFemaleWDemand WRITE setFlushFemaleWDemand)
	Q_PROPERTY(QObject *RatioMaleOccupants READ getRatioMaleOccupants WRITE setRatioMaleOccupants)
	Q_PROPERTY(QObject *DesignLife READ getDesignLife WRITE setDesignLife)



public:
	// Define the constructor and destructor
	RDetailedBuildingLifeCycleWaterUsageModel(QObject *parent, QString &name);
	~RDetailedBuildingLifeCycleWaterUsageModel();

	// Define the mandatory read/write methods for all QProperties
	QObject *getConcreteQuantity() const;
	void setConcreteQuantity(QObject *value);

	QObject *getConcreteWIntensity() const;
	void setConcreteWIntensity(QObject *value);

	QObject *getConcreteTransportationDistance() const;
	void setConcreteTransportationDistance(QObject *value);

	QObject *getConcreteTransportationWIntensity() const;
	void setConcreteTransportationWIntensity(QObject *value);

	QObject *getSteelQuantity() const;
	void setSteelQuantity(QObject *value);

	QObject *getSteelWIntensity() const;
	void setSteelWIntensity(QObject *value);

	QObject *getSteelTransportationDistance() const;
	void setSteelTransportationDistance(QObject *value);

	QObject *getSteelTransportationWIntensity() const;
	void setSteelTransportationWIntensity(QObject *value);

	QObject *getWoodQuantity() const;
	void setWoodQuantity(QObject *value);

	QObject *getWoodWIntensity() const;
	void setWoodWIntensity(QObject *value);

	QObject *getWoodTransportationDistance() const;
	void setWoodTransportationDistance(QObject *value);

	QObject *getWoodTransportationWIntensity() const;
	void setWoodTransportationWIntensity(QObject *value);

	QObject *getGypsumBoardQuantity() const;
	void setGypsumBoardQuantity(QObject *value);

	QObject *getGypsumBoardWIntensity() const;
	void setGypsumBoardWIntensity(QObject *value);

	QObject *getGypsumBoardTransportationDistance() const;
	void setGypsumBoardTransportationDistance(QObject *value);

	QObject *getGypsumBoardTransportationWIntensity() const;
	void setGypsumBoardTransportationWIntensity(QObject *value);

	QObject *getVapourBarrierQuantity() const;
	void setVapourBarrierQuantity(QObject *value);

	QObject *getVapourBarrierWIntensity() const;
	void setVapourBarrierWIntensity(QObject *value);

	QObject *getVapourBarrierTransportationDistance() const;
	void setVapourBarrierTransportationDistance(QObject *value);

	QObject *getVapourBarrierTransportationWIntensity() const;
	void setVapourBarrierTransportationWIntensity(QObject *value);

	QObject *getInsulationQuantity() const;
	void setInsulationQuantity(QObject *value);

	QObject *getInsulationWIntensity() const;
	void setInsulationWIntensity(QObject *value);

	QObject *getInsulationTransportationDistance() const;
	void setInsulationTransportationDistance(QObject *value);

	QObject *getInsulationTransportationWIntensity() const;
	void setInsulationTransportationWIntensity(QObject *value);

	QObject *getGlassQuantity() const;
	void setGlassQuantity(QObject *value);

	QObject *getGlassWIntensity() const;
	void setGlassWIntensity(QObject *value);

	QObject *getGlassTransportationDistance() const;
	void setGlassTransportationDistance(QObject *value);

	QObject *getGlassTransportationWIntensity() const;
	void setGlassTransportationWIntensity(QObject *value);

	QObject *getOtherMaterialQuantity() const;
	void setOtherMaterialQuantity(QObject *value);

	QObject *getOtherMaterialWIntensity() const;
	void setOtherMaterialWIntensity(QObject *value);

	QObject *getOtherMaterialTransportationDistance() const;
	void setOtherMaterialTransportationDistance(QObject *value);

	QObject *getOtherMaterialTransportationWIntensity() const;
	void setOtherMaterialTransportationWIntensity(QObject *value);




	QObject *getWorkerHourWIntensity() const;
	void setWorkerHourWIntensity(QObject *value);

	QObject *getTotalWorkerHours() const;
	void setTotalWorkerHours(QObject *value);

	QObject *getDaysYearlyOperation() const;
	void setDaysYearlyOperation(QObject *value);

	QObject *getTotalBuildingOccupants() const;
	void setTotalBuildingOccupants(QObject *value);

	QObject *getFlowFixture1WDemand() const;
	void setFlowFixture1WDemand(QObject *value);

	QObject *getFlushMaleWDemand() const;
	void setFlushMaleWDemand(QObject *value);

	QObject *getFlushFemaleWDemand() const;
	void setFlushFemaleWDemand(QObject *value);

	QObject *getRatioMaleOccupants() const;
	void setRatioMaleOccupants(QObject *value);

	QObject *getDesignLife() const;
	void setDesignLife(QObject *value);






	// Define the method where the actual model is coded
	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID);

	// returns true as this model is parallelized
	bool canBeRunParallelly();

private:
	// Declare input data members
	RPointer<RParameter> theConcreteQuantity;
	RPointer<RParameter> theConcreteWIntensity;
	RPointer<RParameter> theConcreteTransportationDistance;
	RPointer<RParameter> theConcreteTransportationWIntensity;
	RPointer<RParameter> theSteelQuantity;
	RPointer<RParameter> theSteelWIntensity;
	RPointer<RParameter> theSteelTransportationDistance;
	RPointer<RParameter> theSteelTransportationWIntensity;
	RPointer<RParameter> theWoodQuantity;
	RPointer<RParameter> theWoodWIntensity;
	RPointer<RParameter> theWoodTransportationDistance;
	RPointer<RParameter> theWoodTransportationWIntensity;
	RPointer<RParameter> theGypsumBoardQuantity;
	RPointer<RParameter> theGypsumBoardWIntensity;
	RPointer<RParameter> theGypsumBoardTransportationDistance;
	RPointer<RParameter> theGypsumBoardTransportationWIntensity;
	RPointer<RParameter> theVapourBarrierQuantity;
	RPointer<RParameter> theVapourBarrierWIntensity;
	RPointer<RParameter> theVapourBarrierTransportationDistance;
	RPointer<RParameter> theVapourBarrierTransportationWIntensity;
	RPointer<RParameter> theInsulationQuantity;
	RPointer<RParameter> theInsulationWIntensity;
	RPointer<RParameter> theInsulationTransportationDistance;
	RPointer<RParameter> theInsulationTransportationWIntensity;
	RPointer<RParameter> theGlassQuantity;
	RPointer<RParameter> theGlassWIntensity;
	RPointer<RParameter> theGlassTransportationDistance;
	RPointer<RParameter> theGlassTransportationWIntensity;
	RPointer<RParameter> theOtherMaterialQuantity;
	RPointer<RParameter> theOtherMaterialWIntensity;
	RPointer<RParameter> theOtherMaterialTransportationDistance;
	RPointer<RParameter> theOtherMaterialTransportationWIntensity;

	RPointer<RParameter> theWorkerHourWIntensity;
	RPointer<RParameter> theTotalWorkerHours;
	RPointer<RParameter> theDaysYearlyOperation;
	RPointer<RParameter> theTotalBuildingOccupants;
	RPointer<RParameter> theFlowFixture1WDemand;
	RPointer<RParameter> theFlushMaleWDemand;
	RPointer<RParameter> theFlushFemaleWDemand;
	RPointer<RParameter> theRatioMaleOccupants;
	RPointer<RParameter> theDesignLife;




	// Declare responses
	RResponse *theWaterUsageFineResponse;

};

#endif // RDetailedBuildingLifeCycleWaterUsageModel_H
