#include "RQuadrilateralAreaSourceModel.h"	
	
#include "RParameter.h"	
#include "RLocationResponse.h"	
#include "RContinuousRandomVariable.h"
#include "RLocation.h"

#include "RMainWindow.h"
#include "RMapWidget.h"

#include <math.h>

RQuadrilateralAreaSourceModel::RQuadrilateralAreaSourceModel(QObject *parent, QString name)	
	: RModel(parent, name)
{	
	// Initializing the pointer data members
	/* //$$ 
	thePoint1Location = 0;
	thePoint2Location = 0;
	thePoint3Location = 0;
	theTheta2 = 0;
	theTheta1 = 0;
	theDepth = 0;
	*/
	
	// Instantiating the response object
	theDomain->createObject(objectName() + "Response", "RLocationResponse");
	theEpicentreLocation = qobject_cast<RLocationResponse *>(theDomain->getLastAddedObject());
	theEpicentreLocation->setModel(this);
}	

RQuadrilateralAreaSourceModel::~RQuadrilateralAreaSourceModel()	
{	
	
}	

QObject * RQuadrilateralAreaSourceModel::getPoint1Location() const	
{	
	return thePoint1Location.data();
}	

void RQuadrilateralAreaSourceModel::setPoint1Location(QObject *value)	
{	
	thePoint1Location = qobject_cast<RParameter *>(value);

	// Verification that the user has given an RLocationResponse or an RLocation
	parameterToLocation(thePoint1Location.data());
}	

QObject * RQuadrilateralAreaSourceModel::getPoint2Location() const	
{	
	return thePoint2Location.data();
}	

void RQuadrilateralAreaSourceModel::setPoint2Location(QObject *value)	
{	
	thePoint2Location = qobject_cast<RParameter *>(value);

	// Verification that the user has given an RLocationResponse or an RLocation
	parameterToLocation(thePoint2Location.data());
}	

QObject * RQuadrilateralAreaSourceModel::getPoint3Location() const	
{	
	return thePoint3Location.data();
}	

void RQuadrilateralAreaSourceModel::setPoint3Location(QObject *value)	
{	
	thePoint3Location = qobject_cast<RParameter *>(value);

	// Verification that the user has given an RLocationResponse or an RLocation
	parameterToLocation(thePoint3Location.data());
}	

QObject * RQuadrilateralAreaSourceModel::getTheta2() const	
{	
	return theTheta2.data();
}	

void RQuadrilateralAreaSourceModel::setTheta2(QObject *value)	
{	
	theTheta2 = qobject_cast<RParameter *>(value);
}	

QObject * RQuadrilateralAreaSourceModel::getTheta1() const	
{	
	return theTheta1.data();
}	

void RQuadrilateralAreaSourceModel::setTheta1(QObject *value)	
{	
	theTheta1 = qobject_cast<RParameter *>(value);
}	

QObject * RQuadrilateralAreaSourceModel::getDepth() const	
{	
	return theDepth.data();
}	

void RQuadrilateralAreaSourceModel::setDepth(QObject *value)	
{	
	theDepth = qobject_cast<RParameter *>(value);
}

QList<RParameter *> RQuadrilateralAreaSourceModel::getPhysicalParameterList()
{
	QList<RParameter *> paramList;
	paramList << theTheta1.data() << theTheta2.data() << theDepth.data();
	return paramList;
}

int RQuadrilateralAreaSourceModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)	
{	
	RLocation *thePoint1LocationObject = parameterToLocation(thePoint1Location.data());
	RLocation *thePoint2LocationObject = parameterToLocation(thePoint2Location.data());
	RLocation *thePoint3LocationObject = parameterToLocation(thePoint3Location.data());
	
	// Get information about the area source geometry
	double lng1 = thePoint1LocationObject->getLongitude(threadID);
	double lat1 = thePoint1LocationObject->getLatitude(threadID);
	double lng2 = thePoint2LocationObject->getLongitude(threadID);
	double lat2 = thePoint2LocationObject->getLatitude(threadID);
	double lng3 = thePoint3LocationObject->getLongitude(threadID);
	double lat3 = thePoint3LocationObject->getLatitude(threadID);

	// Get information about the uncertainty in the hypocentre location
	double rv1 = theTheta1.data()->getCurrentValue(threadID);
	double rv2 = theTheta2.data()->getCurrentValue(threadID);
		
	// Getting the pointer to the RLocation object from the RLocationResponse object (theEpicentreLocation)
	RLocation *theEpicentreLocationObject = parameterToLocation(theEpicentreLocation);
	if (!theEpicentreLocationObject) {
		rCritical(threadID) << "Error: The location response" << theEpicentreLocation->objectName() << "does not include any location object.";
		return -1;
	}

	double lng = lng1 + (lng2 - lng1) * rv1 + (lng3 - lng1) * rv2;// + (lng4 - lng1 + (lng3 - lng1 + lng2 - lng1)) * (rv1 + rv2) / 2.0;
	double lat = lat1 + (lat2 - lat1) * rv1 + (lat3 - lat1) * rv2;// + (lat4 - lat1 + (lat3 - lat1 + lat2 - lat1)) * (rv1 + rv2) / 2.0;
	
	theEpicentreLocationObject->setLongitude(lng,threadID);
	theEpicentreLocationObject->setLatitude(lat,threadID);
	theEpicentreLocationObject->setAltitude(-1.0 * theDepth.data()->getCurrentValue(threadID),threadID);

	if (threadID == 0){
		theEpicentreLocationObject->redisplayonMap();
	}

	return 0;
}	




QStringList RQuadrilateralAreaSourceModel::getActionList() //salarifard
{
	QStringList actionList;
	actionList << "&Display on Map" << "&Remove from Map";
	return actionList;
}

void RQuadrilateralAreaSourceModel::displayonMap() //salarifard
{

	if (!thePoint1Location || !thePoint2Location || !thePoint3Location) {
		rCritical() << "Warning: Please enter the location objects in the model" << objectName() << ".";
		return;
	}
	RMainWindow *theMainWindow = qobject_cast<RMainWindow *>(theDomain->parent());
	RMapWidget *theMapWidget = theMainWindow->findChild<RMapWidget *>();
	theMainWindow->setActiveSubWindow(theMapWidget);
	RLocation *thePoint1LocationObject = parameterToLocation(thePoint1Location.data());
	RLocation *thePoint2LocationObject = parameterToLocation(thePoint2Location.data());
	RLocation *thePoint3LocationObject = parameterToLocation(thePoint3Location.data());
	if (!theMapWidget) {
		thePoint1LocationObject->openMap();
		return;
	}

	QList<RLocation *> locationList;

	locationList << thePoint1LocationObject;
	//locationList << thePoint2LocationObject;
	//locationList << thePoint3LocationObject;


	locationList << parameterToLocation(thePoint2LocationObject);

	locationList << parameterToLocation(thePoint3LocationObject);

	RLocation thePoint4LocationObject(0, "Point4");
	thePoint4LocationObject.setLatitude(thePoint3LocationObject->getLatitude() - (thePoint2LocationObject->getLatitude() - thePoint1LocationObject->getLatitude()));
	thePoint4LocationObject.setLongitude(thePoint3LocationObject->getLongitude() + (thePoint1LocationObject->getLongitude() - thePoint2LocationObject->getLongitude()));
	locationList << &thePoint4LocationObject;

	theMapWidget->drawPolygon(this, locationList, Qt::red);



}

void RQuadrilateralAreaSourceModel::removefromMap() //salarifard
{

	RMainWindow *theMainWindow = qobject_cast<RMainWindow *>(theDomain->parent());
	RMapWidget *theMapWidget = theMainWindow->findChild<RMapWidget *>();
	if (theMapWidget) {
		theMainWindow->setActiveSubWindow(theMapWidget);
		theMapWidget->removePolygon(this);
	}


}



bool RQuadrilateralAreaSourceModel::canBeRunParallelly(){
	return true;
}