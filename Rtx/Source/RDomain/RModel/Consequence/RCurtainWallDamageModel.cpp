#include "RCurtainWallDamageModel.h"	
	
#include "RParameter.h"	
#include "RResponse.h"	
	
#include <math.h>	

#include <gsl/gsl_cdf.h>
	
RCurtainWallDamageModel::RCurtainWallDamageModel(QObject *parent, QString name)	
	: RModel(parent, name)
{	
	// Initializing the data members
	//theInterstoryDriftRatio = 0;
	//theGlassToFrameClearance = 0;
	//theTheta1 = 0;
	//theEpsilon = 0;

	theGlassType = RCurtainWallDamageModel::monolithic;
	theLaminatedGlass = RCurtainWallDamageModel::Yes;



	
		// Instantiating the response object
		theDomain->createObject(objectName() + "Response", "RGenericResponse");
		theRCurtainWallDamageRatio = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
		theRCurtainWallDamageRatio->setModel(this);
	}	
		
	RCurtainWallDamageModel::~RCurtainWallDamageModel()	
	{	
		
	}	
		
	QObject * RCurtainWallDamageModel::getInterstoryDriftRatio() const	
	{	
		return theInterstoryDriftRatio.data();
	}	
		
	void RCurtainWallDamageModel::setInterstoryDriftRatio(QObject *value)	
	{	
		theInterstoryDriftRatio = qobject_cast<RParameter *>(value);
	}

	QObject * RCurtainWallDamageModel::getGlassToFrameClearance() const	
	{	
		return theGlassToFrameClearance.data();
	}	
		
	void RCurtainWallDamageModel::setGlassToFrameClearance(QObject *value)	
	{	
		theGlassToFrameClearance = qobject_cast<RParameter *>(value);
	}	
		
	RCurtainWallDamageModel::RGlassType RCurtainWallDamageModel::getGlassType() const	
	{	
		return theGlassType;
	}	
	
	void RCurtainWallDamageModel::setGlassType(RCurtainWallDamageModel::RGlassType value)	
	{	
		theGlassType = value;
	}	
		
	RCurtainWallDamageModel::RLaminatedGlass RCurtainWallDamageModel::getLaminatedGlass() const	
	{	
		return theLaminatedGlass;
	}	
	
	void RCurtainWallDamageModel::setLaminatedGlass(RCurtainWallDamageModel::RLaminatedGlass value)	
	{	
		theLaminatedGlass = value;
	}	
		
	QObject * RCurtainWallDamageModel::getTheta1() const	
	{	
		return theTheta1.data();
	}	
		
	void RCurtainWallDamageModel::setTheta1(QObject *value)	
	{	
		theTheta1 = qobject_cast<RParameter *>(value);
	}	
		
	QObject * RCurtainWallDamageModel::getEpsilon() const	
	{	
		return theEpsilon.data();
	}	
		
	void RCurtainWallDamageModel::setEpsilon(QObject *value)	
	{	
		theEpsilon = qobject_cast<RParameter *>(value);
	}	


	QObject * RCurtainWallDamageModel::getSigma() const
	{
		return theSigma.data();
	}

	void RCurtainWallDamageModel::setSigma(QObject *value)
	{
		theSigma = qobject_cast<RParameter *>(value);
	}



	int RCurtainWallDamageModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int ThreadID)
{	
	double InterstoryDriftRatio = theInterstoryDriftRatio.data()->getCurrentValue();
	double theta1 = theTheta1.data()->getCurrentValue();
	double epsilon = theEpsilon.data()->getCurrentValue();
	double sigma = theSigma.data()->getCurrentValue();

	double glassToFrameClearance = theGlassToFrameClearance.data()->getCurrentValue();
	int IGU = 0;
	int aIGU = 0;
	int mono = 0;
	int lam = 0;
	
	if(theLaminatedGlass==1) {
		lam = 1;
	}

	if(theGlassType==0) {
		mono = 1;
	}
	else if(theGlassType==1) {
		IGU = 1;
	}
	else {
		aIGU = 1;
	}


	double damageRatio = 0.0;
	//try 13_5
	damageRatio = gsl_cdf_ugaussian_P(theta1 * (lam-0.688*(log(InterstoryDriftRatio)+
		6.393*(mono+1.0214*(aIGU+0.812*(IGU-0.62*(log(1+glassToFrameClearance)-0.1727*glassToFrameClearance))))))
		- 0.007*glassToFrameClearance+4.117*(log(1+glassToFrameClearance)-0.17276*glassToFrameClearance) 
		+ 0.12*(aIGU+0.812*(IGU-0.62*(log(1+glassToFrameClearance)-0.17276*glassToFrameClearance))) 
		- 0.825*(mono+1.0214*(aIGU+0.812*(IGU-0.62*(log(1+glassToFrameClearance)-0.17276*glassToFrameClearance)))) 
		- 0.47*(log(InterstoryDriftRatio)+6.393*(mono+1.0214*(aIGU+0.812*(IGU-0.62*(log(1+glassToFrameClearance)
		-0.17276*glassToFrameClearance)))))+ epsilon * sigma);

	theRCurtainWallDamageRatio->setCurrentValue(damageRatio);
	
	return 0;
}	



bool RCurtainWallDamageModel::canBeRunParallelly() {	
	return false;
}