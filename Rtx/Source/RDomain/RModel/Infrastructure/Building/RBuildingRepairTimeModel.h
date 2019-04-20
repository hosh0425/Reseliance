#pragma once
#ifndef RBuildingRepairTimeModel_H	
#define RBuildingRepairTimeModel_H	

#include "RModel.h"	

class RParameter;
class RResponse;

class RBuildingRepairTimeModel : public RModel
{
	Q_OBJECT

		Q_PROPERTY(QObject *BuildingInformationModel READ getBuildingInformationModel WRITE setBuildingInformationModel)
		Q_PROPERTY(QObject *StructuralDamageRatio READ getStructuralDamageRatio WRITE setStructuralDamageRatio)


public:
	RBuildingRepairTimeModel(QObject *parent, QString name);
	~RBuildingRepairTimeModel();

	QObject *getBuildingInformationModel() const;
	void setBuildingInformationModel(QObject *value);

	QObject *getStructuralDamageRatio() const;
	void setStructuralDamageRatio(QObject *value);


	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID);

	// returns true as this model is parallelized
	bool canBeRunParallelly();


private:
	RPointer<RModel> theBuildingInformationModel;
	RPointer<RParameter> theStructuralDamageRatio;

	// Declare responses
	RResponse *theRepairTime;
	RResponse *theRepairStatus;


};

#endif // RBuildingRepairTimeModel_H	
#pragma once
