#ifndef ROccurrenceModel_H
#define ROccurrenceModel_H

#include "RModel.h"

#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

class RParameter;
class RRandomVariable;

class ROccurrenceModel : public RModel
{
	Q_OBJECT
	Q_PROPERTY(QString NonzeroOccurrenceRate READ getNonzeroOccurrenceRate WRITE setNonzeroOccurrenceRate)
	Q_PROPERTY(QObject *MagnitudeModel READ getMagnitudeModel WRITE setMagnitudeModel)
	Q_PROPERTY(QString AssociatedModelList READ getAssociatedModelList WRITE setAssociatedModelList)
	//Q_PROPERTY(QObject *StartTime READ getStartTime WRITE setStartTime)
	//Q_PROPERTY(QObject *EndTime READ getEndTime WRITE setEndTime)

public:
	ROccurrenceModel(QObject *parent, QString name);
	~ROccurrenceModel();

	QString getNonzeroOccurrenceRate();
	void setNonzeroOccurrenceRate(QString value);
	double getNonzeroOccurrenceRateValue();

	QObject *getMagnitudeModel() const;
	void setMagnitudeModel(QObject *value);
	QString getAssociatedModelList();
	void setAssociatedModelList(QString value);
	//QObject *getStartTime() const;
	//void setStartTime(QObject *value);
	//QObject *getEndTime() const;
	//void setEndTime(QObject *value);

	// Randomly defines durations that the model occures in them during the given time span, in the given threadID ($MTh$)
	virtual int establishTimeAxis(double T, gsl_rng *randomGenerator, int threadID) = 0;
	virtual bool isRareHazard() = 0;
	
	// returns the vector of time points that the model's occurence starts at them, in the given threadID ($MTh$)
	QVector<double> getStartTimeVector(int threadID);

	// returns the vector of time points that the model's occurence ends at them, in the given threadID ($MTh$)
	QVector<double> getEndTimeVector(int threadID);

	QList<RModel *> getAssociatedModels();

protected:
	RPointer<RModel> theMagnitudeModel;
	QList<RPointer<RModel> > theAssociatedModelList;
	//RPointer<RParameter> theStartTime;
	//RPointer<RParameter> theEndTime;

	// randomly defined durations that model occures in them (start and end) during a time span ($MTh$)
	QVector<double>* theStartTimeVector;
	QVector<double>* theEndTimeVector;

	RPointer<RParameter> theNonzeroOccurrenceRateParameter;
	double theNonzeroOccurrenceRateDouble;

private:

};

#endif // ROccurrenceModel_H
