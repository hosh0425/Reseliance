#include "ROpenSeesModel.h"

#include <QDir>
#include <QApplication>

#include "RParameter.h"
#include "RResponse.h"
#include "RFileResponse.h"
#include "RCommandResponse.h"

ROpenSeesModel::ROpenSeesModel(QObject *parent, QString name)
	: RExternalSoftwareModel(parent, name)
{
	finishString = "-- OpenSees done.";
}

ROpenSeesModel::~ROpenSeesModel()
{

}

QString ROpenSeesModel::getParameterList()
{
	return parameterListToString(theParameterList);
}

void ROpenSeesModel::setParameterList(QString value)
{
	theParameterList = stringToParameterList(value);
}

QString ROpenSeesModel::getExecutableFile() const
{
	return theExecutableFile;
}

void ROpenSeesModel::setExecutableFile(QString value)
{
	theExecutableFile = RDomain::stringToFilePath(value);
}

QString ROpenSeesModel::getInputFile() const
{
	return theInputFile;
}

void ROpenSeesModel::setInputFile(QString value)
{
	theInputFile = RDomain::stringToFilePath(value);
}

int ROpenSeesModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
{	
	initiateProcess(threadID);
	
	// DELETE OUTPUT FILES
	deleteOutputFiles(threadID);

	if (theProcess[threadID].data()->state() == QProcess::NotRunning) {		
		// START THE PROCESS
		QStringList runOptionsList;
		theProcess[threadID].data()->start(QFileInfo(theExecutableFile).absoluteFilePath(), runOptionsList, QIODevice::ReadWrite);		
		bool started = theProcess[threadID].data()->waitForStarted(-1);
		if (!started) {
			rCritical(threadID) << "Error: The executable associated with" << objectName() << "cannot be started. Please check the path and syntaxes and try again.";
			exitProcess(threadID);
			return -1;
		}
		// Display a message
		if (getDisplayOutput()) {
			rDebug(threadID) << "OpenSees process started...";
		}
	}/* else if (theProcess->state() == QProcess::Running) {
		theProcess->write("wipe\n");
	}*/


	theProcess[threadID].data()->write("wipe\n");
	theProcess[threadID].data()->write(qPrintable(QString("puts \"%1\"\n").arg("-- Starting OpenSees..."))); 


	// UPDATE INPUT PARAMETERS
	QList<RParameter *> parameterList = theAllParametersList;
	for (int i = 0; i < parameterList.count(); i++) {
		QString name = parameterList[i]->objectName();
		double value = parameterList[i]->getCurrentValue(threadID);
		theProcess[threadID].data()->write(qPrintable(QString("set %1 %2\n").arg(name).arg(value, 0, 'g', 14))); 

		//Temporary to show the assignments
		//theProcess->write(qPrintable(QString("puts $%1\n").arg(parameterList[i]->objectName()))); 
	}

	// RUN THE TCL FILE
	theProcess[threadID].data()->write(qPrintable(QString("source \"%1\"\n").arg(QFileInfo(theInputFile).absoluteFilePath())));
	//theProcess->write("set b $a\n");


	processFinishedAnalyzing[threadID] = true;
	theProcess[threadID].data()->write(qPrintable(QString("puts \"%1\"\n").arg(finishString))); 

	// Waiting for the commands to be written in OpenSees or 5 seconds, whichever comes first
	bool isWritten = theProcess[threadID].data()->waitForBytesWritten(5000);
	if (!isWritten) {
		rCritical(threadID) << "Error: Could not write commands to the external application in model" << objectName();
		return -1;
	}


	// STOP THE CODE HERE until the process is done while preventing the GUI to freeze
	waitUntilTheProcessIsDone(true, threadID);


	// UPDATE COMMAND RESPONSES
	int result = 0;
	// Getting the list of responses that are output of this model
	QList<RCommandResponse *> commandResponseList = findChildren<RCommandResponse *>();
	// Looping over the output response list and reading the value of the response (e.g. from the files in case of a file response)
	for (int i = 0; i < commandResponseList.count(); i++) {
		result = commandResponseList[i]->updateValue(threadID);
	}


	//Because "set" does change the parameters inside opensees objects, model should be built each time. Therefore, either the model is "wipe"ed or the process is exitted and deleted. Right now, the option of exiting is chosen, because the wiping does not work properly. Ideally, wiping would be preferred, because it is faster than exiting.
	//theProcess->write("wipe\n");	
	exitProcess(threadID);


	// UPDATE FILE RESPONSES
	// Getting the list of responses that are output of this model
	QList<RFileResponse *>  fileResponseList = findChildren<RFileResponse *>();
	// Looping over the output response list and reading the value of the response (e.g. from the files in case of a file response)
	for (int i = 0; i < fileResponseList.count(); i++) {
		result = fileResponseList[i]->updateValue(threadID);
	}

	// Revert back to the Rt current path, if threadID is 0
	if (threadID == 0)
		bool workingPathIsSet = QDir::setCurrent(theDomain->getCurrentFilePath());

	return result;
}

void ROpenSeesModel::exitProcess(int threadID)
{
	if (!theProcess)
		return;

	if (!theProcess[threadID]) {
		return;
	}

	if (theProcess[threadID].data()->state() == QProcess::NotRunning)
		return;

	theProcess[threadID].data()->write("exit\n");


	/*theProcess->waitForBytesWritten();
	eventLoop(false);*/
	bool finsihed = theProcess[threadID].data()->waitForFinished(5000);
	if (!finsihed) {
		rCritical(threadID) << "Warning: The executable associated with" << objectName() << "cannot be ended normally. Rt will terminate the process.";
	}


	// In case the process is exited, display a message and delete the process
	if (getDisplayOutput()) {
		rDebug(threadID) << "OpenSees process terminated.";
	}
}

void ROpenSeesModel::reset()
{
	if (theProcess) {

		int arraySizeForParallelizedObject = getArraySizeForParallelizedObject();

		for (int thID = 0; thID < arraySizeForParallelizedObject ; thID++)
			exitProcess(thID);
	}
}

QString ROpenSeesModel::getTheMainWorkingPath(){
	return QFileInfo(theInputFile).absolutePath();
}

bool ROpenSeesModel::canBeRunParallelly(){
	return true;
}