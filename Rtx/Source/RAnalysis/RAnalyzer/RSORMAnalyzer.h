// Last modified by Siavash on June 14, 2017

#ifndef RSORMAnalyzer_H
#define RSORMAnalyzer_H

#include "RAnalyzer.h"
#include <QDebug>

class QDebug;

#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>

class RDomain;
class RFunction;
class RTransformer;
class RNonlinSingleConstrSolver;

class RSORMAnalyzer : public RAnalyzer
{

	Q_OBJECT

		// Controls the amount of output that is shown in the output pane; options are None, Minimum, Moderate, and Maximum
		Q_PROPERTY(ROutputDisplayLevelType OutputDisplayLevel READ getOutputDisplayLevel WRITE setOutputDisplayLevel)

		// Sets the limit-state function
		Q_PROPERTY(QObject *LimitStateFunction READ getLimitStateFunction WRITE setLimitStateFunction)

		// Sets the "solver" for nonlinear single-constraint optimization problem in FORM
		Q_PROPERTY(QObject *NonlinearSingleConstraintSolver READ getNonlinearSingleConstraintSolver WRITE setNonlinearSingleConstraintSolver)

		// Sets the RTransformer object, which transforms points and gradients between original and standard normal spaces
		Q_PROPERTY(QObject *Transformer READ getTransformer WRITE setTransformer)

		Q_PROPERTY(RAnalysisMethod AnalysisMethod READ getAnalysisMethod WRITE setAnalysisMethod)

		Q_PROPERTY(RPointFittingAlgorithm PointFittingAlgorithm READ getPointFittingAlgorithm WRITE setPointFittingAlgorithm)

		Q_PROPERTY(int PointFittingSolverMaxIteration READ getMaxSolverIter WRITE setMaxSolverIter)

		Q_ENUMS(RPointFittingAlgorithm)
		Q_ENUMS(RAnalysisMethod)

public:
	RSORMAnalyzer(QObject *parent, QString name);
	~RSORMAnalyzer();

	// Declaring the RPointFittingAlgorithm enum values
	enum RPointFittingAlgorithm { Secant };
	enum RAnalysisMethod { PointFitting 
#ifdef HESSIAN_TRANSFORMATION
		, CurvatureFitting 
#endif 
	};

	// getter/setter methods for meta-properties
	QObject *getLimitStateFunction() const;
	void setLimitStateFunction(QObject *value);
	QObject *getNonlinearSingleConstraintSolver() const;
	void setNonlinearSingleConstraintSolver(QObject *value);
	QObject *getTransformer() const;
	void setTransformer(QObject *value);
	RPointFittingAlgorithm getPointFittingAlgorithm();
	void setPointFittingAlgorithm(RPointFittingAlgorithm value);
	int getMaxSolverIter();
	void setMaxSolverIter(int maxIter);
	RAnalysisMethod getAnalysisMethod();
	void setAnalysisMethod(RAnalysisMethod value);
	// Runs the analysis
	int analyze();

	// An over-loaded method to run the analysis with the option of not parsing (tokenizing) the limit-state function, assuming that it has already been parsed
	// This is only called from within the load coincidence analysis in which the function need not be re-parsed for every FORM analysis for diffrenet hazards
	int analyze(bool populateListsInFunction);

	// Applies a correction on the failure probability (that is obtained by linearizing the limit-state surface at te design point) using Curve Fitting SORM
	int correctWithCurvatureFittedSORM(RFunction *passedFunction, bool populateListsInFunction);

	// Applies a correction on the failure probability (that is obtained by linearizing the limit-state surface at te design point) using Point Fitting SORM
	int correctWithPointFittedSORM(RFunction *passedFunction, bool populateListsInFunction);

	// Applies a correction on the failure probability (that is obtained by linearizing the limit-state surface at te design point) using Efficient SORM

	// Under Development
	int correctWithEfficientSORM(RFunction *passedFunction, bool populateListsInFunction);

	// Transform Uprime to x and evaluate the LSF
	int Evaluate_Uprime_on_x(gsl_matrix * Rotation, gsl_vector * Uprimei, RFunction *theFunction, int randomVariablesCount);

	int computeHessian(bool populateListsInFunction, RFunction * theFunction, int randomVariablesCount, gsl_vector *ustar);

	// Gram-Schmidt implementation
	void Rotate(gsl_vector *vector, int n);
	gsl_matrix *getRotation();

	// Recursive method for determining the trial point for the lth principal axis:
	void Orthogonalize(gsl_vector *uvector, gsl_vector *ustar, gsl_matrix *deltamatrix, int n);

	gsl_vector *get_uj();

	bool canBeRunParallelly();

private:
	//ThreadID

	int threadID;
	// Data members for meta-properties
	QPointer<RFunction> theLimitStateFunction;
	QPointer<RNonlinSingleConstrSolver> theNonlinearSingleConstraintSolver;
	QPointer<RTransformer> theTransformer;
	RPointFittingAlgorithm thePointFittingAlgorithm;
	RAnalysisMethod theAnalysisMethod;
	int maxSolverIter;

	// Data members for compuring the failure probability
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

	// Garm-Schmidt rotation matrix
	gsl_matrix *gramrot;

	// Orthogonalized u
	gsl_vector *uj;

};

#endif // RSORMAnalyzer_H
