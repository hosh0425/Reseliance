#ifndef RDuWang2012IntensityModel_H
#define RDuWang2012IntensityModel_H

#include "RModel.h"

class RParameter;
class RResponse;
class RLocation;

class RDuWang2012IntensityModel : public RModel
{
	Q_OBJECT

	Q_PROPERTY(QString MagnitudeList READ getMagnitudeList WRITE setMagnitudeList)
	Q_PROPERTY(QString HypocentreLocationList READ getHypocentreLocationList WRITE setHypocentreLocationList)
	Q_PROPERTY(QObject *IntereventSigma READ getIntereventSigma WRITE setIntereventSigma)
	Q_PROPERTY(QObject *IntraeventSigma READ getIntraeventSigma WRITE setIntraeventSigma)
	//Q_PROPERTY(QObject *Epsilon READ getEpsilon WRITE setEpsilon)
	Q_PROPERTY(RFaultType FaultType READ getFaultType WRITE setFaultType)
	Q_PROPERTY(RResponseType ResponseType READ getResponseType WRITE setResponseType)
	Q_PROPERTY(QString StructureLocationList READ getStructureLocationList WRITE setStructureLocationList)
	Q_PROPERTY(RSiteClass SiteClass READ getSiteClass WRITE setSiteClass)

	// Model Error Parameters
	Q_PROPERTY(QObject *C8Uncertainty READ getC8Uncertainty WRITE setC8Uncertainty)
	Q_PROPERTY(QObject *C1Uncertainty READ getC1Uncertainty WRITE setC1Uncertainty)
	Q_PROPERTY(QObject *C2Uncertainty READ getC2Uncertainty WRITE setC2Uncertainty)
	Q_PROPERTY(QObject *C3Uncertainty READ getC3Uncertainty WRITE setC3Uncertainty)
	Q_PROPERTY(QObject *C4Uncertainty READ getC4Uncertainty WRITE setC4Uncertainty)
	Q_PROPERTY(QObject *C5Uncertainty READ getC5Uncertainty WRITE setC5Uncertainty)
	Q_PROPERTY(QObject *C6Uncertainty READ getC6Uncertainty WRITE setC6Uncertainty)
	Q_PROPERTY(QObject *C7Uncertainty READ getC7Uncertainty WRITE setC7Uncertainty)
	Q_PROPERTY(QObject *HUncertainty READ getHUncertainty WRITE setHUncertainty)

	Q_ENUMS(RFaultType)
	Q_ENUMS(RResponseType)
	Q_ENUMS(RSiteClass)

public:
	RDuWang2012IntensityModel(QObject *parent, QString name);
	~RDuWang2012IntensityModel();

	enum RFaultType{Unspecified, Normal, Reverse, ReverseOblique};
	enum RResponseType{CAV};
	enum RSiteClass{UnspecifiedClass, SGSSiteClassC , SGSSiteClassD};

	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID);

	QString getMagnitudeList();
	void setMagnitudeList(QString value);
	QString getHypocentreLocationList();
	void setHypocentreLocationList(QString value);
	QObject *getIntereventSigma() const;
	void setIntereventSigma(QObject *value);
	QObject *getIntraeventSigma() const;
	void setIntraeventSigma(QObject *value);
	//QObject *getEpsilon() const;
	//void setEpsilon(QObject *value);
	RFaultType getFaultType() const;
	void setFaultType(RFaultType value);
	RResponseType getResponseType() const;
	void setResponseType(RResponseType value);
	QString getStructureLocationList();
	void setStructureLocationList(QString value);
	RSiteClass getSiteClass() const;
	void setSiteClass(RSiteClass value);
	// Model Error Parameters
	QObject *getC8Uncertainty() const;
	void setC8Uncertainty(QObject *value);
	QObject *getC1Uncertainty() const;
	void setC1Uncertainty(QObject *value);
	QObject *getC2Uncertainty() const;
	void setC2Uncertainty(QObject *value);
	QObject *getC3Uncertainty() const;
	void setC3Uncertainty(QObject *value);
	QObject *getC4Uncertainty() const;
	void setC4Uncertainty(QObject *value);
	QObject *getC5Uncertainty() const;
	void setC5Uncertainty(QObject *value);
	QObject *getC6Uncertainty() const;
	void setC6Uncertainty(QObject *value);
	QObject *getC7Uncertainty() const;
	void setC7Uncertainty(QObject *value);
	QObject *getHUncertainty() const;
	void setHUncertainty(QObject *value);

	// returns true as this model is parallelized
	bool canBeRunParallelly();

private:
	void createResponses();

	QList<RPointer<RParameter> > theMagnitudeList;
	QList<RPointer<RParameter> > theHypocentreLocationList;
	RPointer<RParameter> theIntereventSigma;
	RPointer<RParameter> theIntraeventSigma;
	//RPointer<RParameter> theEpsilon;
	RFaultType theFaultType;
	RResponseType theResponseType;
	QList<RPointer<RParameter> > theStructureLocationList;
	RSiteClass theSiteClass;

	// Model Error Parameters
	RPointer<RParameter> theC8Uncertainty;
	RPointer<RParameter> theC1Uncertainty;
	RPointer<RParameter> theC2Uncertainty;
	RPointer<RParameter> theC3Uncertainty;	
	RPointer<RParameter> theC4Uncertainty;
	RPointer<RParameter> theC5Uncertainty;
	RPointer<RParameter> theC6Uncertainty;
	RPointer<RParameter> theC7Uncertainty;	
	RPointer<RParameter> theHUncertainty;	

	// Declare responses
	QList<RResponse *> theSpectralResponseList;

};

#endif // RDuWang2012IntensityModel_H