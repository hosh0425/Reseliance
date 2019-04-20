#include "RJoynerBoore1981IntensityModel.h"	
	
#include "RParameter.h"	
#include "RResponse.h"	
#include "RLocation.h"
#include "RLocationResponse.h"
#include "RMainWindow.h"

#include <math.h>
	
RJoynerBoore1981IntensityModel::RJoynerBoore1981IntensityModel(QObject *parent, QString name)	
	: RModel(parent, name)
{	
	// Initializing the pointer data members
/* //$$ 
	theMagnitude = 0;
	theAlpha = 0;
	theBeta = 0;
	theB = 0;
	theEpsilon = 0;
	theHypocentreLocation = 0;
*/
}	
	
RJoynerBoore1981IntensityModel::~RJoynerBoore1981IntensityModel()	
{	
	
}
	
QObject * RJoynerBoore1981IntensityModel::getMagnitude() const	
{	
	return theMagnitude.data();
}	
	
void RJoynerBoore1981IntensityModel::setMagnitude(QObject *value)	
{	
	theMagnitude = qobject_cast<RParameter *>(value);
}
	
QObject * RJoynerBoore1981IntensityModel::getAlpha() const	
{	
	return theAlpha.data();
}	
	
void RJoynerBoore1981IntensityModel::setAlpha(QObject *value)	
{	
	theAlpha = qobject_cast<RParameter *>(value);
}
	
QObject * RJoynerBoore1981IntensityModel::getBeta() const	
{	
	return theBeta.data();
}	
	
void RJoynerBoore1981IntensityModel::setBeta(QObject *value)	
{	
	theBeta = qobject_cast<RParameter *>(value);
}
	
QObject * RJoynerBoore1981IntensityModel::getB() const	
{	
	return theB.data();
}	
	
void RJoynerBoore1981IntensityModel::setB(QObject *value)	
{	
	theB = qobject_cast<RParameter *>(value);
}
	
QObject * RJoynerBoore1981IntensityModel::getEpsilon() const	
{	
	return theEpsilon.data();
}	
	
void RJoynerBoore1981IntensityModel::setEpsilon(QObject *value)	
{	
	theEpsilon = qobject_cast<RParameter *>(value);
}
	
QObject * RJoynerBoore1981IntensityModel::getHypocentreLocation() const	
{	
	return theHypocentreLocation.data();
}	
	
void RJoynerBoore1981IntensityModel::setHypocentreLocation(QObject *value)	
{	
	theHypocentreLocation = qobject_cast<RParameter *>(value);

	// Verification that the user has given an RLocationResponse or an RLocation
	parameterToLocation(theHypocentreLocation.data());
}	

QString RJoynerBoore1981IntensityModel::getStructureLocationList()
{
	return parameterListToString(theStructureLocationList);
}

void RJoynerBoore1981IntensityModel::setStructureLocationList(QString value)
{
	theStructureLocationList = stringToParameterList(value);
	
	// Checking if all the passed parameters are either RLocation or RLocationResponse
	parameterListToLocationList(theStructureLocationList);
	
	// Removing old responses
	while (thePGAList.count() > 0) {
		//thePGAList[0]->setModel(0);
		delete thePGAList.takeAt(0);
	}
	
	// Instantiating the response objects one for each structure
	for (int i = 0; i < theStructureLocationList.count(); i++) {
		theDomain->createObject(objectName() + theStructureLocationList[i].data()->objectName() + "Response", "RGenericResponse");
		RResponse *response = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
		response->setModel(this);
		thePGAList << response;
	}

	// Update the domain tree
	RMainWindow *mainWindow = qobject_cast<RMainWindow *>(parent()->parent());
	if (mainWindow) {
		mainWindow->updateTree("Domain");
	}
}
	
int RJoynerBoore1981IntensityModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
{
	// NOTE: IN THIS MODEL, h IS NOT DEPTH. IT'S A PARAMETER EQUAL TO 7.3. THIS SHOULD BE FIXED.

	// For convenice, look up the values of the parameters first
	double M = theMagnitude.data()->getCurrentValue(threadID);
	double alpha = -1.02 * theAlpha.data()->getCurrentValue(threadID);
	double beta = 0.249 * theBeta.data()->getCurrentValue(threadID);
	double b = -0.00255 * theB.data()->getCurrentValue(threadID);
	double epsilon = theEpsilon.data()->getCurrentValue(threadID);

	// Converting the list of parameters to the list of locations
	QList<RLocation *> theLocationList = parameterListToLocationList(theStructureLocationList);

	// Verification that the user has given an RLocationResponse
	RLocation *theHypocentreLocationObject = parameterToLocation(theHypocentreLocation.data());  
	if (!theHypocentreLocationObject) {
		rCritical(threadID) << "Error: The location response" << theHypocentreLocation.data()->objectName() << "does not include any location object.";
		return -1;
	}
	double alt = theHypocentreLocationObject->getAltitude(threadID);
	double h = qAbs(alt);

	for (int i = 0; i < theLocationList.count(); i++) {
		// Calculate distance R
		double R = theHypocentreLocationObject->computeSurfaceDistance(theLocationList[i],threadID);

		// Calulate the Joyner-Boore R
		double r = sqrt(R * R + h * h);
		
		double logPGA = alpha + beta * M - log10(r) + b * r + epsilon * 0.26;//???????????????????????
		double PGA = pow(10.0, logPGA);
		thePGAList[i]->setCurrentValue(PGA,threadID);


		if (evaluateDDM) {
			double log_10 = log(10.0);

			double dPGA_dAlpha = -1.02 * PGA * log_10;
			double dPGA_dBeta = 0.249 * M * PGA * log_10;
			double dPGA_dM = beta * PGA * log_10;
			double dPGA_dEpsilon = 0.26 * PGA * log_10;
			double dPGA_db = -0.00255 * r * PGA * log_10;

			double dPGA_dr = (-1.0 / r + b * log_10) * PGA;
			double dr_dR = R / sqrt(R * R + h * h);
			double dr_dh = h / sqrt(R * R + h * h);

			double dR_dLat;
			double dR_dLng;
			theHypocentreLocationObject->computeSurfaceDistanceGradient(theLocationList[i], &dR_dLat, &dR_dLng,threadID);
			double dPGA_dLat = dPGA_dr * dr_dR * dR_dLat;
			double dPGA_dLng = dPGA_dr * dr_dR * dR_dLng;

			double dh_dAlt = alt > 0 ? 1 : (alt < 0 ? -1 : 0); // signum function
			double dPGA_dAlt = dPGA_dr * dr_dh * dh_dAlt;

			QMap<RParameter *, double> thePGAGradientMap;
			thePGAGradientMap.insertMulti(theMagnitude.data(), dPGA_dM);
			thePGAGradientMap.insertMulti(theAlpha.data(), dPGA_dAlpha);
			thePGAGradientMap.insertMulti(theBeta.data(), dPGA_dBeta);
			thePGAGradientMap.insertMulti(theB.data(), dPGA_db);
			thePGAGradientMap.insertMulti(theEpsilon.data(), dPGA_dEpsilon);
			thePGAGradientMap.insertMulti(theHypocentreLocation.data(), dPGA_dLat);
			thePGAGradientMap.insertMulti(theHypocentreLocation.data(), dPGA_dLng);
			thePGAGradientMap.insertMulti(theHypocentreLocation.data(), dPGA_dAlt);

			theDDMMap.insertMulti(thePGAList[i], thePGAGradientMap);
		}

	}
	return 1;
}

bool RJoynerBoore1981IntensityModel::canBeRunParallelly(){
	return true;
}