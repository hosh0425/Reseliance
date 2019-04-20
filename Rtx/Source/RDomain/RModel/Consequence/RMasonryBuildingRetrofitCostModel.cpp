#include "RMasonryBuildingRetrofitCostModel.h" 

#include "RParameter.h"
#include "RResponse.h"
#include "RBuildingInformationModel.h"

#include <QString>
#include <gsl/gsl_cdf.h>
#include <gsl/gsl_sf.h>

#include <math.h>


RMasonryBuildingRetrofitCostModel::RMasonryBuildingRetrofitCostModel(QObject *parent, QString name)
	: RModel(parent,name)
{
	theWithTieColumn = false;
	theRigidDiaphragm = false;
	theRetrofitType = RMasonryBuildingRetrofitCostModel::Shotcrete;
	theUnitAreaReplacementCost = 0;

	theDomain->createObject(objectName() + "Response", "RGenericResponse");
	theRetrofitCost = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theRetrofitCost->setModel(this);

}
RMasonryBuildingRetrofitCostModel::~RMasonryBuildingRetrofitCostModel() 
{

}

RMasonryBuildingRetrofitCostModel::RRetrofitType RMasonryBuildingRetrofitCostModel::getRetrofitType() const	
{	
	return theRetrofitType;
}	
	
void RMasonryBuildingRetrofitCostModel::setRetrofitType(RMasonryBuildingRetrofitCostModel::RRetrofitType value)	
{	
	theRetrofitType = value;
}

bool RMasonryBuildingRetrofitCostModel::getWithTieColumn() const
{
	return theWithTieColumn;
}
void RMasonryBuildingRetrofitCostModel::setWithTieColumn(bool value)
{
	theWithTieColumn = value;
}

bool RMasonryBuildingRetrofitCostModel::getRigidDiaphragm() const
{
	return theRigidDiaphragm;
}
void RMasonryBuildingRetrofitCostModel::setRigidDiaphragm(bool value)
{
	theRigidDiaphragm = value;
}

QObject * RMasonryBuildingRetrofitCostModel::getBuildingInformationModel() const 
{
	return theBuildingInformationModel.data();
} 
void RMasonryBuildingRetrofitCostModel::setBuildingInformationModel(QObject *value) 
{
	theBuildingInformationModel = qobject_cast<RModel *>(value); 
}

QObject * RMasonryBuildingRetrofitCostModel::getAlpha() const 
{
	return theAlpha.data();
} 
void RMasonryBuildingRetrofitCostModel::setAlpha(QObject *value) 
{
	theAlpha = qobject_cast<RParameter *>(value); 
}

QObject * RMasonryBuildingRetrofitCostModel::getTheta1() const 
{
	return theTheta1.data();
} 
void RMasonryBuildingRetrofitCostModel::setTheta1(QObject *value) 
{
	theTheta1 = qobject_cast<RParameter *>(value); 
}

QObject * RMasonryBuildingRetrofitCostModel::getTheta2() const 
{
	return theTheta2.data();
} 
void RMasonryBuildingRetrofitCostModel::setTheta2(QObject *value) 
{
	theTheta2 = qobject_cast<RParameter *>(value); 
}

QObject * RMasonryBuildingRetrofitCostModel::getTheta0() const 
{
	return theTheta0.data();
} 
void RMasonryBuildingRetrofitCostModel::setTheta0(QObject *value) 
{
	theTheta0 = qobject_cast<RParameter *>(value); 
}

QObject * RMasonryBuildingRetrofitCostModel::getEpsilon() const 
{
	return theEpsilon.data();
} 
void RMasonryBuildingRetrofitCostModel::setEpsilon(QObject *value) 
{
	theEpsilon = qobject_cast<RParameter *>(value); 
}

QObject * RMasonryBuildingRetrofitCostModel::getSigma() const 
{
	return theSigma.data();
} 
void RMasonryBuildingRetrofitCostModel::setSigma(QObject *value) 
{
	theSigma = qobject_cast<RParameter *>(value); 
}

