#ifndef RCommercialLossModel_H	
#define RCommercialLossModel_H	

#include "RModel.h"	

class RParameter;
class RResponse;
class RTime;

class RCommercialLossModel : public RModel
{
	Q_OBJECT

		Q_PROPERTY(QObject *CommercialInformationModel READ getCommercialInformationModel WRITE setCommercialInformationModel)
		Q_PROPERTY(QObject *NormalDailyRevenue READ getNormalDailyRevenue WRITE setNormalDailyRevenue)

		Q_PROPERTY(QObject *StructuralDamageRatio READ getStructuralDamageRatio WRITE setStructuralDamageRatio)
		Q_PROPERTY(QObject *BuildingRepairStatus READ getBuildingRepairStatus WRITE setBuildingRepairStatus)
		Q_PROPERTY(QObject *BuildingDependency READ getBuildingDependency WRITE setBuildingDependency)

		Q_PROPERTY(QObject *BuildingRestorationStatus READ getBuildingRestorationStatus WRITE setBuildingRestorationStatus)
		Q_PROPERTY(QObject *RecaptureFactor READ getRecaptureFactor WRITE setRecaptureFactor)

		Q_PROPERTY(QObject *PowerServiceability READ getPowerServiceability WRITE setPowerServiceability)
		Q_PROPERTY(QObject *PowerDependency READ getPowerDependency WRITE setPowerDependency)

		Q_PROPERTY(QObject *WaterServiceability READ getWaterServiceability WRITE setWaterServiceability)
		Q_PROPERTY(QObject *WaterDependency READ getWaterDependency WRITE setWaterDependency)

		Q_PROPERTY(QObject *TimeParameter READ getTimeParameter WRITE setTimeParameter)

public:
	RCommercialLossModel(QObject *parent, QString name);
	~RCommercialLossModel();

	QObject *getCommercialInformationModel() const;
	void setCommercialInformationModel(QObject *value);
	QObject *getNormalDailyRevenue() const;
	void setNormalDailyRevenue(QObject *value);
	QObject *getStructuralDamageRatio() const;
	void setStructuralDamageRatio(QObject *value);
	QObject *getRecaptureFactor() const;
	void setRecaptureFactor(QObject *value);
	QObject *getBuildingRepairStatus() const;
	void setBuildingRepairStatus(QObject *value);
	QObject *getBuildingRestorationStatus() const;
	void setBuildingRestorationStatus(QObject *value);
	QObject *getPowerServiceability() const;
	void setPowerServiceability(QObject *value);
	QObject *getWaterServiceability() const;
	void setWaterServiceability(QObject *value);

	QObject *getWaterDependency() const;
	void setWaterDependency(QObject *value);
	QObject *getPowerDependency() const;
	void setPowerDependency(QObject *value);
	QObject *getBuildingDependency() const;
	void setBuildingDependency(QObject *value);

	QObject *getTimeParameter() const;
	void setTimeParameter(QObject *value);

	void reset();
	void resetHistoryVariables(int threadID);

	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID);

	// returns true as this model is parallelized
	bool canBeRunParallelly();

private:
	RPointer<RModel> theCommercialInformationModel;
	RPointer<RParameter> theNormalDailyRevenue;
	RPointer<RParameter> theRecaptureFactor;

	RPointer<RParameter> theStructuralDamageRatio;
	RPointer<RParameter> theBuildingRepairStatus;
	RPointer<RParameter> theBuildingRestorationStatus;
	RPointer<RParameter> thePowerServiceability;
	RPointer<RParameter> theWaterServiceability;

	RPointer<RParameter> theBuildingDependency;
	RPointer<RParameter> thePowerDependency;
	RPointer<RParameter> theWaterDependency;
	
	RPointer<RTime> theTimeParameter;

	//A variable that keeps the initial time that the analysis starts
	double theInitialTime;
	double *currentTime;
	double *previousTime;

	//A variable that keeps the current and previous Revenue based on the status of utilities and building and ...
	double *currentRevenue;
	double *previousPotentialRevenue;

	double *cumulativeCommercialLoss;

	// Declare responses
	RResponse *theCommercialLoss;
	RResponse *theCurrentPotentialRevenue;
	RResponse *theCurrentAbsoluteRevenue;
	
};

#endif // RCommercialLossModel_H	
#pragma once
#pragma once
