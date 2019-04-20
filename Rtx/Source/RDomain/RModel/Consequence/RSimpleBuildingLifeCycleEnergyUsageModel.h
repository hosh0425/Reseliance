#ifndef RSimpleBuildingLifeCycleEnergyUsageModel_H
#define RSimpleBuildingLifeCycleEnergyUsageModel_H

#include "RModel.h"

class RParameter;
class RResponse;

class RSimpleBuildingLifeCycleEnergyUsageModel : public RModel
{
	Q_OBJECT

	// Define a QProperty for each input (parameter) data member. The type of the QProperty is "QObject *"
	Q_PROPERTY(QObject *FootprintArea READ getFootprintArea WRITE setFootprintArea)
	Q_PROPERTY(QObject *NStoreys READ getNStoreys WRITE setNStoreys)
	Q_PROPERTY(QObject *PUIntensity READ getPUIntensity WRITE setPUIntensity)
	Q_PROPERTY(QObject *ParkingIntensity READ getParkingIntensity WRITE setParkingIntensity)
	Q_PROPERTY(QObject *SurfaceArea READ getSurfaceArea WRITE setSurfaceArea)
	Q_PROPERTY(QObject *Rww READ getRww WRITE setRww)
	Q_PROPERTY(QObject *Uwindow READ getUwindow WRITE setUwindow)
	Q_PROPERTY(QObject *Uwall READ getUwall WRITE setUwall)
	Q_PROPERTY(QObject *Nac READ getNac WRITE setNac)
	Q_PROPERTY(QObject *IndoorVolume READ getIndoorVolume WRITE setIndoorVolume)
	Q_PROPERTY(QObject *DesignLife READ getDesignLife WRITE setDesignLife)
	Q_PROPERTY(QObject *Hdd READ getHdd WRITE setHdd)
	Q_PROPERTY(QObject *Eff READ getEff WRITE setEff)
	Q_PROPERTY(QObject *EOLIntensity READ getEOLIntensity WRITE setEOLIntensity)


public:
	// Define the constructor and destructor
	RSimpleBuildingLifeCycleEnergyUsageModel(QObject *parent, QString &name);
	~RSimpleBuildingLifeCycleEnergyUsageModel();

	// Define the mandatory read/write methods for all QProperties
	QObject *getFootprintArea() const;
	void setFootprintArea(QObject *value);

	QObject *getNStoreys() const;
	void setNStoreys(QObject *value);

	QObject *getPUIntensity() const;
	void setPUIntensity(QObject *value);

	QObject *getParkingIntensity() const;
	void setParkingIntensity(QObject *value);

	QObject *getSurfaceArea() const;
	void setSurfaceArea(QObject *value);

	QObject *getRww() const;
	void setRww(QObject *value);

	QObject *getUwindow() const;
	void setUwindow(QObject *value);

	QObject *getUwall() const;
	void setUwall(QObject *value);

	QObject *getNac() const;
	void setNac(QObject *value);

	QObject *getIndoorVolume() const;
	void setIndoorVolume(QObject *value);

	QObject *getDesignLife() const;
	void setDesignLife(QObject *value);

	QObject *getHdd() const;
	void setHdd(QObject *value);

	QObject *getEff() const;
	void setEff(QObject *value);

	QObject *getEOLIntensity() const;
	void setEOLIntensity(QObject *value);


	// Define the method where the actual model is coded
	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID);

	// returns true as this model is parallelized
	bool canBeRunParallelly();

private:
	// Declare input data members
	RPointer<RParameter> theFootprintArea;
	RPointer<RParameter> theNStoreys;
	RPointer<RParameter> thePUIntensity;
	RPointer<RParameter> theParkingIntensity;
	RPointer<RParameter> theSurfaceArea;
	RPointer<RParameter> theRww;
	RPointer<RParameter> theUwindow;
	RPointer<RParameter> theUwall;
	RPointer<RParameter> theNac;
	RPointer<RParameter> theIndoorVolume;
	RPointer<RParameter> theDesignLife;
	RPointer<RParameter> theHdd;
	RPointer<RParameter> theEff;
	RPointer<RParameter> theEOLIntensity;


	// Declare responses
	RResponse *theEnergyUsageResponse;

};

#endif // RSimpleBuildingLifeCycleEnergyUsageModel_H
