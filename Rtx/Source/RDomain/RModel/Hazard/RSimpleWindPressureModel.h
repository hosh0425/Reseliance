#ifndef RSimpleWindPressureModel_H
#define RSimpleWindPressureModel_H

#include "RModel.h"

class RParameter;
class RResponse;

class RSimpleWindPressureModel : public RModel
{
	Q_OBJECT

	Q_PROPERTY(QObject *Velocity READ getVelocity WRITE setVelocity)
	Q_PROPERTY(QObject *AirDensity READ getAirDensity WRITE setAirDensity)
	Q_PROPERTY(QObject *ExposureFactor READ getExposureFactor WRITE setExposureFactor)
	Q_PROPERTY(QObject *ExternalPressureFactor READ getExternalPressureFactor WRITE setExternalPressureFactor)
	Q_PROPERTY(QObject *GustFactor READ getGustFactor WRITE setGustFactor)
	Q_PROPERTY(QObject *Epsilon READ getEpsilon WRITE setEpsilon)

public:
	// Define the constructor and destructor
	RSimpleWindPressureModel(QObject *parent, QString name);
	~RSimpleWindPressureModel();

	// Define the mandatory read/write methods for all QProperties
	QObject *getVelocity() const;
	void setVelocity(QObject *value);
	QObject *getAirDensity() const;
	void setAirDensity(QObject *value);
	QObject *getExposureFactor() const;
	void setExposureFactor(QObject *value);
	QObject *getExternalPressureFactor() const;
	void setExternalPressureFactor(QObject *value);
	QObject *getGustFactor() const;
	void setGustFactor(QObject *value);
	QObject *getEpsilon() const;
	void setEpsilon(QObject *value);

	// Define the method where the actual model is coded
	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID);

	// returns true as this model is parallelized
	bool canBeRunParallelly();

private:

	RPointer<RParameter> theVelocity;
	RPointer<RParameter> theAirDensity;
	RPointer<RParameter> theExposureFactor;
	RPointer<RParameter> theExternalPressureFactor;
	RPointer<RParameter> theGustFactor;
	RPointer<RParameter> theEpsilon;
	
	// Declare responses
	RResponse *theWindPressure;

};

#endif // RSimpleWindPressureModel_H
