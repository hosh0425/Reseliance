//Developed by Hossein Nasr
//In this scheduler: 
//1-the capacity is a single parameter.  
//2- the status is 0 or 1; 
//3- At each time only one component is restored; 
//4-Nothing happens when the operation starts; (no parameter is updated when operation starts)
//5-only one parameter is updated when the operation is finished
//6- only finish to start prerequisite is considered


#include "ROperationEventType1Model.h"

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

ROperationEventType1Model::ROperationEventType1Model(QObject *parent, QString name)
	: REventModel(parent, name)
{
	int arraySizeForParallelizedObject = getArraySizeForParallelizedObject();

	//theOperationCapacity = new int[arraySizeForParallelizedObject];
	theBackupOperationCapacity = new int[arraySizeForParallelizedObject];

	//theOperationTimeDoubleList = new QList<double>[arraySizeForParallelizedObject];
	
	sortedImportanceList = new QList<double>[arraySizeForParallelizedObject];
	backUpSortedImportanceList = new QList<double>[arraySizeForParallelizedObject];
	importanceValueList = new QList<double>[arraySizeForParallelizedObject];

	prerequsiteDoneBoolList = new QList <bool>[arraySizeForParallelizedObject];
	prerequisiteOperationsFinishStatusListList = new QList<QList<RParameter *> >[arraySizeForParallelizedObject];

	theOperationFreeOperationFinishStatusList = new QList<RParameter *>[arraySizeForParallelizedObject];

	theUnoperatedOperationTimeList = new QList<RParameter *>[arraySizeForParallelizedObject];
	theUnoperatedOperationTimeDoubleList = new QList<double>[arraySizeForParallelizedObject];
	theUnoperatedOperationFinishStatusList = new QList<RParameter *>[arraySizeForParallelizedObject];
	theUnoperatedPrerequisiteOperationsFinishStatusListList = new QList<QList<RParameter *> >[arraySizeForParallelizedObject];
	theUnoperatedImportanceList = new QList<double>[arraySizeForParallelizedObject];

	theOperatingOperationFinishStatusList = new QList<RParameter *>[arraySizeForParallelizedObject];

	theOperatedOperationFinishStatusList = new QList<RParameter *>[arraySizeForParallelizedObject];


	//theOperationCapacity/*[0]*/ = 0;

	thePriorityRule = ROperationEventType1Model::InOrderOfInput;
	theNumberOfPrerequisiteOperations = 0;




}

ROperationEventType1Model::~ROperationEventType1Model()
{
	//delete[] theOperationCapacity;
	delete[] theBackupOperationCapacity;
	//delete[] theOperationTimeDoubleList;

	delete[] sortedImportanceList;
	delete[] backUpSortedImportanceList;
	delete[] importanceValueList;

	delete[] prerequsiteDoneBoolList;
	delete[] prerequisiteOperationsFinishStatusListList;

	delete[] theOperationFreeOperationFinishStatusList;

	delete[] theUnoperatedOperationTimeList;
	delete[] theUnoperatedOperationTimeDoubleList;
	delete[] theUnoperatedOperationFinishStatusList;
	delete[] theUnoperatedPrerequisiteOperationsFinishStatusListList;
	delete[] theUnoperatedImportanceList;

	delete[] theOperatingOperationFinishStatusList;

	delete[] theOperatedOperationFinishStatusList;
}

//int ROperationEventType1Model::getOperationCapacity(/*int threadID*/) const
//{
//	return theOperationCapacity/*[threadID]*/;
//}
//
////void ROperationEventType1Model::setOperationCapacity(int value, int threadID)
////{
////	theOperationCapacity[threadID] = value;
////}
//
//void ROperationEventType1Model::setOperationCapacity(int value)
//{
//	theOperationCapacity/*[0]*/ = value;
//}

QObject * ROperationEventType1Model::getOperationCapacity() const
{
	return theOperationCapacity.data();
}

void ROperationEventType1Model::setOperationCapacity(QObject *value)
{
	theOperationCapacity = qobject_cast<RParameter *>(value);
}

QString ROperationEventType1Model::getOperationTimeList()
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

void ROperationEventType1Model::setOperationTimeList(QString value)
{
	theOperationTimeList = stringToParameterList(value);
}


QString ROperationEventType1Model::getOperationFinishStatusList()
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

