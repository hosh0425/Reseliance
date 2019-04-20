#include "RSimpleBuildingLifeCycleEnergyUsageModel.h"

#include "RParameter.h"
#include "RResponse.h"

RSimpleBuildingLifeCycleEnergyUsageModel::RSimpleBuildingLifeCycleEnergyUsageModel(QObject *parent, QString &name)
	: RModel(parent, name)
{
	// Initializing the pointer data members
	/* //$$
	theFootprintArea = 0;
	theNStoreys = 0;
	thePUIntensity = 0;
	theParkingIntensity = 0;
	theSurfaceArea = 0;
	theRww = 0;
	theUwindow = 0;
	theUwall = 0;
	theNac = 0;
	theIndoorVolume = 0;
	theDesignLife = 0;
	theHdd = 0;
	theEff = 0;
	theEOLIntensity = 0;
	*/


	// Instantiating the output response
	theDomain->createObject(objectName() + "Response", "RGenericResponse");
	// Setting theResponse pointer
	theEnergyUsageResponse = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	// Setting the "model" property of theResponse to be "this" object
	theEnergyUsageResponse->setModel(this);
}


RSimpleBuildingLifeCycleEnergyUsageModel::~RSimpleBuildingLifeCycleEnergyUsageModel()
{

}

QObject * RSimpleBuildingLifeCycleEnergyUsageModel::getFootprintArea() const	
{	
	return theFootprintArea.data();
}	
	
void RSimpleBuildingLifeCycleEnergyUsageModel::setFootprintArea(QObject *value)	
{	
	theFootprintArea = qobject_cast<RParameter *>(value);
}	
	
QObject * RSimpleBuildingLifeCycleEnergyUsageModel::getNStoreys() const	
{	
	return theNStoreys.data();
}	
	
void RSimpleBuildingLifeCycleEnergyUsageModel::setNStoreys(QObject *value)	
{	
	theNStoreys = qobject_cast<RParameter *>(value);
}	
	
QObject * RSimpleBuildingLifeCycleEnergyUsageModel::getPUIntensity() const	
{	
	return thePUIntensity.data();
}	
	
void RSimpleBuildingLifeCycleEnergyUsageModel::setPUIntensity(QObject *value)	
{	
	thePUIntensity = qobject_cast<RParameter *>(value);
}	
	
QObject * RSimpleBuildingLifeCycleEnergyUsageModel::getParkingIntensity() const	
{	
	return theParkingIntensity.data();
}	
	
void RSimpleBuildingLifeCycleEnergyUsageModel::setParkingIntensity(QObject *value)	
{	
	theParkingIntensity = qobject_cast<RParameter *>(value);
}	
	
QObject * RSimpleBuildingLifeCycleEnergyUsageModel::getSurfaceArea() const	
{	
	return theSurfaceArea.data();
}	
	
void RSimpleBuildingLifeCycleEnergyUsageModel::setSurfaceArea(QObject *value)	
{	
	theSurfaceArea = qobject_cast<RParameter *>(value);
}	
	
QObject * RSimpleBuildingLifeCycleEnergyUsageModel::getRww() const	
{	
	return theRww.data();
}	
	
void RSimpleBuildingLifeCycleEnergyUsageModel::setRww(QObject *value)	
{	
	theRww = qobject_cast<RParameter *>(value);
}	
	
QObject * RSimpleBuildingLifeCycleEnergyUsageModel::getUwindow() const	
{	
	return theUwindow.data();
}	
	
void RSimpleBuildingLifeCycleEnergyUsageModel::setUwindow(QObject *value)	
{	
	theUwindow = qobject_cast<RParameter *>(value);
}	
	
QObject * RSimpleBuildingLifeCycleEnergyUsageModel::getUwall() const	
{	
	return theUwall.data();
}	
	
void RSimpleBuildingLifeCycleEnergyUsageModel::setUwall(QObject *value)	
{	
	theUwall = qobject_cast<RParameter *>(value);
}	
	
QObject * RSimpleBuildingLifeCycleEnergyUsageModel::getNac() const	
{	
	return theNac.data();
}	
	
void RSimpleBuildingLifeCycleEnergyUsageModel::setNac(QObject *value)	
{	
	theNac = qobject_cast<RParameter *>(value);
}	
	
QObject * RSimpleBuildingLifeCycleEnergyUsageModel::getIndoorVolume() const	
{	
	return theIndoorVolume.data();
}	
	
void RSimpleBuildingLifeCycleEnergyUsageModel::setIndoorVolume(QObject *value)	
{	
	theIndoorVolume = qobject_cast<RParameter *>(value);
}	
	
QObject * RSimpleBuildingLifeCycleEnergyUsageModel::getDesignLife() const	
{	
	return theDesignLife.data();
}	
	
void RSimpleBuildingLifeCycleEnergyUsageModel::setDesignLife(QObject *value)	
{	
	theDesignLife = qobject_cast<RParameter *>(value);
}	
	
QObject * RSimpleBuildingLifeCycleEnergyUsageModel::getHdd() const	
{	
	return theHdd.data();
}	
	
void RSimpleBuildingLifeCycleEnergyUsageModel::setHdd(QObject *value)	
{	
	theHdd = qobject_cast<RParameter *>(value);
}	
	
QObject * RSimpleBuildingLifeCycleEnergyUsageModel::getEff() const	
{	
	return theEff.data();
}	
	
void RSimpleBuildingLifeCycleEnergyUsageModel::setEff(QObject *value)	
{	
	theEff = qobject_cast<RParameter *>(value);
}	
	
QObject * RSimpleBuildingLifeCycleEnergyUsageModel::getEOLIntensity() const	
{	
	return theEOLIntensity.data();
}	
	
void RSimpleBuildingLifeCycleEnergyUsageModel::setEOLIntensity(QObject *value)	
{	
	theEOLIntensity = qobject_cast<RParameter *>(value);
}	


int RSimpleBuildingLifeCycleEnergyUsageModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
{	
	// Extract the input parameter values
	double a_f = theFootprintArea.data()->getCurrentValue(threadID);
	double n_s = theNStoreys.data()->getCurrentValue(threadID);
	double i_pu = thePUIntensity.data()->getCurrentValue(threadID);
	double i_p = theParkingIntensity.data()->getCurrentValue(threadID);
	double a = theSurfaceArea.data()->getCurrentValue(threadID);
	double r_ww = theRww.data()->getCurrentValue(threadID);
	double u_win = theUwindow.data()->getCurrentValue(threadID);
	double u_wall = theUwall.data()->getCurrentValue(threadID);
	double n = theNac.data()->getCurrentValue(threadID);
	double v = theIndoorVolume.data()->getCurrentValue(threadID);
	double t_des = theDesignLife.data()->getCurrentValue(threadID);
	double d_hdd = theHdd.data()->getCurrentValue(threadID);
	double eta = theEff.data()->getCurrentValue(threadID);
	double i_eol = theEOLIntensity.data()->getCurrentValue(threadID);



	// Compute energy for difefrent cycle stages
	double e_pu = a_f * (n_s * i_pu + i_p);
	double e_o = t_des * (24.0 * 3600.0 * (a *(r_ww * u_win + (1.0 - r_ww) * u_wall) + 0.33 * n * v) * d_hdd) / eta;
	double e_eol = n_s * a_f * i_eol;

	// Add all life cycle stages
	double response = e_pu + e_o + e_eol;

	// Updating the response current value
	theEnergyUsageResponse->setCurrentValue(response,threadID);

	return 0;
}

bool RSimpleBuildingLifeCycleEnergyUsageModel::canBeRunParallelly(){
	return true;
}