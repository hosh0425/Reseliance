// Last modified by Mojtaba on June 30, 2012

#ifndef RLocationResponse_H
#define RLocationResponse_H

#include "RResponse.h"

class RLocation;

class RLocationResponse : public RResponse
{
	Q_OBJECT

	// Sets the RLocation object that the response "carries"
	Q_PROPERTY(QObject *Location READ getLocation WRITE setLocation)

public:
	RLocationResponse(QObject *parent, QString name);
	~RLocationResponse();
	
	// getter/setter methods for meta-properties
	QObject *getLocation() const;
	void setLocation(QObject *value);

	// Specific implementation of the setObjectName, which sets the object name for the response and its auto-generated RLocation object
	void setObjectName(const QString &name);

	// Backs up the current values of the response (latitude, longitude, and altitude), in the given threadID ($MTh$)
	void backup(int threadID);

	// Backs up the current values of the response (latitude, longitude, and altitude), in the given threadID ($MTh$)
	void restore(int threadID);

	// Returns the vector of response values, used in DDM computations; It has three elements: latitude, longitude, and altitude
	QVector<double> getValueVector(int threadID);

	// Sets the vector of response values as explained above
	void setValueVector(QVector<double> valueVector, int threadID);

	// Returns the backup vector of response values as explained above
	QVector<double> getBackupVector(int threadID);

	// Copies the value at index 0 to the index of the given threadID for data members that are changed to array due to parallelization. Each subclass must also call the method of their superclass
	void initializeForParallelComputation(int threadID);

	// Resets the location parameter for parallel analysis
	void resetAfterParallelComputation();

	// returns true as this parameter is parallelized (only if the location is parallelized too)
	bool canBeRunParallelly();

private:
	// Data member for meta-properties
	RLocation *theLocation;

	// Data member for array of the backup vectors of response values in each threadID as explained above ($MTh$)
	QVector<double> * backupVector;
	
};

#endif // RLocationResponse_H
