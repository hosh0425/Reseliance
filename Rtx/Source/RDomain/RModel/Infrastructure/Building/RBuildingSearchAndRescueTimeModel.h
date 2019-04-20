#pragma once
#ifndef RBuildingSearchAndRescueTimeModel_H	
#define RBuildingSearchAndRescueTimeModel_H	

#include "RModel.h"	

class RParameter;
class RResponse;

class RBuildingSearchAndRescueTimeModel : public RModel
{
	Q_OBJECT

		Q_PROPERTY(QObject *BuildingInformationModel READ getBuildingInformationModel WRITE setBuildingInformationModel)
		Q_PROPERTY(RBuildingType BuildingType READ getBuildingType WRITE setBuildingType)
		Q_PROPERTY(QObject *StructuralDamageRatio READ getStructuralDamageRatio WRITE setStructuralDamageRatio)
		Q_PROPERTY(QObject *SearchAndRescueRate READ getSearchAndRescueRate WRITE setSearchAndRescueRate)
		Q_PROPERTY(QObject *CollapseUncertainty READ getCollapseUncertainty WRITE setCollapseUncertainty)

		Q_ENUMS(RBuildingType)
		
public:
	RBuildingSearchAndRescueTimeModel(QObject *parent, QString name);
	~RBuildingSearchAndRescueTimeModel();

	QObject *getBuildingInformationModel() const;
	void setBuildingInformationModel(QObject *value);

	enum RBuildingType { W1, W2, S1L, S1M, S1H, S2L, S2M, S2H, S3, S4L, S4M, S4H, S5L, S5M, S5H, C1L, C1M, C1H, C2L, C2M, C2H, C3L, C3M, C3H, PC1, PC2L, PC2M, PC2H, RM1L, RM1M, RM2L, RM2M, RM2H, URML, URMM, MH };
	RBuildingType getBuildingType() const;
	void setBuildingType(RBuildingType value);

	QObject *getCollapseUncertainty() const;
	void setCollapseUncertainty(QObject *value);

	QObject *getStructuralDamageRatio() const;
	void setStructuralDamageRatio(QObject *value);

	QObject *getSearchAndRescueRate() const;
	void setSearchAndRescueRate(QObject *value);

	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID);

	// returns true as this model is parallelized
	bool canBeRunParallelly();


private:
	RPointer<RModel> theBuildingInformationModel;
	RPointer<RParameter> theCollapseUncertainty;
	RPointer<RParameter> theStructuralDamageRatio;
	RPointer<RParameter> theSearchAndRescueRate;
	RBuildingType theBuildingType;

	// Declare responses
	RResponse *theSearchAndRescueTime;
	RResponse *theSearchAndRescueStatus;


};

#endif // RBuildingSearchAndRescueTimeModel_H	
#pragma once
