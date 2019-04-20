#pragma once
#ifndef RWaterStationInformationModel_H
#define RWaterStationInformationModel_H

#include "RModel.h"

class RParameter;
class RResponse;

class RWaterStationInformationModel : public RModel
{
	Q_OBJECT

		Q_PROPERTY(QObject *Location READ getLocation WRITE setLocation)
		Q_PROPERTY(QObject *TimeOfConstruction READ getTimeOfConstruction WRITE setTimeOfConstruction)
		Q_PROPERTY(RType Type READ getType WRITE setType)
		Q_PROPERTY(RComponents Components READ getComponents WRITE setComponents)

		Q_ENUMS(RType)
		Q_ENUMS(RComponents)



public:
	RWaterStationInformationModel(QObject *parent, QString name);
	~RWaterStationInformationModel();

	enum RType { Small, Medium, Large };
	enum RComponents { Anchored, Unanchored };

	QObject *getLocation() const;
	void setLocation(QObject *value);

	QObject *getTimeOfConstruction() const;
	void setTimeOfConstruction(QObject *value);


	RType getType() const;
	void setType(RType value);

	RComponents getComponents() const;
	void setComponents(RComponents value);


	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID);

	// returns true as this model is parallelized
	bool canBeRunParallelly();

private:

	RPointer<RParameter> theLocation;
	RPointer<RParameter> theTimeOfConstruction;
	RType theType;
	RComponents theComponents;

};

#endif // RWaterStationInformationModel_H
#pragma once
