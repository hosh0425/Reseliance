#include "RSuspendedCeilingDamageModel.h"	
	
#include "RParameter.h"	
#include "RResponse.h"	

#include <gsl/gsl_cdf.h>
	
#include <math.h>	
	
RSuspendedCeilingDamageModel::RSuspendedCeilingDamageModel(QObject *parent, QString name)	
	: RModel(parent, name)
{	
	// Initializing the data members
	//thePeakFloorAcceleration = 0;
	//theCeilingArea = 0;
	//theTheta1 = 0;
	//theTheta2 = 0;
	//theEpsilon = 0;

	theHasLateralSupport = RSuspendedCeilingDamageModel::Yes;
	
	// Instantiating the response object
	theDomain->createObject(objectName() + "Response", "RGenericResponse");
	theCeilingDamageRatio = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theCeilingDamageRatio->setModel(this);
}

	RSuspendedCeilingDamageModel::~RSuspendedCeilingDamageModel()	
	{	
		
	}	
		
	QObject * RSuspendedCeilingDamageModel::getPeakFloorAcceleration() const	
	{	
		return thePeakFloorAcceleration.data();
	}	
		
	void RSuspendedCeilingDamageModel::setPeakFloorAcceleration(QObject *value)	
	{	
		thePeakFloorAcceleration = qobject_cast<RParameter *>(value);
	}	
		
	double RSuspendedCeilingDamageModel::getCeilingArea() const	
	{	
		return theCeilingArea;
	}	
		
	void RSuspendedCeilingDamageModel::setCeilingArea(double value)	
	{	
		theCeilingArea = value;
	}	
		
		RSuspendedCeilingDamageModel::RHasLateralSupport RSuspendedCeilingDamageModel::getHasLateralSupport() const	
	{	
		return theHasLateralSupport;
	}	
	
	void RSuspendedCeilingDamageModel::setHasLateralSupport(RSuspendedCeilingDamageModel::RHasLateralSupport value)	
	{	
		theHasLateralSupport = value;
	}	
		
	QObject * RSuspendedCeilingDamageModel::getTheta1() const	
	{	
		return theTheta1.data();
	}	
		
	void RSuspendedCeilingDamageModel::setTheta1(QObject *value)	
	{	
		theTheta1 = qobject_cast<RParameter *>(value);
	}	
		
	QObject * RSuspendedCeilingDamageModel::getTheta2() const	
	{	
		return theTheta2.data();
	}	
		
	void RSuspendedCeilingDamageModel::setTheta2(QObject *value)	
	{	
		theTheta2 = qobject_cast<RParameter *>(value);
	}	
		
	QObject * RSuspendedCeilingDamageModel::getEpsilon() const	
	{	
		return theEpsilon.data();
	}	
		
	void RSuspendedCeilingDamageModel::setEpsilon(QObject *value)	
	{	
		theEpsilon = qobject_cast<RParameter *>(value);
	}	

	QObject * RSuspendedCeilingDamageModel::getSigma() const
	{
		return theSigma.data();
	}

	void RSuspendedCeilingDamageModel::setSigma(QObject *value)
	{
		theSigma = qobject_cast<RParameter *>(value);
	}


int RSuspendedCeilingDamageModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int ThreadID)
{	

			
	double peakFloorAcc = thePeakFloorAcceleration.data()->getCurrentValue();
	double CeilingArea = theCeilingArea;
	double theta1 = theTheta1.data()->getCurrentValue();
	double theta2 = theTheta2.data()->getCurrentValue();
	double epsilon = theEpsilon.data()->getCurrentValue();
	double sigma = theSigma.data()->getCurrentValue();

	
	double damageRatio = 0.0;
	//Ceiling Model 14
	damageRatio = gsl_cdf_ugaussian_P(theta1 * (log(peakFloorAcc) + 0.50714 * (log(CeilingArea) - 6.544)) + theta2 * theHasLateralSupport + 0.5655 -
		0.0207 * (log(CeilingArea)-6.544) + epsilon * sigma);

	theCeilingDamageRatio->setCurrentValue(damageRatio);
	return 0;
}	

bool RSuspendedCeilingDamageModel::canBeRunParallelly() {	
	return false;
}