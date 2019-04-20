#pragma once
#ifndef RWaterStationServiceabilityModel_H	
#define RWaterStationServiceabilityModel_H	

#include "RModel.h"	

class RParameter;
class RResponse;

class RWaterStationServiceabilityModel : public RModel
{
	Q_OBJECT

		Q_PROPERTY(QObject *WaterStationInformationModel READ getWaterStationInformationModel WRITE setWaterStationInformationModel)
		Q_PROPERTY(QObject *Functionality READ getFunctionality WRITE setFunctionality)
		Q_PROPERTY(QObject *PowerServiceability READ getPowerServiceability WRITE setPowerServiceability)

public:
	RWaterStationServiceabilityModel(QObject *parent, QString name);
	~RWaterStationServiceabilityModel();

	QObject *getWaterStationInformationModel() const;
	void setWaterStationInformationModel(QObject *value);

	QObject *getFunctionality() const;
	void setFunctionality(QObject *value);

	QObject *getPowerServiceability() const;
	void setPowerServiceability(QObject *value);

	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID);

	// returns true as this model is parallelized
	bool canBeRunParallelly();


private:
	RPointer<RModel> theWaterStationInformationModel;
	RPointer<RParameter> theFunctionality;
	RPointer<RParameter> thePowerServiceability;

	// Declare responses
	RResponse *theServiceability;


};

#endif // RWaterStationServiceabilityModel_H	
#pragma once

