// Last modified by Mojtaba on June 29, 2012

#ifndef RStepperNonlinSingleConstrSolver_H
#define RStepperNonlinSingleConstrSolver_H

#include "RNonlinSingleConstrSolver.h"
#include "RPointer.h" //salarifard

#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>

class RFunction;
class RStepSizeSearcher;
class RStepDirectionSearcher;
class RTransformer;
class RConvergenceChecker;
class RRandomVariable;

class RStepperNonlinSingleConstrSolver : public RNonlinSingleConstrSolver
{
	Q_OBJECT

	// Sets the maximum number of trial steps
	Q_PROPERTY(int MaximumIterations READ getMaximumIterations WRITE setMaximumIterations USER true)

	// Sets the RStepSizeSearcher object, which is used to compute the step size
	Q_PROPERTY(QObject *StepSizeSearcher READ getStepSizeSearcher WRITE setStepSizeSearcher)

	// Sets the RStepDirectionSearcher object, which is used to compute the step direction
	Q_PROPERTY(QObject *StepDirectionSearcher READ getStepDirectionSearcher WRITE setStepDirectionSearcher)

	// Sets the RTransformer object, which transforms points and gradients between original and standard normal spaces
	Q_PROPERTY(QObject *Transformer READ getTransformer WRITE setTransformer)

	// Sets the RConvergenceChecker, which is used to check the convergence at each step
	Q_PROPERTY(QObject *ConvergenceChecker READ getConvergenceChecker WRITE setConvergenceChecker)


public:
	RStepperNonlinSingleConstrSolver(QObject *parent, QString name);
	~RStepperNonlinSingleConstrSolver();

	// getter/setter methods for meta-properties
	QObject *getStepSizeSearcher() const;
	void setStepSizeSearcher(QObject *value);
	QObject *getStepDirectionSearcher() const;
	void setStepDirectionSearcher(QObject *value);
	QObject *getTransformer() const;
	void setTransformer(QObject *value);
	QObject *getConvergenceChecker() const;
	void setConvergenceChecker(QObject *value);
	int getMaximumIterations() const;
	void setMaximumIterations(int value);
	bool canBeRunParallelly();


	// Solves the nonlienar single-constraint optimization problem, which leads to finding the design point
	int findDesignPoint(RFunction *passedFunction, bool populateListsInFunction);

	// Returns the vector of rv realizations in the original space
	gsl_vector * get_x() const;

	// Returns the vector of rv realizations in the standard normal space
	gsl_vector * get_u() const;

	// Returns the vector of Alpha importance measure
	gsl_vector * get_alpha() const;

	// Returns the vector of Gamma importance measure
	gsl_vector * get_gamma() const;

	// Returns the distance from the design point to the origin, i.e., the reliability index
	double getDistance() const;

	// Returns the gradient vector of the limit-state function in the standard normal space
	gsl_vector * getGradientInStandardNormalSpace() const;

	// Returns the failure probability
	double getFailureProbability();

	// Applies a correction on the failure probability (that is obtained by linearizing the limit-state surface at te design point) using the first principal curvature (similar to SORM)
	int correctWithFirstPrincipalCurvature();

	// Returns the value of the first principal curvature
	double getFirstPrincipalCurvature();

	// Computes and returns the sensitivity of the reliability index (as well as the first principal curvature) wrt. the mean and stdv of rvs (Development of the first principal curvature sensitivities are experimental)
	int computeRandomVariableSensitivities(gsl_vector *betaSensitivityWrtMeanVector, gsl_vector *betaSensitivityWrtStdvVector, gsl_vector *kappaSensitivityWrtMeanVector, gsl_vector *kappaSensitivityWrtStdvVector);

	// Returns the number of steps taken
	int getStepsCount() const;

	// Returns the penultimate trial realization of random variables in the standard normal space
	gsl_vector *getSecondLast_u() const;

	// Returns the penultimate realization of the alpha importance vector
	gsl_vector *getSecondLast_alpha() const;

	// Returns the last computed step direction
	gsl_vector *getLastStepDirection() const;

	// Returns the value of the function at its first evaluation
	double getFirstFunctionValue() const;

	// Returns the value of the function at its last evaluation
	double getLastFunctionValue() const;

private:
	// Auxiliary method to free the memory from all allocated matrices and vectors
	void deleteVectorsMatrices();

	// Data members for meta-properties
	RPointer<RStepSizeSearcher> theStepSizeSearcher;
	RPointer<RStepDirectionSearcher> theStepDirectionSearcher;
	RPointer<RTransformer> theTransformer;
	RPointer<RConvergenceChecker> theConvergenceChecker;
	int theMaximumIterations;

	// Data members for finding the design point and compuring the failure probability
	gsl_vector *x;
	gsl_vector *u;
	int iterationIndex;
	double functionValueFirst;
	double functionValueLast;
	gsl_vector *alpha;
	gsl_vector *gamma;
	gsl_vector *delta;
	gsl_vector *eta;
	gsl_vector *uSecondLast;
	gsl_vector *alphaSecondLast;
	gsl_vector *stepDirection;
	gsl_vector *gradientInStandardNormalSpace;
	double theDistance;
	double theFailureProbability;
	double theFirstPrincipalCurvature;
	
};

#endif // RStepperNonlinSingleConstrSolver_H
