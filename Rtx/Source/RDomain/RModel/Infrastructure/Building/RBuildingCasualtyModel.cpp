#include "RBuildingCasualtyModel.h"	

#include "RParameter.h"	
#include "RResponse.h"	
#include "RBuildingInformationModel.h"
#include <math.h>	

RBuildingCasualtyModel::RBuildingCasualtyModel(QObject *parent, QString name)
	: RModel(parent, name)
{
	theBuildingType = RBuildingCasualtyModel::C2H;

	// Instantiating the response object
	theDomain->createObject(objectName() + "RescuedUninjuredResponse", "RGenericResponse");
	theRescuedUninjuredPopulation = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theRescuedUninjuredPopulation->setModel(this);

	theDomain->createObject(objectName() + "RescuedInjuredResponse", "RGenericResponse");
	theRescuedInjuredPopulation = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theRescuedInjuredPopulation->setModel(this);
	
	theDomain->createObject(objectName() + "RescuedDeadResponse", "RGenericResponse");
	theRescuedDeadPopulation = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theRescuedDeadPopulation->setModel(this);

	theDomain->createObject(objectName() + "UnrescuedUninjuredResponse", "RGenericResponse");
	theUnrescuedUninjuredPopulation = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theUnrescuedUninjuredPopulation->setModel(this);

	theDomain->createObject(objectName() + "UnrescuedInjuredResponse", "RGenericResponse");
	theUnrescuedInjuredPopulation = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theUnrescuedInjuredPopulation->setModel(this);

	theDomain->createObject(objectName() + "UnrescuedDeadResponse", "RGenericResponse");
	theUnrescuedDeadPopulation = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theUnrescuedDeadPopulation->setModel(this);
}

RBuildingCasualtyModel::~RBuildingCasualtyModel()
{

}

QObject * RBuildingCasualtyModel::getBuildingInformationModel() const
{
	return theBuildingInformationModel.data();
}

void RBuildingCasualtyModel::setBuildingInformationModel(QObject *value)
{
	theBuildingInformationModel = qobject_cast<RModel *>(value);
}

QObject * RBuildingCasualtyModel::getPopulation() const
{
	return thePopulation.data();
}

void RBuildingCasualtyModel::setPopulation(QObject *value)
{
	thePopulation = qobject_cast<RParameter *>(value);
}

RBuildingCasualtyModel::RBuildingType RBuildingCasualtyModel::getBuildingType() const
{
	return theBuildingType;
}

void RBuildingCasualtyModel::setBuildingType(RBuildingCasualtyModel::RBuildingType value)
{
	theBuildingType = value;
}

QObject * RBuildingCasualtyModel::getStructuralDamageRatio() const
{
	return theStructuralDamageRatio.data();
}

void RBuildingCasualtyModel::setStructuralDamageRatio(QObject *value)
{
	theStructuralDamageRatio = qobject_cast<RParameter *>(value);
}

QObject * RBuildingCasualtyModel::getRescueStatus() const
{
	return theRescueStatus.data();
}

void RBuildingCasualtyModel::setRescueStatus(QObject *value)
{
	theRescueStatus = qobject_cast<RParameter *>(value);
}

int RBuildingCasualtyModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
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

	RBuildingInformationModel::RRegion region = theBIM->getRegion();
	int totalPop = thePopulation.data()->getCurrentValue(threadID);
	double structuralDamageRatio = theStructuralDamageRatio.data()->getCurrentValue(threadID);
	if (structuralDamageRatio > 1)  structuralDamageRatio = 1;
	if (structuralDamageRatio < 0)  structuralDamageRatio = 0;

	double rescueStatus = theRescueStatus.data()->getCurrentValue(threadID);
	int numInjuryLevels = 3;

	QList<double> structDamageRatioList;
	structDamageRatioList << 0 << 0.02 << 0.1 << 0.5 << 1.0;

	//Based on FEMA table 13.3 to 13.8
	QList<QList<double>> casualtyRateListList;

	QList<double> damState0RateList;
	damState0RateList << 100 << 0 << 0;

		casualtyRateListList << damState0RateList;

	QList<double> damState1RateList;
	QList<double> damState2RateList;
	QList<double> damState3RateList;
	QList<double> damState4RateList;

	if (region == RBuildingInformationModel::US) {
		switch (this->getBuildingType()) {

		case (RBuildingCasualtyModel::W1):
			damState1RateList << 99.95 << 0.05 << 0;
			casualtyRateListList << damState1RateList;
			damState2RateList << 99.72 << 0.28 << 0;
			casualtyRateListList << damState2RateList;
			damState3RateList << 98.898 << 1.101 << 0.00100000000000477;
			casualtyRateListList << damState3RateList;
			damState4RateList << 92.1206 << 7.7197 << 0.159700000000001;
			casualtyRateListList << damState4RateList;
			break;
		case (RBuildingCasualtyModel::W2):
			damState1RateList << 99.95 << 0.05 << 0;
			casualtyRateListList << damState1RateList;
			damState2RateList << 99.775 << 0.225 << 0;
			casualtyRateListList << damState2RateList;
			damState3RateList << 98.898 << 1.101 << 0.00100000000000477;
			casualtyRateListList << damState3RateList;
			damState4RateList << 91.9106 << 7.7797 << 0.309699999999992;
			casualtyRateListList << damState4RateList;

			break;
		case (RBuildingCasualtyModel::S1L):
			damState1RateList << 99.95 << 0.05 << 0;
			casualtyRateListList << damState1RateList;
			damState2RateList << 99.775 << 0.225 << 0;
			casualtyRateListList << damState2RateList;
			damState3RateList << 98.898 << 1.101 << 0.00100000000000477;
			casualtyRateListList << damState3RateList;
			damState4RateList << 88.4616 << 10.7292 << 0.80919999999999;
			casualtyRateListList << damState4RateList;

			break;
		case (RBuildingCasualtyModel::S1M):
			damState1RateList << 99.95 << 0.05 << 0;
			casualtyRateListList << damState1RateList;
			damState2RateList << 99.775 << 0.225 << 0;
			casualtyRateListList << damState2RateList;
			damState3RateList << 98.898 << 1.101 << 0.00100000000000477;
			casualtyRateListList << damState3RateList;
			damState4RateList << 90.531 << 8.9595 << 0.509500000000003;
			casualtyRateListList << damState4RateList;

			break;
		case (RBuildingCasualtyModel::S1H):
			damState1RateList << 99.95 << 0.05 << 0;
			casualtyRateListList << damState1RateList;
			damState2RateList << 99.775 << 0.225 << 0;
			casualtyRateListList << damState2RateList;
			damState3RateList << 98.898 << 1.101 << 0.00100000000000477;
			casualtyRateListList << damState3RateList;
			damState4RateList << 91.9106 << 7.7797 << 0.309699999999992;
			casualtyRateListList << damState4RateList;

			break;
		case (RBuildingCasualtyModel::S2L):
			damState1RateList << 99.95 << 0.05 << 0;
			casualtyRateListList << damState1RateList;
			damState2RateList << 99.775 << 0.225 << 0;
			casualtyRateListList << damState2RateList;
			damState3RateList << 98.898 << 1.101 << 0.00100000000000477;
			casualtyRateListList << damState3RateList;
			damState4RateList << 88.4616 << 10.7292 << 0.80919999999999;
			casualtyRateListList << damState4RateList;

			break;
		case (RBuildingCasualtyModel::S2M):
			damState1RateList << 99.95 << 0.05 << 0;
			casualtyRateListList << damState1RateList;
			damState2RateList << 99.775 << 0.225 << 0;
			casualtyRateListList << damState2RateList;
			damState3RateList << 98.898 << 1.101 << 0.00100000000000477;
			casualtyRateListList << damState3RateList;
			damState4RateList << 90.531 << 8.9595 << 0.509500000000003;
			casualtyRateListList << damState4RateList;

			break;
		case (RBuildingCasualtyModel::S2H):
			damState1RateList << 99.95 << 0.05 << 0;
			casualtyRateListList << damState1RateList;
			damState2RateList << 99.775 << 0.225 << 0;
			casualtyRateListList << damState2RateList;
			damState3RateList << 98.898 << 1.101 << 0.00100000000000477;
			casualtyRateListList << damState3RateList;
			damState4RateList << 91.9106 << 7.7797 << 0.309699999999992;
			casualtyRateListList << damState4RateList;

			break;
		case (RBuildingCasualtyModel::S3):
			damState1RateList << 99.95 << 0.05 << 0;
			casualtyRateListList << damState1RateList;
			damState2RateList << 99.775 << 0.225 << 0;
			casualtyRateListList << damState2RateList;
			damState3RateList << 98.898 << 1.101 << 0.00100000000000477;
			casualtyRateListList << damState3RateList;
			damState4RateList << 92.1206 << 7.7197 << 0.159700000000001;
			casualtyRateListList << damState4RateList;

			break;
		case (RBuildingCasualtyModel::S4L):
			damState1RateList << 99.95 << 0.05 << 0;
			casualtyRateListList << damState1RateList;
			damState2RateList << 99.72 << 0.28 << 0;
			casualtyRateListList << damState2RateList;
			damState3RateList << 98.898 << 1.101 << 0.00100000000000477;
			casualtyRateListList << damState3RateList;
			damState4RateList << 88.4616 << 10.7292 << 0.80919999999999;
			casualtyRateListList << damState4RateList;

			break;
		case (RBuildingCasualtyModel::S4M):
			damState1RateList << 99.95 << 0.05 << 0;
			casualtyRateListList << damState1RateList;
			damState2RateList << 99.72 << 0.28 << 0;
			casualtyRateListList << damState2RateList;
			damState3RateList << 98.898 << 1.101 << 0.00100000000000477;
			casualtyRateListList << damState3RateList;
			damState4RateList << 90.531 << 8.9595 << 0.509500000000003;
			casualtyRateListList << damState4RateList;

			break;
		case (RBuildingCasualtyModel::S4H):
			damState1RateList << 99.95 << 0.05 << 0;
			casualtyRateListList << damState1RateList;
			damState2RateList << 99.72 << 0.28 << 0;
			casualtyRateListList << damState2RateList;
			damState3RateList << 98.898 << 1.101 << 0.00100000000000477;
			casualtyRateListList << damState3RateList;
			damState4RateList << 91.9106 << 7.7797 << 0.309699999999992;
			casualtyRateListList << damState4RateList;

			break;
		case (RBuildingCasualtyModel::S5L):
			damState1RateList << 99.95 << 0.05 << 0;
			casualtyRateListList << damState1RateList;
			damState2RateList << 99.775 << 0.225 << 0;
			casualtyRateListList << damState2RateList;
			damState3RateList << 98.898 << 1.101 << 0.00100000000000477;
			casualtyRateListList << damState3RateList;
			damState4RateList << 88.4616 << 10.7292 << 0.80919999999999;
			casualtyRateListList << damState4RateList;

			break;
		case (RBuildingCasualtyModel::S5M):
			damState1RateList << 99.95 << 0.05 << 0;
			casualtyRateListList << damState1RateList;
			damState2RateList << 99.775 << 0.225 << 0;
			casualtyRateListList << damState2RateList;
			damState3RateList << 98.898 << 1.101 << 0.00100000000000477;
			casualtyRateListList << damState3RateList;
			damState4RateList << 90.531 << 8.9595 << 0.509500000000003;
			casualtyRateListList << damState4RateList;

			break;
		case (RBuildingCasualtyModel::S5H):
			damState1RateList << 99.95 << 0.05 << 0;
			casualtyRateListList << damState1RateList;
			damState2RateList << 99.775 << 0.225 << 0;
			casualtyRateListList << damState2RateList;
			damState3RateList << 98.898 << 1.101 << 0.00100000000000477;
			casualtyRateListList << damState3RateList;
			damState4RateList << 91.9106 << 7.7797 << 0.309699999999992;
			casualtyRateListList << damState4RateList;

			break;
		case (RBuildingCasualtyModel::C1L):
			damState1RateList << 99.95 << 0.05 << 0;
			casualtyRateListList << damState1RateList;
			damState2RateList << 99.72 << 0.28 << 0;
			casualtyRateListList << damState2RateList;
			damState3RateList << 98.898 << 1.101 << 0.00100000000000477;
			casualtyRateListList << damState3RateList;
			damState4RateList << 85.0126 << 13.6787 << 1.30869999999999;
			casualtyRateListList << damState4RateList;

			break;
		case (RBuildingCasualtyModel::C1M):
			damState1RateList << 99.95 << 0.05 << 0;
			casualtyRateListList << damState1RateList;
			damState2RateList << 99.72 << 0.28 << 0;
			casualtyRateListList << damState2RateList;
			damState3RateList << 98.898 << 1.101 << 0.00100000000000477;
			casualtyRateListList << damState3RateList;
			damState4RateList << 87.082 << 11.909 << 1.009;
			casualtyRateListList << damState4RateList;

			break;
		case (RBuildingCasualtyModel::C1H):
			damState1RateList << 99.95 << 0.05 << 0;
			casualtyRateListList << damState1RateList;
			damState2RateList << 99.72 << 0.28 << 0;
			casualtyRateListList << damState2RateList;
			damState3RateList << 98.898 << 1.101 << 0.00100000000000477;
			casualtyRateListList << damState3RateList;
			damState4RateList << 90.531 << 8.9595 << 0.509500000000003;
			casualtyRateListList << damState4RateList;

			break;
		case (RBuildingCasualtyModel::C2L):
			damState1RateList << 99.95 << 0.05 << 0;
			casualtyRateListList << damState1RateList;
			damState2RateList << 99.72 << 0.28 << 0;
			casualtyRateListList << damState2RateList;
			damState3RateList << 98.898 << 1.101 << 0.00100000000000477;
			casualtyRateListList << damState3RateList;
			damState4RateList << 85.0126 << 13.6787 << 1.30869999999999;
			casualtyRateListList << damState4RateList;

			break;
		case (RBuildingCasualtyModel::C2M):
			damState1RateList << 99.95 << 0.05 << 0;
			casualtyRateListList << damState1RateList;
			damState2RateList << 99.72 << 0.28 << 0;
			casualtyRateListList << damState2RateList;
			damState3RateList << 98.898 << 1.101 << 0.00100000000000477;
			casualtyRateListList << damState3RateList;
			damState4RateList << 87.082 << 11.909 << 1.009;
			casualtyRateListList << damState4RateList;

			break;
		case (RBuildingCasualtyModel::C2H):
			damState1RateList << 99.95 << 0.05 << 0;
			casualtyRateListList << damState1RateList;
			damState2RateList << 99.72 << 0.28 << 0;
			casualtyRateListList << damState2RateList;
			damState3RateList << 98.898 << 1.101 << 0.00100000000000477;
			casualtyRateListList << damState3RateList;
			damState4RateList << 90.531 << 8.9595 << 0.509500000000003;
			casualtyRateListList << damState4RateList;

			break;
		case (RBuildingCasualtyModel::C3L):
			damState1RateList << 99.95 << 0.05 << 0;
			casualtyRateListList << damState1RateList;
			damState2RateList << 99.775 << 0.225 << 0;
			casualtyRateListList << damState2RateList;
			damState3RateList << 98.898 << 1.101 << 0.00100000000000477;
			casualtyRateListList << damState3RateList;
			damState4RateList << 83.633 << 14.8585 << 1.50850000000001;
			casualtyRateListList << damState4RateList;

			break;
		case (RBuildingCasualtyModel::C3M):
			damState1RateList << 99.95 << 0.05 << 0;
			casualtyRateListList << damState1RateList;
			damState2RateList << 99.775 << 0.225 << 0;
			casualtyRateListList << damState2RateList;
			damState3RateList << 98.898 << 1.101 << 0.00100000000000477;
			casualtyRateListList << damState3RateList;
			damState4RateList << 85.0126 << 13.6787 << 1.30869999999999;
			casualtyRateListList << damState4RateList;

			break;
		case (RBuildingCasualtyModel::C3H):
			damState1RateList << 99.95 << 0.05 << 0;
			casualtyRateListList << damState1RateList;
			damState2RateList << 99.775 << 0.225 << 0;
			casualtyRateListList << damState2RateList;
			damState3RateList << 98.898 << 1.101 << 0.00100000000000477;
			casualtyRateListList << damState3RateList;
			damState4RateList << 87.082 << 11.909 << 1.009;
			casualtyRateListList << damState4RateList;

			break;
		case (RBuildingCasualtyModel::PC1):
			damState1RateList << 99.95 << 0.05 << 0;
			casualtyRateListList << damState1RateList;
			damState2RateList << 99.72 << 0.28 << 0;
			casualtyRateListList << damState2RateList;
			damState3RateList << 98.898 << 1.101 << 0.00100000000000477;
			casualtyRateListList << damState3RateList;
			damState4RateList << 83.633 << 14.8585 << 1.50850000000001;
			casualtyRateListList << damState4RateList;

			break;
		case (RBuildingCasualtyModel::PC2L):
			damState1RateList << 99.95 << 0.05 << 0;
			casualtyRateListList << damState1RateList;
			damState2RateList << 99.72 << 0.28 << 0;
			casualtyRateListList << damState2RateList;
			damState3RateList << 98.898 << 1.101 << 0.00100000000000477;
			casualtyRateListList << damState3RateList;
			damState4RateList << 83.633 << 14.8585 << 1.50850000000001;
			casualtyRateListList << damState4RateList;

			break;
		case (RBuildingCasualtyModel::PC2M):
			damState1RateList << 99.95 << 0.05 << 0;
			casualtyRateListList << damState1RateList;
			damState2RateList << 99.72 << 0.28 << 0;
			casualtyRateListList << damState2RateList;
			damState3RateList << 98.898 << 1.101 << 0.00100000000000477;
			casualtyRateListList << damState3RateList;
			damState4RateList << 85.0126 << 13.6787 << 1.30869999999999;
			casualtyRateListList << damState4RateList;

			break;
		case (RBuildingCasualtyModel::PC2H):
			damState1RateList << 99.95 << 0.05 << 0;
			casualtyRateListList << damState1RateList;
			damState2RateList << 99.72 << 0.28 << 0;
			casualtyRateListList << damState2RateList;
			damState3RateList << 98.898 << 1.101 << 0.00100000000000477;
			casualtyRateListList << damState3RateList;
			damState4RateList << 87.082 << 11.909 << 1.009;
			casualtyRateListList << damState4RateList;

			break;
		case (RBuildingCasualtyModel::RM1L):
			damState1RateList << 99.95 << 0.05 << 0;
			casualtyRateListList << damState1RateList;
			damState2RateList << 99.775 << 0.225 << 0;
			casualtyRateListList << damState2RateList;
			damState3RateList << 98.898 << 1.101 << 0.00100000000000477;
			casualtyRateListList << damState3RateList;
			damState4RateList << 85.0126 << 13.6787 << 1.30869999999999;
			casualtyRateListList << damState4RateList;


			break;
		case (RBuildingCasualtyModel::RM1M):
			damState1RateList << 99.95 << 0.05 << 0;
			casualtyRateListList << damState1RateList;
			damState2RateList << 99.775 << 0.225 << 0;
			casualtyRateListList << damState2RateList;
			damState3RateList << 98.898 << 1.101 << 0.00100000000000477;
			casualtyRateListList << damState3RateList;
			damState4RateList << 87.082 << 11.909 << 1.009;
			casualtyRateListList << damState4RateList;

			break;
		case (RBuildingCasualtyModel::RM2L):
			damState1RateList << 99.95 << 0.05 << 0;
			casualtyRateListList << damState1RateList;
			damState2RateList << 99.775 << 0.225 << 0;
			casualtyRateListList << damState2RateList;
			damState3RateList << 98.898 << 1.101 << 0.00100000000000477;
			casualtyRateListList << damState3RateList;
			damState4RateList << 85.0126 << 13.6787 << 1.30869999999999;
			casualtyRateListList << damState4RateList;

			break;
		case (RBuildingCasualtyModel::RM2M):
			damState1RateList << 99.95 << 0.05 << 0;
			casualtyRateListList << damState1RateList;
			damState2RateList << 99.775 << 0.225 << 0;
			casualtyRateListList << damState2RateList;
			damState3RateList << 98.898 << 1.101 << 0.00100000000000477;
			casualtyRateListList << damState3RateList;
			damState4RateList << 87.082 << 11.909 << 1.009;
			casualtyRateListList << damState4RateList;

			break;
		case (RBuildingCasualtyModel::RM2H):
			damState1RateList << 99.95 << 0.05 << 0;
			casualtyRateListList << damState1RateList;
			damState2RateList << 99.775 << 0.225 << 0;
			casualtyRateListList << damState2RateList;
			damState3RateList << 98.898 << 1.101 << 0.00100000000000477;
			casualtyRateListList << damState3RateList;
			damState4RateList << 90.531 << 8.9595 << 0.509500000000003;
			casualtyRateListList << damState4RateList;

			break;
		case (RBuildingCasualtyModel::URML):
			damState1RateList << 99.95 << 0.05 << 0;
			casualtyRateListList << damState1RateList;
			damState2RateList << 99.248 << 0.751 << 0.000999999999990564;
			casualtyRateListList << damState2RateList;
			damState3RateList << 97.796 << 2.202 << 0.00199999999999534;
			casualtyRateListList << damState3RateList;
			damState4RateList << 78.516 << 19.967 << 1.517;
			casualtyRateListList << damState4RateList;

			break;
		case (RBuildingCasualtyModel::URMM):
			damState1RateList << 99.95 << 0.05 << 0;
			casualtyRateListList << damState1RateList;
			damState2RateList << 99.248 << 0.751 << 0.000999999999990564;
			casualtyRateListList << damState2RateList;
			damState3RateList << 97.796 << 2.202 << 0.00199999999999534;
			casualtyRateListList << damState3RateList;
			damState4RateList << 78.516 << 19.967 << 1.517;
			casualtyRateListList << damState4RateList;

			break;
		case (RBuildingCasualtyModel::MH):
			damState1RateList << 99.95 << 0.05 << 0;
			casualtyRateListList << damState1RateList;
			damState2RateList << 99.72 << 0.28 << 0;
			casualtyRateListList << damState2RateList;
			damState3RateList << 98.898 << 1.101 << 0.00100000000000477;
			casualtyRateListList << damState3RateList;
			damState4RateList << 92.1206 << 7.7197 << 0.159700000000001;
			casualtyRateListList << damState4RateList;

			break;
		}
	}
	if (region == RBuildingInformationModel::Iran) {
		switch (this->getBuildingType()) {

		case (RBuildingCasualtyModel::W1):
			damState1RateList << 99.9035 << 0.0965 << 0;
			casualtyRateListList << damState1RateList;
			damState2RateList << 99.4596 << 0.5404 << 0;
			casualtyRateListList << damState2RateList;
			damState3RateList << 97.87314 << 2.12493 << 0.00193;
			casualtyRateListList << damState3RateList;
			damState4RateList << 84.792758 << 14.899021 << 0.308221;
			casualtyRateListList << damState4RateList;

			break;
		case (RBuildingCasualtyModel::W2):
			damState1RateList << 99.95175 << 0.04825 << 0;
			casualtyRateListList << damState1RateList;
			damState2RateList << 99.782875 << 0.217125 << 0;
			casualtyRateListList << damState2RateList;
			damState3RateList << 98.93657 << 1.062465 << 0.000965;
			casualtyRateListList << damState3RateList;
			damState4RateList << 92.193729 << 7.5074105 << 0.2988605;
			casualtyRateListList << damState4RateList;

			break;
		case (RBuildingCasualtyModel::S1L):
			damState1RateList << 99.9035 << 0.0965 << 0;
			casualtyRateListList << damState1RateList;
			damState2RateList << 99.56575 << 0.43425 << 0;
			casualtyRateListList << damState2RateList;
			damState3RateList << 97.87314 << 2.12493 << 0.00193;
			casualtyRateListList << damState3RateList;
			damState4RateList << 77.730888 << 20.707356 << 1.561756;
			casualtyRateListList << damState4RateList;


			break;
		case (RBuildingCasualtyModel::S1M):
			damState1RateList << 99.9035 << 0.0965 << 0;
			casualtyRateListList << damState1RateList;
			damState2RateList << 99.56575 << 0.43425 << 0;
			casualtyRateListList << damState2RateList;
			damState3RateList << 97.87314 << 2.12493 << 0.00193;
			casualtyRateListList << damState3RateList;
			damState4RateList << 81.72483 << 17.291835 << 0.983335;
			casualtyRateListList << damState4RateList;


			break;
		case (RBuildingCasualtyModel::S1H):
			damState1RateList << 99.9035 << 0.0965 << 0;
			casualtyRateListList << damState1RateList;
			damState2RateList << 99.56575 << 0.43425 << 0;
			casualtyRateListList << damState2RateList;
			damState3RateList << 97.87314 << 2.12493 << 0.00193;
			casualtyRateListList << damState3RateList;
			damState4RateList << 84.387458 << 15.014821 << 0.597721;
			casualtyRateListList << damState4RateList;


			break;
		case (RBuildingCasualtyModel::S2L):
			damState1RateList << 99.9035 << 0.0965 << 0;
			casualtyRateListList << damState1RateList;
			damState2RateList << 99.56575 << 0.43425 << 0;
			casualtyRateListList << damState2RateList;
			damState3RateList << 97.87314 << 2.12493 << 0.00193;
			casualtyRateListList << damState3RateList;
			damState4RateList << 77.730888 << 20.707356 << 1.561756;
			casualtyRateListList << damState4RateList;


			break;
		case (RBuildingCasualtyModel::S2M):
			damState1RateList << 99.9035 << 0.0965 << 0;
			casualtyRateListList << damState1RateList;
			damState2RateList << 99.56575 << 0.43425 << 0;
			casualtyRateListList << damState2RateList;
			damState3RateList << 97.87314 << 2.12493 << 0.00193;
			casualtyRateListList << damState3RateList;
			damState4RateList << 81.72483 << 17.291835 << 0.983335;
			casualtyRateListList << damState4RateList;


			break;
		case (RBuildingCasualtyModel::S2H):
			damState1RateList << 99.9035 << 0.0965 << 0;
			casualtyRateListList << damState1RateList;
			damState2RateList << 99.56575 << 0.43425 << 0;
			casualtyRateListList << damState2RateList;
			damState3RateList << 97.87314 << 2.12493 << 0.00193;
			casualtyRateListList << damState3RateList;
			damState4RateList << 84.387458 << 15.014821 << 0.597721;
			casualtyRateListList << damState4RateList;


			break;
		case (RBuildingCasualtyModel::S3):
			damState1RateList << 99.9035 << 0.0965 << 0;
			casualtyRateListList << damState1RateList;
			damState2RateList << 99.56575 << 0.43425 << 0;
			casualtyRateListList << damState2RateList;
			damState3RateList << 97.87314 << 2.12493 << 0.00193;
			casualtyRateListList << damState3RateList;
			damState4RateList << 84.792758 << 14.899021 << 0.308221;
			casualtyRateListList << damState4RateList;


			break;
		case (RBuildingCasualtyModel::S4L):
			damState1RateList << 99.9035 << 0.0965 << 0;
			casualtyRateListList << damState1RateList;
			damState2RateList << 99.4596 << 0.5404 << 0;
			casualtyRateListList << damState2RateList;
			damState3RateList << 97.87314 << 2.12493 << 0.00193;
			casualtyRateListList << damState3RateList;
			damState4RateList << 77.730888 << 20.707356 << 1.561756;
			casualtyRateListList << damState4RateList;


			break;
		case (RBuildingCasualtyModel::S4M):
			damState1RateList << 99.9035 << 0.0965 << 0;
			casualtyRateListList << damState1RateList;
			damState2RateList << 99.4596 << 0.5404 << 0;
			casualtyRateListList << damState2RateList;
			damState3RateList << 97.87314 << 2.12493 << 0.00193;
			casualtyRateListList << damState3RateList;
			damState4RateList << 81.72483 << 17.291835 << 0.983335;
			casualtyRateListList << damState4RateList;


			break;
		case (RBuildingCasualtyModel::S4H):
			damState1RateList << 99.9035 << 0.0965 << 0;
			casualtyRateListList << damState1RateList;
			damState2RateList << 99.4596 << 0.5404 << 0;
			casualtyRateListList << damState2RateList;
			damState3RateList << 97.87314 << 2.12493 << 0.00193;
			casualtyRateListList << damState3RateList;
			damState4RateList << 84.387458 << 15.014821 << 0.597721;
			casualtyRateListList << damState4RateList;


			break;
		case (RBuildingCasualtyModel::S5L):
			damState1RateList << 99.9035 << 0.0965 << 0;
			casualtyRateListList << damState1RateList;
			damState2RateList << 99.56575 << 0.43425 << 0;
			casualtyRateListList << damState2RateList;
			damState3RateList << 97.87314 << 2.12493 << 0.00193;
			casualtyRateListList << damState3RateList;
			damState4RateList << 77.730888 << 20.707356 << 1.561756;
			casualtyRateListList << damState4RateList;


			break;
		case (RBuildingCasualtyModel::S5M):
			damState1RateList << 99.9035 << 0.0965 << 0;
			casualtyRateListList << damState1RateList;
			damState2RateList << 99.56575 << 0.43425 << 0;
			casualtyRateListList << damState2RateList;
			damState3RateList << 97.87314 << 2.12493 << 0.00193;
			casualtyRateListList << damState3RateList;
			damState4RateList << 81.72483 << 17.291835 << 0.983335;
			casualtyRateListList << damState4RateList;


			break;
		case (RBuildingCasualtyModel::S5H):
			damState1RateList << 99.9035 << 0.0965 << 0;
			casualtyRateListList << damState1RateList;
			damState2RateList << 99.56575 << 0.43425 << 0;
			casualtyRateListList << damState2RateList;
			damState3RateList << 97.87314 << 2.12493 << 0.00193;
			casualtyRateListList << damState3RateList;
			damState4RateList << 84.387458 << 15.014821 << 0.597721;
			casualtyRateListList << damState4RateList;


			break;
		case (RBuildingCasualtyModel::C1L):
			damState1RateList << 99.9035 << 0.0965 << 0;
			casualtyRateListList << damState1RateList;
			damState2RateList << 99.4596 << 0.5404 << 0;
			casualtyRateListList << damState2RateList;
			damState3RateList << 97.87314 << 2.12493 << 0.00193;
			casualtyRateListList << damState3RateList;
			damState4RateList << 71.074318 << 26.399891 << 2.525791;
			casualtyRateListList << damState4RateList;


			break;
		case (RBuildingCasualtyModel::C1M):
			damState1RateList << 99.9035 << 0.0965 << 0;
			casualtyRateListList << damState1RateList;
			damState2RateList << 99.4596 << 0.5404 << 0;
			casualtyRateListList << damState2RateList;
			damState3RateList << 97.87314 << 2.12493 << 0.00193;
			casualtyRateListList << damState3RateList;
			damState4RateList << 75.06826 << 22.98437 << 1.94737;
			casualtyRateListList << damState4RateList;


			break;
		case (RBuildingCasualtyModel::C1H):
			damState1RateList << 99.9035 << 0.0965 << 0;
			casualtyRateListList << damState1RateList;
			damState2RateList << 99.4596 << 0.5404 << 0;
			casualtyRateListList << damState2RateList;
			damState3RateList << 97.87314 << 2.12493 << 0.00193;
			casualtyRateListList << damState3RateList;
			damState4RateList << 81.72483 << 17.291835 << 0.983335;
			casualtyRateListList << damState4RateList;


			break;
		case (RBuildingCasualtyModel::C2L):
			damState1RateList << 99.9035 << 0.0965 << 0;
			casualtyRateListList << damState1RateList;
			damState2RateList << 99.4596 << 0.5404 << 0;
			casualtyRateListList << damState2RateList;
			damState3RateList << 97.87314 << 2.12493 << 0.00193;
			casualtyRateListList << damState3RateList;
			damState4RateList << 71.074318 << 26.399891 << 2.525791;
			casualtyRateListList << damState4RateList;


			break;
		case (RBuildingCasualtyModel::C2M):
			damState1RateList << 99.9035 << 0.0965 << 0;
			casualtyRateListList << damState1RateList;
			damState2RateList << 99.4596 << 0.5404 << 0;
			casualtyRateListList << damState2RateList;
			damState3RateList << 97.87314 << 2.12493 << 0.00193;
			casualtyRateListList << damState3RateList;
			damState4RateList << 75.06826 << 22.98437 << 1.94737;
			casualtyRateListList << damState4RateList;


			break;
		case (RBuildingCasualtyModel::C2H):
			damState1RateList << 99.9035 << 0.0965 << 0;
			casualtyRateListList << damState1RateList;
			damState2RateList << 99.4596 << 0.5404 << 0;
			casualtyRateListList << damState2RateList;
			damState3RateList << 97.87314 << 2.12493 << 0.00193;
			casualtyRateListList << damState3RateList;
			damState4RateList << 81.72483 << 17.291835 << 0.983335;
			casualtyRateListList << damState4RateList;


			break;
		case (RBuildingCasualtyModel::C3L):
			damState1RateList << 99.9035 << 0.0965 << 0;
			casualtyRateListList << damState1RateList;
			damState2RateList << 99.56575 << 0.43425 << 0;
			casualtyRateListList << damState2RateList;
			damState3RateList << 97.87314 << 2.12493 << 0.00193;
			casualtyRateListList << damState3RateList;
			damState4RateList << 68.41169 << 28.676905 << 2.911405;
			casualtyRateListList << damState4RateList;


			break;
		case (RBuildingCasualtyModel::C3M):
			damState1RateList << 99.9035 << 0.0965 << 0;
			casualtyRateListList << damState1RateList;
			damState2RateList << 99.56575 << 0.43425 << 0;
			casualtyRateListList << damState2RateList;
			damState3RateList << 97.87314 << 2.12493 << 0.00193;
			casualtyRateListList << damState3RateList;
			damState4RateList << 71.074318 << 26.399891 << 2.525791;
			casualtyRateListList << damState4RateList;


			break;
		case (RBuildingCasualtyModel::C3H):
			damState1RateList << 99.9035 << 0.0965 << 0;
			casualtyRateListList << damState1RateList;
			damState2RateList << 99.56575 << 0.43425 << 0;
			casualtyRateListList << damState2RateList;
			damState3RateList << 97.87314 << 2.12493 << 0.00193;
			casualtyRateListList << damState3RateList;
			damState4RateList << 75.06826 << 22.98437 << 1.94737;
			casualtyRateListList << damState4RateList;


			break;
		case (RBuildingCasualtyModel::PC1):
			damState1RateList << 99.9035 << 0.0965 << 0;
			casualtyRateListList << damState1RateList;
			damState2RateList << 99.4596 << 0.5404 << 0;
			casualtyRateListList << damState2RateList;
			damState3RateList << 97.87314 << 2.12493 << 0.00193;
			casualtyRateListList << damState3RateList;
			damState4RateList << 68.41169 << 28.676905 << 2.911405;
			casualtyRateListList << damState4RateList;


			break;
		case (RBuildingCasualtyModel::PC2L):
			damState1RateList << 99.9035 << 0.0965 << 0;
			casualtyRateListList << damState1RateList;
			damState2RateList << 99.4596 << 0.5404 << 0;
			casualtyRateListList << damState2RateList;
			damState3RateList << 97.87314 << 2.12493 << 0.00193;
			casualtyRateListList << damState3RateList;
			damState4RateList << 68.41169 << 28.676905 << 2.911405;
			casualtyRateListList << damState4RateList;


			break;
		case (RBuildingCasualtyModel::PC2M):
			damState1RateList << 99.9035 << 0.0965 << 0;
			casualtyRateListList << damState1RateList;
			damState2RateList << 99.4596 << 0.5404 << 0;
			casualtyRateListList << damState2RateList;
			damState3RateList << 97.87314 << 2.12493 << 0.00193;
			casualtyRateListList << damState3RateList;
			damState4RateList << 71.074318 << 26.399891 << 2.525791;
			casualtyRateListList << damState4RateList;


			break;
		case (RBuildingCasualtyModel::PC2H):
			damState1RateList << 99.9035 << 0.0965 << 0;
			casualtyRateListList << damState1RateList;
			damState2RateList << 99.4596 << 0.5404 << 0;
			casualtyRateListList << damState2RateList;
			damState3RateList << 97.87314 << 2.12493 << 0.00193;
			casualtyRateListList << damState3RateList;
			damState4RateList << 75.06826 << 22.98437 << 1.94737;
			casualtyRateListList << damState4RateList;


			break;
		case (RBuildingCasualtyModel::RM1L):
			damState1RateList << 99.9155 << 0.0845 << 0;
			casualtyRateListList << damState1RateList;
			damState2RateList << 99.61975 << 0.38025 << 0;
			casualtyRateListList << damState2RateList;
			damState3RateList << 98.13762 << 1.86069 << 0.00169;
			casualtyRateListList << damState3RateList;
			damState4RateList << 74.671294 << 23.117003 << 2.211703;
			casualtyRateListList << damState4RateList;



			break;
		case (RBuildingCasualtyModel::RM1M):
			damState1RateList << 99.9155 << 0.0845 << 0;
			casualtyRateListList << damState1RateList;
			damState2RateList << 99.61975 << 0.38025 << 0;
			casualtyRateListList << damState2RateList;
			damState3RateList << 98.13762 << 1.86069 << 0.00169;
			casualtyRateListList << damState3RateList;
			damState4RateList << 78.16858 << 20.12621 << 1.70521;
			casualtyRateListList << damState4RateList;


			break;
		case (RBuildingCasualtyModel::RM2L):
			damState1RateList << 99.9155 << 0.0845 << 0;
			casualtyRateListList << damState1RateList;
			damState2RateList << 99.61975 << 0.38025 << 0;
			casualtyRateListList << damState2RateList;
			damState3RateList << 98.13762 << 1.86069 << 0.00169;
			casualtyRateListList << damState3RateList;
			damState4RateList << 74.671294 << 23.117003 << 2.211703;
			casualtyRateListList << damState4RateList;


			break;
		case (RBuildingCasualtyModel::RM2M):
			damState1RateList << 99.9155 << 0.0845 << 0;
			casualtyRateListList << damState1RateList;
			damState2RateList << 99.61975 << 0.38025 << 0;
			casualtyRateListList << damState2RateList;
			damState3RateList << 98.13762 << 1.86069 << 0.00169;
			casualtyRateListList << damState3RateList;
			damState4RateList << 78.16858 << 20.12621 << 1.70521;
			casualtyRateListList << damState4RateList;


			break;
		case (RBuildingCasualtyModel::RM2H):
			damState1RateList << 99.9155 << 0.0845 << 0;
			casualtyRateListList << damState1RateList;
			damState2RateList << 99.61975 << 0.38025 << 0;
			casualtyRateListList << damState2RateList;
			damState3RateList << 98.13762 << 1.86069 << 0.00169;
			casualtyRateListList << damState3RateList;
			damState4RateList << 83.99739 << 15.141555 << 0.861055;
			casualtyRateListList << damState4RateList;


			break;
		case (RBuildingCasualtyModel::URML):
			damState1RateList << 99.7445 << 0.2555 << 0;
			casualtyRateListList << damState1RateList;
			damState2RateList << 96.15728 << 3.83761 << 0.00511;
			casualtyRateListList << damState2RateList;
			damState3RateList << 88.73756 << 11.25222 << 0.01022;
			casualtyRateListList << damState3RateList;
			damState4RateList << 6.20675999999998 << 86.04137 << 7.75187;
			casualtyRateListList << damState4RateList;


			break;
		case (RBuildingCasualtyModel::URMM):
			damState1RateList << 99.7445 << 0.2555 << 0;
			casualtyRateListList << damState1RateList;
			damState2RateList << 96.15728 << 3.83761 << 0.00511;
			casualtyRateListList << damState2RateList;
			damState3RateList << 88.73756 << 11.25222 << 0.01022;
			casualtyRateListList << damState3RateList;
			damState4RateList << 6.20675999999998 << 86.04137 << 7.75187;
			casualtyRateListList << damState4RateList;


			break;
		case (RBuildingCasualtyModel::MH):
			damState1RateList << 99.9035 << 0.0965 << 0;
			casualtyRateListList << damState1RateList;
			damState2RateList << 99.4596 << 0.5404 << 0;
			casualtyRateListList << damState2RateList;
			damState3RateList << 97.87314 << 2.12493 << 0.00193;
			casualtyRateListList << damState3RateList;
			damState4RateList << 84.792758 << 14.899021 << 0.308221;
			casualtyRateListList << damState4RateList;


			break;
		}
	}


	int index = 1;
	for (int i = 1; i < structDamageRatioList.count(); i++) {
		if ((structuralDamageRatio <= structDamageRatioList[i]) && (structuralDamageRatio >= structDamageRatioList[i-1])) {
			index = i;
			break;
		}
	}

	QList<int> injuredPopList;
	injuredPopList << totalPop << 0 << 0;

	for (int i = 0; i < numInjuryLevels; i++) {
		double pop = totalPop * 0.01 * (casualtyRateListList[index][i] * (structuralDamageRatio - structDamageRatioList[index - 1]) + casualtyRateListList[index - 1][i] * (structDamageRatioList[index] - structuralDamageRatio)) / (structDamageRatioList[index] - structDamageRatioList[index - 1]);
		injuredPopList[i] = round(pop);
	}

	//injuredPopList[1] = totalPop - injuredPopList[0] - injuredPopList[2];

	int rescuedUninjuredPopulation = 0;
	int rescuedInjuredPopulation = 0;
	int rescuedDeadPopulation = 0;

	int unrescuedUninjuredPopulation = 0;
	int unrescuedInjuredPopulation = 0;
	int unrescuedDeadPopulation = 0;
	


	// Get the previous values of the number of UU UI UD RU RI RD
	int PreRU= theRescuedUninjuredPopulation->getCurrentValue(threadID);
	int PreRI = theRescuedInjuredPopulation->getCurrentValue(threadID);
	int PreRD = theRescuedDeadPopulation->getCurrentValue(threadID);

	int PreUU = theUnrescuedUninjuredPopulation->getCurrentValue(threadID);
	int PreUI = theUnrescuedInjuredPopulation->getCurrentValue(threadID);
	int PreUD = theUnrescuedDeadPopulation->getCurrentValue(threadID);


	bool alreadyComputedUnrescued = false;
	if (PreUU + PreUI + PreUD > 0) {
		alreadyComputedUnrescued = true;
	}

	if (alreadyComputedUnrescued) {
		if (rescueStatus == 0) {
			unrescuedUninjuredPopulation = PreUU;
			unrescuedInjuredPopulation = PreUI;
			unrescuedDeadPopulation = PreUD;
		}
		if (rescueStatus == 1) {
			rescuedUninjuredPopulation = PreUU;
			rescuedInjuredPopulation = PreUI;
			rescuedDeadPopulation = PreUD;
		}
	}
	else {
		if (rescueStatus == 0) {
			unrescuedUninjuredPopulation = injuredPopList[0];
			unrescuedInjuredPopulation = injuredPopList[1];
			unrescuedDeadPopulation = injuredPopList[2];
		}
		if (rescueStatus == 1) {
			rescuedUninjuredPopulation = injuredPopList[0];
			rescuedInjuredPopulation = injuredPopList[1];
			rescuedDeadPopulation = injuredPopList[2];			
		}
	}
	theRescuedUninjuredPopulation->setCurrentValue(rescuedUninjuredPopulation,threadID);
	theRescuedInjuredPopulation->setCurrentValue(rescuedInjuredPopulation, threadID);
	theRescuedDeadPopulation->setCurrentValue(rescuedDeadPopulation, threadID);

	theUnrescuedUninjuredPopulation->setCurrentValue(unrescuedUninjuredPopulation, threadID);
	theUnrescuedInjuredPopulation->setCurrentValue(unrescuedInjuredPopulation, threadID);
	theUnrescuedDeadPopulation->setCurrentValue(unrescuedDeadPopulation, threadID);


	return 0;
}

bool RBuildingCasualtyModel::canBeRunParallelly() {
	return true;
}

void RBuildingCasualtyModel::resetHistoryVariables(int threadID)
{
	theRescuedUninjuredPopulation->setCurrentValue(0.0, threadID);
	theRescuedInjuredPopulation->setCurrentValue(0.0, threadID);
	theRescuedDeadPopulation->setCurrentValue(0.0, threadID);

	theUnrescuedUninjuredPopulation->setCurrentValue(0.0, threadID);
	theUnrescuedInjuredPopulation->setCurrentValue(0.0, threadID);
	theUnrescuedDeadPopulation->setCurrentValue(0.0, threadID);

}
