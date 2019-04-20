#ifndef RAtkinsonBoore2003IntensityModel_H	
#define RAtkinsonBoore2003IntensityModel_H	
	
#include "RModel.h"	
	
class RParameter;	
class RResponse;
class RLocation;
	
class RAtkinsonBoore2003IntensityModel : public RModel	
{	
	Q_OBJECT
	
	Q_PROPERTY(QString MagnitudeList READ getMagnitudeList WRITE setMagnitudeList)
	Q_PROPERTY(QString HypocentreLocationList READ getHypocentreLocationList WRITE setHypocentreLocationList)
	Q_PROPERTY(QObject *Epsilon READ getEpsilon WRITE setEpsilon)
	Q_PROPERTY(REventType EventType READ getEventType WRITE setEventType)
	Q_PROPERTY(RResponseType ResponseType READ getResponseType WRITE setResponseType)
	Q_PROPERTY(RSmoothness Smoothness READ getSmoothness WRITE setSmoothness)
	Q_PROPERTY(QString PeriodList READ getPeriodList WRITE setPeriodList)
	Q_PROPERTY(QString StructureLocationList READ getStructureLocationList WRITE setStructureLocationList)
	Q_PROPERTY(QString ShearWaveVelocityList READ getShearWaveVelocityList WRITE setShearWaveVelocityList)

	Q_ENUMS(REventType)
	Q_ENUMS(RResponseType)
	Q_ENUMS(RSmoothness)

public:	
	RAtkinsonBoore2003IntensityModel(QObject *parent, QString name);
	~RAtkinsonBoore2003IntensityModel();

	enum REventType{Interface, InSlab};
	enum RResponseType{Sa, PGA};
	enum RSmoothness{Smoothed, Original};

	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID);
	
	QString getMagnitudeList();
	void setMagnitudeList(QString value);
	QString getHypocentreLocationList();
	void setHypocentreLocationList(QString value);
	QObject *getEpsilon() const;
	void setEpsilon(QObject *value);
	REventType getEventType() const;
	void setEventType(REventType value);
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

	// returns true as this model is parallelized
	bool canBeRunParallelly();

private:	
	void createResponses();

	QList<RPointer<RParameter> > theMagnitudeList;
	QList<RPointer<RParameter> > theHypocentreLocationList;
	RPointer<RParameter> theEpsilon;
	REventType theEventType;
	RResponseType theResponseType;
	RSmoothness theSmoothness;
	QList<RPointer<RParameter> >  thePeriodList;
	QList<RPointer<RParameter> > theStructureLocationList;
	QList<RPointer<RParameter> > theShearWaveVelocityList;
		
	// Declare responses
	QList<RResponse *> theSpectralResponseList;
};	
	
#endif // RAtkinsonBoore2003IntensityModel_H	