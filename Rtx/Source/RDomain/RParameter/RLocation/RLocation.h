// Last modified by Mojtaba on June 30, 2012

#ifndef RLocation_H
#define RLocation_H

#include "RParameter.h"

#include <math.h>

class QNetworkReply;
class RMapWidget;
class RMainWindow;

const double EarthRadius = 6372.796924;

class RLocation : public RParameter
{
	Q_OBJECT	

	// Sets the latitude of the location
	Q_PROPERTY(double Latitude READ getLatitude WRITE setLatitude)

	// Sets the longitude of the location
	Q_PROPERTY(double Longitude READ getLongitude WRITE setLongitude)

	// Sets the depth/height of the location; depth is indicated by negative altitude
	Q_PROPERTY(double Altitude READ getAltitude WRITE setAltitude)

	// Sets the address of the location
	Q_PROPERTY(QString Address READ getAddress WRITE setAddress)

public:
	RLocation(QObject *parent, QString name);
	virtual ~RLocation();
	
	// setter methods for meta-properties
	void setLatitude(double value);
	void setLongitude(double value);
	void setAltitude(double value);
	void setAddress(QString value);

	// getter methods for meta-properties
	QString getAddress() const;

	// GIVE THREADID TO THIS METHODS WHEN USING THEM, DEFAULT VALUES ARE FOR GUI USE ONLY
	double getLatitude(int threadID = 0) const;
	double getLongitude(int threadID = 0) const;
	double getAltitude(int threadID = 0) const;

	// thread specific setters, ONLY USE THESE SETTERS
	void setLatitude(double value,int threadID);
	void setLongitude(double value,int threadID);
	void setAltitude(double value,int threadID);

	// Computes the surface distance to another location, in the given threadID ($MTh$)
	double computeSurfaceDistance(const RLocation *destination, int threadID);

	// Computes the bearing (angle with north direction), in the given threadID ($MTh$)
	double computeBearing(const RLocation *destination, int threadID);

	// Computes the destination location given the distance and bearing, in the given threadID ($MTh$)
	void computeDestination(RLocation *destination, double distance, double bearing, int threadID);

	// Computes the derivative of the destination latitude and longitude wrt. the bearing and distance, in the given threadID ($MTh$)
	void computeDestinationGradient(double distance, double bearing, double *dLat_dDistance, double *dLat_dBearing, double *dLng_dDistance, double *dLng_dBearing, int threadID);

	// Computes the derivative of the distance with respect to the latitude and longitude of the destination location by DDM, in the given threadID ($MTh$)
	void computeSurfaceDistanceGradient(const RLocation *destination, double *dDistance_dLat, double *dDistance_dLng, int threadID);

	// Computes the coordinates of the an intermediate point given a ratio of the distance, in the given threadID ($MTh$)
	void computeIntermediatePoint(RLocation *endPoint, RLocation *intermediatePoint, double distanceRatioFromStartPoint, int threadID);

	// Computes the derivative of the latitude and longitude of the intermediate point wrt. the ratio of the distance by DDM, in the given threadID ($MTh$)
	void computeIntermediatePointGradient(RLocation *endPoint, double distanceRatioFromStartPoint ,double *dLat_dDistanceRatio, double *dLng_dDistanceRatio, int threadID);

	// Computes the coordinates of the intersection with a given latitude, in the given threadID ($MTh$)
	int computeIntersectionWithLatitude(double latitude, RLocation *endPoint, double *intersectionLongitude, int threadID);

	// Returns a list of "right-click" menu items
	QStringList getActionList();

	// Opens the RMapWidget object in the main window
	void openMap();

	//Updates property list of this location
	void updateProperty();

public slots:
	// Displays a marker for the new realization of the location on the map while keeping the markers for previous realizations
	void redisplayonMap();

	// Removes all the markers for this location from the map
	void removefromMap();

	// Displays a marker for the current realization of the location on the map while removing all previous markers
	int displayonMap();

	// Starts the operation of getting the latitude and longitude from the user's click on the map
	void getCoordinatesfromMap();

	// Assigns the latitude and longitude once the user is done clicking on the map
	void assignCoordinatesFromMap(double latitude, double longitude);

	// Starts the operation of getting the latitude and longitude by sending the address to Google
	void getCoordinatesfromAddress();

	// Starts the operation of getting the latitude and longitude by sending the address to Google
	void getMultipleCoordinatesfromAddress(QString address);


	// Assigns the latitude and longitude once Google sends back the latitude and longitude for the given address
	void assignCoordinatesFromAddress(double latitude, double longitude);

	// Copies the value at index 0 to the index of the given threadID for data members that are changed to array due to parallelization. Each subclass must also call the method of their superclass
	void initializeForParallelComputation(int threadID);

	// returns true as this parameter is parallelized
	bool canBeRunParallelly();

private:
	// Transforms degrees to radians
	double degreeToRadian(double degree);

	// Transforms radians to degrees
	double radianToDegree(double radian);

	// Data members for meta-properties ($MTh$)
	double * theLatitude;
	double * theLongitude;
	double * theAltitude;
	QString theAddress;

	// Pointer to the RMapWidget Object
	QPointer<RMapWidget> theMapWidget;

	// Pointer to the main window
	RMainWindow *theMainWindow;
};

#endif // RLocation_H
