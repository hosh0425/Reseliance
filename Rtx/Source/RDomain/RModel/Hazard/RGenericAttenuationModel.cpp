#include "RGenericAttenuationModel.h"	

#include <QCoreApplication>

#include "RParameter.h"	
#include "RResponse.h"	
#include "RLocation.h"
#include "RLocationResponse.h"
#include "RMainWindow.h"

#include <math.h>

RGenericAttenuationModel::RGenericAttenuationModel(QObject *parent, QString name)	
	: RModel(parent, name)
{	
	// Initializing the pointer data members
	/* //$$ 
	theTheta1 = 0;
	theTheta2 = 0;
	theTheta3 = 0;
	theTheta4 = 0;
	theEpsilon = 0;
	*/
}	

RGenericAttenuationModel::~RGenericAttenuationModel()	
{	
	
}

QString RGenericAttenuationModel::getMagnitudeList()	
{	
	return parameterListToString(theMagnitudeList);
}	

void RGenericAttenuationModel::setMagnitudeList(QString value)	
{	
	theMagnitudeList = stringToParameterList(value);
}

QString RGenericAttenuationModel::getHypocentreLocationList()	
{	
	return parameterListToString(theHypocentreLocationList);
}	

void RGenericAttenuationModel::setHypocentreLocationList(QString value)	
{	
	theHypocentreLocationList = stringToParameterList(value);

	// Checking if all the passed parameters are either RLocation or RLocationResponse
	parameterListToLocationList(theHypocentreLocationList);
}	

QObject * RGenericAttenuationModel::getTheta1() const	
{	
	return theTheta1.data();
}	

void RGenericAttenuationModel::setTheta1(QObject *value)	
{	
	theTheta1 = qobject_cast<RParameter *>(value);
}

QObject * RGenericAttenuationModel::getTheta2() const	
{	
	return theTheta2.data();
}	

void RGenericAttenuationModel::setTheta2(QObject *value)	
{	
	theTheta2 = qobject_cast<RParameter *>(value);
}

QObject * RGenericAttenuationModel::getTheta3() const
{
	return theTheta3.data();
}

void RGenericAttenuationModel::setTheta3(QObject *value)
{
	theTheta3 = qobject_cast<RParameter *>(value);
}

QObject * RGenericAttenuationModel::getTheta4() const
{
	return theTheta4.data();
}

void RGenericAttenuationModel::setTheta4(QObject *value)
{
	theTheta4 = qobject_cast<RParameter *>(value);
}

QObject * RGenericAttenuationModel::getEpsilon() const	
{	
	return theEpsilon.data();
}	

void RGenericAttenuationModel::setEpsilon(QObject *value)	
{	
	theEpsilon = qobject_cast<RParameter *>(value);
}

QString RGenericAttenuationModel::getStructureLocationList()
{
	return parameterListToString(theStructureLocationList);
}

void RGenericAttenuationModel::setStructureLocationList(QString value)
{
	theStructureLocationList = stringToParameterList(value);

	// Checking if all the passed parameters are either RLocation or RLocationResponse
	parameterListToLocationList(theStructureLocationList);

	createResponses();
}

void RGenericAttenuationModel::createResponses()
{
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

		// A line to keep the GUI responsive
		if (i % 10 == 0) {
			QCoreApplication::processEvents();
		}
	}

	// Update the domain tree
	RMainWindow *mainWindow = qobject_cast<RMainWindow *>(parent()->parent());
	if (mainWindow) {
		mainWindow->updateTree("Domain");
	}
}

int RGenericAttenuationModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
{
	// NOTE: IN THIS MODEL, h IS NOT DEPTH. IT'S A PARAMETER EQUAL TO 7.3. THIS SHOULD BE FIXED.

	// Checking if the number of magnitudes is equal to the number of hypocentre locations
	if (theHypocentreLocationList.count() != theMagnitudeList.count()) {
		rCritical(threadID) << "Error: The number of magnitude parameters should be equal to the number of hypocentre locations in the model" << objectName() << ".";
		return -1;
	}

	// For convenience, look up the values of the parameters first
	double theta1 = theTheta1.data()->getCurrentValue(threadID);
	double theta2 = theTheta2.data()->getCurrentValue(threadID);
	double theta3 = theTheta3.data()->getCurrentValue(threadID);
	double theta4 = theTheta4.data()->getCurrentValue(threadID);
	double epsilon = theEpsilon.data()->getCurrentValue(threadID);

	// Converting the list of parameters to the list of locations
	QList<RLocation *> structureLocationList = parameterListToLocationList(theStructureLocationList);

	QVector<double> MVector;
	QList<RLocation *> hypocentreLocationObjectList;
	for (int i = 0; i < theMagnitudeList.count(); i++) {
		double M = theMagnitudeList[i].data()->getCurrentValue(threadID);
		MVector << M;
			
		// Verification that the user has given an RLocationResponse or an RLocation
		RLocation *hypocentreLocationObject = parameterToLocation(theHypocentreLocationList[i].data());  
		if (!hypocentreLocationObject) {
			rCritical(threadID) << "Error: The location response" << theHypocentreLocationList[i].data()->objectName() << "does not include any location object.";
			return -1;
		}
		hypocentreLocationObjectList << hypocentreLocationObject;
	}
				
	double log_10 = log(10.0);

	for (int i = 0; i < structureLocationList.count(); i++) {
		double totalPGA = 0;

		QMap<RParameter *, double> thePGAGradientMap;
		double total_dPGA_dTheta1 = 0;
		double total_dPGA_dTheta2 = 0;
		double total_dPGA_dTheta3 = 0;
		double total_dPGA_dTheta4 = 0;
		double total_dPGA_dEpsilon = 0;

		for (int j = 0; j < theHypocentreLocationList.count(); j++) {			
			RLocation *hypocentreLocationObject = hypocentreLocationObjectList[j];
			double M = MVector[j];

			double alt = hypocentreLocationObject->getAltitude(threadID);
			double h = qAbs(alt);

			// Calculate distance R
			double R = hypocentreLocationObject->computeSurfaceDistance(structureLocationList[i],threadID);

			// Calulate the Joyner-Boore R
			double r = sqrt(R * R + h * h);
			
			double logPGA = -theta1 + theta2 * M - theta3 * log10(r) + theta4 * r + epsilon;
			double PGA = pow(10.0, logPGA);

			// IMPORTANT: If M = 0, it means that this hazard is off, so the resulting PGA is Zero
			if (M == 0) {
				PGA = 0;
			}

			// Adding the cumulative PGA calculated from other hazards so far to this PGA
			totalPGA += PGA;


			if (evaluateDDM) {
				double factor = log_10 * PGA;

				double dPGA_dTheta1 = -factor;
				double dPGA_dTheta2 = M * factor;
				double dPGA_dTheta3 = -log10(r) * factor;
				double dPGA_dTheta4 = r * factor;
				double dPGA_dEpsilon = factor;

				double dPGA_dM = theta2 * factor;
				double dPGA_dr = (-theta3 / r + theta4 * log_10) * PGA;
				double dr_dR = R / sqrt(R * R + h * h);
				double dr_dh = h / sqrt(R * R + h * h);

				double dR_dLat;
				double dR_dLng;
				hypocentreLocationObject->computeSurfaceDistanceGradient(structureLocationList[i], &dR_dLat, &dR_dLng,threadID);
				double dPGA_dLat = dPGA_dr * dr_dR * dR_dLat;
				double dPGA_dLng = dPGA_dr * dr_dR * dR_dLng;

				double dh_dAlt = alt > 0 ? 1 : (alt < 0 ? -1 : 0); // signum function
				double dPGA_dAlt = dPGA_dr * dr_dh * dh_dAlt;

				total_dPGA_dTheta1 += dPGA_dTheta1;
				total_dPGA_dTheta2 += dPGA_dTheta2;
				total_dPGA_dTheta3 += dPGA_dTheta3;
				total_dPGA_dTheta4 += dPGA_dTheta4;
				total_dPGA_dEpsilon += dPGA_dEpsilon;
				
				thePGAGradientMap.insertMulti(theMagnitudeList[j].data(), dPGA_dM);
				thePGAGradientMap.insertMulti(theHypocentreLocationList[j].data(), dPGA_dLat);
				thePGAGradientMap.insertMulti(theHypocentreLocationList[j].data(), dPGA_dLng);
				thePGAGradientMap.insertMulti(theHypocentreLocationList[j].data(), dPGA_dAlt);


				/*// Finite difefrence check
				double d = 0.0001;
				hypocentreLocationObject->setLatitude(hypocentreLocationObject->getLatitude()+d);
				R = hypocentreLocationObject->computeSurfaceDistance(structureLocationList[j]);		
				r = sqrt(R * R + h * h);
				logPGA = -theta1 + theta2 * M - theta3 * log10(r) + theta4 * r + epsilon;
				double FDM_dPGA_dLat = (pow(10.0, logPGA)-PGA)/d;

				hypocentreLocationObject->setLatitude(hypocentreLocationObject->getLatitude()-d);
				hypocentreLocationObject->setLongitude(hypocentreLocationObject->getLongitude()+d);	
				R = hypocentreLocationObject->computeSurfaceDistance(structureLocationList[j]);	
				r = sqrt(R * R + h * h);
				logPGA = -theta1 + theta2 * M - theta3 * log10(r) + theta4 * r + epsilon;
				double FDM_dPGA_dLng = (pow(10.0, logPGA)-PGA)/d;
				hypocentreLocationObject->setLongitude(hypocentreLocationObject->getLongitude()-d);*/
			}
		}
		// Updating the value of the response
		thePGAList[i]->setCurrentValue(totalPGA,threadID);

		if (evaluateDDM) {
			thePGAGradientMap.insertMulti(theTheta1.data(), total_dPGA_dTheta1);
			thePGAGradientMap.insertMulti(theTheta2.data(), total_dPGA_dTheta2);
			thePGAGradientMap.insertMulti(theTheta3.data(), total_dPGA_dTheta3);
			thePGAGradientMap.insertMulti(theTheta4.data(), total_dPGA_dTheta4);
			thePGAGradientMap.insertMulti(theEpsilon.data(), total_dPGA_dEpsilon);

			theDDMMap.insertMulti(thePGAList[i], thePGAGradientMap);
		}
	}
	return 1;
}

bool RGenericAttenuationModel::canBeRunParallelly(){
	return true;
}