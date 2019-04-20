#ifndef RRelocationCostModel_H	
#define RRelocationCostModel_H	

#include "RModel.h"	

class RParameter;
class RResponse;

class RRelocationCostModel : public RModel
{
	Q_OBJECT

		Q_PROPERTY(QObject *Time READ getTime WRITE setTime)
		Q_PROPERTY(QString PopulationList READ getPopulationList WRITE setPopulationList)
		Q_PROPERTY(QString RepairOrRestorationStatusList READ getRepairOrRestorationStatusList WRITE setRepairOrRestorationStatusList)
		Q_PROPERTY(QObject *RelocationCostPerDayPerPerson READ getRelocationCostPerDayPerPerson WRITE setRelocationCostPerDayPerPerson)

public:
	RRelocationCostModel(QObject *parent, QString name);
	~RRelocationCostModel();

	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID);

	QObject *getTime() const;
	void setTime(QObject *value);
	QString getPopulationList();
	void setPopulationList(QString value);
	QString getRepairOrRestorationStatusList();
	void setRepairOrRestorationStatusList(QString value);
	QObject *getRelocationCostPerDayPerPerson() const;
	void setRelocationCostPerDayPerPerson(QObject *value);

	// returns true as this model is parallelized
	bool canBeRunParallelly();

	void reset();
	void resetHistoryVariables(int threadID);

private:

	RPointer<RParameter> theTime;
	QList<RPointer<RParameter>> thePopulationList;
	QList<RPointer<RParameter>> theRepairOrRestorationStatusList;
	RPointer<RParameter> theRelocationCostPerDayPerPerson;

	double theInitialTime;
	double *currentTime;
	double *previousTime;
	
	double *currentDemand;
	double *previousDemand;
	double *cumulativeRelocationCost;


	// Declare responses
	RResponse *theRelocationDemand;
	RResponse *theRelocationCost;


};

#endif // RRelocationCostModel_H	
