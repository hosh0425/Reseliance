#ifndef RGypsumWallboardPartitionDamageModel_H	
#define RGypsumWallboardPartitionDamageModel_H	
	
#include "RModel.h"	

class RParameter;	
class RResponse;
	
class RGypsumWallboardPartitionDamageModel : public RModel	
{	
	Q_OBJECT
	
	Q_PROPERTY(QObject *InterstoryDriftRatio READ getInterstoryDriftRatio WRITE setInterstoryDriftRatio)
	Q_PROPERTY(RHasReturnWall HasReturnWall READ getHasReturnWall WRITE setHasReturnWall)
	Q_PROPERTY(QObject *Theta1 READ getTheta1 WRITE setTheta1)
	Q_PROPERTY(QObject *Theta2 READ getTheta2 WRITE setTheta2)
	Q_PROPERTY(QObject *Epsilon READ getEpsilon WRITE setEpsilon)
	Q_PROPERTY(QObject *Sigma READ getSigma WRITE setSigma)


	Q_ENUMS(RHasReturnWall)
	
public:	
	RGypsumWallboardPartitionDamageModel(QObject *parent, QString name);
	~RGypsumWallboardPartitionDamageModel();

	enum RHasReturnWall{No, Yes};
	
	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int ThreadID);
	
	QObject *getInterstoryDriftRatio() const;
	void setInterstoryDriftRatio(QObject *value);
	RHasReturnWall getHasReturnWall() const;
	void setHasReturnWall(RHasReturnWall value);
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
	RPointer<RParameter> theInterstoryDriftRatio;
	RHasReturnWall theHasReturnWall;
	RPointer<RParameter> theTheta1;
	RPointer<RParameter> theTheta2;
	RPointer<RParameter> theEpsilon;
	RPointer<RParameter> theSigma;


	// Declare responses
	RResponse *theDamageRatio;

};	

#endif // RGypsumWallboardPartitionDamageModel_H

