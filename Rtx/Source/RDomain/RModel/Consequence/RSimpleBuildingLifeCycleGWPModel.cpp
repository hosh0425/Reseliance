#include "RSimpleBuildingLifeCycleGWPModel.h"

#include "RParameter.h"
#include "RResponse.h"

RSimpleBuildingLifeCycleGWPModel::RSimpleBuildingLifeCycleGWPModel(QObject *parent, QString &name)
	: RModel(parent, name)
{
	// Initializing the pointer data members
	/* //$$ 
	theLifeCycleEnergy = 0;
	theOperationEnergy = 0;
	theGWPIntensityOperation = 0;
	theGWPIntensityNonOperation = 0;
	*/

	// Instantiating the output response
	theDomain->createObject(objectName() + "Response", "RGenericResponse");
	// Setting theResponse pointer
	theGWPCoarseResponse = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	// Setting the "model" property of theResponse to be "this" object
	theGWPCoarseResponse->setModel(this);
}

RSimpleBuildingLifeCycleGWPModel::~RSimpleBuildingLifeCycleGWPModel()
{

}

QObject * RSimpleBuildingLifeCycleGWPModel::getLifeCycleEnergy() const
{
	return theLifeCycleEnergy.data();
}

void RSimpleBuildingLifeCycleGWPModel::setLifeCycleEnergy(QObject *value)
{
	theLifeCycleEnergy = qobject_cast<RParameter *>(value);
}

QObject * RSimpleBuildingLifeCycleGWPModel::getOperationEnergy() const
{
	return theOperationEnergy.data();
}

void RSimpleBuildingLifeCycleGWPModel::setOperationEnergy(QObject *value)
{
	theOperationEnergy = qobject_cast<RParameter *>(value);
}

QObject * RSimpleBuildingLifeCycleGWPModel::getGWPIntensityOperation() const
{
	return theGWPIntensityOperation.data();
}

void RSimpleBuildingLifeCycleGWPModel::setGWPIntensityOperation(QObject *value)
{
	theGWPIntensityOperation = qobject_cast<RParameter *>(value);
}

QObject * RSimpleBuildingLifeCycleGWPModel::getGWPIntensityNonOperation() const
{
	return theGWPIntensityNonOperation.data();
}

void RSimpleBuildingLifeCycleGWPModel::setGWPIntensityNonOperation(QObject *value)
{
	theGWPIntensityNonOperation = qobject_cast<RParameter *>(value);
}


int RSimpleBuildingLifeCycleGWPModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
{	
	// Extract the input parameter values
	double lce = theLifeCycleEnergy.data()->getCurrentValue(threadID); 
	double oe = theOperationEnergy.data()->getCurrentValue(threadID);
	double iop = theGWPIntensityOperation.data()->getCurrentValue(threadID);
	double in = theGWPIntensityNonOperation.data()->getCurrentValue(threadID);



	double response = (lce-oe)*in+oe*iop;

	// Updating the response current value
	theGWPCoarseResponse->setCurrentValue(response,threadID);

	return 0;
}

bool RSimpleBuildingLifeCycleGWPModel::canBeRunParallelly(){
	return true;
}