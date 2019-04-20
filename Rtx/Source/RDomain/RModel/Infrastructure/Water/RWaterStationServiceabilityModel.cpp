#include "RWaterStationServiceabilityModel.h"	

#include "RParameter.h"	
#include "RResponse.h"	
#include "RWaterStationInformationModel.h"


#include <math.h>

RWaterStationServiceabilityModel::RWaterStationServiceabilityModel(QObject *parent, QString name)
	: RModel(parent, name)
{
	// Initializing the data members
	//theWaterStationInformationModel = 0;
	//theDamageRatio = 0;
	//theDamageState = 0;


	// Instantiating the response object Serviceability
	theDomain->createObject(objectName() + "Response", "RGenericResponse");
	theServiceability = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theServiceability->setModel(this);
	theServiceability->setCurrentValue(1.0);
}

RWaterStationServiceabilityModel::~RWaterStationServiceabilityModel()
{

}

QObject * RWaterStationServiceabilityModel::getWaterStationInformationModel() const
{
	return theWaterStationInformationModel.data();
}

void RWaterStationServiceabilityModel::setWaterStationInformationModel(QObject *value)
{
	theWaterStationInformationModel = qobject_cast<RModel *>(value);
}

QObject * RWaterStationServiceabilityModel::getFunctionality() const
{
	return theFunctionality.data();
}

void RWaterStationServiceabilityModel::setFunctionality(QObject *value)
{
	theFunctionality = qobject_cast<RParameter *>(value);
}

QObject * RWaterStationServiceabilityModel::getPowerServiceability() const
{
	return thePowerServiceability.data();
}

void RWaterStationServiceabilityModel::setPowerServiceability(QObject *value)
{
	thePowerServiceability = qobject_cast<RParameter *>(value);
}

int RWaterStationServiceabilityModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
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


	// Getting the information from the WSIM
	RWaterStationInformationModel *theWSIM = qobject_cast<RWaterStationInformationModel *>(theWaterStationInformationModel.data());
	if (!theWSIM) {
		qCritical() << "Error in model" << objectName() << ":" << theWaterStationInformationModel.data()->objectName() << "is not a Water Station information model.";
		return -1;
	}

	double type = theWSIM->getType();
	double components = theWSIM->getComponents();

	double functionality = theFunctionality.data()->getCurrentValue(threadID);
	double powerServiecability = thePowerServiceability.data()->getCurrentValue(threadID);

	//THE ALGORITHM
	//This model is based on engineering judgment and experts' opinion
	double serviceability = 0.0;
	if ((functionality == 1.0) && (powerServiecability == 1.0)) {
		serviceability = 1.0;
	}
	theServiceability->setCurrentValue(serviceability, threadID);

	return 0;
}

bool RWaterStationServiceabilityModel::canBeRunParallelly() {
	return true;
}