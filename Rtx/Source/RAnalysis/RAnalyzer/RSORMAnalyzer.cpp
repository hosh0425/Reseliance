// Last modified by Siavash on February 15, 2017

#include "RSORMAnalyzer.h"

#include <QMap>
#include <QCoreApplication>
#include <QApplication>
#include <QDateTime>

#include "RMainWindow.h"
#include "RDomain.h"
#include "RFunction.h"
#include "RNonlinSingleConstrSolver.h"
#include "RStepperNonlinSingleConstrSolver.h"
#include "RStepSizeSearcher.h"
#include "RStepDirectionSearcher.h"
#include "RConvergenceChecker.h"
#include "RTransformer.h"
#include "RContinuousDecisionVariable.h"
#include "RContinuousRandomVariable.h"
#include "RNatafTransformer.h"
#include "RModel.h"
#include "RResponse.h"
#include "RCorrelation.h"

#include <gsl/gsl_blas.h>
#include <gsl/gsl_cdf.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_eigen.h>



RSORMAnalyzer::RSORMAnalyzer(QObject *parent, QString name)
	: RAnalyzer(parent, name)
{
	theLimitStateFunction = 0;
	theTransformer = 0;

	theNonlinearSingleConstraintSolver = 0;
	theAnalysisMethod = RAnalysisMethod::PointFitting;
	thePointFittingAlgorithm = RPointFittingAlgorithm::Secant;

	theTransformer = 0;

	x = 0;
	u = 0;
	alpha = 0;
	gamma = 0;
	delta = 0;
	eta = 0;
	uSecondLast = 0;
	alphaSecondLast = 0;
	stepDirection = 0;
	gradientInStandardNormalSpace = 0;
	theDistance = 0;
	theFailureProbability = 0;
	gramrot = 0;
	uj = 0;
	threadID = 0;
	maxSolverIter = 20;
}

RSORMAnalyzer::~RSORMAnalyzer()
{

	if (gradientInStandardNormalSpace) {
		gsl_vector_free(gradientInStandardNormalSpace);
		gradientInStandardNormalSpace = 0;
	}
	if (alpha) {
		gsl_vector_free(alpha);
		alpha = 0;
	}
	if (gamma) {
		gsl_vector_free(gamma);
		gamma = 0;
	}
	if (delta) {
		gsl_vector_free(delta);
		delta = 0;
	}
	if (eta) {
		gsl_vector_free(eta);
		eta = 0;
	}
	if (alphaSecondLast) {
		gsl_vector_free(alphaSecondLast);
		alphaSecondLast = 0;
	}
	if (uSecondLast) {
		gsl_vector_free(uSecondLast);
		uSecondLast = 0;
	}
	if (gramrot)
	{
		gsl_matrix_free(gramrot);
	}
	if (uj)
	{
		gsl_vector_free(uj);
	}
	stepDirection = 0;
	iterationIndex = 0;
	functionValueFirst = 0;
	functionValueLast = 0;
	theDistance = 0;
}

QObject * RSORMAnalyzer::getLimitStateFunction() const
{
	return theLimitStateFunction;
}

void RSORMAnalyzer::setLimitStateFunction(QObject *value)
{
	theLimitStateFunction = qobject_cast<RFunction *>(value);
}

QObject * RSORMAnalyzer::getTransformer() const
{
	return theTransformer;
}

void RSORMAnalyzer::setTransformer(QObject *value)
{
	theTransformer = qobject_cast<RTransformer *>(value);
}


QObject * RSORMAnalyzer::getNonlinearSingleConstraintSolver() const
{
	return theNonlinearSingleConstraintSolver;
}

void RSORMAnalyzer::setNonlinearSingleConstraintSolver(QObject *value)
{
	theNonlinearSingleConstraintSolver = qobject_cast<RNonlinSingleConstrSolver *>(value);
}

RSORMAnalyzer::RAnalysisMethod RSORMAnalyzer::getAnalysisMethod()
{
	return theAnalysisMethod;
}

void RSORMAnalyzer::setAnalysisMethod(RSORMAnalyzer::RAnalysisMethod value)
{
	theAnalysisMethod = value;
}


RSORMAnalyzer::RPointFittingAlgorithm RSORMAnalyzer::getPointFittingAlgorithm()
{
	return thePointFittingAlgorithm;
}

void RSORMAnalyzer::setPointFittingAlgorithm(RSORMAnalyzer::RPointFittingAlgorithm value)
{
	thePointFittingAlgorithm = value;
}

int RSORMAnalyzer::getMaxSolverIter()
{
	return maxSolverIter;
}

void RSORMAnalyzer::setMaxSolverIter(int maxIter)
{
	maxSolverIter = maxIter;
}

int RSORMAnalyzer::analyze()
{
	int result = this->analyze(true);
	if (result < 0) {
		return result;
	}
}