void ROperationEventType1Model::setOperationFinishStatusList(QString value)
{
	theOperationFinishStatusList = stringToParameterList(value);
}

ROperationEventType1Model::RPriorityRule ROperationEventType1Model::getPriorityRule() const
{
	return thePriorityRule;
}

void ROperationEventType1Model::setPriorityRule(ROperationEventType1Model::RPriorityRule value)
{
	thePriorityRule = value;
}

QString ROperationEventType1Model::getImportanceMeasureList()
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

void ROperationEventType1Model::setImportanceMeasureList(QString value)
{
	theImportanceMeasureList = stringToParameterList(value);
}

int ROperationEventType1Model::getNumberOfPrerequisiteOperations() const
{
	return theNumberOfPrerequisiteOperations;
}

void ROperationEventType1Model::setNumberOfPrerequisiteOperations(int value)
{
	theNumberOfPrerequisiteOperations = value;
}

QString ROperationEventType1Model::getPrerequisiteOperationsFinishStatusList()
{
	return parameterListToString(thePrerequisiteOperationsFinishStatusList);
}

void ROperationEventType1Model::setPrerequisiteOperationsFinishStatusList(QString value)
{
	thePrerequisiteOperationsFinishStatusList = stringToParameterList(value);

	//QList<RPointer<RParameter> > prerequisiteList = stringToParameterList(value);
	//for (int i = 0; i< prerequisiteList.count(); i++) {
	//	RParameter *parameter = qobject_cast<RParameter *>(prerequisiteList[i].data());
	//	if (parameter) {
	//		prerequisiteList << parameter;
	//	}
	//	else {
	//		// Issue an error if the parameter is not a parameter
	//		rCritical(0) << "Error in" << objectName() << ": The parameter %1 is not a parameter. Please correct and try again";
	//		return;
	//	}
	//}
}

int ROperationEventType1Model::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
{
	return 0;
}

