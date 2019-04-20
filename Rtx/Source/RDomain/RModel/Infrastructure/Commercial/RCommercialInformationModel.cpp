#include "RCommercialInformationModel.h"

#include "RParameter.h"
#include "RResponse.h"
#include "RTime.h"

RCommercialInformationModel::RCommercialInformationModel(QObject *parent, QString name)
	: RModel(parent, name)
{
	//theLocation = 0;
	//theTimeOfEstablishment = 0;

	// The default replacement values are in Table 15.17 of FEMA
	theNumberOfEmployees = 10;
	theSector = RCommercialInformationModel::LocallyOriented;
	theSize = RCommercialInformationModel::Small;

}
RCommercialInformationModel::~RCommercialInformationModel()
{

}

QObject * RCommercialInformationModel::getBuildingInformationModel() const
{
	return theBuildingInformationModel.data();
}

void RCommercialInformationModel::setBuildingInformationModel(QObject *value)
{
	theBuildingInformationModel = qobject_cast<RModel *>(value);
}

QObject * RCommercialInformationModel::getTimeOfEstablishment() const
{
	return theTimeOfEstablishment.data();
}

void RCommercialInformationModel::setTimeOfEstablishment(QObject *value)
{
	theTimeOfEstablishment = qobject_cast<RParameter *>(value);
}

double RCommercialInformationModel::getNumberOfEmployees() const
{
	return theNumberOfEmployees;
}

void RCommercialInformationModel::setNumberOfEmployees(double value)
{
	theNumberOfEmployees = value;
}

RCommercialInformationModel::RSector RCommercialInformationModel::getSector() const
{
	return theSector;
}

void RCommercialInformationModel::setSector(RCommercialInformationModel::RSector value)
{
	theSector = value;
}

RCommercialInformationModel::RSize RCommercialInformationModel::getSize() const
{
	return theSize;
}

void RCommercialInformationModel::setSize(RCommercialInformationModel::RSize value)
{
	theSize = value;
}

int RCommercialInformationModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
{
	return 0;
}

bool RCommercialInformationModel::canBeRunParallelly() {
	return true;
}
