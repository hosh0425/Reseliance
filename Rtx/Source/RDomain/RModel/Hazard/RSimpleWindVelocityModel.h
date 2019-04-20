#ifndef RSimpleWindVelocityModel_H
#define RSimpleWindVelocityModel_H

#include "RModel.h"

class RParameter;
class RResponse;

class RSimpleWindVelocityModel : public RModel
{
	Q_OBJECT

	Q_PROPERTY(QObject *Theta READ getTheta WRITE setTheta)
	Q_PROPERTY(QObject *V10 READ getV10 WRITE setV10)
	Q_PROPERTY(QObject *V50 READ getV50 WRITE setV50)

public:
	// Define the constructor and destructor
	RSimpleWindVelocityModel(QObject *parent, QString name);
	~RSimpleWindVelocityModel();

	// Define the mandatory read/write methods for all QProperties
	QObject *getTheta() const;
	void setTheta(QObject *value);
	QObject *getV10() const;
	void setV10(QObject *value);
	QObject *getV50() const;
	void setV50(QObject *value);

	QList<RParameter *> getPhysicalParameterList();

	// Define the method where the actual model is coded
	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID);

	// returns true as this model is parallelized
	bool canBeRunParallelly();

private:

	RPointer<RParameter> theTheta;
	RPointer<RParameter> theV10;
	RPointer<RParameter> theV50;
	
	// Declare responses
	RResponse *theWindSpeed;

};

#endif // RSimpleWindVelocityModel_H
