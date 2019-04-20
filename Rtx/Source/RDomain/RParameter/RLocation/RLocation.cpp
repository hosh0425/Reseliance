#include "RLocation.h"

#include "RMainWindow.h"
#include "RMapWidget.h"

#include <QMessageBox>
#include <QNetworkReply>

#include <gsl/gsl_math.h>

RLocation::RLocation(QObject *parent, QString name)
	: RParameter(parent, name)
{
	int arraySizeForParallelizedObject = getArraySizeForParallelizedObject();

	theLatitude = new double[arraySizeForParallelizedObject];
	theLongitude = new double[arraySizeForParallelizedObject];
	theAltitude = new double[arraySizeForParallelizedObject];

	theLatitude[0] = 0.0;
	theLongitude[0] = 0.0;
	theAltitude[0] = 0.0;

	if (parent) {
		theMainWindow = qobject_cast<RMainWindow *>(parent->parent());
	}
}

RLocation::~RLocation()
{
	delete [] theLatitude;
	delete [] theAltitude;
	delete [] theLongitude;
}

double RLocation::getLatitude(int threadID) const
{
	return theLatitude[threadID];
}

double RLocation::getLongitude(int threadID) const
{
	return theLongitude[threadID];
}

double RLocation::getAltitude(int threadID) const
{
	return theAltitude[threadID];
}

QString RLocation::getAddress() const
{
	return theAddress;
}


void RLocation::setLatitude(double value)
{
	theLatitude[0] = value;
}

void RLocation::setLongitude(double value)
{
	theLongitude[0] = value;
}

void RLocation::setAltitude(double value)
{
	theAltitude[0] = value;
}

void RLocation::setAddress(QString value)
{
	theAddress = value;
}


void RLocation::setLatitude(double value,int threadID)
{
	theLatitude[threadID] = value;
}

void RLocation::setLongitude(double value, int threadID)
{
	theLongitude[threadID] = value;
}

void RLocation::setAltitude(double value, int threadID)
{
	theAltitude[threadID] = value;
}


double RLocation::computeSurfaceDistance(const RLocation *destination, int threadID)
{
	double lat1 = degreeToRadian(theLatitude[threadID]);
	double lng1 = degreeToRadian(theLongitude[threadID]);

	double lat2 = degreeToRadian(destination->getLatitude(threadID));
	double lng2 = degreeToRadian(destination->getLongitude(threadID));
	
	double distance = acos(sin(lat1) * sin(lat2) + cos(lat1) * cos(lat2) * cos(lng1 - lng2)) * EarthRadius;
	return distance;
}

void RLocation::computeSurfaceDistanceGradient(const RLocation *destination, double *dDistance_dLat, double *dDistance_dLng, int threadID)
{
	double lat1 = degreeToRadian(theLatitude[threadID]);
	double lng1 = degreeToRadian(theLongitude[threadID]);

	double lat2 = degreeToRadian(destination->getLatitude(threadID));
	double lng2 = degreeToRadian(destination->getLongitude(threadID));

	double AA = sin(lat1) * sin(lat2) + cos(lat1) * cos(lat2) * cos(lng1 - lng2);
	
	double dDistance_dLat1 = -EarthRadius * (cos(lat1) * sin(lat2) - sin(lat1) * cos(lat2) * cos(lng1 - lng2)) / sqrt(1 - AA * AA);
	double dDistance_dLng1 = EarthRadius * (cos(lat1) * cos(lat2) * sin(lng1 - lng2)) / sqrt(1 - AA * AA);

	*dDistance_dLat = degreeToRadian(dDistance_dLat1);
	*dDistance_dLng = degreeToRadian(dDistance_dLng1);



		
	/*// Finite difference to check
	double distance = acos(sin(lat1) * sin(lat2) + cos(lat1) * cos(lat2) * cos(lng1 - lng2)) * EarthRadius;
	double d = 0.001;
	double FDMdLat = degreeToRadian((acos(sin(lat1+d) * sin(lat2) + cos(lat1+d) * cos(lat2) * cos(lng1 - lng2)) * EarthRadius - distance) / d);
	double FDMdLng = degreeToRadian((acos(sin(lat1) * sin(lat2) + cos(lat1) * cos(lat2) * cos(lng1+d - lng2)) * EarthRadius - distance) / d);
	double DDMdLat = *dDistance_dLat;
	double DDMdLng = *dDistance_dLng;*/
}

