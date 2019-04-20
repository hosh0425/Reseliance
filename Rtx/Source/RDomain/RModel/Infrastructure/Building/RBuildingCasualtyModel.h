#pragma once
#ifndef RBuildingCasualtyModel_H	
#define RBuildingCasualtyModel_H	

#include "RModel.h"	

class RParameter;
class RResponse;

class RBuildingCasualtyModel : public RModel
{
	Q_OBJECT

		Q_PROPERTY(QObject *BuildingInformationModel READ getBuildingInformationModel WRITE setBuildingInformationModel)
		Q_PROPERTY(QObject *Population READ getPopulation WRITE setPopulation)
		Q_PROPERTY(RBuildingType BuildingType READ getBuildingType WRITE setBuildingType)
		Q_PROPERTY(QObject *StructuralDamageRatio READ getStructuralDamageRatio WRITE setStructuralDamageRatio)
		Q_PROPERTY(QObject *RescueStatus READ getRescueStatus WRITE setRescueStatus)

		Q_ENUMS(RBuildingType)

		
public:
	RBuildingCasualtyModel(QObject *parent, QString name);
	~RBuildingCasualtyModel();

	QObject *getBuildingInformationModel() const;
	void setBuildingInformationModel(QObject *value);

	QObject *getPopulation() const;
	void setPopulation(QObject *value);

	enum RBuildingType { W1, W2, S1L, S1M, S1H, S2L, S2M, S2H, S3, S4L, S4M, S4H, S5L, S5M, S5H, C1L, C1M, C1H, C2L, C2M, C2H, C3L, C3M, C3H, PC1, PC2L, PC2M, PC2H, RM1L, RM1M, RM2L, RM2M, RM2H, URML, URMM, MH };
	RBuildingType getBuildingType() const;
	void setBuildingType(RBuildingType value);

	QObject *getStructuralDamageRatio() const;
	void setStructuralDamageRatio(QObject *value);

	QObject *getRescueStatus() const;
	void setRescueStatus(QObject *value);

	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID);

	// returns true as this model is parallelized
	bool canBeRunParallelly();
	void resetHistoryVariables(int threadID);


private:
	RPointer<RModel> theBuildingInformationModel;
	RPointer<RParameter> thePopulation;

	RBuildingType theBuildingType;

	RPointer<RParameter> theStructuralDamageRatio;
	RPointer<RParameter> theRescueStatus;

	// Declare responses
	RResponse *theRescuedUninjuredPopulation;
	RResponse *theRescuedInjuredPopulation;
	RResponse *theRescuedDeadPopulation;

	RResponse *theUnrescuedUninjuredPopulation;
	RResponse *theUnrescuedInjuredPopulation;
	RResponse *theUnrescuedDeadPopulation;


};

#endif // RBuildingCasualtyModel_H	
#pragma once

