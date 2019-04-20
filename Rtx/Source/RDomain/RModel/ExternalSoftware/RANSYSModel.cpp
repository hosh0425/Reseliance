#include "RANSYSModel.h"

#include <QDir>
#include <QApplication>

#include "RParameter.h"
#include "RResponse.h"
#include "RFileResponse.h"
#include "RCommandResponse.h"


RANSYSModel::RANSYSModel(QObject *parent, QString name)
	: RExternalSoftwareModel(parent, name)
{
	finishString = "-- ANSYS done.";
}

RANSYSModel::~RANSYSModel()
{

}

QString RANSYSModel::getParameterList()
{
	return parameterListToString(theParameterList);
}

void RANSYSModel::setParameterList(QString value)
{
	theParameterList = stringToParameterList(value);
}

QString RANSYSModel::getExecutableFile() const
{
	return theExecutableFile;
}

void RANSYSModel::setExecutableFile(QString value)
{
	theExecutableFile = RDomain::stringToFilePath(value);
}

QString RANSYSModel::getInputFile() const
{
	return theInputFile;
}

void RANSYSModel::setInputFile(QString value)
{
	theInputFile = RDomain::stringToFilePath(value);
}

int RANSYSModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
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

			// This will issue a "set" command to ANSYS in order to set the value of the parameter
			inputFileString = QString("*SET,%1,%2\n").arg(name).arg(value, 0, 'f', 14) + inputFileString;

			// Instead, we could replace the name of the parameter with its value using the command below, which is NOT done now
			//inputFileString.replace(name, QString("%1").arg(value, 0, 'f', 14));
		}

		// Save the resulting input file with a name that includes the keyword "Run" in addition to the file name
		QString inputFilePath = QFileInfo(theInputFile).path();
		QString inputFileAbsolutePath = QFileInfo(theInputFile).absolutePath();
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

		QStringList runOptionsList;

		QString dir = inputFileAbsolutePath;
		if (threadID > 0)
			dir = dir + "/" + QString::number(threadID);

		QString appStr = QString("\"%1\"").arg(theAbsExecutableFile) + " -b -j " + inputFileBaseName + " -dir \"" + dir + "\" -o test.out -i \"" + inputFileOutputFilePath + "\"";
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

	//bool finished = theProcess->waitForFinished(3000);

	//////////??????????????????????????????????????????
	//exitProcess();

	if (getDisplayOutput()) {
		rDebug(threadID) << "";
		rDebug(threadID) << "-- ANSYS done.";
		rDebug(threadID) << "Process terminated.";
		rDebug(threadID) << "";
	}
	
	// UPDATE RESPONSES   ################### RCommandResponse is not considered for a USOFS model
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

void RANSYSModel::exitProcess(int threadID)
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
		rDebug(threadID) << "ANSYS process terminated.";
	}
}

void RANSYSModel::reset()
{
	if (theProcess) {

		int arraySizeForParallelizedObject = getArraySizeForParallelizedObject();

		for (int thID = 0; thID < arraySizeForParallelizedObject ; thID++)
			exitProcess(thID);
	}
}

QString RANSYSModel::getTheMainWorkingPath(){
	return QFileInfo(theInputFile).absolutePath();
}

bool RANSYSModel::canBeRunParallelly(){
	return true;
}