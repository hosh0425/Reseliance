// Last modified by Mojtaba on March 19, 2014

#include "RFOSMAnalyzer.h"

#include <QMap>
#include <QSet>
#include <QDateTime>
#include <QApplication>

#include <gsl/gsl_blas.h>

#include "RMainWindow.h"
#include "RFlowchartWidget.h"
#include "RDomain.h"
#include "RFunction.h"
#include "RResponse.h"
#include "RModel.h"
#include "RContinuousRandomVariable.h"
#include "RCorrelation.h"

RFOSMAnalyzer::RFOSMAnalyzer(QObject *parent, QString name)
	: RAnalyzer(parent, name)
{
	//$$ theFunction = 0;
	thePrintCorrelationMatrix = false;
	thePrintCovarianceMatrix = false;
}

RFOSMAnalyzer::~RFOSMAnalyzer()
{

}

QObject * RFOSMAnalyzer::getFunction() const	
{	
	return theFunction.data();
}	
	
void RFOSMAnalyzer::setFunction(QObject *value)	
{	
	theFunction = qobject_cast<RFunction *>(value);
}	
	
bool RFOSMAnalyzer::getPrintCorrelationMatrix() const	
{	
	return thePrintCorrelationMatrix;
}	
	
void RFOSMAnalyzer::setPrintCorrelationMatrix(bool value)	
{	
	thePrintCorrelationMatrix = value;
}	

bool RFOSMAnalyzer::getPrintCovarianceMatrix() const	
{	
	return thePrintCovarianceMatrix;
}	
	
void RFOSMAnalyzer::setPrintCovarianceMatrix(bool value)	
{	
	thePrintCovarianceMatrix = value;
}	