int ROperationEventType1Model::ScheduleEvents(double T, int threadID)
{
	//check the time parameter
	if (!theTimeParameter.data()) {
		rCritical(threadID) << "Error in" << objectName() << ": Enter a Time object and try again";
		return -1;
	}

	//Issue an error if the operation capacity is not a decision variable
	if (!theOperationCapacity.data()) {
		rCritical(threadID) << "Error in" << objectName() << ": Enter the Operation Capacity";
		return -1;
	}

	//// issue an error if the operation capacity is a random variable
	//RRandomVariable *operationCapacityRandomVariable = qobject_cast<RRandomVariable *>(theOperationCapacity.data());
	//if (operationCapacityRandomVariable) {
	//	rCritical(threadID) << "Error in" << objectName() << ": The Operation Capacity should not be a random variable";
	//	return -1;
	//
	//}

	//if (initialOperationCapacity == 0) {
	//	rCritical(threadID) << "Please enter a non zero operation capacity for" << this->objectName();
	//	return -1;
	//}

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



	// Issue an error if the nothing is entered in the ImportanceMeasureList
	if ((theImportanceMeasureList.isEmpty()) && (this->getPriorityRule() == ROperationEventType1Model::InOrderOfImportance)) {
		rCritical(threadID) << "Error in" << objectName() << ": Enter the importance measure parameters and try again";
		return -1;
	}

	//check the number of importance list
	if (this->getPriorityRule() == ROperationEventType1Model::InOrderOfImportance) {
		if (theImportanceMeasureList.count() != theOperationFinishStatusList.count()) {
			rCritical(threadID) << "The number of importance measure parameters is not equal to the number of operation finish status parameters";
			return -1;
		}
	}


	////Check the prerequisite operations
	//QList<RParameter *> prerequisiteList;
	//for (int i = 0; i< thePrerequisiteOperationsFinishStatusList.count(); i++) {
	//	RParameter *parameter = qobject_cast<RParameter *>(thePrerequisiteOperationsFinishStatusList[i].data());
	//	if (parameter) {
	//		prerequisiteList << parameter;
	//	}
	//	else {
	//		// Issue an error if the parameter is not a parameter
	//		rCritical(threadID) << "Error in" << objectName() << ": The object %1 is not a parameter. Please correct and try again";
	//		return -1;
	//	}
	//}

	//check the number of prerequisite operation parameters based on the number of prerequisite operatios
	if (thePrerequisiteOperationsFinishStatusList.count() != theOperationFinishStatusList.count()*theNumberOfPrerequisiteOperations) {
		rCritical(threadID) << "The number of prerequisite operation parameters is not equal to the number of operation status parameters times the number of prerequisite operations";
		return -1;
	}


	//ALGORITHM
	double currentTime = theTimeParameter.data()->getCurrentValue(threadID);
	theNewEventMap[threadID].clear();



	// for the first time that event model is run the parameterlists are empty and so the event model creates the lists
	if ((theOperationFreeOperationFinishStatusList[threadID].isEmpty()) && (theUnoperatedOperationFinishStatusList[threadID].isEmpty()) && (theOperatingOperationFinishStatusList[threadID].isEmpty()) && (theOperatedOperationFinishStatusList[threadID].isEmpty())) {

		//check the repair capacity
		//Create the listList which has the status of the prerequisite operations
		//the zeroth element is the status of prerequsite 1 and so forth

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


		////Create a list of operation time values
		//for (int i = 0; i < theOperationTimeList.count(); i++) {
		//	theOperationTimeDoubleList[threadID] << theOperationTimeList[i].data()->getCurrentValue(threadID);
		//}

		//create the importance value list which contains the value of importance parameters
		importanceValueList[threadID].clear();
		if (thePriorityRule == ROperationEventType1Model::InOrderOfImportance) {
			for (int i = 0; i<theOperationTimeList.count(); i++) {
				importanceValueList[threadID] << theImportanceMeasureList[i].data()->getCurrentValue();
			}
		}
		else if (this->getPriorityRule() == ROperationEventType1Model::InOrderOfInput) {
			for (int i = 0; i<theOperationTimeList.count(); i++) {
				importanceValueList[threadID] << theOperationTimeList.count() - i;
			}
		}

		//create lists of operationfree, unoperated, operating, operated
		for (int i = 0; i < theOperationFinishStatusList.count(); i++) {
			double operationTime = theOperationTimeList[i].data()->getCurrentValue(threadID);
			double operationStatus = theOperationFinishStatusList[i].data()->getCurrentValue(threadID);
			//The lists are created based on the OPERATION TIME NOT THE OPERATION STATUS
			if (operationTime == 0.0) {
				theOperationFreeOperationFinishStatusList[threadID].append(theOperationFinishStatusList[i].data());
			}
			else {
				theUnoperatedOperationTimeList[threadID].append(theOperationTimeList[i].data());
				theUnoperatedOperationTimeDoubleList[threadID].append(operationTime);

				theUnoperatedOperationFinishStatusList[threadID].append(theOperationFinishStatusList[i].data());
				if (!prerequisiteOperationsFinishStatusListList[threadID].isEmpty()) {
					theUnoperatedPrerequisiteOperationsFinishStatusListList[threadID].append(prerequisiteOperationsFinishStatusListList[threadID][i]);
				}
				theUnoperatedImportanceList[threadID].append(importanceValueList[threadID][i]);
			}
		}

		//create the list which has the importance measure of operations in ascending order
		sortedImportanceList[threadID].clear();
		for (int i = 0; i<theUnoperatedImportanceList[threadID].count(); i++) {
			sortedImportanceList[threadID] << theUnoperatedImportanceList[threadID][i];
		}
		if (this->getPriorityRule() == ROperationEventType1Model::InOrderOfImportance) {
			qSort(sortedImportanceList[threadID].begin(), sortedImportanceList[threadID].end(), qGreater<double>());
		}


		//Create the event that are associated to operation free list, all of them are sheduled for the time Zero

		//operationTimeParameter = theUnoperatedOperationTimeList[maxImportanceIndex];
		for (int i = 0; i < theOperationFreeOperationFinishStatusList[threadID].count(); i++) {
			RParameter *operationFinishStatusParameter = theOperationFreeOperationFinishStatusList[threadID][i];
			double operationTime = 0.0;
			double finishTime = currentTime;

			QVariantList eventVariantList;
			eventVariantList << (currentTime);
			QVariant operationFinishStatusParameterVariant = QVariant::fromValue<QObject *>(operationFinishStatusParameter);
			eventVariantList << operationFinishStatusParameterVariant;

			//create the event map
			QVariantList operationFinishStatusParameterVariantList;
			operationFinishStatusParameterVariantList << operationFinishStatusParameterVariant;


			eventVariantList[1] = operationFinishStatusParameterVariantList;

			// It should be inserted as insertMulti since there might be two events with similar key=finish time
			theAllEventMap[threadID].insertMulti(finishTime, eventVariantList);
			theNewEventMap[threadID].insertMulti(finishTime, eventVariantList);
		}
	}

	//variables needed for each step of scheduling
	RParameter *operationTimeParameter;
	RParameter *operationFinishStatusParameter;
	QVariantList operationFinishStatusParameterVariantList;

	double operationTime;
	double operationFinishStatus;
	double finishTime;
	QVariantList eventVariantList;

	////create the list which has the importance measure of operations in ascending order
	//QList<double> sortedImportanceList;
	//for (int i=0; i<theUnoperatedImportanceList.count();i++) {
	//	sortedImportanceList[i] = theUnoperatedImportanceList[i];
	//}
	//if (this->getPriorityRule() == ROperationEventType1Model::InOrderOfImportance) {
	//	qSort(sortedImportanceList.begin(),sortedImportanceList.end(), qGreater<double>());
	//}

	//create the list which is true if all the prerequsite operations of an operation has been already completed
	prerequsiteDoneBoolList[threadID].clear();
	for (int i = 0; i<theUnoperatedImportanceList[threadID].count(); i++) {
		prerequsiteDoneBoolList[threadID] << true;
		for (int j = 0; j<theNumberOfPrerequisiteOperations; j++) {
			if (theUnoperatedPrerequisiteOperationsFinishStatusListList[threadID][i][j]->getCurrentValue(threadID) != 1.0) {
				prerequsiteDoneBoolList[threadID][i] = false;
				break;
			}
		}
	}

	double maxImportance;
	int maxImportanceIndex;
	int count = 0;//sortedImportanceList.count();

	backUpSortedImportanceList[threadID] = sortedImportanceList[threadID];
	int maxCount = sortedImportanceList[threadID].count();

	//START SCHEDULING
	while ((theOperationCapacity.data()->getCurrentValue(threadID) > 0) && (!backUpSortedImportanceList[threadID].isEmpty())) {
		if (count == maxCount) break;
		operationFinishStatusParameterVariantList.clear();
		maxImportance = sortedImportanceList[threadID][count];
		maxImportanceIndex = theUnoperatedImportanceList[threadID].indexOf(maxImportance);
		eventVariantList.clear();

		if (prerequsiteDoneBoolList[threadID][maxImportanceIndex]) {
			//set the new Operation capacity
			int tempCap = theOperationCapacity.data()->getCurrentValue(threadID);
			theOperationCapacity.data()->setCurrentValue(tempCap - 1, threadID);
			//theBackupOperationCapacity[threadID]--;

			//operationTimeParameter = theUnoperatedOperationTimeList[maxImportanceIndex];
			operationFinishStatusParameter = theUnoperatedOperationFinishStatusList[threadID][maxImportanceIndex];

			//create the event map
			//operationTime = operationTimeParameter->getCurrentValue(threadID);
			operationTime = theUnoperatedOperationTimeDoubleList[threadID][maxImportanceIndex];
			finishTime = operationTime + currentTime;

			eventVariantList << (currentTime);

			QVariant operationFinishStatusParameterVariant = QVariant::fromValue<QObject *>(operationFinishStatusParameter);
			eventVariantList << operationFinishStatusParameterVariant;

			//Since the operation capacity is updated at the beginning and finish of the operation, it should be in the event variant list
			QVariant operationCapacityVariant = QVariant::fromValue<QObject *>(theOperationCapacity.data());


			operationFinishStatusParameterVariantList << operationFinishStatusParameterVariant << operationCapacityVariant;


			eventVariantList[1] = operationFinishStatusParameterVariantList;

			// It should be inserted as insertMulti since there might be two events with similar key=finish time
			theAllEventMap[threadID].insertMulti(finishTime, eventVariantList);
			theNewEventMap[threadID].insertMulti(finishTime, eventVariantList);


			//delete the parameter from unoperated and move it to operating
			theOperatingOperationFinishStatusList[threadID].append(operationFinishStatusParameter);

			theUnoperatedOperationTimeList[threadID].removeAt(maxImportanceIndex);
			theUnoperatedOperationTimeDoubleList[threadID].removeAt(maxImportanceIndex);
			theUnoperatedOperationFinishStatusList[threadID].removeAt(maxImportanceIndex);
			if (!theUnoperatedPrerequisiteOperationsFinishStatusListList[threadID].isEmpty()) {
				theUnoperatedPrerequisiteOperationsFinishStatusListList[threadID].removeAt(maxImportanceIndex);
			}
			theUnoperatedImportanceList[threadID].removeAt(maxImportanceIndex);
			backUpSortedImportanceList[threadID].removeOne(maxImportance);
			prerequsiteDoneBoolList[threadID].removeAt(maxImportanceIndex);
		}
		//sortedImportanceList.removeLast();
		count++;
	}

	sortedImportanceList[threadID] = backUpSortedImportanceList[threadID];

	return 0;
}

