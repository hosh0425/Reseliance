#ifndef RSingleLineSourceModel_H	
#define RSingleLineSourceModel_H	
	
#include "RModel.h"	
	
class RParameter;
class RLocation;
class RLocationResponse;	
	
class RSingleLineSourceModel : public RModel	
{	
	Q_OBJECT

	Q_PROPERTY(QObject *Point1Location READ getPoint1Location WRITE setPoint1Location)
	Q_PROPERTY(QObject *Point2Location READ getPoint2Location WRITE setPoint2Location)
	Q_PROPERTY(QObject *Theta READ getTheta WRITE setTheta)
	Q_PROPERTY(QObject *Depth READ getDepth WRITE setDepth)
	Q_PROPERTY(QObject *LatitudeGeometricalUncertainty READ getLatitudeGeometricalUncertainty WRITE setLatitudeGeometricalUncertainty)
	Q_PROPERTY(QObject *LongitudeGeometricalUncertainty READ getLongitudeGeometricalUncertainty WRITE setLongitudeGeometricalUncertainty)
	
public:	
	RSingleLineSourceModel(QObject *parent, QString name);
	~RSingleLineSourceModel();

	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID);

	QObject *getPoint1Location() const;
	void setPoint1Location(QObject *value);
	QObject *getPoint2Location() const;
	void setPoint2Location(QObject *value);
	QObject *getTheta() const;
	void setTheta(QObject *value);
	QObject *getDepth() const;
	void setDepth(QObject *value);
	QObject *getLatitudeGeometricalUncertainty() const;
	void setLatitudeGeometricalUncertainty(QObject *value);
	QObject *getLongitudeGeometricalUncertainty() const;
	void setLongitudeGeometricalUncertainty(QObject *value);

	QList<RParameter *> getPhysicalParameterList();

	QStringList getActionList();

	// returns true as this model is parallelized
	bool canBeRunParallelly();

public slots:
	void displayonMap();
	void removefromMap();

private:	
	RPointer<RParameter> thePoint1Location;
	RPointer<RParameter> thePoint2Location;
	RPointer<RParameter> theTheta;
	RPointer<RParameter> theDepth;
	RPointer<RParameter> theLatitudeGeometricalUncertainty;
	RPointer<RParameter> theLongitudeGeometricalUncertainty;

	// Declare responses
	RLocationResponse *theHypocentreLocation;
};	

#endif	