int RSORMAnalyzer::analyze(bool populateListsInFunction)
{
	// Inform the user that the analysis has started
	rDebug(threadID) << "##### STARTING FORM ANALYSIS WITH FUNCTION" << theLimitStateFunction->objectName() << "...";
	rDebug(threadID) << "";
	fprintf(stderr, "##### STARTING FORM ANALYSIS WITH FUNCTION");
	QTextStream(stdout) << "string to print" << endl;
	QDateTime startTime = QDateTime::currentDateTime();


	if (!theLimitStateFunction) {
		rDebug(threadID) << "Error in" << objectName() << ": Please choose a function and try again";
		return -1;
	}

	// Checking if all analysis tools are provided
	int result = checkProperties();
	if (result < 0) {
		rDebug(threadID) << "Error in" << objectName() << ": Could find one or more necessary tools";
		return -1;
	}

	if ((qobject_cast<QApplication*>(QCoreApplication::instance()) != 0)) {
		theMainWindow->raiseDock("Output");
	}

	result = theNonlinearSingleConstraintSolver->findDesignPoint(theLimitStateFunction, populateListsInFunction);
	if (result < 0) {
		return result;
	}

	double beta = theNonlinearSingleConstraintSolver->getDistance();
	double pf = theNonlinearSingleConstraintSolver->getFailureProbability();

	// If the user has chosen to correct the results using the first principal curvature
	double betaSORM;
	double pfSORM;
	double firstPrincipalCurvature;
	theFailureProbability = pf;
	theDistance = beta;
	x = theNonlinearSingleConstraintSolver->get_x();
	u = theNonlinearSingleConstraintSolver->get_u();

#ifdef HESSIAN_TRANSFORMATION
	// If the user has chosen to correct the results using the curve fitted SORM
	if (theAnalysisMethod == RAnalysisMethod::CurvatureFitting) {

		// Curve Fitted SORM:
		correctWithCurvatureFittedSORM(theLimitStateFunction, populateListsInFunction);/**/

		pfSORM = theFailureProbability;

		// Compute the corresponding beta
		betaSORM = -gsl_cdf_gaussian_Pinv(pfSORM, 1.0);
	}
#endif

	// If the user has chosen to correct the results using the point fitted SORM
	if (theAnalysisMethod == RAnalysisMethod::PointFitting) {

		// Point Fitted SORM:
		correctWithPointFittedSORM(theLimitStateFunction, populateListsInFunction);/**/

		pfSORM = theFailureProbability;

		// Compute the corresponding beta
		betaSORM = -gsl_cdf_gaussian_Pinv(pfSORM, 1.0);
	}


	rDebug(threadID) << "";
	rDebug(threadID) << "The reliability index:";
	rDebug(threadID) << "Beta =" << beta;
	rDebug(threadID) << "";
	rDebug(threadID) << "The probability of failure:";
	rDebug(threadID) << "pf =" << pf;
	rDebug(threadID) << "";
	rDebug(threadID) << "";

#ifdef HESSIAN_TRANSFORMATION
	if (theAnalysisMethod == RAnalysisMethod::CurvatureFitting) {
		rDebug(threadID) << "the Hessian matrix at the design point in FORM is employed to compute the curvature of the limit-state function.";
		rDebug(threadID) << "";
		rDebug(threadID) << "Corrected probability of failure according to Breitung's improved formula:";
		rDebug(threadID) << "pf =" << pfSORM;
		rDebug(threadID) << "";
		rDebug(threadID) << "Corresponding reliability index:";
		rDebug(threadID) << "Beta =" << betaSORM;
		rDebug(threadID) << "";
	}
#endif

	if (theAnalysisMethod == RAnalysisMethod::PointFitting) {
		rDebug(threadID) << "2*(n-1) points deviated from the design point in FORM are employed to compute the modified probability values.";
		rDebug(threadID) << "";
		rDebug(threadID) << "Corrected probability of failure according to Breitung's improved formula:";
		rDebug(threadID) << "pf =" << pfSORM;
		rDebug(threadID) << "";
		rDebug(threadID) << "Corresponding reliability index:";
		rDebug(threadID) << "Beta =" << betaSORM;
		rDebug(threadID) << "";
	}

	QDateTime finishTime = QDateTime::currentDateTime();
	double numDays = startTime.daysTo(finishTime);
	double numMilliSecs = startTime.time().msecsTo(finishTime.time());
	double numSecs = numDays * 86400.0 + numMilliSecs / 1000.0;

	rDebug(threadID) << "##### SORM ANALYSIS DONE IN" << numSecs << "SECONDS.";
	rDebug(threadID) << "";
	rDebug(threadID) << "";

	return 0;
}

