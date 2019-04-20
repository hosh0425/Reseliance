#include "RHealthcareInformationModel.h"

#include "RParameter.h"
#include "RResponse.h"
#include "RTime.h"

RHealthcareInformationModel::RHealthcareInformationModel(QObject *parent, QString name)
	: RModel(parent, name)
{
	//theLocation = 0;
	//theTimeOfEstablishment = 0;

	//FEMA Table 3.11: Essential Facilities Classification
	theNumberOfBeds = 100;
	theSector = RHealthcareInformationModel::Public;
	theSize = RHealthcareInformationModel::Medium;
	theType = RHealthcareInformationModel::Hospital;

}
RHealthcareInformationModel::~RHealthcareInformationModel()
{

}

QObject * RHealthcareInformationModel::getBuildingInformationModel() const
{
	return theBuildingInformationModel.data();
}

void RHealthcareInformationModel::setBuildingInformationModel(QObject *value)
{
	theBuildingInformationModel = qobject_cast<RModel *>(value);
}

double RHealthcareInformationModel::getNumberOfBeds() const
{
	return theNumberOfBeds;
}

void RHealthcareInformationModel::setNumberOfBeds(double value)
{
	theNumberOfBeds = value;
}

RHealthcareInformationModel::RSector RHealthcareInformationModel::getSector() const
{
	return theSector;
}

void RHealthcareInformationModel::setSector(RHealthcareInformationModel::RSector value)
{
	theSector = value;
}

RHealthcareInformationModel::RSize RHealthcareInformationModel::getSize() const
{
	return theSize;
}

void RHealthcareInformationModel::setSize(RHealthcareInformationModel::RSize value)
{
	theSize = value;
}

RHealthcareInformationModel::RType RHealthcareInformationModel::getType() const
{
	return theType;
}

void RHealthcareInformationModel::setType(RHealthcareInformationModel::RType value)
{
	theType = value;
}

int RHealthcareInformationModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
{
	return 0;
}

bool RHealthcareInformationModel::canBeRunParallelly() {
	return true;
}
