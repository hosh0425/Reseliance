//Developed by Hossein Nasr
#include "RHealthcareTreatmentTimeModel.h"

#include <QCoreApplication>

#include "RParameter.h"
#include "RResponse.h"
#include "RHealthcareInformationModel.h"


RHealthcareTreatmentTimeModel::RHealthcareTreatmentTimeModel(QObject *parent, QString name)
	: RModel(parent, name)
{

	// Instantiating the response object
	theDomain->createObject(objectName() + "Response", "RGenericResponse");
	theHealthcareTreatmentTime = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theHealthcareTreatmentTime->setModel(this);

}

RHealthcareTreatmentTimeModel::~RHealthcareTreatmentTimeModel()
{

}

QObject * RHealthcareTreatmentTimeModel::getHealthcareInformationModel() const
{
	return theHealthcareInformationModel.data();
}

void RHealthcareTreatmentTimeModel::setHealthcareInformationModel(QObject *value)
{
	theHealthcareInformationModel = qobject_cast<RModel *>(value);
}

QObject * RHealthcareTreatmentTimeModel::getTreatmentTimeParameter() const
{
	return theTreatmentTimeParameter.data();
}

void RHealthcareTreatmentTimeModel::setTreatmentTimeParameter(QObject *value)
{
	theTreatmentTimeParameter = qobject_cast<RParameter *>(value);
}

int RHealthcareTreatmentTimeModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
{
	// Checking if all pointer properties are given by the user
	int status = this->checkProperties();
	// Error handling
	if (status < 0) {
		// Printing an error message to the output pane
		rCritical(threadID) << "Error in" << this->objectName() << ":"
			<< "Could not find all the passed objects";
		// Returning the error code
		return -1;
	}

	// Getting the information from the HIM
	RHealthcareInformationModel *theHIM = qobject_cast<RHealthcareInformationModel *>(theHealthcareInformationModel.data());
	if (!theHIM) {
		rCritical(threadID) << "Error in model" << objectName() << ":" << theHealthcareInformationModel.data()->objectName() << "is not a Healthcare information model.";
		return -1;
	}
	
	double treatmentTime = theTreatmentTimeParameter.data()->getCurrentValue(threadID);
	theHealthcareTreatmentTime->setCurrentValue(treatmentTime, threadID);

	return 0;
}

bool RHealthcareTreatmentTimeModel::canBeRunParallelly() {
	return true;
}

