#ifndef ROperationEventType1Model_H
#define ROperationEventType1Model_H

#include "RModel.h"
#include "REventModel.h"

class RResponse;
class RParameter;
class RRandomVariable;


class ROperationEventType1Model : public REventModel
{
	Q_OBJECT
		//Q_PROPERTY(double OperationCapacity READ getOperationCapacity WRITE setOperationCapacity)
		Q_PROPERTY(QObject *OperationCapacity READ getOperationCapacity WRITE setOperationCapacity)

		Q_PROPERTY(QString OperationTimeList READ getOperationTimeList WRITE setOperationTimeList)
		Q_PROPERTY(QString OperationFinishStatusList READ getOperationFinishStatusList WRITE setOperationFinishStatusList)
		Q_PROPERTY(RPriorityRule PriorityRule READ getPriorityRule WRITE setPriorityRule)
		Q_ENUMS(RPriorityRule)
		Q_PROPERTY(QString ImportanceMeasureList READ getImportanceMeasureList WRITE setImportanceMeasureList)

		Q_PROPERTY(double NumberOfPrerequisiteOperations READ getNumberOfPrerequisiteOperations WRITE setNumberOfPrerequisiteOperations)
		Q_PROPERTY(QString PrerequisiteOperationsFinishStatusList READ getPrerequisiteOperationsFinishStatusList WRITE setPrerequisiteOperationsFinishStatusList)

public:

	ROperationEventType1Model(QObject *parent, QString name);
	~ROperationEventType1Model();

	QObject *getOperationCapacity() const;
	void setOperationCapacity(QObject *value);

	//int getOperationCapacity(/*int threadID = 0*/) const;
	//void setOperationCapacity(int value, int threadID);
	//void setOperationCapacity(int value);

	QString getOperationTimeList();
	void setOperationTimeList(QString value);

	QString getOperationFinishStatusList();
	void setOperationFinishStatusList(QString value);

	enum RPriorityRule { InOrderOfInput, InOrderOfImportance };
	RPriorityRule getPriorityRule() const;
	void setPriorityRule(RPriorityRule value);

	QString getImportanceMeasureList();
	void setImportanceMeasureList(QString value);

	int getNumberOfPrerequisiteOperations() const;
	void setNumberOfPrerequisiteOperations(int value);

	QString getPrerequisiteOperationsFinishStatusList();
	void setPrerequisiteOperationsFinishStatusList(QString value);


	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID);
	int ScheduleEvents(double T, int threadID);
	int UpdateOperationStatus(double T, int threadID);

	//A method which is called at the start of each sample of sampling analyses
	void resetHistoryVariables(int threadID);

	//A method which is called at the start of sampling analyses
	void reset();
	// Returns true as the model is parallelized
	bool canBeRunParallelly();
private:
	RPointer<RParameter> theOperationCapacity;
	int initialOperationCapacity;

	int* theBackupOperationCapacity;
	QList<RPointer<RParameter> > theOperationTimeList;
	QList<RPointer<RParameter> > theOperationFinishStatusList;
	RPriorityRule thePriorityRule;
	QList<RPointer<RParameter> > theImportanceMeasureList;

	//QList<double> *theOperationTimeDoubleList;

	int theNumberOfPrerequisiteOperations;
	QList<RPointer<RParameter> >thePrerequisiteOperationsFinishStatusList;

	QList<QList<RParameter *> > *prerequisiteOperationsFinishStatusListList;
	QList<double> *importanceValueList;
	QList<double>* sortedImportanceList;
	QList<double>* backUpSortedImportanceList;

	QList <bool>* prerequsiteDoneBoolList;

	//The 4 lists which contains  operated, operating, unoperated, and OperationFree components
	//OPERATION FREE
	QList<RParameter *>* theOperationFreeOperationFinishStatusList;

	//UNOPERATED
	QList<RParameter *>* theUnoperatedOperationTimeList;
	QList<double>* theUnoperatedOperationTimeDoubleList;
	QList<RParameter *>* theUnoperatedOperationFinishStatusList;
	QList<QList<RParameter *> >* theUnoperatedPrerequisiteOperationsFinishStatusListList;
	QList<double>* theUnoperatedImportanceList;

	//OPERATING
	QList<RParameter *>* theOperatingOperationFinishStatusList;

	//OPERATED
	QList<RParameter *>* theOperatedOperationFinishStatusList;

};

#endif // ROperationEventType1Model_H
#pragma once
