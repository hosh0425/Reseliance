#ifndef RPowerStationInspectionCostModel_H	
#define RPowerStationInspectionCostModel_H	

#include "RModel.h"	

class RParameter;
class RResponse;

class RPowerStationInspectionCostModel : public RModel
{
	Q_OBJECT

		Q_PROPERTY(QObject *PowerStationInformationModel READ getPowerStationInformationModel WRITE setPowerStationInformationModel)
		Q_PROPERTY(QObject *InspectionCostParameter READ getInspectionCostParameter WRITE setInspectionCostParameter)
		Q_PROPERTY(QObject *InspectionStatus READ getInspectionStatus WRITE setInspectionStatus)

public:
	RPowerStationInspectionCostModel(QObject *parent, QString name);
	~RPowerStationInspectionCostModel();

	QObject *getPowerStationInformationModel() const;
	void setPowerStationInformationModel(QObject *value);

	QObject *getInspectionCostParameter() const;
	void setInspectionCostParameter(QObject *value);

	QObject *getInspectionStatus() const;
	void setInspectionStatus(QObject *value);


	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID);

	// returns true as this model is parallelized
	bool canBeRunParallelly();

private:
	RPointer<RModel> thePowerStationInformationModel;
	RPointer<RParameter> theInspectionCostParameter;
	RPointer<RParameter> theInspectionStatus;

	// Declare responses
	RResponse *theInspectionCost;


};

#endif // RPowerStationInspectionCostModel_H	
#pragma once
