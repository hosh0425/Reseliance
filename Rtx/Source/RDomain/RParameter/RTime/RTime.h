// Last modified by Mojtaba on June 30, 2012

#ifndef RTime_H
#define RTime_H

#include "RParameter.h"

#include <QDateTime>

class RTime : public RParameter
{
	Q_OBJECT

	// Current realization of the time in terms of days relative to the reference time
	Q_PROPERTY(double CurrentValue READ getCurrentValue WRITE setCurrentValue)

	// Current realization of time in the format of "yyyy-dd-MM hh:mm:ss.zzz"
	Q_PROPERTY(QString CurrentTime READ getCurrentTime WRITE setCurrentTime)

public:
	RTime(QObject *parent, QString name);
	~RTime();

	// Specific implementation of setCurrentValue for RTime that updates the CurrentTime upon setting the CurrentValue,
	// (DO NOT USE THIS METHOD IN PARALLEL COMPUTATION, use setCurrentValue instead)
	void setCurrentValue(double value);

	// thread specific implementation of setCurrentValue for RTime that updates the CurrentTime upon setting the CurrentValue, in the given threadID ($MTh$)
	void setCurrentValue(double value, int threadID);

	// getter/setter methods for meta-properties , (DO NOT USE THESE METHODS IN PARALLEL COMPUTATION
	QString getCurrentTime() const;
	void setCurrentTime(QString value);

	// Returns the reference time; Defalt is Jan. 1, 1900 at 12:00:00.000am
	QDateTime getReferenceTime() const;

	// Sets the refernce time to the passed value
	void setReferenceTime(QDateTime value);

	// Returns the year, in the given threadID ($MTh$)
	int getCurrentYear(int threadID) const;

	// Returns the month, in the given threadID ($MTh$)
	int getCurrentMonth(int threadID) const;

	// Returns the day, in the given threadID ($MTh$)
	int getCurrentDay(int threadID) const;

	// Returns the day of the week
	int getCurrentDayOfWeek(int threadID) const;

	// Returns the day
	int getCurrentHour(int threadID) const;

	// Returns the day
	int getCurrentMinute(int threadID) const;

	// Returns the format of the date and time, Currently, "yyyy-dd-MM hh:mm:ss.zzz" (zzz=milliseconds)
	static QString getTimeFormat();

	// Copies the value at index 0 to the index of the given threadID for data members that are changed to array due to parallelization. Each subclass must also call the method of their superclass
	void initializeForParallelComputation(int threadID);

	// returns true as this parameter is parallelized
	bool canBeRunParallelly();

private:
	// Auxiliary method that updates the CurrentTime when CurrentValue changes, in the given threadID ($MTh$)
	void updateCurrentTime(int threadID);

	// Auxiliary method that updates the CurrentValue when CurrentTime changes, in the given threadID ($MTh$)
	void updateCurrentValue(int threadID);

	// Data member for meta-properties that stores time in each threadID ($MTh$)
	QDateTime  *theCurrentTime;

	// Data member for the reference date and time
	QDateTime theReferenceTime;

	// Data member for the default value of the reference date and time; Equals Jan. 1, 1900 at 12:00:00.000am
	QDateTime constTime;

	// Data member that stores the format of the date and time; Equals "yyyy-dd-MM hh:mm:ss.zzz" (zzz=milliseconds)
	QString format;
	
};

#endif // RTime_H
