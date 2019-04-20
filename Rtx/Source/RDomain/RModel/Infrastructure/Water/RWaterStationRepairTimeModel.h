#pragma once
#ifndef RWaterStationRepairTimeModel_H	
#define RWaterStationRepairTimeModel_H	

#include "RModel.h"	

class RParameter;
class RResponse;

class RWaterStationRepairTimeModel : public RModel
{
	Q_OBJECT

		Q_PROPERTY(QObject *WaterStationInformationModel READ getWaterStationInformationModel WRITE setWaterStationInformationModel)
		Q_PROPERTY(QObject *DamageRatio READ getDamageRatio WRITE setDamageRatio)
		Q_PROPERTY(QObject *RepairTimeUncertainty READ getRepairTimeUncertainty WRITE setRepairTimeUncertainty)



public:
	RWaterStationRepairTimeModel(QObject *parent, QString name);
	~RWaterStationRepairTimeModel();

	QObject *getWaterStationInformationModel() const;
	void setWaterStationInformationModel(QObject *value);

	QObject *getDamageRatio() const;
	void setDamageRatio(QObject *value);

	QObject *getRepairTimeUncertainty() const;
	void setRepairTimeUncertainty(QObject *value);

	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID);

	// returns true as this model is parallelized
	bool canBeRunParallelly();

private:
	RPointer<RModel> theWaterStationInformationModel;
	RPointer<RParameter> theDamageRatio;
	RPointer<RParameter> theRepairTimeUncertainty;

	// Declare responses
	RResponse *theRepairTime;
	//RResponse *theRepairDemand;


};

#endif // RWaterStationRepairTimeModel_H	

