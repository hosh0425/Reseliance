#ifndef RBuildingUtilityModel_H	
#define RBuildingUtilityModel_H	

#include "RModel.h"	

class RParameter;
class RResponse;

class RBuildingUtilityModel : public RModel
{
	Q_OBJECT

		Q_PROPERTY(QObject *BuildingInformationModel READ getBuildingInformationModel WRITE setBuildingInformationModel)

		Q_PROPERTY(QObject *PowerServiceability READ getPowerServiceability WRITE setPowerServiceability)

		Q_PROPERTY(QObject *WaterServiceability READ getWaterServiceability WRITE setWaterServiceability)

public:
	RBuildingUtilityModel(QObject *parent, QString name);
	~RBuildingUtilityModel();

	QObject *getBuildingInformationModel() const;
	void setBuildingInformationModel(QObject *value);

	QObject *getPowerServiceability() const;
	void setPowerServiceability(QObject *value);
	QObject *getWaterServiceability() const;
	void setWaterServiceability(QObject *value);


	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID);

	// returns true as this model is parallelized
	bool canBeRunParallelly();

private:
	RPointer<RModel> theBuildingInformationModel;

	RPointer<RParameter> thePowerServiceability;
	RPointer<RParameter> theWaterServiceability;


	// Declare responses
	RResponse *theBuildingPowerUtility;
	RResponse *theBuildingWaterUtility;

};

#endif // RBuildingUtilityModel_H	
#pragma once
