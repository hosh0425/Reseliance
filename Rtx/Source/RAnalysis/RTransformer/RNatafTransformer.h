// Last modified by Mojtaba on June 30, 2012

#ifndef RNatafTransformer_H
#define RNatafTransformer_H

#include "RTransformer.h"
#include "RRandomVariable.h"

class QMutex;

class RNatafTransformer : public RTransformer
{
	Q_OBJECT

	Q_PROPERTY(bool SparseCorrelationMatrix READ getSparseCorrelationMatrix WRITE setSparseCorrelationMatrix);

public:
	RNatafTransformer(QObject *parent, QString &name);
	~RNatafTransformer();

	// Determines if the correlation matrix is sparse
	bool getSparseCorrelationMatrix() const;
	void setSparseCorrelationMatrix(bool value);

	// Establishes the correlation matrix and computes the Cholesky decomposition at the start of the analysis
    int initializeAtStartOfAnalysis();

    // Transforms random variable realizations from the standard normal to the original space, in the given threadID ($MTh$)
    int transformPointToOriginalSpace(const gsl_vector *standardNormalVector, gsl_vector *originalVector, int threadID);

	// Transforms random variable realizations from the original to the standard normal space, in the given threadID ($MTh$)
    int transformPointToStandardNormalSpace(const gsl_vector *originalVector, gsl_vector *standardNormalVector, int threadID);

    // Transforms a gradient vector from the standard normal to the original space, in the given threadID ($MTh$)
    int transformGradientToOriginalSpace(const gsl_vector *pointInOriginalSpace, const gsl_vector *pointInStandardNormalSpace, const gsl_vector *vectorInStandardNormalSpace, gsl_vector *vectorInOriginalSpace, int threadID);

	// Transforms a gradient vector from the original to the standard normal space, in the given threadID ($MTh$)
    int transformGradientToStandardNormalSpace(const gsl_vector *pointInOriginalSpace, const gsl_vector *pointInStandardNormalSpace, const gsl_vector *vectorInOriginalSpace, gsl_vector *vectorInStandardNormalSpace, int threadID);

	// Computes and returns the Jacobiam matrix, in the given threadID ($MTh$)
	int computeJacobian(const gsl_vector *pointInOriginalSpace, const gsl_vector *pointInStandardNormalSpace, gsl_matrix *jacobian, RTransformer::RJacobianType jacobianType, int threadID);

	// Computes and returns the Jacobian matrix of the vector of standard normal random variables wrt. the means and stdvs of original random variables, in the given threadID ($MTh$)
	int computeYSensitivityToDistributionParameters(const gsl_vector *pointInOriginalSpace, gsl_matrix *dy_dMean, gsl_matrix *dy_dStdv,  int threadID, QList<RPointer<RRandomVariable> > sensitivityRandomVariableList);

	// Sensitivity implementations
	// Computes the sensitivity of joint PDF in original space wrt Means and Stdvs of random variables, in the given threadID ($MTh$)
	// sensitivityRandomVariableList is the list of random variables for which the sensitivity is needed
	// This method computes (df/dMean)*(1/f) and (df/dStdv)*(1/f) where f is the joint probability distribution of 
	// all random variables in the original space and Mean and Stdv are the mean and standard deviation of the desired random variable.
	int computeJointPDFSensitivityToDistributionParameters(const gsl_vector *x, const gsl_vector *u,  QList<RPointer<RRandomVariable> > sensitivityRandomVariableList, gsl_vector *dfdMean_1_f, gsl_vector *dfdStdv_1_f, double NeglectCorrelationInSensitivity, int threadID);

	// Copies the data member matrices at index 0 to the index of the given threadID for data members that are changed to array due to parallelization. Each subclass must also call the method of their superclass
	void initializeForParallelComputation(int threadID);

	// returns true as this transformer is parallelized
	bool canBeRunParallelly();

protected:

	// Returns a new Nataf Transformer
	RPointer<RTransformer> createNewTransformerOfTheSameType();

private:

	// Auxiliary method to transform the vector of original random variables (x) to uncorelated standard normal random variables (z), in the given threadID ($MTh$)
	int xToz(const gsl_vector *x, gsl_vector *z, int threadID);

	// Auxiliary method to transform the vector of corelated standard normal random variables (y) to uncorelated standard normal random variables (z), in the given threadID ($MTh$)
	int yToz(const gsl_vector *y, gsl_vector *z, int threadID);

