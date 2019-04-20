#pragma once
#ifndef RWaterStationInspectionTimeModel_H	
#define RWaterStationInspectionTimeModel_H	

#include "RModel.h"	

class RParameter;
class RResponse;

class RWaterStationInspectionTimeModel : public RModel
{
	Q_OBJECT

		Q_PROPERTY(QObject *WaterStationInformationModel READ getWaterStationInformationModel WRITE setWaterStationInformationModel)
		Q_PROPERTY(RMonitoring Monitoring READ getMonitoring WRITE setMonitoring)
		Q_PROPERTY(QObject *InspectionTimeParameter READ getInspectionTimeParameter WRITE setInspectionTimeParameter)


		Q_ENUMS(RMonitoring)


public:
	RWaterStationInspectionTimeModel(QObject *parent, QString name);
	~RWaterStationInspectionTimeModel();

	QObject *getWaterStationInformationModel() const;
	void setWaterStationInformationModel(QObject *value);



	QObject *getInspectionTimeParameter() const;
	void setInspectionTimeParameter(QObject *value);


	enum RMonitoring { Sensor, Drone, Visual };
	RMonitoring getMonitoring() const;
	void setMonitoring(RMonitoring value);



	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID);

	// returns true as this model is parallelized
	bool canBeRunParallelly();


private:
	RPointer<RModel> theWaterStationInformationModel;

	RPointer<RParameter> theInspectionTimeParameter;
	RMonitoring theMonitoring;


	// Declare responses
	RResponse *theInspectionTime;
	RResponse *theInspectionStatus;


};

#endif // RWaterStationInspectionTimeModel_H	
#pragma once
