#include "RNoCorrelationTransformer.h"

#include "RDomain.h"
#include "RContinuousRandomVariable.h"
#include "RCorrelation.h"

#include <gsl/gsl_blas.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_cdf.h>

#include <QCoreApplication>

#include <math.h>

RNoCorrelationTransformer::RNoCorrelationTransformer(QObject *parent, QString name)
	: RTransformer(parent, name)
{

}

RNoCorrelationTransformer::~RNoCorrelationTransformer()
{
	
}

int RNoCorrelationTransformer::initializeAtStartOfAnalysis()
{
	// Issue a warning if there are coorelation objects in the domain
	QList<RCorrelation *> correlationList = theDomain->findChildren<RCorrelation *>();
	if (!correlationList.isEmpty()) {
		rCritical() << "Warning: Correlations are neglected using the \"No Correlation\" transformer.";
	}

	// Does nothing. There is no need for the correlation matrix here.
	return 0;
}

int RNoCorrelationTransformer::transformPointToStandardNormalSpace(const gsl_vector *originalVector, gsl_vector *standardNormalVector, int threadID)
{
	RContinuousRandomVariable *randomVariable;
	for (int i = 0; i < randomVariablesCount; i++){
		randomVariable = qobject_cast<RContinuousRandomVariable *>(randomVariableList->value(i));
		double xi = gsl_vector_get(originalVector, i);
				
		double CDF = randomVariable->getCDFValue(xi, threadID);
		// Get the inverse CDF value of the variable "CDF" for the standard normal distribution using GSL
		double ui = gsl_cdf_gaussian_Pinv(CDF, 1.0) + 0.0;
		gsl_vector_set(standardNormalVector, i, ui);
	}

	// Print u
	if (getOutputDisplayLevel() >= RObject::Moderate) {
		rDebug(threadID) << "Current point in standard-normal space, u:";
		theDomain->print(standardNormalVector, threadID);
		rDebug(threadID) << "";
	}
	
	return 0;
}

int RNoCorrelationTransformer::transformPointToOriginalSpace(const gsl_vector *standardNormalVector, gsl_vector *originalVector, int threadID)
{
	RContinuousRandomVariable *randomVariable;
	for (int i = 0; i < randomVariablesCount; i++){
		randomVariable = qobject_cast<RContinuousRandomVariable *>(randomVariableList->value(i));
		double ui = gsl_vector_get(standardNormalVector, i);
	
		// Get the CDF value of zi for the standard normal distribution using GSL
		double CDF = gsl_cdf_gaussian_P(ui - 0.0, 1.0);
		double inverseCDF = randomVariable->getInverseCDFValue(CDF,threadID);
		gsl_vector_set(originalVector, i, inverseCDF);
	}

	// Print X
	if (getOutputDisplayLevel() >= RObject::Moderate) {
		rDebug(threadID) << "Current point in original space, x:";
		theDomain->print(originalVector, threadID);
		rDebug(threadID) << "";
	}
	return 0;
}

int RNoCorrelationTransformer::computeDiagonalJacobian_u_x_AsVector(const gsl_vector *originalVector, const gsl_vector *standardNormalVector, gsl_vector *jacobianV_u_x, int threadID)
{
	RContinuousRandomVariable *randomVariable;
	gsl_vector_set_zero(jacobianV_u_x);

	for (int i = 0; i < randomVariablesCount; i++){		
		randomVariable = qobject_cast<RContinuousRandomVariable *>(randomVariableList->value(i));

		double xi = gsl_vector_get(originalVector, i);		
		double ui = gsl_vector_get(standardNormalVector, i); 

		// Get the CDF value of ui for the standard normal distribution using GSL
		double PDF = gsl_ran_gaussian_pdf(ui - 0.0, 1.0);
		if (PDF == 0.0) {
			rCritical(threadID) << "Error: The PDF value of the random variable" << randomVariableList->value(i)->objectName() << "is zero, probably due to a too large step.";
			//return -1;
		}
		
		// Calculate jacobian_u_x
		gsl_vector_set(jacobianV_u_x, i, randomVariable->getPDFValue(xi,threadID) / PDF);
		//if (gsl_vector_get(jacobianV_u_x, i) == 0.0) {
		//	//????????
		//}
	}

	return 0;
}

