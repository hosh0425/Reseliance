#ifndef RHealthcareRestorationModel_H	
#define RHealthcareRestorationModel_H	

#include "RModel.h"	

class RParameter;
class RResponse;

class RHealthcareRestorationModel : public RModel
{
	Q_OBJECT

		Q_PROPERTY(QObject *HealthcareInformationModel READ getHealthcareInformationModel WRITE setHealthcareInformationModel)
		Q_PROPERTY(QObject *StructuralDamageRatio READ getStructuralDamageRatio WRITE setStructuralDamageRatio)
		Q_PROPERTY(QObject *RestorationTimeUncertainty READ getRestorationTimeUncertainty WRITE setRestorationTimeUncertainty)

public:
	RHealthcareRestorationModel(QObject *parent, QString name);
	~RHealthcareRestorationModel();

	QObject *getHealthcareInformationModel() const;
	void setHealthcareInformationModel(QObject *value);

	QObject *getStructuralDamageRatio() const;
	void setStructuralDamageRatio(QObject *value);

	QObject *getRestorationTimeUncertainty() const;
	void setRestorationTimeUncertainty(QObject *value);

	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID);

	// returns true as this model is parallelized
	bool canBeRunParallelly();

private:
	RPointer<RModel> theHealthcareInformationModel;
	RPointer<RParameter> theStructuralDamageRatio;
	RPointer<RParameter> theRestorationTimeUncertainty;

	// Declare responses
	RResponse *theRestorationTime;
	RResponse *theRestorationStatus;

};

#endif // RHealthcareRestorationModel_H	
#pragma once
