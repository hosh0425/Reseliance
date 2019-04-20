#ifndef RRezaeianDerKiureghianArtificialGroundMotionModel_H	
#define RRezaeianDerKiureghianArtificialGroundMotionModel_H	
	
#include "RModel.h"	
	
class RParameter;	
class RResponse;
class RLocation;
	
class RRezaeianDerKiureghianArtificialGroundMotionModel : public RModel	
{	
	Q_OBJECT
	
	Q_PROPERTY(QString WhiteNoiseVariableList READ getWhiteNoiseVariableList WRITE setWhiteNoiseVariableList)
	Q_PROPERTY(RFaultType FaultType READ getFaultType WRITE setFaultType)
	Q_PROPERTY(QObject *MomentMagnitude READ getMomentMagnitude WRITE setMomentMagnitude)
	Q_PROPERTY(QObject *RuptureLocation READ getRuptureLocation WRITE setRuptureLocation)
	Q_PROPERTY(QObject *StructureLocation READ getStructureLocation WRITE setStructureLocation)
	Q_PROPERTY(QObject *ShearWaveVelocity READ getShearWaveVelocity WRITE setShearWaveVelocity)
	Q_PROPERTY(QObject *V1IntereventOrTotalModelError READ getV1IntereventOrTotalModelError WRITE setV1IntereventOrTotalModelError)
	Q_PROPERTY(QObject *V1IntraeventModelError READ getV1IntraeventModelError WRITE setV1IntraeventModelError)
	Q_PROPERTY(QObject *V2IntereventOrTotalModelError READ getV2IntereventOrTotalModelError WRITE setV2IntereventOrTotalModelError)
	Q_PROPERTY(QObject *V2IntraeventModelError READ getV2IntraeventModelError WRITE setV2IntraeventModelError)
	Q_PROPERTY(QObject *V3IntereventOrTotalModelError READ getV3IntereventOrTotalModelError WRITE setV3IntereventOrTotalModelError)
	Q_PROPERTY(QObject *V3IntraeventModelError READ getV3IntraeventModelError WRITE setV3IntraeventModelError)
	Q_PROPERTY(QObject *V4IntereventOrTotalModelError READ getV4IntereventOrTotalModelError WRITE setV4IntereventOrTotalModelError)
	Q_PROPERTY(QObject *V4IntraeventModelError READ getV4IntraeventModelError WRITE setV4IntraeventModelError)
	Q_PROPERTY(QObject *V5IntereventOrTotalModelError READ getV5IntereventOrTotalModelError WRITE setV5IntereventOrTotalModelError)
	Q_PROPERTY(QObject *V5IntraeventModelError READ getV5IntraeventModelError WRITE setV5IntraeventModelError)
	Q_PROPERTY(QObject *V6IntereventOrTotalModelError READ getV6IntereventOrTotalModelError WRITE setV6IntereventOrTotalModelError)
	Q_PROPERTY(QObject *V6IntraeventModelError READ getV6IntraeventModelError WRITE setV6IntraeventModelError)
	Q_PROPERTY(double Duration READ getDuration WRITE setDuration)
	Q_PROPERTY(double TimeStep READ getTimeStep WRITE setTimeStep)

	Q_ENUMS(RFaultType)

public:	
	RRezaeianDerKiureghianArtificialGroundMotionModel(QObject *parent, QString name);
	~RRezaeianDerKiureghianArtificialGroundMotionModel();