int RSORMAnalyzer::correctWithCurvatureFittedSORM(RFunction *passedFunction, bool populateListsInFunction)
{
#ifdef HESSIAN_TRANSFORMATION
	// Determine the number of random variables
	int randomVariablesCount = x->size;
	QVector<double> curvatureVector;

	int result = 0;
	gsl_matrix *functionHessian = gsl_matrix_calloc(randomVariablesCount, randomVariablesCount);
	double functionValueOld = 1.0;
	double functionValue = 1.0;
	gsl_vector *gradientInStandardNormalSpaceOld = gsl_vector_calloc(randomVariablesCount);
	gradientInStandardNormalSpace = gsl_vector_calloc(randomVariablesCount);
	alpha = gsl_vector_calloc(randomVariablesCount);
	gsl_vector *uOld = gsl_vector_calloc(randomVariablesCount);
	gsl_vector *functionGradient;
	double gradientInStandardNormalSpaceNorm = 0;

	// Implement the passed function
	RFunction *theFunction = passedFunction;

	// Get the design point
	gsl_vector *ustar = gsl_vector_calloc(randomVariablesCount);
	gsl_vector_memcpy(ustar, u);// Store design point in ustar

								// Compute the gradient of G
								// Gradient in original space
	result = theFunction->evaluateGradient(RObject::RandomVariable);
	// Attention: "functionGradient" should already be allocated in "theFunction", here just the memory address is obtained.
	functionGradient = theFunction->getGradient();
	if (result < 0) {
		rDebug(threadID) << "Error: Could not evaluate the gradient of the function.";
		return -1;
	}
	if (getOutputDisplayLevel() >= RObject::Maximum) {
		rDebug(threadID) << "Gradient in original space:";
		theDomain->print(functionGradient, threadID);
		rDebug(threadID) << "";
	}

	// Check if all components of the vector is zero
	double functionGradientNorm = gsl_blas_dnrm2(functionGradient);
	if (functionGradientNorm == 0) {
		rDebug(threadID) << "Error: All components of the gradient vector are zero.";
		return -1;
	}

	gsl_vector_memcpy(gradientInStandardNormalSpaceOld, gradientInStandardNormalSpace);
	// Gradient in standard normal space
	result = theTransformer->transformGradientToStandardNormalSpace(x, u, functionGradient, gradientInStandardNormalSpace, threadID);
	if (result < 0) {
		rDebug(threadID) << "Error: Could not transform the gradient to standard-normal space.";
		return -1;
	}

	if (getOutputDisplayLevel() >= RObject::Maximum) {
		rDebug(threadID) << "Gradient in standard-normal space:";
		theDomain->print(gradientInStandardNormalSpace, threadID);
		rDebug(threadID) << "";
	}

	// Compute the norm of the gradient in standard normal space
	gradientInStandardNormalSpaceNorm = gsl_blas_dnrm2(gradientInStandardNormalSpace);

	// Check that the norm is not zero
	if (gradientInStandardNormalSpaceNorm == 0.0) {
		rDebug(threadID) << "Error: The norm of the gradient in standard-normal space is zero.";
		return -1;
	}

	// Compute alpha-vector using BLAS: alpha = gradientInStandardNormalSpace * (-1 / gradientInStandardNormalSpaceNorm);
	gsl_vector_memcpy(alpha, gradientInStandardNormalSpace);
	gsl_vector_scale(alpha, -1.0 / gradientInStandardNormalSpaceNorm);

	// Compute corrected correlation coefficients

	// Compute the rotation matrix
	// Rotate by Gram-Schmidt process
	Rotate(alpha, randomVariablesCount);

	// Obtain Gram-Schmidt matrix
	gsl_matrix *Rotation = getRotation();

	// Compute the Hessian matrix
	computeHessian(populateListsInFunction, theFunction, randomVariablesCount, ustar);
	//functionHessian = theFunction->getHessian();
	gsl_matrix_memcpy(functionHessian, theFunction->getHessian());


	// A = R*hess_G*R' / norm(grad_G)
	gsl_matrix *A = gsl_matrix_calloc(randomVariablesCount, randomVariablesCount);
	gsl_matrix *B = gsl_matrix_calloc(randomVariablesCount, randomVariablesCount);

	gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, Rotation, functionHessian, 0.0, B);
	gsl_blas_dgemm(CblasNoTrans, CblasTrans, 1.0 / gradientInStandardNormalSpaceNorm, B, Rotation, 0.0, A);

	if (getOutputDisplayLevel() >= RObject::Minimum) {
		rDebug(threadID) << "";
		rDebug(threadID) << "The Rotation Matrix:";
		rDebug(threadID) << "";
		theDomain->print(Rotation, threadID);
		rDebug(threadID) << "";
		rDebug(threadID) << "The Hessian Matrix:";
		rDebug(threadID) << "";
		theDomain->print(functionHessian, threadID);
		rDebug(threadID) << "";
	}

	if (randomVariablesCount > 1)
	{
		// Store the sub-matrix of A
		//gsl_matrix_view subA = gsl_matrix_submatrix(A, 0, 0, randomVariablesCount - 2, randomVariablesCount - 2);
		gsl_matrix *subA = gsl_matrix_calloc(randomVariablesCount - 1, randomVariablesCount - 1);

		for (int i = 0; i < randomVariablesCount - 1; i++)
		{
			for (int j = 0; j < randomVariablesCount - 1; j++)
			{
				gsl_matrix_set(subA, i, j, gsl_matrix_get(A, i, j));
			}

		}

		// Compute and store eigenvalue/vectors
		// Allocate eigenvalue vector and eigenvector matrix
		gsl_vector *eval = gsl_vector_alloc(randomVariablesCount - 1);
		gsl_matrix *evec = gsl_matrix_alloc(randomVariablesCount - 1, randomVariablesCount - 1);

		// Allocate the eigenvalue workspace
		gsl_eigen_symmv_workspace * w = gsl_eigen_symmv_alloc(randomVariablesCount - 1);

		// Calculate store eigenvalue/vectors
		gsl_eigen_symmv(subA, eval, evec, w);


		if (getOutputDisplayLevel() >= RObject::Minimum) {
			rDebug(threadID) << "The Curvature Vector:";
			rDebug(threadID) << "";
			theDomain->print(eval, threadID);
			rDebug(threadID) << "";
		}

		// Compute the corrected failure probabilities
		for (int i = 0; i < randomVariablesCount - 1; i++)
		{
			curvatureVector << gsl_vector_get(eval, i);
		}

		gsl_eigen_symmv_free(w);

		gsl_matrix_free(evec);
		gsl_matrix_free(subA);

		gsl_vector_free(eval);
	}
	else
	{
		rDebug(threadID) << "Error: SORM analysis did not find sufficient degrees of freedom.";
		return -1;
		//curvatureVector << gsl_matrix_get(A, 0, 0);
		//theFirstPrincipalCurvature = gsl_matrix_get(A, 0, 0);
	}

	// Correct the pf of FORM using the Curvature/Kappa Values
	double denominator = gsl_cdf_gaussian_P(-theDistance, 1.0);
	if (denominator == 0.0) {
		rDebug(threadID) << "Error: SORM analysis did not converge due to too a large reliability index value.";
		return -1;
	}
	double psi_beta = gsl_ran_gaussian_pdf(theDistance, 1.0) / denominator;

	double product = 1.0;
	for (int i = 0; i < curvatureVector.count(); i++) {

		if (curvatureVector[i] * psi_beta < -1) {
			rDebug(threadID) << "Warning: The first principal curvature is negative and smaller than the negative inverse of the reliability index. Hence, the failure probability from FORM cannot be corrected with this curvature.";
			return -1;
		}
		product /= sqrt(1.0 + psi_beta * curvatureVector[i]);
	}
	double pfSORM = theFailureProbability * product;

	// Assign the corrected values
	theFailureProbability = pfSORM;

	//	gsl_matrix_free(functionHessian);
	gsl_matrix_free(A);
	gsl_matrix_free(B);

	gsl_vector_free(ustar);
	gsl_vector_free(uOld);
#endif

	return 0;
}

