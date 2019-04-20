// Last modified by Mojtaba on June 30, 2012

#ifndef RRandomVariable_H
#define RRandomVariable_H

#include "RParameter.h"

class RRandomVariable : public RParameter
{
	Q_OBJECT

	// Current realization of the random variable
	Q_PROPERTY(double CurrentValue READ getCurrentValue WRITE setCurrentValue  USER false)

public:
	RRandomVariable(QObject *parent, QString name);
	virtual ~RRandomVariable();

	// Returns the mean value of the random variable as a double, in the given threadID ($MTh$)
	virtual double getMeanValue(int threadID) = 0;

	// Returns the stdv of the random variable as a double, in the given threadID ($MTh$)
	virtual double getStandardDeviationValue(int threadID) = 0;

	// Returns the PDF at a given realization of the random variable, in the given threadID ($MTh$)
	virtual double getPDFValue(double rvValue, int threadID) = 0;

	// Returns the CDF at a given realization of the random variable, in the given threadID ($MTh$)
	virtual double getCDFValue(double rvValue, int threadID) = 0;

	// Returns the inverse CDF at a given probability value, in the given threadID ($MTh$)
	virtual double getInverseCDFValue(double probValue, int threadID) = 0;
	

	// Returns a list of parameters that are input to the random variable as mean, stdv, etc.
	virtual QList<RParameter *> getParameterList() = 0;
	
private:

};

#endif
