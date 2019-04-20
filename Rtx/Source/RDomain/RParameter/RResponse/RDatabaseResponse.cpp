#include "RDatabaseResponse.h"

#include "RDomain.h"

#include <QFile>
#include <QFileInfo>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QVariant>
#include <QDir>


#include "RExternalSoftwareModel.h"

RDatabaseResponse::RDatabaseResponse(QObject *parent, QString name)
	: RResponse(parent, name)
{
	theMaximum = true;
	theAbsolute = true;
}

RDatabaseResponse::~RDatabaseResponse()
{

}

bool RDatabaseResponse::isMaximum() const
{
	return theMaximum;
}

void RDatabaseResponse::setMaximum(bool value)
{
	theMaximum = value;
}

bool RDatabaseResponse::isAbsolute() const
{
	return theAbsolute;
}

void RDatabaseResponse::setAbsolute(bool value)
{
	theAbsolute = value;
}

QString RDatabaseResponse::getTable() const
{
	return theTable;
}

void RDatabaseResponse::setTable(QString value)
{
	theTable = value;
}

QString RDatabaseResponse::getRow() const
{
	return theRow;
}

void RDatabaseResponse::setRow(QString value)
{
	theRow = value;
}

QString RDatabaseResponse::getColumn() const
{
	return theColumn;
}

void RDatabaseResponse::setColumn(QString value)
{
	theColumn = value;
}

QString RDatabaseResponse::getResponseDatabase() const
{
	return theResponseDatabase;
}

void RDatabaseResponse::setResponseDatabase(QString value)
{
	theResponseDatabase = RDomain::stringToFilePath(value);
}

int RDatabaseResponse::updateValue(int threadID)
{
	QSqlDatabase database = QSqlDatabase::addDatabase("QODBC");
	//salarifard: database file address is changed from relative to absolute
	QString currentPath = QDir::currentPath();
	QString databaseAddress = currentPath + "/" + RExternalSoftwareModel::getSuitableFileNameForThreadID(theResponseDatabase, threadID);
	//QString databaseAddress = RExternalSoftwareModel::getSuitableFileNameForThreadID(theResponseDatabase,threadID).replace("/", "\\\\");
	//database.setDatabaseName("Driver={Microsoft Access Driver (*.mdb)};DSN='';DBQ=" + databaseAddress);
	database.setDatabaseName("DRIVER={Microsoft Access Driver (*.mdb, *.accdb)};FIL={MS Access};DBQ=" + databaseAddress);
	if(database.open() == false) {
		rCritical(threadID) << "Error: Could not open the database" << theResponseDatabase << "with the following error:" << database.lastError().text();
		return -2;
	}

	if (theTable.isEmpty()) {
		rCritical(threadID) << "Error in" << this->objectName() << ": The name of the Table in the database is empty.";
		return -3;
	}
	int row = -1;
	if (!theRow.isEmpty()) {
		bool ok;
		row = theRow.toInt(&ok);
		if (!ok) {
			rCritical(threadID) << "Error in" << this->objectName() << ": Row must be an integer.";
			return -3;
		}
		if (row < 1) {
			rCritical(threadID) << "Error in" << this->objectName() << ": Row must be equal to or greater than 1.";
			return -3;
		}
		row--;
	}

	QVector<double> valueList;
	double value;
	bool maximumMistakenlySelected = false;
	if (theMaximum) {
		if (theRow.isEmpty() && theColumn.isEmpty()) {
			QSqlQuery query("SELECT * FROM [" + theTable + "]", database);
			while (query.next()) {
				QVariant valueVariant = 0;
				int i = 0;
				while (valueVariant.isValid()) {
					valueVariant = query.value(i);
					if (valueVariant.isValid()) {
						bool ok;
						double doubleValue = valueVariant.toDouble(&ok);
						if (ok) {
							valueList << doubleValue;
						}
					}
					i++;
				}
			}
		}
		if (!theRow.isEmpty() && theColumn.isEmpty()) {
			QSqlQuery query("SELECT * FROM [" + theTable + "]", database);
			bool ok = query.seek(row, false);
			if (!ok) {
				rCritical(threadID) << "Error in" << this->objectName()<< ": Row" << theRow << "could not be found.";
				return -3;
			}
			QVariant valueVariant(0);
			int i = 0;
			while (valueVariant.isValid()) {
				valueVariant = query.value(i);
				if (valueVariant.isValid()) {
					bool ok;
					double doubleValue = valueVariant.toDouble(&ok);
					if (ok) {
						valueList << doubleValue;
					}
				}
				i++;
			}
		}
		if (theRow.isEmpty() && !theColumn.isEmpty()) {
			QSqlQuery query("SELECT [" + theColumn + "] FROM [" + theTable + "]", database);
			if (query.record().indexOf(theColumn) < 0) {
				rCritical(threadID) << "Error in" << this->objectName() << ": Column" << theColumn << "could not be found.";
				return -3;
			}
			while (query.next()) {
				QVariant valueVariant;
				valueVariant = query.value(0);
				if (valueVariant.isValid()) {
					bool ok;
					double doubleValue = valueVariant.toDouble(&ok);
					if (ok) {
						valueList << doubleValue;
					}
				}
			}
		}
		if (!theRow.isEmpty() && !theColumn.isEmpty()) {
			maximumMistakenlySelected = true;
		}
	}
	if (maximumMistakenlySelected || !theMaximum) {
		if (theRow.isEmpty() || theColumn.isEmpty()) {
			rCritical(threadID) << "Error in" << this->objectName() << ": Row or Column or both are left empty. Enter both or set Maximum to True.";
			return -3;
		}
		QSqlQuery query("SELECT [" + theColumn + "] FROM [" + theTable + "]", database);
		if (query.record().indexOf(theColumn) < 0) {
			rCritical(threadID) << "Error in" << this->objectName() << ": Column" << theColumn << "could not be found.";
			return -3;
		}
		bool ok = query.seek(row, false);
		if (!ok) {
			rCritical(threadID) << "Error in" << this->objectName()<< ": Row" << theRow << "could not be found.";
			return -3;
		}
		QVariant valueVariant;
		valueVariant = query.value(0);
		if (valueVariant.isValid()) {
			bool ok;
			double doubleValue = valueVariant.toDouble(&ok);
			if (ok) {
				if (theAbsolute) {
					value = qAbs(doubleValue);
				} else {
					value = doubleValue;
				}
			}
		}
	}

	database.close();

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

bool RDatabaseResponse::canBeRunParallelly(){
	return true;
}