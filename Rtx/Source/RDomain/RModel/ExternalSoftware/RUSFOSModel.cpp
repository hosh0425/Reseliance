#include "RUSFOSModel.h"

#include <QDir>
#include <QApplication>

#include "RParameter.h"
#include "RResponse.h"
#include "RFileResponse.h"
#include "RCommandResponse.h"

RUSFOSModel::RUSFOSModel(QObject *parent, QString name)
	: RExternalSoftwareModel(parent, name)
{
	finishString = "-- USFOS done.";
}

RUSFOSModel::~RUSFOSModel()
{

}

QString RUSFOSModel::getParameterList()
{
	return parameterListToString(theParameterList);
}

void RUSFOSModel::setParameterList(QString value)
{
	theParameterList = stringToParameterList(value);
}

QString RUSFOSModel::getExecutableFile() const
{
	return theExecutableFile;
}

void RUSFOSModel::setExecutableFile(QString value)
{
	theExecutableFile = RDomain::stringToFilePath(value);
}

QString RUSFOSModel::getControlFile() const	
{	
	return theControlFile;
}	
	
void RUSFOSModel::setControlFile(QString value)	
{	
	theControlFile = RDomain::stringToFilePath(value);
}	
	
QString RUSFOSModel::getModelFile() const	
{	
	return theModelFile;
}	
	
void RUSFOSModel::setModelFile(QString value)	
{	
	theModelFile = RDomain::stringToFilePath(value);
}	
	
QString RUSFOSModel::getOptionalFile() const	
{	
	return theOptionalFile;
}	
	
void RUSFOSModel::setOptionalFile(QString value)	
{	
	theOptionalFile = RDomain::stringToFilePath(value);
}	
	
QString RUSFOSModel::getResultFile() const	
{	
	return theResultFile;
}	
	
void RUSFOSModel::setResultFile(QString value)	
{	
	theResultFile = RDomain::stringToFilePath(value);
}	

int RUSFOSModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
{	
	initiateProcess(threadID);
	
	if (theProcess[threadID].data()->state() == QProcess::NotRunning) {
		// DELETE OUTPUT FILES
		deleteOutputFiles(threadID);


		// UPDATE INPUT PARAMETERS
		QList<RParameter *> parameterList = theAllParametersList;

		// Read the contents of the control file
		QFile controlFile(theControlFile);
		if (!controlFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
			rCritical(threadID) << "Error: Could not open the file" << theControlFile << ".";
			return -1;
		}
		QTextStream controlFileTextStream(&controlFile);
		QString controlFileString = controlFileTextStream.readAll();
		controlFile.close();

		// Read the contents of the model file
		QFile modelFile(theModelFile);
		if (!modelFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
			rCritical(threadID) << "Error: Could not open the file" << theModelFile << ".";
			return -1;
		}
		QTextStream modelFileTextStream(&modelFile);
		QString modelFileString = modelFileTextStream.readAll();
		modelFile.close();

		// Read the contents of the optional file
		QFile optionalFile(theOptionalFile);
		QString optionalFileString = "";
		if (!theOptionalFile.isEmpty()) {
			if (optionalFile.open(QIODevice::ReadOnly | QIODevice::Text)) {		
				QTextStream optionalFileTextStream(&optionalFile);
				optionalFileString= optionalFileTextStream.readAll();
				optionalFile.close();
			}
		}
		
		// Iterate on all parameters to replace their names with their values in all input files
		for (int i = 0; i < parameterList.count(); i++) {
			QString name = parameterList[i]->objectName();
			double value = parameterList[i]->getCurrentValue(threadID);

			// Is this enough precession?
			controlFileString.replace(name, QString("%1").arg(value, 0, 'f', 14));
			modelFileString.replace(name, QString("%1").arg(value, 0, 'f', 14));
			optionalFileString.replace(name, QString("%1").arg(value, 0, 'f', 14));
		}

		QString additionalString = "Run";

		// Save the resulting control file with a name that includes the keyword "Run" in addition to the file name
		QString controlFileOutputName = getSuitableFileNameForThreadID(QFileInfo(theControlFile).path() + "/" + QFileInfo(theControlFile).completeBaseName() + additionalString + "." + QFileInfo(theControlFile).completeSuffix(), threadID);
		QFile outputControlFile(controlFileOutputName);
		if (!outputControlFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
			rCritical(threadID) << "Error: Could not create the file" << controlFileOutputName << ".";
			return -1;
		}
		QByteArray controlByteArray;
		controlByteArray.append(controlFileString);
		outputControlFile.write(controlByteArray);
		outputControlFile.close();

		// Save the resulting model file with a name that includes the keyword "Run" in addition to the file name
		QString modelFileOutputName = getSuitableFileNameForThreadID(QFileInfo(theModelFile).path() + "/" + QFileInfo(theModelFile).completeBaseName() + additionalString + "." + QFileInfo(theModelFile).completeSuffix(), threadID);
		QFile outputModelFile(modelFileOutputName);
		if (!outputModelFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
			rCritical(threadID) << "Error: Could not create the file" << modelFileOutputName << ".";
			return -1;
		}
		QByteArray modelByteArray;
		modelByteArray.append(modelFileString);
		outputModelFile.write(modelByteArray);
		outputModelFile.close();

		// Save the resulting optional file with a name that includes the keyword "Run" in addition to the file name
		QString optionalFileOutputName = getSuitableFileNameForThreadID(QFileInfo(theOptionalFile).path() + "/" + QFileInfo(theOptionalFile).completeBaseName() + additionalString + "." + QFileInfo(theOptionalFile).completeSuffix(), threadID);
		if (!theOptionalFile.isEmpty()) {
			QFile outputOptionalFile(optionalFileOutputName);
			if (!outputOptionalFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
				rCritical(threadID) << "Error: Could not create the file" << optionalFileOutputName << ".";
				return -1;
			}
			QByteArray optionalByteArray;
			optionalByteArray.append(optionalFileString);
			outputOptionalFile.write(optionalByteArray);
			outputOptionalFile.close();
		}

		
		// START THE PROCESS
		if (getDisplayOutput()) {
			rDebug(threadID) << "-- Starting USFOS...";
			//rDebug() << "";
		}



		/* The lines below prevent USFOAS from being run
		// Create an overarching file that includes the names of all USFOS input files
		QString runFileName = "files.inp";
		QFile runFile(runFileName);
		if (!runFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
			rCritical() << "Error: Could not create the file" << runFileName << ".";
			return -1;
		}
		QByteArray runByteArray;
		runByteArray.append(QFileInfo(controlFileOutputName).absolutePath() + "/" + QFileInfo(controlFileOutputName).completeBaseName()+ "\n");
		runByteArray.append(QFileInfo(modelFileOutputName).absolutePath() + "/" + QFileInfo(modelFileOutputName).completeBaseName()+ "\n");
		if (!theOptionalFile.isEmpty()) {
			runByteArray.append(QFileInfo(optionalFileOutputName).absolutePath() + "/" + QFileInfo(optionalFileOutputName).completeBaseName()+ "\n");
		} else {
			runByteArray.append("\n");
		}
		runByteArray.append(QFileInfo(theResultFile).absolutePath() + "/" + QFileInfo(theResultFile).completeBaseName());
		runFile.write(runByteArray);
		runFile.close();*/
		



		QStringList runOptionsList;
		QString appStr = QString("\"%1\"").arg(QFileInfo(theExecutableFile).absoluteFilePath());
		theProcess[threadID].data()->start(appStr, runOptionsList, QIODevice::ReadWrite);	
		bool started = false;
		started = theProcess[threadID].data()->waitForStarted(-1);
		if (!started) {
			rCritical(threadID) << "Error: The executable associated with" << objectName() << "cannot be started. Please check the path and syntaxes and try again.";
			exitProcess(threadID);
			return -1;
		}
		
		
		QString temp = QFileInfo(controlFileOutputName).absolutePath() + "/" + QFileInfo(controlFileOutputName).completeBaseName() + "\n";
		theProcess[threadID].data()->write(qPrintable(temp)); 
		temp = QFileInfo(modelFileOutputName).absolutePath() + "/" + QFileInfo(modelFileOutputName).completeBaseName() + "\n";
		theProcess[threadID].data()->write(qPrintable(temp)); 		
		if (!theOptionalFile.isEmpty()) {
			temp = QFileInfo(optionalFileOutputName).absolutePath() + "/" + QFileInfo(optionalFileOutputName).completeBaseName() + "\n";
		} else {
			temp = "\n";
		}
		theProcess[threadID].data()->write(qPrintable(temp)); 
		temp = QFileInfo(getSuitableFileNameForThreadID(QFileInfo(theResultFile).absoluteFilePath(), threadID)).absolutePath() + "/" + QFileInfo(theResultFile).completeBaseName() + "\n";
		theProcess[threadID].data()->write(qPrintable(temp)); 





		
		
	} 


	// STOP THE CODE HERE until the process is done while preventing the GUI to freeze
	processFinishedAnalyzing[threadID] = true;
	waitUntilTheProcessIsDone(false, threadID);

	//bool finished = theProcess->waitForFinished(3000);

	//////////??????????????????????????????????????????
	//exitProcess();

	if (getDisplayOutput()) {
		rDebug(threadID) << "";
		rDebug(threadID) << "-- USFOS done.";
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

void RUSFOSModel::exitProcess(int threadID)
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
		rDebug(threadID) << "USFOS process terminated.";
	}

}

void RUSFOSModel::reset()
{
	if (theProcess) {

		int arraySizeForParallelizedObject = getArraySizeForParallelizedObject();

		for (int thID = 0; thID < arraySizeForParallelizedObject ; thID++)
			exitProcess(thID);
	}
}

QString RUSFOSModel::getTheMainWorkingPath(){
	return QFileInfo(theControlFile).absolutePath();
}

bool RUSFOSModel::canBeRunParallelly(){
	return true;
}