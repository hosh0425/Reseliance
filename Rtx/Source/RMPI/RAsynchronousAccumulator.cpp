#include "RAsynchronousAccumulator.h"

#include <QVector>
#include <qmutex.h>
#include <qwaitcondition.h>

#include "ROutputManager.h"
#include "RParallelComputationManager.h"

#include <qthread.h>

#include "RAccumulationThread.h"

RAsynchronousAccumulator::RAsynchronousAccumulator(RAccumulator * theMainAccumulator) : RAccumulator(0, theMainAccumulator->objectName())
{

	// setting parameters
	remainedDataLimitToReAllowAccumulation = 10 * RParallelComputationManager::getInstance()->getMaxThreadCount();
	ceilNumberOfData = remainedDataLimitToReAllowAccumulation * 100; 

	this->theMainAccumulator = theMainAccumulator;
	theAccumulationThread = new RAccumulationThread(this);

	theNumberOfDataToBeAccumulated = 0;
	theNumberOfDataToBeAccumulatedLock = new QMutex();

	waitConditionForNewData = new QWaitCondition();
	inWaitingStateForNewData = false;
	inWaitingStateForNewDataLock = new QMutex();

	waitConditionForAccumulatingOldData = new QWaitCondition();
	inWaitingStateForAccumulatingOldData = false;
	inWaitingStateForAccumulatingOldDataLock = new QMutex();

	accumulationDataVectorsLock = new QMutex();

	accumulationSynchronizationLock = new QMutex();

	accumulationEndedCompletely = false;
}

RAsynchronousAccumulator::~RAsynchronousAccumulator(void)
{
	// freeing all the memory for gsl_vectors!
	while (!theGeneratedStandardRandomNumbers.isEmpty())
		if (theGeneratedStandardRandomNumbers.first())
			gsl_vector_free(theGeneratedStandardRandomNumbers.takeFirst());

	delete waitConditionForAccumulatingOldData;
	delete inWaitingStateForAccumulatingOldDataLock;

	delete waitConditionForNewData;
	delete inWaitingStateForNewDataLock;

	delete accumulationDataVectorsLock;
	
	delete theNumberOfDataToBeAccumulatedLock;

	delete accumulationSynchronizationLock;
}

void RAsynchronousAccumulator::setFunctionList(QList<RFunction *> passedFunctionList){
	theMainAccumulator->setFunctionList(passedFunctionList);
}

bool RAsynchronousAccumulator::getContinueFlag(){
	return theMainAccumulator->getContinueFlag();
}

void RAsynchronousAccumulator::reset(){
	theMainAccumulator->reset();
}

void RAsynchronousAccumulator::terminateAccumulation(){
	theMainAccumulator->terminateAccumulation();
}

bool RAsynchronousAccumulator::doesNeedStandardNormalRandomNumbers(){
	return theMainAccumulator->doesNeedStandardNormalRandomNumbers();
}

int RAsynchronousAccumulator::accumulate(QVector<double> functionValueVector, gsl_vector * standardNormalRandomNumbers, QVector<double> sensitivityResultMeanVector, QVector<double> sensitivityResultStdvVector, int threadID){

	// getting the number of accumulated data
	int theNumberOfDataToBeAccumulatedCopy;
	theNumberOfDataToBeAccumulatedLock->lock();
	theNumberOfDataToBeAccumulatedCopy = theNumberOfDataToBeAccumulated;
	theNumberOfDataToBeAccumulatedLock->unlock();

	// if the number of data to be accumulated has reached ceil limit, wait
	if (theNumberOfDataToBeAccumulatedCopy >= ceilNumberOfData){

		inWaitingStateForAccumulatingOldDataLock->lock();
		inWaitingStateForAccumulatingOldData = true;
		inWaitingStateForAccumulatingOldDataLock->unlock();

		QMutex waitConditionLock;
		waitConditionLock.lock();
		waitConditionForAccumulatingOldData->wait(&waitConditionLock);
	}

	if (accumulationEndedCompletely){
		return 0;
	}

	// checking if the main accumulator needs random numbers for accumulation, make a copy of that
	gsl_vector * standardNormalRandomNumbers_copy = 0;
	if (theMainAccumulator->doesNeedStandardNormalRandomNumbers() && standardNormalRandomNumbers){
		standardNormalRandomNumbers_copy = gsl_vector_alloc(standardNormalRandomNumbers->size);
		gsl_vector_memcpy(standardNormalRandomNumbers_copy, standardNormalRandomNumbers);
	}

	// locking to do this part together in one thread
	accumulationSynchronizationLock->lock();

	// dumping output into accumulator's output
	ROutputManager::getInstance()->dumpToAccumulatorBuffers(threadID);

	// adding data to data vectors
	accumulationDataVectorsLock->lock();
	theFunctionValueLinkedList.append(functionValueVector);
	theGeneratedStandardRandomNumbers.append(standardNormalRandomNumbers_copy);
	threadIDLinkedList.append(threadID);
	theSensitivityResultMeanLinkedList.append(sensitivityResultMeanVector);
	theSensitivityResultStdvLinkedList.append(sensitivityResultStdvVector);
	accumulationDataVectorsLock->unlock();

	accumulationSynchronizationLock->unlock();

	// increasing the number of data to be accumulated!
	theNumberOfDataToBeAccumulatedLock->lock();
	theNumberOfDataToBeAccumulated++;
	theNumberOfDataToBeAccumulatedLock->unlock();

	// if the accumulator thread is waiting for data signalling!
	bool inWaitingState;
	inWaitingStateForNewDataLock->lock();
	inWaitingState = inWaitingStateForNewData;
	inWaitingStateForNewDataLock->unlock();

	if (inWaitingState){
		waitConditionForNewData->wakeAll();
	}

	return 0;
}

