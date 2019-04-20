#pragma once
#ifndef RHealthcareDemandModel_H
#define RHealthcareDemandModel_H

#include "RModel.h"
#include "REventModel.h"

class RResponse;
class RParameter;



class RHealthcareDemandModel : public RModel
{
	Q_OBJECT
		Q_PROPERTY(QString NumberOfRescuedInjuriesList READ getNumberOfRescuedInjuriesList WRITE setNumberOfRescuedInjuriesList)

public:

	RHealthcareDemandModel(QObject *parent, QString name);
	~RHealthcareDemandModel();


	QString getNumberOfRescuedInjuriesList();
	void setNumberOfRescuedInjuriesList(QString value);

	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID);
	void resetHistoryVariables(int threadID);
	// Returns true as the model is parallelized
	bool canBeRunParallelly();

private:
	QList<RPointer<RParameter> > theNumberOfRescuedInjuriesList;

	RResponse *theHealthcareNewDemand;
	RResponse *theHealthcareAllDemand;
};

#endif // REventModel_H
#pragma once
