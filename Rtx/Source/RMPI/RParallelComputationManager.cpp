#include "RParallelComputationManager.h"

#include <qthreadpool.h>
#include <qmutex.h>

#include "ROutputManager.h"

RParallelComputationManager * RParallelComputationManager::theRParallelComputationManagerInstance = 0;

void RParallelComputationManager::createInstance(){
	
	if (theRParallelComputationManagerInstance)
		delete theRParallelComputationManagerInstance;

	theRParallelComputationManagerInstance = new RParallelComputationManager();
}

void RParallelComputationManager::deleteInstance(){
	delete theRParallelComputationManagerInstance;
}

RParallelComputationManager::RParallelComputationManager(){
	
	maxThreadsCount = 0;
	theMaxThreadsCountIsFinalized = false;
	isRunningParallelly = false;
	threadIDLock = new QMutex();
}

RParallelComputationManager::~RParallelComputationManager(){
	
	delete threadIDLock;

	if (maxThreadsCount > 0){
		fillFreeThreadIDsPool();
	}
}

void RParallelComputationManager::setMaxThreadCount(int newMaxThreadCount){

	if (newMaxThreadCount == maxThreadsCount){
		return;
	}

	if (newMaxThreadCount > maxThreadsCount){
		// resetting output manager
		ROutputManager::getInstance()->reset(newMaxThreadCount + 1);
	}
	
	maxThreadsCount = newMaxThreadCount;
	fillFreeThreadIDsPool();
	if (maxThreadsCount > 0)
		QThreadPool::globalInstance()->setMaxThreadCount(maxThreadsCount);
}

bool RParallelComputationManager::canChangeThreadCount(int newMaxThreadCount){
	return !(theMaxThreadsCountIsFinalized);
}

RParallelComputationManager * RParallelComputationManager::getInstance(){
	return theRParallelComputationManagerInstance;
}

int RParallelComputationManager::getMaxThreadCount(){
	return maxThreadsCount;
}

bool RParallelComputationManager::initializeForRunningInParallelMode(){

	if (maxThreadsCount > 0){
		isRunningParallelly = true;
		return true;
	}
	else
		return false;
}

bool RParallelComputationManager::canHaveParallelProcess(){

	return maxThreadsCount > 0;
}

bool RParallelComputationManager::isRunningInParallelMode(){
	return isRunningParallelly;
}

void RParallelComputationManager::newObjectCreated(){
	theMaxThreadsCountIsFinalized = true;
}

void RParallelComputationManager::domainIsReset(){
	theMaxThreadsCountIsFinalized = false;
}

void RParallelComputationManager::analysisEnded(){

	// printing the buffer of 0
	if (isRunningParallelly)
		ROutputManager::getInstance()->printBuffers(0);

	isRunningParallelly = false;
}

int  RParallelComputationManager::allocateThreadID(){

	if (!isRunningParallelly)
		return 0;

	int threadID;

	threadIDLock->lock();

	// popping the last threadID
	threadID = freeThreadIDsPool.takeLast();

	threadIDLock->unlock();

	return threadID;
}

void  RParallelComputationManager::releaseThreadID(int threadID){

	if (threadID == 0)
		return;
	
	// printing buffers
	ROutputManager::getInstance()->printBuffers(threadID);

	threadIDLock->lock();

	// pushing the threadID to the end of the freeThreadIDsPool to make it reusable
	freeThreadIDsPool.push_back(threadID);

	threadIDLock->unlock();
}

void RParallelComputationManager::fillFreeThreadIDsPool(){

	freeThreadIDsPool.clear();

	for (int i = maxThreadsCount ; i > 0 ; i--)
		freeThreadIDsPool.push_back(i);
}