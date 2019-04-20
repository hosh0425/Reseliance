//Developed by Hossein Nasr
//In this scheduler: 
//1-the demand is a single parameter. We have multiple capacity 
//2-Nothing happens when the operation starts; (no parameter is updated when operation starts)
//3- only the parameters that are updated at the end of an operation are considered- since the demand is not changed when the operation is finished, it would not be updated
//4- The demand is changed when the start of scheduling each event, but it is not entered in the updatedParaemterList because we only consider the changes at the finish time of each event

#include "ROperationEventType2Model.h"

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

ROperationEventType2Model::ROperationEventType2Model(QObject *parent, QString name)
	: REventModel(parent, name)
{
	int arraySizeForParallelizedObject = getArraySizeForParallelizedObject();

	sortedImportanceList = new QList<double>[arraySizeForParallelizedObject];
	importanceValueList = new QList<double>[arraySizeForParallelizedObject];

	prerequsiteDoneBoolList = new QList <bool>[arraySizeForParallelizedObject];
	prerequisiteOperationsFinishStatusListList = new QList<QList<RParameter *> >[arraySizeForParallelizedObject];

	startSample = new bool[arraySizeForParallelizedObject];
	operationDemandValue = new int[arraySizeForParallelizedObject];
	operatingDemandValue = new int[arraySizeForParallelizedObject];
	operatedDemandValue = new int[arraySizeForParallelizedObject];

	thePriorityRule = ROperationEventType2Model::InOrderOfInput;
	theNumberOfPrerequisiteOperations = 0;

}

ROperationEventType2Model::~ROperationEventType2Model()
{
	delete[] sortedImportanceList;
	delete[] importanceValueList;

	delete[] prerequsiteDoneBoolList;
	delete[] prerequisiteOperationsFinishStatusListList;

	delete[] startSample;
	delete[] operationDemandValue;
	delete[] operatingDemandValue;
	delete[] operatedDemandValue;
}

QObject * ROperationEventType2Model::getNewOperationDemand() const
{
	return theNewOperationDemand.data();
}

void ROperationEventType2Model::setNewOperationDemand(QObject *value)
{
	theNewOperationDemand = qobject_cast<RParameter *>(value);
}

QString ROperationEventType2Model::getOperationCapacityList()
{
	//Check the operation time parameters
	QList<RPointer<RParameter> > OperationCapacityList;
	for (int i = 0; i< theOperationCapacityList.count(); i++) {
		RParameter *parameter = qobject_cast<RParameter *>(theOperationCapacityList[i].data());
		if (parameter) {
			OperationCapacityList << parameter;
		}
		else {
			// Issue an error if the parameter is not a parameter
			rCritical() << "Error in" << objectName() << ": The parameter %1 is not a parameter. Please correct and try again";

		}
	}

	return parameterListToString(OperationCapacityList);
}

void ROperationEventType2Model::setOperationCapacityList(QString value)
{
	theOperationCapacityList = stringToParameterList(value);
}

QString ROperationEventType2Model::getOperationTimeList()
{
	//Check the operation time parameters
	QList<RPointer<RParameter> > operationTimeList;
	for (int i = 0; i< theOperationTimeList.count(); i++) {
		RParameter *parameter = qobject_cast<RParameter *>(theOperationTimeList[i].data());
		if (parameter) {
			operationTimeList << parameter;
		}
		else {
			// Issue an error if the parameter is not a parameter
			rCritical() << "Error in" << objectName() << ": The parameter %1 is not a parameter. Please correct and try again";

		}
	}

	return parameterListToString(operationTimeList);
}

void ROperationEventType2Model::setOperationTimeList(QString value)
{
	theOperationTimeList = stringToParameterList(value);
}


QString ROperationEventType2Model::getOperationFinishStatusList()
{
	//Check the operation status
	QList<RParameter *> operationFinishStatusList;
	for (int i = 0; i< theOperationFinishStatusList.count(); i++) {
		RParameter *parameter = qobject_cast<RParameter *>(theOperationFinishStatusList[i].data());
		if (parameter) {
			operationFinishStatusList << parameter;
		}
		else {
			// Issue an error if the parameter is not a parameter
			rCritical() << "Error in" << objectName() << ": The parameter %1 is not a parameter. Please correct and try again";

		}
	}

	return parameterListToString(theOperationFinishStatusList);
}

void ROperationEventType2Model::setOperationFinishStatusList(QString value)
{
	theOperationFinishStatusList = stringToParameterList(value);
}

ROperationEventType2Model::RPriorityRule ROperationEventType2Model::getPriorityRule() const
{
	return thePriorityRule;
}

