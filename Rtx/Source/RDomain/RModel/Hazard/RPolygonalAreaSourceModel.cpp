#include "RPolygonalAreaSourceModel.h"	
	
#include "RParameter.h"	
#include "RLocationResponse.h"	
#include "RContinuousRandomVariable.h"
#include "RLocation.h"
#include "RMainWindow.h"
#include "RMapWidget.h"

#include <math.h>

#include <gsl/gsl_math.h>

#include <qmutex.h>
#include <QMutexLocker>

RPolygonalAreaSourceModel::RPolygonalAreaSourceModel(QObject *parent, QString name)	
	: RModel(parent, name)
{	
	// Initializing the pointer data members
	/* //$$ 
	theTheta2 = 0;
	theTheta1 = 0;
	theDepth = 0;
	*/

	theLatitudeRandomVariable = 0;
	
	// Instantiating the response object
	theDomain->createObject(objectName() + "Response", "RLocationResponse");
	theHypocentreLocation = qobject_cast<RLocationResponse *>(theDomain->getLastAddedObject());
	theHypocentreLocation->setModel(this);

	latitudeRandomVariableLock = new QMutex();
}	

RPolygonalAreaSourceModel::~RPolygonalAreaSourceModel()	
{	
	if (theLatitudeRandomVariable) {
		delete theLatitudeRandomVariable;
	}
	theLatitudeRandomVariable = 0;

	delete latitudeRandomVariableLock;
}	

QString RPolygonalAreaSourceModel::getBoundaryLocationList()
{
	return parameterListToString(theBoundaryLocationList);
}

void RPolygonalAreaSourceModel::setBoundaryLocationList(QString value)
{
	theBoundaryLocationList = stringToParameterList(value);
	
	// Checking if all the passed parameters are either RLocation or RLocationResponse
	parameterListToLocationList(theBoundaryLocationList, true);

	if (theLatitudeRandomVariable) {
		delete theLatitudeRandomVariable;
	}
	theLatitudeRandomVariable = 0;
}

QObject * RPolygonalAreaSourceModel::getTheta2() const	
{	
	return theTheta2.data();
}	

void RPolygonalAreaSourceModel::setTheta2(QObject *value)	
{	
	theTheta2 = qobject_cast<RParameter *>(value);
}	

QObject * RPolygonalAreaSourceModel::getTheta1() const	
{	
	return theTheta1.data();
}	

void RPolygonalAreaSourceModel::setTheta1(QObject *value)	
{	
	theTheta1 = qobject_cast<RParameter *>(value);
}		

QObject * RPolygonalAreaSourceModel::getDepth() const	
{	
	return theDepth.data();
}	

void RPolygonalAreaSourceModel::setDepth(QObject *value)	
{	
	theDepth = qobject_cast<RParameter *>(value);
}

QList<RParameter *> RPolygonalAreaSourceModel::getPhysicalParameterList()
{
	QList<RParameter *> paramList;
	paramList << theTheta1.data() << theTheta2.data() << theDepth.data();
	return paramList;
}

void RPolygonalAreaSourceModel::reset()
{
	if (theLatitudeRandomVariable) {
		delete theLatitudeRandomVariable;
	}
	theLatitudeRandomVariable = 0;
}

int RPolygonalAreaSourceModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)	
{	
	QList<RLocation *> theLocationList = parameterListToLocationList(theBoundaryLocationList);
		
	// Getting the pointer to the RLocation object from the RLocationResponse object (theHypocentreLocation)
	RLocation *theHypocentreLocationObject = parameterToLocation(theHypocentreLocation);
	if (!theHypocentreLocationObject) {
		rCritical(threadID) << "Error: The location response" << theHypocentreLocation->objectName() << "does not include any location object.";
		return -1;
	}

	int pointCount = theLocationList.count();

	// start of safe area scope
	{
		QMutexLocker locker(latitudeRandomVariableLock);

		if (!theLatitudeRandomVariable) {
			QMap<double, double> latLngMap;

			// The operation below finds the adjacent points that have the same latitude, and adds/subtracts (altenating) an epsilon to the latitude of the second point to ensure that problems will not arise when computing the total width of the polygon at that latitude using the lagorithm below
			double epsilon = 1e-7;
			for (int i = 0; i < pointCount; i++) {
				int j = i + 1;
				if (i == pointCount - 1) {
					j = 0;
				}
				if (theLocationList[i]->getLatitude(threadID) == theLocationList[j]->getLatitude(threadID)) {
					theLocationList[j]->setLatitude(theLocationList[j]->getLatitude() + epsilon, threadID);
					epsilon = -epsilon;
				}
			}

			// Creating a map of all longitudes for each latitude
			for (int i = 0; i < pointCount; i++) {
				latLngMap.insertMulti(theLocationList[i]->getLatitude(threadID), theLocationList[i]->getLongitude(threadID));
			}

			QVector<double> latVector = latLngMap.uniqueKeys().toVector();
			QVector<double> PDFVector;
			for (int i = 0; i < latVector.count(); i++) {
				double lat = latVector[i];

				QVector<double> lngVector;
				for (int j = 0; j < pointCount; j++) {
					// k is the next point
					int k = j + 1;
					if (k == pointCount) {
						k = 0;
					}
					if ((theLocationList[j]->getLatitude(threadID) - lat) * (theLocationList[k]->getLatitude(threadID) - lat) < 0) {
						// The line between j and k intersects with this latitude
						double intersectionLng;
						/*// TEMP....................................................................................................................................................................
						double lng1 = theLocationList[j]->getLongitude();
						double lng2 = theLocationList[k]->getLongitude();
						double lat1 = theLocationList[j]->getLatitude();
						double lat2 = theLocationList[k]->getLatitude();
						intersectionLng = lng1 + (lng2 - lng1)/(lat2 - lat1) * (lat - lat1);*/
						int result = theLocationList[j]->computeIntersectionWithLatitude(lat, theLocationList[k], &intersectionLng,threadID);
						if (result < 0) {
							rCritical(threadID) << "Error: Cannot compute the intersection in model" << objectName() << ".";
						}
						lngVector << intersectionLng;
					
						// OR CALCULATE THE INTERSECTION USING A LINEAR RELATIONSHIP BETWEEN LAT AND LNG. THIS IS HOW WE HAVE EVALAUTE THE PDF OF LATITUDE AFTER ALL.

					} else if (theLocationList[j]->getLatitude(threadID) == lat) {
						// The point j iteself is on the latitude

						// l is the previous point
						int l = j - 1;
						if (l == -1) {
							l = pointCount - 1;
						}

						// Add the point j's longitude to the lngVector only if the the lines that go out of point j are not at the same side of the latitude
						if ((theLocationList[l]->getLatitude(threadID) - lat) * (theLocationList[k]->getLatitude(threadID) - lat) <= 0) {
							lngVector << theLocationList[j]->getLongitude(threadID);
						}
					}
				}
				qSort(lngVector.begin(), lngVector.end());

				double length = 0;
				for (int j = 0; j < lngVector.count()-1; j += 2) {
					length += qAbs(lngVector[j+1] - lngVector[j]);
				}
				length *= cos(lat / 180 * M_PI);

				PDFVector << length;
			}

			theLatitudeRandomVariable = new RContinuousRandomVariable(0, objectName() + "LatRV");
			theLatitudeRandomVariable->setDistributionType("User Defined");
			theLatitudeRandomVariable->setUserDefinedPDF(latVector, PDFVector);
		}
	}
	// end of safe area scope
	
	double theta1 = theTheta1.data()->getCurrentValue(threadID);
	double theta2 = theTheta2.data()->getCurrentValue(threadID);

	double latitude = theLatitudeRandomVariable->getInverseCDFValue(theta1, 0);

	bool isDifferentiable = true;

	QVector<double> lngVector;
	QVector<double> dLngVector;
	double topOrBottomMostLongitude;
	for (int i = 0; i < pointCount; i++) {
		int j = i + 1;
		if (j == pointCount) {
			j = 0;
		}
		if ((theLocationList[i]->getLatitude(threadID) - latitude) * (theLocationList[j]->getLatitude(threadID) - latitude) < 0) {
			double intersectionLng;
			int result = theLocationList[i]->computeIntersectionWithLatitude(latitude, theLocationList[j], &intersectionLng,threadID);
			if (result < 0) {
				rCritical(threadID) << "Error: Cannot compute the intersection in model" << objectName() << ".";
			}
			lngVector << intersectionLng;

			if (evaluateDDM) {
				double lngForDiff;
				double perturb = 0.0001;
				int result = theLocationList[i]->computeIntersectionWithLatitude(latitude * (1 + perturb), theLocationList[j], &lngForDiff,threadID);
				if (result < 0) {
					isDifferentiable = false;
				}
				double dIntersectionLongitude_dLatitude = (lngForDiff - intersectionLng) / (perturb * latitude);
				dLngVector << dIntersectionLongitude_dLatitude;
			}
		} else if (theLocationList[i]->getLatitude(threadID) == latitude) {
			// The point i iteself is on the latitude

			// k is the previous point
			int k = i - 1;
			if (k == -1) {
				k = pointCount - 1;
			}

			// Add the point i's longitude to the lngVector only if the the lines that go out of point j are not at the same side of the latitude
			if ((theLocationList[k]->getLatitude(threadID) - latitude) * (theLocationList[j]->getLatitude(threadID) - latitude) <= 0) {
				lngVector << theLocationList[i]->getLongitude(threadID);
			} else {
				topOrBottomMostLongitude = theLocationList[i]->getLongitude(threadID);
			}
			
			if (evaluateDDM) {
				if ((theLocationList[k]->getLatitude(threadID) - latitude) * (theLocationList[j]->getLatitude(threadID) - latitude) < 0) {
					isDifferentiable = false;	
				} else {
					// This condition is only for the cases when DDM is needed and the point is a vertex whose outgoing lines go in the same side of the latitude or one of the lines goes on the latitude
					double lngForDiff;
					double perturb = 0.0001;
					double intersectionLng = theLocationList[i]->getLongitude(threadID);

					double sign = 1.0;
					if (latitude > theLocationList[j]->getLatitude(threadID)) {
						sign = -1;
					}
					int result = theLocationList[i]->computeIntersectionWithLatitude(latitude * (1 + sign * perturb), theLocationList[j], &lngForDiff,threadID);
					if (result >= 0) {
						double dIntersectionLongitude_dLatitude = (lngForDiff - intersectionLng) / (sign * perturb * latitude);
						dLngVector << dIntersectionLongitude_dLatitude;
					}

					sign = 1.0;
					if (latitude > theLocationList[k]->getLatitude(threadID)) {
						sign = -1;
					}
					result = theLocationList[i]->computeIntersectionWithLatitude(latitude * (1 + sign * perturb), theLocationList[k], &lngForDiff,threadID);
					if (result >= 0) {
						double dIntersectionLongitude_dLatitude = (lngForDiff - intersectionLng) / (sign * perturb * latitude);
						dLngVector << dIntersectionLongitude_dLatitude;
					}

					// Correcting the order of the derivatives in the vector if this is a vertex whose outgoing lines go in the same side of the latitude
					if ((theLocationList[k]->getLatitude(threadID) - latitude) * (theLocationList[j]->getLatitude(threadID) - latitude) != 0) {
						if (theLocationList[j]->getLongitude(threadID) > theLocationList[k]->getLongitude(threadID)) {
							// Swapping
							dLngVector.append(dLngVector[0]);
							dLngVector.remove(0);
						}
					}
				}
			}
		}
	}
	QVector<double> lngVectorBackup = lngVector; 
	qSort(lngVector.begin(), lngVector.end());

	double deltaLng = 0;
	for (int i = 0; i < lngVector.count(); i += 2) {
		deltaLng += qAbs(lngVector[i+1] - lngVector[i]);
	}


	double dLng_dLat = 0;
	if (dLngVector.count() != 2) {
		isDifferentiable = false;
	} else {
		if (evaluateDDM) {
			if (lngVector.count() == 2) {
				// Correcting the order of the derivatives in the vector
				if (lngVectorBackup[0] > lngVectorBackup[1]) {
					dLngVector.append(dLngVector[0]);
					dLngVector.remove(0);
				}
			}
			dLng_dLat = (dLngVector[1] - dLngVector[0]) * theta2 +  dLngVector[0];
		}
	}


	double lngPortion = deltaLng * theta2;
	double lngSoFar = 0;
	double longitude;
	for (int i = 0; i < lngVector.count(); i += 2) {
		lngSoFar += qAbs(lngVector[i+1] - lngVector[i]);
		if (lngSoFar >= lngPortion) {
			longitude = lngVector[i + 1] - (lngSoFar - lngPortion);
			break;
		}
	}

	// If the point is topmost ot bottommost point
	if (lngVector.isEmpty()) {
		longitude = topOrBottomMostLongitude;
	}

	theHypocentreLocationObject->setLatitude(latitude,threadID);
	theHypocentreLocationObject->setLongitude(longitude,threadID);

	//if (theDisplayOutput) {
	//	rDebug() << qPrintable(QString("%1\t%2").arg(longitude, 0, 'g', 12).arg(latitude, 0, 'g', 12));
	//}
	
	double depth = theDepth.data()->getCurrentValue(threadID);
	theHypocentreLocationObject->setAltitude(-1.0 * depth,threadID);

	if (threadID == 0){
		theHypocentreLocationObject->redisplayonMap();
	}

	if (evaluateDDM) {
		if (!isDifferentiable) {
			rCritical(threadID) << "Warning: The model" << objectName() << "is discontinuous and thus, not differentiable using DDM.";
			//return -10;  //?????????????????????????????
		}

		double dLat_dTheta1 = 1.0 / theLatitudeRandomVariable->getPDFValue(latitude,0);
		double dLat_dTheta2 = 0.0;
		double dLng_dTheta1 = dLng_dLat * dLat_dTheta1;
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
			double pert;
			if (theta1 == 0) {
				pert = 0.01;
			} else {
				pert = 0.001 * theta1;
			}
			pert *= -1;
			theta1 += pert;
			double newlat = theLatitudeRandomVariable->getInverseCDFValue(theta1);


			QVector<double> lngVector;
			for (int i = 0; i < pointCount; i++) {
				int j = i + 1;
				if (j == pointCount) {
					j = 0;
				}
				if ((theLocationList[i]->getLatitude() - newlat) * (theLocationList[j]->getLatitude() - newlat) < 0) {
					double intersectionLng;
					int result = theLocationList[i]->computeIntersectionWithLatitude(newlat, theLocationList[j], &intersectionLng);
					if (result < 0) {
						rCritical() << "Error: Cannot compute the intersection in model" << objectName() << ".";
					}
					lngVector << intersectionLng;
				} else if (theLocationList[i]->getLatitude() == newlat) {
					// The point i iteself is on the newlat

					// k is the previous point
					int k = i - 1;
					if (k == -1) {
						k = pointCount - 1;
					}

					// Add the point i's longitude to the lngVector only if the the lines that go out of point j are not at the same side of the newlat
					if ((theLocationList[k]->getLatitude() - newlat) * (theLocationList[j]->getLatitude() - newlat) <= 0) {
						lngVector << theLocationList[i]->getLongitude();
					}
				}
			}
			qSort(lngVector.begin(), lngVector.end());

			double deltaLng = 0;
			for (int i = 0; i < lngVector.count(); i += 2) {
				deltaLng += qAbs(lngVector[i+1] - lngVector[i]);
			}
			double lngPortion = deltaLng * theta2;
			double lngSoFar = 0;
			double newlng;
			for (int i = 0; i < lngVector.count(); i += 2) {
				lngSoFar += qAbs(lngVector[i+1] - lngVector[i]);
				if (lngSoFar >= lngPortion) {
					newlng = lngVector[i + 1] - (lngSoFar - lngPortion);
					break;
				}
			}



			double dLat_dTheta1DF = (newlat - latitude) / pert;
			double dLng_dTheta1DF = (newlng - longitude) / pert;
			theta1 -= pert;
		}
		{
			theta2 *= 1.001;
			double newlat = theLatitudeRandomVariable->getInverseCDFValue(theta1);





			QVector<double> lngVector;
			for (int i = 0; i < pointCount; i++) {
				int j = i + 1;
				if (j == pointCount) {
					j = 0;
				}
				if ((theLocationList[i]->getLatitude() - newlat) * (theLocationList[j]->getLatitude() - latitude) < 0) {
					double intersectionLng;
					int result = theLocationList[i]->computeIntersectionWithLatitude(latitude, theLocationList[j], &intersectionLng);
					if (result < 0) {
						rCritical() << "Error: Cannot compute the intersection in model" << objectName() << ".";
					}
					lngVector << intersectionLng;
				} else if (theLocationList[i]->getLatitude() == latitude) {
					// The point i iteself is on the latitude

					// k is the previous point
					int k = i - 1;
					if (k == -1) {
						k = pointCount - 1;
					}

					// Add the point i's longitude to the lngVector only if the the lines that go out of point j are not at the same side of the latitude
					if ((theLocationList[k]->getLatitude() - latitude) * (theLocationList[j]->getLatitude() - latitude) <= 0) {
						lngVector << theLocationList[i]->getLongitude();
					}
				}
			}
			qSort(lngVector.begin(), lngVector.end());

			double deltaLng = 0;
			for (int i = 0; i < lngVector.count(); i += 2) {
				deltaLng += qAbs(lngVector[i+1] - lngVector[i]);
			}
			double lngPortion = deltaLng * theta2;
			double lngSoFar = 0;
			double newlng;
			for (int i = 0; i < lngVector.count(); i += 2) {
				lngSoFar += qAbs(lngVector[i+1] - lngVector[i]);
				if (lngSoFar >= lngPortion) {
					newlng = lngVector[i + 1] - (lngSoFar - lngPortion);
					break;
				}
			}



			double dLat_dTheta2DF = (newlat - latitude) / (0.001 * theta2);
			double dLng_dTheta2DF = (newlng - longitude) / (0.001 * theta2);
			theta2 /= 1.001;
		}*/
	}

	return 1;
}	

