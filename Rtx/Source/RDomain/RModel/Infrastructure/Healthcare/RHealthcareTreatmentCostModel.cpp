//Developed by Hossein Nasr
#include "RHealthcareTreatmentCostModel.h"

#include <QCoreApplication>

#include "RParameter.h"
#include "RResponse.h"
#include "RHealthcareInformationModel.h"



RHealthcareTreatmentCostModel::RHealthcareTreatmentCostModel(QObject *parent, QString name)
	: RModel(parent, name)
{
	theNumberOfInjuryLevels = 1;

	int arraySizeForParallelizedObject = getArraySizeForParallelizedObject();
	previousNumTreatedList = new QList<double>[arraySizeForParallelizedObject];

	// Instantiating the response object
	theDomain->createObject(objectName() + "Response", "RGenericResponse");
	theHealthcareTreatmentCost = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theHealthcareTreatmentCost->setModel(this);

}

RHealthcareTreatmentCostModel::~RHealthcareTreatmentCostModel()
{
	delete[] previousNumTreatedList;
}

QObject * RHealthcareTreatmentCostModel::getHealthcareInformationModel() const
{
	return theHealthcareInformationModel.data();
}

void RHealthcareTreatmentCostModel::setHealthcareInformationModel(QObject *value)
{
	theHealthcareInformationModel = qobject_cast<RModel *>(value);
}

int RHealthcareTreatmentCostModel::getNumberOfInjuryLevels()
{
	return theNumberOfInjuryLevels;
}

void RHealthcareTreatmentCostModel::setNumberOfInjuryLevels(int value)
{
	theNumberOfInjuryLevels = value;
	previousNumTreatedList[0].clear();
	for (int i = 0; i < theNumberOfInjuryLevels; i++) {
		previousNumTreatedList[0] << 0;
	}
}

QString RHealthcareTreatmentCostModel::getNumberOfTreatedInjuriesList()
{
	return parameterListToString(theNumberOfTreatedInjuriesList);
}

void RHealthcareTreatmentCostModel::setNumberOfTreatedInjuriesList(QString value)
{
	theNumberOfTreatedInjuriesList = stringToParameterList(value);
}

QString RHealthcareTreatmentCostModel::getUnitCostOfTreatmentList()
{
	return parameterListToString(theUnitCostOfTreatmentList);
}

void RHealthcareTreatmentCostModel::setUnitCostOfTreatmentList(QString value)
{
	theUnitCostOfTreatmentList = stringToParameterList(value);
}

int RHealthcareTreatmentCostModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
{

	//Check the operation status
	QList<RParameter *> numberOfTreatedInjuriesList;
	for (int i = 0; i< theNumberOfTreatedInjuriesList.count(); i++) {
		RParameter *parameter = qobject_cast<RParameter *>(theNumberOfTreatedInjuriesList[i].data());
		if (parameter) {
			numberOfTreatedInjuriesList << parameter;
		}
		else {
			// Issue an error if the parameter is not a parameter
			rCritical() << "Error in" << objectName() << ": The parameter %1 is not a parameter. Please correct and try again";
			return -1;
		}
	}

	// Getting the information from the HIM
	RHealthcareInformationModel *theHIM = qobject_cast<RHealthcareInformationModel *>(theHealthcareInformationModel.data());
	if (!theHIM) {
		rCritical(threadID) << "Error in model" << objectName() << ":" << theHealthcareInformationModel.data()->objectName() << "is not a Healthcare information model.";
		return -1;
	}

	QList<RParameter *> unitCostOfTreatmentList;
	for (int i = 0; i< theUnitCostOfTreatmentList.count(); i++) {
		RParameter *parameter = qobject_cast<RParameter *>(theUnitCostOfTreatmentList[i].data());
		if (parameter) {
			unitCostOfTreatmentList << parameter;
		}
		else {
			// Issue an error if the parameter is not a parameter
			rCritical() << "Error in" << objectName() << ": The parameter %1 is not a parameter. Please correct and try again";
			return -1;
		}
	}

	if (numberOfTreatedInjuriesList.count() != theNumberOfInjuryLevels) {
		rCritical() << "Error in" << objectName() << ": Please enter as many number of Treated Injuries as the Number of Injury Levels";
		return -1;
	}
	if (unitCostOfTreatmentList.count() != theNumberOfInjuryLevels) {
		rCritical() << "Error in" << objectName() << ": Please enter as many number of unit cost of treatment as the Number of Injury Levels";
		return -1;
	}
	QList<double> newNumTreatedList;
	for (int i = 0; i < theNumberOfInjuryLevels; i++) {
		double newNum = numberOfTreatedInjuriesList[i]->getCurrentValue(threadID) - previousNumTreatedList[threadID][i];
		newNumTreatedList << newNum;
	}

	double newCost = 0;
	for (int i = 0; i < theNumberOfInjuryLevels; i++) {
		newCost += newNumTreatedList[i] * unitCostOfTreatmentList[i]->getCurrentValue(threadID);
	}
	double previousCost = theHealthcareTreatmentCost->getCurrentValue(threadID);
	double totalCost = newCost + previousCost;

	theHealthcareTreatmentCost->setCurrentValue(totalCost, threadID);

	for (int i = 0; i < theNumberOfInjuryLevels; i++) {
		previousNumTreatedList[threadID][i] = numberOfTreatedInjuriesList[i]->getCurrentValue(threadID);
	}

	return 0;
}

bool RHealthcareTreatmentCostModel::canBeRunParallelly() {
	return true;
}

void RHealthcareTreatmentCostModel::resetHistoryVariables(int threadID)
{
	for (int i = 0; i < theNumberOfTreatedInjuriesList.count(); i++) {
		theNumberOfTreatedInjuriesList[i].data()->setCurrentValue(0,threadID);
	}

	previousNumTreatedList[threadID].clear();
	for (int i = 0; i < theNumberOfInjuryLevels; i++) {
		previousNumTreatedList[threadID] << 0;
	}

	theHealthcareTreatmentCost->setCurrentValue(0, threadID);
}