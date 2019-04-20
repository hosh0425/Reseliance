//Developed by Hossein Nasr
#include "RBuildingFatalityCostModel.h"

#include <QCoreApplication>

#include "RParameter.h"
#include "RResponse.h"
#include "RBuildingInformationModel.h"



RBuildingFatalityCostModel::RBuildingFatalityCostModel(QObject *parent, QString name)
	: RModel(parent, name)
{
	int arraySizeForParallelizedObject = getArraySizeForParallelizedObject();
	previousNumFat = new double [arraySizeForParallelizedObject];

	// Instantiating the response object
	theDomain->createObject(objectName() + "Response", "RGenericResponse");
	theFatalityCost = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theFatalityCost->setModel(this);

}

RBuildingFatalityCostModel::~RBuildingFatalityCostModel()
{
	delete[] previousNumFat;
}

QObject * RBuildingFatalityCostModel::getBuildingInformationModel() const
{
	return theBuildingInformationModel.data();
}

void RBuildingFatalityCostModel::setBuildingInformationModel(QObject *value)
{
	theBuildingInformationModel = qobject_cast<RModel *>(value);
}

QObject *RBuildingFatalityCostModel::getNumberOfFatalities() const
{
	return theNumberOfFatalities.data();
}

void RBuildingFatalityCostModel::setNumberOfFatalities(QObject *value)
{
	theNumberOfFatalities = qobject_cast<RParameter *>(value);
}

QObject *RBuildingFatalityCostModel::getCostOfOneFatality() const
{
	return theCostOfOneFatality.data();
}

void RBuildingFatalityCostModel::setCostOfOneFatality(QObject *value)
{
	theCostOfOneFatality = qobject_cast<RParameter *>(value);
}

int RBuildingFatalityCostModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
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
		rCritical(threadID) << "Error in model" << objectName() << ":" << theBuildingInformationModel.data()->objectName() << "is not a Building information model.";
		return -1;
	}
	double unitFatCost = theCostOfOneFatality.data()->getCurrentValue(threadID);;

	double newNumFat = theNumberOfFatalities.data()->getCurrentValue(threadID) - previousNumFat[threadID];

	//Each time this model is called, the new cost is computed and added to the previously computed costs
	double newCost = 0;
	newCost = newNumFat * unitFatCost;

	double previousCost = theFatalityCost->getCurrentValue(threadID);

	double totalCost = newCost + previousCost;

	theFatalityCost->setCurrentValue(totalCost, threadID);

	previousNumFat[threadID] = theNumberOfFatalities.data()->getCurrentValue(threadID);

	return 0;
}

bool RBuildingFatalityCostModel::canBeRunParallelly() {
	return true;
}

void RBuildingFatalityCostModel::resetHistoryVariables(int threadID)
{
	//At the start of each sample data members and response is set to zero
	previousNumFat[threadID] = 0;
	theFatalityCost->setCurrentValue(0, threadID);
}