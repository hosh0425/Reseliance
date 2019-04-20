#ifndef RPowerStationServiceabilityModel_H	
#define RPowerStationServiceabilityModel_H	

#include "RModel.h"	

class RParameter;
class RResponse;

class RPowerStationServiceabilityModel : public RModel
{
	Q_OBJECT

		Q_PROPERTY(QObject *PowerStationInformationModel READ getPowerStationInformationModel WRITE setPowerStationInformationModel)
		Q_PROPERTY(QObject *DamageRatio READ getDamageRatio WRITE setDamageRatio)
		Q_PROPERTY(QObject *DamageState READ getDamageState WRITE setDamageState)

public:
	RPowerStationServiceabilityModel(QObject *parent, QString name);
	~RPowerStationServiceabilityModel();

	QObject *getPowerStationInformationModel() const;
	void setPowerStationInformationModel(QObject *value);

	QObject *getDamageRatio() const;
	void setDamageRatio(QObject *value);

	QObject *getDamageState() const;
	void setDamageState(QObject *value);


	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID);

	// returns true as this model is parallelized
	bool canBeRunParallelly();


private:
	RPointer<RModel> thePowerStationInformationModel;
	RPointer<RParameter> theDamageRatio;
	RPointer<RParameter> theDamageState;

	// Declare responses
	RResponse *theServiceability;


};

#endif // RPowerStationServiceabilityModel_H	
#pragma once
