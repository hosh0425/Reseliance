#ifndef RTHREAD_H
#define RTHREAD_H

#include <QThread>

class RParallelComputerInterface;

class RThread : public QThread
{
public:

	RThread(int (* parallelizedMethodPtr)(void *, void **, int), void * theObject, void * inputArgs[], RParallelComputerInterface * theOwnerObject, bool hasSomeoneElseResponsibleForPrintingOutput);
	~RThread(void);

	void run();

	int getMethodStatus();

private:

	int (* theParallelizedMethod)(void *, void **, int);
	void * theObject;
	void ** theInputArgs;
	int threadID;

	int methodStatus;

	RParallelComputerInterface * theOwnerObject; // unnecessary but visual studio does not understand when theObject is cast to this type!!!!!! :|

	// to know if there is another object that is responsible for printing output, to clear the extra junk of buffer at the end...
	bool hasSomeoneElseResponsibleForPrintingOutput;
};

#endif

