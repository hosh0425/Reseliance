#ifndef RFemaNibsBridgeDamageModel_H
#define RFemaNibsBridgeDamageModel_H

#include "RModel.h"

class RParameter;
class RResponse;

class RFemaNibsBridgeDamageModel : public RModel
{
	Q_OBJECT

	// Define a QProperty for each input (parameter) data member. The type of the QProperty is "QObject *"
	Q_PROPERTY(RAnalysisType AnalysisType READ getAnalysisType WRITE setAnalysisType)
	Q_PROPERTY(RBridgeType BridgeType READ getBridgeType WRITE setBridgeType)
	Q_PROPERTY(QObject *Sa1 READ getSa1 WRITE setSa1)
	Q_PROPERTY(QObject *Sa2 READ getSa2 WRITE setSa2)
	Q_PROPERTY(QObject *Time READ getTime WRITE setTime)
	Q_PROPERTY(int NumberOfSpans READ getNumberOfSpans WRITE setNumberOfSpans)
	Q_PROPERTY(double AngleOfSkew READ getAngleOfSkew WRITE setAngleOfSkew)
	Q_PROPERTY(QObject *PGD READ getPGD WRITE setPGD)
	Q_PROPERTY(double BridgeLength READ getBridgeLength WRITE setBridgeLength)
	Q_PROPERTY(double SpanWidth READ getSpanWidth WRITE setSpanWidth)
	Q_PROPERTY(QObject *ModelUncertainty READ getModelUncertainty WRITE setModelUncertainty)

	Q_ENUMS(RBridgeType)
	Q_ENUMS(RAnalysisType)

public:
	// Define the constructor and destructor
	RFemaNibsBridgeDamageModel(QObject *parent, QString name);
	~RFemaNibsBridgeDamageModel();

	enum RBridgeType{HWB1, HWB2, HWB3, HWB4, HWB5, HWB6, HWB7, HWB8, HWB9, HWB10, HWB11, HWB12, HWB13, HWB14, HWB15, HWB16, HWB17, HWB18, HWB19, HWB20, HWB21, HWB22, HWB23, HWB24, HWB25, HWB26, HWB27, HWB28};
	enum RAnalysisType{GroundShaking, GroundFailure, Both};

	// Define the mandatory read/write methods for all QProperties
	RAnalysisType getAnalysisType() const;
	void setAnalysisType(RAnalysisType value);
	RBridgeType getBridgeType() const;
	void setBridgeType(RBridgeType value);
	QObject *getSa1() const;
	void setSa1(QObject *value);
	QObject *getSa2() const;
	void setSa2(QObject *value);
	QObject *getTime() const;
	void setTime(QObject *value);
	int getNumberOfSpans() const;
	void setNumberOfSpans(int value);
	double getAngleOfSkew() const;
	void setAngleOfSkew(double value);
	QObject *getPGD() const;
	void setPGD(QObject *value);
	double getBridgeLength() const;
	void setBridgeLength(double value);
	double getSpanWidth() const;
	void setSpanWidth(double value);
	QObject *getModelUncertainty() const;
	void setModelUncertainty(QObject *value);

	// Define the method where the actual model is coded
	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID);

	
	// returns true as this model is parallelized
	bool canBeRunParallelly();

private:
	// Declare input data members
	RAnalysisType theAnalysisType;
	RBridgeType theBridgeType;
	RPointer<RParameter> theSa1;
	RPointer<RParameter> theSa2;
	RPointer<RParameter> theTime;
	int theNumberOfSpans;
	double theAngleOfSkew;
	RPointer<RParameter> thePGD;
	double theBridgeLength;
	double theSpanWidth;
	RPointer<RParameter> theModelUncertainty;
	
	// Declare responses
	RResponse *thePercentFunctional;
	RResponse *theDamageFactor;

};

#endif // RFemaNibsBridgeDamageModel_H
