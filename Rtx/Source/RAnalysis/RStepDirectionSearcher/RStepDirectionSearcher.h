// Last modified by Mojtaba on June 30, 2012

#ifndef RStepDirectionSearcher_H
#define RStepDirectionSearcher_H

#include "RObject.h"

#include <gsl/gsl_vector.h>

class RDomain;

class RStepDirectionSearcher : public RObject
{
	Q_OBJECT

public:
	RStepDirectionSearcher(QObject *parent, QString name);
	virtual ~RStepDirectionSearcher();

	// Computes the step direction in the search for design point in a nonlinear single-constraint optimization problem, used in FORM
	virtual int computeStepDirection(int iterationIndex, gsl_vector const *u, double functionValue, gsl_vector const *gradientInStandardNormalSpace) = 0;

	// Returns the vector of step direction
	virtual gsl_vector * getSearchDirection() const = 0;

protected:
	// Data members that are inherited by subclasses

	RDomain *theDomain;

private:
	
};

#endif // RStepDirectionSearcher_H
