#ifndef RSimpleDamageCurveModel_H	
#define RSimpleDamageCurveModel_H	
	
#include "RModel.h"	

class RParameter;	
class RResponse;
class RLocation;
	
class RSimpleDamageCurveModel : public RModel	
{	
	Q_OBJECT
	Q_PROPERTY(QObject *Intensity READ getIntensity WRITE setIntensity)
	Q_PROPERTY(QObject *MinIntensity READ getMinIntensity WRITE setMinIntensity)
	Q_PROPERTY(QObject *MaxIntensity READ getMaxIntensity WRITE setMaxIntensity)
	Q_PROPERTY(QObject *Epsilon READ getEpsilon WRITE setEpsilon)
	
public:	
	RSimpleDamageCurveModel(QObject *parent, QString name);
	~RSimpleDamageCurveModel();

	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID);

	QObject *getIntensity() const;
	void setIntensity(QObject *value);
	QObject *getMinIntensity() const;
	void setMinIntensity(QObject *value);
	QObject *getMaxIntensity() const;
	void setMaxIntensity(QObject *value);
	QObject *getEpsilon() const;
	void setEpsilon(QObject *value);

	// returns true as this model is parallelized
	bool canBeRunParallelly();

private:	
	RPointer<RParameter> theIntensity;
	RPointer<RParameter> theMinIntensity;
	RPointer<RParameter> theMaxIntensity;
	RPointer<RParameter> theEpsilon;

	// Declare responses
	RResponse *theDamageFactor;
};	

#endif	
