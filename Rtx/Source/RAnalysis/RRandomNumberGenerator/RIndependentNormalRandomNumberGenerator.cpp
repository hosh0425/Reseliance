#include "RIndependentNormalRandomNumberGenerator.h"

#include "RTransformer.h"
#include "RRandomVariable.h"
#include "RDomain.h"

#include <gsl/gsl_cdf.h>

#include <math.h>
#include <QMutex>
#include <QMutexLocker>

RIndependentNormalRandomNumberGenerator::RIndependentNormalRandomNumberGenerator(QObject *parent, QString name)
	: RRandomNumberGenerator(parent, name)
{
	// Initialize data members
	theStandardDeviation = 1.0;
	//$$ theTransformer = 0;

	theStartPoint = 0;
	theRandomVariableList = 0;

	startPointLock = new QMutex();
}


RIndependentNormalRandomNumberGenerator::~RIndependentNormalRandomNumberGenerator()
{
	if (theStartPoint) {
		gsl_vector_free(theStartPoint);
		theStartPoint = 0;
	}

	// free the memory allocated for locks
	delete startPointLock;
}

QObject * RIndependentNormalRandomNumberGenerator::getTransformer() const
{
	return theTransformer.data();
}

void RIndependentNormalRandomNumberGenerator::setTransformer(QObject *value)
{
	theTransformer = qobject_cast<RTransformer *>(value);
}

int RIndependentNormalRandomNumberGenerator::generateStandardNormalNumbers(int count, int threadID)
{
	// Iterate "count" times
	for (int i = 0; i < count; i++) {		
		double stdNormalRandomNumber = gsl_ran_ugaussian(theRandomGenerator[threadID]);
		
		// Add the random number to the vector theGeneratedNumbers
		gsl_vector_set(theGeneratedNumbers[threadID], i, stdNormalRandomNumber);
	}

	return 0;
}

int RIndependentNormalRandomNumberGenerator::generateNumbers(int count, int threadID)
{
	// Allocate theGeneratedNumbers and theStartPoint vector
	if (theGeneratedNumbers[threadID] == 0) {
		theGeneratedNumbers[threadID] = gsl_vector_calloc(count);
	} else if (theGeneratedNumbers[threadID]->size != count) {
		gsl_vector_free(theGeneratedNumbers[threadID]);
		theGeneratedNumbers[threadID] = 0;
		theGeneratedNumbers[threadID] = gsl_vector_calloc(count);
	}
	
	// Attention: if theStartPoint=0 or the size of theStartPoint is not equal to count, theStartPoint will be set to a Zero vector (theStartPoint at mean)
	// start of safe area scope
	{
		QMutexLocker locker(startPointLock);
	
		if (theStartPoint == 0) {
			theStartPoint = gsl_vector_calloc(count);
		} else if (theStartPoint->size != count) {
			gsl_vector_free(theStartPoint);
			theStartPoint = 0;

			// NOTE: CURRENT IMPLEMENTATION ONLY ALLOWS TO SAMPLE AROUND THE ORIGIN IN SCENARIO SAMPLING, BECAUSE OF THIS LINE
			theStartPoint = gsl_vector_calloc(count);
		}
	}
	// end of safe area scope

	// Generate standard-normal numbers
	int result = generateStandardNormalNumbers(count/*, seed*/, threadID);
	if (result < 0) {
		rCritical(threadID) << "Error: Could not generate standard-normal random numbers.";
		return -1;
	}
	
	// Note: Cholesky of the covariance matrix is equal to theStandardDeviation * Identity
	
	// Calcultae using GSL: theGeneratedNumbers = theStartPoint + theStandardDeviation * theGeneratedNumbers;
	gsl_vector_scale(theGeneratedNumbers[threadID], theStandardDeviation);
	gsl_vector_add(theGeneratedNumbers[threadID], theStartPoint);

	return 0;
}

gsl_vector * RIndependentNormalRandomNumberGenerator::getStartingPoint() const
{
	return theStartPoint;
}

int RIndependentNormalRandomNumberGenerator::setRandomVariableList(QList<RRandomVariable *> *passedRandomVariableList)
{
	theRandomVariableList = passedRandomVariableList;

	// Establishing the start point based on the passed list of random variables
	int result  = establishStartPoint();
	if (result < 0) {
		rCritical() << "Error: Could not establish the strating point vector.";
		return -1;
	}

	return 0;
}

int RIndependentNormalRandomNumberGenerator::establishStartPoint()
{	
	// Checking if all pointer properties are filled out
	int status = checkProperties();
	if (status < 0) {
		return -1;
	}
	
	double count = theRandomVariableList->count();

	// Allocating theStartPoint vector
	if (theStartPoint == 0) {
		theStartPoint = gsl_vector_calloc(count);
	} else if (theStartPoint->size != count) {
		gsl_vector_free(theStartPoint);
		theStartPoint = 0;
		theStartPoint = gsl_vector_calloc(count);
	}

	// Zeroing theStartPoint vector if the user has chosen the Origin and returning from the method
	if (theStartPointType == RObject::Origin) {
		gsl_vector_set_zero(theStartPoint);
		return 0;
	}

	// Allocating a temporary vector to store the start point in the original space
	gsl_vector *originalSpaceStartPoint;
	originalSpaceStartPoint = gsl_vector_calloc(count);

	// Iterating on the random variables and extracting the value accordin to user's selection for theStartPointType
	for (int i = 0; i < count; i++) {
		double value;
		RRandomVariable *randomVariable = theRandomVariableList->value(i);
		if (theStartPointType == RObject::CurrentValue) {
			value = randomVariable->getCurrentValue();
		} else if (theStartPointType == RObject::Mean) {
			value = randomVariable->getMeanValue(0);
		}
		gsl_vector_set(originalSpaceStartPoint, i, value);
	}
	
	
	// COMMENTED OUT ASSUMING THAT theRandomVariableList IS ALREADY SET IN theTransformer BY THE ORCHESTRATING ALGORITHM
	// Setting the list of random variables to the transformer
	/*int result = theTransformer->setRandomVariableList(theRandomVariableList);
	if (result < 0) {
		rCritical() << "Error: Could not set the list of random variables to the transformer.";
		return -1;
	}*/

	
	// Transforming the originalSpaceStartPoint to standard-normal space (theStartPoint)
	int result = theTransformer.data()->transformPointToStandardNormalSpace(originalSpaceStartPoint, theStartPoint,0);
	if (result < 0) {
		rCritical() << "Error: Could not transform from original space to standard-normal space.";
		return -1;
	}

	// Deleting the temporary vector originalSpaceStartPoint
	if (originalSpaceStartPoint) {
		gsl_vector_free(originalSpaceStartPoint);
		originalSpaceStartPoint = 0;
	}

	return 0;
}

bool RIndependentNormalRandomNumberGenerator::canBeRunParallelly(){
	return true;
}