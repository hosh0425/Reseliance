#include "RPowerStationServiceabilityModel.h"	

#include "RParameter.h"	
#include "RResponse.h"	
#include "RPowerStationInformationModel.h"


#include <math.h>

RPowerStationServiceabilityModel::RPowerStationServiceabilityModel(QObject *parent, QString name)
	: RModel(parent, name)
{
	// Initializing the data members
	//thePowerStationInformationModel = 0;
	//theDamageRatio = 0;
	//theDamageState = 0;


	// Instantiating the response object Serviceability
	theDomain->createObject(objectName() + "Response", "RGenericResponse");
	theServiceability = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theServiceability->setModel(this);
	theServiceability->setCurrentValue(1.0);
}

RPowerStationServiceabilityModel::~RPowerStationServiceabilityModel()
{

}

QObject * RPowerStationServiceabilityModel::getPowerStationInformationModel() const
{
	return thePowerStationInformationModel.data();
}

void RPowerStationServiceabilityModel::setPowerStationInformationModel(QObject *value)
{
	thePowerStationInformationModel = qobject_cast<RModel *>(value);
}

QObject * RPowerStationServiceabilityModel::getDamageRatio() const
{
	return theDamageRatio.data();
}

void RPowerStationServiceabilityModel::setDamageRatio(QObject *value)
{
	theDamageRatio = qobject_cast<RParameter *>(value);
}

QObject * RPowerStationServiceabilityModel::getDamageState() const
{
	return theDamageState.data();
}

void RPowerStationServiceabilityModel::setDamageState(QObject *value)
{
	theDamageState = qobject_cast<RParameter *>(value);
}

int RPowerStationServiceabilityModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
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


	// Getting the information from the PSIM
	RPowerStationInformationModel *thePSIM = qobject_cast<RPowerStationInformationModel *>(thePowerStationInformationModel.data());
	if (!thePSIM) {
		qCritical() << "Error in model" << objectName() << ":" << thePowerStationInformationModel.data()->objectName() << "is not a Power Station information model.";
		return -1;
	}

	double type = thePSIM->getType();
	double components = thePSIM->getComponents();
	double damageRatio = theDamageRatio.data()->getCurrentValue(threadID);
	int damageState = theDamageState.data()->getCurrentValue(threadID);

	//THE ALGORITHM
	//This model is based on engineering judgment and experts' opinion
	double serviceability = 1.0;
	if ((damageState>1) || (damageRatio>0.1)) {
		serviceability = 0.0;
	}
	theServiceability->setCurrentValue(serviceability, threadID);

	//qCritical() << damageState;
	//qCritical() << damageRatio;
	//qCritical() << serviceability ;

	return 0;
}

bool RPowerStationServiceabilityModel::canBeRunParallelly() {
	return true;
}