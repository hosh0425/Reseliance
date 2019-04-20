#include "RMatlabModel.h"

#include <qthread.h>

#include <QDir>
#include <QApplication>

#include "RParameter.h"
#include "RResponse.h"
#include "RFileResponse.h"
#include "RCommandResponse.h"

RMatlabModel::RMatlabModel(QObject *parent, QString name)
	: RExternalSoftwareModel(parent, name)
{
	finishString = "-- Matlab done.";
	theMinimizeMatlabWindow = true;
}

RMatlabModel::~RMatlabModel()
{

}

QString RMatlabModel::getParameterList()
{
	return parameterListToString(theParameterList);
}

void RMatlabModel::setParameterList(QString value)
{
	theParameterList = stringToParameterList(value);
}

bool RMatlabModel::getMinimizeMatlabWindow() const
{
	return theMinimizeMatlabWindow;
}

void RMatlabModel::setMinimizeMatlabWindow(bool value)
{
	theMinimizeMatlabWindow = value;
}

/*QString RMatlabModel::getExecutableFile() const
{
	return theExecutableFile;
}

void RMatlabModel::setExecutableFile(QString value)
{
	theExecutableFile = RDomain::stringToFilePath(value);
}*/


QString RMatlabModel::getInputFile() const
{
	return theInputFile;
}

void RMatlabModel::setInputFile(QString value)
{
	theInputFile = RDomain::stringToFilePath(value);
}

QString RMatlabModel::getOutputFile() const
{
	return theOutputFile;
}

void RMatlabModel::setOutputFile(QString value)
{
	theOutputFile = RDomain::stringToFilePath(value);
}

int RMatlabModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
{	

	QString threadSpecificOutputFile = getSuitableFileNameForThreadID(QFileInfo(theOutputFile).absoluteFilePath(), threadID);

	initiateProcess(threadID);
	
	// DELETE OUTPUT FILES
	deleteOutputFiles(threadID);

	if (theProcess[threadID].data()->state() == QProcess::NotRunning) {		

		// BUILD THE STRING THAT SETS THE PARAMETER VALUES
		QString parametersString = "";
		QList<RParameter *> parameterList = theAllParametersList;
		for (int i = 0; i < parameterList.count(); i++) {
			QString name = parameterList[i]->objectName();
			double value = parameterList[i]->getCurrentValue(threadID);
			parametersString += QString("%1=%2;").arg(name).arg(value, 0, 'g', 14);
		}

		QString minimizeStr = "";
		if (theMinimizeMatlabWindow) {
			minimizeStr = "-minimize ";
		}

		// getting the absolute file path and modifing it
		QString modifiedInputFile = theInputFile;
		if (theInputFile.endsWith(".m")) {
			modifiedInputFile = theInputFile.left(theInputFile.length()-2);
		}

		// START THE PROCESS
		QString appStr;
		if (threadID > 0)
			appStr = QString("matlab -nosplash -nojvm -wait %1-logfile %2 -r \"%3addpath('%4');run('%5');exit;\"").arg(minimizeStr).arg(threadSpecificOutputFile).arg(parametersString).arg((QFileInfo(theInputFile)).absolutePath()).arg(modifiedInputFile);
		else
			appStr = QString("matlab -nosplash -nojvm -wait %1-logfile %2 -r \"%3%4;exit;\"").arg(minimizeStr).arg(theOutputFile).arg(parametersString).arg(modifiedInputFile);
		
		theProcess[threadID].data()->start(appStr, QIODevice::ReadWrite);		
		bool started = theProcess[threadID].data()->waitForStarted(-1);
		if (!started) {
			rCritical(threadID) << "Error: The executable associated with" << objectName() << "cannot be started. Please check the path and syntaxes and try again.";
			exitProcess(threadID);
			return -1;
		}
		// Display a message
		if (getDisplayOutput()) {
			rDebug(threadID) << "Matlab process started...";
		}
	}

	// STOP THE CODE HERE until the process is done while preventing the GUI to freeze
	processFinishedAnalyzing[threadID] = true;
	waitUntilTheProcessIsDone(false, threadID);

	// UPDATE COMMAND RESPONSES
	int result = 0;
	// Getting the list of responses that are output of this model
	QList<RCommandResponse *> commandResponseList = findChildren<RCommandResponse *>();
	// Looping over the output response list and reading the value of the response (e.g. from the files in case of a file response)
	for (int i = 0; i < commandResponseList.count(); i++) {
		result = commandResponseList[i]->updateValue(threadID);
	}


	if (getDisplayOutput()) {
		QFile file(threadSpecificOutputFile);
		if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
			rCritical(threadID) << "Error: Could not open the file" << threadSpecificOutputFile;
		} else {
			QTextStream data(&file);
			int i = 1;
			while (!data.atEnd()) {
				QString dataLine = data.readLine();
				if (i > 4) {
					rDebug(threadID) << dataLine.toStdString().c_str();
				}
				i++;
			}
		}
	}

	if (getDisplayOutput()) {
		rDebug(threadID) << "";
		rDebug(threadID) << "-- Matlab done.";
		rDebug(threadID) << "Process terminated.";
		rDebug(threadID) << "";
	}

	theProcess[threadID].data()->write("exit\n");

	// UPDATE FILE RESPONSES
	// Getting the list of responses that are output of this model
	QList<RFileResponse *>  fileResponseList = findChildren<RFileResponse *>();
	// Looping over the output response list and reading the value of the response (e.g. from the files in case of a file response)
	result = 0;
	for (int i = 0; i < fileResponseList.count(); i++) {
		result = fileResponseList[i]->updateValue(threadID);
	}

	// Revert back to the Rt current path, if it has been changed before (in the case of threadID = 0)
	if (threadID == 0)
		bool workingPathIsSet = QDir::setCurrent(theDomain->getCurrentFilePath());

	return result;
}

void RMatlabModel::exitProcess(int threadID)
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
		rDebug(threadID) << "Matlab process terminated.";
	}
}

void RMatlabModel::reset()
{
	if (theProcess) {

		int arraySizeForParallelizedObject = getArraySizeForParallelizedObject();

		for (int thID = 0; thID < arraySizeForParallelizedObject ; thID++)
			exitProcess(thID);
	}
}

QString RMatlabModel::getTheMainWorkingPath(){
	return QFileInfo(theInputFile).absolutePath();
}

bool RMatlabModel::canBeRunParallelly(){
	return true;
}