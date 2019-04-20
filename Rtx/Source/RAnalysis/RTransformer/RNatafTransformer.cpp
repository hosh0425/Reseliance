// Last modified by Mojtaba on March 19, 2014

#include "RNatafTransformer.h"

#include <QCoreApplication>

#include "RDomain.h"
#include "RRandomVariable.h"
#include "RContinuousRandomVariable.h"
#include "RCorrelation.h"
#include "sparse.h"

#include <gsl/gsl_blas.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_cdf.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_integration.h>
#include <gsl/gsl_sf_pow_int.h>

#include <math.h>

#include <QMutex>
#include <QMutexLocker>

RNatafTransformer::RNatafTransformer(QObject *parent, QString &name)
		: RTransformer(parent, name)
{
	numRVs = 0;

	// allocating memory for array of matrices
	int arraySizeForParallelizedObject = getArraySizeForParallelizedObject();
	L = new gsl_matrix *[arraySizeForParallelizedObject];
	Linv = new gsl_matrix *[arraySizeForParallelizedObject];
	R0 = new gsl_matrix *[arraySizeForParallelizedObject];

	// initializing matrices of all indices to 0
	for (int thID = 0; thID < arraySizeForParallelizedObject; thID++){
		L[thID] = 0;
		Linv[thID] = 0;
		R0[thID] = 0;
	}

	theSparseCorrelationMatrix = false;
	safeLinvdMapLock = new QMutex();
	linvdMapsHaveBeenFilled = false;
}

RNatafTransformer::~RNatafTransformer()
{

	int arraySizeForParallelizedObject = getArraySizeForParallelizedObject();

	for(int thID = 0; thID < arraySizeForParallelizedObject; thID++){

		if (L[thID]) {
			gsl_matrix_free(L[thID]);
			L[thID] = 0;
		}
		if (Linv[thID]) {
			gsl_matrix_free(Linv[thID]);
			Linv[thID] = 0;
		}
		if (R0[thID]) {
			gsl_matrix_free(R0[thID]);
			R0[thID] = 0;
		}
	}

	delete[] L;
	delete[] Linv;
	delete[] R0;

	// Sensitivity implementations
	freeDLinvMaps();

	delete safeLinvdMapLock;
}

bool RNatafTransformer::getSparseCorrelationMatrix() const
{
	return theSparseCorrelationMatrix;
}

void RNatafTransformer::setSparseCorrelationMatrix(bool value)
{
	theSparseCorrelationMatrix = value;
}

int RNatafTransformer::initializeAtStartOfAnalysis()
{
	// Check that the random variable list has been populated
	if (!randomVariableList) {
		return -1;
	}


	// Get number of random variables
	numRVs = randomVariableList->count();


	// Instantiate the Cholesky matrix
	if (L[0]) {
		gsl_matrix_free(L[0]);
		L[0] = 0;
	}
	L[0] = gsl_matrix_calloc(numRVs, numRVs);

	// Instantiate the inverse Cholesky matrix
	if (Linv[0]) {
		gsl_matrix_free(Linv[0]);
		Linv[0] = 0;
	}
	Linv[0] = gsl_matrix_calloc(numRVs, numRVs);

	// Initialize the correlation matrix
	if (R0[0]) {
		gsl_matrix_free(R0[0]);
		R0[0] = 0;
	}
	R0[0] = gsl_matrix_calloc(numRVs, numRVs);

	// Sensitivity implementations
	freeDLinvMaps();

	// Establish the correlation matrix R0

	// First set R0 to the Identity matrix
	gsl_matrix_set_identity(R0[0]);

	int status  = establishCorrelationMatrix(R0[0], 0);
	if (status < 0) {
		rCritical() << "Error: Cannot establish the correlation matrix.";
		return -1;
	}

	// Print the correlation matrix if the user asked for it
	if (getOutputDisplayLevel() >= RObject::Maximum) {
		rDebug() << "The correlation matrix:";
		theDomain->print(R0[0], 0);
		rDebug() << " ";
	}

	// Compute cholesky using GSL
	status = computeCholesky(R0[0], L[0], 0, Linv[0]);
	if (status < 0) {
		rCritical() << "Error: Cannot compute the Cholesky decomposition of the correlation matrix or its inverse.";
		return -1;
	}

	return 0;
}

int RNatafTransformer::establishCorrelationMatrix(gsl_matrix *R0, int threadID, RRandomVariable *perturbedRV)
{
	// Set the correlation coefficients that the user has given
	QList<RCorrelation *> correlationList = theDomain->findChildren<RCorrelation *>();
	RRandomVariable *rv1;
	RRandomVariable *rv2;
	int index1;
	int index2;
	double rho;
	double rho0;
	for (int i = 0; i < correlationList.count(); i++) {

		// Get the two random variable objects
		rv1 = qobject_cast<RRandomVariable *>(correlationList[i]->getRandomVariable1());
		rv2 = qobject_cast<RRandomVariable *>(correlationList[i]->getRandomVariable2());

		
		// If this correlation matrix is being established as a result of a perturbation for a sensitivity analysis, then check if one of rv1 or rv2 is the perturbed one; otherwise, continue.
		if (perturbedRV) {
			if (rv1 != perturbedRV && rv2 != perturbedRV) {
				continue;
			}
		}



		// Do nothing if one or both of the random variables are not subject to this analysis
		if ((!rv1) || (!rv2)) {
			rCritical(threadID) << "Warning: Two random variables must be given to the correlation object" << correlationList[i]->objectName() << ".";
			continue;
		}


		// Do nothing if the random variable wasn't found in the random variable list
		index1 = randomVariableList->indexOf(rv1);
		index2 = randomVariableList->indexOf(rv2);
		if ((index1 < 0) || (index2 < 0)) {
			continue;
		}


		// Get the correlation value
		rho = correlationList[i]->getCoefficient();


		// Modify the correlation coefficient
		rho0 = modifyCorrelation(rv1, rv2, rho, threadID);


		// Print the correlation matrix if the user asked for it
		if (getOutputDisplayLevel() >= RObject::Moderate && !perturbedRV) {
			rDebug(threadID) << "The correlation coefficient" << correlationList[i]->objectName() << "is changed from" << rho << "to" << rho0;
		}


		// Set the correlation coefficient into the matrix
		gsl_matrix_set(R0, index1, index2, rho0);
		gsl_matrix_set(R0, index2, index1, rho0);
	}

	return 0;
}

int RNatafTransformer::computeCholesky(const gsl_matrix *R0, gsl_matrix *cholesky, int threadID, gsl_matrix *choleskyInverse)
{
	if (theSparseCorrelationMatrix) {
		// Computie Cholesky using the Sparse library
		int NumOfArraysInLowerTriangularPart = (gsl_pow_2(numRVs)-numRVs)/2 + numRVs;
		double *vals = new double[NumOfArraysInLowerTriangularPart];
		int *rowInds = new int[NumOfArraysInLowerTriangularPart];
		int *colPtr = new int[numRVs+1];
		colPtr[0] = 0;
		int count = 0;
		for (int i = 0; i < numRVs; i++) {
			for (int j = (0+i); j < numRVs; j++) {
				rowInds[count] = j;
				vals[count] = gsl_matrix_get(R0, j, i);
				count++;
			}
			colPtr[i+1] = count;
		}
		SparseMatrix *sparseR0 = new SparseMatrix(numRVs, numRVs, vals, rowInds, colPtr);
		SparseMatrix *sparseCholesky = sparseR0->cholesky_decompose_lower_triangular_returning_lower_triangular();

		for (int i = 0; i < numRVs; i++) {
			for (int j = 0; j < numRVs; j++) {
				gsl_matrix_set(cholesky,i,j,(*sparseCholesky)(i, j));
			}
		}
		
		if (choleskyInverse) {
			SparseMatrix *inverseSparseCholesky = sparseCholesky->invert_lower_triangular_cholesky_decomposed_returning_lower_triangular();

			//transform it to gsl
			for (int i = 0; i < numRVs; i++) {
				for (int j = 0; j < numRVs; j++) {
					gsl_matrix_set(choleskyInverse,i,j,(*inverseSparseCholesky)(i, j));
				}
			}
			delete inverseSparseCholesky;
		}

		delete sparseR0;
		delete sparseCholesky;

	} else {

		// Start with a copy of the correlation matrix
		gsl_matrix_memcpy(cholesky, R0);


		/*THERE IS NO NEED TO CHECK THE DETERMINANT TO SEE IF THE MATRIX IS POSITIVE-DEFINITE BECAUSE GSL WILL CHECK AND RETURNS AN ERROR IF IT IS NOT.
		// Take a copy of L to avoid messing it up in the following computations
		gsl_matrix *Lcopy = gsl_matrix_calloc(numRVs, numRVs);
		gsl_matrix_memcpy(Lcopy, cholesky);


		// Check that the Cholesky deomposition is possible
		int signum = 0;
		gsl_permutation *p = gsl_permutation_alloc(numRVs);

		int status = gsl_linalg_LU_decomp(Lcopy, p, &signum);
		if (status) { 
			rCritical() << "Error in GSL:" << gsl_strerror(status);
			return -1;
		}  

		double determinant = gsl_linalg_LU_det(Lcopy, signum);
		if (qAbs(determinant) < 0.00001) {
			rCritical() << "Error: The correlation matrix is not positive definite.";
			return -1;
		}
		gsl_permutation_free(p);


		if (Lcopy) {
			gsl_matrix_free(Lcopy);
			Lcopy = 0;
		}*/

		
		// Perform the Cholesky decomposition on the correlation matrix
		int status = gsl_linalg_cholesky_decomp(cholesky);	
		if (status) { 
			rCritical(threadID) << "Error in GSL:" << gsl_strerror(status);
			return -1;
		}  

		// Set the upper triangle of both cholesky and choleskyInverse equal to zero, as it should be
		for (int i = 0; i < cholesky->size1; i++) {
			for (int j = i+1; j < cholesky->size1; j++) {
				gsl_matrix_set(cholesky, i, j, 0.0);
			}
		}

		if (choleskyInverse) {
			// Take a copy of cholesky to avoid messing it up in the following computations
			gsl_matrix *Lcopy = gsl_matrix_calloc(numRVs, numRVs);
			gsl_matrix_memcpy(Lcopy, cholesky);

			int signum2 = 0;
			gsl_permutation *p = gsl_permutation_alloc(numRVs);

			status = gsl_linalg_LU_decomp(Lcopy, p, &signum2);
			if (status) { 
				rCritical(threadID) << "Error in GSL:" << gsl_strerror(status);
				return -1;
			}

			status = gsl_linalg_LU_invert(Lcopy, p, choleskyInverse);
			if (status) { 
				rCritical(threadID) << "Error in GSL:" << gsl_strerror(status);
				return -1;
			} 

			gsl_permutation_free(p);

			// Free vectors and matrices used in this method
			if (Lcopy) {
				gsl_matrix_free(Lcopy);
				Lcopy = 0;
			}


			/*// We thought the  method below would give the inverse of Cholesky, but it turned out that it gives the inverse of the coorelation matrix
			gsl_matrix_memcpy(choleskyInverse, cholesky);
			status = gsl_linalg_cholesky_invert(choleskyInverse);
			if (status < 0) {
				rCritical() << "Error in GSL:" << gsl_strerror(status);
				return -1;
			}*/
		}
	}

	/*// Writing Cholesky inverse in a text file
	QString fileName = QString("Inverse of Cholesky.txt");
	QFile *output = new QFile(fileName);
	if (output->exists()) {
		output->remove();
	}
	if (!output->open(QIODevice::WriteOnly | QIODevice::Text)) {
		rCritical() << "Warning: Could not create the file" << fileName;
	}

	QString tempString;
	for (int i = 0; i < numRVs; i++) {
		for (int j = 0; j < numRVs; j++) {
			tempString += QString("%1 ").arg(gsl_matrix_get(choleskyInverse,i,j), 0, 'E', 10);
		}
		tempString += QString("\n");
	}
	QByteArray byteArray;
	byteArray.append(tempString);
	output->write(byteArray);
	output->flush();
	output->close();
	delete output;*/

	return 0;
}


int RNatafTransformer::xToz(const gsl_vector *x, gsl_vector *z, int threadID) 
{
	// Loop through the random variables to create the "z-vector"
	RRandomVariable *randomVariable;
	for (int i = 0; i < numRVs; i++) {
		randomVariable = qobject_cast<RRandomVariable *>(randomVariableList->value(i));
		gsl_vector_set(z, i, gsl_cdf_gaussian_Pinv(randomVariable->getCDFValue(gsl_vector_get(x, i), threadID), 1.0));
	}

	return 0;
}

int RNatafTransformer::yToz(const gsl_vector *y, gsl_vector *z, int threadID) 
{
	// Using GSL, carry out the multiplication z = L*y for the trianular matrix L
	//gsl_blas_dgemv(CblasNoTrans, 1.0, L, y, 0.0, z);
	int status = gsl_vector_memcpy(z, y);
	if (status) { 
		rCritical(threadID) << "Error in GSL when copying a matrix into another:" << gsl_strerror(status);
		return -1;
	}  

	status = gsl_blas_dtrmv(CblasLower, CblasNoTrans, CblasNonUnit, L[threadID], z);
	if (status) { 
		rCritical(threadID) << "Error in GSL:" << gsl_strerror(status);
		return -1;
	}  
	return 0;
}

