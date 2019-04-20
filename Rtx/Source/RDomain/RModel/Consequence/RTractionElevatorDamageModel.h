#ifndef RTractionElevatorDamageModel_H	
#define RTractionElevatorDamageModel_H	
	
#include "RModel.h"	
	
class RParameter;	
class RResponse;	
	
class RTractionElevatorDamageModel : public RModel	
{	
	Q_OBJECT
	
	Q_PROPERTY(QObject *PeakGroundAcceleration READ getPeakGroundAcceleration WRITE setPeakGroundAcceleration)
	Q_PROPERTY(QObject *Theta1 READ getTheta1 WRITE setTheta1)
	Q_PROPERTY(QObject *Epsilon READ getEpsilon WRITE setEpsilon)
	Q_PROPERTY(QObject *Sigma READ getSigma WRITE setSigma)



	
public:	
	RTractionElevatorDamageModel(QObject *parent, QString name);
	~RTractionElevatorDamageModel();
	
	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int ThreadID);
	
	QObject *getPeakGroundAcceleration() const;
	void setPeakGroundAcceleration(QObject *value);
	QObject *getTheta1() const;
	void setTheta1(QObject *value);
	QObject *getEpsilon() const;
	void setEpsilon(QObject *value);
	QObject *getSigma() const;
	void setSigma(QObject *value);
	bool canBeRunParallelly();


private:	
	RPointer<RParameter> thePeakGroundAcceleration;
	RPointer<RParameter> theTheta1;
	RPointer<RParameter> theEpsilon;
	RPointer<RParameter> theSigma;

	// Declare responses
	RResponse *theTractionElevatorDamageRatio;
	
};	
	
#endif // RTractionElevatorDamageModel_H	



