#pragma once
#ifndef RHealthcareCapacityModel_H	
#define RHealthcareCapacityModel_H	

#include "RModel.h"	

class RParameter;
class RResponse;
class RTime;

class RHealthcareCapacityModel : public RModel
{
	Q_OBJECT

		Q_PROPERTY(QObject *HealthcareInformationModel READ getHealthcareInformationModel WRITE setHealthcareInformationModel)
		Q_PROPERTY(QObject *NormalDemand READ getNormalDemand WRITE setNormalDemand)

		Q_PROPERTY(QObject *StructuralDamageRatio READ getStructuralDamageRatio WRITE setStructuralDamageRatio)
		Q_PROPERTY(QObject *NonstructuralDriftDamageRatio READ getNonstructuralDriftDamageRatio WRITE setNonstructuralDriftDamageRatio)
		Q_PROPERTY(QObject *NonstructuralAccelerationDamageRatio READ getNonstructuralAccelerationDamageRatio WRITE setNonstructuralAccelerationDamageRatio)
		Q_PROPERTY(QObject *ContentDamageRatio READ getContentDamageRatio WRITE setContentDamageRatio)

		Q_PROPERTY(QObject *RepairStatus READ getRepairStatus WRITE setRepairStatus)
		Q_PROPERTY(QObject *RestorationStatus READ getRestorationStatus WRITE setRestorationStatus)
		Q_PROPERTY(QObject *RecaptureFactor READ getRecaptureFactor WRITE setRecaptureFactor)

		Q_PROPERTY(QObject *PowerServiceability READ getPowerServiceability WRITE setPowerServiceability)
		Q_PROPERTY(QObject *PowerDependency READ getPowerDependency WRITE setPowerDependency)

		Q_PROPERTY(QObject *WaterServiceability READ getWaterServiceability WRITE setWaterServiceability)
		Q_PROPERTY(QObject *WaterDependency READ getWaterDependency WRITE setWaterDependency)

public:
	RHealthcareCapacityModel(QObject *parent, QString name);
	~RHealthcareCapacityModel();

	QObject *getHealthcareInformationModel() const;
	void setHealthcareInformationModel(QObject *value);

	QObject *getNormalDemand() const;
	void setNormalDemand(QObject *value);

	QObject *getStructuralDamageRatio() const;
	void setStructuralDamageRatio(QObject *value);
	QObject *getNonstructuralDriftDamageRatio() const;
	void setNonstructuralDriftDamageRatio(QObject *value);
	QObject *getNonstructuralAccelerationDamageRatio() const;
	void setNonstructuralAccelerationDamageRatio(QObject *value);
	QObject *getContentDamageRatio() const;
	void setContentDamageRatio(QObject *value);

	QObject *getRecaptureFactor() const;
	void setRecaptureFactor(QObject *value);
	QObject *getRestorationStatus() const;
	void setRestorationStatus(QObject *value);
	QObject *getRepairStatus() const;
	void setRepairStatus(QObject *value);
	QObject *getPowerServiceability() const;
	void setPowerServiceability(QObject *value);
	QObject *getWaterServiceability() const;
	void setWaterServiceability(QObject *value);

	QObject *getWaterDependency() const;
	void setWaterDependency(QObject *value);
	QObject *getPowerDependency() const;
	void setPowerDependency(QObject *value);

	void resetHistoryVariables(int threadID);

	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID);

	// returns true as this model is parallelized
	bool canBeRunParallelly();

private:
	RPointer<RModel> theHealthcareInformationModel;

	RPointer<RParameter> theStructuralDamageRatio;
	RPointer<RParameter> theNonstructuralDriftDamageRatio;
	RPointer<RParameter> theNonstructuralAccelerationDamageRatio;
	RPointer<RParameter> theContentDamageRatio;


	RPointer<RParameter> theNormalDemand;

	RPointer<RParameter> theRecaptureFactor;
	RPointer<RParameter> theRestorationStatus;
	RPointer<RParameter> theRepairStatus;
	RPointer<RParameter> thePowerServiceability;
	RPointer<RParameter> theWaterServiceability;

	RPointer<RParameter> thePowerDependency;
	RPointer<RParameter> theWaterDependency;

	int *previousPotentialCapacity;
	// Declare responses
	RResponse *theHealthcarePotentialCapacity;
	RResponse *theHealthcareAvailableCapacity;
	RResponse *theHealthcareAbsoluteCapacity;

};

#endif // RHealthcareCapacityModel_H	
#pragma once
#pragma once