int RNatafTransformer::transformPointToStandardNormalSpace(const gsl_vector *originalVector, gsl_vector *standardNormalVector, int threadID)
{
	// Make sure that the "initialize" method has been called
	if (numRVs == 0) {
		rCritical(threadID) << "ERROR: The transformer has been called to carry out a task without having any random variables or without being initialized.";
	}

	// Create the "z-vector"
	gsl_vector *z = gsl_vector_calloc(numRVs);
	int status = xToz(originalVector, z,threadID);
	if (status < 0) {
		rCritical(threadID) << "Error: Cannot transform the vector of random variables to the standard normal space.";
		return -1;
	}


	// Compute the product standardNormalVector = L^-1 * z for the trianular matrix L^-1
	//gsl_blas_dgemv(CblasNoTrans, 1.0, Linv, z, 0.0, standardNormalVector);
	
	status = gsl_vector_memcpy(standardNormalVector, z);
	if (status) { 
		rCritical(threadID) << "Error in GSL when copying a matrix into another:" << gsl_strerror(status);
		return -1;
	}  

	status = gsl_blas_dtrmv(CblasLower, CblasNoTrans, CblasNonUnit, Linv[threadID], standardNormalVector);
	if (status) { 
		rCritical(threadID) << "Error in GSL:" << gsl_strerror(status);
		return -1;
	}


	// Free vectors and matrices used in this method
	gsl_vector_free(z);


	// Print standardNormalVector if the user calls for it
	if (getOutputDisplayLevel() >= RObject::Moderate) {
		rDebug(threadID) << "Current point in standard-normal space, u:";
		theDomain->print(standardNormalVector, threadID);
		rDebug(threadID) << "";
	}

	return 0;
}

int RNatafTransformer::transformPointToOriginalSpace(const gsl_vector *standardNormalVector, gsl_vector *originalVector, int threadID)
{
	// Make sure that the "initialize" method has been called
	if (numRVs == 0) {
		rCritical(threadID) << "ERROR: The transformer has been called to carry out a task without having any random variables or without being initialized.";
	}


	// z = L*y 
	gsl_vector *z = gsl_vector_calloc(numRVs);
	int status = yToz(standardNormalVector, z, threadID);
	if (status < 0) {
		rCritical(threadID) << "Error: Cannot transform the vector of stanadrd normal random variables to the original space.";
		return -1;
	}
 


	// Loop through the random variables to create the vector in the original space
	RRandomVariable *randomVariable;
	for (int i = 0; i < numRVs; i++) {
		randomVariable = qobject_cast<RRandomVariable *>(randomVariableList->value(i));
		gsl_vector_set(originalVector, i, randomVariable->getInverseCDFValue(gsl_cdf_gaussian_P(gsl_vector_get(z,i), 1.0 ), threadID));
	}


	// Print the original vector if the user asks for it
	if (getOutputDisplayLevel() >= RObject::Moderate) {
		rDebug(threadID) << "Current point in original space, x:";
		theDomain->print(originalVector,threadID);
		rDebug(threadID) << "";
	}


	// Free vectors and matrices used in this method
	gsl_vector_free(z);

	return 0;
}

int RNatafTransformer::transformGradientToOriginalSpace(const gsl_vector *pointInOriginalSpace, const gsl_vector *pointInStandardNormalSpace, const gsl_vector *vectorInStandardNormalSpace, gsl_vector *vectorInOriginalSpace, int threadID)
{
	// Make sure that the "initialize" method has been called
	if (numRVs == 0) {
		rCritical(threadID) << "ERROR: The transformer has been called to carry out a task without having any random variables or without being initialized.";
	}

	
	// z = L*y 
	gsl_vector *z = gsl_vector_calloc(numRVs);
	int status = yToz(pointInStandardNormalSpace, z, threadID);
	if (status < 0) {
		rCritical(threadID) << "Error: Cannot transform the vector of stanadrd normal random variables to the original space.";
		return -1;
	}


	// Establish the matrix diag(f/phi)
	RRandomVariable *randomVariable;
	gsl_matrix *diag = gsl_matrix_calloc(numRVs, numRVs);
	double phi, f;
	for (int i = 0; i < numRVs; i++) {
		randomVariable = qobject_cast<RRandomVariable *>(randomVariableList->value(i));
		phi = gsl_ran_gaussian_pdf(gsl_vector_get(z, i) , 1.0);
		f = randomVariable->getPDFValue(gsl_vector_get(pointInOriginalSpace,i),threadID);
		gsl_matrix_set(diag, i, i, f/phi );
	}


	// Compute the vector-matrix product temp = L^-1 * dGdy for the trianular matrix L^-1
	gsl_vector *temp = gsl_vector_calloc(numRVs);
	//gsl_blas_dgemv(CblasTrans, 1.0, Linv, vectorInStandardNormalSpace, 0.0, temp);
	status = gsl_vector_memcpy(temp, vectorInStandardNormalSpace);
	if (status) { 
		rCritical(threadID) << "Error in GSL when copying a matrix into another:" << gsl_strerror(status);
		return -1;
	} 
	
	status = gsl_blas_dtrmv(CblasLower, CblasTrans, CblasNonUnit, Linv[threadID], temp);
	if (status) { 
		rCritical(threadID) << "Error in GSL:" << gsl_strerror(status);
		return -1;
	}


	// Finalize the product vectorInOriginalSpace = diag(f/phi) * L^-1 * dGdy = diag(f/phi) * temp
	//gsl_blas_dgemv(CblasTrans, 1.0, diag, temp, 0.0, vectorInOriginalSpace);
	status = gsl_vector_memcpy(vectorInOriginalSpace, temp);
	if (status) { 
		rCritical(threadID) << "Error in GSL when copying a matrix into another:" << gsl_strerror(status);
		return -1;
	} 

	status = gsl_blas_dtrmv(CblasLower, CblasTrans, CblasNonUnit, diag, vectorInOriginalSpace);
	if (status) { 
		rCritical(threadID) << "Error in GSL:" << gsl_strerror(status);
		return -1;
	}


	// Free vectors and matrices used in this method
	gsl_vector_free(temp);
	gsl_vector_free(z);

	return 0;
}

int RNatafTransformer::transformGradientToStandardNormalSpace(const gsl_vector *pointInOriginalSpace, const gsl_vector *pointInStandardNormalSpace, const gsl_vector *vectorInOriginalSpace, gsl_vector *vectorInStandardNormalSpace, int threadID)
{
	// Make sure that the "initialize" method has been called
	if (numRVs == 0) {
		rCritical(threadID) << "ERROR: The transformer has been called to carry out a task without having any random variables or without being initialized.";
	}


	// z = L*y 
	gsl_vector *z = gsl_vector_calloc(numRVs);
	int status = yToz(pointInStandardNormalSpace, z, threadID);
	if (status < 0) {
		rCritical(threadID) << "Error: Cannot transform the vector of stanadrd normal random variables to the original space.";
		return -1;
	}


	// Establish the matrix diag(phi/f)
	RRandomVariable *randomVariable;
	gsl_matrix *diag = gsl_matrix_calloc(numRVs, numRVs);
	double phi, f;
	for (int i = 0; i < numRVs; i++) {
		randomVariable = qobject_cast<RRandomVariable *>(randomVariableList->value(i));
		phi = gsl_ran_gaussian_pdf(gsl_vector_get(z, i) , 1.0);
		f = randomVariable->getPDFValue(gsl_vector_get(pointInOriginalSpace, i),threadID);
		gsl_matrix_set(diag, i, i, phi/f );
	}


	// Compute the vector-matrix product temp = diag * dgdx = diag * vectorInOriginalSpace
	gsl_vector *temp = gsl_vector_calloc(numRVs);
	//gsl_blas_dgemv(CblasTrans, 1.0, diag, vectorInOriginalSpace, 0.0,  temp);
	status = gsl_vector_memcpy(temp, vectorInOriginalSpace);
	if (status) { 
		rCritical(threadID) << "Error in GSL when copying a matrix into another:" << gsl_strerror(status);
		return -1;
	}
	
	status = gsl_blas_dtrmv(CblasLower, CblasTrans, CblasNonUnit, diag, temp);
	if (status) { 
		rCritical(threadID) << "Error in GSL:" << gsl_strerror(status);
		return -1;
	}


	// Finalize the product vectorInStandardNormalSpace = L * diag * dgdx = L * temp
	//gsl_blas_dgemv(CblasTrans, 1.0, L, temp, 0.0, vectorInStandardNormalSpace);
	status = gsl_vector_memcpy(vectorInStandardNormalSpace, temp);
	if (status) { 
		rCritical(threadID) << "Error in GSL when copying a matrix into another:" << gsl_strerror(status);
		return -1;
	}

	status = gsl_blas_dtrmv(CblasLower, CblasTrans, CblasNonUnit, L[threadID], vectorInStandardNormalSpace);
	if (status) { 
		rCritical(threadID) << "Error in GSL:" << gsl_strerror(status);
		return -1;
	}


	// Free vectors and matrices used in this method
	gsl_vector_free(temp);
	gsl_vector_free(z);

	return 0;
}

int RNatafTransformer::computeJacobian(const gsl_vector *pointInOriginalSpace, const gsl_vector *pointInStandardNormalSpace, gsl_matrix *jacobian, RTransformer::RJacobianType jacobianType, int threadID)
{
	// Make sure that the "initialize" method has been called
	if (numRVs == 0) {
		rCritical(threadID) << "ERROR: The transformer has been called to carry out a task without having any random variables or without being initialized.";
	}


	// z = L*y 
	gsl_vector *z = gsl_vector_calloc(numRVs);
	int status = yToz(pointInStandardNormalSpace, z, threadID);
	if (status < 0) {
		rCritical(threadID) << "Error: Cannot transform the vector of stanadrd normal random variables to the original space.";
		return -1;
	}


	// Establish the matrix diag(phi/f)
	RRandomVariable *randomVariable;
	gsl_matrix *diag = gsl_matrix_calloc(numRVs, numRVs);
	double phi, f;
	for (int i = 0; i < numRVs; i++) {
		randomVariable = qobject_cast<RRandomVariable *>(randomVariableList->value(i));
		phi = gsl_ran_gaussian_pdf(gsl_vector_get(z,i) , 1.0);
		f = randomVariable->getPDFValue(gsl_vector_get(pointInOriginalSpace,i),threadID);

		double value = 0.0;
		if (jacobianType == RTransformer::u_x) {
			value = f / phi;
		} else if (jacobianType == RTransformer::x_u) {
			value = phi / f;
		}
		gsl_matrix_set(diag, i, i, value);
	}


	// Compute the matrix product diag*L
	if (jacobianType == RTransformer::u_x) {
		gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, Linv[threadID], diag, 0.0, jacobian);
	} else if (jacobianType == RTransformer::x_u) {
		gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, diag, L[threadID], 0.0, jacobian);
	}


	/*No need to calculate the inverse, the other jacobian is not evaluated here
	// Take a copy of the jacobian to avoid messing it up in the following computations
	gsl_matrix *Jxucopy = gsl_matrix_calloc(numRVs, numRVs);
	gsl_matrix_memcpy(Jxucopy, jacobian_x_u);
	// Obtain the other jacobian by inverting
	int signum = 0;
	gsl_permutation *p = gsl_permutation_alloc(numRVs);
	gsl_linalg_LU_decomp(Jxucopy, p, &signum);
	gsl_linalg_LU_invert(Jxucopy, p, jacobian_u_x);*/


	// Free vectors and matrices used in this method
	gsl_vector_free(z);
   // gsl_matrix_free(Jxucopy);
	gsl_matrix_free(diag);

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

	return 0;
}

