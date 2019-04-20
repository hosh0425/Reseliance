#include "RSAP2000Model.h"

#include <QDir>
#include <QApplication>

#include "RParameter.h"
#include "RDatabaseResponse.h"

RSAP2000Model::RSAP2000Model(QObject *parent, QString name)
	: RExternalSoftwareModel(parent, name)
{
	finishString = "-- SAP2000 done.";
	theSolverType = RSAP2000Model::Standard;
}

RSAP2000Model::~RSAP2000Model()
{

}

QString RSAP2000Model::getParameterList()
{
	return parameterListToString(theParameterList);
}

void RSAP2000Model::setParameterList(QString value)
{
	theParameterList = stringToParameterList(value);
}

QString RSAP2000Model::getInputFile() const
{
	return theInputFile;
}

void RSAP2000Model::setInputFile(QString value)
{
	theInputFile = RDomain::stringToFilePath(value);
}

QString RSAP2000Model::getExecutableFile() const
{
	return theExecutableFile;
}

void RSAP2000Model::setExecutableFile(QString value)
{
	theExecutableFile = RDomain::stringToFilePath(value);
}

RSAP2000Model::RSolverType RSAP2000Model::getSolverType() const	
{	
	return theSolverType;
}	
	
void RSAP2000Model::setSolverType(RSAP2000Model::RSolverType value)	
{	
	theSolverType = value;
}	

int RSAP2000Model::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
{
	initiateProcess(threadID);

	if (theProcess[threadID].data()->state() == QProcess::NotRunning) {
		// DELETE OUTPUT FILES
		deleteOutputFiles(threadID);  ///////////////////////??????


		// UPDATE INPUT PARAMETERS
		QList<RParameter *> parameterList = theAllParametersList;

		QFile inputFile(theInputFile);
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
		
		QString outputName =  getSuitableFileNameForThreadID(QFileInfo(theInputFile).path() + "/" + QFileInfo(theInputFile).completeBaseName() + "Run" + "." + QFileInfo(theInputFile).completeSuffix(), threadID);
		QFile outputFile(outputName);
		if (!outputFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
			rCritical(threadID) << "Error: Could not create the file" << theOutputScenarioFile << ".";
			return -1;
		}
		QByteArray outputByteArray;
		outputByteArray.append(inputFileString);

		outputFile.write(outputByteArray);
		outputFile.close();
		QString outputAbsoluteFilePath = QFileInfo(outputName).absoluteFilePath();

		
		// START THE PROCESS
		if (getDisplayOutput()) {
			rDebug(threadID) << "-- Starting SAP2000...";
			//rDebug() << "";
		}

		QStringList runOptionsList;
		QString solverStr;
		if (theSolverType == RSAP2000Model::Standard) {
			solverStr = "S1";
		} else {
			solverStr = "S2";
		}
		runOptionsList << QString("%1").arg(outputAbsoluteFilePath).replace("/","\\") << "/R" << solverStr << "P1" << "/C" << "/K";
		QString appStr = QString("\"%1\"").arg(QFileInfo(theExecutableFile).absoluteFilePath()).replace("/","\\"); 
		theProcess[threadID].data()->start(theExecutableFile, runOptionsList, QIODevice::ReadWrite);		
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
		rDebug(threadID) << "-- SAP2000 done.";
		rDebug(threadID) << "Process terminated.";
		rDebug(threadID) << "";
	}

	
	// UPDATE RESPONSES   
	int result = 0;
	// Getting the list of responses that are output of this model
	QList<RDatabaseResponse *> responseList = findChildren<RDatabaseResponse *>();
	// Looping over the output response list and reading the value of the response (e.g. from the databases in case of a database response)
	for (int i = 0; i < responseList.count(); i++) {
		result = responseList[i]->updateValue(threadID);
	}

	// Revert back to the Rt current path
	if (threadID == 0){
		bool workingPathIsSet = QDir::setCurrent(theDomain->getCurrentFilePath());
	}

	return result;
}

void RSAP2000Model::exitProcess(int threadID)
{
	if (!theProcess)
		return;

	if (!theProcess[threadID]) {
		return;
	}

	if (theProcess[threadID].data()->state() == QProcess::NotRunning)
		return;

	//theProcess->write("q\n");
	
	// Are the following needed to be done here?

}

QString RSAP2000Model::getTheMainWorkingPath(){
	return QFileInfo(theInputFile).absolutePath();
}

bool RSAP2000Model::canBeRunParallelly(){
	return true;
}