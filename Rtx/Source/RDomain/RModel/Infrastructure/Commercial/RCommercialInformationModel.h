#pragma once
#ifndef RCommercialInformationModel_H
#define RCommercialInformationModel_H

#include "RModel.h"

class RParameter;
class RResponse;

class RCommercialInformationModel : public RModel
{
	Q_OBJECT
		Q_PROPERTY(QObject *BuildingInformationModel READ getBuildingInformationModel WRITE setBuildingInformationModel)
		Q_PROPERTY(QObject *TimeOfEstablishment READ getTimeOfEstablishment WRITE setTimeOfEstablishment)
		
		Q_PROPERTY(double NumberOfEmployees READ getNumberOfEmployees WRITE setNumberOfEmployees)
		
		Q_PROPERTY(RSector Sector READ getSector WRITE setSector)
		Q_PROPERTY(RSize Size READ getSize WRITE setSize)

		Q_ENUMS(RSector)
		Q_ENUMS(RSize)



public:
	RCommercialInformationModel(QObject *parent, QString name);
	~RCommercialInformationModel();

	enum RSector { LocallyOriented, ExportOriented };
	enum RSize { Small, Large };

	QObject *getBuildingInformationModel() const;
	void setBuildingInformationModel(QObject *value);

	QObject *getTimeOfEstablishment() const;
	void setTimeOfEstablishment(QObject *value);

	double getNumberOfEmployees() const;
	void setNumberOfEmployees(double value);

	RSector getSector() const;
	void setSector(RSector value);

	RSize getSize() const;
	void setSize(RSize value);


	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID);

	// returns true as this model is parallelized
	bool canBeRunParallelly();

private:
	RPointer<RModel> theBuildingInformationModel;
	RPointer<RParameter> theTimeOfEstablishment;
	double theNumberOfEmployees;
	RSector theSector;
	RSize theSize;

};

#endif // RCommercialInformationModel_H
#pragma once
