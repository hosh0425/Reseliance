#include "RRectangularAreaSourceModel.h"	
	
#include "RParameter.h"	
#include "RLocationResponse.h"	
#include "RContinuousRandomVariable.h"
#include "RLocation.h"
#include "RMainWindow.h"
#include "RMapWidget.h"

#include <gsl/gsl_math.h>

#include <math.h>

RRectangularAreaSourceModel::RRectangularAreaSourceModel(QObject *parent, QString name)	
	: RModel(parent, name)
{	
	// Initializing the pointer data members
	/* //$$ 
	thePoint1Location = 0;
	thePoint2Location = 0;
	theTheta1 = 0;
	theTheta2 = 0;
	theDepth = 0;
	*/
	
	// Instantiating the response object
	theDomain->createObject(objectName() + "Response", "RLocationResponse");
	theHypocentreLocation = qobject_cast<RLocationResponse *>(theDomain->getLastAddedObject());
	theHypocentreLocation->setModel(this);
}	

RRectangularAreaSourceModel::~RRectangularAreaSourceModel()	
{	
	
}	

QObject * RRectangularAreaSourceModel::getPoint1Location() const	
{	
	return thePoint1Location.data();
}	

void RRectangularAreaSourceModel::setPoint1Location(QObject *value)	
{	
	thePoint1Location = qobject_cast<RParameter *>(value);

	// Verification that the user has given an RLocationResponse or an RLocation
	parameterToLocation(thePoint1Location.data());
}	

QObject * RRectangularAreaSourceModel::getPoint2Location() const	
{	
	return thePoint2Location.data();
}	

void RRectangularAreaSourceModel::setPoint2Location(QObject *value)	
{
	thePoint2Location = qobject_cast<RParameter *>(value);

	// Verification that the user has given an RLocationResponse or an RLocation
	parameterToLocation(thePoint2Location.data());
}		

QObject * RRectangularAreaSourceModel::getTheta1() const	
{	
	return theTheta1.data();
}	

void RRectangularAreaSourceModel::setTheta1(QObject *value)	
{	
	theTheta1 = qobject_cast<RParameter *>(value);
}	

QObject * RRectangularAreaSourceModel::getTheta2() const	
{	
	return theTheta2.data();
}	

void RRectangularAreaSourceModel::setTheta2(QObject *value)	
{	
	theTheta2 = qobject_cast<RParameter *>(value);
}

QObject * RRectangularAreaSourceModel::getDepth() const	
{	
	return theDepth.data();
}	

void RRectangularAreaSourceModel::setDepth(QObject *value)	
{	
	theDepth = qobject_cast<RParameter *>(value);
}

QList<RParameter *> RRectangularAreaSourceModel::getPhysicalParameterList()
{
	QList<RParameter *> paramList;
	paramList << theTheta1.data() << theTheta2.data() << theDepth.data();
	return paramList;
}

int RRectangularAreaSourceModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)	
{	
	RLocation *thePoint1LocationObject = parameterToLocation(thePoint1Location.data());
	RLocation *thePoint2LocationObject = parameterToLocation(thePoint2Location.data());
	
	// Get information about the area source geometry
	double lng1 = thePoint1LocationObject->getLongitude(threadID);
	double lat1 = thePoint1LocationObject->getLatitude(threadID);
	double lng2 = thePoint2LocationObject->getLongitude(threadID);
	double lat2 = thePoint2LocationObject->getLatitude(threadID);

	double CC = M_PI / 180.0;
	double northLat = qMax(lat1, lat2) * CC;
	double southLat = qMin(lat1, lat2) * CC;
	double eastLng = qMax(lng1, lng2);
	double westLng = qMin(lng1, lng2);
	
	double deltaLng = eastLng - westLng;

	// Get information about the uncertainty in the hypocentre location
	double theta1 = theTheta1.data()->getCurrentValue(threadID);
	double theta2 = theTheta2.data()->getCurrentValue(threadID);
		
	// Getting the pointer to the RLocation object from the RLocationResponse object (theHypocentreLocation)
	RLocation *theHypocentreLocationObject = parameterToLocation(theHypocentreLocation);
	if (!theHypocentreLocationObject) {
		rCritical(threadID) << "Error: The location response" << theHypocentreLocation->objectName() << "does not include any location object.";
		return -1;
	}
	
	double lat = asin(theta1 * (sin(northLat) - sin(southLat)) + sin(southLat)) / CC;
	double lng = westLng + deltaLng * theta2;
	double depth = -1.0 * theDepth.data()->getCurrentValue(threadID);

	theHypocentreLocationObject->setLatitude(lat,threadID);
	theHypocentreLocationObject->setLongitude(lng,threadID);
	theHypocentreLocationObject->setAltitude(depth,threadID);

	if (getDisplayOutput()) {
		rDebug(threadID) << qPrintable(QString("%1\t%2").arg(theHypocentreLocationObject->getLongitude(threadID), 0, 'g', 12).arg(theHypocentreLocationObject->getLatitude(threadID), 0, 'g', 12));
	}

	if (threadID == 0){
		theHypocentreLocationObject->redisplayonMap();
	}

	if (evaluateDDM) {
		double dLat_dTheta1 = (sin(northLat) - sin(southLat)) / CC / sqrt(1.0 - gsl_pow_2(theta1 * (sin(northLat) - sin(southLat)) + sin(southLat)));
		double dLat_dTheta2 = 0.0;
		double dLng_dTheta1 = 0.0;
		double dLng_dTheta2 = deltaLng;

		double dAlt_dDepth = -1.0;

		QMap<RParameter *, double> theLatGradientMap;
		theLatGradientMap.insertMulti(theTheta1.data(), dLat_dTheta1);
		theLatGradientMap.insertMulti(theTheta2.data(), dLat_dTheta2);
		theDDMMap.insertMulti(theHypocentreLocation, theLatGradientMap);

		QMap<RParameter *, double> theLngGradientMap;
		theLngGradientMap.insertMulti(theTheta1.data(), dLng_dTheta1);
		theLngGradientMap.insertMulti(theTheta2.data(), dLng_dTheta2);
		theDDMMap.insertMulti(theHypocentreLocation, theLngGradientMap);

		QMap<RParameter *, double> theAltGradientMap;
		theAltGradientMap.insertMulti(theDepth.data(), dAlt_dDepth);
		theDDMMap.insertMulti(theHypocentreLocation, theAltGradientMap);

		/*// Finite difference check
		{
			theta1 *= 1.001;
			double newlat = asin(theta1 * (sin(northLat) - sin(southLat)) + sin(southLat)) / CC;
			double newlng = westLng + deltaLng * theta2;
			double dLat_dTheta1DF = (newlat - lat) / (0.001 * theta1);
			double dLng_dTheta1DF = (newlng - lng) / (0.001 * theta1);
			theta1 /= 1.001;
		}
		{
			theta2 *= 1.001;
			double newlat = asin(theta1 * (sin(northLat) - sin(southLat)) + sin(southLat)) / CC;
			double newlng = westLng + deltaLng * theta2;
			double dLat_dTheta2DF = (newlat - lat) / (0.001 * theta2);
			double dLng_dTheta2DF = (newlng - lng) / (0.001 * theta2);
			theta2 /= 1.001;
		}*/
	}

	return 1;
}	

