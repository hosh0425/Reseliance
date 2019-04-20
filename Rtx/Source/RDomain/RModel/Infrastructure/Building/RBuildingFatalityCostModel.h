#pragma once
#ifndef RBuildingFatalityCostModel_H
#define RBuildingFatalityCostModel_H

#include "RModel.h"
#include "REventModel.h"

class RResponse;
class RParameter;



class RBuildingFatalityCostModel : public RModel
{
	Q_OBJECT
		Q_PROPERTY(QObject *BuildingInformationModel READ getBuildingInformationModel WRITE setBuildingInformationModel)
		Q_PROPERTY(QObject *NumberOfFatalities READ getNumberOfFatalities WRITE setNumberOfFatalities)
		Q_PROPERTY(QObject *CostOfOneFatality READ getCostOfOneFatality WRITE setCostOfOneFatality)

public:

	RBuildingFatalityCostModel(QObject *parent, QString name);
	~RBuildingFatalityCostModel();

	QObject *getBuildingInformationModel() const;
	void setBuildingInformationModel(QObject *value);

	QObject *getNumberOfFatalities() const;
	void setNumberOfFatalities(QObject *value);

	QObject *getCostOfOneFatality() const;
	void setCostOfOneFatality(QObject *value);

	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID);
	void resetHistoryVariables(int threadID);
	// Returns true as the model is parallelized
	bool canBeRunParallelly();

private:
	RPointer<RModel> theBuildingInformationModel;
	RPointer<RParameter> theNumberOfFatalities;
	RPointer<RParameter> theCostOfOneFatality;

	double *previousNumFat;


	RResponse *theFatalityCost;
};

#endif // REventModel_H
#pragma once