int RSORMAnalyzer::correctWithPointFittedSORM(RFunction *passedFunction, bool populateListsInFunction)
{
	//////////////////////////////////////////////////////////////////////////////////////l
	// This function computes the fitting points :										//l
	// -the parameter k which define the path is needed									//l
	// -the matrix Uprime1 defining the starting points on each axis is also needed		//l
	// -Rotation is the rotation matrix													//l
	//////////////////////////////////////////////////////////////////////////////////////l


	// Determine number of random variables
	int randomVariablesCount = x->size;
	double k;
	double threshold = 1e-3;
	double a, b, dadb = 0.0;

	int result = 0;
	gsl_matrix *functionHessian;
	double functionValueOld = 1.0;
	double functionValue = 1.0;
	gsl_vector *gradientInStandardNormalSpaceOld = gsl_vector_calloc(randomVariablesCount);
	gsl_vector *gradG = gsl_vector_calloc(randomVariablesCount);
	gradientInStandardNormalSpace = gsl_vector_calloc(randomVariablesCount);
	alpha = gsl_vector_calloc(randomVariablesCount);
	gsl_vector *uOld = gsl_vector_calloc(randomVariablesCount);
	gsl_vector *functionGradient;
	gsl_vector *vect = gsl_vector_calloc(randomVariablesCount); // Useful to compute the scalar product in the computation of b
	gsl_vector *temp = gsl_vector_calloc(randomVariablesCount);

	double gradientInStandardNormalSpaceNorm = 0;

	// Implement the passed function
	RFunction *theFunction = passedFunction;

	QList<RRandomVariable *> *randomVariableList = theFunction->getRandomVariableList();

	// Get the design point
	gsl_vector *ustar = gsl_vector_calloc(randomVariablesCount);
	gsl_vector_memcpy(ustar, u);// Store design point in ustar

								// Compute the gradient of G
								// Gradient in original space
	result = theFunction->evaluateGradient(RObject::RandomVariable);
	// Attention: "functionGradient" should already be allocated in "theFunction", here just the memory address is obtained.
	functionGradient = theFunction->getGradient();
	if (result < 0) {
		rDebug(threadID) << "Error: Could not evaluate the gradient of the function.";
		return -1;
	}

	if (getOutputDisplayLevel() >= RObject::Maximum) {
		rDebug(threadID) << "Gradient in original space:";
		theDomain->print(functionGradient, threadID);
		rDebug(threadID) << "";
	}

	// Check if all components of the vector is zero
	double functionGradientNorm = gsl_blas_dnrm2(functionGradient);
	if (functionGradientNorm == 0) {
		rDebug(threadID) << "Error: All components of the gradient vector are zero.";
		return -1;
	}

	gsl_vector_memcpy(gradientInStandardNormalSpaceOld, gradientInStandardNormalSpace);
	// Gradient in standard normal space
	result = theTransformer->transformGradientToStandardNormalSpace(x, u, functionGradient, gradientInStandardNormalSpace, threadID);
	if (result < 0) {
		rDebug(threadID) << "Error: Could not transform the gradient to standard-normal space.";
		return -1;
	}

	if (getOutputDisplayLevel() >= RObject::Maximum) {
		rDebug(threadID) << "Gradient in standard-normal space:";
		rDebug(threadID) << "";
		theDomain->print(gradientInStandardNormalSpace, threadID);
		rDebug(threadID) << "";
	}

	// Compute the norm of the gradient in standard normal space
	gradientInStandardNormalSpaceNorm = gsl_blas_dnrm2(gradientInStandardNormalSpace);

	// Check that the norm is not zero
	if (gradientInStandardNormalSpaceNorm == 0.0) {
		rDebug(threadID) << "Error: The norm of the gradient in standard-normal space is zero.";
		return -1;
	}

	// Compute alpha-vector using BLAS: alpha = gradientInStandardNormalSpace * (-1 / gradientInStandardNormalSpaceNorm);
	gsl_vector_memcpy(alpha, gradientInStandardNormalSpace);
	gsl_vector_scale(alpha, -1.0 / gradientInStandardNormalSpaceNorm);


	// Rotate by GramSchmidt process
	Rotate(alpha, randomVariablesCount);

	// Rotation matrix obtained by Gram - Schmidt scheme
	gsl_matrix *Rotation = getRotation();

	// Determination of the coefficient k(limit - state surface intersection with path Pi)
	double beta = gsl_blas_dnrm2(u);

	if (abs(beta) < 1.0)
	{
		k = 1.0 / abs(beta);
	}
	else
	{
		if (1.0 <= abs(beta) <= 3.0)
		{
			k = 1;
		}
		else
		{
			k = 3.0 / abs(beta);
		}
	}

	// Check if there are more than one random variables present
	if (randomVariablesCount > 1)
	{

		// Initial trials points of ordinates + beta //

		gsl_vector *yi = gsl_vector_alloc(randomVariablesCount - 1);
		gsl_vector_set_all(yi, 1.0*beta);

		gsl_matrix * Uprimepositive = gsl_matrix_calloc(randomVariablesCount, randomVariablesCount - 1);
		gsl_matrix * Uprimenegative = gsl_matrix_calloc(randomVariablesCount, randomVariablesCount - 1);
		gsl_vector * Uprimei = gsl_vector_alloc(randomVariablesCount);
		gsl_vector * vector_buffer = gsl_vector_alloc(randomVariablesCount);

		gsl_matrix_set_identity(Uprimepositive);
		gsl_matrix_set_identity(Uprimenegative);
		gsl_matrix_scale(Uprimepositive, k*beta);
		gsl_matrix_scale(Uprimenegative, -k*beta);

		gsl_matrix_set_row(Uprimepositive, randomVariablesCount - 1, yi);
		gsl_matrix_set_row(Uprimenegative, randomVariablesCount - 1, yi);

		gsl_vector *aCurvMinus = gsl_vector_calloc(randomVariablesCount - 1);
		gsl_vector *aCurvPlus = gsl_vector_calloc(randomVariablesCount - 1);

		// Determination of the fitting points in the rotated space //
		int counter;
		int sign;
		int stopflag = 0;
		double eta1, eta2, eta3, n1, n2, nnew;
		double g1, g2, g3, g_1, g_2, buffer, gnew;

		if (!(thePointFittingAlgorithm == RPointFittingAlgorithm::Secant)) // Newton Method
		{
#pragma region NewthodMethod
			// Allocate Jacobian matrices
			gsl_matrix *jacobian_x_u = gsl_matrix_calloc(randomVariablesCount, randomVariablesCount);
			gsl_matrix *jacobian_u_x = gsl_matrix_calloc(randomVariablesCount, randomVariablesCount);

			for (int i = 0; i < 2 * (randomVariablesCount - 1); i++)
			{
				stopflag = 0;
				if (i < randomVariablesCount - 1)
				{
					counter = i;
					sign = -1;
					gsl_matrix_get_col(Uprimei, Uprimenegative, counter);
				}
				else
				{
					counter = i - randomVariablesCount + 1;
					sign = 1;
					gsl_matrix_get_col(Uprimei, Uprimepositive, counter);
				}
				gsl_vector_memcpy(uOld, u);

				gsl_vector_set_zero(vect);
				a = gsl_vector_get(Uprimei, counter);
				b = gsl_vector_get(Uprimei, randomVariablesCount - 1);

				// u = R1'*u_prime_i;
				gsl_blas_dgemv(CblasTrans, 1.0, Rotation, Uprimei, 0.0, u);

				// Transform from u to x space
				result = theTransformer->transformPointToOriginalSpace(u, x, threadID);
				if (result < 0) {
					rDebug(threadID) << "Error: Could not transform from standard-normal space to original space.";
					return -1;
				}

				// UPDATING THE CURRENT VALUE OF RANDOM VARIABLES TO THE VALUE OF NEW REALIZATIONS
				for (int i = 0; i < randomVariablesCount; i++) {
					RRandomVariable *randomVariable = randomVariableList->value(i);
					randomVariable->setCurrentValue(gsl_vector_get(x, i));
				}
				result = theFunction->evaluateFunction(true, RObject::RandomVariable, threadID);
				if (result < 0) {
					rDebug(threadID) << "Error: Could not evaluate the function" << theFunction->objectName() << ".";
					return -1;
				}

				// Evaluating the Jacobian matrix
				// Calculate the Jacobian
				result = theTransformer->computeJacobian(x, u, jacobian_u_x, RTransformer::u_x, threadID);
				if (result < 0) {
					rDebug(threadID) << "Error: Could not compute the Jacobian matrix du/dx.";
					return -1;
				}
				result = theTransformer->computeJacobian(x, u, jacobian_x_u, RTransformer::x_u, threadID);
				if (result < 0) {
					rDebug(threadID) << "Error: Could not compute the Jacobian matrix dx/du.";
					return -1;
				}

				gsl_vector_set_zero(gradG);
				gsl_vector_set_zero(temp);
				gsl_blas_dgemv(CblasTrans, 1.0, jacobian_x_u, functionGradient, 0.0, temp);
				gsl_blas_dgemv(CblasNoTrans, 1.0, Rotation, temp, 0.0, gradG);

				g1 = theFunction->getFunctionValue();
				eta1 = gsl_vector_get(Uprimei, randomVariablesCount - 1);

				if (g1 > 0) // trying point in the safe domain
				{
					a = sign*k*beta;// sqrt(pow(k*beta, 2) - pow(b - beta, 2));
					dadb = 0;// sign* sign*(-(b - beta) / (a));
					gsl_vector_set(vect, counter, dadb);
					gsl_vector_set(vect, randomVariablesCount - 1, 1.0);

					int countCheck = 0;
					while (stopflag == 0 && countCheck < maxSolverIter)
					{
						countCheck++;
						gsl_blas_ddot(gradG, vect, &buffer);
						b = b - g1 / (buffer);
						a = sign*k*beta;// sqrt(pow(k*beta, 2) - pow(b - beta, 2));

										// New point coordinates
						gsl_vector_set(Uprimei, counter, a);
						gsl_vector_set(Uprimei, randomVariablesCount - 1, b);

						// u = R1'*u_prime_i;
						gsl_blas_dgemv(CblasTrans, 1.0, Rotation, Uprimei, 0.0, u);

						// Transform from u to x space
						result = theTransformer->transformPointToOriginalSpace(u, x, threadID);
						if (result < 0) {
							rDebug(threadID) << "Error: Could not transform from standard-normal space to original space.";
							return -1;
						}

						// UPDATING THE CURRENT VALUE OF RANDOM VARIABLES TO THE VALUE OF NEW REALIZATIONS
						for (int i = 0; i < randomVariablesCount; i++) {
							RRandomVariable *randomVariable = randomVariableList->value(i);
							randomVariable->setCurrentValue(gsl_vector_get(x, i));
						}

						//theFunction->updateAnalysisStatus();
						result = theFunction->evaluateFunction(true, RObject::RandomVariable, threadID);
						if (result < 0) {
							rDebug(threadID) << "Error: Could not evaluate the function" << theFunction->objectName() << ".";
							return -1;
						}

						functionValue = theFunction->getFunctionValue();

						// Gradient in original space
						result = theFunction->evaluateGradient(RObject::RandomVariable);
						// Attention: "functionGradient" should already be allocated in "theFunction", here just the memory address is obtained.
						functionGradient = theFunction->getGradient();
						if (result < 0) {
							rDebug(threadID) << "Error: Could not evaluate the gradient of the function.";
							return -1;
						}
						if (getOutputDisplayLevel() >= RObject::Maximum) {
							rDebug(threadID) << "Gradient in original space:";
							theDomain->print(functionGradient, threadID);
							rDebug(threadID) << "";
						}

						// Check if all components of the vector is zero
						double functionGradientNorm = gsl_blas_dnrm2(functionGradient);
						if (functionGradientNorm == 0) {
							rDebug(threadID) << "Error: All components of the gradient vector are zero.";
							return -1;
						}

						gsl_vector_memcpy(gradientInStandardNormalSpaceOld, gradientInStandardNormalSpace);

						// Gradient in standard normal space
						result = theTransformer->transformGradientToStandardNormalSpace(x, u, functionGradient, gradientInStandardNormalSpace, threadID);
						if (result < 0) {
							rDebug(threadID) << "Error: Could not transform the gradient to standard-normal space.";
							return -1;
						}

						gsl_vector_memcpy(gradG, gradientInStandardNormalSpace);

						// gradG = R1'*grad_g;
						gsl_blas_dgemv(CblasTrans, 1.0, Rotation, gradG, 0.0, vector_buffer);
						gsl_vector_memcpy(gradG, vector_buffer);

						if (functionValue < threshold)
						{
							stopflag = 1;
						}
					}

					if (countCheck == maxSolverIter)
					{
						rCritical(threadID) << "Error: The Newton Solver has not been able to converge\n";
						return -1;
					}

					if (sign > 0)
					{
						gsl_matrix_set_col(Uprimepositive, counter, Uprimei);
					}
					else
					{
						gsl_matrix_set_col(Uprimenegative, counter, Uprimei);
					}
				}
				else
				{
					a = sign*k*beta;
					dadb = 0;
					gsl_vector_set(vect, counter, dadb);
					gsl_vector_set(vect, randomVariablesCount - 1, 1.0);
					int countCheck = 0;
					while (stopflag == 0 && countCheck < maxSolverIter)
					{
						countCheck++;
						gsl_blas_ddot(gradG, vect, &buffer);
						b = b - g1 / (buffer);
						a = sign*k*beta;

						// New point coordinates
						gsl_vector_set(Uprimei, counter, a);
						gsl_vector_set(Uprimei, randomVariablesCount - 1, b);

						// u = R1'*u_prime_i;
						gsl_blas_dgemv(CblasTrans, 1.0, Rotation, Uprimei, 0.0, u);

						// Transform from u to x space
						result = theTransformer->transformPointToOriginalSpace(u, x, threadID);
						if (result < 0) {
							rDebug(threadID) << "Error: Could not transform from standard-normal space to original space.";
							return -1;
						}

						// UPDATING THE CURRENT VALUE OF RANDOM VARIABLES TO THE VALUE OF NEW REALIZATIONS
						for (int i = 0; i < randomVariablesCount; i++) {
							RRandomVariable *randomVariable = randomVariableList->value(i);
							randomVariable->setCurrentValue(gsl_vector_get(x, i));
						}

						//theFunction->updateAnalysisStatus();
						result = theFunction->evaluateFunction(true, RObject::RandomVariable, threadID);
						if (result < 0) {
							rDebug(threadID) << "Error: Could not evaluate the function" << theFunction->objectName() << ".";
							return -1;
						}

						functionValue = theFunction->getFunctionValue();

						// Gradient in original space
						result = theFunction->evaluateGradient(RObject::RandomVariable);
						// Attention: "functionGradient" should already be allocated in "theFunction", here just the memory address is obtained.
						functionGradient = theFunction->getGradient();
						if (result < 0) {
							rDebug(threadID) << "Error: Could not evaluate the gradient of the function.";
							return -1;
						}

						// Check if all components of the vector is zero
						double functionGradientNorm = gsl_blas_dnrm2(functionGradient);
						if (functionGradientNorm == 0) {
							rDebug(threadID) << "Error: All components of the gradient vector are zero.";
							return -1;
						}


						gsl_vector_memcpy(gradientInStandardNormalSpaceOld, gradientInStandardNormalSpace);
						// Gradient in standard normal space
						result = theTransformer->transformGradientToStandardNormalSpace(x, u, functionGradient, gradientInStandardNormalSpace, threadID);
						if (result < 0) {
							rDebug(threadID) << "Error: Could not transform the gradient to standard-normal space.";
							return -1;
						}

						gsl_vector_memcpy(gradG, gradientInStandardNormalSpace);

						// gradG = R1'*grad_g;
						gsl_blas_dgemv(CblasTrans, 1.0, Rotation, gradG, 0.0, vector_buffer);
						gsl_vector_memcpy(gradG, vector_buffer);

						if (functionValue < threshold)
						{
							stopflag = 1;
						}
					}
					if (countCheck == maxSolverIter)
					{
						rCritical(threadID) << "Error: The Newton Solver has not been able to converge\n";
						return -1;
					}


					if (sign > 0)
					{
						gsl_matrix_set_col(Uprimepositive, counter, Uprimei);
					}
					else
					{
						gsl_matrix_set_col(Uprimenegative, counter, Uprimei);
					}
				}
			}
			gsl_matrix_free(jacobian_u_x);
			gsl_matrix_free(jacobian_x_u);
#pragma endregion NewthodMethod
		}
		else // Secant Method
		{
#pragma region SecantMethod
			for (int i = 0; i < 2 * (randomVariablesCount - 1); i++)
			{
				stopflag = 0;
				if (i < randomVariablesCount - 1)
				{
					counter = i;
					sign = -1;
					gsl_matrix_get_col(Uprimei, Uprimenegative, counter);
				}
				else
				{
					counter = i - randomVariablesCount + 1;
					sign = 1;
					gsl_matrix_get_col(Uprimei, Uprimepositive, counter);
				}
				gsl_vector_memcpy(uOld, u);

				// Evaluate function in original probability space
				result = Evaluate_Uprime_on_x(Rotation, Uprimei, theFunction, randomVariablesCount);

				functionValueOld = functionValue;
				g1 = theFunction->getFunctionValue();
				eta1 = gsl_vector_get(Uprimei, randomVariablesCount - 1);

				if (g1 > 0) // trying point in the safe domain
				{
					gsl_vector_set(Uprimei, randomVariablesCount - 1, (1.0 + 0.5*k*k)*beta);
					gsl_vector_set(Uprimei, counter, sign*sqrt(((k*beta)*(k*beta) - pow((gsl_vector_get(Uprimei, randomVariablesCount - 1) - beta), 2))));

					// Evaluate function in original probability space
					result = Evaluate_Uprime_on_x(Rotation, Uprimei, theFunction, randomVariablesCount);

					g2 = theFunction->getFunctionValue();
					eta3 = eta2 - g2*(eta1 - eta2) / (g1 - g2);

					gsl_vector_set(Uprimei, randomVariablesCount - 1, eta3);
					gsl_vector_set(Uprimei, counter, sign*sqrt(((k*beta)*(k*beta) - pow((gsl_vector_get(Uprimei, randomVariablesCount - 1) - beta), 2))));

					// Evaluate function in original probability space
					result = Evaluate_Uprime_on_x(Rotation, Uprimei, theFunction, randomVariablesCount);

					g3 = theFunction->getFunctionValue();

					// Initialize function while parameters
					g_1 = g2;
					g_2 = g3;
					n1 = eta2;
					n2 = eta3;

					while (stopflag == 0)
					{
						nnew = n2 - g_2*(n1 - n2) / (g_1 - g_2);
						gsl_vector_set(Uprimei, randomVariablesCount - 1, nnew);
						gsl_vector_set(Uprimei, counter, sign*sqrt(((k*beta)*(k*beta) - pow((gsl_vector_get(Uprimei, randomVariablesCount - 1) - beta), 2))));

						// Evaluate function in original probability space
						result = Evaluate_Uprime_on_x(Rotation, Uprimei, theFunction, randomVariablesCount);

						gnew = theFunction->getFunctionValue();
						g_1 = g_2;
						g_2 = gnew;
						n1 = n2;
						n2 = nnew;

						// Test to break loop in function while
						if (abs(gnew) < threshold)
						{
							stopflag = 1;
						}
					}

					if (sign > 0)
					{
						gsl_matrix_set_col(Uprimepositive, counter, Uprimei);
					}
					else
					{
						gsl_matrix_set_col(Uprimenegative, counter, Uprimei);
					}


				}
				else
				{
					gsl_vector_set(Uprimei, randomVariablesCount - 1, (1.0 - 0.5*k*k)*beta);

					// u = R1'*u_prime_i;
					gsl_blas_dgemv(CblasTrans, 1.0, Rotation, Uprimei, 0.0, u);

					// Transform from u to x space
					result = theTransformer->transformPointToOriginalSpace(u, x, threadID);
					if (result < 0) {
						rDebug(threadID) << "Error: Could not transform from standard-normal space to original space.";
						return -1;
					}

					eta2 = gsl_vector_get(Uprimei, randomVariablesCount - 1);

					// UPDATING THE CURRENT VALUE OF RANDOM VARIABLES TO THE VALUE OF NEW REALIZATIONS
					for (int i = 0; i < randomVariablesCount; i++) {
						RRandomVariable *randomVariable = randomVariableList->value(i);
						randomVariable->setCurrentValue(gsl_vector_get(x, i));
					}
					result = theFunction->evaluateFunction(true, RObject::RandomVariable, threadID);
					if (result < 0) {
						rDebug(threadID) << "Error: Could not evaluate the function" << theFunction->objectName() << ".";
						return -1;
					}

					g2 = theFunction->getFunctionValue();
					eta3 = eta2 - g2*(eta1 - eta2) / (g1 - g2);

					gsl_vector_set(Uprimei, randomVariablesCount - 1, eta3);

					// Evaluate function in original probability space
					result = Evaluate_Uprime_on_x(Rotation, Uprimei, theFunction, randomVariablesCount);

					g3 = theFunction->getFunctionValue();

					// Initialize function while parameters
					g_1 = g2;
					g_2 = g3;
					n1 = eta2;
					n2 = eta3;

					while (stopflag == 0)
					{
						nnew = n2 - g_2*(n1 - n2) / (g_1 - g_2);
						gsl_vector_set(Uprimei, randomVariablesCount - 1, nnew);

						// Evaluate function in original probability space
						result = Evaluate_Uprime_on_x(Rotation, Uprimei, theFunction, randomVariablesCount);

						gnew = theFunction->getFunctionValue();
						g_1 = g_2;
						g_2 = gnew;
						n1 = n2;
						n2 = nnew;

						// Test to break loop in function while
						if (abs(gnew) < threshold)
						{
							stopflag = 1;
						}
					}

					if (sign > 0)
					{
						gsl_matrix_set_col(Uprimepositive, counter, Uprimei);
					}
					else
					{
						gsl_matrix_set_col(Uprimenegative, counter, Uprimei);
					}
				}
			}
#pragma endregion SecantMethod
		}

		// Computes the curvatures a_i _ + / -
		for (int i = 0; i < randomVariablesCount - 1; i++)
		{
			gsl_vector_set(aCurvMinus, i, 2.0*(gsl_matrix_get(Uprimenegative, randomVariablesCount - 1, i) - beta) / pow((gsl_matrix_get(Uprimenegative, i, i)), 2));
			gsl_vector_set(aCurvPlus, i, 2.0*(gsl_matrix_get(Uprimepositive, randomVariablesCount - 1, i) - beta) / pow((gsl_matrix_get(Uprimepositive, i, i)), 2));
		}
		if (getOutputDisplayLevel() >= RObject::Moderate) {
			rDebug(threadID) << "\nThe Positive Perturbation Matrix:";
			theDomain->print(Uprimepositive, threadID);
			rDebug(threadID) << "\nThe Negative Perturbation Matrix:";
			theDomain->print(Uprimenegative, threadID);
		}
		if (getOutputDisplayLevel() >= RObject::Minimum) {
			rDebug(threadID) << "\nThe Plus Curvature Vector:";
			theDomain->print(aCurvPlus, threadID);
			rDebug(threadID) << "\nThe Minus Curvature Vector:";
			theDomain->print(aCurvMinus, threadID);
		}
		// Coordinates of fitting points in rotated space
		// along with the value of lsf at the fitting points and the curvature a_i _ + / -in a same matrix

		// Along minus axis

		// Along plus axis

		// Probability of failure & beta Breitung modified by Hohenbichler / Rackwitz
		double denominator = gsl_cdf_gaussian_P(-beta, 1.0);
		if (denominator == 0.0) {
			rDebug(threadID) << "Error: SORM analysis did not converge due to too a large reliability index value.";
			return -1;
		}

		double psi_beta = gsl_ran_gaussian_pdf(beta, 1.0) / denominator;
		double product = 1.0;
		double debugcheck = 0.0;

		for (int i = 0; i < randomVariablesCount - 1; i++)
		{
			debugcheck = gsl_vector_get(aCurvMinus, i);
			debugcheck = gsl_vector_get(aCurvPlus, i);
			product *= 1 / 2.0
				* (
					1.0 / sqrt(1.0 + psi_beta * gsl_vector_get(aCurvMinus, i))
					+
					1.0 / sqrt(1.0 + psi_beta * gsl_vector_get(aCurvPlus, i))
					);
		}

		double pfSORM = theFailureProbability * product;

		// Assign the corrected values
		theFailureProbability = pfSORM;

		gsl_vector_free(aCurvPlus);
		gsl_vector_free(aCurvMinus);
		gsl_vector_free(vector_buffer);
		gsl_vector_free(Uprimei);
		gsl_vector_free(yi);

		gsl_matrix_free(Uprimenegative);
		gsl_matrix_free(Uprimepositive);
	}


	gsl_vector_free(gradientInStandardNormalSpaceOld);
	gsl_vector_free(gradG);
	gsl_vector_free(uOld);
	gsl_vector_free(vect);
	gsl_vector_free(temp);


	return 0;
}


