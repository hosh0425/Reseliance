// Last modified by Mojtaba on June 30, 2012

#ifndef RCorrelation_H
#define RCorrelation_H

#include "RParameter.h"

class RRandomVariable;

class RCorrelation : public RParameter
{
	Q_OBJECT

	// Sets one of the two random variable object that are being correlated
	Q_PROPERTY(QObject *RandomVariable1 READ getRandomVariable1 WRITE setRandomVariable1  USER false)

	// Sets the other one of the two random variable object that are being correlated
	Q_PROPERTY(QObject *RandomVariable2 READ getRandomVariable2 WRITE setRandomVariable2  USER false)

	// Sets the correlation coefficient between RandomVariable1 and RandomVariable2
	Q_PROPERTY(double Coefficient READ getCoefficient WRITE setCoefficient  USER false)	
	

public:
	RCorrelation(QObject *parent, QString name);
	~RCorrelation();

	// getter/setter methods for meta-properties , DO NOT USE SETTERS IN PARALLEL COMPUTATION AT ALL
	QObject *getRandomVariable1() const;
	void setRandomVariable1(QObject *value);
	QObject *getRandomVariable2() const;
	void setRandomVariable2(QObject *value);
	double getCoefficient() const;
	void setCoefficient(double value);

	// returns true as this parameter is parallelized
	bool canBeRunParallelly();

private:
	// Data member for meta-properties
	RPointer<RRandomVariable> theRandomVariable1;
	RPointer<RRandomVariable> theRandomVariable2;
	double theCoefficient;
	
};

#endif // RCorrelation_H
