#include "RExternalSoftwareModelOutputManager.h"

#include <QByteArray>
#include "RExternalSoftwareModel.h"

RExternalSoftwareModelOutputManager::RExternalSoftwareModelOutputManager(int threadID, char * finishString, RExternalSoftwareModel * theOwnerExternalSoftware, bool * processFinishedAnalyzing)
	: RObject(0, "")
{
	this->threadID = threadID;
	this->finishString = finishString;
	this->theOwnerExternalSoftware = theOwnerExternalSoftware;
	this->processFinishedAnalyzing = processFinishedAnalyzing;
	this->processFinishACK = processFinishACK;

	bool ok = connect(this, SIGNAL(processFinished(int, QProcess::ExitStatus)), this, SLOT(onProcessFinished(int, QProcess::ExitStatus)), Qt::DirectConnection);
}

RExternalSoftwareModelOutputManager::~RExternalSoftwareModelOutputManager()
{
}

void RExternalSoftwareModelOutputManager::showOutput()
{
	QByteArray outputBytes = theOwnerExternalSoftware->getProcess(threadID)->readAllStandardOutput();
	if (theOwnerExternalSoftware->getDisplayOutput()) {
		QStringList lines = QString(outputBytes).split("\n");

		if (lines.size() == 0)
			rCritical(threadID) << "empty input again!";

		for (int i = 0; i < lines.count(); ++i){
			if (lines[i].length() > 0) {
				rDebug(threadID) << qPrintable(lines[i]);
			}
		}
	}

	//// The follwoing lines are required in the case that the external software should not be closed at the end of the analysis. They are needed to determine that the external software is done with the analysis.
	if (*processFinishedAnalyzing) {	
		if (outputBytes.contains(finishString)) {

			processFinishACK = true;
			emit processFinished(0, QProcess::NormalExit);
			
			// Disconnect the readyReadStandardOutput from this method, because from now on, we need to read responses and we don't want the results of the commands of the RCommandResponse to be shown in the output pane. The signal will be reconnected in the initiateProcess the next time the evaluateFunction is called
			disconnect(theOwnerExternalSoftware->getProcess(threadID), SIGNAL(readyReadStandardOutput()), this, SLOT(showOutput()));
		}
	}
}

void RExternalSoftwareModelOutputManager::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{

	if (exitStatus == QProcess::CrashExit) {
		rCritical(threadID) << "Warning: The external application in the model" << this->objectName() << "has crashed.";
	}

	disconnect(theOwnerExternalSoftware->getProcess(threadID), SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(onProcessFinished(int, QProcess::ExitStatus)));
	processFinishACK = true;
}

bool RExternalSoftwareModelOutputManager::getProcessFinishACK(){
	return processFinishACK;
}

void RExternalSoftwareModelOutputManager::reset(){
	processFinishACK = false;
}

bool RExternalSoftwareModelOutputManager::canBeRunParallelly() {	
	return false;
}