#ifndef RInjurySocialLossModel_H	
#define RInjurySocialLossModel_H	
	
#include "RModel.h"	
	
class RParameter;	
class RResponse;	
	
class RInjurySocialLossModel : public RModel	
{	
	Q_OBJECT

	Q_PROPERTY(QObject *CostOfOneInjury READ getCostOfOneInjury WRITE setCostOfOneInjury)
	Q_PROPERTY(QObject *CostOfOneFatality READ getCostOfOneFatality WRITE setCostOfOneFatality)

	Q_PROPERTY(QString DamageRatioList READ getDamageRatioList WRITE setDamageRatioList)
	Q_PROPERTY(QString AffectedAreas READ getAffectedAreas WRITE setAffectedAreas)
	Q_PROPERTY(QString PopulationDensityList READ getPopulationDensityList WRITE setPopulationDensityList)
	Q_PROPERTY(QObject *InjuryRate READ getInjuryRate WRITE setInjuryRate)
	Q_PROPERTY(QObject *FatalityRate READ getFatalityRate WRITE setFatalityRate)
	Q_PROPERTY(QString InjuryDamageRatioThresholds READ getInjuryDamageRatioThresholds WRITE setInjuryDamageRatioThresholds)
	Q_PROPERTY(QString FatalityDamageRatioThresholds READ getFatalityDamageRatioThresholds WRITE setFatalityDamageRatioThresholds)
			
public:	
	RInjurySocialLossModel(QObject *parent, QString name);
	~RInjurySocialLossModel();
	
	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int ThreadID);

	QObject *getCostOfOneInjury() const;
	void setCostOfOneInjury(QObject *value);
	QObject *getCostOfOneFatality() const;
	void setCostOfOneFatality(QObject *value);

	QString getDamageRatioList();
	void setDamageRatioList(QString value);
	QString getAffectedAreas();
	void setAffectedAreas(QString value);
	QString getPopulationDensityList();
	void setPopulationDensityList(QString value);
	QObject *getInjuryRate() const;
	void setInjuryRate(QObject *value);
	QObject *getFatalityRate() const;
	void setFatalityRate(QObject *value);
	QString getInjuryDamageRatioThresholds();
	void setInjuryDamageRatioThresholds(QString value);
	QString getFatalityDamageRatioThresholds();
	void setFatalityDamageRatioThresholds(QString value);
	
	QVector<double> getAffectedAreasVector() const;
	QVector<double> getInjuryDamageRatioThresholdsVector() const;
	QVector<double> getFatalityDamageRatioThresholdsVector() const;

	bool canBeRunParallelly();
			
private:	
	int RiFunction(double lq, double uq, double dr, double *count);

	RPointer<RParameter> theCostOfOneInjury;
	RPointer<RParameter> theCostOfOneFatality;

	QList<RPointer<RParameter> > theDamageRatioList;
	QVector<double> theAffectedAreasVector;
	QList<RPointer<RParameter> > thePopulationDensityList;
	RPointer<RParameter> theInjuryRate;
	RPointer<RParameter> theFatalityRate;
	QVector<double> theInjuryDamageRatioThresholdsVector;
	QVector<double> theFatalityDamageRatioThresholdsVector;

	// Declare responses
	RResponse *theFatalityLoss;
	RResponse *theInjuryLoss;
	RResponse *theNumberOfInjuriesResponse;
	RResponse *theNumberOfFatalitiesResponse;
	
};	
	
#endif // RInjurySocialLossModel_H	
