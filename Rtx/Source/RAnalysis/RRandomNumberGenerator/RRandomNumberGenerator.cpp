
#include "RRandomNumberGenerator.h"
#include "RRandomVariable.h"
#include "RDomain.h"

#include <gsl/gsl_randist.h>

#include <qmath.h>



RRandomNumberGenerator::RRandomNumberGenerator(QObject *parent, QString name)
	: RObject(parent, name)
{
	theStartPointType = RObject::Origin;
	theSeed = 0;
	theStandardDeviation = 1;

	theRandomGeneratorType = 0;
	

	// allocating memory for the array of the theGeneratedNumbers
	int arraySizeForParallelizedObject = getArraySizeForParallelizedObject();
	theGeneratedNumbers = new gsl_vector*[arraySizeForParallelizedObject];
	theRandomGenerator = new gsl_rng*[arraySizeForParallelizedObject];

	for (int thID = 0 ; thID < arraySizeForParallelizedObject ; thID++){
		theGeneratedNumbers[thID] = 0;
		theRandomGenerator[thID] = 0;
	}
}


RRandomNumberGenerator::~RRandomNumberGenerator()
{

	int arraySizeForParallelizedObject = getArraySizeForParallelizedObject();
	
	// free the memory of each gsl_vector in the theGeneratedNumbers array
	for (int thID = 0 ; thID < arraySizeForParallelizedObject ; thID++){
		if (!theGeneratedNumbers[thID]){
			gsl_vector_free(theGeneratedNumbers[thID]);
		}
	}

	// all random generator engines are created together at reset, so if the index 0 has been created, all of them are created
	if (theRandomGenerator[0]) {

		// freeing the memory of each random generator engine
		for (int thID = 0 ; thID < arraySizeForParallelizedObject ; thID++){
			gsl_vector_free(theGeneratedNumbers[thID]);
		}
	}

	delete [] theGeneratedNumbers;
	theGeneratedNumbers = 0;

	delete [] theRandomGenerator;
	theRandomGenerator = 0;

	theRandomGeneratorType = 0;
}

RObject::RStartPointType RRandomNumberGenerator::getStartPoint() const
{
	return theStartPointType;
}

void RRandomNumberGenerator::setStartPoint(RObject::RStartPointType value)
{
	theStartPointType = value;
	/*if (theStartPointType == RObject::Mean) {
		theStartPointType = RObject::Origin;
	}*/
}

double RRandomNumberGenerator::getStandardDeviation()
{
	return theStandardDeviation;
}

void RRandomNumberGenerator::setStandardDeviation(double value)
{
	theStandardDeviation= value;
}

gsl_vector * RRandomNumberGenerator::getGeneratedNumbers(int threadID) const
{
	return theGeneratedNumbers[threadID];
}

int RRandomNumberGenerator::getSeed()
{
	return theSeed;
}

void RRandomNumberGenerator::setSeed(int value)
{
	theSeed = value;
}

void RRandomNumberGenerator::reset()
{

	int arraySizeForParallelizedObject = getArraySizeForParallelizedObject();

	// if random generators have been created, free them
	if (theRandomGenerator[0]) {

		for (int thID = 0 ; thID < arraySizeForParallelizedObject ; thID++){
			gsl_rng_free(theRandomGenerator[thID]);
		}

		theRandomGeneratorType = 0;
	}

	// Create a random number generator, for each threadID
	for (int thID = 0 ; thID < arraySizeForParallelizedObject; thID++){
		gsl_rng_env_setup();
		theRandomGeneratorType = gsl_rng_default;
		theRandomGenerator[thID] = gsl_rng_alloc(theRandomGeneratorType);


		// if threadID is 0 use seed, 
		if ((thID == 0) || (thID == 1)){
			gsl_rng_set(theRandomGenerator[thID], theSeed);
		}
		// otherwise use a random number generated from generator engine of index 0
		else{
			gsl_rng_set(theRandomGenerator[thID], qFloor(gsl_ran_flat(theRandomGenerator[0],1,100000000)));
		}
	}
}


gsl_rng * RRandomNumberGenerator::getRandomGeneratorEngine(int threadID) const
{
	return theRandomGenerator[threadID];
}
