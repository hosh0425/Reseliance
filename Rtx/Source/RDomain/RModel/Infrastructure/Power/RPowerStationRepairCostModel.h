#ifndef RPowerStationRepairCostModel_H	
#define RPowerStationRepairCostModel_H	

#include "RModel.h"	

class RParameter;
class RResponse;

class RPowerStationRepairCostModel : public RModel
{
	Q_OBJECT

		Q_PROPERTY(QObject *PowerStationInformationModel READ getPowerStationInformationModel WRITE setPowerStationInformationModel)
		Q_PROPERTY(QObject *DamageRatio READ getDamageRatio WRITE setDamageRatio)
		Q_PROPERTY(QObject *PowerStationServiceability READ getPowerStationServiceability WRITE setPowerStationServiceability)
		Q_PROPERTY(QObject *ReplacementValue READ getReplacementValue WRITE setReplacementValue)

public:
	RPowerStationRepairCostModel(QObject *parent, QString name);
	~RPowerStationRepairCostModel();

	QObject *getPowerStationInformationModel() const;
	void setPowerStationInformationModel(QObject *value);

	QObject *getDamageRatio() const;
	void setDamageRatio(QObject *value);

	QObject *getPowerStationServiceability() const;
	void setPowerStationServiceability(QObject *value);

	QObject *getReplacementValue() const;
	void setReplacementValue(QObject *value);

	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID);

	// returns true as this model is parallelized
	bool canBeRunParallelly();

private:
	RPointer<RModel> thePowerStationInformationModel;
	RPointer<RParameter> theDamageRatio;
	RPointer<RParameter> thePowerStationServiceability;
	RPointer<RParameter> theReplacementValue;

	// Declare responses
	RResponse *theRepairCost;


};

#endif // RPowerStationRepairCostModel_H	
#pragma once
