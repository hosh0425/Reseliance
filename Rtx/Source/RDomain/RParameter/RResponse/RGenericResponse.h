// Last modified by Mojtaba on June 30, 2012

#ifndef RGenericResponse_H
#define RGenericResponse_H

#include "RResponse.h"

class RGenericResponse : public RResponse
{
	Q_OBJECT

public:
	RGenericResponse(QObject *parent, QString name);
	~RGenericResponse();

	// returns true as this parameter is parallelized
	bool canBeRunParallelly();

private:
	
};

#endif // RGenericResponse_H
