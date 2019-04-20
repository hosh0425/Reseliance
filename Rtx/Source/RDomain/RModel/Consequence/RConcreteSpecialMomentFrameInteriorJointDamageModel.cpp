#include "RConcreteSpecialMomentFrameInteriorJointDamageModel.h"	
	
#include "RParameter.h"	
#include "RResponse.h"	
	
#include <math.h>	

#include <gsl/gsl_cdf.h>
	
RConcreteSpecialMomentFrameInteriorJointDamageModel::RConcreteSpecialMomentFrameInteriorJointDamageModel(QObject *parent, QString name)	
	: RModel(parent, name)
{	
	// Initializing the data members
	//theInterstoryDriftRatio = 0;
	//theBeamDepth = 0;
	//theBeamLength = 0;
	//theTheta1 = 0;
	//theEpsilon = 0;



	// Instantiating the response object
	theDomain->createObject(objectName() + "Response", "RGenericResponse");
	theRCSMFDamageRatio = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theRCSMFDamageRatio->setModel(this);
}	




	RConcreteSpecialMomentFrameInteriorJointDamageModel::~RConcreteSpecialMomentFrameInteriorJointDamageModel()	
	{	
		
	}	
		
	QObject * RConcreteSpecialMomentFrameInteriorJointDamageModel::getInterstoryDriftRatio() const	
	{	
		return theInterstoryDriftRatio.data();
	}	
		
	void RConcreteSpecialMomentFrameInteriorJointDamageModel::setInterstoryDriftRatio(QObject *value)	
	{	
		theInterstoryDriftRatio = qobject_cast<RParameter *>(value);
	}	
		
	QObject * RConcreteSpecialMomentFrameInteriorJointDamageModel::getBeamDepth() const	
	{	
		return theBeamDepth.data();
	}	
		
	void RConcreteSpecialMomentFrameInteriorJointDamageModel::setBeamDepth(QObject *value)	
	{	
		theBeamDepth = qobject_cast<RParameter *>(value);
	}	
		
	QObject * RConcreteSpecialMomentFrameInteriorJointDamageModel::getBeamLength() const	
	{	
		return theBeamLength.data();
	}	
		
	void RConcreteSpecialMomentFrameInteriorJointDamageModel::setBeamLength(QObject *value)	
	{	
		theBeamLength = qobject_cast<RParameter *>(value);
	}	
		
	QObject * RConcreteSpecialMomentFrameInteriorJointDamageModel::getTheta1() const	
	{	
		return theTheta1.data();
	}	
		
	void RConcreteSpecialMomentFrameInteriorJointDamageModel::setTheta1(QObject *value)	
	{	
		theTheta1 = qobject_cast<RParameter *>(value);
	}	
		
	QObject * RConcreteSpecialMomentFrameInteriorJointDamageModel::getEpsilon() const	
	{	
		return theEpsilon.data();
	}	
		
	void RConcreteSpecialMomentFrameInteriorJointDamageModel::setEpsilon(QObject *value)	
	{	
		theEpsilon = qobject_cast<RParameter *>(value);
	}	

	QObject * RConcreteSpecialMomentFrameInteriorJointDamageModel::getSigma() const
	{
		return theSigma.data();
	}

	void RConcreteSpecialMomentFrameInteriorJointDamageModel::setSigma(QObject *value)
	{
		theSigma = qobject_cast<RParameter *>(value);
	}




	int RConcreteSpecialMomentFrameInteriorJointDamageModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int ThreadID)
	{	
	double InterstoryDrift = theInterstoryDriftRatio.data()->getCurrentValue();
	double theta1 = theTheta1.data()->getCurrentValue();
	double epsilon = theEpsilon.data()->getCurrentValue();
	double sigma = theSigma.data()->getCurrentValue();
	double beamDepth = theBeamDepth.data()->getCurrentValue();
	double beamLength = theBeamLength.data()->getCurrentValue();

	double damageRatio = 0.0;
	//Model SMF 50
	damageRatio = gsl_cdf_ugaussian_P(theta1 * (beamDepth/beamLength + 0.0722 * (log(InterstoryDrift) - 1.49 * log(beamDepth/beamLength))) + 
		0.52 * log(beamDepth/beamLength) + 0.02 * (log(InterstoryDrift) - 1.49 * log(beamDepth/beamLength)) + epsilon * sigma);
	
	theRCSMFDamageRatio->setCurrentValue(damageRatio);
	return 0;
	}	

	bool RConcreteSpecialMomentFrameInteriorJointDamageModel::canBeRunParallelly() {
		return false;
	}