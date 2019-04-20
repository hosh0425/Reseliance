#include "ROutputManager.h"

#include <QtDebug>

#include <qbuffer.h>
#include <QDebug> //salarifard

#include <qmutex.h>

#include <qstring.h>
#include <qstringlist.h>

#include "RParallelComputationManager.h"

#include <qalgorithms.h>


ROutputManager * ROutputManager::theROutputManagerInstance = 0;


void ROutputManager::createInstance(){

	if (theROutputManagerInstance)
		delete theROutputManagerInstance;

	theROutputManagerInstance = new ROutputManager();
}

void ROutputManager::deleteInstance(){
	delete theROutputManagerInstance;
}

ROutputManager * ROutputManager::getInstance(){
	return theROutputManagerInstance;
}


ROutputManager::ROutputManager() : QObject(0)
{
	size = 0;
	bool ok = connect(this, SIGNAL(printOutputSignal(QString *, QString *)), this, SLOT(printOutputSlot(QString *, QString *)), Qt::QueuedConnection);

	accumulatorRDebug = 0;
	accumulatorRCritical = 0;

	accumulatorBuffersLock = new QMutex();
}

ROutputManager::~ROutputManager(void)
{
	if (size > 0){
		freeOutputBuffers();
	}

	delete accumulatorBuffersLock;
}

void ROutputManager::reset(int newSize){

	if (newSize != size){

		if (size > 0){
			freeOutputBuffers();
		}

		size = newSize;

		if (newSize > 0){
			instantiateOutputBuffers();
		}
	}
}


void ROutputManager::printBuffers(int threadID){

	bool hasOutput = false;

	QString * debugString = 0;
	QString * criticalString = 0;

	// then printing the buffers of the threadID

	if ((threadIDDebugBuffers[threadID]->compare(" ") != 0) && (threadIDDebugBuffers[threadID]->compare("") != 0)){

		debugString = threadIDDebugBuffers[threadID];
		hasOutput = true;

		threadIDDebugBuffers[threadID] = new QString(" ");
		delete threadIDRDebugs[threadID];
		threadIDRDebugs[threadID] = new QDebug(threadIDDebugBuffers[threadID]);
	}

	if ((threadIDCriticalBuffers[threadID]->compare(" ") != 0) && (threadIDCriticalBuffers[threadID]->compare("") != 0)){
		
		criticalString = threadIDCriticalBuffers[threadID];
		hasOutput = true;

		threadIDCriticalBuffers[threadID] = new QString(" ");
		delete threadIDRCriticals[threadID];
		threadIDRCriticals[threadID] = new QDebug(threadIDCriticalBuffers[threadID]);
	}

	if (hasOutput)
		if (threadID > 0)
			emit printOutputSignal(debugString, criticalString);
		else
			printOutputSlot(debugString, criticalString);
}

void ROutputManager::clearBuffers(int threadID){

	threadIDDebugBuffers[threadID]->clear();
	threadIDCriticalBuffers[threadID]->clear();
}

void ROutputManager::printOutputSlot(QString * debugString, QString * criticalString){

	if (debugString){

		qDebug() << qPrintable(*debugString);
		delete debugString;
	}

	if (criticalString){

		QStringList criticalStringLines = criticalString->split("\n");

		for (int i = 0; i < criticalStringLines.count(); ++i) {
			qCritical() << qPrintable(criticalStringLines[i]);
		}

		delete criticalString;
	}

//	printMainBuffers();
}


QDebug ROutputManager::getDebugStreamInput(int threadID){

	// if the program is running in sequential mode, returning the main qDebug
	if (!RParallelComputationManager::getInstance()->isRunningInParallelMode()){
		return qDebug();
	}
	else{
		// appending a \n as the main qDebug will automatically do this but others won't
		(*(threadIDRDebugs[threadID])) << "\n";
		return  (*(threadIDRDebugs[threadID]));
	}
}

QDebug ROutputManager::getCriticalStreamInput(int threadID){

	// if the program is running in sequential mode, returning the main qDebug
	if (!RParallelComputationManager::getInstance()->isRunningInParallelMode()){
		return qCritical();
	}
	else{
		// appending a \n as the main qDebug will automatically do this but others won't
		(*(threadIDRCriticals[threadID])) << "\n";
		return  (*(threadIDRCriticals[threadID]));
	}
}


void ROutputManager::instantiateOutputBuffers(){

	// instantiating buffers, qDebugs and qCriticals, one per threadID from 0 to defined max
	threadIDDebugBuffers = new QString * [size];
	threadIDCriticalBuffers  = new QString * [size];

	// qDebug for each buffer
	threadIDRDebugs = new QDebug * [size];
	threadIDRCriticals = new QDebug * [size];

	for (int thID = 0 ; thID < size ; thID++){
			
		threadIDDebugBuffers[thID] = new QString(" ");
		threadIDCriticalBuffers[thID]  = new QString(" ");
			
		threadIDRDebugs[thID] = new QDebug(threadIDDebugBuffers[thID]);
		threadIDRCriticals[thID] = new QDebug(threadIDCriticalBuffers[thID]);
	}
}