int RNatafTransformer::computeYSensitivityToDistributionParameters(const gsl_vector *pointInOriginalSpace, gsl_matrix *dy_dMean, gsl_matrix *dy_dStdv, int threadID, QList<RPointer<RRandomVariable> > sensitivityRandomVariableList)
{
	// Create the "z-vector"
	gsl_vector *z = gsl_vector_calloc(numRVs);
	int status = xToz(pointInOriginalSpace, z, threadID);
	if (status < 0) {
		rCritical(threadID) << "Error: Cannot transform the vector of random variables to the standard normal space.";
		return -1;
	}

	gsl_vector *dzdMeanTempVector = gsl_vector_calloc(numRVs);
	gsl_vector *dzdStdvTempVector = gsl_vector_calloc(numRVs);
	gsl_vector *meanTempVector = gsl_vector_calloc(numRVs);
	gsl_vector *stdvTempVector = gsl_vector_calloc(numRVs);
	
	gsl_matrix *dLinvdMeanTempMatrix = gsl_matrix_calloc(numRVs, numRVs);
	gsl_matrix *dLinvdStdvTempMatrix = gsl_matrix_calloc(numRVs, numRVs);

	// start of safe area to let only one thread to fill the maps
	{
		QMutexLocker safeFilling(safeLinvdMapLock);

		// if the maps have been filled before, unlocking the lock
		if (linvdMapsHaveBeenFilled)
			safeFilling.unlock();

		bool computedLinv = dLinvdMeanList.isEmpty();

		// Loop over all random variables. The sensitivity of the y-vector to each random variable mean or stdv will be a row of the Jacobian matrix.
		for (int i = 0; i < numRVs; i++) {

			// RPointer<RContinuousRandomVariable> randomVariable = qobject_cast<RPointer<RContinuousRandomVariable >>(randomVariableList->value(i));
			RPointer<RContinuousRandomVariable> randomVariable = randomVariableList->value(i);
			if (!sensitivityRandomVariableList.isEmpty()) {
				if (!sensitivityRandomVariableList.contains(randomVariable.cast_to_super<RRandomVariable>())) {
					if (computedLinv) {
						dLinvdMeanList << 0;
						dLinvdStdvList << 0;
					}

					continue;
				}
			}


			double xi = gsl_vector_get(pointInOriginalSpace, i);
			double zi = gsl_vector_get(z, i);
		
		
		
			// Keep the GUI responsive
			if (threadID == 0)
				QCoreApplication::processEvents();

			// This sensitivity has two terms: (L^-1 * dz/dMean) + (dL^-1/dMean * z)


		
			// First, we need to compute (L^-1 * dz/dMean) and (L^-1 * dz/dStdv)

			// Computing dzdMean and dzdStdv
			double dzdMean = 0.0;
			double dzdStdv = 0.0;
			status = computeDzDMeanStdv(randomVariable.data(), xi, zi, &dzdMean, &dzdStdv, threadID);

			gsl_vector_set_zero(dzdMeanTempVector);
			gsl_vector_set_zero(dzdStdvTempVector);
			gsl_vector_set(dzdMeanTempVector, i, dzdMean);
			gsl_vector_set(dzdStdvTempVector, i, dzdStdv);

		
			// Compute (L^-1 * dz/dMean) and (L^-1 * dz/dStdv): Use the GSL matrix-vector product for the triangular matrices
			status = gsl_blas_dtrmv(CblasLower, CblasNoTrans, CblasNonUnit, Linv[threadID], dzdMeanTempVector);
			if (status) { 
				rCritical(threadID) << "Error in GSL:" << gsl_strerror(status);
				return -1;
			}  
			status = gsl_blas_dtrmv(CblasLower, CblasNoTrans, CblasNonUnit, Linv[threadID], dzdStdvTempVector);
			if (status) { 
				rCritical(threadID) << "Error in GSL:" << gsl_strerror(status);
				return -1;
			}  



			// Second, we need to compute (dL^-1/dMean * z) and (dL^-1/dStdv * z)

			// Computing dL^-1/dMean and dL^-1/dStdv by finite difference

			if (computedLinv) {
				status = computedLinvdMean(randomVariable.data(), dLinvdMeanTempMatrix, dLinvdStdvTempMatrix, true, threadID);
				if (status) {
					rCritical(threadID) << "Error in computing dLinv/dMean and dLinv/dStdv in Nataf Transformer";
					return -1;
				}
				gsl_matrix *tempMatrix = gsl_matrix_calloc(numRVs, numRVs);
				gsl_matrix_memcpy(tempMatrix, dLinvdMeanTempMatrix);
				dLinvdMeanList << tempMatrix;

				tempMatrix = gsl_matrix_calloc(numRVs, numRVs);
				gsl_matrix_memcpy(tempMatrix, dLinvdStdvTempMatrix);
				dLinvdStdvList << tempMatrix;

				//Temprory for checking with finite difference
				//gsl_matrix *dLinvdMean2 = gsl_matrix_calloc(numRVs, numRVs);
				//gsl_matrix *dLinvdStdv2 = gsl_matrix_calloc(numRVs, numRVs);
				//status = computedLinvdMean2(randomVariable, dLinvdMean2, dLinvdStdv2);
				//if (status) {
				//	rCritical() << "Error in computing dLinv/dMean and dLinv/dStdv in Nataf Transformer";
				//	return -1;
				//}
				//theDomain->print(dLinvdMean);
				//rDebug() << "***";
				//theDomain->print(dLinvdMean2);
				//rDebug() << "+++";

				//gsl_matrix *dLinvdMean_error = gsl_matrix_calloc(numRVs, numRVs);
				//gsl_matrix_sub(dLinvdMean2, dLinvdMean);
				//gsl_matrix_div_elements(dLinvdMean2, dLinvdMean);
				//gsl_matrix_scale(dLinvdMean2, 100);
				//gsl_matrix_memcpy(dLinvdMean_error, dLinvdMean2);
				//theDomain->print(dLinvdMean_error);
				//rDebug() << "***";
			} else {
				gsl_matrix_memcpy(dLinvdMeanTempMatrix, dLinvdMeanList[i]);
				gsl_matrix_memcpy(dLinvdStdvTempMatrix, dLinvdStdvList[i]);
			}
			//rDebug() << "";
			///theDomain->print(dLinvdMeanTempMatrix);
			//rDebug() << "";
		




			// Compute temp = dL^-1/dMean * z
			status = gsl_vector_memcpy(meanTempVector, z);
			if (status) { 
				rCritical(threadID) << "Error in GSL when copying a matrix into another:" << gsl_strerror(status);
				return -1;
			}  

			status = gsl_blas_dtrmv(CblasLower, CblasNoTrans, CblasNonUnit, dLinvdMeanTempMatrix, meanTempVector);
			if (status) {
				rCritical(threadID) << "Error in GSL:" << gsl_strerror(status);
				return -1;
			}

			// Compute temp = dL^-1/dStdv * z
			status = gsl_vector_memcpy(stdvTempVector, z);
			if (status) { 
				rCritical(threadID) << "Error in GSL when copying a matrix into another:" << gsl_strerror(status);
				return -1;
			}	

			status = gsl_blas_dtrmv(CblasLower, CblasNoTrans, CblasNonUnit, dLinvdStdvTempMatrix, stdvTempVector);
			if (status) {
				rCritical(threadID) << "Error in GSL:" << gsl_strerror(status);
				return -1;
			}

			// Now, the vectors that are obtained from the first and second part are added. This will be the sensitivity with respect to the random variable i. This will form a row of the Jacobian matrix. Note that the naming does not correspond to what the vector really is at this stage.
			// meanTempVector = meanTempVector + dzdMeanVector
			gsl_vector_add(meanTempVector, dzdMeanTempVector);
			// stdvTempVector = stdvTempVector + dzdStdvVector
			gsl_vector_add(stdvTempVector, dzdStdvTempVector);

			// Setting the ith row of the Jacobian matrix
			gsl_matrix_set_row(dy_dMean, i, meanTempVector);
			gsl_matrix_set_row(dy_dStdv, i, stdvTempVector);
		}

		linvdMapsHaveBeenFilled = true;

	} // end of safe area for filling maps

	

	// Freeing the matrices and vectors
	if (z) {
		gsl_vector_free(z);
		z = 0;
	}
	if (dzdMeanTempVector) {
		gsl_vector_free(dzdMeanTempVector);
		dzdMeanTempVector = 0;
	}
	if (dzdStdvTempVector) {
		gsl_vector_free(dzdStdvTempVector);
		dzdStdvTempVector = 0;
	}
	if (meanTempVector) {
		gsl_vector_free(meanTempVector);
		meanTempVector = 0;
	}
	if (stdvTempVector) {
		gsl_vector_free(stdvTempVector);
		stdvTempVector = 0;
	}
	if (dLinvdStdvTempMatrix) {
		gsl_matrix_free(dLinvdStdvTempMatrix);
		dLinvdStdvTempMatrix= 0;
	}
	if (dLinvdMeanTempMatrix) {
		gsl_matrix_free(dLinvdMeanTempMatrix);
		dLinvdMeanTempMatrix = 0;
	}
	

	return 0;
}


