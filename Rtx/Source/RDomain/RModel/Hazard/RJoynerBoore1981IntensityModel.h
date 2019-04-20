#ifndef RJoynerBoore1981IntensityModel_H	
#define RJoynerBoore1981IntensityModel_H	
	
#include "RModel.h"	
	
class RParameter;	
class RResponse;
class RLocation;
	
class RJoynerBoore1981IntensityModel : public RModel	
{	
	Q_OBJECT
	Q_PROPERTY(QObject *Magnitude READ getMagnitude WRITE setMagnitude)
	Q_PROPERTY(QObject *Alpha READ getAlpha WRITE setAlpha)
	Q_PROPERTY(QObject *Beta READ getBeta WRITE setBeta)
	Q_PROPERTY(QObject *B READ getB WRITE setB)
	Q_PROPERTY(QObject *Epsilon READ getEpsilon WRITE setEpsilon)
	Q_PROPERTY(QObject *HypocentreLocation READ getHypocentreLocation WRITE setHypocentreLocation)
	Q_PROPERTY(QString StructureLocationList READ getStructureLocationList WRITE setStructureLocationList)
	
public:	
	RJoynerBoore1981IntensityModel(QObject *parent, QString name);
	~RJoynerBoore1981IntensityModel();

	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID);
	
	QObject *getMagnitude() const;
	void setMagnitude(QObject *value);
	QObject *getAlpha() const;
	void setAlpha(QObject *value);
	QObject *getBeta() const;
	void setBeta(QObject *value);
	QObject *getB() const;
	void setB(QObject *value);
	QObject *getEpsilon() const;
	void setEpsilon(QObject *value);
	QObject *getHypocentreLocation() const;
	void setHypocentreLocation(QObject *value);
	QString getStructureLocationList();
	void setStructureLocationList(QString value);

	// returns true as this model is parallelized
	bool canBeRunParallelly();
	
private:	
	RPointer<RParameter> theMagnitude;
	RPointer<RParameter> theAlpha;
	RPointer<RParameter> theBeta;
	RPointer<RParameter> theB;
	RPointer<RParameter> theEpsilon;
	RPointer<RParameter> theHypocentreLocation;
	QList<RPointer<RParameter> > theStructureLocationList;
		
	// Declare responses
	QList<RResponse *> thePGAList;
};	
	
#endif