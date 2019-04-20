// Last modified by Mojtaba on June 30, 2012

#ifndef RFixedStepSizeSearcher_H
#define RFixedStepSizeSearcher_H

#include "RStepSizeSearcher.h"

#include <gsl/gsl_vector.h>

class RFixedStepSizeSearcher : public RStepSizeSearcher
{
	Q_OBJECT

	// Sets the fixed step size
	Q_PROPERTY(double StepSize READ getStepSize WRITE setStepSize)

public:
	RFixedStepSizeSearcher(QObject *parent, QString name);
	~RFixedStepSizeSearcher();

	// getter/setter methods for meta-properties
	double getStepSize() const;
	void setStepSize(double value);

	// Computes the step size in the search for design point in a nonlinear single-constraint optimization problem, used in FORM
	int computeStepSize(int iterationIndex, gsl_vector *u, RFunction *function, double passedFunctionValue, gsl_vector *functionGradient, gsl_vector const *stepDirection, QList<RRandomVariable *> *randomVariableList);

	// Returns the value the limit-state function, which is evaluated during the computation of the step size
	double getFunctionValue();

	// Returns the first trial step size
	double getInitialStepSize() const;
	bool canBeRunParallelly();

private:
	// Data member for meta-properties
	double theStepSize;
	
};

#endif // RFixedStepSizeSearcher_H