int RNatafTransformer::computedLinvdMean(RContinuousRandomVariable *randomVariable, gsl_matrix *dLinvdMean, gsl_matrix *dLinvdStdv, bool DDM, int threadID)
{
	gsl_matrix *R0original = gsl_matrix_calloc(numRVs, numRVs);
	gsl_matrix *Loriginal = gsl_matrix_calloc(numRVs, numRVs);

	// Get a copy of the original L and R0
	gsl_matrix_memcpy(Loriginal, L[threadID]);
	gsl_matrix_memcpy(R0original, R0[threadID]);

	gsl_matrix *dLdMean = gsl_matrix_calloc(numRVs, numRVs);
	gsl_matrix *dLdStdv = gsl_matrix_calloc(numRVs, numRVs);

	if  (!DDM) { // Use FDM
		// Perturb R0 for mean sensitivity analysis and compute dLdMean
		double mean = randomVariable->getMeanValue(threadID);
		double stdv = randomVariable->getStandardDeviationValue(threadID);
		double h = stdv / 200.0;
		
		// Perturb the mean
		double par1 = 0.0;
		double par2 = 0.0;
		double par3 = 0.0;
		double par4 = 0.0;

		if  (randomVariable->getDistributionType() == QString("Beta (a, b, min, max)")) {
			// For Beta distribution, if moments can not are set other parameters change. So, to keep stdv, a and b constant and change the mean as much as h, the following is done:
			par3 = randomVariable->getParameter3Value(threadID);
			par4 = randomVariable->getParameter4Value(threadID);
			randomVariable->setMeanValue(mean + h, threadID);
			randomVariable->setParameter3Value(par3 + h, threadID);
			randomVariable->setParameter4Value(par4 + h, threadID);
		} else {
			randomVariable->setMeanValue(mean + h, threadID);
		}


		// Establish the perturbed R0
		int status = establishCorrelationMatrix(R0[threadID], threadID, randomVariable);
		if (status < 0) {
			rCritical(threadID) << "Error: Cannot establish the perturbed correlation matrix.";
			return -1;
		}

		// Compute the perturbed L
		status = computeCholesky(R0[threadID], L[threadID], threadID);
		if (status < 0) {
			rCritical(threadID) << "Error: Cannot compute the Cholesky decomposition of the perturbed correlation matrix.";
			return -1;
		}

		gsl_matrix_memcpy(dLdMean, L[threadID]);
		gsl_matrix_sub(dLdMean, Loriginal);
		gsl_matrix_scale(dLdMean, 1.0 / h);

		// Restore the mean
		if  (randomVariable->getDistributionType() == QString("Beta (a, b, min, max)")) {
			randomVariable->setParameter3Value(par3, threadID);
			randomVariable->setParameter4Value(par4, threadID);
			randomVariable->setMeanValue(mean, threadID);
		} else {
			randomVariable->setMeanValue(mean, threadID);
		}

			
		// Perturb R0 for stdv sensitivity analysis and compute dLdStdv
		
		// Perturb the stdv
		if  (randomVariable->getDistributionType() == QString("Beta (a, b, min, max)")) {
			// For Beta distribution, if moments can not are set other parameters change. So, to keep stdv, a and b constant and change the mean as much as h, the following is done:
			par1 = randomVariable->getParameter1Value(threadID);
			par2 = randomVariable->getParameter2Value(threadID);
			par3 = randomVariable->getParameter3Value(threadID);
			par4 = randomVariable->getParameter4Value(threadID);
			double temp = h / (1.0/(par1 + par2) * sqrt(par1*par2 / (1.0 + par1 + par2)));

			randomVariable->setStandardDeviationValue(stdv + h, threadID);
			randomVariable->setParameter3Value(par3 - temp / 2.0, threadID);
			randomVariable->setParameter4Value(par4 + temp / 2.0, threadID);
		} else {
			randomVariable->setStandardDeviationValue(stdv + h, threadID);
		}

		
		// Establish the perturbed R0
		status = establishCorrelationMatrix(R0[threadID], threadID, randomVariable);
		if (status < 0) {
			rCritical(threadID) << "Error: Cannot establish the perturbed correlation matrix.";
			return -1;
		}

		// Compute the perturbed L
		status = computeCholesky(R0[threadID], L[threadID], threadID);
		if (status < 0) {
			rCritical(threadID) << "Error: Cannot compute the Cholesky decomposition of the perturbed correlation matrix.";
			return -1;
		}

		gsl_matrix_memcpy(dLdStdv, L[threadID]);
		gsl_matrix_sub(dLdStdv, Loriginal);
		gsl_matrix_scale(dLdStdv, 1.0 / h);

		// Restore the stdv
		if  (randomVariable->getDistributionType() == QString("Beta (a, b, min, max)")) {
			randomVariable->setStandardDeviationValue(stdv, threadID);
			randomVariable->setParameter3Value(par3, threadID);
			randomVariable->setParameter4Value(par4, threadID);
		} else {
			randomVariable->setStandardDeviationValue(stdv, threadID);
		}

		
		// Restore L and R0
		gsl_matrix_memcpy(L[threadID], Loriginal);
		gsl_matrix_memcpy(R0[threadID], R0original);

	} else { // Use DDM
		gsl_matrix *R0copy = gsl_matrix_calloc(numRVs, numRVs);
		gsl_matrix *Lcopy = gsl_matrix_calloc(numRVs, numRVs);

		// Get a copy of the original L and R0
		gsl_matrix_memcpy(Lcopy, L[threadID]);
		gsl_matrix_memcpy(R0copy, R0[threadID]);

		double tempValueMean = 0;
		double tempValue2Mean = 0;
		double tempValue3 = 0;
		double tempValue4Mean = 0;
		double tempValue5Mean = 0;
		double tempValue6Mean = 0;
		double tempValueStdv = 0;
		double tempValue2Stdv = 0;
		double tempValue4Stdv = 0;
		double tempValue5Stdv = 0;
		double tempValue6Stdv = 0;
		
		/* To compute tempValue5, i.e. sensitvity of correlation coefficient, by finite diffrence uncomment these part and the part below.
		//Compute dR0/dMean by finite difference 

		gsl_matrix *R0pert = gsl_matrix_calloc(numRVs, numRVs);
		double mean = randomVariable->getMeanValue();
		double stdv = randomVariable->getStandardDeviationValue();
		double par3 = 0.0;
		double par4 = 0.0;
		double h = stdv / 200.0;

		// Perturb the mean
		if  (randomVariable->getDistributionType() == QString("Beta (a, b, min, max)")) {
			par3 = randomVariable->getParameter3Value();
			par4 = randomVariable->getParameter4Value();
			randomVariable->setMeanValue(mean + h);
			randomVariable->setParameter3Value(par3 + h);
			randomVariable->setParameter4Value(par4 + h);
		} else {
			randomVariable->setMeanValue(mean + h);
		}

		establishCorrelationMatrix(R0pert);

		// Restore the mean
		if  (randomVariable->getDistributionType() == QString("Beta (a, b, min, max)")) {
			randomVariable->setParameter3Value(par3);
			randomVariable->setParameter4Value(par4);
			randomVariable->setMeanValue(mean);
		} else {
			randomVariable->setMeanValue(mean);
		}*/


		//Compute dR0/dMean by DDM 
		//QList<RRandomVariable *> *randomVariableList = getRandomVariableList();
		int numRV = randomVariableList->indexOf(randomVariable);
		double drhoMean = 0.0;
		double drhoStdv = 0.0;
	
		for (int i = 0; i < numRVs; i++) {
			for (int j = i+1; j < numRVs; j++) {
				tempValueMean = 0;
				tempValue2Mean = 0;
				tempValue3 = 0;
				tempValue4Mean = 0;
				tempValue5Mean = 0;
				tempValue6Mean = 0;
				tempValueStdv = 0;
				tempValue2Stdv = 0;
				tempValue4Stdv = 0;
				tempValue5Stdv = 0;
				tempValue6Stdv = 0;
				if (i==j) {
					for (int k = 0; k < i-1; k++) {
						tempValue2Mean += gsl_matrix_get(Lcopy, i, k)*gsl_matrix_get(dLdMean, i, k);
						tempValue2Stdv += gsl_matrix_get(Lcopy, i, k)*gsl_matrix_get(dLdStdv, i, k);
						tempValue3 += gsl_matrix_get(Lcopy, i, k)*gsl_matrix_get(Lcopy, i, k);
					}
					tempValueMean = (-1.0*tempValue2Mean)/(sqrt(gsl_matrix_get(R0copy, i, i)-tempValue3));
					gsl_matrix_set(dLdMean, i, i, tempValueMean);

					tempValueStdv = (-1.0*tempValue2Stdv)/(sqrt(gsl_matrix_get(R0copy, i, i)-tempValue3));
					gsl_matrix_set(dLdStdv, i, i, tempValueStdv);

				} else {
					for (int k = 0; k < i-1; k++) {
						tempValue2Mean += gsl_matrix_get(Lcopy, i, k)*gsl_matrix_get(dLdMean, j, k)+ gsl_matrix_get(Lcopy, j, k)*gsl_matrix_get(dLdMean, i, k);
						tempValue2Stdv += gsl_matrix_get(Lcopy, i, k)*gsl_matrix_get(dLdStdv, j, k)+ gsl_matrix_get(Lcopy, j, k)*gsl_matrix_get(dLdStdv, i, k);
						tempValue3 += gsl_matrix_get(Lcopy, i, k)*gsl_matrix_get(Lcopy, j, k);
					}
					tempValue4Mean = gsl_matrix_get(dLdMean, i, i)/gsl_pow_2(gsl_matrix_get(Lcopy, i, i))*(gsl_matrix_get(R0copy, i, j)-tempValue3);
					tempValue4Stdv = gsl_matrix_get(dLdStdv, i, i)/gsl_pow_2(gsl_matrix_get(Lcopy, i, i))*(gsl_matrix_get(R0copy, i, j)-tempValue3);
					/* To compute tempValue5, i.e. sensitvity of correlation coefficient, by finite diffrence uncomment these part and the part above.
					tempValue5 = (gsl_matrix_get(R0pert,i,j)-gsl_matrix_get(R0copy,i,j))/h;
					*/


					if (i==numRV)
					{
						drhoMean = computeCorrelationSensitvity(randomVariable, randomVariableList->value(j), gsl_matrix_get(R0copy, i, j), true, threadID);
						drhoStdv = computeCorrelationSensitvity(randomVariable, randomVariableList->value(j), gsl_matrix_get(R0copy, i, j), false, threadID);
					}
					else if (j==numRV)
					{
						drhoMean = computeCorrelationSensitvity(randomVariableList->value(j), randomVariable, gsl_matrix_get(R0copy, i, j), true, threadID);
						drhoStdv = computeCorrelationSensitvity(randomVariableList->value(j), randomVariable, gsl_matrix_get(R0copy, i, j), false, threadID);
					}
					else
					{
						drhoMean = 0;
						drhoStdv = 0;
					}
					tempValue5Mean = drhoMean;
					tempValue5Stdv = drhoStdv;

					tempValue6Mean = (1/gsl_matrix_get(Lcopy, i, i))*(tempValue5Mean-tempValue2Mean);
					tempValue6Stdv = (1/gsl_matrix_get(Lcopy, i, i))*(tempValue5Stdv-tempValue2Stdv);

					tempValueMean = tempValue6Mean - tempValue4Mean;
					tempValueStdv = tempValue6Stdv - tempValue4Stdv;

					gsl_matrix_set(dLdMean, j, i, tempValueMean);
					gsl_matrix_set(dLdStdv, j, i, tempValueStdv);
				}
			}
		}
	}


	// Compute dL^-1/dMean & dL^-1/dStdv   (dL^-1/dMean = -L^-1 * dL/dMean * L^-1  and so for stdv)
	gsl_matrix *tempMatrix = gsl_matrix_calloc(numRVs, numRVs);
	gsl_matrix *tempMatrix2 = gsl_matrix_calloc(numRVs, numRVs);

	// Compute tempMatrix = dL/dMean * L^-1 
	int status = gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, dLdMean, Linv[threadID], 0.0, tempMatrix);
	if (status) {
		rCritical(threadID) << "Error in GSL:" << gsl_strerror(status);
		return -1;
	}


	// Compute tempMatrix = -L^-1* dL/dMean * L^-1 = -1.0 * L^-1 * tempMatrix			
	status = gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, -1.0, Linv[threadID], tempMatrix, 0.0, tempMatrix2);
	if (status) {
		rCritical(threadID) << "Error in GSL:" << gsl_strerror(status);
		return -1;
	}

	
	gsl_matrix_memcpy(dLinvdMean, tempMatrix2);

	
	gsl_matrix_set_zero(tempMatrix);
	gsl_matrix_set_zero(tempMatrix2);

	// Compute tempMatrix = dL/dStdv * L^-1 
	status = gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, dLdStdv, Linv[threadID], 0.0, tempMatrix);
	if (status) {
		rCritical(threadID) << "Error in GSL:" << gsl_strerror(status);
		return -1;
	}

	// Compute tempMatrix = -L^-1* dL/dStdv * L^-1 = -1.0 * L^-1 * tempMatrix			
	status = gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, -1.0, Linv[threadID], tempMatrix, 0.0, tempMatrix2);
	if (status) {
		rCritical(threadID) << "Error in GSL:" << gsl_strerror(status);
		return -1;
	}

	gsl_matrix_memcpy(dLinvdStdv, tempMatrix2);

	// Unallocating matrices
	if (R0original) {
		gsl_matrix_free(R0original);
		R0original = 0;
	}
	if (Loriginal) {
		gsl_matrix_free(Loriginal);
		Loriginal = 0;
	}
	if (dLdMean) {
		gsl_matrix_free(dLdMean);
		dLdMean = 0;
	}
	if (dLdStdv) {
		gsl_matrix_free(dLdStdv);
		dLdStdv = 0;
	}
	if (tempMatrix) {
		gsl_matrix_free(tempMatrix);
		tempMatrix = 0;
	}
	if (tempMatrix2) {
		gsl_matrix_free(tempMatrix2);
		tempMatrix2 = 0;
	}


	return 0;
}


double RNatafTransformer::phi2(double z_i, double z_j, double rho)
{
	double par = z_i*z_i + z_j*z_j - 2.0*rho*z_i*z_j;
	double theExp = exp(-par/(2.0*(1.0-rho*rho)));
	double result = theExp/(2.0*M_PI*sqrt(1.0-rho*rho));

	return result;
}

double RNatafTransformer::integrand(RRandomVariable *rv_i, double z_i, double mean_i, double stdv_i, RRandomVariable *rv_j, double z_j, double mean_j, double stdv_j, double rho_zero, bool evaluateDerivative, int threadID)
{
	double x_i = rv_i->getInverseCDFValue(gsl_cdf_gaussian_P(z_i, 1.0),threadID);
	double x_j = rv_j->getInverseCDFValue(gsl_cdf_gaussian_P(z_j, 1.0),threadID);

	double thePhi2 = phi2(z_i, z_j, rho_zero);

	double result;
	if (evaluateDerivative) {
		double factor = rho_zero/(2*M_PI*(1.0-rho_zero*rho_zero)) + (z_i*z_j*(1.0-rho_zero*rho_zero) - (rho_zero)*( z_i*z_i - 2*rho_zero*z_i*z_j+z_j*z_j ) ) / ((1.0-rho_zero*rho_zero)*(1.0-rho_zero*rho_zero));      
		result = ( (x_i-mean_i)/stdv_i * (x_j-mean_j)/stdv_j * thePhi2 * factor);
	}
	else {
		result = ( (x_i-mean_i)/stdv_i * (x_j-mean_j)/stdv_j * thePhi2 );
	}

	return result;
}

