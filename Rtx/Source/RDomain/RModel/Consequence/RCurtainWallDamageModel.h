#ifndef RCurtainWallDamageModel_H	
#define RCurtainWallDamageModel_H	
	
#include "RModel.h"	
	
class RParameter;	
class RResponse;	
	
class RCurtainWallDamageModel : public RModel	
{	
	Q_OBJECT
	
	Q_PROPERTY(QObject *InterstoryDriftRatio READ getInterstoryDriftRatio WRITE setInterstoryDriftRatio)
	Q_PROPERTY(QObject *GlassToFrameClearance READ getGlassToFrameClearance WRITE setGlassToFrameClearance)
	Q_PROPERTY(RGlassType GlassType READ getGlassType WRITE setGlassType)
	Q_PROPERTY(RLaminatedGlass LaminatedGlass READ getLaminatedGlass WRITE setLaminatedGlass)
	Q_PROPERTY(QObject *Theta1 READ getTheta1 WRITE setTheta1)
	Q_PROPERTY(QObject *Epsilon READ getEpsilon WRITE setEpsilon)
	Q_PROPERTY(QObject *Sigma READ getSigma WRITE setSigma)



	Q_ENUMS(RGlassType)
	Q_ENUMS(RLaminatedGlass)
	
public:	
	RCurtainWallDamageModel(QObject *parent, QString name);
	~RCurtainWallDamageModel();

	enum RGlassType{monolithic, IGU, aIGU};
	enum RLaminatedGlass{No, Yes};
	
	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int ThreadID);
	
	QObject *getInterstoryDriftRatio() const;
	void setInterstoryDriftRatio(QObject *value);
	QObject *getGlassToFrameClearance() const;
	void setGlassToFrameClearance(QObject *value);
	RGlassType getGlassType() const;
	void setGlassType(RGlassType value);
	RLaminatedGlass getLaminatedGlass() const;
	void setLaminatedGlass(RLaminatedGlass value);
	QObject *getTheta1() const;
	void setTheta1(QObject *value);
	QObject *getEpsilon() const;
	void setEpsilon(QObject *value);
	QObject *getSigma() const;
	void setSigma(QObject *value);
	bool canBeRunParallelly();



private:	
	RPointer<RParameter> theInterstoryDriftRatio;
	RPointer<RParameter> theGlassToFrameClearance;
	RGlassType theGlassType;
	RLaminatedGlass theLaminatedGlass;
	RPointer<RParameter> theTheta1;
	RPointer<RParameter> theEpsilon;
	RPointer<RParameter> theSigma;


	// Declare responses
	RResponse *theRCurtainWallDamageRatio;
	
};	
	
#endif // RCurtainWallDamageModel_H	


