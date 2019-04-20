// Last modified by Mojtaba on June 29, 2012

#ifndef RStandardConvergenceChecker_H
#define RStandardConvergenceChecker_H

#include "RConvergenceChecker.h"

#include <gsl/gsl_vector.h>

class RStandardConvergenceChecker : public RConvergenceChecker
{
	Q_OBJECT

	// Sets the first convergence criterion, which deals with the limit-state function value being zero
	Q_PROPERTY(double E1 READ getE1 WRITE setE1)

	// Sets the second convergence criterion, which deals with the direction of the gradient of the limit-state function passing through origin in the standard normal space
	Q_PROPERTY(double E2 READ getE2 WRITE setE2)

public:
	RStandardConvergenceChecker(QObject *parent, QString name);
	~RStandardConvergenceChecker();

	// getter/setter methods for meta-properties
	double getE1();
	void setE1(double value);
	double getE2();
	void setE2(double value);

	// Checks if the FORM analysis has indeed convereged 
	int check(gsl_vector const *u, double functionValue, gsl_vector const *gradientInStandardNormalSpace);

	// Returns the number of convergence criteria, which is 2 for this sub-class
	int getNumberOfCriteria();

	// Returns the value of each convergence criterion
	double getCriteriaValue(int whichCriteria);

	// Returns the value that is used to scale the limit-state function value in each step of FORM
	double getScaleValue();

	// Sets the value that is used to scale the limit-state function value in each step of FORM
	void setScaleValue(double value);
	bool canBeRunParallelly();

private:
	// Data members for meta-properties
	double theE1;
	double theE2;

	// Data members for checking the convergence of FORM
	double theScaleValue;
	double criterium1;
	double criterium2;
	
};

#endif // RStandardConvergenceChecker_H