double RLocation::computeBearing(const RLocation *destination, int threadID)
{
	double lat1 = degreeToRadian(theLatitude[threadID]);
	double lng1 = degreeToRadian(theLongitude[threadID]);

	double lat2 = degreeToRadian(destination->getLatitude(threadID));
	double lng2 = degreeToRadian(destination->getLongitude(threadID));
	
	double bearing = atan2(sin(lng2 - lng1) * cos(lat2), cos(lat1) * sin(lat2) - sin(lat1) * cos(lat2) * cos(lng2 - lng1));
	//bearing += M_PI;

	if (bearing < 0) {
		bearing += 2 * M_PI;
	}

	return bearing;
}

void RLocation::computeDestination(RLocation *destination, double distance, double bearing, int threadID)
{
	double lat1 = degreeToRadian(theLatitude[threadID]);
	double lng1 = degreeToRadian(theLongitude[threadID]);

	double d_R = distance/EarthRadius;

	double lat2 = asin(sin(lat1) * cos(d_R) + cos(lat1) * sin(d_R) * cos(bearing));
	double lng2 = lng1 + atan2(sin(bearing) * sin(d_R) * cos(lat1), cos(d_R) - sin(lat1) * sin(lat2));

	destination->setLatitude(radianToDegree(lat2),threadID);
	destination->setLongitude(radianToDegree(lng2),threadID);
}

void RLocation::computeDestinationGradient(double distance, double bearing, double *dLat_dDistance, double *dLat_dBearing, double *dLng_dDistance, double *dLng_dBearing, int threadID)
{
	double lat1 = degreeToRadian(theLatitude[threadID]);
	double lng1 = degreeToRadian(theLongitude[threadID]);

	double d_R = distance/EarthRadius;

	double AA = sin(lat1) * cos(d_R) + cos(lat1) * sin(d_R) * cos(bearing);
	double dLat2_dDistance = (-sin(lat1) * sin(d_R) + cos(lat1) * cos(d_R) * cos(bearing)) / EarthRadius / sqrt(1 - AA * AA);
	double dLat2_dBearing = -cos(lat1) * sin(d_R) * sin(bearing) / sqrt(1 - AA * AA);

	double BB = sin(bearing) * sin(d_R) * cos(lat1);
	double CC = cos(d_R) - sin(lat1) * AA;
	double dLng2_dDistance = (sin(bearing) * cos(d_R) * cos(lat1) * CC - (-sin(d_R) + sin(d_R) * sin(lat1) * sin(lat1) - cos(lat1) * sin(lat1) * cos(d_R) * cos(bearing)) * BB) / EarthRadius / (BB * BB + CC * CC);
	double dLng2_dBearing = (cos(bearing) * sin(d_R) * cos(lat1) * CC - cos(lat1) * sin(lat1) * sin(d_R) * sin(bearing) * BB) / (BB * BB + CC * CC);

	*dLat_dDistance = radianToDegree(dLat2_dDistance);
	*dLat_dBearing = radianToDegree(dLat2_dBearing);
	*dLng_dDistance = radianToDegree(dLng2_dDistance);
	*dLng_dBearing = radianToDegree(dLng2_dBearing);
}

void RLocation::computeIntermediatePoint(RLocation *endPoint, RLocation *intermediatePoint, double distanceRatioFromStartPoint, int threadID)
{
	double lat1 = degreeToRadian(theLatitude[threadID]);
	double lng1 = degreeToRadian(theLongitude[threadID]);

	double lat2 = degreeToRadian(endPoint->getLatitude(threadID));
	double lng2 = degreeToRadian(endPoint->getLongitude(threadID));

	double d = computeSurfaceDistance(endPoint,threadID) / EarthRadius;
	double f = distanceRatioFromStartPoint;
	double AA = sin((1 - f) * d) / sin(d);
	double BB = sin(f * d) / sin(d);
	double xx = AA * cos(lat1) * cos(lng1)+ BB * cos(lat2) * cos(lng2);
	double yy = AA * cos(lat1) * sin(lng1)+ BB * cos(lat2) * sin(lng2);
	double zz = AA * sin(lat1) + BB * sin(lat2);
	double lat = atan2(zz, sqrt(gsl_pow_2(xx) + gsl_pow_2(yy)));
	double lng = atan2(yy, xx);

	intermediatePoint->setLatitude(radianToDegree(lat),threadID);
	intermediatePoint->setLongitude(radianToDegree(lng),threadID);
}

