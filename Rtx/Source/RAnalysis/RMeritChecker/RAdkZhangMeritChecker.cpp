#include "RAdkZhangMeritChecker.h"

#include <gsl/gsl_blas.h>

RAdkZhangMeritChecker::RAdkZhangMeritChecker(QObject *parent, QString name)
	: RMeritChecker(parent, name)
{
	theMultiplier = 2.0;
	theAdder = 10.0;
	theFactor = 0.5;
}

RAdkZhangMeritChecker::~RAdkZhangMeritChecker()
{

}

double RAdkZhangMeritChecker::getMultiplier() const	
{	
	return theMultiplier;
}	

void RAdkZhangMeritChecker::setMultiplier(double value)	
{	
	theMultiplier = value;
}	

double RAdkZhangMeritChecker::getAdder() const	
{	
	return theAdder;
}	

void RAdkZhangMeritChecker::setAdder(double value)	
{	
	theAdder = value;
}	

double RAdkZhangMeritChecker::getFactor() const	
{	
	return theFactor;
}	

void RAdkZhangMeritChecker::setFactor(double value)	
{	
	theFactor = value;
}	

int RAdkZhangMeritChecker::check(gsl_vector *u, double functionValue, gsl_vector *functionGradient, double stepSize, const gsl_vector *stepDirection, double newFunctionValue)
{
	// Update penalty parameter 'c' (should remain constant along the search direction)
	updateMeritParameters(u, functionValue, functionGradient);
	
	// New point in standard normal space: Compute using BLAS: uNew = u + searchDirection * stepSize
	int count = u->size;
	gsl_vector *uNew = gsl_vector_calloc(count);
	gsl_vector_memcpy(uNew, stepDirection);
	gsl_vector_scale(uNew, stepSize);
	gsl_vector_add(uNew, u);

	// Compute value of merit functions
	double meritOld = getMeritFunctionValue(u, functionValue, 0);
	double meritNew = getMeritFunctionValue(uNew, newFunctionValue, 0);

	// Gradient of the merit function
	double signum;
	if (functionValue != 0.0) {
		signum = functionValue / qAbs(functionValue);
	} else {
		signum = 1.0;
	}

	// Compute using BLAS: gradMOld = u + c * signum * functionGradient
	gsl_vector *gradMOld = gsl_vector_calloc(count);
	gsl_vector_memcpy(gradMOld, functionGradient);
	gsl_vector_scale(gradMOld, c * signum);
	gsl_vector_add(gradMOld, u);

	// Compute dotProduct using BLAS: dotProduct = gradMOld * stepDirection
	double dotProduct;
	gsl_blas_ddot(gradMOld, stepDirection, &dotProduct);

	if (gradMOld) {
		gsl_vector_free(gradMOld);
		gradMOld = 0;
	}
	if (uNew) {
		gsl_vector_free(uNew);
		uNew = 0;
	}
	
	// Do the check
	if (meritNew - meritOld <= theFactor * stepSize * dotProduct) {
		return 0;  // ok
	} else {
		return -1; // not ok
	}
}

double RAdkZhangMeritChecker::getMeritFunctionValue(gsl_vector *u, double functionValue, gsl_vector *functionGradient)
{
	// Note that it is correct to keep 'c' constant
	// Compute merit function: Compute dotProduct using BLAS: u2 = u * u
	double u2;
	gsl_blas_ddot(u, u, &u2);
	double merit = 0.5 * u2 + c * qAbs(functionValue);

	// Return the result
	return merit;
}

int RAdkZhangMeritChecker::updateMeritParameters(gsl_vector *u, double functionValue, gsl_vector *functionGradient)
{
	// Update penalty factor 'c'
	c = (gsl_blas_dnrm2(u) / gsl_blas_dnrm2(functionGradient)) * theMultiplier + theAdder;
	return 0;
}

bool RAdkZhangMeritChecker::canBeRunParallelly() {	
	return false;
}
