#include "RCommandResponse.h"

#include <QProcess>

#include "RExternalSoftwareModel.h"

RCommandResponse::RCommandResponse(QObject *parent, QString name)
	: RResponse(parent, name)
{

}

RCommandResponse::~RCommandResponse()
{

}

QString RCommandResponse::getCommand() const
{
	return theCommand;
}

void RCommandResponse::setCommand(QString value)
{
	theCommand = value;
}

bool RCommandResponse::isAbsolute() const
{
	return theAbsolute;
}

void RCommandResponse::setAbsolute(bool value)
{
	theAbsolute = value;
}

int RCommandResponse::updateValue(int threadID)
{
	RExternalSoftwareModel *model = qobject_cast<RExternalSoftwareModel *>(this->getModel());
	if (!model) {
		rCritical(threadID) << "Error: The response" << objectName() << "is not given any models.";
		return -1;
	}

	QProcess *process = model->getProcess(threadID);
	if (!process) {
		rCritical(threadID) << "Error: The response" << objectName() << "cannot get the process from the model" << model->objectName();
		return -1;
	}

	process->write(qPrintable(theCommand + "\n"));
	// Wait until something is ready to be read or 3000 milliseconds, which ever comes first
	
	bool ok;

	ok = process->waitForReadyRead(3000);
	if (!ok) {
		rCritical(threadID) << "Error: Could not get the value of the command response" << objectName() << "which is the output of the model" << model->objectName();
		return -3;
	}

	QByteArray tempBA = process->readAllStandardOutput();
	QString tempStr = QString(tempBA).trimmed();
	

	double value = tempStr.toDouble(&ok);
	if (theAbsolute) {
		value = qAbs(value);
	}
	
	if (ok) {
		this->setCurrentValue(value, threadID);
	} else {
		rCritical(threadID) << "Error: Could not update the value of the command response" << objectName() << "which is the output of the model" << model->objectName();
		return -3;
	}

	return 0;
}

bool RCommandResponse::canBeRunParallelly(){
	return true;
}