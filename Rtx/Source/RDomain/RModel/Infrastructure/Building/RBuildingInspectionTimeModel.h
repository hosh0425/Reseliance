#pragma once
#ifndef RBuildingInspectionTimeModel_H	
#define RBuildingInspectionTimeModel_H	

#include "RModel.h"	

class RParameter;
class RResponse;

class RBuildingInspectionTimeModel : public RModel
{
	Q_OBJECT

		Q_PROPERTY(QObject *BuildingInformationModel READ getBuildingInformationModel WRITE setBuildingInformationModel)
		Q_PROPERTY(RMonitoring Monitoring READ getMonitoring WRITE setMonitoring)
		Q_PROPERTY(QObject *InspectionTimeParameter READ getInspectionTimeParameter WRITE setInspectionTimeParameter)


		Q_ENUMS(RMonitoring)


public:
	RBuildingInspectionTimeModel(QObject *parent, QString name);
	~RBuildingInspectionTimeModel();

	QObject *getBuildingInformationModel() const;
	void setBuildingInformationModel(QObject *value);



	QObject *getInspectionTimeParameter() const;
	void setInspectionTimeParameter(QObject *value);


	enum RMonitoring { Sensor, Drone, Visual };
	RMonitoring getMonitoring() const;
	void setMonitoring(RMonitoring value);



	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID);

	// returns true as this model is parallelized
	bool canBeRunParallelly();


private:
	RPointer<RModel> theBuildingInformationModel;

	RPointer<RParameter> theInspectionTimeParameter;
	RMonitoring theMonitoring;


	// Declare responses
	RResponse *theInspectionTime;
	RResponse *theInspectionStatus;


};

#endif // RBuildingInspectionTimeModel_H	
#pragma once
