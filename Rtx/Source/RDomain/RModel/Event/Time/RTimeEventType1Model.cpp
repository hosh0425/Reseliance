//Developed by Hossein Nasr
//In this scheduler: 
//At the specified instants of time the current value of parameter is modified.
//Note that this event model schedules event only for once at the begining of the analysis
#include "RTimeEventType1Model.h"

#include <QCoreApplication>

#include "RParameter.h"
#include "RResponse.h"
#include "RContinuousRandomVariable.h"
#include "RTime.h"
#include <iterator>
#include <qvariant.h>
#include <math.h>
#include <stdio.h>
#include <gsl/gsl_statistics.h>

RTimeEventType1Model::RTimeEventType1Model(QObject *parent, QString name)
	: REventModel(parent, name)
{
	int arraySizeForParallelizedObject = getArraySizeForParallelizedObject();
		
	sheduled = new bool [arraySizeForParallelizedObject];

	theModificationRule = RTimeEventType1Model::Add;
		
}

RTimeEventType1Model::~RTimeEventType1Model()
{
	delete[] sheduled;
}

QString RTimeEventType1Model::getEventTimeList()
{
	//Check the operation time parameters
	QList<RPointer<RParameter> > EventTimeList;
	for (int i = 0; i< theEventTimeList.count(); i++) {
		RParameter *parameter = qobject_cast<RParameter *>(theEventTimeList[i].data());
		if (parameter) {
			EventTimeList << parameter;
		}
		else {
			// Issue an error if the parameter is not a parameter
			rCritical() << "Error in" << objectName() << ": The parameter %1 is not a parameter. Please correct and try again";

		}
	}

	return parameterListToString(EventTimeList);
}

void RTimeEventType1Model::setEventTimeList(QString value)
{
	theEventTimeList = stringToParameterList(value);
}


QString RTimeEventType1Model::getParameterList()
{
	//Check the operation status
	QList<RParameter *> parameterList;
	for (int i = 0; i< theParameterList.count(); i++) {
		RParameter *parameter = qobject_cast<RParameter *>(theParameterList[i].data());
		if (parameter) {
			parameterList << parameter;
		}
		else {
			// Issue an error if the parameter is not a parameter
			rCritical() << "Error in" << objectName() << ": The parameter %1 is not a parameter. Please correct and try again";

		}
	}

	return parameterListToString(theParameterList);
}

void RTimeEventType1Model::setParameterList(QString value)
{
	theParameterList = stringToParameterList(value);
}

RTimeEventType1Model::RModificationRule RTimeEventType1Model::getModificationRule() const
{
	return theModificationRule;
}

void RTimeEventType1Model::setModificationRule(RTimeEventType1Model::RModificationRule value)
{
	theModificationRule = value;
}

QString RTimeEventType1Model::getModificationFactorList()
{
	//Check the operation status
	QList<RParameter *> modificationFactorList;
	for (int i = 0; i< theModificationFactorList.count(); i++) {
		RParameter *parameter = qobject_cast<RParameter *>(theModificationFactorList[i].data());
		if (parameter) {
			modificationFactorList << parameter;
		}
		else {
			// Issue an error if the parameter is not a parameter
			rCritical() << "Error in" << objectName() << ": The parameter %1 is not a parameter. Please correct and try again";

		}
	}

	return parameterListToString(theModificationFactorList);
}

void RTimeEventType1Model::setModificationFactorList(QString value)
{
	theModificationFactorList = stringToParameterList(value);
}

int RTimeEventType1Model::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
{
	return 0;
}

