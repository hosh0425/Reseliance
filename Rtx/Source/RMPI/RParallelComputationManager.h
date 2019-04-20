#ifndef RParallelComputationManager_H
#define RParallelComputationManager_H

#include <qlinkedlist.h>

class QThreadPool;
class QMutex;


class RParallelComputationManager
{

public :

	static void createInstance();

	static void deleteInstance();

	~RParallelComputationManager();

	// Sets the maxThreadsCount that parallel analyzers can use
	void setMaxThreadCount(int newMaxThreadCount);

	// Returns true if maxThreadsCount can be changed otherwise false
	bool canChangeThreadCount(int newMaxThreadCount);

	// returns the only static object of the RParallelComputationManager type
	static RParallelComputationManager * getInstance();

	// returns the max number of threads the program can have
	int getMaxThreadCount();

	// Initializes the fields for running in parallel mode, returns true if running in parallel mode is possible
	bool initializeForRunningInParallelMode();

	// Returns true if processes can be run parallelly
	bool canHaveParallelProcess();

	 // returns true if the program is in parallel mode, false otherwise
	 bool isRunningInParallelMode();

	 // Informs the manager a new object has been created to finalize some fields!
	 void newObjectCreated();

	 // Informs the parallel computation manager that new RDomain has been created
	 void domainIsReset();

	 // Resets the fields
	 void analysisEnded();

	 // for allocating a threadID for a new thread in which the calculation will be carried out ($MTh$)
	 int allocateThreadID();

	// for releasing a taken threadID ($MTh$)
	 void releaseThreadID(int threadID);

private :

	// private constructor for preventing creating more than one object of this class (signleton pattern)
	RParallelComputationManager();

	// the only object of the class type in the whole program (singleton pattern)
	static RParallelComputationManager * theRParallelComputationManagerInstance;
	
	// for filling the freeThreadIDsPool according to the user defined max thread count
	void fillFreeThreadIDsPool();

	// the number of threads that can be used by parallel analyzers to run their analysis
	int maxThreadsCount;

	// A boolean fag that when true does not need any further change in the max thread count (like when objects have been created)
	bool theMaxThreadsCountIsFinalized;

	bool isRunningParallelly;

	// a linkedList (used as stack) for keeping available threadIDs
	QLinkedList<int> freeThreadIDsPool;

	// a lock for taking and releasing threadID and resetting the max number of threadIDs
	 QMutex * threadIDLock;
};

#endif

