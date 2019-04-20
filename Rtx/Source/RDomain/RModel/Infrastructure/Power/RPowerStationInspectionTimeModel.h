#ifndef RPowerStationInspectionTimeModel_H	
#define RPowerStationInspectionTimeModel_H	

#include "RModel.h"	

class RParameter;
class RResponse;

class RPowerStationInspectionTimeModel : public RModel
{
	Q_OBJECT

		Q_PROPERTY(QObject *PowerStationInformationModel READ getPowerStationInformationModel WRITE setPowerStationInformationModel)
		Q_PROPERTY(RMonitoring Monitoring READ getMonitoring WRITE setMonitoring)
		Q_PROPERTY(QObject *InspectionTimeParameter READ getInspectionTimeParameter WRITE setInspectionTimeParameter)


		Q_ENUMS(RMonitoring)


public:
	RPowerStationInspectionTimeModel(QObject *parent, QString name);
	~RPowerStationInspectionTimeModel();

	QObject *getPowerStationInformationModel() const;
	void setPowerStationInformationModel(QObject *value);



	QObject *getInspectionTimeParameter() const;
	void setInspectionTimeParameter(QObject *value);


	enum RMonitoring { Sensor, Drone, Visual };
	RMonitoring getMonitoring() const;
	void setMonitoring(RMonitoring value);



	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID);

	// returns true as this model is parallelized
	bool canBeRunParallelly();


private:
	RPointer<RModel> thePowerStationInformationModel;

	RPointer<RParameter> theInspectionTimeParameter;
	RMonitoring theMonitoring;


	// Declare responses
	RResponse *theInspectionTime;
	RResponse *theInspectionStatus;


};

#endif // RPowerStationInspectionTimeModel_H	
#pragma once
