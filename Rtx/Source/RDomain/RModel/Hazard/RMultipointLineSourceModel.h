#ifndef RMultipointLineSourceModel_H	
#define RMultipointLineSourceModel_H	
	
#include "RModel.h"	
	
class RParameter;
class RLocation;
class RLocationResponse;	

class QMutex;
	
class RMultipointLineSourceModel : public RModel	
{	
	Q_OBJECT
	Q_PROPERTY(QString PointLocationList READ getPointLocationList WRITE setPointLocationList)
	Q_PROPERTY(QObject *Theta READ getTheta WRITE setTheta)
	Q_PROPERTY(QObject *Depth READ getDepth WRITE setDepth)
	Q_PROPERTY(QObject *LatitudeGeometricalUncertainty READ getLatitudeGeometricalUncertainty WRITE setLatitudeGeometricalUncertainty)
	Q_PROPERTY(QObject *LongitudeGeometricalUncertainty READ getLongitudeGeometricalUncertainty WRITE setLongitudeGeometricalUncertainty)
 
	
public:	
	RMultipointLineSourceModel(QObject *parent, QString name);
	~RMultipointLineSourceModel();

	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID);
	
	QString getPointLocationList();
	void setPointLocationList(QString value);
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
	void reset();

	// returns true as this model is parallelized
	bool canBeRunParallelly();

public slots:
	void displayonMap();
	void removefromMap();

private:	
	QList<RPointer<RParameter> > thePointLocationList;
	RPointer<RParameter> theTheta;
	RPointer<RParameter> theDepth;
	RPointer<RParameter> theLatitudeGeometricalUncertainty;
	RPointer<RParameter> theLongitudeGeometricalUncertainty;

	// These two parameters are only calculated in reset -> they are assumed to be the same for all threads
	double theFaultLength;
	QVector<double> thePieceLengthVector;

	// Declare responses
	RLocationResponse *theEpicentreLocation;

	// a mutex for preventing computing theFaultLength and thePieceLengthVector in more than one thread
	QMutex * faultLengthLock;
};	

#endif	
