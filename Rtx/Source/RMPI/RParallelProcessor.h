#ifndef RParallelProcessor_H
#define RParallelProcessor_H

#include <QObject>

class QMutex;
class QReadWriteLock;

class RParallelComputerInterface;
class RThread;

// all of the functions of this class are thread safe = They don't need a safe area to be called within
class RParallelProcessor : public QObject
{

	Q_OBJECT

public:

	RParallelProcessor(RParallelComputerInterface * theOwnerObject);
	virtual ~RParallelProcessor(void);

	// prints the buffers related to the given threadID
	void printOutput(int threadID);

	// Returns the value of the continue condition flag, threadID is given to know which thread is asking! (Because this means one loop has been ended at that thread)
	bool getContinueCondition();

	// Runs the passed method (parallelly if possible) and returns its status, object max needed threads is used when there can be not so much iterations like in function, in order to not to produce extra threads
	int run(int (* parallelizedMethod)(void *,void **, int), void * parallelizedOwnerObject, void * inputArgs[], int objectMaxNeededThreads = -1);

signals:

	// signal which is emitted when parallel computation is completed
	void parallelComputationFinished();

public slots:
	
	// for when a thread ends
	void onThreadEnd();



private :

	// a method that is called for resetting data members for starting a new analysis, returns true if the analysis is doing to run in parallel mode
	bool initializeForNewAnalysis();

	void resetAfterParallelComputation();

	// Rus an event loop and waits untill the parallel computation finishes
	void waitUntillParallelComputationFinishes();

	// a counter for keeping the number of threads which have not finished yet, for the instance of computer class
	int theNumberOfRunningThreads;

	// the flag which is used to determine if the computation must be continued
	bool continueCondition;

	// a lock for safely accessing continue condition flag
	QReadWriteLock * continueConditionLock;

	// a reference to the object the parallel analysis is being conducted in
	RParallelComputerInterface * theOwnerObject;

	RThread ** processThreads;
	int theNumberOfProcessThreads;
	
	// after the parallel analysis finishes, this would be -1 if the analysis in any of threads encountered error otherwise 0
	int analysisStatus;
};

#endif

