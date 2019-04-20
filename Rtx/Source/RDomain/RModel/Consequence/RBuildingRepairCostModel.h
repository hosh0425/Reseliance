#ifndef RBuildingRepairCostModel_H
#define RBuildingRepairCostModel_H

#include "RModel.h"

class RParameter;
class RResponse;

class RBuildingRepairCostModel : public RModel
{
	Q_OBJECT	
		
	Q_PROPERTY(QObject *BuildingInformationModel READ getBuildingInformationModel WRITE setBuildingInformationModel)
	Q_PROPERTY(double RepairabilityStructuralDamageRatio READ getRepairabilityStructuralDamageRatio WRITE setRepairabilityStructuralDamageRatio)
	Q_PROPERTY(QObject *StructuralDamageRatio READ getStructuralDamageRatio WRITE setStructuralDamageRatio)
	Q_PROPERTY(QObject *NonstructuralDriftDamageRatio READ getNonstructuralDriftDamageRatio WRITE setNonstructuralDriftDamageRatio)
	Q_PROPERTY(QObject *NonstructuralAccelerationDamageRatio READ getNonstructuralAccelerationDamageRatio WRITE setNonstructuralAccelerationDamageRatio)
	Q_PROPERTY(QObject *ContentDamageRatio READ getContentDamageRatio WRITE setContentDamageRatio)
	Q_PROPERTY(double StructuralUnitAreaCost READ getStructuralUnitAreaCost WRITE setStructuralUnitAreaCost)
	Q_PROPERTY(double NonstructuralDriftUnitAreaCost READ getNonstructuralDriftUnitAreaCost WRITE setNonstructuralDriftUnitAreaCost)
	Q_PROPERTY(double NonstructuralAccelerationUnitAreaCost READ getNonstructuralAccelerationUnitAreaCost WRITE setNonstructuralAccelerationUnitAreaCost)
	Q_PROPERTY(double ContentUnitAreaCost READ getContentUnitAreaCost WRITE setContentUnitAreaCost)
	Q_PROPERTY(QObject *Time READ getTime WRITE setTime)
	Q_PROPERTY(QObject *AnnualInterestRate READ getAnnualInterestRate WRITE setAnnualInterestRate)
	Q_PROPERTY(QObject *Epsilon READ getEpsilon WRITE setEpsilon)

	Q_PROPERTY(QObject *RepairStatus READ getRepairStatus WRITE setRepairStatus)

public:
	RBuildingRepairCostModel(QObject *parent, QString name);
	~RBuildingRepairCostModel();

	QObject *getBuildingInformationModel() const;
	void setBuildingInformationModel(QObject *value);
	QObject *getStructuralDamageRatio() const;
	void setStructuralDamageRatio(QObject *value);
	QObject *getNonstructuralDriftDamageRatio() const;
	void setNonstructuralDriftDamageRatio(QObject *value);
	QObject *getNonstructuralAccelerationDamageRatio() const;
	void setNonstructuralAccelerationDamageRatio(QObject *value);
	QObject *getContentDamageRatio() const;
	void setContentDamageRatio(QObject *value);
	double getStructuralUnitAreaCost() const;
	void setStructuralUnitAreaCost(double value);
	double getNonstructuralDriftUnitAreaCost() const;
	void setNonstructuralDriftUnitAreaCost(double value);
	double getNonstructuralAccelerationUnitAreaCost() const;
	void setNonstructuralAccelerationUnitAreaCost(double value);
	double getContentUnitAreaCost() const;
	void setContentUnitAreaCost(double value);
	QObject *getTime() const;
	void setTime(QObject *value);
	QObject *getAnnualInterestRate() const;
	void setAnnualInterestRate(QObject *value);
	QObject *getEpsilon() const;
	void setEpsilon(QObject *value);

	QObject *getRepairStatus() const;
	void setRepairStatus(QObject *value);

	double getRepairabilityStructuralDamageRatio() const;
	void setRepairabilityStructuralDamageRatio(double value);

	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID);
	void reset();

	// returns true as this model is parallelized
	bool canBeRunParallelly();

private:
	RPointer<RModel> theBuildingInformationModel;
	RPointer<RParameter> theStructuralDamageRatio;
	RPointer<RParameter> theNonstructuralDriftDamageRatio;
	RPointer<RParameter> theNonstructuralAccelerationDamageRatio;
	RPointer<RParameter> theContentDamageRatio;
	double theStructuralUnitAreaCost;
	double theNonstructuralDriftUnitAreaCost;
	double theNonstructuralAccelerationUnitAreaCost;
	double theContentUnitAreaCost;
	RPointer<RParameter> theTime;
	RPointer<RParameter> theAnnualInterestRate;
	RPointer<RParameter> theEpsilon;
	double theRepairabilityStructuralDamageRatio;

	RPointer<RParameter> theRepairStatus;

	double theInitialTime;

	RResponse *theRepairCost;

};

#endif // RBuildingRepairCostModel_H
