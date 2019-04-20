// Last modified by Mojtaba on June 29, 2012

#ifndef RMeritChecker_H
#define RMeritChecker_H

#include "RObject.h"

#include <gsl/gsl_vector.h>

class RDomain;

class RMeritChecker : public RObject
{
	Q_OBJECT

	// Controls the amount of output that is shown in the output pane; options are None, Minimum, Moderate, and Maximum
	Q_PROPERTY(ROutputDisplayLevelType OutputDisplayLevel READ getOutputDisplayLevel WRITE setOutputDisplayLevel)

public:
	RMeritChecker(QObject *parent, QString name);
	virtual ~RMeritChecker();

	// Checks the merit function to see if the current step size in FORM is acceptable
	virtual int check(gsl_vector *u, double functionValue, gsl_vector *functionGradient, double stepSize, const gsl_vector *stepDirection, double newFunctionValue) = 0;

	// Computes and returns the value of the merit function
	virtual double getMeritFunctionValue(gsl_vector *u, double functionValue, gsl_vector *functionGradient) = 0;

	// Updates the merit function parameters
	virtual int updateMeritParameters(gsl_vector *u, double functionValue, gsl_vector *functionGradient) = 0;

protected:
	// Data member that is inherited by subclasses
	RDomain *theDomain;

private:
	
};

#endif // RMeritChecker_H
