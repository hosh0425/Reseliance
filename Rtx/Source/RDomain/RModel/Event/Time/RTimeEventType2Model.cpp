//Developed by Hossein Nasr
//In this scheduler: 
//At the specified instants of time a percent of pamameter 1 and a percent of paramter 2 is added to the parameter 3.
//Note that the sum of three parameters remains constant
//Note that this event model schedules event only for once at the begining of the analysis

#include "RTimeEventType2Model.h"

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

RTimeEventType2Model::RTimeEventType2Model(QObject *parent, QString name)
	: REventModel(parent, name)
{
	int arraySizeForParallelizedObject = getArraySizeForParallelizedObject();

	sheduled = new bool[arraySizeForParallelizedObject];
	//finishTimeList = new QList<double>[arraySizeForParallelizedObject];

}

RTimeEventType2Model::~RTimeEventType2Model()
{
	delete[] sheduled;
	//delete[] finishTimeList;
}

QString RTimeEventType2Model::getEventTimeList()
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

void RTimeEventType2Model::setEventTimeList(QString value)
{
	theEventTimeList = stringToParameterList(value);
}


QString RTimeEventType2Model::getParameterList1()
{
	//Check the operation status
	QList<RParameter *> ParameterList1;
	for (int i = 0; i< theParameterList1.count(); i++) {
		RParameter *parameter = qobject_cast<RParameter *>(theParameterList1[i].data());
		if (parameter) {
			ParameterList1 << parameter;
		}
		else {
			// Issue an error if the parameter is not a parameter
			rCritical() << "Error in" << objectName() << ": The parameter %1 is not a parameter. Please correct and try again";

		}
	}

	return parameterListToString(theParameterList1);
}

void RTimeEventType2Model::setParameterList1(QString value)
{
	theParameterList1 = stringToParameterList(value);
}

QString RTimeEventType2Model::getParameterList2()
{
	//Check the operation status
	QList<RParameter *> ParameterList2;
	for (int i = 0; i< theParameterList2.count(); i++) {
		RParameter *parameter = qobject_cast<RParameter *>(theParameterList2[i].data());
		if (parameter) {
			ParameterList2 << parameter;
		}
		else {
			// Issue an error if the parameter is not a parameter
			rCritical() << "Error in" << objectName() << ": The parameter %1 is not a parameter. Please correct and try again";

		}
	}

	return parameterListToString(theParameterList2);
}

void RTimeEventType2Model::setParameterList2(QString value)
{
	theParameterList2 = stringToParameterList(value);
}

QString RTimeEventType2Model::getParameterList3()
{
	//Check the operation status
	QList<RParameter *> ParameterList3;
	for (int i = 0; i< theParameterList3.count(); i++) {
		RParameter *parameter = qobject_cast<RParameter *>(theParameterList3[i].data());
		if (parameter) {
			ParameterList3 << parameter;
		}
		else {
			// Issue an error if the parameter is not a parameter
			rCritical() << "Error in" << objectName() << ": The parameter %1 is not a parameter. Please correct and try again";

		}
	}

	return parameterListToString(theParameterList3);
}

void RTimeEventType2Model::setParameterList3(QString value)
{
	theParameterList3 = stringToParameterList(value);
}


QString RTimeEventType2Model::getPercentagesOfParameter1AddedToParameter3List()
{
	//Check the operation status
	QList<RParameter *> percent1List;
	for (int i = 0; i< thePercentagesOfParameter1AddedToParameter3List.count(); i++) {
		RParameter *parameter = qobject_cast<RParameter *>(thePercentagesOfParameter1AddedToParameter3List[i].data());
		if (parameter) {
			percent1List << parameter;
		}
		else {
			// Issue an error if the parameter is not a parameter
			rCritical() << "Error in" << objectName() << ": The parameter %1 is not a parameter. Please correct and try again";

		}
	}

	return parameterListToString(thePercentagesOfParameter1AddedToParameter3List);
}

void RTimeEventType2Model::setPercentagesOfParameter1AddedToParameter3List(QString value)
{
	thePercentagesOfParameter1AddedToParameter3List = stringToParameterList(value);
}