int RNoCorrelationTransformer::computeJacobian(const gsl_vector *pointInOriginalSpace, const gsl_vector *pointInStandardNormalSpace, gsl_matrix *jacobian, RTransformer::RJacobianType jacobianType, int threadID)
{
	RContinuousRandomVariable *randomVariable;
	gsl_matrix_set_zero(jacobian);

	int size = pointInOriginalSpace->size;
	gsl_vector *jacobianV_u_x = gsl_vector_calloc(size);

	int result = computeDiagonalJacobian_u_x_AsVector(pointInOriginalSpace, pointInStandardNormalSpace, jacobianV_u_x, threadID);
	if (result < 0) {
		rCritical(threadID) << "Error: Could not compute the diagonal Jacobian matrix.";
		return -1;
	}

	for (int i = 0; i < randomVariablesCount; i++){
		double value = 0.0;
		if (jacobianType == RTransformer::u_x) {
			value = gsl_vector_get(jacobianV_u_x, i);
		} else if (jacobianType == RTransformer::x_u) {
			value = 1.0 / gsl_vector_get(jacobianV_u_x, i);
		}
		gsl_matrix_set(jacobian, i, i, value);
	}


	// Print jacobian
	if (getOutputDisplayLevel() >= RObject::Maximum) {
		QString jacobianName;
		if (jacobianType == RTransformer::u_x) {
			jacobianName = "du/dx";
		} else if (jacobianType == RTransformer::x_u) {
			jacobianName = "dx/du";
		}
		rDebug(threadID) << qPrintable(QString("Jacobian matrix of %1:").arg(jacobianName));
		theDomain->print(jacobian, threadID);
		rDebug(threadID) << "";
	}

	if (jacobianV_u_x) {
		gsl_vector_free(jacobianV_u_x);
		jacobianV_u_x = 0;
	}

	return 0;
}

int RNoCorrelationTransformer::transformGradientToOriginalSpace(const gsl_vector *pointInOriginalSpace, const gsl_vector *pointInStandardNormalSpace, const gsl_vector *vectorInStandardNormalSpace, gsl_vector *vectorInOriginalSpace, int threadID)
{
	int size = pointInOriginalSpace->size;
	gsl_vector *jacobianV_u_x = gsl_vector_calloc(size);

	int result = computeDiagonalJacobian_u_x_AsVector(pointInOriginalSpace, pointInStandardNormalSpace, jacobianV_u_x, threadID);
	if (result < 0) {
		rCritical(threadID) << "Error: Could not compute the diagonal Jacobian matrix.";
		return -1;
	}

	// Compute:  vectorInOriginalSpace = jacobian_u_x^T * vectorInStandardNormalSpace
	// Since the jacobian is calculated as a vector here, a simple vector multiplication is performed:
	gsl_vector_memcpy(vectorInOriginalSpace, vectorInStandardNormalSpace);
	gsl_vector_mul(vectorInOriginalSpace, jacobianV_u_x);

	if (jacobianV_u_x) {
		gsl_vector_free(jacobianV_u_x);
		jacobianV_u_x = 0;
	}
	return 0;
}

int RNoCorrelationTransformer::transformGradientToStandardNormalSpace(const gsl_vector *pointInOriginalSpace, const gsl_vector *pointInStandardNormalSpace, const gsl_vector *vectorInOriginalSpace, gsl_vector *vectorInStandardNormalSpace, int threadID)
{
	int size = pointInOriginalSpace->size;
	gsl_vector *jacobianV_u_x = gsl_vector_calloc(size);

	int result = computeDiagonalJacobian_u_x_AsVector(pointInOriginalSpace, pointInStandardNormalSpace, jacobianV_u_x, threadID);
	if (result < 0) {
		rCritical(threadID) << "Error: Could not compute the diagonal Jacobian matrix.";
		return -1;
	}

	// Compute:  vectorInStandardNormalSpace = jacobian_x_u^T * vectorInOriginalSpace
	// Since the jacobian is calculated as a vector here, a simple vector division is performed:
	gsl_vector_memcpy(vectorInStandardNormalSpace, vectorInOriginalSpace);
	gsl_vector_div(vectorInStandardNormalSpace, jacobianV_u_x);

	if (jacobianV_u_x) {
		gsl_vector_free(jacobianV_u_x);
		jacobianV_u_x = 0;
	}

	return 0;
}

