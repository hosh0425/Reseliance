#pragma once
//!!! the name can be changed to Injury cost in order to include some indirect costs rather than the treatment cost
#ifndef RHealthcareTreatmentCostModel_H
#define RHealthcareTreatmentCostModel_H

#include "RModel.h"
#include "REventModel.h"

class RResponse;
class RParameter;



class RHealthcareTreatmentCostModel : public RModel
{
	Q_OBJECT
		Q_PROPERTY(QObject *HealthcareInformationModel READ getHealthcareInformationModel WRITE setHealthcareInformationModel)
		Q_PROPERTY(int NumberOfInjuryLevels READ getNumberOfInjuryLevels WRITE setNumberOfInjuryLevels)
		Q_PROPERTY(QString NumberOfTreatedInjuriesList READ getNumberOfTreatedInjuriesList WRITE setNumberOfTreatedInjuriesList)
		Q_PROPERTY(QString UnitCostOfTreatmentList READ getUnitCostOfTreatmentList WRITE setUnitCostOfTreatmentList)

public:

	RHealthcareTreatmentCostModel(QObject *parent, QString name);
	~RHealthcareTreatmentCostModel();

	QObject *getHealthcareInformationModel() const;
	void setHealthcareInformationModel(QObject *value);

	int getNumberOfInjuryLevels();
	void setNumberOfInjuryLevels(int value);

	QString getNumberOfTreatedInjuriesList();
	void setNumberOfTreatedInjuriesList(QString value);

	QString getUnitCostOfTreatmentList();
	void setUnitCostOfTreatmentList(QString value);

	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID);
	void resetHistoryVariables(int threadID);
	// Returns true as the model is parallelized
	bool canBeRunParallelly();

private:
	RPointer<RModel> theHealthcareInformationModel;
	int theNumberOfInjuryLevels;
	QList<RPointer<RParameter> > theNumberOfTreatedInjuriesList;
	QList<RPointer<RParameter> > theUnitCostOfTreatmentList;

	QList<double> *previousNumTreatedList;
	RResponse *theHealthcareTreatmentCost;
};

#endif // REventModel_H
#pragma once