QObject * RMasonryBuildingRetrofitCostModel::getCostEpsilon() const 
{
	return theCostEpsilon.data();
} 
void RMasonryBuildingRetrofitCostModel::setCostEpsilon(QObject *value) 
{
	theCostEpsilon = qobject_cast<RParameter *>(value); 
}

double RMasonryBuildingRetrofitCostModel::getUnitAreaReplacementCost() const 
{
	return theUnitAreaReplacementCost;
} 
void RMasonryBuildingRetrofitCostModel::setUnitAreaReplacementCost(double value) 
{
	theUnitAreaReplacementCost = value; 
}

QObject * RMasonryBuildingRetrofitCostModel::getRetrofitStatus() const
{
	return theRetrofitStatus.data();
}

void RMasonryBuildingRetrofitCostModel::setRetrofitStatus(QObject *value)
{
	theRetrofitStatus = qobject_cast<RParameter *>(value);
}

int RMasonryBuildingRetrofitCostModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
{
	// Looking up Masonry type and diaphragm type
	int M = getWithTieColumn();
	int D = getRigidDiaphragm();

	// Looking up the alpha value = percenatage of increase in lateral strength of structure due tto retrofit
	double alpha = theAlpha.data()->getCurrentValue(threadID);

	// Looking up model parameters
	double theta1 = theTheta1.data()->getCurrentValue(threadID);
	double theta2 = theTheta2.data()->getCurrentValue(threadID);
	double theta0 = theTheta0.data()->getCurrentValue(threadID);

	// Looking up the model error parameters
	double sigma = theSigma.data()->getCurrentValue(threadID);
	double epsilon = theEpsilon.data()->getCurrentValue(threadID) * sigma;
	double costEpsilon = theCostEpsilon.data()->getCurrentValue(threadID);

	// Getting the information from the BIM
	RBuildingInformationModel *theBIM = qobject_cast<RBuildingInformationModel *>(theBuildingInformationModel.data());
	if (!theBIM) {
		rCritical(threadID) << "Error in model" << objectName() << ":" << theBuildingInformationModel.data()->objectName() << "is not a building information model.";
		return -1;
	}

	double totalArea = theBIM->getTotalArea();

	double theUARC = getUnitAreaReplacementCost();

	// Constnats
	/*double a = 0;
	double b = 0;
	if(theRetrofitType == RMasonryBuildingRetrofitCostModel::Shotcrete)
	{
		a = 0.639005;
		b = -5.153794;
	}
	else if(theRetrofitType == RMasonryBuildingRetrofitCostModel::FRP)
	{
		a = 0.643745;
		b = -5.594859;
	}
	else
	{
		a = 0.652310;
		b = -5.211230;
	}*/

	//calculating damage ratio
	double retrofitCostRatio = 0;
	retrofitCostRatio = gsl_sf_exp(theta1*D+theta2*M+theta0*(1-1.82505136468991E-07*theUARC-0.000210572859376824*totalArea-0.187606016934928*atan(alpha)) -5.23174336771223E-07*theUARC -0.000384852893651257*totalArea + 0.204664868525691*atan(alpha) + epsilon);

	double retrofitCost = 0;
	retrofitCost = retrofitCostRatio * theUARC * totalArea * costEpsilon;
	//If the retrofitStatus is zero, i.e., the retrofit operation has not been done yet, the retrofit cost is zero. Once the retrofit operation is done the cost is added. 
	double retrofitStatus = theRetrofitStatus.data()->getCurrentValue(threadID);
	retrofitCost = retrofitCost * retrofitStatus;

	theRetrofitCost -> setCurrentValue(retrofitCost, threadID);

	if (evaluateDDM)
	{
		// IMPLEMENT
	}
	
	return 0;
}

bool RMasonryBuildingRetrofitCostModel::canBeRunParallelly(){
	return true;
}