double RNatafTransformer::doubleIntegral(RRandomVariable *rv_i, double mean_i, double stdv_i, RRandomVariable *rv_j, double mean_j, double stdv_j, double rho, int threadID, bool evaluateDerivative, int numIntegrand, double drho_zerodMean)
{
	// The grid of integration points:
	// 1, 2, ..., i, ..., 2*n in x-direction with intervals h
	// 1, 2, ..., j, ..., 2*m in y-direction with intervals k


	// Selected integration boundaries
	double bound = 5.6;
	double z_i0 = -bound;
	double z_in = bound;
	double z_j0 = -bound;
	double z_jm = bound;

	// Half the number of integration points
	int numPoints = 25;
	int n = numPoints;
	int m = numPoints;

	// Integration point intervals
	double h = (z_in-z_i0)/(2.0*n);
	double k = (z_jm-z_j0)/(2.0*m);

	// Grid of integration points
	QVector<double> z_i;
	QVector<double> z_j;
	for (int i = 0; i<=2*n; i++) {
		z_i << z_i0 + i*h;
	}
	for (int j=0; j<=2*m; j++) {
		z_j << z_j0 + j*k;
	}

	// Computing sums (naming terms according to p. 126 in "Numerical Methods" by Faires & Burden)
	double term1 = 0.0;
	double term2 = 0.0;
	double term3 = 0.0;
	double term4 = 0.0;
	double term5 = 0.0;
	double term6 = 0.0;
	double term7 = 0.0;
	double term8 = 0.0;
	double term9 = 0.0;
	double term10 = 0.0;
	double term11 = 0.0;
	double term12 = 0.0;
	double term13 = 0.0;
	double term14 = 0.0;
	double term15 = 0.0;
	double term16 = 0.0;
	double result = 0.0;

	if (numIntegrand==1){

		term1 = integrand( rv_i, z_i[0] ,    mean_i, stdv_i, rv_j,   z_j[0] ,     mean_j, stdv_j, rho, evaluateDerivative, threadID);
		term4 = integrand( rv_i, z_i[2*n] ,  mean_i, stdv_i, rv_j,   z_j[0] ,     mean_j, stdv_j, rho, evaluateDerivative, threadID);
		term13 = integrand(rv_i, z_i[0] ,    mean_i, stdv_i, rv_j,   z_j[2*m],    mean_j, stdv_j, rho, evaluateDerivative, threadID);
		term16 = integrand(rv_i, z_i[2*n],   mean_i, stdv_i, rv_j,   z_j[2*m],    mean_j, stdv_j, rho, evaluateDerivative, threadID);

		for (int i=1; i<=n; i++) {
			term3 += integrand(rv_i,  z_i[2*i-1],     mean_i, stdv_i, rv_j,  z_j[0],       mean_j, stdv_j, rho, evaluateDerivative, threadID);
			term15 += integrand(rv_i, z_i[2*i-1],     mean_i, stdv_i, rv_j,  z_j[2*m],     mean_j, stdv_j, rho, evaluateDerivative, threadID);
		}

		for (int i=1; i<=n-1; i++) {
			term2 += integrand(rv_i,  z_i[2*i],       mean_i, stdv_i, rv_j,  z_j[0],       mean_j, stdv_j, rho, evaluateDerivative, threadID);
			term14 += integrand(rv_i, z_i[2*i],       mean_i, stdv_i, rv_j,  z_j[2*m],     mean_j, stdv_j, rho, evaluateDerivative, threadID);
		}

		for (int j=1; j<=m-1; j++) {
			term5 += integrand(rv_i,   z_i[0],    mean_i, stdv_i, rv_j,   z_j[2*j],   mean_j, stdv_j, rho, evaluateDerivative, threadID);
			term8 += integrand(rv_i,   z_i[2*n],  mean_i, stdv_i, rv_j,   z_j[2*j],   mean_j, stdv_j, rho, evaluateDerivative, threadID);
		}

		for (int j=1; j<=m; j++) {
			term9 += integrand(rv_i,   z_i[0],   mean_i, stdv_i, rv_j,   z_j[2*j-1],   mean_j, stdv_j, rho, evaluateDerivative, threadID);
			term12 += integrand(rv_i,  z_i[2*n], mean_i, stdv_i, rv_j,   z_j[2*j-1],   mean_j, stdv_j, rho, evaluateDerivative, threadID);
		}

		for (int j=1; j<=(m-1); j++) {
			for (int i=1; i<=(n-1); i++) {
				term6 += integrand(rv_i, z_i[2*i], mean_i, stdv_i, rv_j, z_j[2*j], mean_j, stdv_j, rho, evaluateDerivative, threadID);
			}
		}

		for (int j=1; j<=(m-1); j++) {
			for (int i=1; i<=(n); i++) {
				term7 += integrand(rv_i, z_i[2*i-1], mean_i, stdv_i, rv_j, z_j[2*j], mean_j, stdv_j, rho, evaluateDerivative, threadID);
			}
		}
		for (int j=1; j<=(m); j++) {
			for (int i=1; i<=(n-1); i++) {
				term10 += integrand(rv_i, z_i[2*i], mean_i, stdv_i, rv_j, z_j[2*j-1], mean_j, stdv_j, rho, evaluateDerivative, threadID);
			}
		}
		for (int j=1; j<=(m); j++) {
			for (int i=1; i<=(n); i++) {
				term11 += integrand(rv_i, z_i[2*i-1], mean_i, stdv_i, rv_j, z_j[2*j-1], mean_j, stdv_j, rho, evaluateDerivative, threadID);
			}
		}


		double par1 = term1 + 2.0*term2 + 4.0*term3 + term4;
		double par2 = term5 + 2.0*term6 + 4.0*term7 + term8;
		double par3 = term9 + 2.0*term10 + 4.0*term11 + term12;
		double par4 = term13 + 2.0*term14 + 4.0*term15 + term16;
		result = h*k/9.0 * (par1 + 2.0*par2 + 4.0*par3 + par4);
	}
	else if (numIntegrand==2)	{

		term1 = integrand2( rv_i, z_i[0] ,    mean_i, stdv_i, rv_j,   z_j[0] ,     mean_j, stdv_j, rho, evaluateDerivative, threadID);
		term4 = integrand2( rv_i, z_i[2*n] ,  mean_i, stdv_i, rv_j,   z_j[0] ,     mean_j, stdv_j, rho, evaluateDerivative, threadID);
		term13 = integrand2(rv_i, z_i[0] ,    mean_i, stdv_i, rv_j,   z_j[2*m],    mean_j, stdv_j, rho, evaluateDerivative, threadID);
		term16 = integrand2(rv_i, z_i[2*n],   mean_i, stdv_i, rv_j,   z_j[2*m],    mean_j, stdv_j, rho, evaluateDerivative, threadID);

		for (int i=1; i<=n; i++) {
			term3 += integrand2(rv_i,  z_i[2*i-1],     mean_i, stdv_i, rv_j,  z_j[0],       mean_j, stdv_j, rho, evaluateDerivative, threadID);
			term15 += integrand2(rv_i, z_i[2*i-1],     mean_i, stdv_i, rv_j,  z_j[2*m],     mean_j, stdv_j, rho, evaluateDerivative, threadID);
		}

		for (int i=1; i<=n-1; i++) {
			term2 += integrand2(rv_i,  z_i[2*i],       mean_i, stdv_i, rv_j,  z_j[0],       mean_j, stdv_j, rho, evaluateDerivative, threadID);
			term14 += integrand2(rv_i, z_i[2*i],       mean_i, stdv_i, rv_j,  z_j[2*m],     mean_j, stdv_j, rho, evaluateDerivative, threadID);
		}

		for (int j=1; j<=m-1; j++) {
			term5 += integrand2(rv_i,   z_i[0],    mean_i, stdv_i, rv_j,   z_j[2*j],   mean_j, stdv_j, rho, evaluateDerivative, threadID);
			term8 += integrand2(rv_i,   z_i[2*n],  mean_i, stdv_i, rv_j,   z_j[2*j],   mean_j, stdv_j, rho, evaluateDerivative, threadID);
		}

		for (int j=1; j<=m; j++) {
			term9 += integrand2(rv_i,   z_i[0],   mean_i, stdv_i, rv_j,   z_j[2*j-1],   mean_j, stdv_j, rho, evaluateDerivative, threadID);
			term12 += integrand2(rv_i,  z_i[2*n], mean_i, stdv_i, rv_j,   z_j[2*j-1],   mean_j, stdv_j, rho, evaluateDerivative, threadID);
		}

		for (int j=1; j<=(m-1); j++) {
			for (int i=1; i<=(n-1); i++) {
				term6 += integrand2(rv_i, z_i[2*i], mean_i, stdv_i, rv_j, z_j[2*j], mean_j, stdv_j, rho, evaluateDerivative, threadID);
			}
		}

		for (int j=1; j<=(m-1); j++) {
			for (int i=1; i<=(n); i++) {
				term7 += integrand2(rv_i, z_i[2*i-1], mean_i, stdv_i, rv_j, z_j[2*j], mean_j, stdv_j, rho, evaluateDerivative, threadID);
			}
		}
		for (int j=1; j<=(m); j++) {
			for (int i=1; i<=(n-1); i++) {
				term10 += integrand2(rv_i, z_i[2*i], mean_i, stdv_i, rv_j, z_j[2*j-1], mean_j, stdv_j, rho, evaluateDerivative, threadID);
			}
		}
		for (int j=1; j<=(m); j++) {
			for (int i=1; i<=(n); i++) {
				term11 += integrand2(rv_i, z_i[2*i-1], mean_i, stdv_i, rv_j, z_j[2*j-1], mean_j, stdv_j, rho, evaluateDerivative, threadID);
			}
		}


		double par1 = term1 + 2.0*term2 + 4.0*term3 + term4;
		double par2 = term5 + 2.0*term6 + 4.0*term7 + term8;
		double par3 = term9 + 2.0*term10 + 4.0*term11 + term12;
		double par4 = term13 + 2.0*term14 + 4.0*term15 + term16;
		result = h*k/9.0 * (par1 + 2.0*par2 + 4.0*par3 + par4);
	}
	else if (numIntegrand==3)
	{
		term1 = integrand3( rv_i, z_i[0] ,    mean_i, stdv_i, rv_j,   z_j[0] ,     mean_j, stdv_j, rho, evaluateDerivative, threadID);
		term4 = integrand3( rv_i, z_i[2*n] ,  mean_i, stdv_i, rv_j,   z_j[0] ,     mean_j, stdv_j, rho, evaluateDerivative, threadID);
		term13 = integrand3(rv_i, z_i[0] ,    mean_i, stdv_i, rv_j,   z_j[2*m],    mean_j, stdv_j, rho, evaluateDerivative, threadID);
		term16 = integrand3(rv_i, z_i[2*n],   mean_i, stdv_i, rv_j,   z_j[2*m],    mean_j, stdv_j, rho, evaluateDerivative, threadID);

		for (int i=1; i<=n; i++) {
			term3 += integrand3(rv_i,  z_i[2*i-1],     mean_i, stdv_i, rv_j,  z_j[0],       mean_j, stdv_j, rho, evaluateDerivative, threadID);
			term15 += integrand3(rv_i, z_i[2*i-1],     mean_i, stdv_i, rv_j,  z_j[2*m],     mean_j, stdv_j, rho, evaluateDerivative, threadID);
		}

		for (int i=1; i<=n-1; i++) {
			term2 += integrand3(rv_i,  z_i[2*i],       mean_i, stdv_i, rv_j,  z_j[0],       mean_j, stdv_j, rho, evaluateDerivative, threadID);
			term14 += integrand3(rv_i, z_i[2*i],       mean_i, stdv_i, rv_j,  z_j[2*m],     mean_j, stdv_j, rho, evaluateDerivative, threadID);
		}

		for (int j=1; j<=m-1; j++) {
			term5 += integrand3(rv_i,   z_i[0],    mean_i, stdv_i, rv_j,   z_j[2*j],   mean_j, stdv_j, rho, evaluateDerivative, threadID);
			term8 += integrand3(rv_i,   z_i[2*n],  mean_i, stdv_i, rv_j,   z_j[2*j],   mean_j, stdv_j, rho, evaluateDerivative, threadID);
		}

		for (int j=1; j<=m; j++) {
			term9 += integrand3(rv_i,   z_i[0],   mean_i, stdv_i, rv_j,   z_j[2*j-1],   mean_j, stdv_j, rho, evaluateDerivative, threadID);
			term12 += integrand3(rv_i,  z_i[2*n], mean_i, stdv_i, rv_j,   z_j[2*j-1],   mean_j, stdv_j, rho, evaluateDerivative, threadID);
		}

		for (int j=1; j<=(m-1); j++) {
			for (int i=1; i<=(n-1); i++) {
				term6 += integrand3(rv_i, z_i[2*i], mean_i, stdv_i, rv_j, z_j[2*j], mean_j, stdv_j, rho, evaluateDerivative, threadID);
			}
		}

		for (int j=1; j<=(m-1); j++) {
			for (int i=1; i<=(n); i++) {
				term7 += integrand3(rv_i, z_i[2*i-1], mean_i, stdv_i, rv_j, z_j[2*j], mean_j, stdv_j, rho, evaluateDerivative, threadID);
			}
		}
		for (int j=1; j<=(m); j++) {
			for (int i=1; i<=(n-1); i++) {
				term10 += integrand3(rv_i, z_i[2*i], mean_i, stdv_i, rv_j, z_j[2*j-1], mean_j, stdv_j, rho, evaluateDerivative, threadID);
			}
		}
		for (int j=1; j<=(m); j++) {
			for (int i=1; i<=(n); i++) {
				term11 += integrand3(rv_i, z_i[2*i-1], mean_i, stdv_i, rv_j, z_j[2*j-1], mean_j, stdv_j, rho, evaluateDerivative, threadID);
			}
		}


		double par1 = term1 + 2.0*term2 + 4.0*term3 + term4;
		double par2 = term5 + 2.0*term6 + 4.0*term7 + term8;
		double par3 = term9 + 2.0*term10 + 4.0*term11 + term12;
		double par4 = term13 + 2.0*term14 + 4.0*term15 + term16;
		result = h*k/9.0 * (par1 + 2.0*par2 + 4.0*par3 + par4);
	}
	else if (numIntegrand==4)
	{
		term1 = integrand3( rv_i, z_i[0] ,    mean_i, stdv_i, rv_j,   z_j[0] ,     mean_j, stdv_j, rho, drho_zerodMean, threadID);
		term4 = integrand3( rv_i, z_i[2*n] ,  mean_i, stdv_i, rv_j,   z_j[0] ,     mean_j, stdv_j, rho, drho_zerodMean, threadID);
		term13 = integrand3(rv_i, z_i[0] ,    mean_i, stdv_i, rv_j,   z_j[2*m],    mean_j, stdv_j, rho, drho_zerodMean, threadID);
		term16 = integrand3(rv_i, z_i[2*n],   mean_i, stdv_i, rv_j,   z_j[2*m],    mean_j, stdv_j, rho, drho_zerodMean, threadID);

		for (int i=1; i<=n; i++) {
			term3 += integrand4(rv_i,  z_i[2*i-1],     mean_i, stdv_i, rv_j,  z_j[0],       mean_j, stdv_j, rho, drho_zerodMean, threadID);
			term15 += integrand4(rv_i, z_i[2*i-1],     mean_i, stdv_i, rv_j,  z_j[2*m],     mean_j, stdv_j, rho, drho_zerodMean, threadID);
		}

		for (int i=1; i<=n-1; i++) {
			term2 += integrand4(rv_i,  z_i[2*i],       mean_i, stdv_i, rv_j,  z_j[0],       mean_j, stdv_j, rho, drho_zerodMean, threadID);
			term14 += integrand4(rv_i, z_i[2*i],       mean_i, stdv_i, rv_j,  z_j[2*m],     mean_j, stdv_j, rho, drho_zerodMean, threadID);
		}

		for (int j=1; j<=m-1; j++) {
			term5 += integrand4(rv_i,   z_i[0],    mean_i, stdv_i, rv_j,   z_j[2*j],   mean_j, stdv_j, rho, drho_zerodMean, threadID);
			term8 += integrand4(rv_i,   z_i[2*n],  mean_i, stdv_i, rv_j,   z_j[2*j],   mean_j, stdv_j, rho, drho_zerodMean, threadID);
		}

		for (int j=1; j<=m; j++) {
			term9 += integrand4(rv_i,   z_i[0],   mean_i, stdv_i, rv_j,   z_j[2*j-1],   mean_j, stdv_j, rho, drho_zerodMean, threadID);
			term12 += integrand4(rv_i,  z_i[2*n], mean_i, stdv_i, rv_j,   z_j[2*j-1],   mean_j, stdv_j, rho, drho_zerodMean, threadID);
		}

		for (int j=1; j<=(m-1); j++) {
			for (int i=1; i<=(n-1); i++) {
				term6 += integrand4(rv_i, z_i[2*i], mean_i, stdv_i, rv_j, z_j[2*j], mean_j, stdv_j, rho, drho_zerodMean, threadID);
			}
		}

		for (int j=1; j<=(m-1); j++) {
			for (int i=1; i<=(n); i++) {
				term7 += integrand4(rv_i, z_i[2*i-1], mean_i, stdv_i, rv_j, z_j[2*j], mean_j, stdv_j, rho, drho_zerodMean,threadID);
			}
		}
		for (int j=1; j<=(m); j++) {
			for (int i=1; i<=(n-1); i++) {
				term10 += integrand4(rv_i, z_i[2*i], mean_i, stdv_i, rv_j, z_j[2*j-1], mean_j, stdv_j, rho, drho_zerodMean, threadID);
			}
		}
		for (int j=1; j<=(m); j++) {
			for (int i=1; i<=(n); i++) {
				term11 += integrand4(rv_i, z_i[2*i-1], mean_i, stdv_i, rv_j, z_j[2*j-1], mean_j, stdv_j, rho, drho_zerodMean, threadID);
			}
		}


		double par1 = term1 + 2.0*term2 + 4.0*term3 + term4;
		double par2 = term5 + 2.0*term6 + 4.0*term7 + term8;
		double par3 = term9 + 2.0*term10 + 4.0*term11 + term12;
		double par4 = term13 + 2.0*term14 + 4.0*term15 + term16;
		result = h*k/9.0 * (par1 + 2.0*par2 + 4.0*par3 + par4);
	}
	else
	{
		rCritical(threadID) << "Error in computing correlation sensitivity in Nataf Transformer";	
	}

	return result;
}