QStringList RRectangularAreaSourceModel::getActionList()
{
	QStringList actionList;
	actionList << "&Display on Map" << "&Remove from Map";
	return actionList;
}

void RRectangularAreaSourceModel::displayonMap()
{


	if (!thePoint1Location || !thePoint2Location) {
		rCritical() << "Warning: Please enter the location objects in the model" << objectName() << ".";
		return;
	}
	RMainWindow *theMainWindow = qobject_cast<RMainWindow *>(theDomain->parent());
	RMapWidget *theMapWidget = theMainWindow->findChild<RMapWidget *>();
	theMainWindow->setActiveSubWindow(theMapWidget);
	RLocation *thePoint1LocationObject = parameterToLocation(thePoint1Location.data());
	RLocation *thePoint3LocationObject = parameterToLocation(thePoint2Location.data());
	if (!theMapWidget) {
		thePoint1LocationObject->openMap();
		return;
	}

	QList<RLocation *> locationList;

	locationList << thePoint1LocationObject;

	RLocation thePoint2LocationObject(0, "Point2");
	thePoint2LocationObject.setLatitude(thePoint1LocationObject->getLatitude());
	thePoint2LocationObject.setLongitude(thePoint3LocationObject->getLongitude());
	locationList << &thePoint2LocationObject;

	locationList << parameterToLocation(thePoint3LocationObject);

	RLocation thePoint4LocationObject(0, "Point4");
	thePoint4LocationObject.setLatitude(thePoint3LocationObject->getLatitude());
	thePoint4LocationObject.setLongitude(thePoint1LocationObject->getLongitude());
	locationList << &thePoint4LocationObject;

	theMapWidget->drawPolygon(this, locationList, Qt::red);


	#ifndef WEB_PROTOTYPE
if (!thePoint1Location || !thePoint2Location) {
		rCritical() << "Warning: Please enter the location objects in the model" << objectName() << ".";
		return;
	}
	RMainWindow *theMainWindow = qobject_cast<RMainWindow *>(theDomain->parent());
	RMapWidget *theMapWidget = theMainWindow->findChild<RMapWidget *>();
	theMainWindow->setActiveSubWindow(theMapWidget);
	RLocation *thePoint1LocationObject = parameterToLocation(thePoint1Location.data());
	RLocation *thePoint3LocationObject = parameterToLocation(thePoint2Location.data());
	if (!theMapWidget) {
		thePoint1LocationObject->openMap();
		return;
	}

	QList<RLocation *> locationList;
	
	locationList << thePoint1LocationObject;

	RLocation thePoint2LocationObject(0, "Point2");
	thePoint2LocationObject.setLatitude(thePoint1LocationObject->getLatitude());
	thePoint2LocationObject.setLongitude(thePoint3LocationObject->getLongitude());
	locationList << &thePoint2LocationObject;

	locationList << parameterToLocation(thePoint3LocationObject);

	RLocation thePoint4LocationObject(0, "Point4");
	thePoint4LocationObject.setLatitude(thePoint3LocationObject->getLatitude());
	thePoint4LocationObject.setLongitude(thePoint1LocationObject->getLongitude());
	locationList << &thePoint4LocationObject;

	theMapWidget->drawPolygon(this, locationList, Qt::red);
#endif // WEB_PROTOTYPE
}

void RRectangularAreaSourceModel::removefromMap()
{


	RMainWindow *theMainWindow = qobject_cast<RMainWindow *>(theDomain->parent());
	RMapWidget *theMapWidget = theMainWindow->findChild<RMapWidget *>();
	if (theMapWidget) {
		theMainWindow->setActiveSubWindow(theMapWidget);
		theMapWidget->removePolygon(this);
	}

#ifndef WEB_PROTOTYPE
	RMainWindow *theMainWindow = qobject_cast<RMainWindow *>(theDomain->parent());
	RMapWidget *theMapWidget = theMainWindow->findChild<RMapWidget *>();
	if (theMapWidget) {
		theMainWindow->setActiveSubWindow(theMapWidget);
		theMapWidget->removePolygon(this);
	}
#endif // WEB_PROTOTYPE
}

bool RRectangularAreaSourceModel::canBeRunParallelly(){
	return true;
}