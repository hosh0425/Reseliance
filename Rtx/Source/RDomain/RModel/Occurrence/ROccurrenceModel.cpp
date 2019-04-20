#include "ROccurrenceModel.h"

#include "RParameter.h"
#include "RRandomVariable.h"

#include "RDomain.h"

ROccurrenceModel::ROccurrenceModel(QObject *parent, QString name)
	: RModel(parent, name)
{
	//$$ theMagnitudeModel = 0;
	//theStartTime = 0;
	//theEndTime = 0;

	int arraySizeForParallelizedObject = getArraySizeForParallelizedObject();

	theStartTimeVector = new QVector<double>[arraySizeForParallelizedObject];
	theEndTimeVector = new QVector<double>[arraySizeForParallelizedObject];
}

ROccurrenceModel::~ROccurrenceModel()
{
	delete [] theStartTimeVector;
	delete [] theEndTimeVector;
}

QObject * ROccurrenceModel::getMagnitudeModel() const	
{	
	return theMagnitudeModel.data();
}	
	

QString ROccurrenceModel::getNonzeroOccurrenceRate()
{
	return getStringParameter(&theNonzeroOccurrenceRateDouble, theNonzeroOccurrenceRateParameter.data());
}

void ROccurrenceModel::setNonzeroOccurrenceRate(QString value)
{
	setStringParameter(value, &theNonzeroOccurrenceRateDouble, &theNonzeroOccurrenceRateParameter);
}

double ROccurrenceModel::getNonzeroOccurrenceRateValue()
{
	return getDoubleParameter(&theNonzeroOccurrenceRateDouble, theNonzeroOccurrenceRateParameter.data());
}

void ROccurrenceModel::setMagnitudeModel(QObject *value)	
{	
	theMagnitudeModel = qobject_cast<RModel *>(value);
}	

QString ROccurrenceModel::getAssociatedModelList()	
{	
	return modelListToString(theAssociatedModelList);
}	
	
void ROccurrenceModel::setAssociatedModelList(QString value)	
{	
	theAssociatedModelList = stringToModelList(value);
}	

//QObject * ROccurrenceModel::getStartTime() const	
//{	
//	return theStartTime;
//}	
//	
//void ROccurrenceModel::setStartTime(QObject *value)	
//{	
//	theStartTime = qobject_cast<RParameter *>(value);
//}	
//	
//QObject * ROccurrenceModel::getEndTime() const	
//{	
//	return theEndTime;
//}	
//	
//void ROccurrenceModel::setEndTime(QObject *value)	
//{	
//	theEndTime = qobject_cast<RParameter *>(value);
//}	


QVector<double> ROccurrenceModel::getStartTimeVector(int threadID)
{
	return theStartTimeVector[threadID];
}

QVector<double> ROccurrenceModel::getEndTimeVector(int threadID)
{
	return theEndTimeVector[threadID];
}

QList<RModel *> ROccurrenceModel::getAssociatedModels()
{
	QList<RModel *> modelList;
	for (int i = 0; i< theAssociatedModelList.count(); i++) {
		RModel *model = qobject_cast<RModel *>(theAssociatedModelList[i].data());
		if (model) {
			modelList << model;
		}
	}
	return modelList;
}
