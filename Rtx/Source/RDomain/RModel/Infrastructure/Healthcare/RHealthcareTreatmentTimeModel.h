#pragma once
//!!! the name can be changed to Injury cost in order to include some indirect costs rather than the treatment cost
#ifndef RHealthcareTreatmentTimeModel_H
#define RHealthcareTreatmentTimeModel_H

#include "RModel.h"
#include "REventModel.h"

class RResponse;
class RParameter;



class RHealthcareTreatmentTimeModel : public RModel
{
	Q_OBJECT
		Q_PROPERTY(QObject *HealthcareInformationModel READ getHealthcareInformationModel WRITE setHealthcareInformationModel)
		Q_PROPERTY(QObject *TreatmentTimeParameter READ getTreatmentTimeParameter WRITE setTreatmentTimeParameter)

public:

	RHealthcareTreatmentTimeModel(QObject *parent, QString name);
	~RHealthcareTreatmentTimeModel();

	QObject *getHealthcareInformationModel() const;
	void setHealthcareInformationModel(QObject *value);

	QObject *getTreatmentTimeParameter() const;
	void setTreatmentTimeParameter(QObject *value);

	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID);

	// Returns true as the model is parallelized
	bool canBeRunParallelly();

private:
	RPointer<RModel> theHealthcareInformationModel;
	RPointer<RParameter> theTreatmentTimeParameter;

	RResponse *theHealthcareTreatmentTime;
};

#endif // REventModel_H
#pragma once
