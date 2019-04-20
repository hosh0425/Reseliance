#ifndef RPopulationDensityModel_H	
#define RPopulationDensityModel_H	
	
#include "RModel.h"	
	
class RParameter;	
class RResponse;	
	
class RPopulationDensityModel : public RModel	
{	
	Q_OBJECT
	
	Q_PROPERTY(QObject *Time READ getTime WRITE setTime)
	Q_PROPERTY(QObject *MaximumPopulationDensity READ getMaximumPopulationDensity WRITE setMaximumPopulationDensity)
	Q_PROPERTY(QString PopulationRatiosPerMonth READ getPopulationRatiosPerMonth WRITE setPopulationRatiosPerMonth)
	Q_PROPERTY(QString PopulationRatiosPerWeekday READ getPopulationRatiosPerWeekday WRITE setPopulationRatiosPerWeekday)
	Q_PROPERTY(QString PopulationRatiosPerHour READ getPopulationRatiosPerHour WRITE setPopulationRatiosPerHour)
	Q_PROPERTY(QObject *TimeOffset READ getTimeOffset WRITE setTimeOffset)

public:	
	RPopulationDensityModel(QObject *parent, QString name);
	~RPopulationDensityModel();
	
	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int ThreadID);
	
	QObject *getTime() const;
	void setTime(QObject *value);
	QObject *getMaximumPopulationDensity() const;
	void setMaximumPopulationDensity(QObject *value);
	QString getPopulationRatiosPerMonth();
	void setPopulationRatiosPerMonth(QString value);
	QString getPopulationRatiosPerWeekday();
	void setPopulationRatiosPerWeekday(QString value);
	QString getPopulationRatiosPerHour();
	void setPopulationRatiosPerHour(QString value);

	QVector<double> getPopulationRatiosPerMonthVector() const;
	QVector<double> getPopulationRatiosPerWeekdayVector() const;
	QVector<double> getPopulationRatiosPerHourVector() const;
	
	QObject *getTimeOffset() const;
	void setTimeOffset(QObject *value);

	// returns true as this model is parallelized
	bool canBeRunParallelly();

private:	

	int RiFunction(double lq, double uq, double rl, double ru, double dr, double *count);

	RPointer<RParameter> theTime;
	RPointer<RParameter> theMaximumPopulationDensity;
	QVector<double> thePopulationRatiosPerMonthVector;
	QVector<double> thePopulationRatiosPerWeekdayVector;
	QVector<double> thePopulationRatiosPerHourVector;
	
	RPointer<RParameter> theTimeOffset;

	// Declare responses
	RResponse *thePopulationDensity;
	
};	
	
#endif // RPopulationDensityModel_H
