#ifndef RSimpleBuildingRepairCostModel_H
#define RSimpleBuildingRepairCostModel_H

#include "RModel.h"

class RParameter;
class RResponse;

class RSimpleBuildingRepairCostModel : public RModel
{
	Q_OBJECT

	// Define a QProperty for each input (parameter) data member. The type of the QProperty is "QObject *"
	Q_PROPERTY(QObject *DamageFactor READ getDamageFactor WRITE setDamageFactor)
	Q_PROPERTY(double NumberOfStories READ getNumberOfStories WRITE setNumberOfStories)
	Q_PROPERTY(double FootprintArea READ getFootprintArea WRITE setFootprintArea)
	Q_PROPERTY(QObject *UnitAreaCost READ getUnitAreaCost WRITE setUnitAreaCost)
	Q_PROPERTY(QObject *Epsilon READ getEpsilon WRITE setEpsilon)
	Q_PROPERTY(QObject *RepairStatus READ getRepairStatus WRITE setRepairStatus)


public:
	// Define the constructor and destructor
	RSimpleBuildingRepairCostModel(QObject *parent, QString name);
	~RSimpleBuildingRepairCostModel();

	// Define the mandatory read/write methods for all QProperties
	QObject *getDamageFactor() const;
	void setDamageFactor(QObject *value);
	double getFootprintArea() const;
	void setFootprintArea(double value);
	QObject *getUnitAreaCost() const;
	void setUnitAreaCost(QObject *value);
	double getNumberOfStories() const;
	void setNumberOfStories(double value);
	QObject *getEpsilon() const;
	void setEpsilon(QObject *value);
	QObject *getRepairStatus() const;
	void setRepairStatus(QObject *value);

	// Define the method where the actual model is coded
	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID);

	// returns true as this model is parallelized
	bool canBeRunParallelly();

private:
	// Declare input data members
	RPointer<RParameter> theDamageFactor;
	double theFootprintArea;
	RPointer<RParameter> theUnitAreaCost;
	double theNumberOfStories;
	RPointer<RParameter> theEpsilon;
	RPointer<RParameter> theRepairStatus;

	// Declare responses
	RResponse *theRepairCostResponse;

};

#endif // RSimpleBuildingRepairCostModel_H
