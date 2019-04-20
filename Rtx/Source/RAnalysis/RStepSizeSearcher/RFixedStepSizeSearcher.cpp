#include "RFixedStepSizeSearcher.h"

#include "RDomain.h"

RFixedStepSizeSearcher::RFixedStepSizeSearcher(QObject *parent, QString name)
	: RStepSizeSearcher(parent, name)
{
	theStepSize = 1;
}

RFixedStepSizeSearcher::~RFixedStepSizeSearcher()
{

}

double RFixedStepSizeSearcher::getStepSize() const
{
	return theStepSize;
}

void RFixedStepSizeSearcher::setStepSize(double value)
{
	theStepSize = value;
}

int RFixedStepSizeSearcher::computeStepSize(int iterationIndex, gsl_vector *u, RFunction *function, double functionValue, gsl_vector *functionGradient, gsl_vector const *stepDirection, QList<RRandomVariable *> *randomVariableList)
{
	// This method is in fact not neccesary for the fixed step size rule. The user has already given the step size.
	return 0;
}

double RFixedStepSizeSearcher::getFunctionValue()
{
	return 0;
}

double RFixedStepSizeSearcher::getInitialStepSize() const
{
	return 0;
}

bool RFixedStepSizeSearcher::canBeRunParallelly() {	
	return false;
}
