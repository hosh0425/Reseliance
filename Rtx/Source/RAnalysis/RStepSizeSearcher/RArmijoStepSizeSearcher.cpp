#include "RArmijoStepSizeSearcher.h"

#include <QCoreApplication>

#include "RDomain.h"
#include "RRandomVariable.h"
#include "RMeritChecker.h"
#include "RTransformer.h"
#include "RFunction.h"

#include <gsl/gsl_blas.h>
#include <gsl/gsl_math.h>

RArmijoStepSizeSearcher::RArmijoStepSizeSearcher(QObject *parent, QString name)
	: RStepSizeSearcher(parent, name)
{
	// Initializing data members
	//$$ theTransformer = 0;
	//$$ theMeritChecker = 0;
	//theRootFinder = 0;
	theMaximumReductions = 10;
	theBase = 0.5;
	theInitialStepSize = 1.0;
	theInitialStepsCount = 2;
	theSphereRadius = 50.0;
	theSphereDistance = 0.1;
	theSphereEvolution = 0.5;

	isCloseToSphere = false;
	theFunctionValue = 0;
}

RArmijoStepSizeSearcher::~RArmijoStepSizeSearcher()
{

}

QObject * RArmijoStepSizeSearcher::getTransformer() const	
{	
	return theTransformer.data();
}	

void RArmijoStepSizeSearcher::setTransformer(QObject *value)	
{	
	//theTransformer = qobject_cast<RPointer<RTransformer >>(value);
	theTransformer = value;
}	

QObject * RArmijoStepSizeSearcher::getMeritChecker() const	
{	
	return theMeritChecker.data();
}	

void RArmijoStepSizeSearcher::setMeritChecker(QObject *value)	
{	
	//theMeritChecker = qobject_cast<RPointer<RMeritChecker >>(value);
	theMeritChecker = value;
}	

//QObject * RArmijoStepSizeSearcher::getRootFinder() const	
//{	
//	return theRootFinder;
//}	
//
//void RArmijoStepSizeSearcher::setRootFinder(QObject *value)	
//{	
//	theRootFinder = qobject_cast<RRootFinder *>(value);
//}	

int RArmijoStepSizeSearcher::getMaximumReductions() const	
{	
	return theMaximumReductions;
}	

void RArmijoStepSizeSearcher::setMaximumReductions(int value)	
{	
	theMaximumReductions = value;
}	

double RArmijoStepSizeSearcher::getBase() const	
{	
	return theBase;
}	

void RArmijoStepSizeSearcher::setBase(double value)	
{	
	theBase = value;
}	

double RArmijoStepSizeSearcher::getInitialStepSize() const	
{	
	return theInitialStepSize;
}	

void RArmijoStepSizeSearcher::setInitialStepSize(double value)	
{	
	theInitialStepSize = value;
}	

int RArmijoStepSizeSearcher::getInitialStepsCount() const	
{	
	return theInitialStepsCount;
}	

void RArmijoStepSizeSearcher::setInitialStepsCount(int value)	
{	
	theInitialStepsCount = value;
}	

double RArmijoStepSizeSearcher::getSphereRadius() const	
{	
	return theSphereRadius;
}	

void RArmijoStepSizeSearcher::setSphereRadius(double value)	
{	
	theSphereRadius = value;
}	

double RArmijoStepSizeSearcher::getSphereDistance() const	
{	
	return theSphereDistance;
}	

void RArmijoStepSizeSearcher::setSphereDistance(double value)	
{	
	theSphereDistance = value;
}	

double RArmijoStepSizeSearcher::getSphereEvolution() const	
{	
	return theSphereEvolution;
}	

void RArmijoStepSizeSearcher::setSphereEvolution(double value)	
{	
	theSphereEvolution = value;
}	

double RArmijoStepSizeSearcher::getStepSize() const
{
	return theStepSize;
}