void ROperationEventType2Model::setPriorityRule(ROperationEventType2Model::RPriorityRule value)
{
	thePriorityRule = value;
}

QString ROperationEventType2Model::getImportanceMeasureList()
{
	//Check the priority
	QList<RParameter *> importanceList;
	for (int i = 0; i< theImportanceMeasureList.count(); i++) {
		RParameter *parameter = qobject_cast<RParameter *>(theImportanceMeasureList[i].data());
		if (parameter) {
			importanceList << parameter;
		}
		else {
			// Issue an error if the parameter is not a parameter
			rCritical() << "Error in" << objectName() << ": The parameter %1 is not a parameter. Please correct and try again";
            return QString("-1");
		}
	}

	

	return parameterListToString(theImportanceMeasureList);
}

void ROperationEventType2Model::setImportanceMeasureList(QString value)
{
	theImportanceMeasureList = stringToParameterList(value);
}

int ROperationEventType2Model::getNumberOfPrerequisiteOperations() const
{
	return theNumberOfPrerequisiteOperations;
}

void ROperationEventType2Model::setNumberOfPrerequisiteOperations(int value)
{
	theNumberOfPrerequisiteOperations = value;
}

QString ROperationEventType2Model::getPrerequisiteOperationsFinishStatusList()
{
	return parameterListToString(thePrerequisiteOperationsFinishStatusList);
}

void ROperationEventType2Model::setPrerequisiteOperationsFinishStatusList(QString value)
{
	thePrerequisiteOperationsFinishStatusList = stringToParameterList(value);
}

int ROperationEventType2Model::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
{
	return 0;
}

