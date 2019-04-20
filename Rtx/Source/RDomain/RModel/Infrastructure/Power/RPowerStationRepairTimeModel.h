#ifndef RPowerStationRepairTimeModel_H	
#define RPowerStationRepairTimeModel_H	

#include "RModel.h"	

class RParameter;
class RResponse;

class RPowerStationRepairTimeModel : public RModel
{
	Q_OBJECT

		Q_PROPERTY(QObject *PowerStationInformationModel READ getPowerStationInformationModel WRITE setPowerStationInformationModel)
		Q_PROPERTY(QObject *DamageRatio READ getDamageRatio WRITE setDamageRatio)
		Q_PROPERTY(QObject *RepairTimeUncertainty READ getRepairTimeUncertainty WRITE setRepairTimeUncertainty)



public:
	RPowerStationRepairTimeModel(QObject *parent, QString name);
	~RPowerStationRepairTimeModel();

	QObject *getPowerStationInformationModel() const;
	void setPowerStationInformationModel(QObject *value);

	QObject *getDamageRatio() const;
	void setDamageRatio(QObject *value);

	QObject *getRepairTimeUncertainty() const;
	void setRepairTimeUncertainty(QObject *value);

	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID);

	// returns true as this model is parallelized
	bool canBeRunParallelly();

private:
	RPointer<RModel> thePowerStationInformationModel;
	RPointer<RParameter> theDamageRatio;
	RPointer<RParameter> theRepairTimeUncertainty;

	// Declare responses
	RResponse *theRepairTime;
	//RResponse *theRepairDemand;


};

#endif // RPowerStationRepairTimeModel_H	
#pragma once
