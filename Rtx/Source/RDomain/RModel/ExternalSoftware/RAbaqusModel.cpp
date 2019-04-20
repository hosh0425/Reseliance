#include "RAbaqusModel.h"

#include <QDir>
#include <QApplication>

#include "RParameter.h"
#include "RResponse.h"
#include "RFileResponse.h"
#include "RCommandResponse.h"

RAbaqusModel::RAbaqusModel(QObject *parent, QString name)
	: RExternalSoftwareModel(parent, name)
{
	finishString = "-- Abaqus done.";
	theNumberOfProcessors = 1;
}

RAbaqusModel::~RAbaqusModel()
{

}

QString RAbaqusModel::getParameterList()
{
	return parameterListToString(theParameterList);
}

void RAbaqusModel::setParameterList(QString value)
{
	theParameterList = stringToParameterList(value);
}

QString RAbaqusModel::getExecutableFile() const
{
	return theExecutableFile;
}

void RAbaqusModel::setExecutableFile(QString value)
{
	theExecutableFile = RDomain::stringToFilePath(value);
}

QString RAbaqusModel::getInputFile() const
{
	return theInputFile;
}

void RAbaqusModel::setInputFile(QString value)
{
	theInputFile = RDomain::stringToFilePath(value);
}

QString RAbaqusModel::getPostProcessingPythonScript() const
{
	return thePostProcessingPythonScript;
}

void RAbaqusModel::setPostProcessingPythonScript(QString value)
{
	thePostProcessingPythonScript = RDomain::stringToFilePath(value);
	QString temp = QFileInfo(thePostProcessingPythonScript).completeBaseName() + "." + QFileInfo(thePostProcessingPythonScript).completeSuffix();
	if (thePostProcessingPythonScript != temp && !thePostProcessingPythonScript.isEmpty()) {
		rCritical() << "Warning: The \"Output Extraction Python Script\" in object" << objectName() << "must not include the file path. Rather, it must only include the file name and extension. Rt will remove the file path from the given value. This file must be placed in the same address as the \"Input file\".";
		thePostProcessingPythonScript = temp;
	}
}

int RAbaqusModel::getNumberOfProcessors() const
{
	return theNumberOfProcessors;
}

void RAbaqusModel::setNumberOfProcessors(int value)
{
	if (value < 1) {
		value = 1;
		rCritical() << "Warning: The number of processors given to" << objectName() << "is less than 1. Rt will use the value of 1.";
	}
	theNumberOfProcessors = value;
}

int RAbaqusModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
{	
	initiateProcess(threadID);
	
	// using absolute file path for executable file 
	QString theAbsExecutableFile = QFileInfo(theExecutableFile).absoluteFilePath();

	if (theProcess[threadID].data()->state() == QProcess::NotRunning) {
		// DELETE OUTPUT FILES
		deleteOutputFiles(threadID);

		// UPDATE INPUT PARAMETERS
		QList<RParameter *> parameterList = theAllParametersList;

		// Read the contents of the input file
		QFile inputFile(theInputFile);
		if (!inputFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
			rCritical(threadID) << "Error: Could not open the file" << theInputFile << ".";
			return -1;
		}
		QTextStream inputFileTextStream(&inputFile);
		QString inputFileString = inputFileTextStream.readAll();
		inputFile.close();

		
		// Iterate on all parameters to replace their names with their values in all input files
		for (int i = 0; i < parameterList.count(); i++) {
			QString name = parameterList[i]->objectName();
			double value = parameterList[i]->getCurrentValue(threadID);

			// Is this enough precession?
			inputFileString.replace(name, QString("%1").arg(value, 0, 'f', 14));
		}

		// Save the resulting input file with a name that includes the keyword "Run" in addition to the file name
		QString inputFilePath = QFileInfo(theInputFile).path();
		QString inputFileBaseName = QFileInfo(theInputFile).completeBaseName();
		QString inputFileOutputBaseName = QFileInfo(theInputFile).completeBaseName() + "Run";
		QString inputFileOutputName = getSuitableFileNameForThreadID(inputFilePath + "/" + inputFileOutputBaseName + "." + QFileInfo(theInputFile).completeSuffix(), threadID);

		//inputFileOutputName = inputFileOutputName.replace(inputFileBaseName, inputFileOutputBaseName);
		QFile outputInputFile(inputFileOutputName);
		if (!outputInputFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
			rCritical(threadID) << "Error: Could not create the file" << inputFileOutputName << ".";
			return -1;
		}
		QByteArray inputByteArray;
		inputByteArray.append(inputFileString);
		outputInputFile.write(inputByteArray);
		outputInputFile.close();		

		QString inputFileOutputFilePath = QFileInfo(inputFileOutputName).absoluteFilePath();

		
		QString inputFileExtension = QFileInfo(theInputFile).suffix();
		QString appStr;
		QStringList runOptionsList;
		if (inputFileExtension.toLower() == QString ("py").toLower()) {
			// Run the Python script to start abaqus if the input file is indeed a .py file
			// E.g., "C:\Program Files\SIMULIA\Abaqus\6.10-1\exec\abq6101.exe" cae nogui=in.py
			// In this case, theNumberOfProcessors will not be used
			appStr = QString("\"%1\"").arg(theAbsExecutableFile) + " cae nogui=" + inputFileOutputName;
		} else {
			// Create the syntax to run Abaqus if the input file is Abaqus' native input
			// E.g., "C:\Program Files\SIMULIA\Abaqus\6.10-1\exec\abq6101.exe" cpus=1 job=Job-Exp input="C:\Abaqus Example\Job-Exp.inp" memory="3000 mb" interactive
			appStr = QString("\"%1\"").arg(theAbsExecutableFile) + QString(" cpus=%1").arg(theNumberOfProcessors) + " job=" + inputFileBaseName + " input=\"" + inputFileOutputFilePath + "\" interactive";
		}

		theProcess[threadID].data()->start(appStr, runOptionsList, QIODevice::ReadWrite);	
		bool started = false;
		started = theProcess[threadID].data()->waitForStarted(-1);
		if (!started) {
			rCritical(threadID) << "Error: The executable associated with" << objectName() << "cannot be started. Please check the path and syntaxes and try again.";
			exitProcess(threadID);
			return -1;
		}
	} 


	// STOP THE CODE HERE until the process is done while preventing the GUI to freeze
	processFinishedAnalyzing[threadID] = true;
	waitUntilTheProcessIsDone(false, threadID);


	// Run the Python script to extract the outputs into text files
	// This is only done when Abaqus is run in explicit mode
	// E.g., "C:\Program Files\SIMULIA\Abaqus\6.10-1\exec\abq6101.exe" cae nogui=ex.py
	if (!thePostProcessingPythonScript.isEmpty()) {
		QStringList runOptionsList;
		QString appStr = QString("\"%1\"").arg(theAbsExecutableFile) + " cae nogui=" + thePostProcessingPythonScript;
		theProcess[threadID].data()->start(appStr, runOptionsList, QIODevice::ReadWrite);	
		bool started = false;
		started = theProcess[threadID].data()->waitForStarted(-1);
		if (!started) {
			rCritical(threadID) << "Error: The executable associated with" << objectName() << "cannot be started. Please check the path and syntaxes and try again.";
			exitProcess(threadID);
			return -1;
		}


		// STOP THE CODE HERE until the process is done while preventing the GUI to freeze
		processFinishedAnalyzing[threadID] = true;
		waitUntilTheProcessIsDone(false, threadID);
	} else {
		rCritical(threadID) << "Error: No Python script is provided for the model" << objectName() << "to extract the output from Abaqus.";
		exitProcess(threadID);
		return -1;
	}



	if (getDisplayOutput()) {
		rDebug(threadID) << "";
		rDebug(threadID) << "-- Abaqus done.";
		rDebug(threadID) << "Process terminated.";
		rDebug(threadID) << "";
	}

	
	// UPDATE RESPONSES   ################### RCommandResponse is not considered for this model
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

void RAbaqusModel::exitProcess(int threadID)
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
		rDebug(threadID) << "ABAQUS process terminated.";
	}
}

void RAbaqusModel::reset()
{
	if (theProcess) {

		int arraySizeForParallelizedObject = getArraySizeForParallelizedObject();

		for (int thID = 0; thID < arraySizeForParallelizedObject ; thID++)
			exitProcess(thID);
	}
}

QString RAbaqusModel::getTheMainWorkingPath(){
	return QFileInfo(theInputFile).absolutePath();
}

bool RAbaqusModel::canBeRunParallelly(){
	return true;
}