void ROutputManager::freeOutputBuffers(){

	for (int thID = 0 ; thID < size ; thID++){
			
		delete(threadIDDebugBuffers[thID]);
		delete(threadIDCriticalBuffers[thID]);
			
		delete(threadIDRDebugs[thID]);
		delete(threadIDRCriticals[thID]);
	}

	delete [] threadIDDebugBuffers;
	delete [] threadIDCriticalBuffers;

	delete [] threadIDRDebugs;
	delete [] threadIDRCriticals;
}


void ROutputManager::dumpToAccumulatorBuffers(int threadID){

	QString * debugString = 0;
	QString * criticalString = 0;

	if (threadIDDebugBuffers[threadID]->compare(" ") != 0){

		debugString = threadIDDebugBuffers[threadID];

		threadIDDebugBuffers[threadID] = new QString(" ");
		delete threadIDRDebugs[threadID];
		threadIDRDebugs[threadID] = new QDebug(threadIDDebugBuffers[threadID]);
	}
	else
		debugString = new QString(" ");

	if (threadIDCriticalBuffers[threadID]->compare(" ") != 0){
		
		criticalString = threadIDCriticalBuffers[threadID];

		threadIDCriticalBuffers[threadID] = new QString(" ");
		delete threadIDRCriticals[threadID];
		threadIDRCriticals[threadID] = new QDebug(threadIDCriticalBuffers[threadID]);
	}
	else
		criticalString = new QString(" ");

	// appending strings to buffer lists, in a safe area
	accumulatorBuffersLock->lock();

	accumulatorDebugBufferList.append(debugString);
	accumulatorCriticalBufferList.append(criticalString);


	// checking if qDebug and qCritical of accumulator are null, instantiating them
	if (!accumulatorRDebug){
		accumulatorRDebug = new QDebug(accumulatorDebugBufferList.first());
		accumulatorRCritical = new QDebug(accumulatorCriticalBufferList.first());
	}

	accumulatorBuffersLock->unlock();
}


void ROutputManager::printAccumulatorBuffers(){

	// taking head data for accumulation in a safe area, and deleting the stream inputs
	accumulatorBuffersLock->lock();

	QString * debugString = accumulatorDebugBufferList.takeFirst();
	QString * criticalString = accumulatorCriticalBufferList.takeFirst();

	delete accumulatorRDebug;
	accumulatorRDebug = 0;
	delete accumulatorRCritical;
	accumulatorRCritical = 0;

	// if the buffer has next data, setting qDebug and qCritical
	if (!accumulatorDebugBufferList.isEmpty()){
		accumulatorRDebug = new QDebug(accumulatorDebugBufferList.first());
		accumulatorRCritical = new QDebug(accumulatorCriticalBufferList.first());
	}

	accumulatorBuffersLock->unlock();

	if (debugString->compare(" ") == 0){
		delete debugString;
		debugString = 0;
	}

	if (criticalString->compare(" ") == 0){
		delete criticalString;
		criticalString = 0;
	}

	if (debugString || criticalString)
		emit printOutputSignal(debugString, criticalString);
}

QDebug ROutputManager::getDebugStreamInputForAccumulator(){

	// if the program is running in sequential mode, returning the main qDebug
	if (!RParallelComputationManager::getInstance()->isRunningInParallelMode()){
		return qDebug();
	}
	else{
		// appending a \n as the main qDebug will automatically do this but others won't
		(*(accumulatorRDebug)) << "\n";
		return  (*(accumulatorRDebug));
	}
}

QDebug ROutputManager::getCriticalStreamInputForAccumulator(){

	// if the program is running in sequential mode, returning the main aCritical
	if (!RParallelComputationManager::getInstance()->isRunningInParallelMode()){
		return qCritical();
	}
	else{
		// appending a \n as the main qCritical will automatically do this but others won't
		(*(accumulatorRCritical)) << "\n";
		return  (*(accumulatorRCritical));
	}
}

void ROutputManager::resetAccumulationBuffers(){
	
	qDeleteAll(accumulatorDebugBufferList);
	qDeleteAll(accumulatorCriticalBufferList);

	accumulatorDebugBufferList.clear();
	accumulatorCriticalBufferList.clear();

	if (accumulatorRDebug){
		delete accumulatorRDebug;
		accumulatorRDebug = 0;
	}

	if (accumulatorRCritical){
		delete accumulatorRCritical;
		accumulatorRCritical = 0;
	}
}
