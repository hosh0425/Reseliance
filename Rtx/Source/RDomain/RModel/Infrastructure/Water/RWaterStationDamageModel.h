#pragma once
#ifndef RWaterStationDamageModel_H	
#define RWaterStationDamageModel_H	

#include "RModel.h"	

class RParameter;
class RResponse;

class RWaterStationDamageModel : public RModel
{
	Q_OBJECT

		Q_PROPERTY(QObject *WaterStationInformationModel READ getWaterStationInformationModel WRITE setWaterStationInformationModel)
		Q_PROPERTY(QObject *Intensity READ getIntensity WRITE setIntensity)
		Q_PROPERTY(QObject *DamageStateUncertainty READ getDamageStateUncertainty WRITE setDamageStateUncertainty)
		Q_PROPERTY(QObject *DamageRatioUncertainty READ getDamageRatioUncertainty WRITE setDamageRatioUncertainty)

public:
	RWaterStationDamageModel(QObject *parent, QString name);
	~RWaterStationDamageModel();

	QObject *getWaterStationInformationModel() const;
	void setWaterStationInformationModel(QObject *value);
	QObject *getIntensity() const;
	void setIntensity(QObject *value);
	QObject *getDamageStateUncertainty() const;
	void setDamageStateUncertainty(QObject *value);
	QObject *getDamageRatioUncertainty() const;
	void setDamageRatioUncertainty(QObject *value);


	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID);

	// returns true as this model is parallelized
	bool canBeRunParallelly();

private:
	RPointer<RModel> theWaterStationInformationModel;
	RPointer<RParameter> theIntensity;
	RPointer<RParameter> theDamageStateUncertainty;
	RPointer<RParameter> theDamageRatioUncertainty;

	// Declare responses
	RResponse *theDamageRatio;
	RResponse *theDamageState;
	//RResponse *theFunctionality;

};

#endif // RWaterStationDamageModel_H	
#pragma once
