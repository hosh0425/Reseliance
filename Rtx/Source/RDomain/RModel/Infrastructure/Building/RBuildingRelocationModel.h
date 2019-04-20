#pragma once
#ifndef RBuildingRelocationModel_H	
#define RBuildingRelocationModel_H	

#include "RModel.h"	

class RParameter;
class RResponse;

class RBuildingRelocationModel : public RModel
{
	Q_OBJECT

		Q_PROPERTY(QObject *BuildingInformationModel READ getBuildingInformationModel WRITE setBuildingInformationModel)
		Q_PROPERTY(QObject *TimeParameter READ getTimeParameter WRITE setTimeParameter)
		Q_PROPERTY(QObject *StructuralDamageRatio READ getStructuralDamageRatio WRITE setStructuralDamageRatio)
		Q_PROPERTY(QObject *RepairOrRestorationStatus READ getRepairOrRestorationStatus WRITE setRepairOrRestorationStatus)


public:
	RBuildingRelocationModel(QObject *parent, QString name);
	~RBuildingRelocationModel();

	QObject *getBuildingInformationModel() const;
	void setBuildingInformationModel(QObject *value);

	QObject *getTimeParameter() const;
	void setTimeParameter(QObject *value);

	QObject *getStructuralDamageRatio() const;
	void setStructuralDamageRatio(QObject *value);

	QObject *getRepairOrRestorationStatus() const;
	void setRepairOrRestorationStatus(QObject *value);

	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID);

	// returns true as this model is parallelized
	bool canBeRunParallelly();
	void reset();
	void resetHistoryVariables(int threadID);

private:
	RPointer<RModel> theBuildingInformationModel;

	RPointer<RParameter> theTimeParameter;
	RPointer<RParameter> theStructuralDamageRatio;
	RPointer<RParameter> theRepairOrRestorationStatus;

	double theInitialTime;
	double *currentTime;
	double *previousTime;

	//double *currentDemand;
	//double *previousDemand;
	//double *cumulativeRelocationCost;

	// Declare responses
	RResponse *theBuildingRelocationCost;


};

#endif // RBuildingRelocationModel_H	
#pragma once
