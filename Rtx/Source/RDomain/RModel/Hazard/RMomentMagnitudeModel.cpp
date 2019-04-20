#include "RMomentMagnitudeModel.h"

#include "RParameter.h"
#include "RResponse.h"

#include <math.h>

RMomentMagnitudeModel::RMomentMagnitudeModel(QObject *parent, QString name)
	: RModel(parent, name)
{
	// Initializing the pointer data members
	/* //$$ 
	theShearModulus = 0;
	theRuptureArea = 0;
	theRuptureDisplacement = 0;
	theTheta1 = 0;
	theTheta2 = 0;
	theEpsilon = 0;
	*/
	
	// Instantiating the output response theResponse1
	theDomain->createObject(objectName() + "Response", "RGenericResponse");
	// Setting theResponse1 pointer
	theMomentMagnitude = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	// Setting the "model" property of theResponse1 to be "this" object
	theMomentMagnitude->setModel(this);
}

RMomentMagnitudeModel::~RMomentMagnitudeModel()
{

}

QObject * RMomentMagnitudeModel::getShearModulus() const
{
	return theShearModulus.data();
}

void RMomentMagnitudeModel::setShearModulus(QObject *value)
{
	theShearModulus = qobject_cast<RParameter *>(value);
}

QObject * RMomentMagnitudeModel::getRuptureArea() const
{
	return theRuptureArea.data();
}

void RMomentMagnitudeModel::setRuptureArea(QObject *value)
{
	theRuptureArea = qobject_cast<RParameter *>(value);
}

QObject * RMomentMagnitudeModel::getRuptureDisplacement() const
{
	return theRuptureDisplacement.data();
}

void RMomentMagnitudeModel::setRuptureDisplacement(QObject *value)
{
	theRuptureDisplacement = qobject_cast<RParameter *>(value);
}

QObject * RMomentMagnitudeModel::getTheta1() const	
{	
	return theTheta1.data();
}	

void RMomentMagnitudeModel::setTheta1(QObject *value)	
{	
	theTheta1 = qobject_cast<RParameter *>(value);
}	

QObject * RMomentMagnitudeModel::getTheta2() const	
{	
	return theTheta2.data();
}	

void RMomentMagnitudeModel::setTheta2(QObject *value)	
{	
	theTheta2 = qobject_cast<RParameter *>(value);
}	

QObject * RMomentMagnitudeModel::getEpsilon() const	
{	
	return theEpsilon.data();
}	

void RMomentMagnitudeModel::setEpsilon(QObject *value)	
{	
	theEpsilon = qobject_cast<RParameter *>(value);
}	

QList<RParameter *> RMomentMagnitudeModel::getPhysicalParameterList()
{
	QList<RParameter *> paramList;
	paramList << theShearModulus.data() << theRuptureArea.data() << theRuptureDisplacement.data();
	return paramList;
}

int RMomentMagnitudeModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
{	
	double G = theShearModulus.data()->getCurrentValue(threadID);
	double A = theRuptureArea.data()->getCurrentValue(threadID);
	double D = theRuptureDisplacement.data()->getCurrentValue(threadID);
	double theta1 = theTheta1.data()->getCurrentValue(threadID);
	double theta2 = theTheta2.data()->getCurrentValue(threadID);
	double epsilon = theEpsilon.data()->getCurrentValue(threadID);
	double seismicMoment = G * A * D * 1e+7;
	double magnitude = theta1 * log10(seismicMoment) - theta2 + epsilon;

	// Updating the response current value
	theMomentMagnitude->setCurrentValue(magnitude,threadID);

	if (evaluateDDM) {
		double log_10 = log(10.0);
		double dMomentMagnitude_dG = theta1 / G / log_10;
		double dMomentMagnitude_dA = theta1 / A / log_10;
		double dMomentMagnitude_dD = theta1 / D / log_10;
		double dMomentMagnitude_dTheta1 = log10(seismicMoment);
		double dMomentMagnitude_dTheta2 = -1.0;
		double dMomentMagnitude_dEpsilon = 1.0;

		QMap<RParameter *, double> theMomentMagnitudeGradientMap;
		theMomentMagnitudeGradientMap.insertMulti(theShearModulus.data(), dMomentMagnitude_dG);
		theMomentMagnitudeGradientMap.insertMulti(theRuptureArea.data(), dMomentMagnitude_dA);
		theMomentMagnitudeGradientMap.insertMulti(theRuptureDisplacement.data(), dMomentMagnitude_dD);
		theMomentMagnitudeGradientMap.insertMulti(theTheta1.data(), dMomentMagnitude_dTheta1);
		theMomentMagnitudeGradientMap.insertMulti(theTheta2.data(), dMomentMagnitude_dTheta2);
		theMomentMagnitudeGradientMap.insertMulti(theEpsilon.data(), dMomentMagnitude_dEpsilon);

		theDDMMap.insertMulti(theMomentMagnitude, theMomentMagnitudeGradientMap);
	}

	return 1;
}	

bool RMomentMagnitudeModel::canBeRunParallelly(){
	return true;
}