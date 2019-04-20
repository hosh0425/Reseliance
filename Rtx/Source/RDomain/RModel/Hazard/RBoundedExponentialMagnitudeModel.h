#ifndef RBoundedExponentialMagnitudeModel_H
#define RBoundedExponentialMagnitudeModel_H

#include "RModel.h"

class RParameter;
class RResponse;

class RBoundedExponentialMagnitudeModel : public RModel
{
	Q_OBJECT

	Q_PROPERTY(QObject *MinimumMagnitude READ getMinimumMagnitude WRITE setMinimumMagnitude)
	Q_PROPERTY(QObject *MaximumMagnitude READ getMaximumMagnitude WRITE setMaximumMagnitude)
	Q_PROPERTY(QObject *Beta READ getBeta WRITE setBeta)
	Q_PROPERTY(QObject *Theta READ getTheta WRITE setTheta)


public:
	RBoundedExponentialMagnitudeModel(QObject *parent, QString name);
	~RBoundedExponentialMagnitudeModel();

	QObject *getMinimumMagnitude() const;
	void setMinimumMagnitude(QObject *value);
	QObject *getMaximumMagnitude() const;
	void setMaximumMagnitude(QObject *value);
	QObject *getBeta() const;
	void setBeta(QObject *value);
	QObject *getTheta() const;
	void setTheta(QObject *value);

	QList<RParameter *> getPhysicalParameterList();

	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID);

	// returns true as this model is parallelized
	bool canBeRunParallelly();

private:
	RPointer<RParameter> theMinimumMagnitude;
	RPointer<RParameter> theMaximumMagnitude;
	RPointer<RParameter> theBeta;
	RPointer<RParameter> theTheta;
	
	RResponse *theMagnitude;

};

#endif // RBoundedExponentialMagnitudeModel_H
