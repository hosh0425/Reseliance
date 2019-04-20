#include "RSimpleDamageCurveModel.h"	
	
#include "RParameter.h"	
#include "RResponse.h"	
#include "RLocation.h"

#include "math.h"	

#include <gsl/gsl_math.h>

RSimpleDamageCurveModel::RSimpleDamageCurveModel(QObject *parent, QString name)	
	: RModel(parent, name)
{	
	// Initializing the pointer data members
	/* //$$ 
	theIntensity = 0;
	theMinIntensity = 0;
	theMaxIntensity = 0;
	theEpsilon = 0;
	*/
	
	// Instantiating the response object
	theDomain->createObject(objectName() + "Response", "RGenericResponse");
	theDamageFactor = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theDamageFactor->setModel(this);
}	

RSimpleDamageCurveModel::~RSimpleDamageCurveModel()	
{	
	
}	

QObject * RSimpleDamageCurveModel::getIntensity() const	
{	
	return theIntensity.data();
}	

void RSimpleDamageCurveModel::setIntensity(QObject *value)	
{	
	theIntensity = qobject_cast<RParameter *>(value);
}	

QObject * RSimpleDamageCurveModel::getMinIntensity() const	
{	
	return theMinIntensity.data();
}	

void RSimpleDamageCurveModel::setMinIntensity(QObject *value)	
{	
	theMinIntensity = qobject_cast<RParameter *>(value);
}	

QObject * RSimpleDamageCurveModel::getMaxIntensity() const	
{	
	return theMaxIntensity.data();
}	

void RSimpleDamageCurveModel::setMaxIntensity(QObject *value)	
{	
	theMaxIntensity = qobject_cast<RParameter *>(value);
}

QObject * RSimpleDamageCurveModel::getEpsilon() const
{
	return theEpsilon.data();
}

void RSimpleDamageCurveModel::setEpsilon(QObject *value)
{
	theEpsilon = qobject_cast<RParameter *>(value);
}

int RSimpleDamageCurveModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)	
{		
	double intensity = theIntensity.data()->getCurrentValue(threadID);
	double minIntensity = theMinIntensity.data()->getCurrentValue(threadID);
	double Epsilon = theEpsilon.data()->getCurrentValue(threadID);
	double maxIntensity = theMaxIntensity.data()->getCurrentValue(threadID) * Epsilon;
	double maxDamage = 1.0;
	double damageFactor;

	if (intensity < minIntensity) {
		damageFactor = 0.0;
	} else if (intensity > maxIntensity) {
		damageFactor = maxDamage;
	} else {
		double a = M_PI / (maxIntensity - minIntensity);
		double b = - M_PI / (maxIntensity - minIntensity) * minIntensity - 0.5 * M_PI;
		damageFactor = 0.5 * maxDamage * sin(a * intensity + b) + 0.5 * maxDamage;
	}

	theDamageFactor->setCurrentValue(damageFactor,threadID);

	if (evaluateDDM) {
		double AA = maxIntensity - minIntensity;
		double BB = 0.5 * maxDamage * M_PI * cos(M_PI * ((intensity - minIntensity) / AA - 0.5));

		double dDF_dIntensity = BB / AA;
		double dDF_dMinIntensity = BB * (intensity - maxIntensity) / (AA * AA);
		double dDF_dMaxIntensity = BB * Epsilon * (minIntensity - intensity) / (AA * AA);
		double dDF_dEpsilon = BB * (maxIntensity / Epsilon) * (minIntensity - intensity) / (AA * AA);

		QMap<RParameter *, double> theDFGradientMap;
		theDFGradientMap.insertMulti(theIntensity.data(), dDF_dIntensity);
		theDFGradientMap.insertMulti(theMinIntensity.data(), dDF_dMinIntensity);
		theDFGradientMap.insertMulti(theMaxIntensity.data(), dDF_dMaxIntensity);
		theDFGradientMap.insertMulti(theEpsilon.data(), dDF_dEpsilon);

		theDDMMap.insertMulti(theDamageFactor, theDFGradientMap);
	}

	return 1;
}	

bool RSimpleDamageCurveModel::canBeRunParallelly(){
	return true;
}