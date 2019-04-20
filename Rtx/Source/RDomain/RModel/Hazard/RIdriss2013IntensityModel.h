#ifndef RIdriss2013IntensityModel_H	
#define RIdriss2013IntensityModel_H	
	
#include "RModel.h"	
	
class RParameter;	
class RResponse;
class RLocation;
	
class RIdriss2013IntensityModel : public RModel	
{	
	Q_OBJECT
	
	Q_PROPERTY(QString MagnitudeList READ getMagnitudeList WRITE setMagnitudeList)
	Q_PROPERTY(QString DepthToTopOfRuptureList READ getDepthToTopOfRuptureList WRITE setDepthToTopOfRuptureList)
	Q_PROPERTY(QString RuptureDistanceList READ getRupDistList WRITE setRupDistList)
	Q_PROPERTY(QString HypocentreLocationList READ getHypocentreLocationList WRITE setHypocentreLocationList)
	Q_PROPERTY(QString EpsilonUncertaintyList READ getEpsilonUncertaintyList WRITE setEpsilonUncertaintyList)
	Q_PROPERTY(QString FaultTypes READ getFaultTypes WRITE setFaultTypes)
	Q_PROPERTY(RResponseType ResponseType READ getResponseType WRITE setResponseType)
	Q_PROPERTY(QString PeriodList READ getPeriodList WRITE setPeriodList)
	Q_PROPERTY(QString StructureLocationList READ getStructureLocationList WRITE setStructureLocationList)
	Q_PROPERTY(QString ShearWaveVelocityList READ getShearWaveVelocityList WRITE setShearWaveVelocityList)

	Q_ENUMS(RFaultType)
	Q_ENUMS(RResponseType)
	

public:	
	RIdriss2013IntensityModel(QObject *parent, QString name);
	~RIdriss2013IntensityModel();

	enum RFaultType{Unspecified, StrikeSlip, NormalSlip, ReverseSlip};
	enum RResponseType{Sa, PGA};
	
	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID = 0);
	
	QString getMagnitudeList();
	void setMagnitudeList(QString value);
	QString getDepthToTopOfRuptureList();
	void setDepthToTopOfRuptureList(QString value);
	QString getHypocentreLocationList();
	void setHypocentreLocationList(QString value);
	QString getRupDistList();
	void setRupDistList(QString Value);
	QString getEpsilonUncertaintyList();
	void setEpsilonUncertaintyList(QString value);
	QString getFaultTypes() const;
	void setFaultTypes(QString value);
	RResponseType getResponseType() const;
	void setResponseType(RResponseType value);
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
	QList<RPointer<RParameter> > theDepthToTopOfRuptureList;
	QList<RPointer<RParameter> > theRupDistList;
	QList<RPointer<RParameter> > theHypocentreLocationList;
	QList<RPointer<RParameter> >  theEpsilonUncertaintyList;
	QList<RFaultType> theFaultTypes;
	RResponseType theResponseType;
	QList<RPointer<RParameter> >  thePeriodList;
	QList<RPointer<RParameter> > theStructureLocationList;
	QList<RPointer<RParameter> > theShearWaveVelocityList;
		
	// Declare responses
	QList<RResponse *> theSpectralResponseList;
	
};	
	
#endif // RIdriss2013IntensityModel_H	