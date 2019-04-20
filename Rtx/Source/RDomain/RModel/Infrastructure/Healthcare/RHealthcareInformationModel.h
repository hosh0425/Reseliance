#pragma once
#ifndef RHealthcareInformationModel_H
#define RHealthcareInformationModel_H

#include "RModel.h"

class RParameter;
class RResponse;

class RHealthcareInformationModel : public RModel
{
	Q_OBJECT
		Q_PROPERTY(QObject *BuildingInformationModel READ getBuildingInformationModel WRITE setBuildingInformationModel)

		Q_PROPERTY(double NumberOfBeds READ getNumberOfBeds WRITE setNumberOfBeds)

		Q_PROPERTY(RSector Sector READ getSector WRITE setSector)
		Q_PROPERTY(RSize Size READ getSize WRITE setSize)
		Q_PROPERTY(RType Type READ getType WRITE setType)

		Q_ENUMS(RSector)
		Q_ENUMS(RSize)
		Q_ENUMS(RType)

public:
	RHealthcareInformationModel(QObject *parent, QString name);
	~RHealthcareInformationModel();

	enum RSector { Public, Private };
	enum RSize { Small, Medium, Large };
	enum RType {Hospital, MedicalOfficeOrClinic};


	QObject *getBuildingInformationModel() const;
	void setBuildingInformationModel(QObject *value);

	double getNumberOfBeds() const;
	void setNumberOfBeds(double value);

	RSector getSector() const;
	void setSector(RSector value);

	RSize getSize() const;
	void setSize(RSize value);

	RType getType() const;
	void setType(RType value);

	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID);

	// returns true as this model is parallelized
	bool canBeRunParallelly();

private:
	RPointer<RModel> theBuildingInformationModel;
	double theNumberOfBeds;
	RSector theSector;
	RSize theSize;
	RType theType;
};

#endif // RHealthcareInformationModel_H
#pragma once
