#ifndef RAbrahamsonSilva2008IntensityModel_H	
#define RAbrahamsonSilva2008IntensityModel_H	
	
#include "RModel.h"	
	
class RParameter;	
class RResponse;
class RLocation;
	
class RAbrahamsonSilva2008IntensityModel : public RModel	
{	
	Q_OBJECT
	
	Q_PROPERTY(QString MagnitudeList READ getMagnitudeList WRITE setMagnitudeList)
	Q_PROPERTY(QString HypocentreLocationList READ getHypocentreLocationList WRITE setHypocentreLocationList)
	Q_PROPERTY(QString FaultTypes READ getFaultTypes WRITE setFaultTypes)
	Q_PROPERTY(RShockType ShockType READ getShockType WRITE setShockType)
	Q_PROPERTY(RResponseType ResponseType READ getResponseType WRITE setResponseType)
	Q_PROPERTY(QString PeriodList READ getPeriodList WRITE setPeriodList)
	Q_PROPERTY(QString EpsilonUncertaintyList READ getEpsilonUncertaintyList WRITE setEpsilonUncertaintyList)
	Q_PROPERTY(QString EtaUncertaintyList READ getEtaUncertaintyList WRITE setEtaUncertaintyList)
	Q_PROPERTY(QString StructureLocationList READ getStructureLocationList WRITE setStructureLocationList)
	Q_PROPERTY(QString ShearWaveVelocityList READ getShearWaveVelocityList WRITE setShearWaveVelocityList)

	Q_ENUMS(RFaultType)
	Q_ENUMS(RShockType)
	Q_ENUMS(RResponseType)

public:	
	RAbrahamsonSilva2008IntensityModel(QObject *parent, QString name);
	~RAbrahamsonSilva2008IntensityModel();

	enum RFaultType{StrikeSlip, NormalSlip, ReverseSlip};
	enum RShockType{MainShock , AfterShock};
	enum RResponseType{Sa, PGA, PGV};

	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID);
	
	QString getMagnitudeList();
	void setMagnitudeList(QString value);
	QString getHypocentreLocationList();
	void setHypocentreLocationList(QString value);
	QString getFaultTypes() const;
	void setFaultTypes(QString value);
	RShockType getShockType() const;
	void setShockType(RShockType value);
	RResponseType getResponseType() const;
	void setResponseType(RResponseType value);
	QString getPeriodList();
	void setPeriodList(QString value);
	QString getEpsilonUncertaintyList();
	void setEpsilonUncertaintyList(QString value);
	QString getEtaUncertaintyList();
	void setEtaUncertaintyList(QString value);
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
	QList<RFaultType> theFaultTypes;
	RShockType theShockType;
	RResponseType theResponseType;
	QList<RPointer<RParameter> >  thePeriodList;
	QList<RPointer<RParameter> >  theEtaUncertaintyList;
	QList<RPointer<RParameter> >  theEpsilonUncertaintyList;
	QList<RPointer<RParameter> > theStructureLocationList;
	QList<RPointer<RParameter> > theShearWaveVelocityList;
		
	// Declare responses
	QList<RResponse *> theSpectralResponseList;
	
};	
	
#endif // RAbrahamsonSilva2008IntensityModel_H	