void RNatafTransformer::residualFunction(double rho_original, double rho, RRandomVariable *rv_i, double mean_i, double stdv_i, RRandomVariable *rv_j, double mean_j, double stdv_j, double *f, double *df, int threadID)
{
	*f = rho_original - doubleIntegral(rv_i, mean_i, stdv_i, rv_j, mean_j, stdv_j, rho, threadID, false);
	//*df = -doubleIntegral(rv_i, mean_i, stdv_i, rv_j, mean_j, stdv_j, rho, true);

	return;
}

void RNatafTransformer::residualFunctionSensitivity(double constantPart, double rho_zero, double drho_zerodMean, RRandomVariable *rv_i, double mean_i, double stdv_i, RRandomVariable *rv_j, double mean_j, double stdv_j, double *f, int threadID)
{
	double integral4 = doubleIntegral(rv_i, mean_i, stdv_i, rv_j, mean_j, stdv_j, rho_zero, threadID, false, 4, drho_zerodMean);
	*f = constantPart + integral4;

	return;
}

double RNatafTransformer::modifyCorrelation(RRandomVariable *rv_i, RRandomVariable *rv_j, double rho_original, int threadID)
{
	double mean_i = rv_i->getMeanValue(threadID);
	double mean_j = rv_j->getMeanValue(threadID);

	double stdv_i = rv_i->getStandardDeviationValue(threadID);
	double stdv_j = rv_j->getStandardDeviationValue(threadID);

	double result = 0.0;

	double tol = 1.0e-6;
	double pert = 1.0e-4;

	double rho_old = rho_original;
	double rho_new;
	double f = 0.0;
	double df = 0.0;
	double perturbed_f;

	for (int i = 1; i <= 100; i++) {

		// Evaluate the function
		residualFunction(rho_original,rho_old, rv_i, mean_i, stdv_i, rv_j, mean_j, stdv_j, &f, &df, threadID);


		///////////////////////////////////////////////////////////////////////////////////////////
		// Evaluate perturbed function
		residualFunction(rho_original, (rho_old+pert),  rv_i, mean_i, stdv_i, rv_j, mean_j, stdv_j, &perturbed_f, &df, threadID);

		// Evaluate derivative of function
		df = (perturbed_f - f) / pert;
		///////////////////////////////////////////////////////////////////////////////////////////////////////////


		if (qAbs(df) < 1.0e-15) {
			rCritical(threadID) << "Error: The derivative used in the Newton algorithm within the Nataf transformation is zero.";
		}
		else {

			// Take a Newton step
			rho_new = rho_old - f/df;

			if (threadID == 0)
				QCoreApplication::processEvents();



			// Check convergence; quit or continue
			if (qAbs(1.0-qAbs(rho_old/rho_new)) < tol) {
				result = rho_new;
				return result;
			}
			else {
				if (i==100) {
					rCritical(threadID) << "Error: The Newton algorithm within the Nataf transformation did not converge.";
					result = 0.0;
					return result;
				}
				else {
					rho_old = rho_new;
				}

			}
		}
	}


	return result;
}

double RNatafTransformer::integrand2(RRandomVariable *rv_i, double z_i, double mean_i, double stdv_i, RRandomVariable *rv_j, double z_j, double mean_j, double stdv_j, double rho_zero, bool evaluateDerivative, int threadID)
{
	double x_i = rv_i->getInverseCDFValue(gsl_cdf_gaussian_P(z_i, 1.0), threadID);
	double x_j = rv_j->getInverseCDFValue(gsl_cdf_gaussian_P(z_j, 1.0), threadID);
	RContinuousRandomVariable *RV_i = qobject_cast<RContinuousRandomVariable *>(rv_i);

	double thePhi2 = phi2(z_i, z_j, rho_zero);

	double dmean_dtheta = 1.0;
	double dstdv_dtheta = 0.0;

	double dInverseCDFdMean = 0.0;
	double dInverseCDFdStdv = 0.0;
	double status = RV_i->computeInverseCDFSensitivityToParameters(gsl_cdf_gaussian_P(z_i, 1.0), &dInverseCDFdMean, &dInverseCDFdStdv, threadID);

	double result = 1.0/stdv_i * (dInverseCDFdMean - dmean_dtheta - (x_i-mean_i)/stdv_i*dstdv_dtheta) * (x_j-mean_j)/stdv_j * thePhi2 ;

	return result;
}

double RNatafTransformer::integrand3(RRandomVariable *rv_i, double z_i, double mean_i, double stdv_i, RRandomVariable *rv_j, double z_j, double mean_j, double stdv_j, double rho_zero, bool evaluateDerivative, int threadID)
{
	double x_i = rv_i->getInverseCDFValue(gsl_cdf_gaussian_P(z_i, 1.0), threadID);
	double x_j = rv_j->getInverseCDFValue(gsl_cdf_gaussian_P(z_j, 1.0), threadID);
	RContinuousRandomVariable *RV_i = qobject_cast<RContinuousRandomVariable *>(rv_i);

	double thePhi2 = phi2(z_i, z_j, rho_zero);

	double dmean_dtheta = 0.0;
	double dstdv_dtheta = 1.0;

	double dInverseCDFdMean = 0.0;
	double dInverseCDFdStdv = 0.0;
	double status = RV_i->computeInverseCDFSensitivityToParameters(gsl_cdf_gaussian_P(z_i, 1.0), &dInverseCDFdMean, &dInverseCDFdStdv, threadID);

	double result = 1.0/stdv_i * (dInverseCDFdMean - dmean_dtheta - (x_i-mean_i)/stdv_i*dstdv_dtheta) * (x_j-mean_j)/stdv_j * thePhi2 ;

	return result;
}

double RNatafTransformer::integrand4(RRandomVariable *rv_i, double z_i, double mean_i, double stdv_i, RRandomVariable *rv_j, double z_j, double mean_j, double stdv_j, double rho_zero, double drho_zerodMean, int threadID)
{
	double x_i = rv_i->getInverseCDFValue(gsl_cdf_gaussian_P(z_i, 1.0), threadID);
	double x_j = rv_j->getInverseCDFValue(gsl_cdf_gaussian_P(z_j, 1.0), threadID);

	double thePhi2 = phi2(z_i, z_j, rho_zero);
	double dPhidrho_zero = (rho_zero / (1.0-gsl_pow_2(rho_zero)) + (z_i*z_j + z_i*z_j*gsl_pow_2(rho_zero) - rho_zero*(gsl_pow_2(z_i)+gsl_pow_2(z_j))) / gsl_pow_2(1.0-gsl_pow_2(rho_zero))) * thePhi2;
	
	double result = (x_i-mean_i)/stdv_i * (x_j-mean_j)/stdv_j * dPhidrho_zero * drho_zerodMean;

	return result;
}


double RNatafTransformer::computeCorrelationSensitvity(RRandomVariable *rv_i, RRandomVariable *rv_j, double rho_zero, bool MeanSensitvity, int threadID)
{

	if (rho_zero == 0.0) {
		return 0.0;
	}

	double mean_i = rv_i->getMeanValue(threadID);
	double mean_j = rv_j->getMeanValue(threadID);

	double stdv_i = rv_i->getStandardDeviationValue(threadID);
	double stdv_j = rv_j->getStandardDeviationValue(threadID);

	double result = 0.0;

	double tol = 1.0e-6;
	double pert = 1.0e-6;

	double drho_old = 0;
	double drho_new;
	double f = 0.0;
	double df = 0.0;
	double perturbed_f;

	double constIntegral = 0.0;
	if (MeanSensitvity= true) {
		constIntegral = doubleIntegral(rv_i, mean_i, stdv_i, rv_j, mean_j, stdv_j, rho_zero, threadID, false, 2);
	} else {
		constIntegral = doubleIntegral(rv_i, mean_i, stdv_i, rv_j, mean_j, stdv_j, rho_zero, threadID, false, 3);
	}

	for (int i = 1; i <= 100; i++) {

		// Evaluate the function
		residualFunctionSensitivity(constIntegral, rho_zero, drho_old, rv_i, mean_i, stdv_i, rv_j, mean_j, stdv_j, &f, threadID);


		///////////////////////////////////////////////////////////////////////////////////////////
		// Evaluate perturbed function
		residualFunctionSensitivity(constIntegral, rho_zero, (drho_old+pert),  rv_i, mean_i, stdv_i, rv_j, mean_j, stdv_j, &perturbed_f, threadID);

		// Evaluate derivative of function
		df = (perturbed_f - f) / pert;
		///////////////////////////////////////////////////////////////////////////////////////////////////////////


		if (qAbs(df) < 1.0e-15) {
			rCritical(threadID) << "Error: The derivative used in the Newton algorithm within the Nataf transformation is zero.";
		}
		else {

			// Take a Newton step
			drho_new = drho_old - f/df;

			if (threadID == 0)
				QCoreApplication::processEvents();



			// Check convergence; quit or continue
			if (fabs(1.0-fabs(drho_old/drho_new)) < tol) {
				result = drho_new;
				return result;
			}
			else {
				if (i==100) {
					rCritical(threadID) << "Error: The Newton algorithm to find correlation sensitivity within the Nataf transformation did not converge.";
					result = 0.0;
					return result;
				}
				else {
					drho_old = drho_new;
				}

			}
		}
	}


	return result;
}

