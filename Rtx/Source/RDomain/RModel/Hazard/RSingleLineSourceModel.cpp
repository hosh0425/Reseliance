#include "RSingleLineSourceModel.h"	

#include "RLocation.h"
#include "RParameter.h"	
#include "RResponse.h"	
#include "RLocationResponse.h"
#include "RMainWindow.h"
#include "RMapWidget.h"

#include <math.h>

RSingleLineSourceModel::RSingleLineSourceModel(QObject *parent, QString name)	
	: RModel(parent, name)
{	
	// Initializing the pointer data members
	/* //$$ 
	thePoint1Location = 0;
	thePoint2Location = 0;
	theTheta = 0;
	theDepth = 0;
	theLatitudeGeometricalUncertainty = 0;
	theLongitudeGeometricalUncertainty = 0;
	*/
	
	// Instantiating the response object
	theDomain->createObject(objectName() + "Response", "RLocationResponse");
	theHypocentreLocation = qobject_cast<RLocationResponse *>(theDomain->getLastAddedObject());
	theHypocentreLocation->setModel(this);
}	

RSingleLineSourceModel::~RSingleLineSourceModel()	
{	
	
}	

QObject * RSingleLineSourceModel::getPoint1Location() const	
{	
	return thePoint1Location.data();
}	

void RSingleLineSourceModel::setPoint1Location(QObject *value)	
{	
	thePoint1Location = qobject_cast<RParameter *>(value);
}	

QObject * RSingleLineSourceModel::getPoint2Location() const	
{	
	return thePoint2Location.data();
}	

void RSingleLineSourceModel::setPoint2Location(QObject *value)	
{	
	thePoint2Location = qobject_cast<RParameter *>(value);
}	

QObject * RSingleLineSourceModel::getTheta() const	
{	
	return theTheta.data();
}	

QObject * RSingleLineSourceModel::getDepth() const	
{	
	return theDepth.data();
}	

QObject * RSingleLineSourceModel::getLatitudeGeometricalUncertainty() const	
{	
	return theLatitudeGeometricalUncertainty.data();
}	
	
void RSingleLineSourceModel::setLatitudeGeometricalUncertainty(QObject *value)	
{	
	theLatitudeGeometricalUncertainty = qobject_cast<RParameter *>(value);
}	
	
QObject * RSingleLineSourceModel::getLongitudeGeometricalUncertainty() const	
{	
	return theLongitudeGeometricalUncertainty.data();
}	
	
void RSingleLineSourceModel::setLongitudeGeometricalUncertainty(QObject *value)	
{	
	theLongitudeGeometricalUncertainty = qobject_cast<RParameter *>(value);
}

void RSingleLineSourceModel::setTheta(QObject *value)	
{	
	theTheta = qobject_cast<RParameter *>(value);

	/*// Check that the user has given a uniform random variable between 0 and 1:
	RContinuousRandomVariable *theRV = qobject_cast<RContinuousRandomVariable *>(value);  
	if (!theRV) {
		rCritical() << "Warning: The parameter given to the Line Source Model must be a continuous random variable.";
		return;
	}

	QString type = theRV->getDistributionType();
	double a = theRV->getParameter1Value();
	double b = theRV->getParameter2Value();
	if ((type != QString("Uniform (a, b)")) || (a != 0.0) || (b != 1.0)) {
		rCritical() << "Warning: The random variable given to the Line Source Model must be uniformly distributed between 0 and 1.";
	}*/
}	

void RSingleLineSourceModel::setDepth(QObject *value)	
{	
	theDepth = qobject_cast<RParameter *>(value);
}	

QList<RParameter *> RSingleLineSourceModel::getPhysicalParameterList()
{
	QList<RParameter *> paramList;
	paramList << theTheta.data() << theDepth.data();
	return paramList;
}

int RSingleLineSourceModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)	
{	
	RLocation *thePoint1LocationObject = parameterToLocation(thePoint1Location.data());
	RLocation *thePoint2LocationObject = parameterToLocation(thePoint2Location.data());

	double theta = theTheta.data()->getCurrentValue(threadID);

	// Getting the pointer to the RLocation object from the RLocationResponse object (theHypocentreLocation)
	RLocation *theHypocentreLocationObject = qobject_cast<RLocation *>(theHypocentreLocation->getLocation());
	if (!theHypocentreLocationObject) {
		rCritical(threadID) << "Error: The location response" << theHypocentreLocation->objectName() << "does not include any location object.";
		return -1;
	}
	
	
	// Implementation assuming that the earth is flat!
	/*double long1 = thePoint1LocationObject->getLongitude();
	double lat1 = thePoint1LocationObject->getLatitude();
		
	double long2 = thePoint2LocationObject->getLongitude();
	double lat2 = thePoint2LocationObject->getLatitude();

	double delta_long = long2-long1;
	double delta_lat = lat2-lat1;

	double locationLat = lat1 + delta_lat * theta;
	double locationLng = long1 + delta_long * theta;*/



	// Implementation assuming that the earth is spherical
	thePoint1LocationObject->computeIntermediatePoint(thePoint2LocationObject, theHypocentreLocationObject, theta,threadID);
	
	// Adding the uncertainty of the geometry of the fault to the latitude and longitude
	theHypocentreLocationObject->setLatitude(theHypocentreLocationObject->getLatitude(threadID) + theLatitudeGeometricalUncertainty.data()->getCurrentValue(threadID), threadID);
	theHypocentreLocationObject->setLongitude(theHypocentreLocationObject->getLongitude(threadID) + theLongitudeGeometricalUncertainty.data()->getCurrentValue(threadID), threadID);

	theHypocentreLocationObject->setAltitude(-1.0 * theDepth.data()->getCurrentValue(threadID),threadID);

	if (threadID == 0){
		theHypocentreLocationObject->redisplayonMap();
	}

	if (evaluateDDM) {
		double dLat_dTheta;
		double dLat_dDepth;
		double dLat_dLatitudeGeometricalUncertainty;
		double dLat_dLongitudeGeometricalUncertainty;
		double dLng_dTheta;
		double dLng_dDepth;
		double dLng_dLatitudeGeometricalUncertainty;
		double dLng_dLongitudeGeometricalUncertainty;
		double dAlt_dTheta;
		double dAlt_dDepth;
		double dAlt_dLatitudeGeometricalUncertainty;
		double dAlt_dLongitudeGeometricalUncertainty;

		// Implementation assuming that the earth is flat!
		/*dLat_dTheta = delta_lat;
		dLng_dTheta = delta_long;*/

		// Implementation assuming that the earth is spherical
		thePoint1LocationObject->computeIntermediatePointGradient(thePoint2LocationObject, theta, &dLat_dTheta, &dLng_dTheta,threadID);



		dLat_dDepth = 0.0;
		dLng_dDepth = 0.0;
		dAlt_dTheta = 0.0;
		dAlt_dDepth = -1.0;

		dLat_dLatitudeGeometricalUncertainty = 1.0;
		dLat_dLongitudeGeometricalUncertainty = 0.0;
		dLng_dLatitudeGeometricalUncertainty = 0.0;
		dLng_dLongitudeGeometricalUncertainty = 1.0;
		dAlt_dLatitudeGeometricalUncertainty = 0.0;
		dAlt_dLongitudeGeometricalUncertainty = 0.0;


		QMap<RParameter *, double> theLatGradientMap;
		theLatGradientMap.insertMulti(theTheta.data(), dLat_dTheta);
		theLatGradientMap.insertMulti(theDepth.data(), dLat_dDepth);
		theLatGradientMap.insertMulti(theLatitudeGeometricalUncertainty.data(), dLat_dLatitudeGeometricalUncertainty);
		theLatGradientMap.insertMulti(theLongitudeGeometricalUncertainty.data(), dLat_dLongitudeGeometricalUncertainty);
		theDDMMap.insertMulti(theHypocentreLocation, theLatGradientMap);

		QMap<RParameter *, double> theLngGradientMap;
		theLngGradientMap.insertMulti(theTheta.data(), dLng_dTheta);
		theLngGradientMap.insertMulti(theDepth.data(), dLng_dDepth);
		theLngGradientMap.insertMulti(theLatitudeGeometricalUncertainty.data(), dLng_dLatitudeGeometricalUncertainty);
		theLngGradientMap.insertMulti(theLongitudeGeometricalUncertainty.data(), dLng_dLongitudeGeometricalUncertainty);
		theDDMMap.insertMulti(theHypocentreLocation, theLngGradientMap);

		QMap<RParameter *, double> theAltGradientMap;
		theAltGradientMap.insertMulti(theTheta.data(), dAlt_dTheta);
		theAltGradientMap.insertMulti(theDepth.data(), dAlt_dDepth);
		theAltGradientMap.insertMulti(theLatitudeGeometricalUncertainty.data(), dAlt_dLatitudeGeometricalUncertainty);
		theAltGradientMap.insertMulti(theLongitudeGeometricalUncertainty.data(), dAlt_dLongitudeGeometricalUncertainty); 
		theDDMMap.insertMulti(theHypocentreLocation, theAltGradientMap);
	}

	return 1;
}	

QStringList RSingleLineSourceModel::getActionList()
{
	QStringList actionList;
	actionList << "&Display on Map" << "&Remove from Map";
	return actionList;
}

void RSingleLineSourceModel::displayonMap()
{
	QList<RLocation *> locationList;
	locationList << parameterToLocation(thePoint1Location.data());
	locationList << parameterToLocation(thePoint2Location.data());
	if (!locationList[0] || !locationList[1]) {
		rCritical() << "Warning: Please enter the 2 location objects in the model" << objectName() << ".";
		return;
	}
	RMainWindow *theMainWindow = qobject_cast<RMainWindow *>(theDomain->parent());
	RMapWidget *theMapWidget = theMainWindow->findChild<RMapWidget *>();
	theMainWindow->setActiveSubWindow(theMapWidget);
	if (!theMapWidget) {
		locationList[0]->openMap();
		return;
	}
	theMapWidget->drawPolyline(this, locationList, Qt::black);
}

void RSingleLineSourceModel::removefromMap()
{
	RMainWindow *theMainWindow = qobject_cast<RMainWindow *>(theDomain->parent());
	RMapWidget *theMapWidget = theMainWindow->findChild<RMapWidget *>();
	if (theMapWidget) {
		theMainWindow->setActiveSubWindow(theMapWidget);
		theMapWidget->removePolyline(this);
	}
}

bool RSingleLineSourceModel::canBeRunParallelly(){
	return true;
}