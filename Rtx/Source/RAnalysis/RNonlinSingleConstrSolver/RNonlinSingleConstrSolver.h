// Last modified by Mojtaba on June 29, 2012

#ifndef RNonlinSingleConstrSolver_H
#define RNonlinSingleConstrSolver_H

#include "RObject.h"

#include <gsl/gsl_vector.h>

class RDomain;
class RFunction;

class RNonlinSingleConstrSolver : public RObject
{
	Q_OBJECT

	// Controls the amount of output that is shown in the output pane; options are None, Minimum, Moderate, and Maximum
	Q_PROPERTY(ROutputDisplayLevelType OutputDisplayLevel READ getOutputDisplayLevel WRITE setOutputDisplayLevel)

	// Indicates the type of the starting point for the FORM analysis; options are Mean, Origin, and CurrentValue
	Q_PROPERTY(RObject::RStartPointType StartPoint READ getStartPoint WRITE setStartPoint)

public:
	RNonlinSingleConstrSolver(QObject *parent, QString name);
	~RNonlinSingleConstrSolver();

	// getter/setter methods for meta-properties
	RObject::RStartPointType getStartPoint() const;
	void setStartPoint(RObject::RStartPointType value);

	// Solves the nonlienar single-constraint optimization problem, which leads to finding the design point
	virtual int findDesignPoint(RFunction *passedFunction, bool populateListsInFunction) = 0;

	// Returns the vector of rv realizations in the original space
	virtual gsl_vector * get_x() const = 0;

	// Returns the vector of rv realizations in the standard normal space
	virtual gsl_vector * get_u() const = 0;

	// Returns the vector of Alpha importance measure
	virtual gsl_vector * get_alpha() const = 0;

	// Returns the vector of Gamma importance measure
	virtual gsl_vector * get_gamma() const = 0;

	// Returns the distance from the design point to the origin, i.e., the reliability index
	virtual double getDistance() const = 0;

	// Returns the gradient vector of the limit-state function in the standard normal space
	virtual gsl_vector * getGradientInStandardNormalSpace() const = 0;

	// Returns the failure probability
	virtual double getFailureProbability() = 0;

	// Applies a correction on the failure probability (that is obtained by linearizing the limit-state surface at te design point) using the first principal curvature (similar to SORM)
	virtual int correctWithFirstPrincipalCurvature() = 0;

	// Returns the value of the first principal curvature
	virtual double getFirstPrincipalCurvature() = 0;

	// Computes and returns the sensitivity of the reliability index (as well as the first principal curvature) wrt. the mean and stdv of rvs (Development of the first principal curvature sensitivities are experimental)
	virtual int computeRandomVariableSensitivities(gsl_vector *betaSensitivityWrtMeanVector, gsl_vector *betaSensitivityWrtStdvVector, gsl_vector *kappaSensitivityWrtMeanVector = 0, gsl_vector *kappaSensitivityWrtStdvVector = 0) = 0;

protected:
	// Data members that are inherited by subclasses

	RDomain *theDomain;

	// Data members for meta-properties
	RObject::RStartPointType theStartPointType;

private:
	
};

#endif // RNonlinSingleConstrSolver_H
