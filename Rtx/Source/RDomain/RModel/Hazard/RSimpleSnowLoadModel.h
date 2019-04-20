#ifndef RSimpleSnowLoadModel_H
#define RSimpleSnowLoadModel_H

#include "RModel.h"

class RParameter;
class RResponse;

class RSimpleSnowLoadModel : public RModel
{
	Q_OBJECT

	Q_PROPERTY(QObject *MaximumDepth READ getMaximumDepth WRITE setMaximumDepth)
	Q_PROPERTY(int StartMonth READ getStartMonth WRITE setStartMonth)
	Q_PROPERTY(int EndMonth READ getEndMonth WRITE setEndMonth)
	Q_PROPERTY(QObject *Density READ getDensity WRITE setDensity)
	Q_PROPERTY(QObject *Time READ getTime WRITE setTime)
	Q_PROPERTY(QObject *Epsilon READ getEpsilon WRITE setEpsilon)

public:
	// Define the constructor and destructor
	RSimpleSnowLoadModel(QObject *parent, QString name);
	~RSimpleSnowLoadModel();

	// Define the mandatory read/write methods for all QProperties
	QObject *getMaximumDepth() const;
	void setMaximumDepth(QObject *value);
	double getStartMonth() const;
	void setStartMonth(double value);
	double getEndMonth() const;
	void setEndMonth(double value);
	QObject *getDensity() const;
	void setDensity(QObject *value);
	QObject *getTime() const;
	void setTime(QObject *value);
	QObject *getEpsilon() const;
	void setEpsilon(QObject *value);

	// Define the method where the actual model is coded
	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID);

	// returns true as this model is parallelized
	bool canBeRunParallelly();

private:

	RPointer<RParameter> theMaximumDepth;
	double theStartMonth;
	double theEndMonth;
	RPointer<RParameter> theDensity;
	RPointer<RParameter> theTime;
	RPointer<RParameter> theEpsilon;
	
	// Declare responses
	RResponse *theSnowLoad;
	RResponse *theSnowDepth;

};

#endif // RSimpleSnowLoadModel_H