void RLocation::computeIntermediatePointGradient(RLocation *endPoint, double distanceRatioFromStartPoint ,double *dLat_dDistanceRatio, double *dLng_dDistanceRatio, int threadID)
{
	double lat1 = degreeToRadian(theLatitude[threadID]);
	double lng1 = degreeToRadian(theLongitude[threadID]);

	double lat2 = degreeToRadian(endPoint->getLatitude(threadID));
	double lng2 = degreeToRadian(endPoint->getLongitude(threadID));

	double d = computeSurfaceDistance(endPoint,threadID) / EarthRadius;
	double f = distanceRatioFromStartPoint;
	double AA = sin((1 - f) * d) / sin(d);
	double BB = sin(f * d) / sin(d);
	double xx = AA * cos(lat1) * cos(lng1)+ BB * cos(lat2) * cos(lng2);
	double yy = AA * cos(lat1) * sin(lng1)+ BB * cos(lat2) * sin(lng2);
	double zz = AA * sin(lat1) + BB * sin(lat2);

	double dAA_df = -d * cos((1 - f) * d) / sin(d);
	double dBB_df = d * cos(f * d) / sin(d);
	double dxx_df = cos(lat1) * cos(lng1) * dAA_df + cos(lat2) * cos(lng2) * dBB_df;
	double dyy_df = cos(lat1) * sin(lng1) * dAA_df + cos(lat2) * sin(lng2) * dBB_df;
	double dzz_df = sin(lat1) * dAA_df + sin(lat2) * dBB_df;
	double cc = 1.0 / sqrt(gsl_pow_2(xx) + gsl_pow_2(yy));
	double dLat_df = (-zz * xx *  gsl_pow_3(cc)) / (1 + gsl_pow_2(zz * cc)) * dxx_df + (-zz * yy *  gsl_pow_3(cc)) / (1 + gsl_pow_2(zz * cc)) * dyy_df + cc / (1 + gsl_pow_2(zz * cc)) * dzz_df;
	double dLng_df = (1.0 / xx / (1 + gsl_pow_2(yy / xx))) * dyy_df + (-yy / (xx * xx + yy * yy)) * dxx_df;

	*dLat_dDistanceRatio = radianToDegree(dLat_df);
	*dLng_dDistanceRatio = radianToDegree(dLng_df);
	
	
	/*// Finite difference to check
	double lat = radianToDegree(atan2(zz, sqrt(pow(xx, 2.0) + pow(yy, 2.0))));
	double lng = radianToDegree(atan2(yy, xx));
	double df = 0.01;
	f += df;
	AA = sin((1 - f) * d) / sin(d);
	BB = sin(f * d) / sin(d);
	xx = AA * cos(lat1) * cos(lng1)+ BB * cos(lat2) * cos(lng2);
	yy = AA * cos(lat1) * sin(lng1)+ BB * cos(lat2) * sin(lng2);
	zz = AA * sin(lat1) + BB * sin(lat2);
	double newLat = radianToDegree(atan2(zz, sqrt(pow(xx, 2.0) + pow(yy, 2.0))));
	double newLng = radianToDegree(atan2(yy, xx));
	double FDMdLat = (newLat - lat) / df;
	double FDMdLng = (newLng - lng) / df;

	double DDMdLat = *dLat_dDistanceRatio;
	double DDMdLng = *dLng_dDistanceRatio;*/
}

