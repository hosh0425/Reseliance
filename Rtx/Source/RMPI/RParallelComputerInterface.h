#ifndef RParallelComputerInterface_H
#define RParallelComputerInterface_H

#include <qobject.h>

class RParallelComputerInterface
{
public:

	RParallelComputerInterface(void);
	virtual ~RParallelComputerInterface(void);

	// Returns true if they (and all their important contributed objects) can be run parallelly
	virtual bool canBeRunParallelly() = 0;

	// Initializes the object which is going to run analysis parallelly = Copies the value at index 0 to the index of the given threadID for data members that are changed to array due to parallelization. Each subclass must also call the method of their superclass
	virtual void initializeForParallelComputation(int threadID) = 0;

	// Resets the object after the parallel analysis is completed
	virtual void resetAfterParallelComputation() = 0;

	// member functions for getting and setting accumulator, if any exists.
	virtual QObject * getAccumulator() const;
	virtual void setAccumulator(QObject *value);
};

#endif

