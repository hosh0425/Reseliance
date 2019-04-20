#ifndef RExternalSoftwareModelOutputManager_H
#define RExternalSoftwareModelOutputManager_H

#include <QProcess> //salarifard
#include "RObject.h"

class RExternalSoftwareModel;

class RExternalSoftwareModelOutputManager : public RObject
{
	Q_OBJECT

public:
	
	RExternalSoftwareModelOutputManager(int threadID, char * finishString, RExternalSoftwareModel * theOwnerExternalSoftware, bool * processFinishedAnalyzing);
	virtual ~RExternalSoftwareModelOutputManager();

	bool getProcessFinishACK();

	void reset();

signals:

	// Signal that is emitted when the external software application is finished running
	void processFinished(int exitCode, QProcess::ExitStatus exitStatus);

public slots:

	// Shows the output of the external application in the output pane, the sender of the signal must have a property named threadID with its value ($MTh$)
	void showOutput();

	// Slot that issues a warning if the external application crashes, the sender of the signal must have a property named threadID with its value ($MTh$)
	void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);

	bool canBeRunParallelly();


private:
	
	int threadID;

	RExternalSoftwareModel * theOwnerExternalSoftware;
	char * finishString;
	bool processFinishACK;
	bool * processFinishedAnalyzing;
};

#endif // RExternalSoftwareModel_H
