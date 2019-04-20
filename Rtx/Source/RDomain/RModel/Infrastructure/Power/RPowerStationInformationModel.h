#ifndef RPowerStationInformationModel_H
#define RPowerStationInformationModel_H

#include "RModel.h"

class RParameter;
class RResponse;

class RPowerStationInformationModel : public RModel
{
	Q_OBJECT

		Q_PROPERTY(QObject *Location READ getLocation WRITE setLocation)
		Q_PROPERTY(QObject *TimeOfConstruction READ getTimeOfConstruction WRITE setTimeOfConstruction)
		Q_PROPERTY(double ReplacementCost READ getReplacementCost WRITE setReplacementCost)
		Q_PROPERTY(RType Type READ getType WRITE setType)
		Q_PROPERTY(RComponents Components READ getComponents WRITE setComponents)

		Q_ENUMS(RType)
		Q_ENUMS(RComponents)



public:
	RPowerStationInformationModel(QObject *parent, QString name);
	~RPowerStationInformationModel();

	enum RType { LowVoltage, MediumVoltage, HighVoltage };
	enum RComponents { Anchored, Unanchored };

	QObject *getLocation() const;
	void setLocation(QObject *value);

	QObject *getTimeOfConstruction() const;
	void setTimeOfConstruction(QObject *value);

	double getReplacementCost() const;
	void setReplacementCost(double value);

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
	double theReplacementCost;
	RType theType;
	RComponents theComponents;

};

#endif // RPowerStationInformationModel_H
#pragma once