int RSORMAnalyzer::correctWithEfficientSORM(RFunction *passedFunction, bool populateListsInFunction) // Under Development
{
	return 0;
}

int RSORMAnalyzer::Evaluate_Uprime_on_x(gsl_matrix *Rotationmat, gsl_vector *Uprimei_vect, RFunction *theFunction, int randomVariablesCount)
{
	int result;

	QList<RRandomVariable *> *randomVariableList = theFunction->getRandomVariableList();


	// u = R1'*u_prime_i;
	gsl_blas_dgemv(CblasTrans, 1.0, Rotationmat, Uprimei_vect, 0.0, u);

	// Transform from u to x space
	result = theTransformer->transformPointToOriginalSpace(u, x, threadID);
	if (result < 0) {
		rDebug(threadID) << "Error: Could not transform from standard-normal space to original space.";
		return -1;
	}

	// UPDATING THE CURRENT VALUE OF RANDOM VARIABLES TO THE VALUE OF NEW REALIZATIONS
	for (int i = 0; i < randomVariablesCount; i++) {
		RRandomVariable *randomVariable = randomVariableList->value(i);
		randomVariable->setCurrentValue(gsl_vector_get(x, i));
	}

	result = theFunction->evaluateFunction(true, RObject::RandomVariable, threadID);
	if (result < 0) {
		rDebug(threadID) << "Error: Could not evaluate the function" << theFunction->objectName() << ".";
		return -1;
	}

	return 0;
}