QString RTimeEventType2Model::getPercentagesOfParameter2AddedToParameter3List()
{
	//Check the operation status
	QList<RParameter *> percent2List;
	for (int i = 0; i< thePercentagesOfParameter2AddedToParameter3List.count(); i++) {
		RParameter *parameter = qobject_cast<RParameter *>(thePercentagesOfParameter2AddedToParameter3List[i].data());
		if (parameter) {
			percent2List << parameter;
		}
		else {
			// Issue an error if the parameter is not a parameter
			rCritical() << "Error in" << objectName() << ": The parameter %1 is not a parameter. Please correct and try again";

		}
	}

	return parameterListToString(thePercentagesOfParameter2AddedToParameter3List);
}

void RTimeEventType2Model::setPercentagesOfParameter2AddedToParameter3List(QString value)
{
	thePercentagesOfParameter2AddedToParameter3List = stringToParameterList(value);
}

int RTimeEventType2Model::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
{
	return 0;
}

int RTimeEventType2Model::ScheduleEvents(double T, int threadID)
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
			rCritical() << "Error in" << objectName() << ": The parameter %1 is not a parameter. Please correct and try again";
			return -1;
		}
	}

	// Issue an error if the nothing is entered in the parameter list
	if (theParameterList1.isEmpty()) {
		rCritical(threadID) << "Error in" << objectName() << ": Enter at least one parameter and try again";
		return -1;
	}

	for (int i = 0; i < theParameterList1.count(); i++) {
		RParameter *param = qobject_cast<RParameter *>(theParameterList1[i].data());
		if (!param) {
			// Issue an error if the parameter is not a parameter
			rCritical() << "Error in" << objectName() << ": The parameter %1 is not a parameter. Please correct and try again";
			return -1;
		}
	}


	//Check the number of inputs
	//if (theParameterList1.count() != theEventTimeList.count()) {
	//	rCritical() << "Error in" << objectName() << ": Please enter as many parameter 1 as the number of event times";
	//	return -1;
	//}

	if (theParameterList1.count() != theParameterList2.count()) {
		rCritical() << "Error in" << objectName() << ": Please enter as many parameter 2 as the number parameter 1";
		return -1;
	}
	if (theParameterList1.count() != theParameterList3.count()) {
		rCritical() << "Error in" << objectName() << ": Please enter as many parameter 3 as the number parameter 1";
		return -1;
	}

	if (thePercentagesOfParameter1AddedToParameter3List.count() == 1) {
		for (int i = 1; i < theParameterList1.count(); i++) {
			thePercentagesOfParameter1AddedToParameter3List << thePercentagesOfParameter1AddedToParameter3List[0].data();
		}
	}
	if (thePercentagesOfParameter1AddedToParameter3List.count() != theParameterList1.count()) {
		rCritical() << "Error in" << objectName() << ": Please enter as many percentages of parameter 1 as the number of parameters";
		return -1;
	}

	if (thePercentagesOfParameter2AddedToParameter3List.count() == 1) {
		for (int i = 1; i < theParameterList1.count(); i++) {
			thePercentagesOfParameter2AddedToParameter3List << thePercentagesOfParameter2AddedToParameter3List[0].data();
		}
	}
	if (thePercentagesOfParameter2AddedToParameter3List.count() != theParameterList1.count()) {
		rCritical() << "Error in" << objectName() << ": Please enter as many percentages of parameter 2 as the number of parameters";
		return -1;
	}

	//for (int i = 0; i < thePercentagesOfParameter1AddedToParameter3List.count(); i++) {
	//	double val = thePercentagesOfParameter1AddedToParameter3List[i].data()->getCurrentValue(threadID);
	//	if ((val < 0) || (val > 100)) {
	//		rCritical() << "Error in" << objectName() << ": the values of Percentages of parameter 1 must be between 0 and 100";
	//		return -1;
	//		break;
	//	}
	//}

	//for (int i = 0; i < thePercentagesOfParameter2AddedToParameter3List.count(); i++) {
	//	double val = thePercentagesOfParameter2AddedToParameter3List[i].data()->getCurrentValue(threadID);
	//	if ((val < 0) || (val > 100)) {
	//		rCritical() << "Error in" << objectName() << ": the values of Percentages of parameter 2 values must be between 0 and 100";
	//		return -1;
	//		break;
	//	}
	//}

	//ALGORITHM
	double currentTime = theTimeParameter.data()->getCurrentValue(threadID);
	theNewEventMap[threadID].clear();

	//variables needed for each step of scheduling
	QVariantList parameterVariantList;
	double eventTime;
	double finishTime;
	QVariantList eventVariantList;

	//START SCHEDULING
	//This Event Model is scheduled once at the beginining of the analysis. The "sheduled" data member determines if the event model has already scheduled or not.
	if (!sheduled[threadID]) {
		sheduled[threadID] = true;

		parameterVariantList.clear();
		//Since at this event model, at each scheduled time all parameters are changed, the parameterVariantList contains the list of all parameters, i.e., theParameterList1 and theParameterList2 and theParameterList3
		for (int i = 0; i < theParameterList1.count(); i++) {
			QVariant parameter1Variant = QVariant::fromValue<QObject *>(theParameterList1[i].data());
			parameterVariantList << parameter1Variant;
			QVariant parameter2Variant = QVariant::fromValue<QObject *>(theParameterList2[i].data());
			parameterVariantList << parameter2Variant;
			QVariant parameter3Variant = QVariant::fromValue<QObject *>(theParameterList3[i].data());
			parameterVariantList << parameter3Variant;
		}

		//finishTimeList[threadID].clear();
		for (int i = 0; i < theEventTimeList.count(); i++) {
			eventVariantList.clear();				

			eventTime = theEventTimeList[i].data()->getCurrentValue(threadID);
			finishTime = eventTime + currentTime;

			//finishTimeList[threadID] << finishTime;

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

int RTimeEventType2Model::UpdateOperationStatus(double T, int threadID)
{
	//At the specified instants of time a percent of pamameter 1 and a percent of paramter 2 is added to the parameter 3.
	//Note that the sum of three parameters remains constant


	QList<QVariantList> currentEventVariantListList = theAllEventMap[threadID].values(T);
	//int currentIndex = finishTimeList[threadID].indexOf(T);

	for (int i = 0; i < currentEventVariantListList.count(); i++) {
		QVariantList currentEventVariantList = currentEventVariantListList[i];

		double startedTime = currentEventVariantList[0].toDouble();
		QVariantList currentParameterVariantList = currentEventVariantList[1].toList();
		for (int j = 0; j < currentParameterVariantList.count()/3; j++) {

			QObject *currentObject1 = qvariant_cast<QObject *>(currentParameterVariantList[3 * j + 0]);
			RParameter *currentParameter1 = qobject_cast<RParameter *>(currentObject1);
			QObject *currentObject2 = qvariant_cast<QObject *>(currentParameterVariantList[3 * j + 1]);
			RParameter *currentParameter2 = qobject_cast<RParameter *>(currentObject2);
			QObject *currentObject3 = qvariant_cast<QObject *>(currentParameterVariantList[3 * j + 2]);
			RParameter *currentParameter3 = qobject_cast<RParameter *>(currentObject3);

			//Change the parameter of the model after the Operation is finished
			double percent1 = thePercentagesOfParameter1AddedToParameter3List[j].data()->getCurrentValue(threadID);
			double percent2 = thePercentagesOfParameter2AddedToParameter3List[j].data()->getCurrentValue(threadID);

			int changeValue1 = currentParameter1->getCurrentValue(threadID) * percent1 / 100;
			int changeValue2 = currentParameter2->getCurrentValue(threadID) * percent2 / 100;

			int previousValue1 = currentParameter1->getCurrentValue(threadID);
			int previousValue2 = currentParameter2->getCurrentValue(threadID);
			int previousValue3 = currentParameter3->getCurrentValue(threadID);
			
			currentParameter1->setCurrentValue(previousValue1 - changeValue1, threadID);
			currentParameter2->setCurrentValue(previousValue2 - changeValue2, threadID);
			currentParameter3->setCurrentValue(previousValue3 + changeValue1 + changeValue2, threadID);
		}
	}


	return 0;
}

//Reset the history of the lists at the begining of each sample
void RTimeEventType2Model::resetHistoryVariables(int threadID)
{
	theAllEventMap[threadID].clear();
	sheduled[threadID] = false;

}

bool RTimeEventType2Model::canBeRunParallelly() {
	return true;
}