int RNatafTransformer::computeJointPDFSensitivityToDistributionParameters(const gsl_vector *x, const gsl_vector *u,  QList<RPointer<RRandomVariable> > sensitivityRandomVariableList, gsl_vector *dfdMean_1_f, gsl_vector *dfdStdv_1_f, double NeglectCorrelationInSensitivity, int threadID)
{
	// This method computes (df/dMean)*(1/f) and (df/dStdv)*(1/f) where f is the joint probability distribution of 
	// all random variables in the original space and Mean and Stdv are the mean and standard deviation of the desired random variable.
	
	// Keep the GUI responsive
	if (threadID == 0)
		QCoreApplication::processEvents();

	if (NeglectCorrelationInSensitivity) {

		// Compute df/dMean & df/dStdv
		double dfdMean = 0.0;
		double dfdStdv = 0.0;
		for (int i = 0; i < sensitivityRandomVariableList.count(); i++) {
			
			RContinuousRandomVariable *RV = qobject_cast<RContinuousRandomVariable *>(sensitivityRandomVariableList[i].data());
			
			// Compute f: Value of marginal PDF in original space at x
			double f = RV->getPDFValue(RV->getCurrentValue(threadID), threadID);

			// Compute f: Value of marginal PDF sensitivity in original space at x
			int status = RV->computePDFSensitivityToParameters(RV->getCurrentValue(threadID), &dfdMean, &dfdStdv, threadID);
			if (status) {
				rCritical(threadID) << "Error in computing PDF sensitivity in Sampling sensitivity computation";
				return -1;
			}

			double dCost_dMean = RV->getIncrementalCostOfMeanValue(threadID);
			double dCost_dStdv = RV->getIncrementalCostOfStandardDeviationValue(threadID);

			gsl_vector_set(dfdMean_1_f, i, (dfdMean / f)/dCost_dMean);
			gsl_vector_set(dfdStdv_1_f, i, (dfdStdv / f)/dCost_dStdv);

			/*double cv = RV->getCurrentValue(threadID);///Temprory just for normal dist
			double m = RV->getMeanValue(threadID);
			double s = RV->getStandardDeviationValue(threadID);
			double value = (cv-m)/gsl_sf_pow_int(s,2);
			gsl_vector_set(dfdMean_1_f, i, value);
			gsl_vector_set(dfdStdv_1_f, i, 0);*/
		}

		return 0;
	} else {
	
		// Pre-calculations //

		// Allocating necessary matrices
		gsl_matrix *tempMatrix = gsl_matrix_calloc(numRVs, numRVs);
		gsl_matrix *tempMatrix2 = gsl_matrix_calloc(numRVs, numRVs);

		// compute determinant of Jacobian matrix
		gsl_matrix *JacobianInverse = gsl_matrix_calloc(numRVs, numRVs);
		int status = computeJacobian(x, u, JacobianInverse, RTransformer::x_u, threadID);
		if (status < 0) {
			rCritical(threadID) << "Error: Cannot compute Jacobian matrix.";
			return -1;
		}

		int signum2 = 0;
		gsl_permutation *p = gsl_permutation_alloc(numRVs);
		gsl_matrix *JacobianInverseLU = gsl_matrix_calloc(numRVs, numRVs);
		gsl_matrix_memcpy(JacobianInverseLU, JacobianInverse);

		status = gsl_linalg_LU_decomp(JacobianInverseLU, p, &signum2);
		if (status) { 
			rCritical(threadID) << "Error in GSL:" << gsl_strerror(status);
			return -1;
		}
		double detJacobian = 1.0 / gsl_linalg_LU_det(JacobianInverseLU, signum2); 

		gsl_permutation_free(p);
		if (JacobianInverseLU) {
			gsl_matrix_free(JacobianInverseLU);
			JacobianInverseLU = 0;
		}
				
		// Compute adj(J) = J^-1 * det(J)
		gsl_matrix *adj_J = gsl_matrix_calloc(numRVs, numRVs);
		gsl_matrix_memcpy(adj_J, JacobianInverse);
		gsl_matrix_scale(adj_J, detJacobian);

		// Compute f: Value of joint PDF in original space at x
		double f = phi(u) * detJacobian;
		
		// Compute du_dMean & du_dStdv
		gsl_matrix *dy_dMean = gsl_matrix_calloc(numRVs, numRVs);
		gsl_matrix *dy_dStdv = gsl_matrix_calloc(numRVs, numRVs);

		status = computeYSensitivityToDistributionParameters(x, dy_dMean, dy_dStdv, threadID, sensitivityRandomVariableList);
		if (status < 0) {
			rCritical(threadID) << "Error: Cannot compute the sensitivity of variables in the standard normal space with respect to the distribution parameters.";
			return -1;
		}
	
		// Compute z from y
		gsl_vector *z = gsl_vector_calloc(numRVs);

		status = yToz(u, z, threadID);
		if (status) {
			rCritical(threadID) << "Error in GSL:" << gsl_strerror(status);
			return -1;
		}

		// Main calculations //

		// df/dMean & df/dStdv consist of two expressions as follows:

		// df/dMean: (dphi/dMean)*det(Jacobian) + (phi)*(d(det(Jacobian))/dMean)
		// df/dStdv: (dphi/dStdv)*det(Jacobian) + (phi)*(d(det(Jacobian))/dStdv)
	

		// Loop on random variables for which the sensitivity is needed
		for (int i = 0; i < sensitivityRandomVariableList.count(); i++) {
			// Compute the first part of derivative (dphi/dMean)*det(Jacobian) & (dphi/dStdv)*det(Jacobian)

			double sumMean = 0; // keeps the SIGMA{(dphi/dui)*(dui/dMean)}
			double sumStdv = 0; // keeps the SIGMA{(dphi/dui)*(dui/dStdv)}
			int indexOfSRV = 0;

			RContinuousRandomVariable *RV = qobject_cast<RContinuousRandomVariable *>(sensitivityRandomVariableList[i].data());

				// Compute (dphi/du)*(du/dMean) & (dphi/du)*(du/dStdv)
			//indexOfSRV = findSensitivityRandomVariables(theRandomVariableList, sensitivityRandomVariableList[i]->objectName());
			indexOfSRV = randomVariableList->indexOf(RV);
			for (int j = 0; j < numRVs ; j++) {
				sumMean += dphi_dui(u, j) * gsl_matrix_get(dy_dMean, indexOfSRV, j);
				sumStdv += dphi_dui(u, j) * gsl_matrix_get(dy_dStdv, indexOfSRV, j);
			}
				// Verify (dphi/dMean) by perturbation
			/*
			double meanBackup = RV->getMeanValue(threadID);
			double initialPhi = phi(u);
			RV->setMeanValue(meanBackup+0.0001, threadID);
			gsl_vector *u_pert = gsl_vector_calloc(numRVs);
			transformPointToStandardNormalSpace(x, u_pert, threadID);
			double pertPhi = phi(u_pert);

			double dphi_dmu = sumMean;
			double dphi_dmu_pert = (pertPhi - initialPhi)/0.0001;
			
			double dphi_dmu_error = (dphi_dmu_pert-dphi_dmu)/dphi_dmu_pert*100;
			rDebug(threadID) << dphi_dmu_error;

			RV->setMeanValue(meanBackup, threadID);*/

			//	// Verify (dy/dMean) by perturbation
			//
			//double meanBackup = RV->getMeanValue(threadID);
			//gsl_vector *initialu =  gsl_vector_calloc(numRVs);
			//gsl_vector_memcpy(initialu, u);
			//RV->setMeanValue(meanBackup+0.01, threadID);
			//gsl_vector *u_pert = gsl_vector_calloc(numRVs);
			//transformPointToStandardNormalSpace(x, u_pert, threadID);

			//gsl_vector *du_dmu = gsl_vector_calloc(numRVs);
			//gsl_matrix_get_row(du_dmu,dy_dMean, indexOfSRV); 

			//gsl_vector *du_dmu_pert = gsl_vector_calloc(numRVs);
			//gsl_vector_sub(u_pert, initialu);
			//gsl_vector_scale(u_pert, 1/0.01);
			//gsl_vector_memcpy(du_dmu_pert, u_pert);

			//gsl_vector *du_dmu_error = gsl_vector_calloc(numRVs);
			//gsl_vector_sub(du_dmu_pert, du_dmu);
			//gsl_vector_div(du_dmu_pert, du_dmu);
			//gsl_vector_scale(du_dmu_pert, 100);
			//gsl_vector_memcpy(du_dmu_error, du_dmu_pert);
			//double min_out = 0;
			//double max_out = 0;
			//gsl_vector_minmax (du_dmu_error, &min_out, &max_out);

			//rDebug(threadID) << max_out << '\t' << min_out;

			//RV->setMeanValue(meanBackup, threadID);



		
			double DphiDMeanDetJ = sumMean * detJacobian;
			double DphiDStdvDetJ = sumStdv * detJacobian;

			// Compute the second part of derivative: (phi)*(d(det(Jacobian))/dMean) & (phi)*(d(det(Jacobian))/dStdv)

			// Compute the derivative of jacobian matrix wrt Mean & Stdv: dJdMean & dJdStdv
			// It consists of two terms:
			// First: (dL^-1/dMean)*[fi(xi)/phi(zi)] & (dL^-1/dStdv)*[fi(xi)/phi(zi)]

			// step 1: computing dL^-1/dMean & dL^-1/dStdv
			gsl_matrix *dLinvdMean = gsl_matrix_calloc(numRVs, numRVs);
			gsl_matrix *dLinvdStdv = gsl_matrix_calloc(numRVs, numRVs);

			//status = computedLinvdMean(RV, dLinvdMean, dLinvdStdv); 
			//if (status) {
			//	rCritical(threadID) << "Error in computing dLinv/dMean and dLinv/dStdv in Sampling sensitivity computation.";
			//	return -1;
			//}		
			gsl_matrix_memcpy(dLinvdMean, dLinvdMeanList[indexOfSRV]);
			gsl_matrix_memcpy(dLinvdStdv, dLinvdStdvList[indexOfSRV]);

			// step 2: computing [fi(xi)/phi(zi)] which is a diagonal matrix (Jacobian of uncorrelated transformation)
			gsl_matrix *fiphi = gsl_matrix_calloc(numRVs, numRVs);

			// [fi(xi)/phi(zi)] = L * Jacobian
			/*status = gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, L, Jacobian, 0.0, fiphi);
			if (status) {
				rCritical(threadID) << "Error in GSL:" << gsl_strerror(status);
				return -1;
			}*/
			for (int j = 0; j < numRVs; j++) {
				gsl_matrix_set(fiphi, j, j, (RV -> getPDFValue(gsl_vector_get(x ,j), threadID)) / gsl_ran_ugaussian_pdf(gsl_vector_get(z ,j)));
			}

			// Final: Computing first term
			gsl_matrix *DLinvDMean_fiphi = gsl_matrix_calloc(numRVs, numRVs);
			status = gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, dLinvdMean, fiphi, 0.0, DLinvDMean_fiphi);
			if (status) {
				rCritical(threadID) << "Error in GSL:" << gsl_strerror(status);
				return -1;
			}

			gsl_matrix *DLinvDStdv_fiphi = gsl_matrix_calloc(numRVs, numRVs);
			status = gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, dLinvdStdv, fiphi, 0.0, DLinvDStdv_fiphi);
			if (status) {
				rCritical(threadID) << "Error in GSL:" << gsl_strerror(status);
				return -1;
			}

			// Second: (L^-1)*[{(dfi(xi)/dMean)*phi(zi)-(fi(xi)*(dphi(zi)/dMean)}/phi(zi)^2] (so for Stdv)
		
			double dzdMean = 0.0;
			double dzdStdv = 0.0;
			gsl_matrix_set_zero(tempMatrix);
			gsl_matrix_set_zero(tempMatrix2);
			double tempValueMean = 0.0;
			double tempValueStdv = 0.0;
			double factor1 = 0.0;
			double factor2 = 0.0;
			double dphidMean = 0.0;
			double dphidStdv = 0.0;
			double dfidMean = 0.0;
			double dfidStdv = 0.0;

			// Loop to construct the diagonal matrix in the bracket in above formula
			for (int j = 0; j < numRVs; j++) {
				if ( j == indexOfSRV ) {
						// step 1: computing dphi(zi)/dMean = dphi(zi)/dzi * dzi/dMean (so for Stdv)
					status = computeDzDMeanStdv(RV, gsl_vector_get(x, j), gsl_vector_get(z, j), &dzdMean, &dzdStdv, threadID);
					if (status) {
						rCritical(threadID) << "Error in computing dz/dMean or dz/dStdv for variable" << RV->objectName() << ".";
						return -1;
					}

					factor1 = -1.0 * gsl_vector_get(z ,j) * gsl_ran_ugaussian_pdf(gsl_vector_get(z ,j));
					dphidMean  = factor1 * dzdMean;
					dphidStdv  = factor1 * dzdStdv;
			
					// step 2: computing dfi(xi)/dMean & dfi(xi)/dStdv
				
					status = RV -> computePDFSensitivityToParameters(gsl_vector_get(x ,j), &dfidMean, &dfidStdv, threadID);
					if (status) {
						rCritical(threadID) << "Error in computing PDF sensitivity in Sampling sensitivity computation";
						return -1;
					}
				

					// tempValue = {(dfi(xi)/dMean)*phi(zi)-(fi(xi)*(dphi(zi)/dMean)}/phi(zi)^2 (so for Stdv)
					factor2 = gsl_sf_pow_int(gsl_ran_ugaussian_pdf(gsl_vector_get(z ,j)),-2); // 1/phi(zi)^2
					tempValueMean = factor2 * (dfidMean * gsl_ran_ugaussian_pdf(gsl_vector_get(z ,j)) - dphidMean * RV -> getPDFValue(gsl_vector_get(x ,j), threadID));
					tempValueStdv = factor2 * (dfidStdv * gsl_ran_ugaussian_pdf(gsl_vector_get(z ,j)) - dphidStdv * RV -> getPDFValue(gsl_vector_get(x ,j), threadID));
			
					gsl_matrix_set(tempMatrix, j, j, tempValueMean);
					gsl_matrix_set(tempMatrix2, j, j, tempValueStdv);
				}
				else
				{
					gsl_matrix_set(tempMatrix, j, j, 0.0);
					gsl_matrix_set(tempMatrix2, j, j, 0.0);
				}
			}

			// Final: Computing second term					
			gsl_matrix *Linv_DfiphiDMean = gsl_matrix_calloc(numRVs, numRVs);
			status = gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, Linv[threadID], tempMatrix, 0.0, Linv_DfiphiDMean);			
			if (status) {
				rCritical(threadID) << "Error in GSL:" << gsl_strerror(status);
				return -1;
			}

			gsl_matrix *Linv_DfiphiDStdv = gsl_matrix_calloc(numRVs, numRVs);
			status = gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, Linv[threadID], tempMatrix2, 0.0, Linv_DfiphiDStdv);			
			if (status) {
				rCritical(threadID) << "Error in GSL:" << gsl_strerror(status);
				return -1;
			}

			// Computing the derivative of jacobian matrix wrt Mean: dJdMean & dJdStdv
			gsl_matrix *dJdMean = gsl_matrix_calloc(numRVs, numRVs);
			status = gsl_matrix_add(DLinvDMean_fiphi, Linv_DfiphiDMean);
			if (status) {
				rCritical(threadID) << "Error in GSL:" << gsl_strerror(status);
				return -1;
			}
			gsl_matrix_memcpy(dJdMean, DLinvDMean_fiphi);

			gsl_matrix *dJdStdv = gsl_matrix_calloc(numRVs, numRVs);
			status = gsl_matrix_add(DLinvDStdv_fiphi, Linv_DfiphiDStdv);
			if (status) {
				rCritical(threadID) << "Error in GSL:" << gsl_strerror(status);
				return -1;
			}
			gsl_matrix_memcpy(dJdStdv, DLinvDStdv_fiphi);


			////// Temprory: Verify (dJ/dMean) by perturbation
			//
			//double meanBackup = RV->getMeanValue(threadID);
			//gsl_matrix *initialJ =  gsl_matrix_calloc(numRVs, numRVs);
			//gsl_matrix_memcpy(initialJ, Jacobian);
			//RV->setMeanValue(meanBackup+.01, threadID);
			//gsl_vector *u_pert = gsl_vector_calloc(numRVs);
			//transformPointToStandardNormalSpace(x, u_pert, threadID);

			//gsl_matrix *J_pert = gsl_matrix_calloc(numRVs, numRVs);
			//int status = computeJacobian(x, u_pert, J_pert , RTransformer::u_x);
			//if (status < 0) {
			//	rCritical(threadID) << "Error: Cannot compute Jacobian matrix.";
			//	return -1;
			//}
			///*theDomain->print(initialJ);
			//rDebug(threadID) << "***";
			//theDomain->print(J_pert, threadID);
			//rDebug(threadID) << "+++";*/

			//gsl_matrix *dJ_dmu = gsl_matrix_calloc(numRVs, numRVs);
			//gsl_matrix_memcpy(dJ_dmu, dJdMean); 

			//gsl_matrix *dJ_dmu_pert = gsl_matrix_calloc(numRVs, numRVs);
			//gsl_matrix_sub(J_pert, initialJ);
			//gsl_matrix_scale(J_pert, 1/0.01);
			//gsl_matrix_memcpy(dJ_dmu_pert, J_pert);

			//gsl_matrix *dJ_dmu_error = gsl_matrix_calloc(numRVs, numRVs);
			//gsl_matrix_sub(dJ_dmu_pert, dJ_dmu);
			//gsl_matrix_div_elements(dJ_dmu_pert, dJ_dmu);
			//gsl_matrix_scale(dJ_dmu_pert, 100);
			//gsl_matrix_memcpy(dJ_dmu_error, dJ_dmu_pert);
			//double min_out = gsl_matrix_get(dJ_dmu_error, 0, 0);
			//double max_out = gsl_matrix_get(dJ_dmu_error, 0, 0);
			//gsl_matrix_minmax (dJ_dmu_error, &min_out, &max_out);

			//for (int j = 0; j < numRVs; j++) {
			//	for (int k = 0; k < numRVs; k++) {
			//		if (gsl_matrix_get(dJ_dmu_error, j, k) > max_out && gsl_matrix_get(dJ_dmu_error, j, k)==gsl_matrix_get(dJ_dmu_error, j, k)) {
			//			max_out = gsl_matrix_get(dJ_dmu_error, j, k);
			//		}
			//		if (gsl_matrix_get(dJ_dmu_error, j, k) < min_out && gsl_matrix_get(dJ_dmu_error, j, k)==gsl_matrix_get(dJ_dmu_error, j, k)) {
			//			min_out = gsl_matrix_get(dJ_dmu_error, j, k);
			//		}
			//	}
			//}

			////theDomain->print(dJ_dmu_pert);
			////rDebug(threadID) << "+++";
			////rDebug(threadID) << max_out << '\t' << min_out;
			//theDomain->print(dJ_dmu, threadID);
			//rDebug(threadID) << "***";
			//theDomain->print(dJ_dmu_pert);
			//rDebug(threadID) << "+++";

			//RV->setMeanValue(meanBackup, threadID);
			///////////////////////////////////////////////////////////////////////////

			// continue Computeing the second part of derivative: (phi)*(d(det(Jacobian))/dMean) & (phi)*(d(det(Jacobian))/dStdv)

			// compute d(det(Jacobian))/dMean = tr(adj(J)*(dJ/Mean)) (so for Stdv)

			// compute tr(adj(J)*(dJ/Mean)) & tr(adj(J)*(dJ/Stdv))
			gsl_matrix_set_zero(tempMatrix);
			gsl_matrix_set_zero(tempMatrix2);

			status = gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, adj_J, dJdMean, 0.0, tempMatrix);			
			if (status) {
				rCritical(threadID) << "Error in GSL:" << gsl_strerror(status);
				return -1;
			}

			status = gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, adj_J, dJdStdv, 0.0, tempMatrix2);			
			if (status) {
				rCritical(threadID) << "Error in GSL:" << gsl_strerror(status);
				return -1;
			}

			double traceMean = 0;
			double traceStdv = 0;

			for (int j = 0; j < numRVs; j++) {
				traceMean += gsl_matrix_get(tempMatrix, j, j);
				traceStdv += gsl_matrix_get(tempMatrix2, j, j);
			} // d(det(Jacobian))/dMean and d(det(Jacobian))/dStdv  are computed

			





			double phi_dDetJdMean = traceMean * phi(u);
			double phi_dDetJdStdv = traceStdv * phi(u);

			// Finally, df/dMean * 1/f is computed and is set in the ith row of vectors dfdMean_1_f & dfdStdv_1_f

			gsl_vector_set(dfdMean_1_f, i, (DphiDMeanDetJ + phi_dDetJdMean)/f);
			gsl_vector_set(dfdStdv_1_f, i, (DphiDStdvDetJ + phi_dDetJdStdv)/f);


			//Temprory: Check the claculated dfdMean with perturbation results
			// Verify df/dMean by perturbation
			//double meanBackup = RV->getMeanValue(threadID);
			//double initialf = detJ * phi(u);
			//RV->setMeanValue(meanBackup+0.1, threadID);
			//gsl_vector *u_pert = gsl_vector_calloc(numRVs);
			//transformPointToStandardNormalSpace(x, u_pert, threadID);
			//
			//gsl_matrix *Jacobian_pert = gsl_matrix_calloc(numRVs, numRVs);
			//int status = computeJacobian(x, u_pert, Jacobian_pert , RTransformer::u_x);
			//if (status < 0) {
			//	rCritical(threadID) << "Error: Cannot compute Jacobian matrix.";
			//	return -1;
			//}

			//gsl_matrix *Jcopy_pert = gsl_matrix_calloc(numRVs, numRVs);
			//gsl_matrix_memcpy(Jcopy_pert, Jacobian_pert);

			//int signum2_pert = 0;
			//gsl_permutation *p_pert = gsl_permutation_alloc(numRVs);

			//status = gsl_linalg_LU_decomp(Jcopy_pert, p_pert, &signum2_pert);
			//if (status) { 
			//	rCritical(threadID) << "Error in GSL:" << gsl_strerror(status);
			//	return -1;
			//}

			//double detJ_pert = gsl_linalg_LU_det(Jacobian_pert, signum2_pert);
			//double DdetJ_dmu_pert = (detJ_pert - detJ)/0.01;
			//double DdetJ_dmu_error = (DdetJ_dmu_pert-traceMean)/traceMean*100;
			//rDebug(threadID) << DdetJ_dmu_pert << "\t" << traceMean << "\t" << DdetJ_dmu_error;

			//double f_pert = detJ_pert * phi(u_pert); 

			//double df_dmu_pert = (f_pert - initialf)/0.1;
			//
			//double df_dmu_error = (df_dmu_pert-(DphiDMeanDetJ + phi_dDetJdMean))/df_dmu_pert*100;
			////rDebug(threadID) << df_dmu_pert << "\t" << (DphiDMeanDetJ + phi_dDetJdMean) << "\t" << df_dmu_error;

			//RV->setMeanValue(meanBackup, threadID);

			//gsl_vector_set(dfdMean_1_f, i, df_dmu_pert/f);
			
		
			// Unallocate the matrices and vectors
			if (dLinvdMean) {
				gsl_matrix_free(dLinvdMean);
				dLinvdMean = 0;
			}
			if (dLinvdStdv) {
				gsl_matrix_free(dLinvdStdv);
				dLinvdStdv = 0;
			}
			if (fiphi) {
				gsl_matrix_free(fiphi);
				fiphi = 0;
			}
			if (DLinvDMean_fiphi) {
				gsl_matrix_free(DLinvDMean_fiphi);
				DLinvDMean_fiphi = 0;
			}
			if (DLinvDStdv_fiphi) {
				gsl_matrix_free(DLinvDStdv_fiphi);
				DLinvDStdv_fiphi = 0;
			}
			if (Linv_DfiphiDMean) {
				gsl_matrix_free(Linv_DfiphiDMean);
				Linv_DfiphiDMean = 0;
			}
			if (Linv_DfiphiDStdv) {
				gsl_matrix_free(Linv_DfiphiDStdv);
				Linv_DfiphiDStdv = 0;
			}
			if (dJdMean) {
				gsl_matrix_free(dJdMean);
				dJdMean = 0;
			}
			if (dJdStdv) {
				gsl_matrix_free(dJdStdv);
				dJdStdv = 0;
			}
		}




		// Unallocate the matrices and vectors
		if (tempMatrix) {
			gsl_matrix_free(tempMatrix);
			tempMatrix = 0;
		}
		if (tempMatrix2) {
			gsl_matrix_free(tempMatrix2);
			tempMatrix2 = 0;
		}
		if (JacobianInverse) {
			gsl_matrix_free(JacobianInverse);
			JacobianInverse = 0;
		}
		if (adj_J) {
			gsl_matrix_free(adj_J);
			adj_J = 0;
		}
		if (dy_dMean) {
			gsl_matrix_free(dy_dMean);
			dy_dMean = 0;
		}
		if (dy_dStdv) {
			gsl_matrix_free(dy_dStdv);
			dy_dStdv = 0;
		}
		if (z) {
			gsl_vector_free(z);
			z = 0;
		}

		return 0;
	}
}

