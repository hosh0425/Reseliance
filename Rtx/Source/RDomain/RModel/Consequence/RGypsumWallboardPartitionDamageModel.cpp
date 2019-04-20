#include "RGypsumWallboardPartitionDamageModel.h"	
	
#include "RParameter.h"	
#include "RResponse.h"	

#include "math.h"	


RGypsumWallboardPartitionDamageModel::RGypsumWallboardPartitionDamageModel(QObject *parent, QString name)	
	: RModel(parent, name)
{	
	// Initializing the data members
	//theInterstoryDriftRatio = 0;
	//theTheta1 = 0;
	//theTheta2 = 0;
	//theEpsilon = 0;

	theHasReturnWall = RGypsumWallboardPartitionDamageModel::Yes;


	
	// Instantiating the response object
	theDomain->createObject(objectName() + "Response", "RGenericResponse");
	theDamageRatio = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theDamageRatio->setModel(this);
}	


	RGypsumWallboardPartitionDamageModel::~RGypsumWallboardPartitionDamageModel()	
	{	
		
	}	
		
	QObject * RGypsumWallboardPartitionDamageModel::getInterstoryDriftRatio() const	
	{	
		return theInterstoryDriftRatio.data();
	}	
		
	void RGypsumWallboardPartitionDamageModel::setInterstoryDriftRatio(QObject *value)	
	{	
		theInterstoryDriftRatio = qobject_cast<RParameter *>(value);
	}	
		
	RGypsumWallboardPartitionDamageModel::RHasReturnWall RGypsumWallboardPartitionDamageModel::getHasReturnWall() const	
	{	
		return theHasReturnWall;
	}	
	
	void RGypsumWallboardPartitionDamageModel::setHasReturnWall(RGypsumWallboardPartitionDamageModel::RHasReturnWall value)	
	{	
		theHasReturnWall = value;
	}	
		
	QObject * RGypsumWallboardPartitionDamageModel::getTheta1() const	
	{	
		return theTheta1.data();
	}	
		
	void RGypsumWallboardPartitionDamageModel::setTheta1(QObject *value)	
	{	
		theTheta1 = qobject_cast<RParameter *>(value);
	}	
		
	QObject * RGypsumWallboardPartitionDamageModel::getTheta2() const	
	{	
		return theTheta2.data();
	}	
		
	void RGypsumWallboardPartitionDamageModel::setTheta2(QObject *value)	
	{	
		theTheta2 = qobject_cast<RParameter *>(value);
	}	
		
	QObject * RGypsumWallboardPartitionDamageModel::getEpsilon() const	
	{	
		return theEpsilon.data();
	}	
		
	void RGypsumWallboardPartitionDamageModel::setEpsilon(QObject *value)	
	{	
		theEpsilon = qobject_cast<RParameter *>(value);
	}

	QObject * RGypsumWallboardPartitionDamageModel::getSigma() const
	{
		return theSigma.data();
	}

	void RGypsumWallboardPartitionDamageModel::setSigma(QObject *value)
	{
		theSigma = qobject_cast<RParameter *>(value);
	}


int RGypsumWallboardPartitionDamageModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int ThreadID)
{		
	double InterstoryDriftRatio = theInterstoryDriftRatio.data()->getCurrentValue();
	double theta1 = theTheta1.data()->getCurrentValue();
	double theta2 = theTheta2.data()->getCurrentValue();
	double epsilon = theEpsilon.data()->getCurrentValue();
	double sigma = theSigma.data()->getCurrentValue();
	double alaki = theHasReturnWall;

	double damageRatio = 0.0;
	//TRY39_1
	damageRatio = 1 - exp(-1 * exp(theta1 * theHasReturnWall + theta2 * (log10(InterstoryDriftRatio) + 2.26) - 1.47 + epsilon*sigma));

	theDamageRatio->setCurrentValue(damageRatio);
	return 0;
}	

bool RGypsumWallboardPartitionDamageModel::canBeRunParallelly() {
	return false;
}