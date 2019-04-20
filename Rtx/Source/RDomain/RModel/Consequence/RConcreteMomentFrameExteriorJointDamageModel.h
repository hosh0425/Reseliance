#ifndef RConcreteMomentFrameExteriorJointDamageModel_H	
#define RConcreteMomentFrameExteriorJointDamageModel_H	
	
#include "RModel.h"	
	
class RParameter;	
class RResponse;	
	
class RConcreteMomentFrameExteriorJointDamageModel : public RModel	
{	
	Q_OBJECT
	
	Q_PROPERTY(QObject *InterstoryDriftRatio READ getInterstoryDriftRatio WRITE setInterstoryDriftRatio)
	Q_PROPERTY(QObject *Theta1 READ getTheta1 WRITE setTheta1)
	Q_PROPERTY(QObject *Epsilon READ getEpsilon WRITE setEpsilon)
	Q_PROPERTY(QObject *Sigma READ getSigma WRITE setSigma)



	Q_ENUMS(RHasLateralSupport)
	
public:	
	RConcreteMomentFrameExteriorJointDamageModel(QObject *parent, QString name);
	~RConcreteMomentFrameExteriorJointDamageModel();

	
	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int ThreadID);
	
	QObject *getInterstoryDriftRatio() const;
	void setInterstoryDriftRatio(QObject *value);
	QObject *getTheta1() const;
	void setTheta1(QObject *value);
	QObject *getEpsilon() const;
	void setEpsilon(QObject *value);
	QObject *getSigma() const;
	void setSigma(QObject *value);
	bool canBeRunParallelly();


private:	
	RPointer<RParameter> theInterstoryDriftRatio;
	RPointer<RParameter> theTheta1;
	RPointer<RParameter> theEpsilon;
	RPointer<RParameter> theSigma;



	// Declare responses
	RResponse *theExteriorJointDamageRatio;
	
};	
	
#endif // RConcreteMomentFrameExteriorJointDamageModel_H	


