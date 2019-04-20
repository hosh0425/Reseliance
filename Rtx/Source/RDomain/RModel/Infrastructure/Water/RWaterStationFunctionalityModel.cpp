#include "RWaterStationFunctionalityModel.h"	

#include "RParameter.h"	
#include "RResponse.h"	
#include "RWaterStationInformationModel.h"


#include <math.h>

RWaterStationFunctionalityModel::RWaterStationFunctionalityModel(QObject *parent, QString name)
	: RModel(parent, name)
{
	// Initializing the data members
	//theWaterStationInformationModel = 0;
	//theDamageRatio = 0;
	//theDamageState = 0;


	// Instantiating the response object Functionality
	theDomain->createObject(objectName() + "Response", "RGenericResponse");
	theFunctionality = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theFunctionality->setModel(this);
	theFunctionality->setCurrentValue(1.0);
}

RWaterStationFunctionalityModel::~RWaterStationFunctionalityModel()
{

}

QObject * RWaterStationFunctionalityModel::getWaterStationInformationModel() const
{
	return theWaterStationInformationModel.data();
}

void RWaterStationFunctionalityModel::setWaterStationInformationModel(QObject *value)
{
	theWaterStationInformationModel = qobject_cast<RModel *>(value);
}

QObject * RWaterStationFunctionalityModel::getDamageRatio() const
{
	return theDamageRatio.data();
}

void RWaterStationFunctionalityModel::setDamageRatio(QObject *value)
{
	theDamageRatio = qobject_cast<RParameter *>(value);
}

QObject * RWaterStationFunctionalityModel::getDamageState() const
{
	return theDamageState.data();
}

void RWaterStationFunctionalityModel::setDamageState(QObject *value)
{
	theDamageState = qobject_cast<RParameter *>(value);
}
int RWaterStationFunctionalityModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
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
	double damageRatio = theDamageRatio.data()->getCurrentValue(threadID);
	int damageState = theDamageState.data()->getCurrentValue(threadID);

	//THE ALGORITHM
	//This model is based on engineering judgment and experts' opinion
	double Functionality = 1.0;
	if ((damageState>1) || (damageRatio>0.1)) {
		Functionality = 0.0;
	}
	theFunctionality->setCurrentValue(Functionality, threadID);

	//qCritical() << damageState;
	//qCritical() << damageRatio;
	//qCritical() << Functionality ;

	return 0;
}

bool RWaterStationFunctionalityModel::canBeRunParallelly() {
	return true;
}