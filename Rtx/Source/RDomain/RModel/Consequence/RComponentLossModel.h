#ifndef RComponentLossModel_H	
#define RComponentLossModel_H	
	
#include "RModel.h"	
	
class RParameter;	
class RResponse;	
	
class RComponentLossModel : public RModel	
{	
	Q_OBJECT
	
	Q_PROPERTY(RAreaDenepdent AreaDenepdent READ getAreaDenepdent WRITE setAreaDenepdent)
	Q_PROPERTY(RBulkDiscount BulkDiscount READ getBulkDiscount WRITE setBulkDiscount)
	Q_PROPERTY(double AreaOfOneQuantity READ getAreaOfOneQuantity WRITE setAreaOfOneQuantity)
	Q_PROPERTY(QString DamageRatioList READ getDamageRatioList WRITE setDamageRatioList)
	Q_PROPERTY(QString Areas READ getAreas WRITE setAreas)
	Q_PROPERTY(QObject *ReplacementCostPerUnit READ getReplacementCostPerUnit WRITE setReplacementCostPerUnit)
	Q_PROPERTY(QString ThresholdDamageRatios READ getThresholdDamageRatios WRITE setThresholdDamageRatios)
	Q_PROPERTY(QString LowerQuantity READ getLowerQuantity WRITE setLowerQuantity)
	Q_PROPERTY(QString UpperQuantity READ getUpperQuantity WRITE setUpperQuantity)
	Q_PROPERTY(QString DiscountRates READ getDiscountRates WRITE setDiscountRates)
	Q_PROPERTY(QObject *RepairStatus READ getRepairStatus WRITE setRepairStatus)


	Q_ENUMS(RAreaDenepdent)
	Q_ENUMS(RBulkDiscount)
	
public:	
	RComponentLossModel(QObject *parent, QString name);
	~RComponentLossModel();

	enum RAreaDenepdent{No, Yes};
	enum RBulkDiscount{False, True};
	
	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int ThreadID);
	
	RAreaDenepdent getAreaDenepdent() const;
	void setAreaDenepdent(RAreaDenepdent value);
	RBulkDiscount getBulkDiscount() const;
	void setBulkDiscount(RBulkDiscount value);
	double getAreaOfOneQuantity() const;
	void setAreaOfOneQuantity(double value);
	QString getDamageRatioList();
	void setDamageRatioList(QString value);
	QString getAreas();
	void setAreas(QString value);
	QObject *getReplacementCostPerUnit() const;
	void setReplacementCostPerUnit(QObject *value);
	QString getThresholdDamageRatios();
	void setThresholdDamageRatios(QString value);
	QString getLowerQuantity();
	void setLowerQuantity(QString value);
	QString getUpperQuantity();
	void setUpperQuantity(QString value);
	QString getDiscountRates();
	void setDiscountRates(QString value);

	QVector<double> getAreasVector() const;
	QVector<double> getThresholdDamageRatiosVector() const;
	QVector<double> getLowerQuantityVector() const;
	QVector<double> getUpperQuantityVector() const;
	QVector<double> getDiscountRatesVector() const;

	QObject *getRepairStatus() const;
	void setRepairStatus(QObject *value);
	bool canBeRunParallelly();

private:	
	int RiFunction(double lq, double uq, double dr, double *count);
	int RCRFunction(double lq, double uq, double rl, double ru, double n, double *coeff);

	RAreaDenepdent theAreaDenepdent;
	RBulkDiscount theBulkDiscount;
	double theAreaOfOneQuantity;
	QList<RPointer<RParameter> > theDamageRatioList;
	QVector<double> theAreasVector;
	RPointer<RParameter> theReplacementCostPerUnit;
	QVector<double> theThresholdDamageRatiosVector;
	QVector<double> theLowerQuantityVector;
	QVector<double> theUpperQuantityVector;
	QVector<double> theDiscountRatesVector;
	
	RPointer<RParameter> theRepairStatus;

	// Declare responses
	RResponse *theRBuildingLoss;
};	
	
#endif // RComponentLossModel_H	