int RTimeEventType1Model::ScheduleEvents(double T, int threadID)
{
	//check the time parameter
	if (!theTimeParameter.data()) {
		rCritical(threadID) << "Error in" << objectName() << ": Enter a Time object and try again";
		return -1;
	}

	// Issue an error if nothing is entered in the EventTimeList
	if (theEventTimeList.isEmpty()) {
		rCritical(threadID) << "Error in" << objectName() << ": Enter at least one operation time parameter and try again";
		return -1;
	}
	for (int i = 0; i < theEventTimeList.count(); i++) {
		RParameter *param = qobject_cast<RParameter *>(theEventTimeList[i].data());
		if (!param) {
			// Issue an error if the parameter is not a parameter
			rCritical(threadID) << "Error in" << objectName() << ": The parameter %1 is not a parameter. Please correct and try again";
			return -1;
		}
	}

	// Issue an error if the nothing is entered in the parameter list
	if (theParameterList.isEmpty()) {
		rCritical(threadID) << "Error in" << objectName() << ": Enter at least one parameter and try again";
		return -1;
	}

	for (int i = 0; i < theParameterList.count(); i++) {
		RParameter *param = qobject_cast<RParameter *>(theParameterList[i].data());
		if (!param) {
			// Issue an error if the parameter is not a parameter
			rCritical(threadID) << "Error in" << objectName() << ": The parameter %1 is not a parameter. Please correct and try again";
			return -1;
		}
	}
	//if (theEventTimeList.count() == 1) {
	//	for (int i = 1; i < theParameterList.count(); i++) {
	//		theEventTimeList << theEventTimeList[0];
	//	}
	//}

	//if (theParameterList.count() != theEventTimeList.count()) {
	//	rCritical() << "Error in" << objectName() << ": Please enter as many Event Times as the number of parameters";
	//	return -1;
	//}

	if (theModificationFactorList.count() == 1) {
		for (int i = 1; i < theParameterList.count(); i++) {
			theModificationFactorList << theModificationFactorList[0].data();
		}
	}
	if (theParameterList.count() != theModificationFactorList.count()) {
		rCritical(threadID) << "Error in" << objectName() << ": Please enter either as many modification factor as the number of parameters or one modification factor";
		return -1;
	}

	//ALGORITHM
	double currentTime = theTimeParameter.data()->getCurrentValue(threadID);
	theNewEventMap[threadID].clear();

	//variables needed for each step of scheduling
	QVariantList parameterVariantList;
	double eventTime;
	double finishTime;
	QVariantList eventVariantList;

	//START SCHEDULING
	if (!sheduled[threadID]) {
		sheduled[threadID] = true;

		//for (int i = 0; i < theParameterList.count(); i++) {
		//	parameterVariantList.clear();
		//	eventVariantList.clear();

		//	QVariant parameterVariant = QVariant::fromValue<QObject *>(theParameterList[i].data());
		//	parameterVariantList << parameterVariant;

		//	eventTime = theEventTimeList[i].data()->getCurrentValue(threadID);
		//	finishTime = eventTime + currentTime;

		//	eventVariantList << (currentTime);
		//	
		//	QVariant temp;
		//	eventVariantList << temp;
		//	eventVariantList[1] = parameterVariantList;
		//	
		//	// It should be inserted as insertMulti since there might be two events with similar key=finish time
		//	theAllEventMap[threadID].insertMulti(finishTime, eventVariantList);
		//	theNewEventMap[threadID].insertMulti(finishTime, eventVariantList);
		//}

		parameterVariantList.clear();
		for (int i = 0; i < theParameterList.count(); i++) {
			QVariant parameterVariant = QVariant::fromValue<QObject *>(theParameterList[i].data());
			parameterVariantList << parameterVariant;
		}
		
		for (int i = 0; i < theEventTimeList.count(); i++) {
			eventVariantList.clear();
			eventTime = theEventTimeList[i].data()->getCurrentValue(threadID);
			finishTime = eventTime + currentTime;

			eventVariantList << (currentTime);

			QVariant temp;
			eventVariantList << temp;

			eventVariantList[1] = parameterVariantList;

			// It should be inserted as insertMulti since there might be two events with similar key=finish time
			theAllEventMap[threadID].insertMulti(finishTime, eventVariantList);
			theNewEventMap[threadID].insertMulti(finishTime, eventVariantList);
		}
	}

	return 0;
}

