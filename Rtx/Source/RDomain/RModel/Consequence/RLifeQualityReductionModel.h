#ifndef RLifeQualityReductionModel_H	
#define RLifeQualityReductionModel_H	

#include "RModel.h"	

class RParameter;
class RResponse;

class RLifeQualityReductionModel : public RModel
{
	Q_OBJECT

		Q_PROPERTY(QObject *Time READ getTime WRITE setTime)
		Q_PROPERTY(QString PopulationList READ getPopulationList WRITE setPopulationList)
		Q_PROPERTY(QString RepairOrRestorationStatusList READ getRepairOrRestorationStatusList WRITE setRepairOrRestorationStatusList)
		Q_PROPERTY(QString PowerServiceabilityList READ getPowerServiceabilityList WRITE setPowerServiceabilityList)
		Q_PROPERTY(QString WaterServiceabilityList READ getWaterServiceabilityList WRITE setWaterServiceabilityList)
		Q_PROPERTY(QObject *UnitCostOfHavingNoHouse READ getUnitCostOfHavingNoHouse WRITE setUnitCostOfHavingNoHouse)
		Q_PROPERTY(QObject *UnitCostOfHavingHouseWithoutPower READ getUnitCostOfHavingHouseWithoutPower WRITE setUnitCostOfHavingHouseWithoutPower)
		Q_PROPERTY(QObject *UnitCostOfHavingHouseWithoutWater READ getUnitCostOfHavingHouseWithoutWater WRITE setUnitCostOfHavingHouseWithoutWater)
		Q_PROPERTY(QObject *UnitCostOfHavingHouseWithoutPowerAndWater READ getUnitCostOfHavingHouseWithoutPowerAndWater WRITE setUnitCostOfHavingHouseWithoutPowerAndWater)

public:
	RLifeQualityReductionModel(QObject *parent, QString name);
	~RLifeQualityReductionModel();

	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID);

	QObject *getTime() const;
	void setTime(QObject *value);
	QString getPopulationList();
	void setPopulationList(QString value);
	QString getRepairOrRestorationStatusList();
	void setRepairOrRestorationStatusList(QString value);
	QString getPowerServiceabilityList();
	void setPowerServiceabilityList(QString value);
	QString getWaterServiceabilityList();
	void setWaterServiceabilityList(QString value);

	QObject *getUnitCostOfHavingNoHouse() const;
	void setUnitCostOfHavingNoHouse(QObject *value);
	QObject *getUnitCostOfHavingHouseWithoutPower() const;
	void setUnitCostOfHavingHouseWithoutPower(QObject *value);
	QObject *getUnitCostOfHavingHouseWithoutWater() const;
	void setUnitCostOfHavingHouseWithoutWater(QObject *value);
	QObject *getUnitCostOfHavingHouseWithoutPowerAndWater() const;
	void setUnitCostOfHavingHouseWithoutPowerAndWater(QObject *value);

	// returns true as this model is parallelized
	bool canBeRunParallelly();

	void reset();
	void resetHistoryVariables(int threadID);

private:

	RPointer<RParameter> theTime;
	QList<RPointer<RParameter>> thePopulationList;
	QList<RPointer<RParameter>> theRepairOrRestorationStatusList;
	QList<RPointer<RParameter>> thePowerServiceabilityList;
	QList<RPointer<RParameter>> theWaterServiceabilityList;

	RPointer<RParameter> theUnitCostOfHavingNoHouse;
	RPointer<RParameter> theUnitCostOfHavingHouseWithoutPower;
	RPointer<RParameter> theUnitCostOfHavingHouseWithoutWater;
	RPointer<RParameter> theUnitCostOfHavingHouseWithoutPowerAndWater;

	double theInitialTime;
	double *currentTime;
	double *previousTime;

	double *currentPopWithoutHouse;
	double *previousPopWithoutHouse;

	double *currentPopWithoutPower;
	double *previousPopWithoutPower;

	double *currentPopWithoutWater;
	double *previousPopWithoutWater;

	double *currentPopWithoutPowerAndWater;
	double *previousPopWithoutPowerAndWater;

	double *cumulativeLifeQualityReduction;


	// Declare responses
	RResponse *PopulationWithoutHouse;
	RResponse *PopulationWithoutOnlyPower;
	RResponse *PopulationWithoutOnlyWater;
	RResponse *PopulationWithoutPowerAndWater;
	RResponse *LifeQualityReductionCost;


};

#endif // RLifeQualityReductionModel_H	
