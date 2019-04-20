#ifndef RGenericAttenuationModel_H	
#define RGenericAttenuationModel_H	
	
#include "RModel.h"	
	
class RParameter;	
class RResponse;
class RLocation;
	
class RGenericAttenuationModel : public RModel	
{	
	Q_OBJECT

	Q_PROPERTY(QString StructureLocationList READ getStructureLocationList WRITE setStructureLocationList)
	Q_PROPERTY(QString MagnitudeList READ getMagnitudeList WRITE setMagnitudeList)
	Q_PROPERTY(QString HypocentreLocationList READ getHypocentreLocationList WRITE setHypocentreLocationList)
	Q_PROPERTY(QObject *Theta1 READ getTheta1 WRITE setTheta1)
    Q_PROPERTY(QObject *Theta2 READ getTheta2 WRITE setTheta2)
    Q_PROPERTY(QObject *Theta3 READ getTheta3 WRITE setTheta3)
	Q_PROPERTY(QObject *Theta4 READ getTheta4 WRITE setTheta4)
    Q_PROPERTY(QObject *Epsilon READ getEpsilon WRITE setEpsilon)
	
public:	
	RGenericAttenuationModel(QObject *parent, QString name);
	~RGenericAttenuationModel();

	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID);
	
	QString getMagnitudeList();
	void setMagnitudeList(QString value);
	QString getHypocentreLocationList();
	void setHypocentreLocationList(QString value);
	QObject *getTheta1() const;
	void setTheta1(QObject *value);
    QObject *getTheta2() const;
    void setTheta2(QObject *value);
    QObject *getTheta3() const;
    void setTheta3(QObject *value);
	QObject *getTheta4() const;
	void setTheta4(QObject *value);
    QObject *getEpsilon() const;
	void setEpsilon(QObject *value);
	QString getStructureLocationList();
	void setStructureLocationList(QString value);
	
	// returns true as this model is parallelized
	bool canBeRunParallelly();
	
private:	
	void createResponses();

	QList<RPointer<RParameter> > theMagnitudeList;
	QList<RPointer<RParameter> > theHypocentreLocationList;
	RPointer<RParameter> theTheta1;
    RPointer<RParameter> theTheta2;
    RPointer<RParameter> theTheta3;
	RPointer<RParameter> theTheta4;
    RPointer<RParameter> theEpsilon;
	QList<RPointer<RParameter> > theStructureLocationList;
		
	// Declare responses
	QList<RResponse *> thePGAList;
};	
	
#endif