int ROperationEventType2Model::ScheduleEvents(double T, int threadID)
{
	//check the time parameter
	if (!theTimeParameter.data()) {
		rCritical(threadID) << "Error in" << objectName() << ": Enter a Time object and try again";
		return -1;
	}

	//Issue an error if the operation capacity is not a decision variable
	if (!theNewOperationDemand.data()) {
		rCritical(threadID) << "Error in" << objectName() << ": Enter the Operation Demand";
		return -1;
	}

	//if (initialOperationCapacity == 0) {
	//	rCritical(threadID) << "There should be at least one free operation capacity" << this->objectName();
	//	return -1;
	//}

	// Issue an error if nothing is entered in the operationTimeList
	if (theOperationTimeList.isEmpty()) {
		rCritical(threadID) << "Error in" << objectName() << ": Enter at least one operation capacity parameter and try again";
		return -1;
	}

	// Issue an error if nothing is entered in the operationTimeList
	if (theOperationTimeList.isEmpty()) {
		rCritical(threadID) << "Error in" << objectName() << ": Enter at least one operation time parameter and try again";
		return -1;
	}


	// Issue an error if the nothing is entered in the operationFinishStatusList
	if (theOperationFinishStatusList.isEmpty()) {
		rCritical(threadID) << "Error in" << objectName() << ": Enter at least one operation finish status parameter and try again";
		return -1;
	}

	//check if the number of operation time is not the same as operation finish status
	if (theOperationFinishStatusList.count() != theOperationTimeList.count()) {
		rCritical(threadID) << "The number of operation time parameters is not equal to the number of operation status parameters";
		return -1;
	}
	
	//check if the number of operation time is not the same as operation capacities
	if (theOperationCapacityList.count() != theOperationTimeList.count()) {
		rCritical(threadID) << "The number of operation time parameters is not equal to the number of operation capacity parameters";
		return -1;
	}


	// Issue an error if the nothing is entered in the ImportanceMeasureList
	if ((theImportanceMeasureList.isEmpty()) && (this->getPriorityRule() == ROperationEventType2Model::InOrderOfImportance)) {
		rCritical(threadID) << "Error in" << objectName() << ": Enter the importance measure parameters and try again";
		return -1;
	}

	//check the number of importance list
	if (this->getPriorityRule() == ROperationEventType2Model::InOrderOfImportance) {
		if (theImportanceMeasureList.count() != theOperationCapacityList.count()) {
			rCritical(threadID) << "The number of importance measure parameters is not equal to the number of operation capacity parameters";
			return -1;
		}
	}

	//check the number of prerequisite operation parameters based on the number of prerequisite operatios
	if (thePrerequisiteOperationsFinishStatusList.count() != theOperationCapacityList.count()*theNumberOfPrerequisiteOperations) {
		rCritical(threadID) << "The number of prerequisite operation parameters is not equal to the number of operation capacity parameters times the number of prerequisite operations";
		return -1;
	}


	//ALGORITHM
	double currentTime = theTimeParameter.data()->getCurrentValue(threadID);
	theNewEventMap[threadID].clear();


	////////////////////////////////// it should be created once
	// Required Lists are created at the first time the event model is called
	if (startSample[threadID]) {
		startSample[threadID] = false;

		//Create Prerequisite ListList
		prerequisiteOperationsFinishStatusListList[threadID].clear();
		double numberOfPrerequisiteOperations = theNumberOfPrerequisiteOperations;

		////In first version: prerequisite 1 and 2 is for operation 1 and 3 and 4 for operation 2
		//if (numberOfPrerequisiteOperations != 0) {
		//	for (int i = 0; i<theOperationFinishStatusList.count(); i++) {
		//		QList<RParameter *> thisOperationPrerequisiteList;
		//		for (int j = numberOfPrerequisiteOperations*(i); j<numberOfPrerequisiteOperations*(i + 1); j++) {
		//			thisOperationPrerequisiteList.append(thePrerequisiteOperationsFinishStatusList[j].data());
		//		}
		//		prerequisiteOperationsFinishStatusListList[threadID].append(thisOperationPrerequisiteList);
		//		thisOperationPrerequisiteList.clear();
		//	}
		//}

		//In second and current version: prerequisite 1 and 3 is for operation 1 and 2 and 4 for operation 2
		if (numberOfPrerequisiteOperations != 0) {
			double operationCount = theOperationFinishStatusList.count();
			for (int i = 0; i < operationCount; i++) {
				QList<RParameter *> thisOperationPrerequisiteList;
				for (int j = 0; j < numberOfPrerequisiteOperations; j++) {
					thisOperationPrerequisiteList.append(thePrerequisiteOperationsFinishStatusList[i + j * operationCount].data());
				}
				prerequisiteOperationsFinishStatusListList[threadID].append(thisOperationPrerequisiteList);
				thisOperationPrerequisiteList.clear();
			}
		}




		//create the importance value list which contains the value of importance parameters
		importanceValueList[threadID].clear();
		if (thePriorityRule == ROperationEventType2Model::InOrderOfImportance) {
			for (int i = 0; i < theOperationTimeList.count(); i++) {
				importanceValueList[threadID] << theImportanceMeasureList[i].data()->getCurrentValue();
			}
		}
		else if (this->getPriorityRule() == ROperationEventType2Model::InOrderOfInput) {
			for (int i = 0; i < theOperationTimeList.count(); i++) {
				importanceValueList[threadID] << theOperationTimeList.count() - i;
			}
		}

		//create the list which has the importance measure of operations in ascending order
		sortedImportanceList[threadID].clear();
		for (int i = 0; i < importanceValueList[threadID].count(); i++) {
			sortedImportanceList[threadID] << importanceValueList[threadID][i];
		}
		if (this->getPriorityRule() == ROperationEventType2Model::InOrderOfImportance) {
			qSort(sortedImportanceList[threadID].begin(), sortedImportanceList[threadID].end(), qGreater<double>());
		}

		for (int i = 0; i < theOperationTimeList.count(); i++) {
			theOperationTimeDoubleList << theOperationTimeList[i].data()->getCurrentValue(threadID);
		}


		for (int i = 0; i < theOperationFinishStatusList.count(); i++) {
			theOperationFinishStatusList[i].data()->setCurrentValue(0, threadID);
		}
	}
	/////////////////////////////////
	int newDemand = theNewOperationDemand.data()->getCurrentValue(threadID);
	operationDemandValue[threadID] = operationDemandValue[threadID] + newDemand;
	theNewOperationDemand.data()->setCurrentValue(0.0,threadID);

	//variables needed for each step of scheduling
	RParameter *operationTimeParameter;
	RParameter *operationFinishStatusParameter;
	QVariantList operationFinishStatusParameterVariantList;

	double operationTime;
	double operationFinishStatus;
	double finishTime;
	QVariantList eventVariantList;

	//create the list which is true if all the prerequsite operations of an operation has been already completed
	prerequsiteDoneBoolList[threadID].clear();
	for (int i = 0; i < theOperationCapacityList.count(); i++) {
		prerequsiteDoneBoolList[threadID] << true;
		for (int j = 0; j<theNumberOfPrerequisiteOperations; j++) {
			if (prerequisiteOperationsFinishStatusListList[threadID][i][j]->getCurrentValue(threadID) != 1.0) {
				prerequsiteDoneBoolList[threadID][i] = false;
				break;
			}
		}
	}

	// to Check that if there is any capacity left or not
	bool haveCapacity = false;
	for (int i = 0; i < theOperationCapacityList.count(); i++) {
		if (theOperationCapacityList[i].data()->getCurrentValue(threadID) > 0) {
			haveCapacity = true;
			break;
		}
	}

	double maxImportance;
	int maxImportanceIndex;
	int count = 0;//sortedImportanceList.count();

	int maxCount = sortedImportanceList[threadID].count();

	//START SCHEDULING
	while ((operationDemandValue[threadID] > 0) && (haveCapacity)) {
		if (count == maxCount) break;
		operationFinishStatusParameterVariantList.clear();
		maxImportance = sortedImportanceList[threadID][count];
		maxImportanceIndex = importanceValueList[threadID].indexOf(maxImportance);
		
		if ((prerequsiteDoneBoolList[threadID][maxImportanceIndex]) && (theOperationCapacityList[maxImportanceIndex].data()->getCurrentValue(threadID) > 0)) {
			//Calculate the number of events that can be scheduled
			int capacity = theOperationCapacityList[maxImportanceIndex].data()->getCurrentValue(threadID);
			int demand = operationDemandValue[threadID];

			int NumPossibleEvents = qMin(demand, capacity);

			for (int j = 0; j < NumPossibleEvents; j++) {
				int tempCap = theOperationCapacityList[maxImportanceIndex].data()->getCurrentValue(threadID);
				int tempDemand = operationDemandValue[threadID];
				
				//Compute the new demand and capacity
				theOperationCapacityList[maxImportanceIndex].data()->setCurrentValue(tempCap - 1, threadID);
				operationDemandValue[threadID] = tempDemand - 1;
				operatingDemandValue[threadID] += 1;

				//create the event map
				eventVariantList.clear();
				operationFinishStatusParameterVariantList.clear();

				operationFinishStatusParameter = theOperationFinishStatusList[maxImportanceIndex].data();

				operationTime = theOperationTimeDoubleList[maxImportanceIndex];
				finishTime = operationTime + currentTime;

				eventVariantList << (currentTime);


				QVariant operationFinishStatusParameterVariant = QVariant::fromValue<QObject *>(operationFinishStatusParameter);
				eventVariantList << operationFinishStatusParameterVariant;

				//Since the operation capacity is updated at the beginning and finish of the operation, it should be in the event variant list
				QVariant operationCapacityVariant = QVariant::fromValue<QObject *>(theOperationCapacityList[maxImportanceIndex].data());
				operationFinishStatusParameterVariantList << operationFinishStatusParameterVariant << operationCapacityVariant;
				
				eventVariantList[1] = operationFinishStatusParameterVariantList;

				// It should be inserted as insertMulti since there might be two events with similar key=finish time
				theAllEventMap[threadID].insertMulti(finishTime, eventVariantList);
				theNewEventMap[threadID].insertMulti(finishTime, eventVariantList);

			}
			
		}

		haveCapacity = false;
		// to check if there is any capacity left after an event is scheduled
		for (int i = 0; i < theOperationCapacityList.count(); i++) {
			if (theOperationCapacityList[i].data()->getCurrentValue(threadID) > 0) {
				haveCapacity = true;
				break;
			}
		}

		count++;
	}

	return 0;
}

