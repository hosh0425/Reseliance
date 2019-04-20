#include "RTractionElevatorDamageModel.h"	
	
#include "RParameter.h"	
#include "RResponse.h"	

#include <gsl/gsl_cdf.h>

#include <math.h>	
	
RTractionElevatorDamageModel::RTractionElevatorDamageModel(QObject *parent, QString name)	
	: RModel(parent, name)
{	
	// Initializing the data members
	//thePeakGroundAcceleration = 0;
	//theTheta1 = 0;
	//theEpsilon = 0;

	// Instantiating the response object
	theDomain->createObject(objectName() + "Response", "RGenericResponse");
	theTractionElevatorDamageRatio = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theTractionElevatorDamageRatio->setModel(this);

}	



	RTractionElevatorDamageModel::~RTractionElevatorDamageModel()	
	{	
		
	}	
		
	QObject * RTractionElevatorDamageModel::getPeakGroundAcceleration() const	
	{	
		return thePeakGroundAcceleration.data();
	}	
		
	void RTractionElevatorDamageModel::setPeakGroundAcceleration(QObject *value)	
	{	
		thePeakGroundAcceleration = qobject_cast<RParameter *>(value);
	}	
		
	QObject * RTractionElevatorDamageModel::getTheta1() const	
	{	
		return theTheta1.data();
	}	
		
	void RTractionElevatorDamageModel::setTheta1(QObject *value)	
	{	
		theTheta1 = qobject_cast<RParameter *>(value);
	}	
		
	QObject * RTractionElevatorDamageModel::getEpsilon() const	
	{	
		return theEpsilon.data();
	}	
		
	void RTractionElevatorDamageModel::setEpsilon(QObject *value)	
	{	
		theEpsilon = qobject_cast<RParameter *>(value);
	}


	QObject * RTractionElevatorDamageModel::getSigma() const
	{
		return theSigma.data();
	}

	void RTractionElevatorDamageModel::setSigma(QObject *value)
	{
		theSigma = qobject_cast<RParameter *>(value);

	}

int RTractionElevatorDamageModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int ThreadID)
{	
	// WRITE THE ALGORITHM
	double PGA = thePeakGroundAcceleration.data()->getCurrentValue();
	double Theta1 = theTheta1.data()->getCurrentValue();
	double epsilon = theEpsilon.data()->getCurrentValue();
	double sigma = theSigma.data()->getCurrentValue();


	double damageRatio = 0.0;

	damageRatio = gsl_cdf_ugaussian_P(Theta1 * (1 - 1.938 * PGA) - 2.968 * PGA + epsilon * sigma);
	
	theTractionElevatorDamageRatio->setCurrentValue(damageRatio);
	return 0;
}	

bool RTractionElevatorDamageModel::canBeRunParallelly() {	
	return false;
}