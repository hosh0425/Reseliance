#include "RSimpleWindPressureModel.h"

#include "RParameter.h"
#include "RResponse.h"
#include "RContinuousRandomVariable.h"

#include <gsl/gsl_randist.h>
#include <gsl/gsl_cdf.h>

#include <math.h>

RSimpleWindPressureModel::RSimpleWindPressureModel(QObject *parent, QString name)
	: RModel(parent, name)
{
	// Initializing the data members
	/* //$$ 
	theVelocity = 0;
	theAirDensity = 0;
	theExposureFactor = 0;
	theExternalPressureFactor = 0;
	theGustFactor = 0;
	theEpsilon = 0;
	*/
	
	// Instantiating the response object
	theDomain->createObject(objectName() + "Response", "RGenericResponse");
	theWindPressure = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theWindPressure->setModel(this);
}

RSimpleWindPressureModel::~RSimpleWindPressureModel()
{

}
	
QObject * RSimpleWindPressureModel::getVelocity() const	
{	
	return theVelocity.data();
}	
	
void RSimpleWindPressureModel::setVelocity(QObject *value)	
{	
	theVelocity = qobject_cast<RParameter *>(value);
}	
	
QObject * RSimpleWindPressureModel::getAirDensity() const	
{	
	return theAirDensity.data();
}	
	
void RSimpleWindPressureModel::setAirDensity(QObject *value)	
{	
	theAirDensity = qobject_cast<RParameter *>(value);
}	

QObject * RSimpleWindPressureModel::getExposureFactor() const	
{	
	return theExposureFactor.data();
}	
	
void RSimpleWindPressureModel::setExposureFactor(QObject *value)	
{	
	theExposureFactor = qobject_cast<RParameter *>(value);
}	
	
QObject * RSimpleWindPressureModel::getExternalPressureFactor() const	
{	
	return theExternalPressureFactor.data();
}	
	
void RSimpleWindPressureModel::setExternalPressureFactor(QObject *value)	
{	
	theExternalPressureFactor = qobject_cast<RParameter *>(value);
}	
	
QObject * RSimpleWindPressureModel::getGustFactor() const	
{	
	return theGustFactor.data();
}	
	
void RSimpleWindPressureModel::setGustFactor(QObject *value)	
{	
	theGustFactor = qobject_cast<RParameter *>(value);
}	
	
QObject * RSimpleWindPressureModel::getEpsilon() const	
{	
	return theEpsilon.data();
}	
	
void RSimpleWindPressureModel::setEpsilon(QObject *value)	
{	
	theEpsilon = qobject_cast<RParameter *>(value);
}	

int RSimpleWindPressureModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
{	
	// Formulation from:  Bartlett, F M.; Hong, H P.; Zhou, W, "Load factor calibration for the proposed 2005 edition of the National Building Code of Canada: Statistics of loads and load effects", Canadian Journal of Civil Engineering, Volume 30, Number 2, 1 April 2003 , pp. 429-439(11).
	double velocity = theVelocity.data()->getCurrentValue(threadID);

	double roh = theAirDensity.data()->getCurrentValue(threadID);
	double exposureFactor = theExposureFactor.data()->getCurrentValue(threadID);
	double externalPressureFactor = theExternalPressureFactor.data()->getCurrentValue(threadID);
	double gustFactor = theGustFactor.data()->getCurrentValue(threadID);
	double epsilon = theEpsilon.data()->getCurrentValue(threadID);

	double q = 0.5 * roh * velocity * velocity;
	double windPressure = q * exposureFactor * externalPressureFactor * gustFactor;
	windPressure *= epsilon;

	//ATTENTION: SHOULD MAKE SURE THAT A ZERO MAGNITUDE (VELOCITY) RESULTS IN A ZERO INTENSITY (PRESSURE). IN THIS CASE, THIS RELATION IS IN PLACE.

	theWindPressure->setCurrentValue(windPressure,threadID);

	return 0;
}

bool RSimpleWindPressureModel::canBeRunParallelly(){
	return true;
}