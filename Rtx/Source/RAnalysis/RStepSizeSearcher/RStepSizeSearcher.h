// Last modified by Mojtaba on June 30, 2012

#ifndef RStepSizeSearcher_H
#define RStepSizeSearcher_H

#include "RObject.h"

#include <gsl/gsl_vector.h>

class RDomain;
class RFunction;
class RRandomVariable;

class RStepSizeSearcher : public RObject
{
	Q_OBJECT

	// Controls the amount of output that is shown in the output pane; options are None, Minimum, Moderate, and Maximum
	Q_PROPERTY(ROutputDisplayLevelType OutputDisplayLevel READ getOutputDisplayLevel WRITE setOutputDisplayLevel)

public:
	RStepSizeSearcher(QObject *parent, QString name);
	virtual ~RStepSizeSearcher();

	// Computes the step size in the search for design point in a nonlinear single-constraint optimization problem, used in FORM
	virtual int computeStepSize(int iterationIndex, gsl_vector *u, RFunction *function, double functionValue, gsl_vector *functionGradient, gsl_vector const *stepDirection, QList<RRandomVariable *> *randomVariableList) = 0;
	
	// Returns the value the limit-state function, which is evaluated during the computation of the step size
	virtual double getFunctionValue() = 0;

	// Returns the computed step size
	virtual double getStepSize() const = 0;

	// Returns the first trial step size
	virtual double getInitialStepSize() const = 0;

protected:
	// Data members that are inherited by subclasses
	RDomain *theDomain;

private:
	
};

#endif // RStepSizeSearcher_H
