// Last modified by Mojtaba on June 30, 2012

#ifndef RFileResponse_H
#define RFileResponse_H

#include "RResponse.h"

class RFileResponse : public RResponse
{
	Q_OBJECT

	// Sets the file path of the text file
	Q_PROPERTY(QString ResponseFile READ getResponseFile WRITE setResponseFile)

	// Indicates if the response value should be set to the maximum of the values read from the file
	Q_PROPERTY(bool Maximum READ isMaximum WRITE setMaximum)

	// Indicates if the response value should be set to the absolute value of what is read from the file
	Q_PROPERTY(bool Absolute READ isAbsolute WRITE setAbsolute)

	// Sets a "tag" that is searched for in the file; The "Row" number below is relative to that tag in the text file
	Q_PROPERTY(QString Tag READ getTag WRITE setTag)

	// Sets the row number in the file that contains the response value; If entered as 0, all rows at the given "Column" are considered
	Q_PROPERTY(int Row READ getRow WRITE setRow)

	// Sets the column number in the file that contains the response value; If entered as 0, all columns at the given "Row" are considered
	Q_PROPERTY(int Column READ getColumn WRITE setColumn)

public:
	RFileResponse(QObject *parent, QString name);
	~RFileResponse();
	
	// getter/setter methods for meta-properties
	QString getResponseFile() const;
	void setResponseFile(QString value);
	bool isMaximum() const;
	void setMaximum(bool value);
	bool isAbsolute() const;
	void setAbsolute(bool value);
	QString getTag() const;
	void setTag(QString value);
	int getRow() const;
	void setRow(int value);
	int getColumn() const;
	void setColumn(int value);


	// Invokes the operation of updating the response value by reading the file, in the given threadID ($MTh$)
	int updateValue(int threadID);

	// returns true as this parameter is parallelized
	bool canBeRunParallelly();

private:
	// Data member for meta-properties
	QString theResponseFile;
	bool theMaximum;
	bool theAbsolute;
	QString theTag;
	int theRow;
	int theColumn;
	
};

#endif // RFileResponse_H
