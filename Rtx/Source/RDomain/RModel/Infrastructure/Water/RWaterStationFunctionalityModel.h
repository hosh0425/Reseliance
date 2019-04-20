#pragma once
#ifndef RWaterStationFunctionalityModel_H	
#define RWaterStationFunctionalityModel_H	

#include "RModel.h"	

class RParameter;
class RResponse;

class RWaterStationFunctionalityModel : public RModel
{
	Q_OBJECT

		Q_PROPERTY(QObject *WaterStationInformationModel READ getWaterStationInformationModel WRITE setWaterStationInformationModel)
		Q_PROPERTY(QObject *DamageRatio READ getDamageRatio WRITE setDamageRatio)
		Q_PROPERTY(QObject *DamageState READ getDamageState WRITE setDamageState)

public:
	RWaterStationFunctionalityModel(QObject *parent, QString name);
	~RWaterStationFunctionalityModel();

	QObject *getWaterStationInformationModel() const;
	void setWaterStationInformationModel(QObject *value);

	QObject *getDamageRatio() const;
	void setDamageRatio(QObject *value);

	QObject *getDamageState() const;
	void setDamageState(QObject *value);


	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID);

	// returns true as this model is parallelized
	bool canBeRunParallelly();


private:
	RPointer<RModel> theWaterStationInformationModel;
	RPointer<RParameter> theDamageRatio;
	RPointer<RParameter> theDamageState;

	// Declare responses
	RResponse *theFunctionality;


};

#endif // RWaterStationFunctionalityModel_H	
#pragma once

