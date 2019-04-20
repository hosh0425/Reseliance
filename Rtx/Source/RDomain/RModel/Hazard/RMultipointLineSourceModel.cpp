#include "RMultipointLineSourceModel.h"	

#include "RLocation.h"
#include "RParameter.h"	
#include "RResponse.h"	
#include "RLocationResponse.h"
#include "RMainWindow.h"
#include "RMapWidget.h"

#include <math.h>

#include <qmutex.h>
#include <QMutexLocker>

RMultipointLineSourceModel::RMultipointLineSourceModel(QObject *parent, QString name)	
	: RModel(parent, name)
{	
	// Initializing the pointer data members
	/* //$$ 
	theTheta = 0;
	theDepth = 0;
	theLatitudeGeometricalUncertainty = 0;
	theLongitudeGeometricalUncertainty = 0;
	*/

	theFaultLength = 0.0;
	
	// Instantiating the response object
	theDomain->createObject(objectName() + "Response", "RLocationResponse");
	theEpicentreLocation = qobject_cast<RLocationResponse *>(theDomain->getLastAddedObject());
	theEpicentreLocation->setModel(this);

	faultLengthLock = new QMutex();
}	

RMultipointLineSourceModel::~RMultipointLineSourceModel()	
{	
	delete faultLengthLock;
}	

QString RMultipointLineSourceModel::getPointLocationList()
{
	return parameterListToString(thePointLocationList);
}

void RMultipointLineSourceModel::setPointLocationList(QString value)
{
	thePointLocationList = stringToParameterList(value);
	
	// Checking if all the passed parameters are either RLocation or RLocationResponse
	parameterListToLocationList(thePointLocationList, true);
}

QObject * RMultipointLineSourceModel::getTheta() const	
{	
	return theTheta.data();
}	

QObject * RMultipointLineSourceModel::getDepth() const	
{	
	return theDepth.data();
}	

QObject * RMultipointLineSourceModel::getLatitudeGeometricalUncertainty() const	
{	
	return theLatitudeGeometricalUncertainty.data();
}	
	
void RMultipointLineSourceModel::setLatitudeGeometricalUncertainty(QObject *value)	
{	
	theLatitudeGeometricalUncertainty = qobject_cast<RParameter *>(value);
}	
	
QObject * RMultipointLineSourceModel::getLongitudeGeometricalUncertainty() const	
{	
	return theLongitudeGeometricalUncertainty.data();
}	
	
void RMultipointLineSourceModel::setLongitudeGeometricalUncertainty(QObject *value)	
{	
	theLongitudeGeometricalUncertainty = qobject_cast<RParameter *>(value);
}

void RMultipointLineSourceModel::setTheta(QObject *value)	
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

void RMultipointLineSourceModel::setDepth(QObject *value)	
{	
	theDepth = qobject_cast<RParameter *>(value);
}	

QList<RParameter *> RMultipointLineSourceModel::getPhysicalParameterList()
{
	QList<RParameter *> paramList;
	paramList << theTheta.data() << theDepth.data();
	return paramList;
}

void RMultipointLineSourceModel::reset()
{
	theFaultLength = 0;
	thePieceLengthVector.clear();
}

int RMultipointLineSourceModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)	
{	
	if (thePointLocationList.count() < 2) {
		rCritical(threadID) << "Error in" << objectName() << ": The number of point locations should be at least 2.";
		return -1;
	}

	// Converting the list of parameters to the list of locations
	QList<RLocation *> theLocationList = parameterListToLocationList(thePointLocationList);
	
	// start of safe area scope
	{
		QMutexLocker locker(faultLengthLock);

		if (theFaultLength == 0.0) {
			for (int i = 0; i < theLocationList.count() - 1; i++) {
				double pieceLength = theLocationList[i]->computeSurfaceDistance(theLocationList[i+1],threadID);
				thePieceLengthVector << pieceLength;
				theFaultLength += pieceLength;
			}
		}
	}
	// end of safe area scope

	double lengthRealization = theTheta.data()->getCurrentValue(threadID) * theFaultLength;

	// Getting the pointer to the RLocation object from the RLocationResponse object (theEpicentreLocation)
	RLocation *theHypocentreLocationObject = qobject_cast<RLocation *>(theEpicentreLocation->getLocation());
	if (!theHypocentreLocationObject) {
		rCritical(threadID) << "Error: The location response" << theEpicentreLocation->objectName() << "does not include any location object.";
		return -1;
	}

	double theFaultLengthSoFar = 0;
	for (int i = 0; i < thePieceLengthVector.count(); i++) {
		double oldFaultLength = theFaultLengthSoFar;
		theFaultLengthSoFar += thePieceLengthVector[i];
		if ((theFaultLengthSoFar >= lengthRealization) && (lengthRealization > oldFaultLength)) {
			double lengthRatio = (lengthRealization - oldFaultLength) / thePieceLengthVector[i];
			
			// Implementation assuming that the earth is flat!
			/*double lng1 = theLocationList[i]->getLongitude();
			double lat1 = theLocationList[i]->getLatitude();
			double lng2 = theLocationList[i+1]->getLongitude();
			double lat2 = theLocationList[i+1]->getLatitude();
			double deltaLng = lng2 - lng1;
			double deltaLat = lat2 - lat1;
			double locationLat = lat1 + deltaLat * lengthRatio;
			double locationLng = lng1 + deltaLng * lengthRatio;
			theHypocentreLocationObject->setLatitude(locationLat);
			theHypocentreLocationObject->setLongitude(locationLng);*/

			// Implementation assuming that the earth is spherical
			theLocationList[i]->computeIntermediatePoint(theLocationList[i+1], theHypocentreLocationObject, lengthRatio,threadID);

			break;
		}
	}

	double locationAlt = -1.0 * theDepth.data()->getCurrentValue(threadID);
	theHypocentreLocationObject->setAltitude(locationAlt,threadID);

	// Adding the uncertainty of the geometry of the fault to the latitude and longitude
	theHypocentreLocationObject->setLatitude(theHypocentreLocationObject->getLatitude(threadID) + theLatitudeGeometricalUncertainty.data()->getCurrentValue(threadID), threadID);
	theHypocentreLocationObject->setLongitude(theHypocentreLocationObject->getLongitude(threadID) + theLongitudeGeometricalUncertainty.data()->getCurrentValue(threadID), threadID);
 	
	

	if (getDisplayOutput()) {
		rDebug(threadID) << qPrintable(QString("%1\t%2").arg(theHypocentreLocationObject->getLongitude(threadID), 0, 'g', 12).arg(theHypocentreLocationObject->getLatitude(threadID), 0, 'g', 12));
	}

	if (threadID == 0){
		theHypocentreLocationObject->redisplayonMap();
	}

	return 0;
}	

QStringList RMultipointLineSourceModel::getActionList()
{
	QStringList actionList;
	actionList << "&Display on Map" << "&Remove from Map";
	return actionList;
}

void RMultipointLineSourceModel::displayonMap()
{
	QList<RLocation *> locationList = parameterListToLocationList(thePointLocationList);
	if (locationList.count() < 2) {
		rCritical() << "Warning: Please enter at least 2 location objects in the model" << objectName() << ".";
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

void RMultipointLineSourceModel::removefromMap()
{
	RMainWindow *theMainWindow = qobject_cast<RMainWindow *>(theDomain->parent());
	RMapWidget *theMapWidget = theMainWindow->findChild<RMapWidget *>();
	if (theMapWidget) {
		theMainWindow->setActiveSubWindow(theMapWidget);
		theMapWidget->removePolyline(this);
	}
}

bool RMultipointLineSourceModel::canBeRunParallelly(){
	return true;
}