void RAsynchronousAccumulator::run(){

	while (theMainAccumulator->getContinueFlag()){

		// if there is no data for accumulation wait until data arrives!

		// getting the number of current data to be accumulated
		int theNumberOfDataToBeAccumulatedCopy = 0;

		theNumberOfDataToBeAccumulatedLock->lock();
		theNumberOfDataToBeAccumulatedCopy = theNumberOfDataToBeAccumulated;
		theNumberOfDataToBeAccumulatedLock->unlock();
		
		// if there is no data wait for new data to be arrived
		while (theNumberOfDataToBeAccumulatedCopy == 0){
			inWaitingStateForNewData = true;
			QMutex waitLock;
			waitLock.lock();
			waitConditionForNewData->wait(&waitLock);

			theNumberOfDataToBeAccumulatedLock->lock();
			theNumberOfDataToBeAccumulatedCopy = theNumberOfDataToBeAccumulated;
			theNumberOfDataToBeAccumulatedLock->unlock();
		}

		// turning inWaitingState to false
		inWaitingStateForNewDataLock->lock();
		inWaitingStateForNewData = false;
		inWaitingStateForNewDataLock->unlock();

		// extract out next data inside a safe area
		accumulationDataVectorsLock->lock();

		QVector<double> nextFunctionValueList = theFunctionValueLinkedList.takeFirst();
		QVector<double> nextSensitivityResultMean = theSensitivityResultMeanLinkedList.takeFirst();
		QVector<double> nextSensitivityResultStdv = theSensitivityResultStdvLinkedList.takeFirst();
		gsl_vector * nextGeneratedStandardRandomNumbers = theGeneratedStandardRandomNumbers.takeFirst();
		int nextThreadID = threadIDLinkedList.takeFirst();

		accumulationDataVectorsLock->unlock();

		// accumulating
		int result = theMainAccumulator->accumulate(nextFunctionValueList, nextGeneratedStandardRandomNumbers, nextSensitivityResultMean, nextSensitivityResultStdv, nextThreadID);

		if (result < 0)
			rCritical(nextThreadID) << "Error: Could not accumulate the results of this sample."; 

		// printing the accumulator buffers!
		ROutputManager::getInstance()->printAccumulatorBuffers();

		// freeing the memory of generated random numbers
		if (nextGeneratedStandardRandomNumbers)
			gsl_vector_free(nextGeneratedStandardRandomNumbers);

		// decreasing the number of data!
		theNumberOfDataToBeAccumulatedLock->lock();
		theNumberOfDataToBeAccumulatedCopy = --theNumberOfDataToBeAccumulated;
		theNumberOfDataToBeAccumulatedLock->unlock();

		bool inWaitingStateForAccumulatingOldDataCopy;
		inWaitingStateForAccumulatingOldDataLock->lock();
		inWaitingStateForAccumulatingOldDataCopy = inWaitingStateForAccumulatingOldData;
		inWaitingStateForAccumulatingOldDataLock->unlock();

		// if the number of data has reached the limit for informing and there are threads in waiting state for accumulating old data, wake them all!
		if ((theNumberOfDataToBeAccumulatedCopy <= remainedDataLimitToReAllowAccumulation) && inWaitingStateForAccumulatingOldDataCopy){
			waitConditionForAccumulatingOldData->wakeAll();
			inWaitingStateForAccumulatingOldData = false;
		}

		if (result < 0)
			break;
	}
	// end of accumulation

	accumulationEndedCompletely = true;

	// signalling to wake up all waiting threads!
	waitConditionForAccumulatingOldData->wakeAll();

	// resetting accumulation buffers in ROutputManager, to remove extra data
	ROutputManager::getInstance()->resetAccumulationBuffers();
}

void RAsynchronousAccumulator::start(){

	// starting the thread
	theAccumulationThread->start();
}

void RAsynchronousAccumulator::setSensitivityFunctionFlagList(QList<bool> flaglist){
	theMainAccumulator->setSensitivityFunctionFlagList(flaglist);
}

bool RAsynchronousAccumulator::canBeRunParallelly() {
	return false;
}