int RSORMAnalyzer::computeHessian(bool populateListsInFunction, RFunction * theFunction, int randomVariablesCount, gsl_vector *ustar)
{
#ifdef HESSIAN_TRANSFORMATION
	int result;
	// Compute the Hessian matrix

	// Parse the passed function
	if (populateListsInFunction) {
		result = theFunction->populateLists();
		if (result < 0) {
			rDebug(threadID) << "Error: Could not parse the function" << theFunction->objectName() << ".";
			return -1;
		}
	}

	// Compute the perturbation vectors in original probability space
	QList<RRandomVariable *> theRandomVariableList = *theLimitStateFunction->getRandomVariableList();
	QList<RParameter *> parameterList;
	double hi, hj;
	for (int i = 0; i < theRandomVariableList.count(); i++) {
		parameterList << theRandomVariableList[i];
	}

	gsl_vector* perturbationVector_fore = gsl_vector_calloc(randomVariablesCount);
	gsl_vector* perturbationVector_back = gsl_vector_calloc(randomVariablesCount);

	for (int i = 0; i < randomVariablesCount; i++)
	{
		RParameter *parameteri = parameterList[i];
		// Getting the standard deviation of the random variable
		RRandomVariable *randomVariablei = qobject_cast<RRandomVariable *>(parameteri);

		// Perturbing the current value of the random variable 
		hi = 1.0 / 2000.0;
		double currentValueBackup = gsl_vector_get(ustar, i);
		double purterbedValue = currentValueBackup + hi;
		gsl_vector_set(perturbationVector_fore, i, purterbedValue);
		purterbedValue = currentValueBackup - hi;
		gsl_vector_set(perturbationVector_back, i, purterbedValue);
	}

	gsl_vector* x_backup = gsl_vector_calloc(randomVariablesCount);
	gsl_vector_memcpy(x_backup, x);

	// Transform from u to x space
	result = theTransformer->transformPointToOriginalSpace(perturbationVector_fore, x, threadID);
	if (result < 0) {
		rDebug(threadID) << "Error: Could not transform from standard-normal space to original space.";
		return -1;
	}

	gsl_vector_memcpy(perturbationVector_fore, x);

	// Transform from u to x space
	result = theTransformer->transformPointToOriginalSpace(perturbationVector_back, x, threadID);
	if (result < 0) {
		rDebug(threadID) << "Error: Could not transform from standard-normal space to original space.";
		return -1;
	}

	gsl_vector_memcpy(perturbationVector_back, x);
	gsl_vector_memcpy(x, x_backup);

	// Evaluate and store the Hessian matrix
	theFunction->evaluateHessian(RObject::RandomVariable, perturbationVector_fore, perturbationVector_back, 1000.0, threadID);

	gsl_vector_free(perturbationVector_fore);
	gsl_vector_free(perturbationVector_back);
	gsl_vector_free(x_backup);
#endif

	return 0;
}

