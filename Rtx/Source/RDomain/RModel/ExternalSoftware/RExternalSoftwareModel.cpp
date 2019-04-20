#include "RExternalSoftwareModel.h"

#include <QDir>
#include <QEventLoop>
#include <QApplication>

#include "RFileResponse.h"
#include "RExternalSoftwareModelOutputManager.h"

// QProcess::ExitStatus need to be registered to be used in Queued signal-slot connection
int RExternalSoftwareModel::QprocessExitStatusRegisterationID = qRegisterMetaType<QProcess::ExitStatus>("QProcess::ExitStatus");


RExternalSoftwareModel::RExternalSoftwareModel(QObject *parent, QString name)
	: RModel(parent, name)
{

	// Instantiating QProcesses & runEventLoops & processFinishedAnalyzing flags

	// array size = the number of threads
	int arraySizeForParallelizedObject = getArraySizeForParallelizedObject();

	theProcess = new QPointer<QProcess>[arraySizeForParallelizedObject];
	processFinishedAnalyzing = new bool[arraySizeForParallelizedObject];
	outputManager = new RExternalSoftwareModelOutputManager *[arraySizeForParallelizedObject];

	for (int thID = 0 ; thID < arraySizeForParallelizedObject; thID++){
		
		theProcess[thID] = 0;
		processFinishedAnalyzing[thID] = false;
		outputManager[thID] = 0;
	}
}

RExternalSoftwareModel::~RExternalSoftwareModel()
{

	// Deleting QProcesses

	int arraySizeForParallelizedObject = getArraySizeForParallelizedObject();

	for (int thID = 0 ; thID < arraySizeForParallelizedObject; thID++){
		delete(theProcess[thID]);
		if (outputManager[thID])
			delete(outputManager[thID]);
	}

	delete [] theProcess;
	delete [] processFinishedAnalyzing;
	delete [] outputManager;
}

QProcess * RExternalSoftwareModel::getProcess(int threadID)
{
	return theProcess[threadID].data();
}

void RExternalSoftwareModel::deleteOutputFiles(int threadID)
{
	QList<RFileResponse *> responseList = findChildren<RFileResponse *>();
	for (int i = 0; i < responseList.count(); i++) {
		QString filePath = responseList[i]->getResponseFile();

		// if threadID is not zero, file path must be modifie
		if (threadID != 0)
			filePath = getSuitableFileNameForThreadID(filePath, threadID);

		QFile file(filePath);
		file.remove();
	}
}

void RExternalSoftwareModel::initiateProcess(int threadID)
{
	if (!theProcess[threadID]){
		theProcess[threadID] = QPointer<QProcess>(new QProcess());
		theProcess[threadID]->setProperty("threadID", QVariant(threadID));
		outputManager[threadID] = new RExternalSoftwareModelOutputManager(threadID, finishString, this, &processFinishedAnalyzing[threadID]);
	}
	outputManager[threadID]->reset();

	QString workingPath = getTheMainWorkingPath();

	// if threadID = 0, changing working path because input files won't be modified to handle absolute path in that case (some models won't work like opensees)
	if (threadID == 0)
		bool workingPathIsSet = QDir::setCurrent(workingPath);

	// Checking if the QProcess is not running
	if (theProcess[threadID]->state() == QProcess::NotRunning) {

		// Setting system environment variables to QProcess
		QStringList environmentList = QProcess::systemEnvironment();
		theProcess[threadID]->setEnvironment(environmentList);

		// Merging the standard and error channels of the QProcess
		theProcess[threadID]->setProcessChannelMode(QProcess::MergedChannels);

		// Setting the working directory of the QProcess to the directory of the main input file (if threadID == 0)
		if (threadID == 0){
			theProcess[threadID]->setWorkingDirectory(workingPath);
		}
		// for other threads set the working path to the path_of_main_input_file/threadID so relative outputs will be created there
		else{

			QString workingPathForThreadID = workingPath + "/" + QString::number(threadID);

			// checking if directory does not exist, creating it
			QDir theMainDir(workingPath);
			if (! theMainDir.exists(QString::number(threadID))){
				theMainDir.mkdir(QString::number(threadID));
			}

			theProcess[threadID]->setWorkingDirectory(workingPathForThreadID);
		}

		// Connecting the signal of finished to the slot of onProcessFinished to check if the process has exited normally or crashed
		bool ok = connect(theProcess[threadID], SIGNAL(finished(int, QProcess::ExitStatus)), outputManager[threadID], SLOT(onProcessFinished(int, QProcess::ExitStatus)), Qt::DirectConnection);
		//connect(theProcess[threadID], SIGNAL(started()), this, SLOT(processStarted()));
	}
		
	// Connecting the signal "readyReadStandardOutput" of the QProcess to the slot "showOutput", which displays the output in the output pane
	bool ok = connect(theProcess[threadID], SIGNAL(readyReadStandardOutput()), outputManager[threadID], SLOT(showOutput()), Qt::DirectConnection);
}

