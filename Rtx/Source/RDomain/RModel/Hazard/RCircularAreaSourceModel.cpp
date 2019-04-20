#include "RCircularAreaSourceModel.h"	

#include "RParameter.h"	
#include "RLocationResponse.h"	
#include "RContinuousRandomVariable.h"
#include "RLocation.h"
#include "RMainWindow.h"
#include "RMapWidget.h"

#include <gsl/gsl_math.h>

#include <math.h>

RCircularAreaSourceModel::RCircularAreaSourceModel(QObject *parent, QString name)	
	: RModel(parent, name)
{	
	// Initializing the pointer data members
	//$$ theCentreLocation = 0;
	theMinimumRadius = 0.0;
	theMaximumRadius = 0.0;
	//$$ theTheta1 = 0;
	//$$ theTheta2 = 0;
	//$$ theDepth = 0;
	
	// Instantiating the response object
	theDomain->createObject(objectName() + "Response", "RLocationResponse");
	theHypocentreLocation = qobject_cast<RLocationResponse *>(theDomain->getLastAddedObject());
	theHypocentreLocation->setModel(this);
}	

RCircularAreaSourceModel::~RCircularAreaSourceModel()	
{	
	
}	

QObject * RCircularAreaSourceModel::getCentreLocation() const	
{	
	return theCentreLocation.data();
}	

void RCircularAreaSourceModel::setCentreLocation(QObject *value)	
{	
	theCentreLocation = qobject_cast<RParameter *>(value);

	// Verification that the user has given an RLocationResponse or an RLocation
	parameterToLocation(theCentreLocation.data()); 
}	

double RCircularAreaSourceModel::getMinimumRadius() const	
{	
	return theMinimumRadius;
}	

void RCircularAreaSourceModel::setMinimumRadius(double value)	
{	
	theMinimumRadius = value;
}	

double RCircularAreaSourceModel::getMaximumRadius() const	
{	
	return theMaximumRadius;
}	

void RCircularAreaSourceModel::setMaximumRadius(double value)	
{	
	theMaximumRadius = value;
}	

QObject * RCircularAreaSourceModel::getTheta1() const	
{	
	return theTheta1.data();
}	

void RCircularAreaSourceModel::setTheta1(QObject *value)	
{	
	theTheta1 = qobject_cast<RParameter *>(value);
}	

QObject * RCircularAreaSourceModel::getTheta2() const	
{	
	return theTheta2.data();
}	

void RCircularAreaSourceModel::setTheta2(QObject *value)	
{	
	theTheta2 = qobject_cast<RParameter *>(value);
}	

QObject * RCircularAreaSourceModel::getDepth() const	
{	
	return theDepth.data();
}	

void RCircularAreaSourceModel::setDepth(QObject *value)	
{	
	theDepth = qobject_cast<RParameter *>(value);
}

QList<RParameter *> RCircularAreaSourceModel::getPhysicalParameterList()
{
	QList<RParameter *> paramList;
	paramList << theTheta1.data() << theTheta2.data() << theDepth.data();
	return paramList;
}

int RCircularAreaSourceModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)	
{	
	RLocation *theCentreLocationObject = parameterToLocation(theCentreLocation.data());
	
	double longCentre = theCentreLocationObject->getLongitude(threadID);
	double latCentre = theCentreLocationObject->getLatitude(threadID);
		
	// Getting the pointer to the RLocation object from the RLocationResponse object (theHypocentreLocation)
	RLocation *theHypocentreLocationObject = parameterToLocation(theHypocentreLocation);
	if (!theHypocentreLocationObject) {
		rCritical(threadID) << "Error: The location response" << theHypocentreLocation->objectName() << "does not include any location object.";
		return -1;
	}
	
	double theta1 = theTheta1.data()->getCurrentValue(threadID);
	double theta2 = theTheta2.data()->getCurrentValue(threadID);

	// Implementation assuming a spherical earth
	double temp = cos(theMaximumRadius / EarthRadius) - cos(theMinimumRadius / EarthRadius);
	double distance = EarthRadius * acos(theta1 * temp + cos(theMinimumRadius / EarthRadius));
	double bearing = 2 * M_PI * theta2;

	theCentreLocationObject->computeDestination(theHypocentreLocationObject, distance, bearing,threadID);

	//if (theDisplayOutput) {
	//	rDebug() << qPrintable(QString("%1\t%2").arg(theHypocentreLocationObject->getLongitude(), 0, 'g', 12).arg(theHypocentreLocationObject->getLatitude(), 0, 'g', 12));
	//}

	double depth = theDepth.data()->getCurrentValue(threadID);
	theHypocentreLocationObject->setAltitude(-1.0 * depth,threadID);

	if (threadID == 0){
		theHypocentreLocationObject->redisplayonMap();
	}

	if (evaluateDDM) {
		double dLat_dDistance;
		double dLat_dBearing;
		double dLng_dDistance;
		double dLng_dBearing;
		theCentreLocationObject->computeDestinationGradient(distance, bearing, &dLat_dDistance, &dLat_dBearing, &dLng_dDistance, &dLng_dBearing,threadID);

		double dDistance_dTheta1 = -EarthRadius * temp / sqrt(1.0 - gsl_pow_2(theta1 * temp + cos(theMinimumRadius / EarthRadius)));
		double dBearing_dTheta2 = 2 * M_PI;

		double dLat_dTheta1 = dLat_dDistance * dDistance_dTheta1;
		double dLat_dTheta2 = dLat_dBearing * dBearing_dTheta2;
		double dLng_dTheta1 = dLng_dDistance * dDistance_dTheta1;
		double dLng_dTheta2 = dLng_dBearing * dBearing_dTheta2;

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

		// Finite difference check
		/*double lat = theHypocentreLocationObject->getLatitude();
		double lng = theHypocentreLocationObject->getLongitude();
		{
			theta1 *= 1.001;
			double newdistance = EarthRadius * acos(theta1 * temp + cos(theMinimumRadius / EarthRadius));
			theCentreLocationObject->computeDestination(theHypocentreLocationObject, newdistance, newbearing);
			double dLat_dTheta1DF = (theHypocentreLocationObject->getLatitude() - lat) / (0.001 * theta1);
			double dLng_dTheta1DF = (theHypocentreLocationObject->getLongitude() - lng) / (0.001 * theta1);
			theta1 /= 1.001;
		}
		{
			theta2 *= 1.001;
			double newbearing = 2 * M_PI * theta2;
			theCentreLocationObject->computeDestination(theHypocentreLocationObject, newdistance, newbearing);
			double dLat_dTheta2DF = (theHypocentreLocationObject->getLatitude() - lat) / (0.001 * theta2);
			double dLng_dTheta2DF = (theHypocentreLocationObject->getLongitude() - lng) / (0.001 * theta2);
			theta2 /= 1.001;
		}*/
	}

	return 1;
}	

QStringList RCircularAreaSourceModel::getActionList()
{
	QStringList actionList;
	actionList << "&Display on Map" << "&Remove from Map";
	return actionList;
}

void RCircularAreaSourceModel::displayonMap()
{
	if (!theCentreLocation || (theMaximumRadius == 0.0)) {
		rCritical() << "Warning: Please enter the centre location object and the maximum radius in the model" << objectName() << ".";
		return;
	}
	RMainWindow *theMainWindow = qobject_cast<RMainWindow *>(theDomain->parent());
	RMapWidget *theMapWidget = theMainWindow->findChild<RMapWidget *>();
	theMainWindow->setActiveSubWindow(theMapWidget);
	RLocation *theCentreLocationObject = parameterToLocation(theCentreLocation.data());
	if (!theMapWidget) {
		theCentreLocationObject->openMap();
		return;
	}

	QList<RPointer<RLocation> > locationList;
	int pointCount = 50;

	for (int i = 0; i < pointCount; i++) {
		RLocation *point = new RLocation(0, QString("Point%1").arg(i));
		theCentreLocationObject->computeDestination(point, theMaximumRadius, 2 * M_PI * i / pointCount,0);
		locationList << point;
	}

	if (theMinimumRadius > 0) {
		locationList << locationList.first();

		int newPointCount = pointCount;// * theMinimumRadius / theMaximumRadius;
		for (int i = 0; i <= newPointCount; i++) {
			RLocation *point = new RLocation(0, QString("Point%1").arg(pointCount + i));
			theCentreLocationObject->computeDestination(point, theMinimumRadius, -2 * M_PI * i / newPointCount,0);
			locationList << point;
		}		
		//locationList << locationList[pointCount];
	}

	QList<RLocation *> tempLocationList;
	for (int i = 0; i < locationList.count(); i++) {
		tempLocationList << locationList[i].data();
	}

	theMapWidget->drawPolygon(this, tempLocationList, Qt::red);

	for (int i = 0; i < locationList.count(); i++) {
		if (locationList[i]) {
			delete locationList[i].data();
//$$ 			locationList[i] = 0;
		}
	}
}

void RCircularAreaSourceModel::removefromMap()
{
	RMainWindow *theMainWindow = qobject_cast<RMainWindow *>(theDomain->parent());
	RMapWidget *theMapWidget = theMainWindow->findChild<RMapWidget *>();
	if (theMapWidget) {
		theMainWindow->setActiveSubWindow(theMapWidget);
		theMapWidget->removePolygon(this);
	}
}

bool RCircularAreaSourceModel::canBeRunParallelly(){
	return true;
}