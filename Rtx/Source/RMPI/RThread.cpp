#include "RThread.h"

#include "RParallelComputationManager.h"
#include "RParallelComputerInterface.h"
#include "ROutputManager.h"

RThread::RThread(int (* parallelizedMethodPtr)(void *, void **, int), void * theObject, void * inputArgs[], RParallelComputerInterface * theOwnerObject, bool hasSomeoneElseResponsibleForPrintingOutput){
	
	this->theParallelizedMethod = parallelizedMethodPtr;
	this->theObject = theObject;
	this->theInputArgs = inputArgs;
	this->theOwnerObject = theOwnerObject;
	this->hasSomeoneElseResponsibleForPrintingOutput = hasSomeoneElseResponsibleForPrintingOutput;
}

RThread::~RThread(void){}

void RThread::run(){

	// allocating a threadID
	threadID = RParallelComputationManager::getInstance()->allocateThreadID();

	// initializing in threadID
	theOwnerObject->initializeForParallelComputation(threadID);

	// running the method
	methodStatus = theParallelizedMethod(theObject, theInputArgs, threadID);

	// check if it is needed to clear the buffer at the end (otherwise the remainder is junk and must be cleared)
	if (hasSomeoneElseResponsibleForPrintingOutput)
		ROutputManager::getInstance()->clearBuffers(threadID);
	else
		ROutputManager::getInstance()->printBuffers(threadID);

	// freeing threadID
	RParallelComputationManager::getInstance()->releaseThreadID(threadID);
}

int RThread::getMethodStatus(){
	return methodStatus;
}