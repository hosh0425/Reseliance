#ifndef RAbrahamsonEtAl2013IntensityModel_H	
#define RAbrahamsonEtAl2013IntensityModel_H	
	
#include "RModel.h"	
	
class RParameter;	
class RResponse;
class RLocation;
	
class RAbrahamsonEtAl2013IntensityModel : public RModel	
{	
	Q_OBJECT
	
	Q_PROPERTY(QString MagnitudeList READ getMagnitudeList WRITE setMagnitudeList)
	Q_PROPERTY(QString DepthToTopOfRuptureList READ getDepthToTopOfRuptureList WRITE setDepthToTopOfRuptureList)
	Q_PROPERTY(QString DownDipRuptureWidthList READ getDownDipRupWidthList WRITE setDownDipRupWidthList)
	Q_PROPERTY(QString HorizontalDistanceFromTopOfRuptureList READ getHrzDistTopOfRupList WRITE setHrzDistTopOfRupList)
	Q_PROPERTY(QString Ry0HorizontalDistanceOffEndOfRupList READ getHrzDistEndOfRupList WRITE setHrzDistEndOfRupList)
	Q_PROPERTY(QString RuptureDistanceList READ getRupDistList WRITE setRupDistList)
	Q_PROPERTY(QString DipAngles READ getDipAngles WRITE setDipAngles)
	Q_PROPERTY(QString VsFlags READ getVsFlags WRITE setVsFlags)
	Q_PROPERTY(QString HypocentreLocationList READ getHypocentreLocationList WRITE setHypocentreLocationList)
	Q_PROPERTY(QString CentroidRJBList READ getCentroidRJBList WRITE setCentroidRJBList)
	Q_PROPERTY(QString FaultTypes READ getFaultTypes WRITE setFaultTypes)
	Q_PROPERTY(QString WallTypes READ getWallTypes WRITE setWallTypes)
	Q_PROPERTY(QString Regions READ getRegions WRITE setRegions)
	Q_PROPERTY(RShockType ShockType READ getShockType WRITE setShockType)
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
	Q_ENUMS(RShockType)
	Q_ENUMS(RResponseType)

public:	
	RAbrahamsonEtAl2013IntensityModel(QObject *parent, QString name);
	~RAbrahamsonEtAl2013IntensityModel();

	enum RFaultType{StrikeSlip, NormalSlip, ReverseSlip, ReverseObliqueSlip};
	enum RWallType{HangingWall , FootingWall , Unknown};
	enum RRegion{California , Japan , China ,Taiwan , Other};
	enum RShockType{MainShock , AfterShock};
	enum RResponseType{Sa, PGA, PGV};

	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID = 0);
	
	QString getMagnitudeList();
	void setMagnitudeList(QString value);
	QString getDepthToTopOfRuptureList();
	void setDepthToTopOfRuptureList(QString value);
	QString getDownDipRupWidthList();
	void setDownDipRupWidthList(QString Value);
	QString getHrzDistTopOfRupList();
	void setHrzDistTopOfRupList(QString Value);
	QString getHrzDistEndOfRupList();
	void setHrzDistEndOfRupList(QString Value);
	QString getRupDistList();
	void setRupDistList(QString Value);
	QString getDipAngles();
	void setDipAngles(QString Value);
	QString getVsFlags();
	void setVsFlags(QString Value);
	QString getHypocentreLocationList();
	void setHypocentreLocationList(QString value);
	QString getCentroidRJBList();
	void setCentroidRJBList(QString value);
	QString getFaultTypes() const;
	void setFaultTypes(QString value);
	QString getWallTypes() const;
	void setWallTypes(QString value);
	QString getRegions() const;
	void setRegions(QString value);
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
	QString getDepthToVs1List();
	void setDepthToVs1List(QString value);

	QList<RParameter *> getPhysicalParameterList();

	// returns true as this model is parallelized
	bool canBeRunParallelly();

private:	
	void createResponses();

	QList<RPointer<RParameter> > theMagnitudeList;
	QList<RPointer<RParameter> > theDepthToTopOfRuptureList;
	QList<RPointer<RParameter> > theDownDipRupWidthList;
	QList<RPointer<RParameter> > theHrzDistTopOfRupList;
	QList<RPointer<RParameter> > theHrzDistEndOfRupList;
	QList<RPointer<RParameter> > theRupDistList;
	QVector<double> theDipAngleVector;
	QVector<double> theVsFlagVector;
	QList<RPointer<RParameter> > theHypocentreLocationList;
	QList<RPointer<RParameter> > theCentroidRJBList;
	QList<RFaultType> theFaultTypes;
	QList<RWallType> theWallTypes;
	QList<RRegion> theRegions;
	RShockType theShockType;
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
	
#endif // RAbrahamsonEtAl2013IntensityModel_H	