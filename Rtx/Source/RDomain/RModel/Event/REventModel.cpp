#include "REventModel.h"

#include <QMap>
#include <QVariant>
#include "RParameter.h"
#include "RRandomVariable.h"
#include "RTime.h"
#include <iterator>
#include <list>

REventModel::REventModel(QObject *parent, QString name)
	: RModel(parent, name)
{

	int arraySizeForParallelizedObject = getArraySizeForParallelizedObject();

	theAllEventMap = new QMap <double, QVariantList>[arraySizeForParallelizedObject];
	theNewEventMap = new QMap <double, QVariantList>[arraySizeForParallelizedObject];
	isAnalyzedFlag = new bool[arraySizeForParallelizedObject];
}

REventModel::~REventModel()
{
	delete[] theAllEventMap;
	delete[] theNewEventMap;
	delete[] isAnalyzedFlag;
}
QObject * REventModel::getTimeParameter() const
{
	return theTimeParameter.data();
}

void REventModel::setTimeParameter(QObject *value)
{
	if (qobject_cast<RTime *>(value)) {
		theTimeParameter = qobject_cast<RTime *>(value);
	}
	else {
		rCritical() << "Error in" << this->objectName() << " : the input Time Parameter should be a parameter";
	}
}

bool REventModel::isAnalyzed(int threadID)
{
	return isAnalyzedFlag[threadID];
}
//
//void REventModel::setAnalyzed(bool analyzed)
//{
//	isAnalyzedFlag = analyzed;
//}

//QMap <double,double> REventModel::getNewEventTimeMap()
//{
//	QMap <double,double> newEventTimeMap;
//	QVariantList newEventVariantList;
//
//	QMap <double,QVariantList>::iterator i;
//	for (i=theNewEventMap.begin(); i!=theNewEventMap.end(); ++i) {
//		double finishTime = i.key();
//		newEventVariantList = i.value();
//		double startTime = newEventVariantList[0].toDouble();
//		newEventTimeMap.insert(finishTime,startTime);
//	}
//
//	return newEventTimeMap;
//}
QList<RPointer<RParameter> > REventModel::getUpdatedParamterList(double T, int threadID)
{
	QList<RPointer<RParameter> > updatedParameterList;
	QVariantList newEventVariantList;
	QVariantList updatedParameterVariantList;

	updatedParameterList.clear();
	newEventVariantList.clear();
	updatedParameterVariantList.clear();

	QList<QVariantList> currentEventVariantListList = theAllEventMap[threadID].values(T);
	for (int i = 0; i < currentEventVariantListList.count(); i++) {
		QVariantList currentEventVariantList = currentEventVariantListList[i];

		double startedTime = currentEventVariantList[0].toDouble();
		QVariantList currentOperationFinishStatusParameterVariantList = currentEventVariantList[1].toList();
		for (int j = 0; j < currentOperationFinishStatusParameterVariantList.count(); j++) {
			QObject *currentOperationFinishStatusObject = qvariant_cast<QObject *>(currentOperationFinishStatusParameterVariantList[j]);
			RParameter *currentOperationFinishStatusParameter = qobject_cast<RParameter *>(currentOperationFinishStatusObject);
			updatedParameterList << currentOperationFinishStatusParameter;
		}
	}




	/*QMap <double,QVariantList>::iterator i;
	for (i=theNewEventMap.begin(); i!=theNewEventMap.end(); ++i) {
	newEventVariantList = i.value();
	if (!newEventVariantList[1].isNull()) {
	updatedParameterVariantList = newEventVariantList[1].toList();
	for (int j=0; j<updatedParameterVariantList.count(); j++) {
	QObject *StatusQObject = qvariant_cast<QObject *>(updatedParameterVariantList[j]);
	RParameter *StatusParameter = qobject_cast<RParameter *>(StatusQObject);
	updatedParameterList << StatusParameter;
	}
	}
	}*/

	return updatedParameterList;
}

QList <QList <double> > REventModel::getNewEventTimeListList(int threadID)
{
	QVariantList newEventVariantList;
	QList <QList <double> > newEventTimeListList;
	QList <double> newEventtimeList;

	QMap <double, QVariantList>::iterator i;
	for (i = theNewEventMap[threadID].begin(); i != theNewEventMap[threadID].end(); ++i) {
		newEventtimeList.clear();
		double finishTime = i.key();
		newEventVariantList = i.value();
		double startTime = newEventVariantList[0].toDouble();
		newEventtimeList << finishTime << startTime;
		if (!newEventTimeListList.contains(newEventtimeList)) {
			newEventTimeListList.append(newEventtimeList);
		}
	}

	//After that the event model gave the timelist to the analzer, its isAnalzed flag should become true
	isAnalyzedFlag[threadID] = true;

	return newEventTimeListList;
}

void REventModel::setResponseAnalyzed(bool analyzed, int threadID)
{
	isAnalyzedFlag[threadID] = analyzed;
}