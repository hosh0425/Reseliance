#include "RConcreteMomentFrameExteriorJointDamageModel.h"	
	
#include "RParameter.h"	
#include "RResponse.h"	

#include <gsl/gsl_cdf.h>
	
#include <math.h>	
	
RConcreteMomentFrameExteriorJointDamageModel::RConcreteMomentFrameExteriorJointDamageModel(QObject *parent, QString name)	
	: RModel(parent, name)
{	
	// Initializing the data members
	//theInterstoryDriftRatio = 0;
	//theTheta1 = 0;
	//theEpsilon = 0;


	
	// Instantiating the response object
	theDomain->createObject(objectName() + "Response", "RGenericResponse");
	theExteriorJointDamageRatio = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theExteriorJointDamageRatio->setModel(this);
}	



	RConcreteMomentFrameExteriorJointDamageModel::~RConcreteMomentFrameExteriorJointDamageModel()	
	{	
		
	}	
		
	QObject * RConcreteMomentFrameExteriorJointDamageModel::getInterstoryDriftRatio() const	
	{	
		return theInterstoryDriftRatio.data();
	}	
		
	void RConcreteMomentFrameExteriorJointDamageModel::setInterstoryDriftRatio(QObject *value)	
	{	
		theInterstoryDriftRatio = qobject_cast<RParameter *>(value);
	}	
		
	QObject * RConcreteMomentFrameExteriorJointDamageModel::getTheta1() const	
	{	
		return theTheta1.data();
	}	
		
	void RConcreteMomentFrameExteriorJointDamageModel::setTheta1(QObject *value)	
	{	
		theTheta1 = qobject_cast<RParameter *>(value);
	}	
		
	QObject * RConcreteMomentFrameExteriorJointDamageModel::getEpsilon() const	
	{	
		return theEpsilon.data();
	}	
		
	void RConcreteMomentFrameExteriorJointDamageModel::setEpsilon(QObject *value)	
	{	
		theEpsilon = qobject_cast<RParameter *>(value);
	}	

	QObject * RConcreteMomentFrameExteriorJointDamageModel::getSigma() const
	{
		return theSigma.data();
	}

	void RConcreteMomentFrameExteriorJointDamageModel::setSigma(QObject *value)
	{
		theSigma = qobject_cast<RParameter *>(value);
	}



int RConcreteMomentFrameExteriorJointDamageModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int ThreadID)
{	

			
	double InterstoryDriftR = theInterstoryDriftRatio.data()->getCurrentValue();
	double theta1 = theTheta1.data()->getCurrentValue();
	double epsilon = theEpsilon.data()->getCurrentValue();
	double sigma = theSigma.data()->getCurrentValue();

	
	double damageRatio = 0.0;

	damageRatio = gsl_cdf_ugaussian_P(theta1 * (log(InterstoryDriftR) + 3.83) - 0.67 + epsilon*sigma);

	theExteriorJointDamageRatio->setCurrentValue(damageRatio);
	return 0;
}	


bool RConcreteMomentFrameExteriorJointDamageModel::canBeRunParallelly() {	
	return false;
}