void RNatafTransformer::freeDLinvMaps()
{
	for (int i = 0; i < dLinvdMeanList.count(); i++) {
		gsl_matrix *tempMatrix = dLinvdMeanList[i];
		if (tempMatrix) {
			gsl_matrix_free(tempMatrix);
		}
	}
	dLinvdMeanList.clear();

	for (int i = 0; i < dLinvdStdvList.count(); i++) {
		gsl_matrix *tempMatrix = dLinvdStdvList[i];
		if (tempMatrix) {
			gsl_matrix_free(tempMatrix);
		}
	} 
	dLinvdStdvList.clear();

	linvdMapsHaveBeenFilled = false;
}

RPointer<RTransformer> RNatafTransformer::createNewTransformerOfTheSameType(){
    QString x= QString(""); //salarifard
    return new RNatafTransformer(theDomain, x);
}

void RNatafTransformer::initializeForParallelComputation(int threadID){

	if (L[threadID]) {
		gsl_matrix_free(L[threadID]);
		L[threadID] = 0;
	}
	if (Linv[threadID]) {
		gsl_matrix_free(Linv[threadID]);
		Linv[threadID] = 0;
	}
	if (R0[threadID]) {
		gsl_matrix_free(R0[threadID]);
		R0[threadID] = 0;
	}

	L[threadID] = gsl_matrix_calloc(numRVs, numRVs);
	Linv[threadID] = gsl_matrix_calloc(numRVs, numRVs);
	R0[threadID] = gsl_matrix_calloc(numRVs, numRVs);

	gsl_matrix_memcpy(L[threadID], L[0]);
	gsl_matrix_memcpy(Linv[threadID], Linv[0]);
	gsl_matrix_memcpy(R0[threadID], R0[0]);
}

bool RNatafTransformer::canBeRunParallelly(){
	return true;
}
