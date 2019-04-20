#ifndef RAtkinson1997IntensityModel_H	
#define RAtkinson1997IntensityModel_H	
	
#include "RModel.h"	
	
class RParameter;	
class RResponse;
class RLocation;
	
class RAtkinson1997IntensityModel : public RModel	
{	
	Q_OBJECT
	Q_PROPERTY(QString PeriodParameterList READ getPeriodParameterList WRITE setPeriodParameterList)
	Q_PROPERTY(QObject *Magnitude READ getMagnitude WRITE setMagnitude)
	Q_PROPERTY(QObject *HypocentreLocation READ getHypocentreLocation WRITE setHypocentreLocation)
	Q_PROPERTY(RSoilType SoilType READ getSoilType WRITE setSoilType)
	Q_PROPERTY(QObject *Epsilon READ getEpsilon WRITE setEpsilon)
	Q_PROPERTY(QString StructureLocationList READ getStructureLocationList WRITE setStructureLocationList)

	// Model Error Parameters
	Q_PROPERTY(QObject *C0Uncertainty READ getC0Uncertainty WRITE setC0Uncertainty)
	Q_PROPERTY(QObject *C1Uncertainty READ getC1Uncertainty WRITE setC1Uncertainty)
	Q_PROPERTY(QObject *C2Uncertainty READ getC2Uncertainty WRITE setC2Uncertainty)
	Q_PROPERTY(QObject *C3Uncertainty READ getC3Uncertainty WRITE setC3Uncertainty)
	Q_PROPERTY(QObject *Ca3Uncertainty READ getCa3Uncertainty WRITE setCa3Uncertainty)
	Q_PROPERTY(QObject *Ca4Uncertainty READ getCa4Uncertainty WRITE setCa4Uncertainty)
	Q_PROPERTY(QObject *CsigmaUncertainty READ getCsigmaUncertainty WRITE setCsigmaUncertainty)
	Q_PROPERTY(QObject *CsUncertainty READ getCsUncertainty WRITE setCsUncertainty)

	Q_ENUMS(RSoilType)

public:	
	RAtkinson1997IntensityModel(QObject *parent, QString name);
	~RAtkinson1997IntensityModel();

	enum RSoilType{Rock, FirmSoil};

	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID);
	
	QString getPeriodParameterList();
	void setPeriodParameterList(QString value);
	QObject *getHypocentreLocation() const;
	void setHypocentreLocation(QObject *value);
	QString getStructureLocationList();
	void setStructureLocationList(QString value);
	QObject *getMagnitude() const;
	void setMagnitude(QObject *value);
	RSoilType getSoilType() const;
	void setSoilType(RSoilType value);
	QObject *getEpsilon() const;
	void setEpsilon(QObject *value);


	// Model Error Parameters
	QObject *getC0Uncertainty() const;
	void setC0Uncertainty(QObject *value);
	QObject *getC1Uncertainty() const;
	void setC1Uncertainty(QObject *value);
	QObject *getC2Uncertainty() const;
	void setC2Uncertainty(QObject *value);
	QObject *getC3Uncertainty() const;
	void setC3Uncertainty(QObject *value);
	QObject *getCa3Uncertainty() const;
	void setCa3Uncertainty(QObject *value);
	QObject *getCa4Uncertainty() const;
	void setCa4Uncertainty(QObject *value);
	QObject *getCsigmaUncertainty() const;
	void setCsigmaUncertainty(QObject *value);
	QObject *getCsUncertainty() const;
	void setCsUncertainty(QObject *value);

	// returns true as this model is parallelized
	bool canBeRunParallelly();
	
private:	
	QList<RPointer<RParameter> > thePeriodParameterList;
	RPointer<RParameter> theHypocentreLocation;
	QList<RPointer<RParameter> > theStructureLocationList;
	RPointer<RParameter> theMagnitude;
	RSoilType theSoilType;
	RPointer<RParameter> theEpsilon;

	// Model Error Parameters
	RPointer<RParameter> theC0Uncertainty;
	RPointer<RParameter> theC1Uncertainty;
	RPointer<RParameter> theC2Uncertainty;
	RPointer<RParameter> theC3Uncertainty;
	RPointer<RParameter> theCa3Uncertainty;
	RPointer<RParameter> theCa4Uncertainty;
	RPointer<RParameter> theCsigmaUncertainty;
	RPointer<RParameter> theCsUncertainty;

		
	// Declare responses
	QList<RResponse *> thePSAResponseList;
	
};	
	
#endif // RAtkinson1997IntensityModel_H	