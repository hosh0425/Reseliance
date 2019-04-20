#pragma once
#ifndef RBuildingSearchAndRescueCostModel_H	
#define RBuildingSearchAndRescueCostModel_H	

#include "RModel.h"	

class RParameter;
class RResponse;

class RBuildingSearchAndRescueCostModel : public RModel
{
	Q_OBJECT

		Q_PROPERTY(QObject *BuildingInformationModel READ getBuildingInformationModel WRITE setBuildingInformationModel)
		Q_PROPERTY(QObject *SearchAndRescueCostRate READ getSearchAndRescueCostRate WRITE setSearchAndRescueCostRate)
		Q_PROPERTY(QObject *StructuralDamageRatio READ getStructuralDamageRatio WRITE setStructuralDamageRatio)
		Q_PROPERTY(QObject *SearchAndRescueStatus READ getSearchAndRescueStatus WRITE setSearchAndRescueStatus)
		Q_PROPERTY(QObject *SearchAndRescueTime READ getSearchAndRescueTime WRITE setSearchAndRescueTime)


public:
	RBuildingSearchAndRescueCostModel(QObject *parent, QString name);
	~RBuildingSearchAndRescueCostModel();

	QObject *getBuildingInformationModel() const;
	void setBuildingInformationModel(QObject *value);


	QObject *getStructuralDamageRatio() const;
	void setStructuralDamageRatio(QObject *value);
	QObject *getSearchAndRescueCostRate() const;
	void setSearchAndRescueCostRate(QObject *value);
	QObject *getSearchAndRescueStatus() const;
	void setSearchAndRescueStatus(QObject *value);
	QObject *getSearchAndRescueTime() const;
	void setSearchAndRescueTime(QObject *value);

	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID);

	// returns true as this model is parallelized
	bool canBeRunParallelly();


private:
	RPointer<RModel> theBuildingInformationModel;
	RPointer<RParameter> theStructuralDamageRatio;
	RPointer<RParameter> theSearchAndRescueStatus;
	RPointer<RParameter> theSearchAndRescueTime;
	RPointer<RParameter> theSearchAndRescueCostRate;

	// Declare responses
	RResponse *theSearchAndRescueCost;



};

#endif // RBuildingSearchAndRescueCostModel_H	
#pragma once
