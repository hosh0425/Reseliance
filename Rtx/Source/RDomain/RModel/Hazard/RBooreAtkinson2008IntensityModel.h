#ifndef RBooreAtkinson2008IntensityModel_H	
#define RBooreAtkinson2008IntensityModel_H	
	
#include "RModel.h"	
	
class RParameter;	
class RResponse;
class RLocation;
	
class RBooreAtkinson2008IntensityModel : public RModel	
{	
	Q_OBJECT
	
	Q_PROPERTY(QString MagnitudeList READ getMagnitudeList WRITE setMagnitudeList)
	Q_PROPERTY(QString HypocentreLocationList READ getHypocentreLocationList WRITE setHypocentreLocationList)
	Q_PROPERTY(QString EpsilonUncertaintyList READ getEpsilonUncertaintyList WRITE setEpsilonUncertaintyList)
	Q_PROPERTY(QString EtaUncertaintyList READ getEtaUncertaintyList WRITE setEtaUncertaintyList)
	Q_PROPERTY(QString FaultTypes READ getFaultTypes WRITE setFaultTypes)
	Q_PROPERTY(RResponseType ResponseType READ getResponseType WRITE setResponseType)
	Q_PROPERTY(RSmoothness Smoothness READ getSmoothness WRITE setSmoothness)
	Q_PROPERTY(QString PeriodList READ getPeriodList WRITE setPeriodList)
	Q_PROPERTY(QString StructureLocationList READ getStructureLocationList WRITE setStructureLocationList)
	Q_PROPERTY(QString ShearWaveVelocityList READ getShearWaveVelocityList WRITE setShearWaveVelocityList)

	Q_ENUMS(RFaultType)
	Q_ENUMS(RResponseType)
	Q_ENUMS(RSmoothness)

public:	
	RBooreAtkinson2008IntensityModel(QObject *parent, QString name);
	~RBooreAtkinson2008IntensityModel();

	enum RFaultType{Unspecified, StrikeSlip, NormalSlip, ReverseSlip};
	enum RResponseType{Sa, PGA, PGV};
	enum RSmoothness{Smoothed, Original};

	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID);
	
	QString getMagnitudeList();
	void setMagnitudeList(QString value);
	QString getHypocentreLocationList();
	void setHypocentreLocationList(QString value);
	QString getEpsilonUncertaintyList();
	void setEpsilonUncertaintyList(QString value);
	QString getEtaUncertaintyList();
	void setEtaUncertaintyList(QString value);
	QString getFaultTypes() const;
	void setFaultTypes(QString value);
	RResponseType getResponseType() const;
	void setResponseType(RResponseType value);
	RSmoothness getSmoothness() const;
	void setSmoothness(RSmoothness value);
	QString getPeriodList();
	void setPeriodList(QString value);
	QString getStructureLocationList();
	void setStructureLocationList(QString value);
	QString getShearWaveVelocityList();
	void setShearWaveVelocityList(QString value);

	QList<RParameter *> getPhysicalParameterList();

	// returns true as this model is parallelized
	bool canBeRunParallelly();

private:	
	void createResponses();

	QList<RPointer<RParameter> > theMagnitudeList;
	QList<RPointer<RParameter> > theHypocentreLocationList;
	QList<RPointer<RParameter> >  theEtaUncertaintyList;
	QList<RPointer<RParameter> >  theEpsilonUncertaintyList;
	QList<RFaultType> theFaultTypes;
	RResponseType theResponseType;
	RSmoothness theSmoothness;
	QList<RPointer<RParameter> >  thePeriodList;
	QList<RPointer<RParameter> > theStructureLocationList;
	QList<RPointer<RParameter> > theShearWaveVelocityList;
		
	// Declare responses
	QList<RResponse *> theSpectralResponseList;
	
};	
	
#endif // RBooreAtkinson2008IntensityModel_H	