	enum RFaultType{StrikeSlip, Reverse};

	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int ThreadID); 
	
	QString getWhiteNoiseVariableList();
	void setWhiteNoiseVariableList(QString value);
	RFaultType getFaultType() const;
	void setFaultType(RFaultType value);
	QObject *getMomentMagnitude() const;
	void setMomentMagnitude(QObject *value);
	QObject *getRuptureLocation() const;
	void setRuptureLocation(QObject *value);
	QObject *getShearWaveVelocity() const;
	void setShearWaveVelocity(QObject *value);
	QObject *getV1IntereventOrTotalModelError() const;
	void setV1IntereventOrTotalModelError(QObject *value);
	QObject *getV1IntraeventModelError() const;
	void setV1IntraeventModelError(QObject *value);
	QObject *getV2IntereventOrTotalModelError() const;
	void setV2IntereventOrTotalModelError(QObject *value);
	QObject *getV2IntraeventModelError() const;
	void setV2IntraeventModelError(QObject *value);
	QObject *getV3IntereventOrTotalModelError() const;
	void setV3IntereventOrTotalModelError(QObject *value);
	QObject *getV3IntraeventModelError() const;
	void setV3IntraeventModelError(QObject *value);
	QObject *getV4IntereventOrTotalModelError() const;
	void setV4IntereventOrTotalModelError(QObject *value);
	QObject *getV4IntraeventModelError() const;
	void setV4IntraeventModelError(QObject *value);
	QObject *getV5IntereventOrTotalModelError() const;
	void setV5IntereventOrTotalModelError(QObject *value);
	QObject *getV5IntraeventModelError() const;
	void setV5IntraeventModelError(QObject *value);
	QObject *getV6IntereventOrTotalModelError() const;
	void setV6IntereventOrTotalModelError(QObject *value);
	QObject *getV6IntraeventModelError() const;
	void setV6IntraeventModelError(QObject *value);
	QObject *getStructureLocation() const;
	void setStructureLocation(QObject *value);
	double getDuration() const;
	void setDuration(double value);
	double getTimeStep() const;
	void setTimeStep(double value);

	QList<RParameter *> getPhysicalParameterList();

	// returns true as this model is parallelized
	bool canBeRunParallelly();


private:
	int simulate(double dt, int accN, double ariasInt, double effDur, double midStrong, double omegP, double omegM, double kis, gsl_vector *whiteNoiseVec, gsl_vector *simulMotion);
	int calculatingParameters(double magn, double rupDist, double shVel, int ftType, double v1Err, double v2Err, double v3Err, double v4Err, double v5Err, double v6Err, gsl_matrix *beta, double *ariasInt, double *effDur, double *midStrong, double *omegP, double *omegM, double *kis);
	int solve(double tMid, double duration, double *alfa2, double *alfa3);
	int sigmaSU(int m, double dT, double t, double tMid, double omegP, double omegM, double kis, gsl_vector *whiteNoiseVec, double *sigma);
	int pseudoAccResponseSDOF(double totTime, double time,double kis,double omega, double *h);
	int highPassFilter(int n, double dt, gsl_vector *simGM, double omC);
	//int alfaFinding_f (const gsl_vector * x, void *params, gsl_vector * f);

	QList<RPointer<RParameter> > theWhiteNoiseVariableList;
	RFaultType theFaultType;
	RPointer<RParameter> theMomentMagnitude;
	RPointer<RParameter> theRuptureLocation;
	RPointer<RParameter> theShearWaveVelocity;
	RPointer<RParameter> theV1IntereventOrTotalModelError;
	RPointer<RParameter> theV2IntereventOrTotalModelError;
	RPointer<RParameter> theV3IntereventOrTotalModelError;
	RPointer<RParameter> theV4IntereventOrTotalModelError;
	RPointer<RParameter> theV5IntereventOrTotalModelError;
	RPointer<RParameter> theV6IntereventOrTotalModelError;
	RPointer<RParameter> theV1IntraeventModelError;
	RPointer<RParameter> theV2IntraeventModelError;
	RPointer<RParameter> theV3IntraeventModelError;
	RPointer<RParameter> theV4IntraeventModelError;
	RPointer<RParameter> theV5IntraeventModelError;
	RPointer<RParameter> theV6IntraeventModelError;
	RPointer<RParameter> theStructureLocation;
	double theDuration;
	double theTimeStep;

		
	// Declare responses
	RResponse *thePeakGroundAccelerationResponse;
	RResponse *theRuptureDistanceResponse;
};	
	
#endif // RRezaeianDerKiureghianArtificialGroundMotionModel_H	