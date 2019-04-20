#pragma once
#ifndef RBuildingInspectionCostModel_H	
#define RBuildingInspectionCostModel_H	

#include "RModel.h"	

class RParameter;
class RResponse;

class RBuildingInspectionCostModel : public RModel
{
	Q_OBJECT

		Q_PROPERTY(QObject *BuildingInformationModel READ getBuildingInformationModel WRITE setBuildingInformationModel)
		Q_PROPERTY(QObject *InspectionCostParameter READ getInspectionCostParameter WRITE setInspectionCostParameter)
		Q_PROPERTY(QObject *InspectionStatus READ getInspectionStatus WRITE setInspectionStatus)


		Q_ENUMS(RMonitoring)


public:
	RBuildingInspectionCostModel(QObject *parent, QString name);
	~RBuildingInspectionCostModel();

	QObject *getBuildingInformationModel() const;
	void setBuildingInformationModel(QObject *value);
	
	QObject *getInspectionCostParameter() const;
	void setInspectionCostParameter(QObject *value);

	QObject *getInspectionStatus() const;
	void setInspectionStatus(QObject *value);

	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID);

	// returns true as this model is parallelized
	bool canBeRunParallelly();


private:
	RPointer<RModel> theBuildingInformationModel;

	RPointer<RParameter> theInspectionCostParameter;
	RPointer<RParameter> theInspectionStatus;


	// Declare responses
	RResponse *theInspectionCost;


};

#endif // RBuildingInspectionCostModel_H	
#pragma once
