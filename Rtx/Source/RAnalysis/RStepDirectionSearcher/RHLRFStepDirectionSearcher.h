// Last modified by Mojtaba on June 30, 2012

#ifndef RHLRFStepDirectionSearcher_H
#define RHLRFStepDirectionSearcher_H

#include "RStepDirectionSearcher.h"

#include <gsl/gsl_vector.h>

class RHLRFStepDirectionSearcher : public RStepDirectionSearcher
{
	Q_OBJECT

public:
	RHLRFStepDirectionSearcher(QObject *parent, QString name);
	~RHLRFStepDirectionSearcher();

	// Computes the step direction in the search for design point in a nonlinear single-constraint optimization problem, used in FORM
	int computeStepDirection(int iterationIndex, gsl_vector const *u, double functionValue, gsl_vector const *gradientInStandardNormalSpace);

	// Returns the vector of step direction
	gsl_vector * getSearchDirection() const;
	bool canBeRunParallelly();

private:
	// Data member that stores the step direction vector
	gsl_vector *searchDirection;
	
};

#endif // RHLRFStepDirectionSearcher_H
