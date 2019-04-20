#ifndef ROUTPUTMANAGER_H
#define ROUTPUTMANAGER_H

#include <qobject.h>
#include <qlinkedlist.h>

class QDebug;
class QMutex;

class ROutputManager  : public QObject
{

	Q_OBJECT

public :

	static void createInstance();

	static void deleteInstance();

	~ROutputManager();

	// returns the only static object of the RParallelComputationManager type
	static ROutputManager * getInstance();

	// for changing the number of buffers when the number of max threads change
	void reset(int newSize);

	// Returns the Debug stream of the given threadID ($MTh$)
	 QDebug getDebugStreamInput(int threadID);

	// Returns the Critical stream of the given threadID ($MTh$)
	 QDebug getCriticalStreamInput(int threadID);

	 // Returns the Debug stream specific to the accumulator
	 QDebug getDebugStreamInputForAccumulator();

	// Returns the Critical stream specific to the accumulator
	 QDebug getCriticalStreamInputForAccumulator();

	 // for printing the buffers of the given threadID in the main output stream. ($MTh$)
	 void printBuffers(int threadID);

	 // Clears the buffers of the given threadID (For when someone else like asynchronousAccumulator is responsible for printing the output of each thread not the thread itself, this method is used to clear the remainder buffer at last)
	 void clearBuffers(int threadID);

	 // dumps the buffers of the given threadID to the list of accumulator buffers, so when the results are completed the buffers would be printed ($MTh$)
	 void dumpToAccumulatorBuffers(int threadID);

	 // prints next buffer of accumulator (this method must be called from the main thread)
	 void printAccumulatorBuffers();

	// Empties accumulation buffers
	 void resetAccumulationBuffers();

signals :

	 void printOutputSignal(QString * debugString, QString * criticalString);

private slots:

	// for printing debugString and criticalString respectivey at main Debug stream and Critical stream
	void printOutputSlot(QString * debugString, QString * criticalString);

private :

	// private constructor for preventing creating more than one object of this class (signleton pattern)
	ROutputManager();

	// the only object of the class type in the whole program (singleton pattern)
	static ROutputManager * theROutputManagerInstance;
	
	// for allocating memory for buffers
	void instantiateOutputBuffers();

	// for freeing the allocated memory for buffers
	void freeOutputBuffers();

	// buffer per each threadIDs ($MTh$)
	QString ** threadIDDebugBuffers;
	QString ** threadIDCriticalBuffers;

	// Debug stream input for each buffer (stream input) ($MTh$)
	QDebug ** threadIDRDebugs;
	QDebug ** threadIDRCriticals;

	int size;

	// for Accumulator :

	// a linkedlist of buffers which are dumped and waiting for accumulator to add its data and print them
	QLinkedList<QString *> accumulatorDebugBufferList;
	QLinkedList<QString *> accumulatorCriticalBufferList;

	// inputs of accumulator buffer lists
	QDebug * accumulatorRDebug;
	QDebug * accumulatorRCritical;

	// a lock for creating safe area for appending data to and popping data from the accumulation buffers
	QMutex * accumulatorBuffersLock;

};

#endif

