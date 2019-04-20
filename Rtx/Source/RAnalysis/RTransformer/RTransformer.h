// Last modified by Mojtaba on June 30, 2012

#ifndef RTransformer_H
#define RTransformer_H

#include "RObject.h"
#include "RPointer.h" //salarifard

#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>

class RDomain;
class RRandomVariable;
class RContinuousRandomVariable;

class RTransformer : public RObject
{
	Q_OBJECT

	// Controls the amount of output that is shown in the output pane; options are None, Minimum, Moderate, and Maximum
	Q_PROPERTY(ROutputDisplayLevelType OutputDisplayLevel READ getOutputDisplayLevel WRITE setOutputDisplayLevel USER true)

	// Enum for the type of Jacobian to be computed
	Q_ENUMS(RJacobianType)

public:

	RTransformer(QObject *parent, QString name);
	virtual ~RTransformer();

	// Declaring the RJacobianType enum values
	enum RJacobianType{u_x, x_u};

	int setRandomVariableList(QList<RRandomVariable *> *passedRandomVariableList);

    // Establishes the correlation matrix and the computes the Cholesky decomposition at the start of the analysis
    virtual int initializeAtStartOfAnalysis() = 0;

    // Transforms random variable realizations from the standard normal to the original space, in the given threadID ($MTh$)
    virtual int transformPointToOriginalSpace(const gsl_vector *standardNormalVector, gsl_vector *originalVector, int threadID) = 0;

	// Transforms random variable realizations from the original to the standard normal space, in the given thradID ($MTh$)
    virtual int transformPointToStandardNormalSpace(const gsl_vector *originalVector, gsl_vector *standardNormalVector, int threadID) = 0;

    // Transforms a gradient vector from the standard normal to the original space, in the given thradID ($MTh$)
    virtual int transformGradientToOriginalSpace(const gsl_vector *pointInOriginalSpace, const gsl_vector *pointInStandardNormalSpace, const gsl_vector *vectorInStandardNormalSpace, gsl_vector *vectorInOriginalSpace, int threadID) = 0;

	// Transforms a gradient vector from the original to the standard normal space, in the given thradID ($MTh$)
    virtual int transformGradientToStandardNormalSpace(const gsl_vector *pointInOriginalSpace, const gsl_vector *pointInStandardNormalSpace, const gsl_vector *vectorInOriginalSpace, gsl_vector *vectorInStandardNormalSpace, int threadID) = 0;

	// Computes and returns the Jacobiam matrix, in the given thradID ($MTh$)
	virtual int computeJacobian(const gsl_vector *pointInOriginalSpace, const gsl_vector *pointInStandardNormalSpace, gsl_matrix *jacobian, RTransformer::RJacobianType jacobianType, int threadID) = 0;

	// Computes and returns the Jacobian matrix of the vector of standard normal random variables wrt. the means and stdvs of original random variables, in the given thradID ($MTh$)
	virtual int computeYSensitivityToDistributionParameters(const gsl_vector *pointInOriginalSpace, gsl_matrix *dy_dMean, gsl_matrix *dy_dStdv, int threadID, QList<RPointer<RRandomVariable> > sensitivityRandomVariableList = QList<RPointer<RRandomVariable> >()) = 0;

	// Sensitivity implementations
	// Runs the sensitivity analysis of joint PDF in original space wrt Means and Stdvs of random variables 
	virtual int computeJointPDFSensitivityToDistributionParameters(const gsl_vector *x, const gsl_vector *u,  QList<RPointer<RRandomVariable> > sensitivityRandomVariableList, gsl_vector *dfdMean_1_f, gsl_vector *dfdStdv_1_f, double NeglectCorrelationInSensitivity, int threadID) = 0;

	// Method for creating the helper transformer
	RPointer<RTransformer> createHelperTransformer();

	// overwriting theOutputDisplayLevel's setter to transfer changes to the helper transformer too
	void setOutputDisplayLevel(ROutputDisplayLevelType value);

protected:
	// Auxiliary method to compute the derivative of the uncorrelated standard normal variables (z) wrt. the means and stdvs of original random variables, in the given threadID ($MTh$)
	int computeDzDMeanStdv(RContinuousRandomVariable *randomVariable, double x, double z, double *DzDMean, double *DzDStdv, int threadID);

	// Auxiliary method to compute and return the derivative of Cholesky matrix wrt to Mean and stdv of a random variable, dL/dMean and dL/dStdv, by FDM & DDM, in the given thradID ($MTh$)
	virtual int computedLinvdMean(RContinuousRandomVariable *randomVariable, gsl_matrix *dLinvdMean, gsl_matrix *dLinvdStdv, bool DDM, int threadID) = 0;

	// Sensitivity implementations
	// calculates the derivative of a multivariate standard normal joint probability distribution wrt one of the variables (its number in vector is ui) at a certain poit (u)
	double dphi_dui(const gsl_vector *u, int ui);
	// calculates a multivariate standard normal joint probability distribution at a certain poit (u)
	double phi(const gsl_vector *u);

	// Returns a new Nataf Transformer
	virtual RPointer<RTransformer> createNewTransformerOfTheSameType() = 0;

	// Data members that are inherited by subclasses
	RDomain *theDomain;
	int randomVariablesCount;
	QList<RRandomVariable *> *randomVariableList;

private:
	
	// a helper transformer for handling the 2nd data series with the same properties as this transformer
	RPointer<RTransformer> theHelperTransformer;
};

#endif 