int ROperationEventType2Model::UpdateOperationStatus(double T, int threadID)
{
	QList<QVariantList> currentEventVariantListList = theAllEventMap[threadID].values(T);
	for (int i = 0; i < currentEventVariantListList.count(); i++) {
		//compute the new operating and operated demand value
		operatingDemandValue[threadID] -= 1;
		operatedDemandValue[threadID] += 1;
		
		QVariantList currentEventVariantList = currentEventVariantListList[i];

		double startedTime = currentEventVariantList[0].toDouble();
		QVariantList currentOperationParameterVariantList = currentEventVariantList[1].toList();
		for (int j = 0; j < currentOperationParameterVariantList.count(); j++) {
			QObject *currentOperationObject = qvariant_cast<QObject *>(currentOperationParameterVariantList[j]);
			RParameter *currentOperationParameter = qobject_cast<RParameter *>(currentOperationObject);

			//Change the parameter of the model after the Operation is finished
			//Both the operation capacity and the finish status are added by one value
			int tempVal = currentOperationParameter->getCurrentValue(threadID);
			currentOperationParameter->setCurrentValue(tempVal + 1, threadID);
		}
	}

	return 0;
}

//Reset the history of the lists at the begining of each sample
void ROperationEventType2Model::resetHistoryVariables(int threadID)
{
	theAllEventMap[threadID].clear();

	operationDemandValue[threadID] = 0;
	operatingDemandValue[threadID] = 0;
	operatedDemandValue[threadID] = 0;
	
	startSample[threadID] = true;

}

void ROperationEventType2Model::reset()
{

}
bool ROperationEventType2Model::canBeRunParallelly() {
	return true;
}
