#pragma once
#ifndef RBuildingRestorationModel_H	
#define RBuildingRestorationModel_H	

#include "RModel.h"	

class RParameter;
class RResponse;

class RBuildingRestorationModel : public RModel
{
	Q_OBJECT

		Q_PROPERTY(QObject *BuildingInformationModel READ getBuildingInformationModel WRITE setBuildingInformationModel)
		Q_PROPERTY(QObject *StructuralDamageRatio READ getStructuralDamageRatio WRITE setStructuralDamageRatio)
		Q_PROPERTY(QObject *RepairTime READ getRepairTime WRITE setRepairTime)
		Q_PROPERTY(QObject *MobilizationTime READ getMobilizationTime WRITE setMobilizationTime)


public:
	RBuildingRestorationModel(QObject *parent, QString name);
	~RBuildingRestorationModel();

	QObject *getBuildingInformationModel() const;
	void setBuildingInformationModel(QObject *value);

	QObject *getStructuralDamageRatio() const;
	void setStructuralDamageRatio(QObject *value);

	QObject *getRepairTime() const;
	void setRepairTime(QObject *value);
	QObject *getMobilizationTime() const;
	void setMobilizationTime(QObject *value);

	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID);

	// returns true as this model is parallelized
	bool canBeRunParallelly();


private:
	RPointer<RModel> theBuildingInformationModel;
	RPointer<RParameter> theStructuralDamageRatio;
	RPointer<RParameter> theRepairTime;
	RPointer<RParameter> theMobilizationTime;

	// Declare responses
	RResponse *theRestorationTime;
	RResponse *theRestorationStatus;


};

#endif // RBuildingRestorationModel_H	
#pragma once
