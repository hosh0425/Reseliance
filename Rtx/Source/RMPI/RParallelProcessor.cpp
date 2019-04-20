#include "RParallelProcessor.h"

#include <QEventLoop>

#include <QReadWriteLock>

#include "RParallelComputationManager.h"
#include "RParallelComputerInterface.h"
#include "ROutputManager.h"
#include "RThread.h"

#include "RAsynchronousAccumulator.h"

RParallelProcessor::RParallelProcessor(RParallelComputerInterface * theOwnerObject) : QObject()
{

	this->theOwnerObject = theOwnerObject;
	theNumberOfRunningThreads = 0;

	// instantiating locks
	continueConditionLock = new QReadWriteLock();

	processThreads = 0;
	theNumberOfProcessThreads = 0;
}

RParallelProcessor::~RParallelProcessor(void)
{
	// deleting locks
	delete continueConditionLock;
}

bool RParallelProcessor::initializeForNewAnalysis(){

	continueCondition = true;

	if (!RParallelComputationManager::getInstance()->canHaveParallelProcess())
		return false;

	// checking if the analysis can be run parallelly in contributed objects
	if (theOwnerObject->canBeRunParallelly()){
		RParallelComputationManager::getInstance()->initializeForRunningInParallelMode();
		return true;
	}

	RObject::rDebug() << "Hence, the analysis cannot be run using parallel processing.";

	return false;
}

bool RParallelProcessor::getContinueCondition(){

	// printing the buffers of the thread when then loop has been fin

	// copying the value of the continue condition in a safe area

	bool value;

	continueConditionLock->lockForRead();
	value = continueCondition;
	continueConditionLock->unlock();

	// returning the copy
	return value;
}

void RParallelProcessor::onThreadEnd(){

	// reading continue condition doesn't need any lock here, as this slot is the only method that changes continue condition and it is called via Qt::QueuedConnection which means it won't be run concurrently from different threads
	if (continueCondition){
		// creating safe area for writing into continue condition
		continueConditionLock->lockForWrite();
		continueCondition = false;
		continueConditionLock->unlock();
	}

	// decreasing the number of running threads
	theNumberOfRunningThreads--;

	if (theNumberOfRunningThreads == 0){
		emit parallelComputationFinished();
	}
}

void RParallelProcessor::printOutput(int threadID){
	ROutputManager::getInstance()->printBuffers(threadID);
}

void RParallelProcessor::waitUntillParallelComputationFinishes(){

	// using an eventloop to wait
	QEventLoop waitEventLoop;
	connect(this, SIGNAL(parallelComputationFinished()), &waitEventLoop, SLOT(quit()), Qt::QueuedConnection);
	waitEventLoop.exec();
	disconnect(this, SIGNAL(parallelComputationFinished()), &waitEventLoop, SLOT(quit()));

	// resetting...
	resetAfterParallelComputation();
}

void RParallelProcessor::resetAfterParallelComputation(){

	// printing the main buffers, for probable contents
	printOutput(0);

	// reset parallel computation manager
	RParallelComputationManager::getInstance()->analysisEnded();

	// reset the object after parallel computation
	theOwnerObject->resetAfterParallelComputation();

	analysisStatus = 0;

	// freeing the memory of process threads
	for (int i = 0 ; i < theNumberOfProcessThreads; i++){
		
		if (processThreads[i]->getMethodStatus() < 0)
			analysisStatus = -1;

		disconnect(processThreads[i], SIGNAL(finished()), this, SLOT(onThreadEnd()));
		delete processThreads[i];
	}

	delete [] processThreads;
}

int RParallelProcessor::run(int (* parallelizedMethod)(void *, void **, int), void * parallelizedOwnerObject, void * inputArgs[], int objectMaxNeededThreads){

	// initialize and check if running parallelly is possible
	bool runningInParallelMode = initializeForNewAnalysis();

	// if it is not parallel, calling the method normally
	if (!runningInParallelMode){
		return parallelizedMethod(parallelizedOwnerObject, inputArgs, 0);
	}
	else{

		// if there is an accumulator, replace it with new asynchronous accumulator
		RAsynchronousAccumulator * theNewAsynchronousAccumulator = 0;
		RAccumulator * theMainAccumulator = (RAccumulator *) theOwnerObject->getAccumulator();
		if (theMainAccumulator){
			theNewAsynchronousAccumulator = new RAsynchronousAccumulator(theMainAccumulator);
			theOwnerObject->setAccumulator(theNewAsynchronousAccumulator);
			theNewAsynchronousAccumulator->start();
		}

		// creating all process threads
		theNumberOfProcessThreads = RParallelComputationManager::getInstance()->getMaxThreadCount();
		if ((objectMaxNeededThreads > 0) && (theNumberOfProcessThreads > objectMaxNeededThreads))
			theNumberOfProcessThreads = objectMaxNeededThreads;

		theNumberOfRunningThreads = theNumberOfProcessThreads;

		processThreads = new RThread *[theNumberOfProcessThreads];
		for (int i = 0 ; i < theNumberOfProcessThreads ; i++){

			processThreads[i] = new RThread(parallelizedMethod, parallelizedOwnerObject, inputArgs, theOwnerObject, theNewAsynchronousAccumulator != 0);
			connect(processThreads[i], SIGNAL(finished()), this, SLOT(onThreadEnd()), Qt::QueuedConnection);
			processThreads[i]->start();
		}

		// waiting untill all process is done
		waitUntillParallelComputationFinishes();

		if (theNewAsynchronousAccumulator){
			delete theNewAsynchronousAccumulator;
			theOwnerObject->setAccumulator(theMainAccumulator);
		}

		return analysisStatus;
	}
}