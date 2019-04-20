#include "RAccumulationThread.h"

#include "RAsynchronousAccumulator.h"

RAccumulationThread::RAccumulationThread(RAsynchronousAccumulator * theAccumulator){

	this->theAccumulator = theAccumulator;
}

RAccumulationThread::~RAccumulationThread(){}

void RAccumulationThread::run(){
	theAccumulator->run();
}