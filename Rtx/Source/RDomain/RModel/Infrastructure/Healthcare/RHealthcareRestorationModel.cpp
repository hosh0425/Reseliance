#include "RHealthcareRestorationModel.h"	

#include "RParameter.h"	
#include "RResponse.h"	
#include "RHealthcareInformationModel.h"
#include <math.h>	

RHealthcareRestorationModel::RHealthcareRestorationModel(QObject *parent, QString name)
	: RModel(parent, name)
{
	// Initializing the data members
	//theHealthcareInformationModel = 0;
	//theStructuralDamageRatio = 0;
	//therestorationTimeUncertainty = 0;
	//theHealthcareServiceability = 0;

	// Instantiating the response object
	theDomain->createObject(objectName() + "TimeResponse", "RGenericResponse");
	theRestorationTime = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theRestorationTime->setModel(this);

	theDomain->createObject(objectName() + "StatusResponse", "RGenericResponse");
	theRestorationStatus = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theRestorationStatus->setModel(this);
	theRestorationStatus->setCurrentValue(1);
}

RHealthcareRestorationModel::~RHealthcareRestorationModel()
{

}

QObject * RHealthcareRestorationModel::getHealthcareInformationModel() const
{
	return theHealthcareInformationModel.data();
}

void RHealthcareRestorationModel::setHealthcareInformationModel(QObject *value)
{
	theHealthcareInformationModel = qobject_cast<RModel *>(value);
}

QObject * RHealthcareRestorationModel::getStructuralDamageRatio() const
{
	return theStructuralDamageRatio.data();
}

void RHealthcareRestorationModel::setStructuralDamageRatio(QObject *value)
{
	theStructuralDamageRatio = qobject_cast<RParameter *>(value);
}

QObject * RHealthcareRestorationModel::getRestorationTimeUncertainty() const
{
	return theRestorationTimeUncertainty.data();
}

void RHealthcareRestorationModel::setRestorationTimeUncertainty(QObject *value)
{
	theRestorationTimeUncertainty = qobject_cast<RParameter *>(value);
}

int RHealthcareRestorationModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
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


	// Getting the information from the HIM
	RHealthcareInformationModel *theHIM = qobject_cast<RHealthcareInformationModel *>(theHealthcareInformationModel.data());
	if (!theHIM) {
		qCritical() << "Error in model" << objectName() << ":" << theHealthcareInformationModel.data()->objectName() << "is not a Healthcare information model.";
		return -1;
	}

	double structuralDamageRatio = theStructuralDamageRatio.data()->getCurrentValue(threadID);
	if (structuralDamageRatio > 1)  structuralDamageRatio = 1;
	if (structuralDamageRatio < 0)  structuralDamageRatio = 0;

	//Get the input standard normal random variable
	double randNum1 = theRestorationTimeUncertainty.data()->getCurrentValue(threadID);

	//Computing the repair time based on the FEMA
	QList<double> structDamageRatioList;
	structDamageRatioList << 0 << 0.02 << 0.1 << 0.5 << 1.0;
	
	//Vectors of mean repair time and stdev
	QList<double> meanTimeList;
	QList<double> stdevTimeList;

	//Parameters of restoration time per Table 6.11 of FEMA - p288
	meanTimeList << 0 << 5 << 20 << 90 << 180;
	stdevTimeList << 0 << 1 << 2 << 10 << 20;

	//THE ALGORITHM
	int index = 1;
	for (int i = 1; i < structDamageRatioList.count(); i++) {
		if ((structuralDamageRatio <= structDamageRatioList[i]) && (structuralDamageRatio >= structDamageRatioList[i-1])) {
			index = i;
			break;
		}
	}
	double meanRestorationTime = (meanTimeList[index] * (structuralDamageRatio - structDamageRatioList[index - 1]) + meanTimeList[index - 1] * (structDamageRatioList[index] - structuralDamageRatio)) / (structDamageRatioList[index] - structDamageRatioList[index - 1]);
	double stdevRestorationTime = (stdevTimeList[index] * (structuralDamageRatio - structDamageRatioList[index - 1]) + stdevTimeList[index - 1] * (structDamageRatioList[index] - structuralDamageRatio)) / (structDamageRatioList[index] - structDamageRatioList[index - 1]);

	//Compute the restoration time
	double restorationTime = 0;
	restorationTime = randNum1*stdevRestorationTime + meanRestorationTime;

	//if restoration time is less than one day it is assumed to be zero
	if (restorationTime<1) {
		restorationTime = 0;
	}
	double restorationStatus = 0;
	if (restorationTime == 0) {
		restorationStatus = 1;
	}

	theRestorationTime->setCurrentValue(restorationTime, threadID);
	theRestorationStatus->setCurrentValue(restorationStatus, threadID);

	return 0;
}

bool RHealthcareRestorationModel::canBeRunParallelly() {
	return true;
}