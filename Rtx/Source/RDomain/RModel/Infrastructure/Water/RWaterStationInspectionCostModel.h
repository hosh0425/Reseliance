#pragma once
#ifndef RWaterStationInspectionCostModel_H	
#define RWaterStationInspectionCostModel_H	

#include "RModel.h"	

class RParameter;
class RResponse;

class RWaterStationInspectionCostModel : public RModel
{
	Q_OBJECT

		Q_PROPERTY(QObject *WaterStationInformationModel READ getWaterStationInformationModel WRITE setWaterStationInformationModel)
		Q_PROPERTY(QObject *InspectionCostParameter READ getInspectionCostParameter WRITE setInspectionCostParameter)
		Q_PROPERTY(QObject *InspectionStatus READ getInspectionStatus WRITE setInspectionStatus)

public:
	RWaterStationInspectionCostModel(QObject *parent, QString name);
	~RWaterStationInspectionCostModel();

	QObject *getWaterStationInformationModel() const;
	void setWaterStationInformationModel(QObject *value);

	QObject *getInspectionCostParameter() const;
	void setInspectionCostParameter(QObject *value);

	QObject *getInspectionStatus() const;
	void setInspectionStatus(QObject *value);


	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID);

	// returns true as this model is parallelized
	bool canBeRunParallelly();

private:
	RPointer<RModel> theWaterStationInformationModel;
	RPointer<RParameter> theInspectionCostParameter;
	RPointer<RParameter> theInspectionStatus;

	// Declare responses
	RResponse *theInspectionCost;


};

#endif // RWaterStationInspectionCostModel_H	
#pragma once