int ROperationEventType1Model::UpdateOperationStatus(double T, int threadID)
{
	QList<QVariantList> currentEventVariantListList = theAllEventMap[threadID].values(T);
	for (int i = 0; i < currentEventVariantListList.count(); i++) {

		////set the new Operation capacity according to the number scheduled events
		//int tempCap = theOperationCapacity.data()->getCurrentValue(threadID);
		//theOperationCapacity.data()->setCurrentValue(tempCap + 1, threadID);

		QVariantList currentEventVariantList = currentEventVariantListList[i];

		double startedTime = currentEventVariantList[0].toDouble();
		QVariantList currentOperationParameterVariantList = currentEventVariantList[1].toList();

		//UPDATE THE OPERATION STATUS PARAMETER
		QObject *currentOperationFinishStatusObject = qvariant_cast<QObject *>(currentOperationParameterVariantList[0]);
		RParameter *currentOperationFinishStatusParameter = qobject_cast<RParameter *>(currentOperationFinishStatusObject);
			
		//Change the parameter of the model after the Operation is finished
		//****CHECK WHETHER IT IS RUN FROM THE RRESPONSE OR RPARAMETER**** ---- CHECKED : IT IS RUN FROM RResponse if the parameter is a response object
		currentOperationFinishStatusParameter->setCurrentValue(1.0, threadID);

		//delete the parameter from operatinging status list and move it to operated 
		theOperatingOperationFinishStatusList[threadID].removeOne(currentOperationFinishStatusParameter);
		theOperatedOperationFinishStatusList[threadID].append(currentOperationFinishStatusParameter);

		
		//UPDATE THE OPERATION CAPACITY (if existed)
		if (currentOperationParameterVariantList.count() == 2) {
			QObject *currentOperationCapacityObject = qvariant_cast<QObject *>(currentOperationParameterVariantList[1]);
			RParameter *currentOperationCapacityParameter = qobject_cast<RParameter *>(currentOperationCapacityObject);

			//Change the operation capacity after the operation has finished
			int tempCap = currentOperationCapacityParameter->getCurrentValue(threadID);
			currentOperationCapacityParameter->setCurrentValue(tempCap + 1, threadID);
		}

	}

	return 0;
}

//Reset the history of the lists at the begining of each sample
void ROperationEventType1Model::resetHistoryVariables(int threadID)
{
	theOperationCapacity.data()->setCurrentValue(initialOperationCapacity, threadID);

	theAllEventMap[threadID].clear();
	theOperationFreeOperationFinishStatusList[threadID].clear();

	theUnoperatedOperationTimeList[threadID].clear();
	theUnoperatedOperationFinishStatusList[threadID].clear();
	theUnoperatedPrerequisiteOperationsFinishStatusListList[threadID].clear();
	theUnoperatedImportanceList[threadID].clear();

	theOperatingOperationFinishStatusList[threadID].clear();

	theOperatedOperationFinishStatusList[threadID].clear();

	//theOperationTimeDoubleList[threadID].clear();
	theUnoperatedOperationTimeDoubleList[threadID].clear();

}

void ROperationEventType1Model::reset()
{
	if (theOperationCapacity) {
		initialOperationCapacity = theOperationCapacity.data()->getCurrentValue();
	}
	else {
		rCritical() << "Please enter the Operation Capacity Parameter";
	}
}
bool ROperationEventType1Model::canBeRunParallelly() {
	return true;
}