QStringList RPolygonalAreaSourceModel::getActionList()
{
	QStringList actionList;
	actionList << "&Display on Map" << "&Remove from Map";
	return actionList;
}

void RPolygonalAreaSourceModel::displayonMap()
{


	QList<RLocation *> locationList = parameterListToLocationList(theBoundaryLocationList);
	if (locationList.count() < 3) {
		rCritical() << "Warning: Please enter at least 3 location objects in the model" << objectName() << ".";
		return;
	}
	RMainWindow *theMainWindow = qobject_cast<RMainWindow *>(theDomain->parent());
	RMapWidget *theMapWidget = theMainWindow->findChild<RMapWidget *>();
	theMainWindow->setActiveSubWindow(theMapWidget);
	if (!theMapWidget) {
		locationList[0]->openMap();
		return;
	}
	theMapWidget->drawPolygon(this, parameterListToLocationList(theBoundaryLocationList), Qt::red);


#ifndef WEB_PROTOTYPE
	QList<RLocation *> locationList = parameterListToLocationList(theBoundaryLocationList);
	if (locationList.count() < 3) {
		rCritical() << "Warning: Please enter at least 3 location objects in the model" << objectName() << ".";
		return;
	}
	RMainWindow *theMainWindow = qobject_cast<RMainWindow *>(theDomain->parent());
	RMapWidget *theMapWidget = theMainWindow->findChild<RMapWidget *>();
	theMainWindow->setActiveSubWindow(theMapWidget);
	if (!theMapWidget) {
		locationList[0]->openMap();
		return;
	}
	theMapWidget->drawPolygon(this, parameterListToLocationList(theBoundaryLocationList), Qt::red);
#endif // WEB_PROTOTYPE
}

void RPolygonalAreaSourceModel::removefromMap()
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

bool RPolygonalAreaSourceModel::canBeRunParallelly(){
	return true;
}