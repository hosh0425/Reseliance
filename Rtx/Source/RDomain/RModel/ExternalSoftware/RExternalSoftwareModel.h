#ifndef RExternalSoftwareModel_H
#define RExternalSoftwareModel_H

#include "RModel.h"

#include <QProcess>


class RExternalSoftwareModelOutputManager;


class RExternalSoftwareModel : public RModel
{
	Q_OBJECT

public:
	
	RExternalSoftwareModel(QObject *parent, QString name);
	virtual ~RExternalSoftwareModel();

	// Returns a pointer to the QProcess object that is used to run an external software application, in the given threadID ($MTh$)
	QProcess *getProcess(int threadID);

	// this method gets the name of the file, processes it and returnes the suitable name for threadID from it (now path/threadID/name.postfix)
	static QString getSuitableFileNameForThreadID(QString filename, int threadID);

	void resetAfterParallelComputation();

protected:

	// Methods and data members that are used to communicate with an external software application
	// Only for file communication with the external software application: deletes the files that are generated by the external software application, in the given threadID ($MTh$)
	void deleteOutputFiles(int threadID);

	// Starts the QProcess that runs the external software application, in the given threadID ($MTh$)
	void initiateProcess(int threadID);

	// Only for file communication with the external software application: Reads and assigns the response values from files after the external application running is complete
	int updateFileResponses(); 

	// waits for and blocks the caller until getting the approperiate answer from the process in the given threadID
	void waitUntilTheProcessIsDone(bool continuousProcess, int threadID);

	// Returns the path which is set as working path in sequential analysis
	virtual QString getTheMainWorkingPath() = 0;

	// Pointer to the QProcess (array of pointers one per threadID) that runs the external software application ($MTh$)
	QPointer<QProcess> * theProcess;

	// String that is written to output when the external software application is finished running
	char * finishString;

	// Indicates whether the external software application has finihsed running in each threadID ($MTh$)
	bool * processFinishedAnalyzing;

private:
	
	// Prevents Rt from freezing when the external software application is being run (and is only used for threadID == 0)
	void eventLoop(bool continuousProcess);

	// checks if the process in the given threadID has finished
	bool checkProcessFinish(int threadID);

	static int QprocessExitStatusRegisterationID;

	// an array of classes to manage the output of external software in each threadID ($MTh$)
	RExternalSoftwareModelOutputManager ** outputManager;
};

#endif // RExternalSoftwareModel_H
