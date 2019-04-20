// Last modified by Mojtaba on June 29, 2012

#ifndef RIndependentNormalRandomNumberGenerator_H
#define RIndependentNormalRandomNumberGenerator_H

#include "RRandomNumberGenerator.h"

#include "RPointer.h" //salarifard

#include <gsl/gsl_vector.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

class RTransformer;
class QMutex;

class RIndependentNormalRandomNumberGenerator : public RRandomNumberGenerator
{
	Q_OBJECT

	// Sets the RTransformer object, which transforms points between original and standard normal spaces
	Q_PROPERTY(QObject *Transformer READ getTransformer WRITE setTransformer)

public:
	RIndependentNormalRandomNumberGenerator(QObject *parent, QString name);
	~RIndependentNormalRandomNumberGenerator();

	// getter/setter methods for meta-properties
	QObject *getTransformer() const;
	void setTransformer(QObject *value);

	// Generates random numbers for the given threadID ($MTh$)
	int generateNumbers(int count, int threadID);

	// Sets the list of random vairables and invokes the establishStartPoint() method
	int setRandomVariableList(QList<RRandomVariable *> *passedRandomVariableList);

	// Returns the vector of starting point in the standard-normal space 
	gsl_vector *getStartingPoint() const;

	// returns true as this random number generator is parallelized
	bool canBeRunParallelly();

private:
	// Auxiliary method to generates "count" random numbers for the given threadID ($MTh$)
	int generateStandardNormalNumbers(int count, int threadID);

	// Auxiliary method to establish theStartPoint vector
	int establishStartPoint();

	// Data members for meta-properties
	RPointer<RTransformer> theTransformer;

	// Data members for random number generation
	gsl_vector *theStartPoint;
	QList<RRandomVariable *> *theRandomVariableList;
	
	// a lock for safely accessing theStartPoint in all threads
	QMutex * startPointLock;
};

#endif // RIndependentNormalRandomNumberGenerator_H
