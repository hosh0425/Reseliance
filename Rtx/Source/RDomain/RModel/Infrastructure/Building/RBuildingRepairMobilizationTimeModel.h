#pragma once
#ifndef RBuildingRepairMobilizationTimeModel_H	
#define RBuildingRepairMobilizationTimeModel_H	

#include "RModel.h"	

class RParameter;
class RResponse;

class RBuildingRepairMobilizationTimeModel : public RModel
{
	Q_OBJECT

		Q_PROPERTY(QObject *BuildingInformationModel READ getBuildingInformationModel WRITE setBuildingInformationModel)
		Q_PROPERTY(QObject *StructuralDamageRatio READ getStructuralDamageRatio WRITE setStructuralDamageRatio)
		//Q_PROPERTY(QObject *RepairTime READ getRepairTime WRITE setRepairTime)


public:
	RBuildingRepairMobilizationTimeModel(QObject *parent, QString name);
	~RBuildingRepairMobilizationTimeModel();

	QObject *getBuildingInformationModel() const;
	void setBuildingInformationModel(QObject *value);

	QObject *getStructuralDamageRatio() const;
	void setStructuralDamageRatio(QObject *value);

	//QObject *getRepairTime() const;
	//void setRepairTime(QObject *value);



	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID);

	// returns true as this model is parallelized
	bool canBeRunParallelly();


private:
	RPointer<RModel> theBuildingInformationModel;
	RPointer<RParameter> theStructuralDamageRatio;
	//RPointer<RParameter> theRepairTime;

	// Declare responses
	RResponse *theMobilizationTime;
	RResponse *theMobilizationStatus;


};

#endif // RBuildingRepairMobilizationTimeModel_H	
#pragma once
