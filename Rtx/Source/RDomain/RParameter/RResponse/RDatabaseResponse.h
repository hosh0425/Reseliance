// Last modified by Mojtaba on June 30, 2012

#ifndef RDatabaseResponse_H
#define RDatabaseResponse_H

#include "RResponse.h"

class RDatabaseResponse : public RResponse
{
	Q_OBJECT

	// Sets the file path of the database file
	Q_PROPERTY(QString ResponseDatabase READ getResponseDatabase WRITE setResponseDatabase)

	// Indicates if the response value should be set to the maximum of the values read from the file
	Q_PROPERTY(bool Maximum READ isMaximum WRITE setMaximum)

	// Indicates if the response value should be set to the absolute value of what is read from the file
	Q_PROPERTY(bool Absolute READ isAbsolute WRITE setAbsolute)

	// Sets the Table name in the database
	Q_PROPERTY(QString Table READ getTable WRITE setTable)

	// Sets the row tag in the table that contains the response value; If empty, all rows at the given "Column" are considered
	Q_PROPERTY(QString Row READ getRow WRITE setRow)

	// Sets the column tag in the table that contains the response value; If empty, all columns at the given "Row" are considered
	Q_PROPERTY(QString Column READ getColumn WRITE setColumn)

public:
	RDatabaseResponse(QObject *parent, QString name);
	~RDatabaseResponse();
	
	// getter/setter methods for meta-properties
	QString getResponseDatabase() const;
	void setResponseDatabase(QString value);
	bool isMaximum() const;
	void setMaximum(bool value);
	bool isAbsolute() const;
	void setAbsolute(bool value);
	QString getTable() const;
	void setTable(QString value);
	QString getRow() const;
	void setRow(QString value);
	QString getColumn() const;
	void setColumn(QString value);

	// Invokes the operation of updating the response value by reading the file, in the given threadID ($MTh$)
	int updateValue(int threadID);

	// returns true as this parameter is parallelized
	bool canBeRunParallelly();

private:
	// Data member for meta-properties
	QString theResponseDatabase;
	bool theMaximum;
	bool theAbsolute;
	QString theTable;
	QString theRow;
	QString theColumn;
	
};

#endif // RDatabaseResponse_H