int RNoCorrelationTransformer::computeYSensitivityToDistributionParameters(const gsl_vector *pointInOriginalSpace, gsl_matrix *dy_dMean, gsl_matrix *dy_dStdv, int threadID, QList<RPointer<RRandomVariable> > sensitivityRandomVariableList)
{
	// Create the "z-vector"
	int numRVs = pointInOriginalSpace->size;
	gsl_vector *z = gsl_vector_calloc(numRVs);
	int status = transformPointToStandardNormalSpace(pointInOriginalSpace, z, threadID);
	if (status < 0) {
		rCritical(threadID) << "Error: Cannot transform the vector of random variables to the standard normal space.";
		return -1;
	}

	gsl_vector *dzdMeanVector = gsl_vector_calloc(numRVs);
	gsl_vector *dzdStdvVector = gsl_vector_calloc(numRVs);


	// Loop over all random variables. The sensitivity of the y-vector to each random variable mean or stdv will be a row of the Jacobian matrix.
	for (int i = 0; i < numRVs; i++) {
		RContinuousRandomVariable *randomVariable = qobject_cast<RContinuousRandomVariable *>(randomVariableList->value(i));
		//gsl_vector_set(z, i, gsl_cdf_gaussian_Pinv(randomVariable->getCDFValue(gsl_vector_get(x, i)), 1.0));
		double xi = gsl_vector_get(pointInOriginalSpace, i);
		double zi = gsl_vector_get(z, i);
		
		
		
		// First, we need to compute (L^-1 * dzdMean) and (L^-1 * dzdStdv)
		// Note: L=I here because there is no correlation

		// Computing dzdMean and dzdStdv
		double dzdMean = 0.0;
		double dzdStdv = 0.0;
		status = computeDzDMeanStdv(randomVariable, xi, zi, &dzdMean, &dzdStdv,threadID);
		

		gsl_vector_set_zero(dzdMeanVector);
		gsl_vector_set_zero(dzdStdvVector);
		gsl_vector_set(dzdMeanVector, i, dzdMean);
		gsl_vector_set(dzdStdvVector, i, dzdStdv);

	

		// Second, we need to compute d(L^-1)/d(Mean) * z, which is equal to zero, because L=I and dL/dMean = 0


		// This will be the sensitivity with respect to the random variable i. This will form a row of the Jacobian matrix.

		// Setting the ith row of the Jacobian matrix
		gsl_matrix_set_row(dy_dMean, i, dzdMeanVector);
		gsl_matrix_set_row(dy_dStdv, i, dzdStdvVector);
	}

	// Feering the matrices and vectors
	if (z) {
		gsl_vector_free(z);
		z = 0;
	}
	if (dzdMeanVector) {
		gsl_vector_free(dzdMeanVector);
		dzdMeanVector = 0;
	}
	if (dzdStdvVector) {
		gsl_vector_free(dzdStdvVector);
		dzdStdvVector = 0;
	}

	return 0;
}

int RNoCorrelationTransformer::computedLinvdMean(RContinuousRandomVariable *randomVariable, gsl_matrix *dLinvdMean, gsl_matrix *dLinvdStdv, bool DDM, int threadID)
{
	gsl_matrix_set_zero(dLinvdMean);
	gsl_matrix_set_zero(dLinvdStdv);
	
	return 0;
}

int RNoCorrelationTransformer::computeJointPDFSensitivityToDistributionParameters(const gsl_vector *x, const gsl_vector *u,  QList<RPointer<RRandomVariable> > sensitivityRandomVariableList, gsl_vector *dfdMean_1_f, gsl_vector *dfdStdv_1_f, double NeglectCorrelationInSensitivity, int threadID)
{
	// This method computes (df/dMean)*(1/f) and (df/dStdv)*(1/f) where f is the joint probability distribution of 
	// all random variables in the original space and Mean and Stdv are the mean and standard deviation of the desired random variable.
	
	// Keep the GUI responsive
	if (threadID == 0)
		QCoreApplication::processEvents();

	// Compute df/dMean & df/dStdv
	double dfdMean = 0.0;
	double dfdStdv = 0.0;
	for (int i = 0; i < sensitivityRandomVariableList.count(); i++) {
			
		RContinuousRandomVariable *RV = qobject_cast<RContinuousRandomVariable *>(sensitivityRandomVariableList[i].data());
		
		// Compute f: Value of marginal PDF in original space at x
		double f = RV -> getPDFValue(RV->getCurrentValue(threadID), threadID);

		// Compute f: Value of marginal PDF sensitivity in original space at x
		int status = RV -> computePDFSensitivityToParameters(RV->getCurrentValue(threadID), &dfdMean, &dfdStdv, threadID);
		if (status) {
			rCritical(threadID) << "Error in computing PDF sensitivity in Sampling sensitivity computation";
			return -1;
		}
		
		double dCost_dMean = RV->getIncrementalCostOfMeanValue(threadID);
		double dCost_dStdv = RV->getIncrementalCostOfStandardDeviationValue(threadID);

		gsl_vector_set(dfdMean_1_f, i, (dfdMean / f)/dCost_dMean);
		gsl_vector_set(dfdStdv_1_f, i, (dfdStdv / f)/dCost_dStdv);

	}

	return 0;
}

RPointer<RTransformer> RNoCorrelationTransformer::createNewTransformerOfTheSameType(){
	return new RNoCorrelationTransformer(theDomain, QString());
}

bool RNoCorrelationTransformer::canBeRunParallelly(){
	return true;
}