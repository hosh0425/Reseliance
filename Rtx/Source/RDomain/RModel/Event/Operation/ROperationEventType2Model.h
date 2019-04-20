#ifndef ROperationEventType2Model_H
#define ROperationEventType2Model_H

#include "RModel.h"
#include "REventModel.h"

class RResponse;
class RParameter;
class RRandomVariable;


class ROperationEventType2Model : public REventModel
{
	Q_OBJECT
		Q_PROPERTY(QObject *NewOperationDemand READ getNewOperationDemand WRITE setNewOperationDemand)
		Q_PROPERTY(QString OperationCapacityList READ getOperationCapacityList WRITE setOperationCapacityList)

		Q_PROPERTY(QString OperationTimeList READ getOperationTimeList WRITE setOperationTimeList)
		Q_PROPERTY(QString OperationFinishStatusList READ getOperationFinishStatusList WRITE setOperationFinishStatusList)
		Q_PROPERTY(RPriorityRule PriorityRule READ getPriorityRule WRITE setPriorityRule)
		Q_ENUMS(RPriorityRule)
		Q_PROPERTY(QString ImportanceMeasureList READ getImportanceMeasureList WRITE setImportanceMeasureList)

		Q_PROPERTY(double NumberOfPrerequisiteOperations READ getNumberOfPrerequisiteOperations WRITE setNumberOfPrerequisiteOperations)
		Q_PROPERTY(QString PrerequisiteOperationsFinishStatusList READ getPrerequisiteOperationsFinishStatusList WRITE setPrerequisiteOperationsFinishStatusList)

public:

	ROperationEventType2Model(QObject *parent, QString name);
	~ROperationEventType2Model();

	QObject *getNewOperationDemand() const;
	void setNewOperationDemand(QObject *value);

	QString getOperationCapacityList();
	void setOperationCapacityList(QString value);

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
	RPointer<RParameter> theNewOperationDemand;
	int initialOperationCapacity;

	int* theBackupNewOperationDemand;
	
	QList<RPointer<RParameter> > theOperationCapacityList;
	QList<RPointer<RParameter> > theOperationTimeList;
	QList<RPointer<RParameter> > theOperationFinishStatusList;
	RPriorityRule thePriorityRule;
	QList<RPointer<RParameter> > theImportanceMeasureList;

	QList<double> theOperationTimeDoubleList;

	int theNumberOfPrerequisiteOperations;
	QList<RPointer<RParameter> > thePrerequisiteOperationsFinishStatusList;

	QList<QList<RParameter *> > *prerequisiteOperationsFinishStatusListList;
	QList<double> *importanceValueList;
	QList<double> *sortedImportanceList;

	QList <bool> *prerequsiteDoneBoolList;

	//Determines if it is the first time that this model is called
	bool *startSample;

	int *operationDemandValue;
	int *operatingDemandValue;
	int *operatedDemandValue;
};

#endif // ROperationEventType2Model_H
#pragma once
