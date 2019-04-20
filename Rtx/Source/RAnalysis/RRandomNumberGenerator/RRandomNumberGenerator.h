// Last modified by Mojtaba on June 29, 2012

#ifndef RRandomNumberGenerator_H
#define RRandomNumberGenerator_H

#include "RObject.h"

#include <gsl/gsl_vector.h>
#include <gsl/gsl_rng.h>

class RRandomVariable;

class RRandomNumberGenerator : public RObject
{
	Q_OBJECT

	// Indicates the type of the starting point random number generatorr; options are Mean, Origin, and CurrentValue
	Q_PROPERTY(RObject::RStartPointType StartPoint READ getStartPoint WRITE setStartPoint)

	// Sets the stdv of the generated numbers; the default is 1
	Q_PROPERTY(double StandardDeviation READ getStandardDeviation WRITE setStandardDeviation)

	// Sets the seed for the random number generation; the default is 0
	Q_PROPERTY(int Seed READ getSeed WRITE setSeed)

public:
	RRandomNumberGenerator(QObject *parent, QString name);
	virtual ~RRandomNumberGenerator();

	// getter/setter methods for meta-properties
	RObject::RStartPointType getStartPoint() const;
	void setStartPoint(RObject::RStartPointType value);
	double getStandardDeviation();
	void setStandardDeviation(double value);
	int getSeed();
	void setSeed(int value);

	// a thread safe method that generates random numbers for the given threadID ($MTh$)
	virtual int	generateNumbers(int count, int threadID) = 0;

	// Sets the list of random vairables
	virtual int setRandomVariableList(QList<RRandomVariable *> *passedRandomVariableList) = 0;

	// Returns the vector of starting point in the standard-normal space 
	virtual gsl_vector *getStartingPoint() const = 0;
	
	// Returns the vector of the generated random numbers for the given threadID ($MTh$)
	gsl_vector *getGeneratedNumbers(int threadID) const;

	// Resets the value of some data members before a new analysis
	void reset();

	// Returns a pointer to the GSL random number generator engine of the given threadID ($MTh$)
	gsl_rng *getRandomGeneratorEngine(int threadID) const;

protected:
	// Data members for meta-properties
	RObject::RStartPointType theStartPointType;
	double theStandardDeviation;
	int theSeed;

	/* Data members for random number generation using GSL */

	// an array of pointers to gsl_vectors for keeping the generated random numbers for each threadID ($MTh$)
	gsl_vector **theGeneratedNumbers;
	const gsl_rng_type *theRandomGeneratorType;
	gsl_rng **theRandomGenerator;

private:

};

#endif // RRandomNumberGenerator_H
