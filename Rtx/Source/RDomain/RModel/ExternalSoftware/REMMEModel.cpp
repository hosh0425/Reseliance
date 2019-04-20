#include "REMMEModel.h"

#include <QDir>
#include <QApplication>

#include "RParameter.h"
#include "RFileResponse.h"

REMMEModel::REMMEModel(QObject *parent, QString name)
	: RExternalSoftwareModel(parent, name)
{
	finishString = "-- EMME done.";
}

REMMEModel::~REMMEModel()
{

}

QString REMMEModel::getParameterList()
{
	return parameterListToString(theParameterList);
}

void REMMEModel::setParameterList(QString value)
{
	theParameterList = stringToParameterList(value);
}

QString REMMEModel::getInputScenarioFile() const
{
	return theInputScenarioFile;
}

void REMMEModel::setInputScenarioFile(QString value)
{
	theInputScenarioFile = RDomain::stringToFilePath(value);
}

QString REMMEModel::getOutputScenarioFile() const
{
	return theOutputScenarioFile;
}

void REMMEModel::setOutputScenarioFile(QString value)
{
	theOutputScenarioFile = RDomain::stringToFilePath(value);
}

int REMMEModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
{
	initiateProcess(threadID);

	if (theProcess[threadID].data()->state() == QProcess::NotRunning) {

		// DELETE OUTPUT FILES
		deleteOutputFiles(threadID);

		// UPDATE INPUT PARAMETERS
		QList<RParameter *> parameterList = theAllParametersList;

		QFile inputFile(theInputScenarioFile);
		if (!inputFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
			rCritical(threadID) << "Error: Could not open the file" << theInputScenarioFile << ".";
			return -1;
		}

		QTextStream inputFileTextStream(&inputFile);
		QString inputFileString = inputFileTextStream.readAll();
		inputFile.close();
		
		for (int i = 0; i < parameterList.count(); i++) {
			QString name = parameterList[i]->objectName();
			double value = parameterList[i]->getCurrentValue(threadID);

			// Is this enough precession?
			inputFileString.replace(name, QString("%1").arg(value, 0, 'f', 14));
		}
		
		QString outputScenarioFileForThreadID = getSuitableFileNameForThreadID(theOutputScenarioFile, threadID);
		QFile outputFile(outputScenarioFileForThreadID);
		if (!outputFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
			rCritical(threadID) << "Error: Could not create the file" << theOutputScenarioFile << ".";
			return -1;
		}
		QByteArray outputByteArray;
		outputByteArray.append(inputFileString);

		outputFile.write(outputByteArray);
		outputFile.close();

		
		// START THE PROCESS
		if (getDisplayOutput()) {
			rDebug(threadID) << "-- Starting EMME...";
			//rDebug() << "";
		}
		//QStringList runOptionsList = QStringList() << "-ng" << "--set-iks 127.0.0.1" << QString("000 -m %1").arg(theInputFile);
		//QStringList runOptionsList = QStringList() << QString("-ng --set-iks 127.0.0.1 000 -m %1").arg(theInputFile);
		//theProcess->start(executableFilePath, runOptionsList, QIODevice::ReadWrite);
		QString appStr = QString("emme -ng --set-iks 127.0.0.1 000 -m %1").arg(outputScenarioFileForThreadID);
		theProcess[threadID].data()->start(appStr, QIODevice::ReadWrite);	
		bool started = theProcess[threadID].data()->waitForStarted(-1);
		if (!started) {
			rCritical(threadID) << "Error: The executable associated with" << objectName() << "cannot be started. Please check the path and syntaxes and try again.";
			exitProcess(threadID);
			return -1;
		}
	} 


	// STOP THE CODE HERE until the process is done while preventing the GUI to freeze
	processFinishedAnalyzing[threadID] = true;
	waitUntilTheProcessIsDone(false, threadID);

	if (getDisplayOutput()) {
		rDebug(threadID) << "";
		rDebug(threadID) << "-- EMME done.";
		rDebug(threadID) << "Process terminated.";
		rDebug(threadID) << "";
	}
	
	// UPDATE RESPONSES   ################### RCommandResponse is not considered for a EMME model
	int result = 0;
	// Getting the list of responses that are output of this model
	QList<RFileResponse *> responseList = findChildren<RFileResponse *>();
	// Looping over the output response list and reading the value of the response (e.g. from the files in case of a file response)
	for (int i = 0; i < responseList.count(); i++) {
		result = responseList[i]->updateValue(threadID);
	}

	// Revert back to the Rt current path
	if (threadID == 0){
		bool workingPathIsSet = QDir::setCurrent(theDomain->getCurrentFilePath());
	}

	return result;
}

void REMMEModel::exitProcess(int threadID)
{
	if (!theProcess)
		return;

	if (!theProcess[threadID]) {
		return;
	}

	if (theProcess[threadID].data()->state() == QProcess::NotRunning)
		return;

	theProcess[threadID].data()->write("q\n");  // Is this syntax right????????????
	theProcess[threadID].data()->write("q\n");
}

QString REMMEModel::getTheMainWorkingPath(){
	return QFileInfo(theInputScenarioFile).absolutePath();
}

bool REMMEModel::canBeRunParallelly(){
	return true;
}