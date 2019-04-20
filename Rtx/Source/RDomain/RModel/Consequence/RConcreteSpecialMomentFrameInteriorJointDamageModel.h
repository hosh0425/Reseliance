#ifndef RConcreteSpecialMomentFrameInteriorJointDamageModel_H	
#define RConcreteSpecialMomentFrameInteriorJointDamageModel_H	
	
#include "RModel.h"	
	
class RParameter;	
class RResponse;	
	
class RConcreteSpecialMomentFrameInteriorJointDamageModel : public RModel	
{	
	Q_OBJECT
	
	Q_PROPERTY(QObject *InterstoryDriftRatio READ getInterstoryDriftRatio WRITE setInterstoryDriftRatio)
	Q_PROPERTY(QObject *BeamDepth READ getBeamDepth WRITE setBeamDepth)
	Q_PROPERTY(QObject *BeamLength READ getBeamLength WRITE setBeamLength)
	Q_PROPERTY(QObject *Theta1 READ getTheta1 WRITE setTheta1)
	Q_PROPERTY(QObject *Epsilon READ getEpsilon WRITE setEpsilon)
	Q_PROPERTY(QObject *Sigma READ getSigma WRITE setSigma)



	
public:	
	RConcreteSpecialMomentFrameInteriorJointDamageModel(QObject *parent, QString name);
	~RConcreteSpecialMomentFrameInteriorJointDamageModel();
	
	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int ThreadID);
	
	QObject *getInterstoryDriftRatio() const;
	void setInterstoryDriftRatio(QObject *value);
	QObject *getBeamDepth() const;
	void setBeamDepth(QObject *value);
	QObject *getBeamLength() const;
	void setBeamLength(QObject *value);
	QObject *getTheta1() const;
	void setTheta1(QObject *value);
	QObject *getEpsilon() const;
	void setEpsilon(QObject *value);
	QObject *getSigma() const;
	void setSigma(QObject *value);
	bool canBeRunParallelly();



private:	
	RPointer<RParameter> theInterstoryDriftRatio;
	RPointer<RParameter> theBeamDepth;
	RPointer<RParameter> theBeamLength;
	RPointer<RParameter> theTheta1;
	RPointer<RParameter> theEpsilon;
	RPointer<RParameter> theSigma;


	// Declare responses
	RResponse *theRCSMFDamageRatio;
	
};	
	
#endif // RConcreteSpecialMomentFrameInteriorJointDamageModel_H	




