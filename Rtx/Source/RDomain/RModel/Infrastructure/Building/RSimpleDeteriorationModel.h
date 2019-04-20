#ifndef RSimpleDeteriorationModel_H
#define RSimpleDeteriorationModel_H

#include "RModel.h"

class RParameter;
class RResponse;

class RSimpleDeteriorationModel : public RModel
{
	Q_OBJECT	
		
	//Q_PROPERTY(QObject *IntactValue READ getIntactValue WRITE setIntactValue)
	Q_PROPERTY(QObject *Time READ getTime WRITE setTime)
	Q_PROPERTY(QObject *DeteriorationRate READ getDeteriorationRate WRITE setDeteriorationRate)

public:
	RSimpleDeteriorationModel(QObject *parent, QString name);
	~RSimpleDeteriorationModel();

	//QObject *getIntactValue() const;
	//void setIntactValue(QObject *value);
	QObject *getTime() const;
	void setTime(QObject *value);
	QObject *getDeteriorationRate() const;
	void setDeteriorationRate(QObject *value);

	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID);
	void reset();

	// returns true as this model is parallelized
	bool canBeRunParallelly();

private:
	//RPointer<RParameter> theIntactValue;
	RPointer<RParameter> theTime;
	RPointer<RParameter> theDeteriorationRate;

	double theInitialTime;

	RResponse *theDeteriorationFactor;

};

#endif // RSimpleDeteriorationModel_H
