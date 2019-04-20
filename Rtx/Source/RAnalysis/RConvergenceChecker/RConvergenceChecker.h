// Last modified by Mojtaba on June 29, 2012

#ifndef RConvergenceChecker_H
#define RConvergenceChecker_H

#include "RObject.h"

#include <gsl/gsl_vector.h>

class RDomain;

class RConvergenceChecker : public RObject
{
	Q_OBJECT

	// Controls the amount of output that is shown in the output pane; options are None, Minimum, Moderate, and Maximum
	Q_PROPERTY(ROutputDisplayLevelType OutputDisplayLevel READ getOutputDisplayLevel WRITE setOutputDisplayLevel)

public:
	RConvergenceChecker(QObject *parent, QString name);
	virtual ~RConvergenceChecker();

	// Checks if the FORM analysis has indeed convereged 
	virtual int check(gsl_vector const *u, double functionValue, gsl_vector const *gradientInStandardNormalSpace) = 0;

	// Returns the number of convergence criteria
	virtual int getNumberOfCriteria() = 0;

	// Returns the value of each convergence criterion
	virtual double getCriteriaValue(int whichCriteria) = 0;

	// Returns the value that is used to scale the limit-state function value in each step of FORM
	virtual double getScaleValue() = 0;

	// Sets the value that is used to scale the limit-state function value in each step of FORM
	virtual void setScaleValue(double value) = 0;

protected:
	// Data member that is inherited by subclasses
	RDomain *theDomain;

private:
	
};

#endif // RConvergenceChecker_H
