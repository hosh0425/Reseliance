// Last modified by Mojtaba on March 19, 2014

#include "RAccumulator.h"
#include "RFunction.h"
#include <QReadWriteLock>

#include "ROutputManager.h"
//test git
RAccumulator::RAccumulator(QObject *parent, QString name)
	: RObject(parent, name)
{
	thePlottingInterval = 1;
	theMaximumIterations = 100000;
	theContinueFlag = true;
	theIterationIndex = 0;
	theDisplayDiagrams = true;

	// Sensitivity implementations
	theSRVSize = 0;
	theComputeRandomVariableSensitivities = false;

	// instantiating locks
	theContinueFlagLock = new QReadWriteLock();

	theMaximumIterationsLock = new QReadWriteLock();
	thePlottingIntervalLock = new QReadWriteLock();
	theDisplayDiagramsLock = new QReadWriteLock();
}

RAccumulator::~RAccumulator()
{
	// deleting locks
	delete theContinueFlagLock;
	delete theMaximumIterationsLock;
	delete thePlottingIntervalLock;
	delete theDisplayDiagramsLock;
}

long RAccumulator::getMaximumIterations()
{
	theMaximumIterationsLock->lockForRead();
	long val = theMaximumIterations;
	theMaximumIterationsLock->unlock();

	return val;
}

void RAccumulator::setMaximumIterations(long value)
{
	theMaximumIterationsLock->lockForWrite();
	
	theMaximumIterations = value;
	if (theMaximumIterations < 1) {
		theMaximumIterations = 1;
	}
	
	theMaximumIterationsLock->unlock();
}

long RAccumulator::getPlottingInterval() const
{
	thePlottingIntervalLock->lockForRead();
	long val = thePlottingInterval;
	thePlottingIntervalLock->unlock();

	return val;
}

void RAccumulator::setPlottingInterval(long value)
{
	thePlottingIntervalLock->lockForWrite();

	thePlottingInterval = value;
	if (thePlottingInterval < 1) {
		thePlottingInterval = 1;
	}

	thePlottingIntervalLock->unlock();
}

bool RAccumulator::getDisplayDiagrams()
{
	theDisplayDiagramsLock->lockForRead();
	bool val = theDisplayDiagrams;
	theDisplayDiagramsLock->unlock();

	return val;
}

void RAccumulator::setDisplayDiagrams(bool value)
{
	theDisplayDiagramsLock->lockForWrite();
	theDisplayDiagrams = value;
	theDisplayDiagramsLock->unlock();
}


void RAccumulator::setComputeRandomVariableSensitivities(bool value)
{
	theComputeRandomVariableSensitivities = value;
}

void RAccumulator::setSensitivityRandomVariableList(QList<RPointer<RRandomVariable> > randomVariableList)
{
	theSensitivityRandomVariableList = randomVariableList;
	theSRVSize = theSensitivityRandomVariableList.count();
}


bool RAccumulator::getTheContinueFlagValue(){

	bool value;
	theContinueFlagLock->lockForRead();
	value = theContinueFlag;
	theContinueFlagLock->unlock();
	return value;
}
	
void RAccumulator::setTheContinueFlagValue(bool newValue){
	
	theContinueFlagLock->lockForWrite();
	theContinueFlag = newValue;
	theContinueFlagLock->unlock();
}

QDebug RAccumulator::rDebug(int threadID){

	return ROutputManager::getInstance()->getDebugStreamInputForAccumulator();
}

QDebug RAccumulator::rCritical(int threadID){

	return ROutputManager::getInstance()->getCriticalStreamInputForAccumulator();
}