void RExternalSoftwareModel::eventLoop(bool continuousProcess)
{		

	QEventLoop theEventLoop;

	bool ok;

	ok = connect(theProcess[0], SIGNAL(finished(int, QProcess::ExitStatus)), &theEventLoop, SLOT(quit()));
	if (continuousProcess) {
		ok = connect(outputManager[0], SIGNAL(processFinished(int, QProcess::ExitStatus)), &theEventLoop, SLOT(quit()));
	}

	//QTimer timer;
	//connect(&timer, SIGNAL(timeout()), &runEventLoop, SLOT(quit()));
	//timer.start(7000);

	if (processFinishedAnalyzing[0]) {
		theEventLoop.exec();
	}

	//bool active = timer.isActive();
	//if (!active) {
	//	rCritical() << "Warning: Rt has waited for 7 seconds, but the analysis of the external software is not done yet. Terminating the analysis";
	//}

	ok = disconnect(theProcess[0], SIGNAL(finished(int, QProcess::ExitStatus)), &theEventLoop, SLOT(quit()));
	
	if (continuousProcess) {
		ok = disconnect(outputManager[0], SIGNAL(processFinished(int, QProcess::ExitStatus)), &theEventLoop, SLOT(quit()));
	}
}

void RExternalSoftwareModel::waitUntilTheProcessIsDone(bool continuousProcess, int threadID){

	if (threadID == 0)
		eventLoop(continuousProcess);
	else{

		while (!outputManager[threadID]->getProcessFinishACK() && !checkProcessFinish(threadID)){
			theProcess[threadID]->waitForReadyRead(1000);
		}
	}
}

QString RExternalSoftwareModel::getSuitableFileNameForThreadID(QString fileName, int threadID){


	// if threadID is 0, returning the same name
	if (threadID == 0){
		return fileName;
	}

	QFileInfo fileInfo(fileName);

	QString pureFileName = fileInfo.fileName();

	QString modifiedName = fileName;
	modifiedName.replace(pureFileName, QString::number(threadID) + "/" + pureFileName);

	return modifiedName;
}

bool RExternalSoftwareModel::checkProcessFinish(int threadID){

	QByteArray outputBytes = theProcess[threadID]->readAllStandardOutput();

	//// The follwoing lines are required in the case that the external software should not be closed at the end of the analysis. They are needed to determine that the external software is done with the analysis.
	if (processFinishedAnalyzing[threadID]) {	
		if (outputBytes.contains(finishString)) {
			return true;
		}
	}
	return false;
}

void RExternalSoftwareModel::resetAfterParallelComputation(){

	// calling reset in super class
	RModel::resetAfterParallelComputation();

	// checking if extera directories have been created for thread specific analysis, remove them

	QDir mainWorkingDir = QDir(getTheMainWorkingPath());

	// if the directory 1 doesn't exist, it means the model has not been run parallelly
	if (! mainWorkingDir.exists("1")){
		return;
	}

	// otherwise removing all of the directories which are created for analysis in different threads
	int arraySizeForParallelizedObject = getArraySizeForParallelizedObject();

	for (int thID = 1; thID < arraySizeForParallelizedObject ; thID++){
		mainWorkingDir.rmpath(QString::number(thID));
	}

	// freeing the memory of processes and output managers
	for (int thID = 1; thID < arraySizeForParallelizedObject ; thID++){
		delete theProcess[thID];
 		theProcess[thID] = 0;
		if (outputManager[thID])
			delete outputManager[thID];
			outputManager[thID] = 0;
	}
}