void RSORMAnalyzer::Rotate(gsl_vector *vector, int n)
{
	if (gramrot)
	{
		gsl_matrix_free(gramrot);
	}

	gramrot = gsl_matrix_calloc(n, n);
	gsl_matrix *r0 = gsl_matrix_calloc(n, n);
	gsl_matrix *r = gsl_matrix_calloc(n, n);
	gsl_matrix *matrix = gsl_matrix_calloc(n, n);

	//Set r0 to Identity
	gsl_matrix_set_identity(r0);

	double element;
	double temp;
	gsl_vector *sumj = gsl_vector_alloc(n);

	//set the n-th row to V
	gsl_matrix_set_row(r0, n - 1, vector);
	gsl_matrix_set_row(r, n - 1, vector);

	//Define the Vector views of matrices
	gsl_vector * r0n = gsl_vector_alloc(n);
	gsl_vector * rn = gsl_vector_alloc(n);

	//Computation of the Rows of R
	for (int i = n - 2; i > -1; i--)
	{
		gsl_matrix_set_zero(matrix);

		//rn = rn;
		for (int j = i + 1; j < n; j++)
		{
			gsl_matrix_get_row(rn, r, j);
			gsl_matrix_get_row(r0n, r0, i);

			//		//	theDomain->print(&rn.vector);
			//		//theDomain->print(&r0n.vector);

			gsl_blas_ddot(r0n, rn, &element);
			gsl_blas_ddot(rn, rn, &temp);
			element = element / temp;
			gsl_blas_dscal(element, rn);
			gsl_matrix_set_row(matrix, j, rn);
		}
		gsl_vector_set_zero(sumj);

		//Compute sumj
		for (int k = 0; k < n; k++)
		{
			for (int l = 0; l < n; l++)
			{
				gsl_vector_set(sumj, l, gsl_vector_get(sumj, l) + gsl_matrix_get(matrix, k, l));
			}
		}

		//Subtract sumj from r0(i,:)
		for (int k = 0; k < n; k++)
		{
			gsl_matrix_set(r, i, k, gsl_matrix_get(r0, i, k) - gsl_vector_get(sumj, k));
		}
	}

	//Normalization of each row separately
	for (int i = 0; i < n - 1; i++)
	{
		gsl_matrix_get_row(rn, r, i);
		gsl_blas_dscal(1.0 / gsl_blas_dnrm2(rn), rn);
		gsl_matrix_set_row(r, i, rn);
	}


	gsl_matrix_memcpy(gramrot, r);
	gsl_matrix_free(r);
	gsl_matrix_free(r0);
	gsl_matrix_free(matrix);
	gsl_vector_free(sumj);
}

