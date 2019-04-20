#ifndef REventModel_H
#define REventModel_H

#include "RModel.h"

class RTime;
class RParameter;
//#include <gsl/gsl_rng.h>
//#include <gsl/gsl_randist.h>

class REventModel : public RModel
{
	Q_OBJECT

		Q_PROPERTY(QObject *TimeParameter READ getTimeParameter WRITE setTimeParameter)


public:
	REventModel(QObject *parent, QString name);
	~REventModel();

	QObject *getTimeParameter() const;
	void setTimeParameter(QObject *value);

	//returns the list which has the finish and start times of scheduled events
	QList <QList <double> > getNewEventTimeListList(int threadID);

	// returns the list containing the parameters that have been updated at an event
	QList<RPointer<RParameter> > getUpdatedParamterList(double T, int threadID);

	// Specific implementation of setResponseAnalyzed for REventModel that sets the isAnalyzed flag to true
	void setResponseAnalyzed(bool analyzed, int threadID);

	//// Sets the isAnalyzed flag
	bool isAnalyzed(int threadID);
	//void setAnalyzed(bool analyzed);

	//Pure virtual methods that have specific implmentations for each type of event models
	//The method which Schedules the events and creates the Future Event List (AllEventMap)
	virtual int ScheduleEvents(double T, int threadID) = 0;

	//The method which does the predefined duty of the event model at the time when an event occurs
	virtual int UpdateOperationStatus(double T, int threadID) = 0;

protected:

	RPointer<RTime> theTimeParameter;
	bool* isAnalyzedFlag;
	
	//theAllEventMap Contain the list of all events that the event model has scheduled
	QMap <double, QVariantList>* theAllEventMap;
	//theNewEventMap Contain the list of new events that the event model has scheduled at the time is called and run
	QMap <double, QVariantList>* theNewEventMap;

	//For both Maps the elements are as follows:
	//The key is finish time of the events and the vlaue is a QVariantList that
		//The first element of the QVariantList is "StartTime"
		//The second element of the QVariantList is the list of pointers to parameters which should be updated when the event finishes


private:


};
#endif // REventModel_H
#pragma once