	// Auxiliary method to establish the modified correlation matrix; the optional argument will perturb one of the random variables, which results in a purturbed correlation matrix that is used in computing sensitivities, in the given threadID ($MTh$)
	int establishCorrelationMatrix(gsl_matrix *R0, int threadID, RRandomVariable *perturbedRV = 0);

	// Auxiliary method to compute the Cholesky decomposition of the correlation matrix, in the given threadID ($MTh$)
	int computeCholesky(const gsl_matrix *R0, gsl_matrix *cholesky, int threadID, gsl_matrix *choleskyInverse = 0);

	// Auxiliary methods to compute the modified correlation coefficients and its derivative
	double phi2(double z_i, double z_j, double rho);
	double integrand(RRandomVariable *rv_i, double z_i, double mean_i, double stdv_i, RRandomVariable *rv_j, double z_j, double mean_j, double stdv_j, double rho_zero, bool evaluateDerivative, int threadID);
	
	// If the last two argument of the following method are not given, it will compute the ordinary Nataf integral. If they are given other than default values, the integrand will change ton those needed to compute the derviative of the correlation coefficient
	double doubleIntegral(RRandomVariable *rv_i, double mean_i, double stdv_i, RRandomVariable *rv_j, double mean_j, double stdv_j, double rho, int threadID, bool evaluateDerivative, int numIntegrand = 1, double drho_zerodMean = 0.0);
	void residualFunction(double rho_original, double rho, RRandomVariable *rv_i, double mean_i, double stdv_i, RRandomVariable *rv_j, double mean_j, double stdv_j, double *f, double *df, int threadID);
	double modifyCorrelation(RRandomVariable *rv_i, RRandomVariable *rv_j, double rho_original, int threadID);

	// Auxiliary methods to compute the derivative of correlation coefficients wrt distribution parameters
	double integrand2(RRandomVariable *rv_i, double z_i, double mean_i, double stdv_i, RRandomVariable *rv_j, double z_j, double mean_j, double stdv_j, double rho_zero, bool evaluateDerivative, int threadID);
	double integrand3(RRandomVariable *rv_i, double z_i, double mean_i, double stdv_i, RRandomVariable *rv_j, double z_j, double mean_j, double stdv_j, double rho_zero, bool evaluateDerivative, int threadID);
	double integrand4(RRandomVariable *rv_i, double z_i, double mean_i, double stdv_i, RRandomVariable *rv_j, double z_j, double mean_j, double stdv_j, double rho_zero, double drho_zerodMean, int threadID);
	double computeCorrelationSensitvity(RRandomVariable *rv_i, RRandomVariable *rv_j, double rho_zero, bool MeanSensitvity, int threadID);
	void residualFunctionSensitivity(double constantPart, double rho_zero, double drho_zerodMean, RRandomVariable *rv_i, double mean_i, double stdv_i, RRandomVariable *rv_j, double mean_j, double stdv_j, double *f, int threadID);

	// Auxiliary method to compute and return the derivative of Cholesky matrix wrt to Mean and stdv of a random variable, dL/dMean and dL/dStdv, by FDM & DDM, in the given threadID ($MTh$)
	int computedLinvdMean(RContinuousRandomVariable *randomVariable, gsl_matrix *dLinvdMean, gsl_matrix *dLinvdStdv, bool DDM, int threadID);

	void freeDLinvMaps();

	// Data member that stores the number of random variables
	int numRVs;

	// Data member that stores the correlation matrix, for each thread $MTh$
	gsl_matrix **R0;

	// Data member that stores the Cholesky decomposition of the correlation matrix, for each thread $MTh$
	gsl_matrix **L;

	// Data member that stores the inverse Cholesky decomposition of the correlation matrix, for each thread $MTh$
	gsl_matrix **Linv;

	// Data memeber that stores the sparsity of the correlation matrix
	bool theSparseCorrelationMatrix;

	// QLists to store values of dLinv/dMean and dLinv/dStdv for the sensitivity random variable with the same index
	QList<gsl_matrix *> dLinvdMeanList;
	QList<gsl_matrix *> dLinvdStdvList;

	// a boolean flag to indicate wether maps have been filled
	bool linvdMapsHaveBeenFilled;

	// a lock to prevent multiple threads from creating the maps together
	QMutex * safeLinvdMapLock;
};

#endif
