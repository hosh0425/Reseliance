#ifndef RPolygonalAreaSourceModel_H	
#define RPolygonalAreaSourceModel_H	
	
#include "RModel.h"	
	
class RParameter;	
class RLocationResponse;
class RLocation;
class RContinuousRandomVariable;
class QMutex;
	
class RPolygonalAreaSourceModel : public RModel	
{	
	Q_OBJECT
	Q_PROPERTY(QString BoundaryLocationList READ getBoundaryLocationList WRITE setBoundaryLocationList)
	Q_PROPERTY(QObject *Theta1 READ getTheta1 WRITE setTheta1)
	Q_PROPERTY(QObject *Theta2 READ getTheta2 WRITE setTheta2)
	Q_PROPERTY(QObject *Depth READ getDepth WRITE setDepth)

public:	
	RPolygonalAreaSourceModel(QObject *parent, QString name);
	~RPolygonalAreaSourceModel();

	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID);
	
	QString getBoundaryLocationList();
	void setBoundaryLocationList(QString value);
	QObject *getTheta1() const;
	void setTheta1(QObject *value);
	QObject *getTheta2() const;
	void setTheta2(QObject *value);
	QObject *getDepth() const;
	void setDepth(QObject *value);

	QList<RParameter *> getPhysicalParameterList();
	void reset();

	QStringList getActionList();

	// returns true as this model is parallelized
	bool canBeRunParallelly();

public slots:
	void displayonMap();
	void removefromMap();
	
private:	

	QList<RPointer<RParameter> > theBoundaryLocationList;
	RPointer<RParameter> theTheta1;
	RPointer<RParameter> theTheta2;
	RPointer<RParameter> theDepth;

	// Declare responses
	RLocationResponse *theHypocentreLocation;

	// Internal data members (It is assumed that no value is set in this data member during analysis except for the first time)
	RContinuousRandomVariable *theLatitudeRandomVariable;

	// a mutex lock for preventing creating theLatitudeRandomVariablein more than one thread
	QMutex* latitudeRandomVariableLock;

};	

#endif	
