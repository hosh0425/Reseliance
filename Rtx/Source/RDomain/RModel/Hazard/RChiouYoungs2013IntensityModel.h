#ifndef RChiouYoungs2013IntensityModel_H	
#define RChiouYoungs2013IntensityModel_H	
	
#include "RModel.h"	
	
class RParameter;	
class RResponse;
class RLocation;
	
class RChiouYoungs2013IntensityModel : public RModel	
{	
	Q_OBJECT
	
	Q_PROPERTY(QString MagnitudeList READ getMagnitudeList WRITE setMagnitudeList)
	Q_PROPERTY(QString DepthToTopOfRuptureList READ getDepthToTopOfRuptureList WRITE setDepthToTopOfRuptureList)
	Q_PROPERTY(QString HorizontalDistanceFromTopOfRuptureList READ getHorizontalDistanceFromTopOfRuptureList WRITE setHorizontalDistanceFromTopOfRuptureList)
	Q_PROPERTY(QString RuptureDistanceList READ getRuptureDistanceList WRITE setRuptureDistanceList)
	Q_PROPERTY(QString DipAngles READ getDipAngles WRITE setDipAngles)
	Q_PROPERTY(QString VsFlags READ getVsFlags WRITE setVsFlags)
	Q_PROPERTY(QString HypocentreLocationList READ getHypocentreLocationList WRITE setHypocentreLocationList)
	Q_PROPERTY(QString FaultTypes READ getFaultTypes WRITE setFaultTypes)
	Q_PROPERTY(QString WallTypes READ getWallTypes WRITE setWallTypes)
	Q_PROPERTY(QString Regions READ getRegions WRITE setRegions)
	Q_PROPERTY(RResponseType ResponseType READ getResponseType WRITE setResponseType)
	Q_PROPERTY(QString PeriodList READ getPeriodList WRITE setPeriodList)
	Q_PROPERTY(QString EpsilonUncertaintyList READ getEpsilonUncertaintyList WRITE setEpsilonUncertaintyList)
	Q_PROPERTY(QString EtaUncertaintyList READ getEtaUncertaintyList WRITE setEtaUncertaintyList)
	Q_PROPERTY(QString StructureLocationList READ getStructureLocationList WRITE setStructureLocationList)
	Q_PROPERTY(QString ShearWaveVelocityList READ getShearWaveVelocityList WRITE setShearWaveVelocityList)
	Q_PROPERTY(QString DepthToVs1List READ getDepthToVs1List WRITE setDepthToVs1List)

	Q_ENUMS(RFaultType)
	Q_ENUMS(RWallType)
	Q_ENUMS(RRegion)
	Q_ENUMS(RResponseType)

public:	
	RChiouYoungs2013IntensityModel(QObject *parent, QString name);
	~RChiouYoungs2013IntensityModel();

	enum RFaultType{StrikeSlip, NormalSlip, ReverseSlip, ReverseObliqueSlip};
	enum RWallType{HangingWall , FootingWall , Unknown};
	enum RRegion{California , Japan , China ,Taiwan , Italy , Other};
	enum RResponseType{Sa, PGA, PGV};

	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID = 0);
	
	QString getMagnitudeList();
	void setMagnitudeList(QString value);
	QString getDepthToTopOfRuptureList();
	void setDepthToTopOfRuptureList(QString value);
	QString getHorizontalDistanceFromTopOfRuptureList();
	void setHorizontalDistanceFromTopOfRuptureList(QString Value);
	QString getRuptureDistanceList();
	void setRuptureDistanceList(QString Value);
	QString getDipAngles();
	void setDipAngles(QString Value);
	QString getVsFlags();
	void setVsFlags(QString Value);
	QString getHypocentreLocationList();
	void setHypocentreLocationList(QString value);
	QString getFaultTypes() const;
	void setFaultTypes(QString value);
	QString getWallTypes() const;
	void setWallTypes(QString value);
	QString getRegions() const;
	void setRegions(QString value);
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
	QString getDepthToVs1List();
	void setDepthToVs1List(QString value);

	QList<RParameter *> getPhysicalParameterList();

	// returns true as this model is parallelized
	bool canBeRunParallelly();

private:	
	void createResponses();

	QList<RPointer<RParameter> > theMagnitudeList;
	QList<RPointer<RParameter> > theDepthToTopOfRuptureList;
	QList<RPointer<RParameter> > theHorizontalDistanceFromTopOfRuptureList;
	QList<RPointer<RParameter> > theRuptureDistanceList;
	QVector<double> theDipAngleVector;
	QVector<double> theVsFlagVector;
	QList<RPointer<RParameter> > theHypocentreLocationList;
	QList<RFaultType> theFaultTypes;
	QList<RWallType> theWallTypes;
	QList<RRegion> theRegions;
	RResponseType theResponseType;
	QList<RPointer<RParameter> >  thePeriodList;
	QList<RPointer<RParameter> >  theEtaUncertaintyList;
	QList<RPointer<RParameter> >  theEpsilonUncertaintyList;
	QList<RPointer<RParameter> > theStructureLocationList;
	QList<RPointer<RParameter> > theShearWaveVelocityList;
	QList<RPointer<RParameter> > theDepthToVs1List;
		
	// Declare responses
	QList<RResponse *> theSpectralResponseList;
	
};	
	
#endif // RChiouYoungs2013IntensityModel_H	