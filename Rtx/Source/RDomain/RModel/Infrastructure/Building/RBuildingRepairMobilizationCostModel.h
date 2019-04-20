#pragma once
#pragma once
#ifndef RBuildingRepairMobilizationCostModel_H	
#define RBuildingRepairMobilizationCostModel_H	

#include "RModel.h"	

class RParameter;
class RResponse;

class RBuildingRepairMobilizationCostModel : public RModel
{
	Q_OBJECT

		Q_PROPERTY(QObject *BuildingInformationModel READ getBuildingInformationModel WRITE setBuildingInformationModel)
		Q_PROPERTY(QObject *StructuralDamageRatio READ getStructuralDamageRatio WRITE setStructuralDamageRatio)
		Q_PROPERTY(QObject *MobilizationCostParameter READ getMobilizationCostParameter WRITE setMobilizationCostParameter)
		Q_PROPERTY(QObject *MobilizationStatus READ getMobilizationStatus WRITE setMobilizationStatus)


public:
	RBuildingRepairMobilizationCostModel(QObject *parent, QString name);
	~RBuildingRepairMobilizationCostModel();

	QObject *getBuildingInformationModel() const;
	void setBuildingInformationModel(QObject *value);

	QObject *getStructuralDamageRatio() const;
	void setStructuralDamageRatio(QObject *value);

	QObject *getMobilizationCostParameter() const;
	void setMobilizationCostParameter(QObject *value);

	QObject *getMobilizationStatus() const;
	void setMobilizationStatus(QObject *value);

	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID);

	// returns true as this model is parallelized
	bool canBeRunParallelly();


private:
	RPointer<RModel> theBuildingInformationModel;
	RPointer<RParameter> theStructuralDamageRatio;

	RPointer<RParameter> theMobilizationCostParameter;
	RPointer<RParameter> theMobilizationStatus;


	// Declare responses
	RResponse *theMobilizationCost;


};

#endif // RBuildingRepairMobilizationCostModel_H	
#pragma once
