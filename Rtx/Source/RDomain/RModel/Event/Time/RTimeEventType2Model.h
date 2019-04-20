#pragma once
#ifndef RTimeEventType2Model_H
#define RTimeEventType2Model_H

#include "RModel.h"
#include "REventModel.h"

class RResponse;
class RParameter;



class RTimeEventType2Model : public REventModel
{
	Q_OBJECT
		Q_PROPERTY(QString EventTimeList READ getEventTimeList WRITE setEventTimeList)
		Q_PROPERTY(QString ParameterList1 READ getParameterList1 WRITE setParameterList1)
		Q_PROPERTY(QString ParameterList2 READ getParameterList2 WRITE setParameterList2)
		Q_PROPERTY(QString ParameterList3 READ getParameterList3 WRITE setParameterList3)
		
		Q_PROPERTY(QString PercentagesOfParameter1AddedToParameter3List READ getPercentagesOfParameter1AddedToParameter3List WRITE setPercentagesOfParameter1AddedToParameter3List)
		Q_PROPERTY(QString PercentagesOfParameter2AddedToParameter3List READ getPercentagesOfParameter2AddedToParameter3List WRITE setPercentagesOfParameter2AddedToParameter3List)


public:

	RTimeEventType2Model(QObject *parent, QString name);
	~RTimeEventType2Model();

	QString getEventTimeList();
	void setEventTimeList(QString value);

	QString getParameterList1();
	void setParameterList1(QString value);

	QString getParameterList2();
	void setParameterList2(QString value);

	QString getParameterList3();
	void setParameterList3(QString value);

	QString getPercentagesOfParameter1AddedToParameter3List();
	void setPercentagesOfParameter1AddedToParameter3List(QString value);

	QString getPercentagesOfParameter2AddedToParameter3List();
	void setPercentagesOfParameter2AddedToParameter3List(QString value);


	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID);
	int ScheduleEvents(double T, int threadID);
	int UpdateOperationStatus(double T, int threadID);

	//A method which is called at the start of each sample of sampling analyses
	void resetHistoryVariables(int threadID);

	// Returns true as the model is parallelized
	bool canBeRunParallelly();

private:

	QList<RPointer<RParameter> > theEventTimeList;
	QList<RPointer<RParameter> > theParameterList1;
	QList<RPointer<RParameter> > theParameterList2;
	QList<RPointer<RParameter> > theParameterList3;

	QList<RPointer<RParameter> > thePercentagesOfParameter1AddedToParameter3List;

	QList<RPointer<RParameter> > thePercentagesOfParameter2AddedToParameter3List;

	// determines if this event model has done its scheduling duty or not yet
	bool *sheduled;
};

#endif // RTimeEventType2Model_H
