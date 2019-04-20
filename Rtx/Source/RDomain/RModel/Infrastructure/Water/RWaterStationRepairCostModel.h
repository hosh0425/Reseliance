#ifndef RWaterStationRepairCostModel_H	
#define RWaterStationRepairCostModel_H	

#include "RModel.h"	

class RParameter;
class RResponse;

class RWaterStationRepairCostModel : public RModel
{
	Q_OBJECT

		Q_PROPERTY(QObject *WaterStationInformationModel READ getWaterStationInformationModel WRITE setWaterStationInformationModel)
		Q_PROPERTY(QObject *DamageRatio READ getDamageRatio WRITE setDamageRatio)
		Q_PROPERTY(QObject *WaterStationFunctionality READ getWaterStationFunctionality WRITE setWaterStationFunctionality)
		Q_PROPERTY(QObject *ReplacementValue READ getReplacementValue WRITE setReplacementValue)

public:
	RWaterStationRepairCostModel(QObject *parent, QString name);
	~RWaterStationRepairCostModel();

	QObject *getWaterStationInformationModel() const;
	void setWaterStationInformationModel(QObject *value);

	QObject *getDamageRatio() const;
	void setDamageRatio(QObject *value);

	QObject *getWaterStationFunctionality() const;
	void setWaterStationFunctionality(QObject *value);

	QObject *getReplacementValue() const;
	void setReplacementValue(QObject *value);

	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID);

	// returns true as this model is parallelized
	bool canBeRunParallelly();

private:
	RPointer<RModel> theWaterStationInformationModel;
	RPointer<RParameter> theDamageRatio;
	RPointer<RParameter> theWaterStationFunctionality;
	RPointer<RParameter> theReplacementValue;

	// Declare responses
	RResponse *theRepairCost;


};

#endif // RWaterStationRepairCostModel_H	
#pragma once
