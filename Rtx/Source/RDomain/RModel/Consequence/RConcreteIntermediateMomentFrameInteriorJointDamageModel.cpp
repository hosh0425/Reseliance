#include "RConcreteIntermediateMomentFrameInteriorJointDamageModel.h"	
	
#include "RParameter.h"	
#include "RResponse.h"	
	
#include <math.h>	

#include <gsl/gsl_cdf.h>
	
RConcreteIntermediateMomentFrameInteriorJointDamageModel::RConcreteIntermediateMomentFrameInteriorJointDamageModel(QObject *parent, QString name)	
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
	theRCIMFDamageRatio = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theRCIMFDamageRatio->setModel(this);
}	




	RConcreteIntermediateMomentFrameInteriorJointDamageModel::~RConcreteIntermediateMomentFrameInteriorJointDamageModel()	
	{	
		
	}	
		
	QObject * RConcreteIntermediateMomentFrameInteriorJointDamageModel::getInterstoryDriftRatio() const	
	{	
		return theInterstoryDriftRatio.data();
	}	
		
	void RConcreteIntermediateMomentFrameInteriorJointDamageModel::setInterstoryDriftRatio(QObject *value)	
	{	
		theInterstoryDriftRatio = qobject_cast<RParameter *>(value);
	}	
		
	QObject * RConcreteIntermediateMomentFrameInteriorJointDamageModel::getBeamDepth() const	
	{	
		return theBeamDepth.data();
	}	
		
	void RConcreteIntermediateMomentFrameInteriorJointDamageModel::setBeamDepth(QObject *value)	
	{	
		theBeamDepth = qobject_cast<RParameter *>(value);
	}	
		
	QObject * RConcreteIntermediateMomentFrameInteriorJointDamageModel::getBeamLength() const	
	{	
		return theBeamLength.data();
	}	
		
	void RConcreteIntermediateMomentFrameInteriorJointDamageModel::setBeamLength(QObject *value)	
	{	
		theBeamLength = qobject_cast<RParameter *>(value);
	}	
		
	QObject * RConcreteIntermediateMomentFrameInteriorJointDamageModel::getTheta1() const	
	{	
		return theTheta1.data();
	}	
		
	void RConcreteIntermediateMomentFrameInteriorJointDamageModel::setTheta1(QObject *value)	
	{	
		theTheta1 = qobject_cast<RParameter *>(value);
	}	
		
	QObject * RConcreteIntermediateMomentFrameInteriorJointDamageModel::getEpsilon() const	
	{	
		return theEpsilon.data();
	}	
		
	void RConcreteIntermediateMomentFrameInteriorJointDamageModel::setEpsilon(QObject *value)	
	{	
		theEpsilon = qobject_cast<RParameter *>(value);
	}	

	QObject * RConcreteIntermediateMomentFrameInteriorJointDamageModel::getSigma() const
	{
		return theSigma.data();
	}

	void RConcreteIntermediateMomentFrameInteriorJointDamageModel::setSigma(QObject *value)
	{
		theSigma = qobject_cast<RParameter *>(value);
	}


	int RConcreteIntermediateMomentFrameInteriorJointDamageModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int ThreadID)
	{	
	double InterstoryDrift = theInterstoryDriftRatio.data()->getCurrentValue();
	double theta1 = theTheta1.data()->getCurrentValue();
	double epsilon = theEpsilon.data()->getCurrentValue();
	double sigma = theSigma.data()->getCurrentValue();
	double beamDepth = theBeamDepth.data()->getCurrentValue();
	double beamLength = theBeamLength.data()->getCurrentValue();

	double damageRatio = 0.0;
	//Model IMF 43
	damageRatio = gsl_cdf_ugaussian_P(theta1 * (log(InterstoryDrift) - 4.48 * (log(beamDepth/beamLength) + 9.055 * (beamDepth/beamLength))) + 
		21.36 * (beamDepth/beamLength) + 3.3 * (log(beamDepth/beamLength) + 9.055 * (beamDepth/beamLength)) + epsilon * sigma);
	
	theRCIMFDamageRatio->setCurrentValue(damageRatio);
	return 0;
	}	


	bool RConcreteIntermediateMomentFrameInteriorJointDamageModel::canBeRunParallelly() {
		return false;
	}