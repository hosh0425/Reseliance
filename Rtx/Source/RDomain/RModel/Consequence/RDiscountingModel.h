#ifndef RDiscountingModel_H
#define RDiscountingModel_H

#include "RModel.h"

class RParameter;
class RResponse;

class RDiscountingModel : public RModel
{
	Q_OBJECT	
		
	Q_PROPERTY(QObject *FutureValue READ getFutureValue WRITE setFutureValue)
	Q_PROPERTY(QObject *Time READ getTime WRITE setTime)
	Q_PROPERTY(QObject *AnnualInterestRate READ getAnnualInterestRate WRITE setAnnualInterestRate)

public:
	RDiscountingModel(QObject *parent, QString name);
	~RDiscountingModel();

	QObject *getFutureValue() const;
	void setFutureValue(QObject *value);
	QObject *getTime() const;
	void setTime(QObject *value);
	QObject *getAnnualInterestRate() const;
	void setAnnualInterestRate(QObject *value);

	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID);
	void reset();

	// returns true as this model is parallelized
	bool canBeRunParallelly();

private:
	RPointer<RParameter> theFutureValue;
	RPointer<RParameter> theTime;
	RPointer<RParameter> theAnnualInterestRate;

	double theInitialTime;

	RResponse *thePresentValue;

};

#endif // RDiscountingModel_H
