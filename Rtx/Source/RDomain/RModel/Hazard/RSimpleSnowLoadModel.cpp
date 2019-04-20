#include "RSimpleSnowLoadModel.h"

#include "RParameter.h"
#include "RResponse.h"
#include "RContinuousRandomVariable.h"
#include "RTime.h"

#include <gsl/gsl_math.h>

#include <math.h>

RSimpleSnowLoadModel::RSimpleSnowLoadModel(QObject *parent, QString name)
	: RModel(parent, name)
{
	// Initializing the data members
	//$$ theMaximumDepth = 0;
	theStartMonth = 0;
	theEndMonth = 0;
	//$$ theDensity = 0;
	//$$ theTime = 0;
	//$$ theEpsilon = 0;
	
	// Instantiating the response object
	theDomain->createObject(objectName() + "LoadResponse", "RGenericResponse");
	theSnowLoad = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theSnowLoad->setModel(this);
	theDomain->createObject(objectName() + "DepthResponse", "RGenericResponse");
	theSnowDepth = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theSnowDepth->setModel(this);
}

RSimpleSnowLoadModel::~RSimpleSnowLoadModel()
{

}

QObject * RSimpleSnowLoadModel::getMaximumDepth() const	
{	
	return theMaximumDepth.data();
}	
	
void RSimpleSnowLoadModel::setMaximumDepth(QObject *value)	
{	
	theMaximumDepth = qobject_cast<RParameter *>(value);
}	
	
double RSimpleSnowLoadModel::getStartMonth() const	
{	
	return theStartMonth;
}	
	
void RSimpleSnowLoadModel::setStartMonth(double value)	
{	
	theStartMonth = value;
}	
	
double RSimpleSnowLoadModel::getEndMonth() const	
{	
	return theEndMonth;
}	
	
void RSimpleSnowLoadModel::setEndMonth(double value)	
{	
	theEndMonth = value;
}	
	
QObject * RSimpleSnowLoadModel::getDensity() const	
{	
	return theDensity.data();
}	
	
void RSimpleSnowLoadModel::setDensity(QObject *value)	
{	
	theDensity = qobject_cast<RParameter *>(value);
}	
	
QObject * RSimpleSnowLoadModel::getTime() const	
{	
	return theTime.data();
}	
	
void RSimpleSnowLoadModel::setTime(QObject *value)	
{	
	theTime = qobject_cast<RParameter *>(value);
}	
	
QObject * RSimpleSnowLoadModel::getEpsilon() const	
{	
	return theEpsilon.data();
}	
	
void RSimpleSnowLoadModel::setEpsilon(QObject *value)	
{	
	theEpsilon = qobject_cast<RParameter *>(value);
}	
	
int RSimpleSnowLoadModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
{	
	RTime *theTimeObject = qobject_cast<RTime *>(theTime.data());
	if (!theTime) {
		rCritical() << "Error in model" << objectName() << ": A time object is needed for the \"Time\" property.";
		return -1;
	}
	double currentMonth = theTimeObject->getCurrentMonth(threadID);
	double currentDay = theTimeObject->getCurrentDay(threadID);
	currentMonth += (currentDay - 1.0) / 30.0;
	double maximumDepth = theMaximumDepth.data()->getCurrentValue(threadID);
	double epsilon = theEpsilon.data()->getCurrentValue(threadID);
	double density = theDensity.data()->getCurrentValue(threadID);

	double endMonth = theEndMonth;
	double startMonth = theStartMonth;

	if (endMonth <= startMonth) {
		endMonth += 12.0;
	}
	if (currentMonth <= startMonth) {
		currentMonth += 12.0;
	}
	
	//ATTENTION: SHOULD MAKE SURE THAT A ZERO MAGNITUDE (theMaximumDepth) RESULTS IN A ZERO INTENSITY (load/depth). IN THIS CASE, THIS RELATION IS IN PLACE.
	
	double depth = 0;
	if (currentMonth >= startMonth && currentMonth <= endMonth) {
		depth = maximumDepth * epsilon / 2.0 * (1 + sin(2.0 * M_PI * ((currentMonth - startMonth) / (endMonth - startMonth) - 1.0 / 4.0)));
	}
	double load = depth * density;

	theSnowLoad->setCurrentValue(load,threadID);
	theSnowDepth->setCurrentValue(depth,threadID);

	return 0;
}

bool RSimpleSnowLoadModel::canBeRunParallelly(){
	return true;
}