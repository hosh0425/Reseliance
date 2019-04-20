//Developed by Hossein Nasr
#include "RHealthcareDemandModel.h"

#include <QCoreApplication>

#include "RParameter.h"
#include "RResponse.h"
#include "RContinuousRandomVariable.h"
#include "RTime.h"
#include <iterator>
#include <qvariant.h>
#include <math.h>
#include <stdio.h>
#include <gsl/gsl_statistics.h>

RHealthcareDemandModel::RHealthcareDemandModel(QObject *parent, QString name)
	: RModel(parent, name)
{
	// Instantiating the response object
	theDomain->createObject(objectName() + "AllResponse", "RGenericResponse");
	theHealthcareAllDemand = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theHealthcareAllDemand->setModel(this);

	// Instantiating the response object
	theDomain->createObject(objectName() + "NewResponse", "RGenericResponse");
	theHealthcareNewDemand = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theHealthcareNewDemand->setModel(this);

}

RHealthcareDemandModel::~RHealthcareDemandModel()
{

}

QString RHealthcareDemandModel::getNumberOfRescuedInjuriesList()
{
	return parameterListToString(theNumberOfRescuedInjuriesList);
}

void RHealthcareDemandModel::setNumberOfRescuedInjuriesList(QString value)
{
	theNumberOfRescuedInjuriesList = stringToParameterList(value);
}

int RHealthcareDemandModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
{

	//Check the operation status
	QList<RParameter *> NumberOfRescuedInjuriesList;
	for (int i = 0; i< theNumberOfRescuedInjuriesList.count(); i++) {
		RParameter *parameter = qobject_cast<RParameter *>(theNumberOfRescuedInjuriesList[i].data());
		if (parameter) {
			NumberOfRescuedInjuriesList << parameter;
		}
		else {
			// Issue an error if the parameter is not a parameter
			rCritical() << "Error in" << objectName() << ": The parameter %1 is not a parameter. Please correct and try again";
			return -1;
		}
	}

	int allDemand = 0;
	for (int i = 0; i < NumberOfRescuedInjuriesList.count(); i++) {
		allDemand += NumberOfRescuedInjuriesList[i]->getCurrentValue(threadID);
	}

	int previousDemand = theHealthcareAllDemand->getCurrentValue(threadID);
	int newDemand = allDemand - previousDemand;

	theHealthcareAllDemand->setCurrentValue(allDemand, threadID);
	theHealthcareNewDemand->setCurrentValue(newDemand, threadID);

	return 0;
}

bool RHealthcareDemandModel::canBeRunParallelly() {
	return true;
}

void RHealthcareDemandModel::resetHistoryVariables(int threadID)
{
	theHealthcareNewDemand->setCurrentValue(0, threadID);
	theHealthcareAllDemand->setCurrentValue(0, threadID);
}