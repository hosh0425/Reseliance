#include "RLocationResponse.h"

#include "RDomain.h"
#include "RLocation.h"

RLocationResponse::RLocationResponse(QObject *parent, QString name)
	: RResponse(parent, name)
{
	RDomain *theDomain = qobject_cast<RDomain *>(parent);

	// Instantiate an RLocation
	theDomain->createObject(objectName() + "Location", "RLocation");
	theLocation = qobject_cast<RLocation *>(theDomain->getLastAddedObject());

	// initializing the vector
	int arraySizeForParallelizedObject = getArraySizeForParallelizedObject();
	backupVector = new QVector<double>[arraySizeForParallelizedObject];
}

RLocationResponse::~RLocationResponse()
{
	if (theLocation) {
		delete theLocation;
	}
	theLocation = 0;

	delete [] backupVector;
}

QObject * RLocationResponse::getLocation() const
{
	return theLocation;
}

void RLocationResponse::setLocation(QObject *value)
{
	theLocation = qobject_cast<RLocation *>(value);
}

void RLocationResponse::setObjectName(const QString &name)
{
	QString oldName = objectName();
	QObject *object = qobject_cast<QObject *>(this);
	object->setObjectName(name);

	QString locationName = theLocation->objectName();
	if (locationName.contains(oldName)) {
		QString newLocationName = locationName.replace(oldName, name);
		theLocation->setObjectName(newLocationName);
	}
}

void RLocationResponse::backup(int threadID)
{
	backupVector[threadID].clear();
	backupVector[threadID] << theLocation->getLatitude(threadID);
	backupVector[threadID] << theLocation->getLongitude(threadID);
	backupVector[threadID] << theLocation->getAltitude(threadID);
}

void RLocationResponse::restore(int threadID)
{
	if (backupVector[threadID].count() != 3) {
		rCritical(threadID) << "Error: There is no backup vector for the response" << objectName() << ".";
		return;
	}

	theLocation->setLatitude(backupVector[threadID][0], threadID);
	theLocation->setLongitude(backupVector[threadID][1], threadID);
	theLocation->setAltitude(backupVector[threadID][2], threadID);
}

QVector<double> RLocationResponse::getValueVector(int threadID)
{
	QVector<double> resultVector;
	resultVector << theLocation->getLatitude(threadID);
	resultVector << theLocation->getLongitude(threadID);
	resultVector << theLocation->getAltitude(threadID);
	return resultVector;
}

QVector<double> RLocationResponse::getBackupVector(int threadID)
{
	return backupVector[threadID];
}

void RLocationResponse::setValueVector(QVector<double> valueVector, int threadID)
{
	if (valueVector.count() != 3) {
		rCritical(threadID) << "Error: The value vector passed to the response" << objectName() << "does not include enough elements.";
		return;
	}
	theLocation->setLatitude(valueVector[0], threadID);
	theLocation->setLongitude(valueVector[1], threadID);
	theLocation->setAltitude(valueVector[2], threadID);
}

void RLocationResponse::initializeForParallelComputation(int threadID){
	
	// calling the method in super class
	RResponse::initializeForParallelComputation(threadID);

	// calling the method in location data member
	theLocation->initializeForParallelComputation(threadID);
}

void RLocationResponse::resetAfterParallelComputation(){

	// calling the method in super class
	RResponse::resetAfterParallelComputation();

	// calling the method in location data member
	theLocation->resetAfterParallelComputation();
}

bool RLocationResponse::canBeRunParallelly(){
	
	return theLocation->canBeRunParallelly();
}