int RTimeEventType1Model::UpdateOperationStatus(double T, int threadID)
{
	QList<QVariantList> currentEventVariantListList = theAllEventMap[threadID].values(T);

	switch (RTimeEventType1Model::getModificationRule()) {

	case (RTimeEventType1Model::Add) :
		for (int i = 0; i < currentEventVariantListList.count(); i++) {
			QVariantList currentEventVariantList = currentEventVariantListList[i];

			double startedTime = currentEventVariantList[0].toDouble();
			QVariantList currentParameterVariantList = currentEventVariantList[1].toList();
			for (int j = 0; j < currentParameterVariantList.count(); j++) {
				QObject *currentObject = qvariant_cast<QObject *>(currentParameterVariantList[j]);
				RParameter *currentParameter = qobject_cast<RParameter *>(currentObject);

				//Change the parameter of the model after the Operation is finished
				double modificationFactor = theModificationFactorList[j].data()->getCurrentValue(threadID);
				double newValue = currentParameter->getCurrentValue(threadID) + modificationFactor;
				currentParameter->setCurrentValue(newValue, threadID);
			}
		}
	break;

	case (RTimeEventType1Model::Subtract):
		for (int i = 0; i < currentEventVariantListList.count(); i++) {
			QVariantList currentEventVariantList = currentEventVariantListList[i];

			double startedTime = currentEventVariantList[0].toDouble();
			QVariantList currentParameterVariantList = currentEventVariantList[1].toList();
			for (int j = 0; j < currentParameterVariantList.count(); j++) {
				QObject *currentObject = qvariant_cast<QObject *>(currentParameterVariantList[j]);
				RParameter *currentParameter = qobject_cast<RParameter *>(currentObject);

				//Change the parameter of the model after the Operation is finished
				double modificationFactor = theModificationFactorList[j].data()->getCurrentValue(threadID);
				double newValue = currentParameter->getCurrentValue(threadID) - modificationFactor;
				currentParameter->setCurrentValue(newValue, threadID);
			}
		}
		break;

	case (RTimeEventType1Model::Multiply):
		for (int i = 0; i < currentEventVariantListList.count(); i++) {
			QVariantList currentEventVariantList = currentEventVariantListList[i];

			double startedTime = currentEventVariantList[0].toDouble();
			QVariantList currentParameterVariantList = currentEventVariantList[1].toList();
			for (int j = 0; j < currentParameterVariantList.count(); j++) {
				QObject *currentObject = qvariant_cast<QObject *>(currentParameterVariantList[j]);
				RParameter *currentParameter = qobject_cast<RParameter *>(currentObject);

				//Change the parameter of the model after the Operation is finished
				double modificationFactor = theModificationFactorList[j].data()->getCurrentValue(threadID);
				double newValue = currentParameter->getCurrentValue(threadID) * modificationFactor;
				currentParameter->setCurrentValue(newValue, threadID);
			}
		}
		break;
	case (RTimeEventType1Model::Divide):
		for (int i = 0; i < currentEventVariantListList.count(); i++) {
			QVariantList currentEventVariantList = currentEventVariantListList[i];

			double startedTime = currentEventVariantList[0].toDouble();
			QVariantList currentParameterVariantList = currentEventVariantList[1].toList();
			for (int j = 0; j < currentParameterVariantList.count(); j++) {
				QObject *currentObject = qvariant_cast<QObject *>(currentParameterVariantList[j]);
				RParameter *currentParameter = qobject_cast<RParameter *>(currentObject);

				//Change the parameter of the model after the Operation is finished
				double modificationFactor = theModificationFactorList[j].data()->getCurrentValue(threadID);
				double newValue = currentParameter->getCurrentValue(threadID) / modificationFactor;
				currentParameter->setCurrentValue(newValue, threadID);
			}
		}
		break;

	case (RTimeEventType1Model::SetEqualtTo):
		for (int i = 0; i < currentEventVariantListList.count(); i++) {
			QVariantList currentEventVariantList = currentEventVariantListList[i];

			double startedTime = currentEventVariantList[0].toDouble();
			QVariantList currentParameterVariantList = currentEventVariantList[1].toList();
			for (int j = 0; j < currentParameterVariantList.count(); j++) {
				QObject *currentObject = qvariant_cast<QObject *>(currentParameterVariantList[j]);
				RParameter *currentParameter = qobject_cast<RParameter *>(currentObject);

				//Change the parameter of the model after the Operation is finished
				double modificationFactor = theModificationFactorList[j].data()->getCurrentValue(threadID);
				double newValue = modificationFactor;
				currentParameter->setCurrentValue(newValue, threadID);
			}
		}
		break;
	}

	return 0;
}

//Reset the history of the lists at the begining of each sample
void RTimeEventType1Model::resetHistoryVariables(int threadID)
{
	theAllEventMap[threadID].clear();
	sheduled[threadID] = false;
	
}

bool RTimeEventType1Model::canBeRunParallelly() {
	return true;
}