#include "RBuildingSearchAndRescueTimeModel.h"	

#include "RParameter.h"	
#include "RResponse.h"	
#include "RBuildingInformationModel.h"
#include <math.h>	
#include <gsl/gsl_cdf.h>

RBuildingSearchAndRescueTimeModel::RBuildingSearchAndRescueTimeModel(QObject *parent, QString name)
	: RModel(parent, name)
{
	// Initializing the data members
	//theBuildingInformationModel = 0;
	//theStructuralDamageRatio = 0;

	// Instantiating the response object
	theDomain->createObject(objectName() + "TimeResponse", "RGenericResponse");
	theSearchAndRescueTime = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theSearchAndRescueTime->setModel(this);

	theDomain->createObject(objectName() + "StatusResponse", "RGenericResponse");
	theSearchAndRescueStatus = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theSearchAndRescueStatus->setModel(this);
	theSearchAndRescueStatus->setCurrentValue(1);
}

RBuildingSearchAndRescueTimeModel::~RBuildingSearchAndRescueTimeModel()
{

}

QObject * RBuildingSearchAndRescueTimeModel::getBuildingInformationModel() const
{
	return theBuildingInformationModel.data();
}

void RBuildingSearchAndRescueTimeModel::setBuildingInformationModel(QObject *value)
{
	theBuildingInformationModel = qobject_cast<RModel *>(value);
}

QObject * RBuildingSearchAndRescueTimeModel::getStructuralDamageRatio() const
{
	return theStructuralDamageRatio.data();
}

void RBuildingSearchAndRescueTimeModel::setStructuralDamageRatio(QObject *value)
{
	theStructuralDamageRatio = qobject_cast<RParameter *>(value);
}

QObject * RBuildingSearchAndRescueTimeModel::getSearchAndRescueRate() const
{
	return theSearchAndRescueRate.data();
}

void RBuildingSearchAndRescueTimeModel::setSearchAndRescueRate(QObject *value)
{
	theSearchAndRescueRate = qobject_cast<RParameter *>(value);
}

QObject * RBuildingSearchAndRescueTimeModel::getCollapseUncertainty() const
{
	return theCollapseUncertainty.data();
}

void RBuildingSearchAndRescueTimeModel::setCollapseUncertainty(QObject *value)
{
	theCollapseUncertainty = qobject_cast<RParameter *>(value);
}

RBuildingSearchAndRescueTimeModel::RBuildingType RBuildingSearchAndRescueTimeModel::getBuildingType() const
{
	return theBuildingType;
}

void RBuildingSearchAndRescueTimeModel::setBuildingType(RBuildingSearchAndRescueTimeModel::RBuildingType value)
{
	theBuildingType = value;
}

int RBuildingSearchAndRescueTimeModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
{

	// Checking if all pointer properties are given by the user
	int status = this->checkProperties();
	// Error handling
	if (status < 0) {
		// Printing an error message to the output pane
		qCritical() << "Error in" << this->objectName() << ":"
			<< "Could not find all the passed objects";
		// Returning the error code
		return -1;
	}
	
	// Getting the information from the BIM
	RBuildingInformationModel *theBIM = qobject_cast<RBuildingInformationModel *>(theBuildingInformationModel.data());
	if (!theBIM) {
		qCritical() << "Error in model" << objectName() << ":" << theBuildingInformationModel.data()->objectName() << "is not a Building information model.";
		return -1;
	}

	double region = theBIM->getRegion();
	double totalArea = theBIM->getTotalArea();
	double height = theBIM->getHeight();
	double material = theBIM->getMaterial();
	double bearingSystem = theBIM->getLoadBearingSystem();
	double structuralDamageRatio = theStructuralDamageRatio.data()->getCurrentValue(threadID);
	if (structuralDamageRatio > 1)  structuralDamageRatio = 1;
	if (structuralDamageRatio < 0)  structuralDamageRatio = 0;

	double rescueTime = 0.0;
	double rescueStatus = 1.0;

	double collapseProbability;

	//The collapse probabilities are computed for complete damage state based on FEMA 2012 Table 13.8
	switch (this->getBuildingType()) {
	case (RBuildingSearchAndRescueTimeModel::W1):
		collapseProbability = 0.03;
		break;
	case (RBuildingSearchAndRescueTimeModel::W2):
		collapseProbability = 0.03;
		break;
	case (RBuildingSearchAndRescueTimeModel::S1L):
		collapseProbability = 0.08;
		break;
	case (RBuildingSearchAndRescueTimeModel::S1M):
		collapseProbability = 0.05;
		break;
	case (RBuildingSearchAndRescueTimeModel::S1H):
		collapseProbability = 0.03;
		break;
	case (RBuildingSearchAndRescueTimeModel::S2L):
		collapseProbability = 0.08;
		break;
	case (RBuildingSearchAndRescueTimeModel::S2M):
		collapseProbability = 0.05;
		break;
	case (RBuildingSearchAndRescueTimeModel::S2H):
		collapseProbability = 0.03;
		break;
	case (RBuildingSearchAndRescueTimeModel::S3):
		collapseProbability = 0.03;
		break;
	case (RBuildingSearchAndRescueTimeModel::S4L):
		collapseProbability = 0.08;
		break;
	case (RBuildingSearchAndRescueTimeModel::S4M):
		collapseProbability = 0.05;
		break;
	case (RBuildingSearchAndRescueTimeModel::S4H):
		collapseProbability = 0.03;
		break;
	case (RBuildingSearchAndRescueTimeModel::S5L):
		collapseProbability = 0.08;
		break;
	case (RBuildingSearchAndRescueTimeModel::S5M):
		collapseProbability = 0.05;
		break;
	case (RBuildingSearchAndRescueTimeModel::S5H):
		collapseProbability = 0.03;
		break;
	case (RBuildingSearchAndRescueTimeModel::C1L):
		collapseProbability = 0.13;
		break;
	case (RBuildingSearchAndRescueTimeModel::C1M):
		collapseProbability = 0.1;
		break;
	case (RBuildingSearchAndRescueTimeModel::C1H):
		collapseProbability = 0.05;
		break;
	case (RBuildingSearchAndRescueTimeModel::C2L):
		collapseProbability = 0.13;
		break;
	case (RBuildingSearchAndRescueTimeModel::C2M):
		collapseProbability = 0.1;
		break;
	case (RBuildingSearchAndRescueTimeModel::C2H):
		collapseProbability = 0.05;
		break;
	case (RBuildingSearchAndRescueTimeModel::C3L):
		collapseProbability = 0.15;
		break;
	case (RBuildingSearchAndRescueTimeModel::C3M):
		collapseProbability = 0.13;
		break;
	case (RBuildingSearchAndRescueTimeModel::C3H):
		collapseProbability = 0.1;
		break;
	case (RBuildingSearchAndRescueTimeModel::PC1):
		collapseProbability = 0.15;
		break;
	case (RBuildingSearchAndRescueTimeModel::PC2L):
		collapseProbability = 0.15;
		break;
	case (RBuildingSearchAndRescueTimeModel::PC2M):
		collapseProbability = 0.13;
		break;
	case (RBuildingSearchAndRescueTimeModel::PC2H):
		collapseProbability = 0.1;
		break;
	case (RBuildingSearchAndRescueTimeModel::RM1L):
		collapseProbability = 0.13;
		break;
	case (RBuildingSearchAndRescueTimeModel::RM1M):
		collapseProbability = 0.1;
		break;
	case (RBuildingSearchAndRescueTimeModel::RM2L):
		collapseProbability = 0.13;
		break;
	case (RBuildingSearchAndRescueTimeModel::RM2M):
		collapseProbability = 0.1;
		break;
	case (RBuildingSearchAndRescueTimeModel::RM2H):
		collapseProbability = 0.05;
		break;
	case (RBuildingSearchAndRescueTimeModel::URML):
		collapseProbability = 0.15;
		break;
	case (RBuildingSearchAndRescueTimeModel::URMM):
		collapseProbability = 0.15;
		break;
	case (RBuildingSearchAndRescueTimeModel::MH):
		collapseProbability = 0.03;
		break;
	}
	
	//Tranform normal random numbers to uniform (0,1)
	double randNum1 = gsl_cdf_ugaussian_P(theCollapseUncertainty.data()->getCurrentValue(threadID));

	//The average Search and Rescue rate 
	double rate = theSearchAndRescueRate.data()->getCurrentValue();

	if (structuralDamageRatio > 0.80) {
		if (randNum1 < collapseProbability) {
			rescueTime = rate * totalArea * structuralDamageRatio ;
		}
	}	

	if (rescueTime > 0)
		rescueStatus = 0.0;

	theSearchAndRescueTime->setCurrentValue(rescueTime, threadID);
	theSearchAndRescueStatus->setCurrentValue(rescueStatus, threadID);
	
	return 0;
}

bool RBuildingSearchAndRescueTimeModel::canBeRunParallelly() {
	return true;
}