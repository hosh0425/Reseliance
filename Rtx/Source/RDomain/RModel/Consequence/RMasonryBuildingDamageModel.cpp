#include "RMasonryBuildingDamageModel.h" 

#include "RParameter.h"
#include "RResponse.h"
#include "RBuildingInformationModel.h"

#include <QString>
#include <gsl/gsl_cdf.h>
#include <gsl/gsl_sf.h>

#include <math.h>


RMasonryBuildingDamageModel::RMasonryBuildingDamageModel(QObject *parent, QString name)
	: RModel(parent,name)
{
	theWithTieColumn = false;
	theRigidDiaphragm = false;


	theDomain->createObject(objectName() + "MDResponse", "RGenericResponse");
	theMasonryDamageResponse = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theMasonryDamageResponse->setModel(this);

}
RMasonryBuildingDamageModel::~RMasonryBuildingDamageModel() 
{

}

bool RMasonryBuildingDamageModel::getWithTieColumn() const
{
	return theWithTieColumn;
}
void RMasonryBuildingDamageModel::setWithTieColumn(bool value)
{
	theWithTieColumn = value;
}

bool RMasonryBuildingDamageModel::getRigidDiaphragm() const
{
	return theRigidDiaphragm;
}
void RMasonryBuildingDamageModel::setRigidDiaphragm(bool value)
{
	theRigidDiaphragm = value;
}

QObject * RMasonryBuildingDamageModel::getBuildingInformationModel() const 
{
	return theBuildingInformationModel.data();
} 
void RMasonryBuildingDamageModel::setBuildingInformationModel(QObject *value) 
{
	theBuildingInformationModel = qobject_cast<RModel *>(value); 
}

QObject * RMasonryBuildingDamageModel::getDemandPGA() const 
{
	return theDemandPGA.data();
} 
void RMasonryBuildingDamageModel::setDemandPGA(QObject *value) 
{
	theDemandPGA = qobject_cast<RParameter *>(value); 
}

QObject * RMasonryBuildingDamageModel::getDesignBaseShearCoefficient() const 
{
	return theDesignBaseShearCoefficient.data();
} 
void RMasonryBuildingDamageModel::setDesignBaseShearCoefficient(QObject *value) 
{
	theDesignBaseShearCoefficient = qobject_cast<RParameter *>(value); 
}

QObject * RMasonryBuildingDamageModel::getTheta1() const 
{
	return theTheta1.data();
} 
void RMasonryBuildingDamageModel::setTheta1(QObject *value) 
{
	theTheta1 = qobject_cast<RParameter *>(value); 
}

QObject * RMasonryBuildingDamageModel::getTheta2() const 
{
	return theTheta2.data();
} 
void RMasonryBuildingDamageModel::setTheta2(QObject *value) 
{
	theTheta2 = qobject_cast<RParameter *>(value); 
}

QObject * RMasonryBuildingDamageModel::getTheta3() const 
{
	return theTheta3.data();
} 
void RMasonryBuildingDamageModel::setTheta3(QObject *value) 
{
	theTheta3 = qobject_cast<RParameter *>(value); 
}

QObject * RMasonryBuildingDamageModel::getTheta4() const 
{
	return theTheta4.data();
} 
void RMasonryBuildingDamageModel::setTheta4(QObject *value) 
{
	theTheta4 = qobject_cast<RParameter *>(value); 
}

QObject * RMasonryBuildingDamageModel::getTheta5() const 
{
	return theTheta5.data();
} 
void RMasonryBuildingDamageModel::setTheta5(QObject *value) 
{
	theTheta5 = qobject_cast<RParameter *>(value); 
}

QObject * RMasonryBuildingDamageModel::getTheta6() const 
{
	return theTheta6.data();
} 
void RMasonryBuildingDamageModel::setTheta6(QObject *value) 
{
	theTheta6 = qobject_cast<RParameter *>(value); 
}

QObject * RMasonryBuildingDamageModel::getEpsilon() const 
{
	return theEpsilon.data();
} 
void RMasonryBuildingDamageModel::setEpsilon(QObject *value) 
{
	theEpsilon = qobject_cast<RParameter *>(value); 
}

QObject * RMasonryBuildingDamageModel::getSigma() const 
{
	return theSigma.data();
} 
void RMasonryBuildingDamageModel::setSigma(QObject *value) 
{
	theSigma = qobject_cast<RParameter *>(value); 
}

int RMasonryBuildingDamageModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
{
	// Looking up Masonry type and diaphragm type
	int M = 1.0 - getWithTieColumn();
	int D = getRigidDiaphragm();

	// Getting the information from the BIM
	RBuildingInformationModel *theBIM = qobject_cast<RBuildingInformationModel *>(theBuildingInformationModel.data());
	if (!theBIM) 
	{
		rCritical(threadID) << "Error in model" << objectName() << ":" << theBuildingInformationModel.data()->objectName() << "is not a building information model.";
		return -1;
	}
	
	// Looking up the plan irregularity
	int Ip = 0;
	if (theBIM -> getPlanIrregularity()) 
	{
		Ip = 1.0;
	}

	// Looking up the number pf stories
	QVector<double> noStoreys = theBIM->getNumberOfStoreysVector();
	double N = noStoreys[0];

	// Looking up the demand PGA
	double PGA = gsl_sf_log(theDemandPGA.data()->getCurrentValue(threadID));

	// Looking up base shear coefficient and calculating its cumulative standar normal distribution
	double BSC =  gsl_cdf_gaussian_P(theDesignBaseShearCoefficient.data() -> getCurrentValue(threadID),1.0);

	// Looking up model parameters
	double theta1 = theTheta1.data()->getCurrentValue(threadID);
	double theta2 = theTheta2.data()->getCurrentValue(threadID);
	double theta3 = theTheta3.data()->getCurrentValue(threadID);
	double theta4 = theTheta4.data()->getCurrentValue(threadID);
	double theta5 = theTheta5.data()->getCurrentValue(threadID);
	double theta6 = theTheta6.data()->getCurrentValue(threadID);

	// Looking up the model error parameters
	double sigma = theSigma.data()->getCurrentValue(threadID);
	double epsilon = theEpsilon.data()->getCurrentValue(threadID) * sigma;

	//calculating damage ratio
	double StructuralDamageRatio = 0;
	StructuralDamageRatio = gsl_cdf_gaussian_P(theta1*M + theta2*Ip + theta3*D + theta4*N + theta5*PGA + theta6*BSC + epsilon, 1.0);

	theMasonryDamageResponse -> setCurrentValue(StructuralDamageRatio, threadID);

	if (evaluateDDM)
	{
		// IMPLEMENT
	}
	
	return 0;
}

bool RMasonryBuildingDamageModel::canBeRunParallelly(){
	return true;
}