#include "RFileResponse.h"

#include "RDomain.h"
#include "RExternalSoftwareModel.h"

#include <QFile>
#include <QFileInfo>

RFileResponse::RFileResponse(QObject *parent, QString name)
	: RResponse(parent, name)
{
	theMaximum = true;
	theAbsolute = true;
	theRow = 0;
	theColumn = 0;
}

RFileResponse::~RFileResponse()
{

}

bool RFileResponse::isMaximum() const
{
	return theMaximum;
}

void RFileResponse::setMaximum(bool value)
{
	theMaximum = value;
}

bool RFileResponse::isAbsolute() const
{
	return theAbsolute;
}

void RFileResponse::setAbsolute(bool value)
{
	theAbsolute = value;
}

QString RFileResponse::getTag() const
{
	return theTag;
}

void RFileResponse::setTag(QString value)
{
	theTag = value;
}

int RFileResponse::getRow() const
{
	return theRow;
}

void RFileResponse::setRow(int value)
{
	theRow = value;
}

int RFileResponse::getColumn() const
{
	return theColumn;
}

void RFileResponse::setColumn(int value)
{
	theColumn = value;
}

QString RFileResponse::getResponseFile() const
{
	return theResponseFile;
}

void RFileResponse::setResponseFile(QString value)
{
	theResponseFile = RDomain::stringToFilePath(value);
}


int RFileResponse::updateValue(int threadID)
{
	QRegExp regExp = QRegExp("[ ,;\t]");

	int row = theRow;
	int column = theColumn;
	QString filePath = theResponseFile;

	if (threadID > 0)
		filePath = RExternalSoftwareModel::getSuitableFileNameForThreadID(theResponseFile, threadID);

	QString tag = theTag;
	bool maximum = theMaximum;
	QFile file(filePath);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		rCritical(threadID) << "Error: Could not open the file" << filePath;
		return -2;
	}
	QTextStream data(&file);		

	QVector<double> valueList;
	double value = 0;
	int index = 0;
	bool tagReached = false;
	if (tag.isEmpty()) {
		tagReached = true;
	}
	while (!data.atEnd()) {
		QString line = data.readLine();
		if (tagReached) {
			if (maximum) {				
				if ((column > 0) && (row == 0)) {
					QStringList rowList = line.split(regExp, QString::SkipEmptyParts);
					if (rowList.count() < column) {
						continue;
					}
					bool ok;
					double doubleValue = rowList[column - 1].toDouble(&ok);
					if (ok) {
						valueList.append(doubleValue);
					}
				} else if ((column == 0) && (row > 0) && (index == row - 1)) {
					QStringList rowList = line.split(regExp, QString::SkipEmptyParts);
					for (int j = 0; j < rowList.count(); j++) {
						bool ok;
						double doubleValue = rowList[j].toDouble(&ok);
						if (ok) {
							valueList.append(doubleValue);
						}
					}
					break;
				} else if ((row > 0) && (column > 0) && (index == row - 1)) {
					QStringList rowList = line.split(regExp, QString::SkipEmptyParts);
					if (rowList.count() < column) {
						rCritical(threadID) <<  "Error: Could not find a number in the file" << filePath << "at row number" << row << "and column number" << column;
						return -3;
					}
					bool ok;
					double doubleValue = rowList[column - 1].toDouble(&ok);
					if (ok) {
						valueList.append(doubleValue);
					} else {
						rCritical(threadID) <<  "Error: Could not find a number in the file" << filePath << "at row number" << row << "and column number" << column << ".";
						return -3;
					}
					break;				
				} else if ((row == 0) && (column == 0)) {
					QStringList rowList = line.split(regExp, QString::SkipEmptyParts);
					for (int j = 0; j < rowList.count(); j++) {
						bool ok;
						double doubleValue = rowList[j].toDouble(&ok);
						if (ok) {
							valueList.append(doubleValue);
						}
					}
				}
			} else {
				if (row == 0) {
					row++;
				}
				if (column == 0) {
					column++;
				}
				if (index == row - 1) {
					QStringList rowList = line.split(regExp, QString::SkipEmptyParts);
					if (rowList.count() < column) {
						rCritical(threadID) <<  "Error: Could not find a number in the file" << filePath << "at row number" << row << "and column number" << column;
						return -3;
					}
					bool ok;
					double doubleValue = rowList[column - 1].toDouble(&ok);
					if (ok) {
						value = doubleValue;
						if (theAbsolute) {
							value = qAbs(value);
						}
					} else {
						rCritical(threadID) <<  "Error: Could not find a number in the file" << filePath << "at row number" << row << "and column number" << column << ".";
						return -3;
					}
					break;
				}
			}
			index++;
		}
		if (line.trimmed() == tag.trimmed()) {
			tagReached = true;
		}
	}
	file.close();

	if (!(valueList.isEmpty())) { // Means that maximum is true. 
		double max = 0;
		for (int j = 0; j < valueList.count(); j++) {
			if (theAbsolute) {
				max = qMax(qAbs(valueList[j]), max);
			} else {
				max = qMax(valueList[j], max);
			}
		} 
		value = max;
	}
	this->setCurrentValue(value, threadID);

	return 0;
	//this->setAnalyzed(true, threadID);
}

bool RFileResponse::canBeRunParallelly(){
	return true;
}