int RLocation::computeIntersectionWithLatitude(double latitude, RLocation *endPoint, double *intersectionLongitude, int threadID)
{
	/* There will be one or two crossings.

	In Cartesian space we have three equations and three unknowns.
	x^2 + y^2 + z^2 = r^2 : sphere
	ax + by + cz = 0      : plane of the great circle
	z = z[0]              : plane of latitude

	We can simplify by substituting some constant terms.
	Let s = r^2 - z^2    so x^2 + y^2 = s
	and let p = -cz      so ax + by = p

	According to Mathematica

	x = (p - ((b^2 * p)/(a^2 + b^2)) (-+) (ab * sqrt(-p^2 +sa^2 +sb^2)/(a^2+b^2)))/a
	and
	y = (bp (+-) a * sqrt(-p^2 +sa^2 +sb^2)) / (a^2 + b^2)  */

	// The given latitude
	double lat = degreeToRadian(latitude);

	// Coordinates of the points on the great circle
	double lat1 = degreeToRadian(theLatitude[threadID]);
	double lng1 = degreeToRadian(theLongitude[threadID]);
	double lat2 = degreeToRadian(endPoint->getLatitude(threadID));
	double lng2 = degreeToRadian(endPoint->getLongitude(threadID));

	if (!(lat >= qMin(lat1, lat2) && (lat <= qMax(lat1, lat2)))) {
		rCritical(threadID) << "There is no intersection.";
		return -1;
	}

	// Special cases
	if (lat2 == lat1) {
		rCritical(threadID) << "The locations have the exact same latitude.";
		return -1;
	}
	if (lng1 == lng2) {
		*intersectionLongitude = theLongitude[threadID];
		return 0;
	}
	if (lat == lat1) {
		*intersectionLongitude = theLongitude[threadID];
		return 0;
	}
	if (lat == lat2) {
		*intersectionLongitude = endPoint->getLongitude(threadID);
		return 0;
	}

	// Conversion to Cartesian coordinates
	double xx1 = EarthRadius * cos(lng1) * cos(lat1);
	double yy1 = EarthRadius * sin(lng1) * cos(lat1);
	double zz1 = EarthRadius * sin(lat1);
	double xx2 = EarthRadius * cos(lng2) * cos(lat2);
	double yy2 = EarthRadius * sin(lng2) * cos(lat2);
	double zz2 = EarthRadius * sin(lat2);

	// The equation for the plane of the great circle: ax + by + cz = 0 
	double aa = yy1 * zz2 - yy2 * zz1;
	double bb = xx2 * zz1 - xx1 * zz2;
	double cc = xx1 * yy2 - xx2 * yy1;

	// Some constants
	double latZ = EarthRadius * sin(lat);
	double planarConst = -cc * latZ;
	double sphereConst = EarthRadius * EarthRadius - latZ * latZ;
	double scale = aa * aa + bb * bb;
	double rad = sphereConst * aa * aa + sphereConst * bb * bb - planarConst * planarConst;

	/* If this is negative taking the square root would require imaginary numbers.
	It probably means the two planes intersect outside the sphere.
	That could happen if the latitude was above or below the inflection point. */

	if (rad < 0) {
		rCritical(threadID) << "There is no intersection.";
		return -1;
	}
	rad = sqrt(rad);

	double firstX = bb * bb * planarConst / scale;
	double secondX = aa * bb * rad / scale;

	// Coordinates of the intersection
	double xxi = (planarConst - firstX + secondX) / aa; //(planarConst - firstX - secondX) / aa
	double yyi = (bb * planarConst - aa * rad) / scale; //(bb * planarConst + aa * rad) / scale;
	double zzi = latZ;

	// Converting the intersection to lat and lng
	double lati = asin(zzi / EarthRadius);
	double lngi = atan2(yyi, xxi);

	
	if (!(lngi >= qMin(lng1, lng2) && (lngi <= qMax(lng1, lng2)))) {
		// Try the other answer of the quadratic equation
		xxi = (planarConst - firstX - secondX) / aa;
		yyi = (bb * planarConst + aa * rad) / scale;
		lati = asin(zzi / EarthRadius);
		lngi = atan2(yyi, xxi);

		if (!(lngi >= qMin(lng1, lng2) && (lngi <= qMax(lng1, lng2)))) {
			rCritical(threadID) << "There is no intersection.";
			return -1;
		}
	}

	*intersectionLongitude = radianToDegree(lngi);

	return 0;
}

