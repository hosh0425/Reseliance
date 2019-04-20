#include "RStandardConvergenceChecker.h"

#include "RDomain.h"

#include <gsl/gsl_blas.h>

RStandardConvergenceChecker::RStandardConvergenceChecker(QObject *parent, QString name)
	: RConvergenceChecker(parent, name)
{
	theE1 = 0.001;
	theE2 = 0.001;
	theScaleValue = 0;

	criterium1 = 0;
	criterium2 = 0;
}

RStandardConvergenceChecker::~RStandardConvergenceChecker()
{

}

double RStandardConvergenceChecker::getE1()
{
	return theE1;
}

void RStandardConvergenceChecker::setE1(double value)
{
	theE1 = value;
}

double RStandardConvergenceChecker::getE2()
{
	return theE2;
}

void RStandardConvergenceChecker::setE2(double value)
{
	theE2 = value;
}

double RStandardConvergenceChecker::getScaleValue()
{
	return theScaleValue;
}

void RStandardConvergenceChecker::setScaleValue(double value)
{
	theScaleValue = value;
}

int RStandardConvergenceChecker::check(gsl_vector const *u, double functionValue, gsl_vector const *gradientInStandardNormalSpace)
{
	if (theScaleValue == 0.0) {
		rCritical() << "Error: Scale value has not been set in the convergence checker" << objectName();
		return -1;
	}

	// Cerate the alpha-vector
	gsl_vector *alpha = gsl_vector_calloc(gradientInStandardNormalSpace->size);
	// Compute the norm of the gradient
	double gradientNorm = gsl_blas_dnrm2(gradientInStandardNormalSpace);
	// Check that the norm is not zero
	if (gradientNorm == 0.0) {
		rCritical() << "Error: The norm of the entered gradient in direction searcher is zero.";
		return -1;
	}
	// Compute alpha-vector using BLAS: alpha = gradientInStandardNormalSpace * (-1 / gradientNorm)
	gsl_vector_memcpy(alpha, gradientInStandardNormalSpace);
	gsl_vector_scale(alpha, -1/gradientNorm);


	// The scaling factor
	double temp0 = gsl_blas_dnrm2(u);
	if (temp0 < 1.0) {
		temp0 = 1.0; 
	}

	// Compute scaled u-vector using BLAS: u_scaled = (1 / temp0) * u
	gsl_vector *u_scaled = gsl_vector_calloc(u->size);
	gsl_vector_memcpy(u_scaled, u);
	gsl_vector_scale(u_scaled, 1/temp0);

	// First convergence criteria
	criterium1 = qAbs(functionValue / theScaleValue);

	// Second convergence criteria
	// Compute the u_scaled.alpha using BLAS: u_scaled_times_alpha = u_scaled * alpha
	double u_scaled_times_alpha = 0;
	gsl_blas_ddot(u_scaled, alpha, &u_scaled_times_alpha);
	// Compute using BLAS: u_scaled = u_scaled - u_scaled_times_alpha * alpha;
	gsl_blas_daxpy(-u_scaled_times_alpha, alpha, u_scaled);
	criterium2 = gsl_blas_dnrm2(u_scaled);

	// Inform user about convergence status 
	if (getOutputDisplayLevel() > RObject::None) {
		rDebug() << "Check1 =" << criterium1 << ", Check2 =" << criterium2 << ", Distance =" << gsl_blas_dnrm2(u);
		rDebug() << "";
	}

	// Delete the allocated vectors
	if (alpha) {
		gsl_vector_free(alpha);
		alpha = 0;
	}
	if (u_scaled) {
		gsl_vector_free(u_scaled);
		u_scaled = 0;
	}

	// Return '1' if the analysis converged ('-1' otherwise)
	if ((criterium1 < theE1) && (criterium2 < theE2)) {
		return 1;
	}
	else {
		return -1;
	}
}

int RStandardConvergenceChecker::getNumberOfCriteria()
{
	return 2;
}

double RStandardConvergenceChecker::getCriteriaValue(int whichCriteria)
{
	if (whichCriteria == 1) {
		return criterium1; 
	}
	else if (whichCriteria == 2) {
		return criterium2;
	}
	else {
		rCritical() << "Error: Criterium number " << whichCriteria << " does not exist.";
		return 0;
	}
}

bool RStandardConvergenceChecker::canBeRunParallelly() {	
	return false;
}
