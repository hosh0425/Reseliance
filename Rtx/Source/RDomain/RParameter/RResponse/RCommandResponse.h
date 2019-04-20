// Last modified by Mojtaba on June 30, 2012

#ifndef RCommandResponse_H
#define RCommandResponse_H

#include "RResponse.h"

class RCommandResponse : public RResponse
{
	Q_OBJECT

	// Sets the command that is issued to the external software application to get the response value
	Q_PROPERTY(QString Command READ getCommand WRITE setCommand)

	// Indicates if the response value should be set to the absolute value of what is returned by the external software application
	Q_PROPERTY(bool Absolute READ isAbsolute WRITE setAbsolute)

public:
	RCommandResponse(QObject *parent, QString name);
	~RCommandResponse();
	
	// getter/setter methods for meta-properties
	QString getCommand() const;
	void setCommand(QString value);
	bool isAbsolute() const;
	void setAbsolute(bool value);

	// Invokes the operation of updating the response value by issuing the command to the external software application, in the given threadID ($MTh$)
	int updateValue(int threadID);

	// returns true as this parameter is parallelized
	bool canBeRunParallelly();

private:
	// Data member for meta-properties
	QString theCommand;
	bool theAbsolute;
	
};

#endif // RCommandResponse_H
