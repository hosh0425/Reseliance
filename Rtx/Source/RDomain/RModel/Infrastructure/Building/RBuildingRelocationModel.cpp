#include "RBuildingRelocationModel.h"	

#include "RParameter.h"	
#include "RResponse.h"	
#include "RBuildingInformationModel.h"
#include <math.h>	

RBuildingRelocationModel::RBuildingRelocationModel(QObject *parent, QString name)
	: RModel(parent, name)
{
	// Initializing the data members
	//theBuildingInformationModel = 0;

	int arraySizeForParallelizedObject = getArraySizeForParallelizedObject();

	currentTime = new double[arraySizeForParallelizedObject];
	previousTime = new double[arraySizeForParallelizedObject];
	//currentDemand = new double[arraySizeForParallelizedObject];
	//previousDemand = new double[arraySizeForParallelizedObject];
	//cumulativeRelocationCost = new double[arraySizeForParallelizedObject];

	// Instantiating the response object
	theDomain->createObject(objectName() + "Response", "RGenericResponse");
	theBuildingRelocationCost = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theBuildingRelocationCost->setModel(this);

}

RBuildingRelocationModel::~RBuildingRelocationModel()
{
	delete[] currentTime;
	delete[] previousTime;
	//delete[] currentDemand;
	//delete[] previousDemand;
	//delete[] cumulativeRelocationCost;
}

QObject * RBuildingRelocationModel::getBuildingInformationModel() const
{
	return theBuildingInformationModel.data();
}

void RBuildingRelocationModel::setBuildingInformationModel(QObject *value)
{
	theBuildingInformationModel = qobject_cast<RModel *>(value);
}

QObject * RBuildingRelocationModel::getTimeParameter() const
{
	return theTimeParameter.data();
}

void RBuildingRelocationModel::setTimeParameter(QObject *value)
{
	theTimeParameter = qobject_cast<RParameter *>(value);
}

QObject * RBuildingRelocationModel::getStructuralDamageRatio() const
{
	return theStructuralDamageRatio.data();
}

void RBuildingRelocationModel::setStructuralDamageRatio(QObject *value)
{
	theStructuralDamageRatio = qobject_cast<RParameter *>(value);
}

QObject * RBuildingRelocationModel::getRepairOrRestorationStatus() const
{
	return theRepairOrRestorationStatus.data();
}

void RBuildingRelocationModel::setRepairOrRestorationStatus(QObject *value)
{
	theRepairOrRestorationStatus = qobject_cast<RParameter *>(value);
}

int RBuildingRelocationModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
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
	double area = theBIM->getTotalArea();
	double numStories = theBIM->getTotalNumberOfStoreys();

	double floorAreaMeter2 = area / numStories;
	double floorAreaFoot2 = 10.7639 * floorAreaMeter2;

	double structuralDamageRatio = theStructuralDamageRatio.data()->getCurrentValue(threadID);

	//According to FEMA Table 15.12 and Table 15.13
	double percentOwnerOccupied = 0.0;
	double RentalCost = 0.0;
	double DisruptionCosts = 0.0;

	switch (theBIM->getOccupancyType()) {

	case (RBuildingInformationModel::SingleResidence):
		percentOwnerOccupied = 0.80;
		RentalCost = 0.58 ;
		DisruptionCosts = 0.82 ;
		break;
	case (RBuildingInformationModel::MultiResidence):
		percentOwnerOccupied = 0.35;
		RentalCost = .61 ;
		DisruptionCosts = 0.82 ;
		break;
	case (RBuildingInformationModel::InstitutionalResidence):
		percentOwnerOccupied = 0.0;
		RentalCost = 0.41 ;
		DisruptionCosts = 0.82 ;
		break;
	case (RBuildingInformationModel::Trade):
		percentOwnerOccupied = 0.55;
		RentalCost = 0.82 ;
		DisruptionCosts = 1.0 ;
		break;
	case (RBuildingInformationModel::Services):
		percentOwnerOccupied = 0.55;
		RentalCost = 1.36 ;
		DisruptionCosts = 0.95 ;
		break;
	case (RBuildingInformationModel::Medical):
		percentOwnerOccupied = 0.95;
		RentalCost = 1.36 ;
		DisruptionCosts = 1.36 ;
		break;
	case (RBuildingInformationModel::Recreation):
		percentOwnerOccupied = 0.55;
		RentalCost = 1.70 ;
		DisruptionCosts = 0.0;
		break;
	case (RBuildingInformationModel::Parking):
		percentOwnerOccupied = 0.25;
		RentalCost = 0.34 ;
		DisruptionCosts = 0.0;
		break;
	case (RBuildingInformationModel::School):
		percentOwnerOccupied = 0.95;
		RentalCost = 1.02 ;
		DisruptionCosts = 0.95 ;
		break;
	case (RBuildingInformationModel::University):
		percentOwnerOccupied = 0.90;
		RentalCost = 1.36 ;
		DisruptionCosts = 0.95 ;
		break;
	case (RBuildingInformationModel::Government):
		percentOwnerOccupied = 0.825;
		RentalCost = 1.36 ;
		DisruptionCosts = 0.95 ;
		break;
	case (RBuildingInformationModel::Industry):
		percentOwnerOccupied = 0.75;
		RentalCost = 0.24 ;
		DisruptionCosts = 0.95 ;
		break;
	case (RBuildingInformationModel::Agriculture):
		percentOwnerOccupied = 0.95;
		RentalCost = 0.68 ;
		DisruptionCosts = 0.68 ;
		break;
	}


	currentTime[threadID] = theTimeParameter.data()->getCurrentValue(threadID);

	double relocationCost = 0.0;

	//Damage ratio 0.05 is equivalent to Damage state moderate and more - Expert opinion
	if (structuralDamageRatio > 0.05) {
		//According to FEMA formula 15-14
		double relocationDurationInMonth = (currentTime[threadID] - previousTime[threadID]) / 30;
		relocationCost = floorAreaFoot2 * ((1 - percentOwnerOccupied) * DisruptionCosts + percentOwnerOccupied * (DisruptionCosts + RentalCost*relocationDurationInMonth));
	}

	double RepairOrRestorationStatus = theRepairOrRestorationStatus.data()->getCurrentValue(threadID);


	//THE ALGORITHM
	//Compute the relocation cost
	theBuildingRelocationCost->setCurrentValue(relocationCost, threadID);
	
	previousTime[threadID] = currentTime[threadID] ;
	
	return 0;
}

bool RBuildingRelocationModel::canBeRunParallelly() {
	return true;
}

void RBuildingRelocationModel::reset()
{
	theInitialTime = theTimeParameter.data()->getCurrentValue();
}

void RBuildingRelocationModel::resetHistoryVariables(int threadID)
{
	//cumulativeRelocationCost[threadID] = 0;

	currentTime[threadID] = theInitialTime;
	previousTime[threadID] = theInitialTime;

	//currentDemand[threadID] = 0;
	//previousDemand[threadID] = 0;
}