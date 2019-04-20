#ifndef RAsynchronousAccumulator_H
#define RAsynchronousAccumulator_H

#include "RAccumulator.h"

#include <qlinkedlist.h>

class QWaitCondition;
class QMutex;
class RAccumulationThread;

// all of the functions of this class are thread safe = They don't need a safe area to be called within
class RAsynchronousAccumulator : public RAccumulator
{

public:

	RAsynchronousAccumulator(RAccumulator * theMainAccumulator);
	~RAsynchronousAccumulator(void);

	/* All pure virtual methods of the accumulator */

	// Accumulates the result of the sample which is prepared in the given threadID
	int accumulate(QVector<double> functionValueVector, gsl_vector * standardNormalRandomNumbers, QVector<double> sensitivityResultMeanVector, QVector<double> sensitivityResultStdvVector, int threadID);

	// Sets the list of functions
	void setFunctionList(QList<RFunction *> passedFunctionList);

	// Gets a flag that indicates if the sampling should continue	
	bool getContinueFlag();

	// Resets the value of some data members before a new analysis
	void reset();

	// This method is called to do the final stuff when analysis is ended 
	void terminateAccumulation();

	// Returns true if the accumulator needs standard normal random numbers  in order to accumulate
	bool doesNeedStandardNormalRandomNumbers();

	// sensitivity methods
	void setSensitivityFunctionFlagList(QList<bool> flaglist);

	// Runs the accumulation loop!
	void run();

	// starts accumulating asynchronously
	void start();

	bool canBeRunParallelly();



private:

	RAccumulator * theMainAccumulator;
		
	// a subclass of QThread to run the accumulation in another thread
	RAccumulationThread * theAccumulationThread;

	// linked list of data ready to be accumulated
	QLinkedList<QVector<double>> theFunctionValueLinkedList;
	QLinkedList<gsl_vector *> theGeneratedStandardRandomNumbers;
	QLinkedList<int> threadIDLinkedList;

	QLinkedList<QVector<double>> theSensitivityResultMeanLinkedList;
	QLinkedList<QVector<double>> theSensitivityResultStdvLinkedList;

	// a wait condition for thread to wait when there is no data to accumulate till it receives data
	QWaitCondition * waitConditionForNewData;
	bool inWaitingStateForNewData;
	QMutex * inWaitingStateForNewDataLock;

	// a wait condition for when data to be accumulated goes over 1000
	QWaitCondition * waitConditionForAccumulatingOldData;
	bool inWaitingStateForAccumulatingOldData;
	QMutex * inWaitingStateForAccumulatingOldDataLock;

	// a lock for safely appending to the list and popping data from the list
	QMutex * accumulationDataVectorsLock;

	// a lock for doing some stuff in dumping data together in one thread
	QMutex * accumulationSynchronizationLock;

	int theNumberOfDataToBeAccumulated;
	QMutex * theNumberOfDataToBeAccumulatedLock;

	bool accumulationEndedCompletely;

	// ceil number for limiting data of buffers to be accumulated and 
	int ceilNumberOfData;

	// the minimum limit that when reached accumulator reallows accumulating data to threads
	int remainedDataLimitToReAllowAccumulation;
};
#endif