int RArmijoStepSizeSearcher::computeStepSize(int iterationIndex, gsl_vector *u, RFunction *function, double functionValue, gsl_vector *functionGradient, gsl_vector const *stepDirection, QList<RRandomVariable *> *randomVariableList)
{
	// Initial declarations
	bool isOutsideSphere;
	bool isSecondTime;
	bool analysisConvergence;
	double result;
	int count = u->size;
	gsl_vector *xNew = gsl_vector_calloc(count);
	gsl_vector *uNew = gsl_vector_calloc(count);
	gsl_vector *newFunctionGradient = gsl_vector_calloc(count);

	// Inform user
	if (getOutputDisplayLevel() >= RObject::Moderate) {
		rDebug() << "Entering the Armijo rule to compute step size ...";
	}
	
	// Check that the norm of the gradient is not zero
	double gradNorm = gsl_blas_dnrm2(functionGradient);
	if (gradNorm == 0.0) {
		rCritical() << "Error in" << objectName() << ": The norm of the gradient is zero.";
		return -1;
	}

	// Check if this potentially could be the second time close to the surface
	if (isCloseToSphere) {
		isSecondTime = true;
	} else {
		isSecondTime = false;
	}

	double newLambda;
	double newFunctionValue;
	int i = 0;
	bool continueFlag = true;

	while (continueFlag) {
		// A line to keep the GUI responsive
		QCoreApplication::processEvents();

		// Set the first trial step size
		if (iterationIndex <= theInitialStepsCount) {
			newLambda = theInitialStepSize;
		} else {
			newLambda = 1.0;
		}

		if (i > 0) {
			// Notify user that step sizes are being reduced
			if (getOutputDisplayLevel() > RObject::None) {			
				rDebug() << "Armijo: Reducing step size...";			
			}

			// Cut the step size in half (or whichever theBase the user has set) and try that instead
			newLambda *= gsl_pow_int(theBase, i);
		}

		// Take a trial step in standard normal space: Compute using BLAS: uNew = u + searchDirection * newLambda
		gsl_vector_memcpy(uNew, stepDirection);
		gsl_vector_scale(uNew, newLambda);
		gsl_vector_add(uNew, u);

		// Check if we are beyond the bounding sphere
		double uNewNorm = gsl_blas_dnrm2(uNew);
		if (uNewNorm > theSphereRadius) {
			isOutsideSphere = true;

			if (i == 0) {
				// Set some dummy values to newFunctionValue and newFunctionGradient; it doesn't matter that the merit functions then will be 'wrong'; the step will fail in any case because it is outside the sphere. 
				newFunctionValue = functionValue;
				gsl_vector_memcpy(newFunctionGradient, functionGradient);
			}

			// Inform the user
			if (getOutputDisplayLevel() > RObject::None) {
				rDebug() << "Armijo: Skipping the function evaluation because of the hyper sphere requirement.";
			}
		} else {
			isOutsideSphere = false;

			// Register where this u was close to the sphere surface
			if ((uNewNorm > theSphereRadius - theSphereDistance) && (uNewNorm < theSphereRadius + theSphereDistance)) {
				isCloseToSphere = true;
				if (isSecondTime) {
					theSphereRadius = theSphereRadius + theSphereEvolution;
				}
			} else {
				isCloseToSphere = false;
			}

			if (getOutputDisplayLevel() > RObject::Minimum) {
				rDebug() <<  "Armijo: Function evaluation at distance" << uNewNorm;
			}

			// Transform the trial point into original space
			result = theTransformer.data()->transformPointToOriginalSpace(uNew, xNew,0);
			if (result < 0) {
				rCritical() << "Error: Could not transform from standard-normal space to original space.";
				return -1;
			}
			
			// UPDATING THE CURRENT VALUE OF RANDOM VARIABLES TO THE VALUE OF NEW REALIZATIONS
			for (int i = 0; i < randomVariableList->count(); i++) {
				RRandomVariable *randomVariable = randomVariableList->value(i);
				randomVariable->setCurrentValue(gsl_vector_get(xNew, i));

			// Evaluate the function
			analysisConvergence = true;
			}

			// Calling the method to update the isAnalyzed flags of RResponses due to the new realization of random variables
			function->updateAnalysisStatus(0);
			
			// NOTE: THE "true" IN THE FOLLOWING LINE MEANS THAT IF DDM IS SELECTED, THE DDMs WILL BE EVLUATED WITH FUNCTION AT EACH FUNCTION EVALUATION, EVEN THOUGH THEY ARE ONLY NEEDED AT THE LAST ITERATION FOR STEP SIZE: THIS IS NOT EFFICIENT
			//		 BUT IT'S INEVITABLE. THE ALTERNATIVE IS TO EVALUATE THE FUNCTION ONCE MORE WITH DDMs WHEN THE MERIT CHECK BECOMES OK.
			result = function->evaluateFunction(true, RObject::RandomVariable,0);
			if (result < 0) {
				rCritical() << "Error in" << objectName() << ": Could not evaluate the function" << function->objectName() << ".";
				return -1;
			}
			newFunctionValue = function->getFunctionValue();		

			//// Possibly project the point onto the limit-state surface
			//// (it is assumed that the user has provided a projection object 
			//// if this is to be done)
			//// Note that this has effect on the previously computed search direction
			//// because the new values of the performance function and its gradient
			//// is being used...
			//// Q: Is the uNew point also being kept in the orchestrating algorithm?
			//if ((theRootFinder != 0) && (i > 0)) {
			//	theRootFinder->findLimitStateSurface(2, newFunctionValue, functionGradient, uNew);
			//}
		}

		// Increment counter
		i++;

		// Check if we need to go on
		continueFlag = false;

		int checkResult = theMeritChecker.data()->check(u, functionValue, functionGradient, newLambda, stepDirection, newFunctionValue);

		if (checkResult < 0) {
			continueFlag = true;
		}
		if (!analysisConvergence) {
			continueFlag = true;
		}
		if (isOutsideSphere) {
			continueFlag = true;
		}
		if (i > theMaximumReductions) {
			continueFlag = false;
		} 	
	}

	theStepSize = newLambda;
	theFunctionValue = newFunctionValue;

	//DELETE
	/*gsl_vector_sub(uNew, u);
	double uNewNorm = gsl_blas_dnrm2(uNew);
	if (iterationIndex > 1) {
		if (qAbs((uNewNorm - oldDistance) / oldDistance) < 0.01) {
			theMaximumReductions--;
			if (theMaximumReductions == 1) {
				theMaximumReductions++;
			}
		}
		if (qAbs((uNewNorm - oldDistance) / oldDistance) > 0.2) {
			theMaximumReductions++;
			if (theMaximumReductions > 10) {
				theMaximumReductions = 10;
			}
		}
	}
	oldDistance = uNewNorm;*/


	if (xNew) {
		gsl_vector_free(xNew);
		xNew = 0;
	}
	if (uNew) {
		gsl_vector_free(uNew);
		uNew = 0;
	}
	if (newFunctionGradient) {
		gsl_vector_free(newFunctionGradient);
		newFunctionGradient = 0;
	}

	return 1;
}

double RArmijoStepSizeSearcher::getFunctionValue()
{
	return theFunctionValue;
}

bool RArmijoStepSizeSearcher::canBeRunParallelly() {	
	return false;
}