gsl_matrix * RSORMAnalyzer::getRotation()
{
	return gramrot;
}

void RSORMAnalyzer::Orthogonalize(gsl_vector *uvector, gsl_vector *ustar, gsl_matrix *deltamatrix, int n)
{
	gsl_vector *ui = gsl_vector_calloc(uvector->size);
	gsl_vector *ubuffer = gsl_vector_calloc(uvector->size);
	gsl_vector *deltaj = gsl_vector_calloc(uvector->size);
	gsl_vector_memcpy(ui, uvector);
	double scale = 1.0;
	double buffer;
	for (int i = 0; i < n; i++)
	{
		gsl_matrix_get_row(deltaj, deltamatrix, i);

		scale = 1.0;
		// uj = u[j-1] - 1/|deltaj|^2 * deltaj^T (ustar - u[j-1]) deltaj
		scale = 1.0 / gsl_blas_dnrm2(deltaj);
		scale *= scale;
		gsl_vector_memcpy(ubuffer, ustar);
		gsl_vector_sub(ubuffer, ui);


		gsl_blas_ddot(deltaj, ubuffer, &buffer);
		gsl_vector_scale(deltaj, scale*buffer);
		gsl_vector_add(ui, deltaj);
	}
	uj = ui;
}

gsl_vector * RSORMAnalyzer::get_uj()
{
	return uj;
}

bool RSORMAnalyzer::canBeRunParallelly() {
	return false;
}