double RLocation::degreeToRadian(double degree)
{
	return degree / 180 * M_PI;
}

double RLocation::radianToDegree(double radian)
{
	return radian / M_PI * 180;
}

void RLocation::openMap()
{
	theMapWidget = theMainWindow->findChild<RMapWidget *>();
	if (!theMapWidget) {
		theMainWindow->viewMap();
		QMessageBox::information(0, tr("Information"), tr("The map is not open. Rt will now open the map. Please try again when the loading of the map is complete"), QMessageBox::Ok | QMessageBox::Default);
	}
}

QStringList RLocation::getActionList()
{
	QStringList actionList;
	actionList << "&Display on Map" << "&Remove from Map" << "&Get Coordinates from Map" << "&Get Coordinates from Address";
	return actionList;
}

int RLocation::displayonMap()
{
	openMap();
	if (!theMapWidget) {
		return -1;
	}
	theMainWindow->setActiveSubWindow(theMapWidget);
	//int index = theMapWidget->findLocationIndex(this);
	//if (index == -1) {
	theMapWidget->clearAndDisplayLocation(this);
	//}
	return 0;
}

void RLocation::redisplayonMap()
{
	theMapWidget = theMainWindow->findChild<RMapWidget *>();
	if (!theMapWidget) {
		return;
	}
	//int index = theMapWidget->findLocationIndex(this);
	//if (index == -1) {
	theMapWidget->displayLocation(this);
	//}
}

void RLocation::removefromMap()
{
	theMapWidget = theMainWindow->findChild<RMapWidget *>(); 
	if (!theMapWidget) {
		return;
	}
	theMapWidget->removeLocation(this);
}

void RLocation::getCoordinatesfromMap()
{
	openMap();
	if (!theMapWidget) {
		return;
	}
	theMainWindow->setActiveSubWindow(theMapWidget);	
	bool ok = connect(theMapWidget, SIGNAL(locationAdded(double, double, QString)), this, SLOT(assignCoordinatesFromMap(double, double)));
	theMapWidget->getCoordinatesfromMap(this);
}

void RLocation::assignCoordinatesFromMap(double latitude, double longitude)
{
	openMap();
	if (!theMapWidget) {
		return;
	}
	bool ok = disconnect(theMapWidget, SIGNAL(locationAdded(double, double, QString)), this, SLOT(assignCoordinatesFromMap(double, double)));
	theLatitude[0] = latitude;
	theLongitude[0] = longitude;
	theMainWindow->updatePropertyList();
}

void RLocation::getMultipleCoordinatesfromAddress(QString address)
{
	openMap();
	if (!theMapWidget) {
		return;
	}
	theMainWindow->setActiveSubWindow(theMapWidget);
	theMapWidget->getCoordinatesfromAddress(address);
}

void RLocation::updateProperty() {
	theMainWindow->updatePropertyList();
}


void RLocation::getCoordinatesfromAddress()
{

	openMap();
	if (!theMapWidget) {
		return;
	}
	theMainWindow->setActiveSubWindow(theMapWidget);
	theMapWidget->getSingleCoordinatesfromAddress(theAddress, this->objectName());



}



void RLocation::assignCoordinatesFromAddress(double latitude, double longitude)
{

	openMap();
	if (!theMapWidget) {
		return;
	}
	bool ok = disconnect(theMapWidget, SIGNAL(locationAdded(double, double, QString)), this, SLOT(assignCoordinatesFromAddress(double , double )));
	

	theLatitude[0] = latitude;
	theLongitude[0] = longitude;
	theMainWindow->updatePropertyList();
	

}

void RLocation::initializeForParallelComputation(int threadID){

	// initialize in superclass
	RParameter::initializeForParallelComputation(threadID);

	theLatitude[threadID] = theLatitude[0];
	theLongitude[threadID] = theLongitude[0];
	theAltitude[threadID] = theAltitude[0];
}

bool RLocation::canBeRunParallelly(){
	return true;
}