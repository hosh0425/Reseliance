#pragma once
#ifndef RTimeEventType1Model_H
#define RTimeEventType1Model_H

#include "RModel.h"
#include "REventModel.h"

class RResponse;
class RParameter;



class RTimeEventType1Model : public REventModel
{
	Q_OBJECT
		Q_PROPERTY(QString EventTimeList READ getEventTimeList WRITE setEventTimeList)
		Q_PROPERTY(QString ParameterList READ getParameterList WRITE setParameterList)
		Q_PROPERTY(RModificationRule ModificationRule READ getModificationRule WRITE setModificationRule)
		Q_PROPERTY(QString ModificationFactorList READ getModificationFactorList WRITE setModificationFactorList)
				
		Q_ENUMS(RModificationRule)
	
public:

	RTimeEventType1Model(QObject *parent, QString name);
	~RTimeEventType1Model();

	QString getEventTimeList();
	void setEventTimeList(QString value);

	QString getParameterList();
	void setParameterList(QString value);

	enum RModificationRule { Add, Subtract, Multiply, Divide, SetEqualtTo};
	RModificationRule getModificationRule() const;
	void setModificationRule(RModificationRule value);

	QString getModificationFactorList();
	void setModificationFactorList(QString value);


	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID);
	int ScheduleEvents(double T, int threadID);
	int UpdateOperationStatus(double T, int threadID);

	//A method which is called at the start of each sample of sampling analyses
	void resetHistoryVariables(int threadID);

	// Returns true as the model is parallelized
	bool canBeRunParallelly();

private:

	QList<RPointer<RParameter> > theEventTimeList;
	QList<RPointer<RParameter> > theParameterList;
	RModificationRule theModificationRule;
	QList<RPointer<RParameter> > theModificationFactorList;
		
	// determines if this event model has done its scheduling duty or not yet
	bool *sheduled;
};

#endif // RTimeEventType1Model_H
#pragma once
