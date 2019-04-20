// Last modified by Mojtaba on June 30, 2012

#ifndef RNoCorrelationTransformer_H
#define RNoCorrelationTransformer_H

#include "RTransformer.h"

class RNoCorrelationTransformer : public RTransformer
{
	Q_OBJECT
		
public:
	RNoCorrelationTransformer(QObject *parent, QString name);
	~RNoCorrelationTransformer();

	// Establishes the correlation matrix and the computes the Cholesky decomposition at the start of the analysis
    int initializeAtStartOfAnalysis();

    // Transforms random variable realizations from the standard normal to the original space in the given threadID ($MTh$)
    int transformPointToOriginalSpace(const gsl_vector *standardNormalVector, gsl_vector *originalVector, int threadID);

	// Transforms random variable realizations from the original to the standard normal space, in the given threadID ($MTh$)
    int transformPointToStandardNormalSpace(const gsl_vector *originalVector, gsl_vector *standardNormalVector, int threadID);

    // Transforms a gradient vector from the standard normal to the original space, in the given thradID ($MTh$)
    int transformGradientToOriginalSpace(const gsl_vector *pointInOriginalSpace, const gsl_vector *pointInStandardNormalSpace, const gsl_vector *vectorInStandardNormalSpace, gsl_vector *vectorInOriginalSpace, int threadID);

	// Transforms a gradient vector from the original to the standard normal space, in the given thradID ($MTh$)
    int transformGradientToStandardNormalSpace(const gsl_vector *pointInOriginalSpace, const gsl_vector *pointInStandardNormalSpace, const gsl_vector *vectorInOriginalSpace, gsl_vector *vectorInStandardNormalSpace, int threadID);

	// Computes and returns the Jacobiam matrix, in the given thradID ($MTh$)
	int computeJacobian(const gsl_vector *pointInOriginalSpace, const gsl_vector *pointInStandardNormalSpace, gsl_matrix *jacobian, RTransformer::RJacobianType jacobianType, int threadID);

	// Computes and returns the Jacobian matrix of the vector of standard normal random variables wrt. the means and stdvs of original random variables, in the given thradID ($MTh$)
	int computeYSensitivityToDistributionParameters(const gsl_vector *pointInOriginalSpace, gsl_matrix *dy_dMean, gsl_matrix *dy_dStdv,  int threadID, QList<RPointer<RRandomVariable> > sensitivityRandomVariableList);

	// Sensitivity implementations
	// Computes the sensitivity of joint PDF in original space wrt Means and Stdvs of random variables, in the given thradID ($MTh$)
	// sensitivityRandomVariableList is the list of random variables for which the sensitivity is needed
	int computeJointPDFSensitivityToDistributionParameters(const gsl_vector *x, const gsl_vector *u,  QList<RPointer<RRandomVariable> > sensitivityRandomVariableList, gsl_vector *dfdMean_1_f, gsl_vector *dfdStdv_1_f, double NeglectCorrelationInSensitivity, int threadID);

	// returns true as this transformer is parallelized
	bool canBeRunParallelly();

protected:

	// Returns a new RNoCorrelationTransformer
	RPointer<RTransformer> createNewTransformerOfTheSameType();

private:

	// Auxiliary method to compute the diagonal Jacobian matrix of the standard normal variables (u) wrt. the original random variables (x), in the given thradID ($MTh$)
	int computeDiagonalJacobian_u_x_AsVector(const gsl_vector *originalVector, const gsl_vector *standardNormalVector, gsl_vector *jacobianV_u_x, int threadID);
	
	// Auxiliary method to compute and return the derivative of Cholesky matrix wrt to Mean and stdv of a random variable, dL/dMean and dL/dStdv, by FDM & DDM, in the given thradID ($MTh$)
	int computedLinvdMean(RContinuousRandomVariable *randomVariable, gsl_matrix *dLinvdMean, gsl_matrix *dLinvdStdv, bool DDM, int threadID);
};

#endif // RNoCorrelationTransformer_H