int RFOSMAnalyzer::analyze()
{
	// Inform the user that the analysis has started
	rDebug() << "##### STARTING FOSM ANALYSIS WITH FUNCTION" << theFunction.data()->objectName() << "...";
	rDebug() << "";

	QDateTime startTime = QDateTime::currentDateTime();


	if (!theFunction) {
		rCritical() << "Error in" << objectName() << ": Please choose a function and try again";
		return -1;
	}

	if (qobject_cast<QApplication*>(QCoreApplication::instance()) != 0) {
		theMainWindow->raiseDock("Output");
	}

	// Parse the passed function
	int result = theFunction.data()->populateLists();
	if (result < 0) {
		rCritical() << "Error: Could not parse the function" << theFunction.data()->objectName();
		return -1;
	}

	QList<RRandomVariable *> randomVariableList = (*theFunction.data()->getRandomVariableList());

    // Get number of random variables
	int numRVs = randomVariableList.count();
	    
	// Check that the random variable list has been populated
	if (numRVs == 0) {
        return -1;
    }
	
	// Backing up the currentValue of the random variables
	QVector<double> randomVariablesBackup;
	for (int i = 0; i < numRVs; i++) {
		randomVariablesBackup.append(randomVariableList[i]->getCurrentValue());
	}
	
	// Setting the current value of all the random variables to their mean
	for (int i = 0; i < numRVs; i++) {
		randomVariableList[i]->setCurrentValue(randomVariableList[i]->getMeanValue(0));
	}
	

	// Evaluate the function at the mean
	theFunction.data()->updateAnalysisStatus(0);
	if (theFunction.data()->getGradientAnalysisType() == RFunction::DDM) {
		result = theFunction.data()->evaluateFunction(true, RObject::RandomVariable,0);
	} else {
		result = theFunction.data()->evaluateFunction(false, RObject::RandomVariable,0);
	}	
	if (result < 0) {
		return -1;
	}
	double mean = theFunction.data()->getFunctionValue();

	// Evaluate the gradient
	gsl_vector *gradientVector = 0;
	result = theFunction.data()->evaluateGradient(RObject::RandomVariable);	
	if (result < 0) {
		return -2;
	}
	gradientVector = theFunction.data()->getGradient();

	
	
	// Comupte the covariance matrix

	// Establishing the D matrix
    gsl_matrix *DMatrix = gsl_matrix_calloc(numRVs, numRVs);
    for (int i = 0; i < numRVs; i++) {
		gsl_matrix_set(DMatrix, i, i, randomVariableList[i]->getStandardDeviationValue(0));
	}

    // Initialize the correlation matrix
    gsl_matrix *RMatrix = gsl_matrix_calloc(numRVs, numRVs);
    gsl_matrix_set_identity(RMatrix);

    // Set the correlation coefficients that the user has given
    QList<RCorrelation *> correlationList = theDomain->findChildren<RCorrelation *>();
    RRandomVariable *rv1, *rv2;
    int index1, index2;
    double rho;
    for (int i = 0; i < correlationList.count(); i++) {

        // Get the two random variable objects
        rv1 = qobject_cast<RRandomVariable *>(correlationList[i]->getRandomVariable1());
        rv2 = qobject_cast<RRandomVariable *>(correlationList[i]->getRandomVariable2());

        // Do nothing if one or both of the random variables are not subject to this analysis
        if ((!rv1) || (!rv2)) {
            rCritical() << "Warning: Two random variables must be given to the correlation object" << correlationList[i]->objectName();
            continue;
        }

        // Do nothing if the random variable wasn't found in the random variable list
        index1 = randomVariableList.indexOf(rv1);
        index2 = randomVariableList.indexOf(rv2);
        if ((index1 < 0) || (index2 < 0)) {
            continue;
        }

        // Get the correlation value
        rho = correlationList[i]->getCoefficient();


        // Set the correlation coefficient into the matrix
        gsl_matrix_set(RMatrix, index1, index2, rho);
        gsl_matrix_set(RMatrix, index2, index1, rho);
    }
	// Compute the sigmaMatrix
    gsl_matrix *sigmaMatrix = gsl_matrix_calloc(numRVs, numRVs);
	gsl_matrix *tempMatrix = gsl_matrix_calloc(numRVs, numRVs);
	// Compute using BLAS: tempMatrix = RMatrix * DMatrix
	gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, RMatrix, DMatrix, 0.0, tempMatrix);
	// Compute using BLAS: sigmaMatrix = DMatrix * tempMatrix
	gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, DMatrix, tempMatrix, 0.0, sigmaMatrix);


	// Compute the standard deviation
	gsl_vector *tempVector = gsl_vector_calloc(numRVs);
	// Compute using BLAS:  tempVector = sigmaMatrix * gradientVector
	gsl_blas_dgemv(CblasNoTrans, 1.0, sigmaMatrix, gradientVector, 0.0, tempVector);
	// Compute using BLAS:  stdv = gradientVector . tempVector
	double stdv;
	gsl_blas_ddot(gradientVector, tempVector, &stdv);
	stdv = sqrt(stdv);



	// Compute FOSM importance measure: dg/dx * stdv
	gsl_vector *FOSMImportanceVector = gsl_vector_calloc(numRVs);
	for (int i = 0; i < numRVs; i++) {
		double stdv = randomVariableList[i]->getStandardDeviationValue(0);
		gsl_vector_set(FOSMImportanceVector, i, gsl_vector_get(gradientVector, i) * stdv);
		RContinuousRandomVariable *cRV = qobject_cast<RContinuousRandomVariable *>(randomVariableList[i]);
		cRV->setFOSMImportanceMeasure(gsl_vector_get(gradientVector, i) * stdv);
	}

	// Normalizing the inportance vector
	double norm = gsl_blas_dnrm2(FOSMImportanceVector);

	// Contrary to OpenSees, this importance measure is computed as -dg/dx*sigma, because the NEGATIVE sign will make the sign of the elements similar to alpha and gamma vectors
	gsl_vector_scale(FOSMImportanceVector, -1.0 / norm);

	// Assigning to random variables
	for (int i = 0; i < numRVs; i++) {
		RContinuousRandomVariable *cRV = qobject_cast<RContinuousRandomVariable *>(randomVariableList[i]);
		cRV->setFOSMImportanceMeasure(gsl_vector_get(FOSMImportanceVector, i));
	}
	


	rDebug() << "Function mean = " << mean;
	rDebug() << "Function standard deviation = " << stdv;
	rDebug() << "FOSM Beta = " << mean / stdv;
	rDebug() << "";

	if (thePrintCorrelationMatrix) {
		rDebug() << "Correlation matrix:";
		theDomain->print(RMatrix, 0);
		rDebug() << "";
	}
	if (thePrintCovarianceMatrix) {
		rDebug() << "Covariance matrix:";
		theDomain->print(sigmaMatrix, 0);
		rDebug() << "";
	}


	// Free temporary matrices and vectors
	if (RMatrix) {
		gsl_matrix_free(RMatrix);
		RMatrix = 0;
	}
	if (DMatrix) {
		gsl_matrix_free(DMatrix);
		DMatrix = 0;
	}
	if (sigmaMatrix) {
		gsl_matrix_free(sigmaMatrix);
		sigmaMatrix = 0;
	}
	if (tempMatrix) {
		gsl_matrix_free(tempMatrix);
		tempMatrix = 0;
	}
	if (tempVector) {
		gsl_vector_free(tempVector);
		tempVector = 0;
	}
	if (FOSMImportanceVector) {
		gsl_vector_free(FOSMImportanceVector);
		FOSMImportanceVector = 0;
	}


	QDateTime finishTime = QDateTime::currentDateTime();
	double numDays = startTime.daysTo(finishTime);
	double numMilliSecs = startTime.time().msecsTo(finishTime.time());
	double numSecs = numDays * 86400.0 + numMilliSecs / 1000.0;

	rDebug() << "##### FOSM ANALYSIS DONE IN" << numSecs << "SECONDS.";
	rDebug() << "";
	rDebug() << "";

	// Restoring the currentValue of the random variables from the storedValue
	for (int i = 0; i < numRVs; i++) {
		randomVariableList[i]->setCurrentValue(randomVariablesBackup[i]);
	}



	return 0;
}

bool RFOSMAnalyzer::canBeRunParallelly() {
	return false;
}