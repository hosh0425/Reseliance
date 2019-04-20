#ifndef RPowerStationDamageModel_H	
#define RPowerStationDamageModel_H	

#include "RModel.h"	

class RParameter;
class RResponse;

class RPowerStationDamageModel : public RModel
{
	Q_OBJECT

		Q_PROPERTY(QObject *PowerStationInformationModel READ getPowerStationInformationModel WRITE setPowerStationInformationModel)
		Q_PROPERTY(QObject *Intensity READ getIntensity WRITE setIntensity)
		Q_PROPERTY(QObject *DamageStateUncertainty READ getDamageStateUncertainty WRITE setDamageStateUncertainty)
		Q_PROPERTY(QObject *DamageRatioUncertainty READ getDamageRatioUncertainty WRITE setDamageRatioUncertainty)

public:
	RPowerStationDamageModel(QObject *parent, QString name);
	~RPowerStationDamageModel();

	QObject *getPowerStationInformationModel() const;
	void setPowerStationInformationModel(QObject *value);
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
	RPointer<RModel> thePowerStationInformationModel;
	RPointer<RParameter> theIntensity;
	RPointer<RParameter> theDamageStateUncertainty;
	RPointer<RParameter> theDamageRatioUncertainty;

	// Declare responses
	RResponse *theDamageRatio;
	RResponse *theDamageState;
	//RResponse *theServiceability;
};

#endif // RPowerStationDamageModel_H	
#pragma once
