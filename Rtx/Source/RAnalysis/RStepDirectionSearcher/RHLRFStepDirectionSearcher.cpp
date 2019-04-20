#include "RHLRFStepDirectionSearcher.h"

#include "RDomain.h"

#include <gsl/gsl_blas.h>

RHLRFStepDirectionSearcher::RHLRFStepDirectionSearcher(QObject *parent, QString name)
	: RStepDirectionSearcher(parent, name)
{
	searchDirection = 0;
}

RHLRFStepDirectionSearcher::~RHLRFStepDirectionSearcher()
{
	if (searchDirection) {
		gsl_vector_free(searchDirection);
		searchDirection = 0;
	}
}

int RHLRFStepDirectionSearcher::computeStepDirection(int iterationIndex, gsl_vector const *u, double functionValue, gsl_vector const *gradientInStandardNormalSpace)
{
	// Compute the norm of the gradient
	double gradientNorm = gsl_blas_dnrm2(gradientInStandardNormalSpace);

	// Check that the norm is not zero
	if (gradientNorm == 0.0) {
		rCritical() << "Error: The norm of the entered gradient in direction searcher is zero.";
		return -1;
	}
	
	// Cerate the alpha-vector
	gsl_vector *alpha = gsl_vector_calloc(gradientInStandardNormalSpace->size);

	// Compute alpha-vector using BLAS: alpha = gradientInStandardNormalSpace * (-1 / gradientNorm)
	gsl_vector_memcpy(alpha, gradientInStandardNormalSpace);
	gsl_vector_scale(alpha, -1.0/gradientNorm);

	// Compute the alpha.u using BLAS: alpha_times_u = alpha * u
	double alpha_times_u = 0;
	gsl_blas_ddot(alpha, u, &alpha_times_u);
	// Compute the search direction vector using BLAS: direction = alpha * (functionValue / gradientNorm + alpha_times_u) - u
	searchDirection = gsl_vector_calloc(gradientInStandardNormalSpace->size);
	gsl_vector_memcpy(searchDirection, alpha);
	gsl_vector_scale(searchDirection, functionValue / gradientNorm + alpha_times_u);
	gsl_vector_sub(searchDirection, u);

	// Delete the allocated vectors
	if (alpha) {
		gsl_vector_free(alpha);
		alpha = 0;
	}

	return 0;
}

gsl_vector * RHLRFStepDirectionSearcher::getSearchDirection() const
{
	return searchDirection;
}

bool RHLRFStepDirectionSearcher::canBeRunParallelly() {	
	return false;
}
