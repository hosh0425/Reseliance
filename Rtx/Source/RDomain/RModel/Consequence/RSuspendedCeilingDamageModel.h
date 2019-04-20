#ifndef RSuspendedCeilingDamageModel_H	
#define RSuspendedCeilingDamageModel_H	
	
#include "RModel.h"	
	
class RParameter;	
class RResponse;	
	
class RSuspendedCeilingDamageModel : public RModel	
{	
	Q_OBJECT
	
	Q_PROPERTY(QObject *PeakFloorAcceleration READ getPeakFloorAcceleration WRITE setPeakFloorAcceleration)
	Q_PROPERTY(double CeilingArea READ getCeilingArea WRITE setCeilingArea)
	Q_PROPERTY(RHasLateralSupport HasLateralSupport READ getHasLateralSupport WRITE setHasLateralSupport)
	Q_PROPERTY(QObject *Theta1 READ getTheta1 WRITE setTheta1)
	Q_PROPERTY(QObject *Theta2 READ getTheta2 WRITE setTheta2)
	Q_PROPERTY(QObject *Epsilon READ getEpsilon WRITE setEpsilon)
	Q_PROPERTY(QObject *Sigma READ getSigma WRITE setSigma)



	Q_ENUMS(RHasLateralSupport)
	
public:	
	RSuspendedCeilingDamageModel(QObject *parent, QString name);
	~RSuspendedCeilingDamageModel();

	enum RHasLateralSupport{No, Yes};
	
	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int ThreadID);
	
	QObject *getPeakFloorAcceleration() const;
	void setPeakFloorAcceleration(QObject *value);
	double getCeilingArea() const;
	void setCeilingArea(double value);
	RHasLateralSupport getHasLateralSupport() const;
	void setHasLateralSupport(RHasLateralSupport value);
	QObject *getTheta1() const;
	void setTheta1(QObject *value);
	QObject *getTheta2() const;
	void setTheta2(QObject *value);
	QObject *getEpsilon() const;
	void setEpsilon(QObject *value);
	QObject *getSigma() const;
	void setSigma(QObject *value);
	bool canBeRunParallelly();

private:	

	RPointer<RParameter> thePeakFloorAcceleration;
	double theCeilingArea;
	RHasLateralSupport theHasLateralSupport;
	RPointer<RParameter> theTheta1;
	RPointer<RParameter> theTheta2;
	RPointer<RParameter> theEpsilon;
	RPointer<RParameter> theSigma;

	// Declare responses
	RResponse *theCeilingDamageRatio;
	
};	
	
#endif // RSuspendedCeilingDamageModel_H	


