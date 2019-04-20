#include "RInjurySocialLossModel.h"	
	
#include "RParameter.h"	
#include "RResponse.h"	

#include <gsl/gsl_vector.h>
#include <gsl/gsl_math.h>
	
#include <math.h>	
	
RInjurySocialLossModel::RInjurySocialLossModel(QObject *parent, QString name)	
	: RModel(parent, name)
{	
	// Initializing the data members
	
	//theCostOfOneInjury = 0;
	//theCostOfOneFatality = 0;
		
	// Instantiating the response object
	theDomain->createObject(objectName() + "FatalityLossResponse", "RGenericResponse");
	theFatalityLoss = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theFatalityLoss->setModel(this);

	theDomain->createObject(objectName() + "InjuryLossResponse", "RGenericResponse");
	theInjuryLoss = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theInjuryLoss->setModel(this);

	theDomain->createObject(objectName() + "NumberOfInjuriesResponse", "RGenericResponse");
	theNumberOfInjuriesResponse = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theNumberOfInjuriesResponse->setModel(this);

	theDomain->createObject(objectName() + "NumberOfFatalitiesResponse", "RGenericResponse");
	theNumberOfFatalitiesResponse = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theNumberOfFatalitiesResponse->setModel(this);
}	

	RInjurySocialLossModel::~RInjurySocialLossModel()	
	{	
		
	}	

	QObject * RInjurySocialLossModel::getCostOfOneInjury() const	
	{	
		return theCostOfOneInjury.data();
	}	
		
	void RInjurySocialLossModel::setCostOfOneInjury(QObject *value)	
	{	
		theCostOfOneInjury = qobject_cast<RParameter *>(value);
	}	

	QObject * RInjurySocialLossModel::getCostOfOneFatality() const	
	{	
		return theCostOfOneFatality.data();
	}	
		
	void RInjurySocialLossModel::setCostOfOneFatality(QObject *value)	
	{	
		theCostOfOneFatality = qobject_cast<RParameter *>(value);
	}	
		
	QString RInjurySocialLossModel::getDamageRatioList()
	{	
	 return parameterListToString(theDamageRatioList);
	}	
		
	void RInjurySocialLossModel::setDamageRatioList(QString value)	
	{	
		theDamageRatioList = stringToParameterList(value);
	}

	QString RInjurySocialLossModel::getAffectedAreas()	
	{	
	QString result = doubleVectorToString(theAffectedAreasVector);
	return result;
	}	
	
	void RInjurySocialLossModel::setAffectedAreas(QString value)	
	{	
	theAffectedAreasVector = stringToDoubleVector(value);

	}	
		
	QString RInjurySocialLossModel::getPopulationDensityList()
	{	
	 return parameterListToString(thePopulationDensityList);
	}	
		
	void RInjurySocialLossModel::setPopulationDensityList(QString value)	
	{	
		thePopulationDensityList = stringToParameterList(value);
	}

	QObject * RInjurySocialLossModel::getInjuryRate() const	
	{	
		return theInjuryRate.data();
	}	
		
	void RInjurySocialLossModel::setInjuryRate(QObject *value)	
	{	
		theInjuryRate = qobject_cast<RParameter *>(value);
	}	

	QObject * RInjurySocialLossModel::getFatalityRate() const	
	{	
		return theFatalityRate.data();
	}	
		
	void RInjurySocialLossModel::setFatalityRate(QObject *value)	
	{	
		theFatalityRate = qobject_cast<RParameter *>(value);
	}	

	QString RInjurySocialLossModel::getFatalityDamageRatioThresholds()	
	{	
	QString result = doubleVectorToString(theFatalityDamageRatioThresholdsVector);
	return result;
	}	
	
	void RInjurySocialLossModel::setFatalityDamageRatioThresholds(QString value)	
	{	
	theFatalityDamageRatioThresholdsVector = stringToDoubleVector(value);
	}	

	QString RInjurySocialLossModel::getInjuryDamageRatioThresholds()	
	{	
	QString result = doubleVectorToString(theInjuryDamageRatioThresholdsVector);
	return result;
	}	
	
	void RInjurySocialLossModel::setInjuryDamageRatioThresholds(QString value)	
	{	
	theInjuryDamageRatioThresholdsVector = stringToDoubleVector(value);

	}	

	
			
	int RInjurySocialLossModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int ThreadID)
	{	
	double PI = M_PI;
	double injRate = theInjuryRate.data()->getCurrentValue();
	double fatRate = theFatalityRate.data()->getCurrentValue();
	double costOfOneInjury = theCostOfOneInjury.data()->getCurrentValue();
	double costOfOneFatality = theCostOfOneFatality.data()->getCurrentValue();
	

	int drCount = theDamageRatioList.count();
	gsl_vector *drVector = gsl_vector_calloc(drCount);
	for (int i = 0; i < drCount; i++) {
	gsl_vector_set(drVector, i, theDamageRatioList[i].data()->getCurrentValue());
	}
	if (drCount!=theAffectedAreasVector.count()) {
		qCritical() << "Error: The Size Of Two Lists Of Damage Ratio And Affected Area Are Not The Same";
		return -1;
	}
	gsl_vector *areaVector = gsl_vector_calloc(drCount);
	for (int i = 0; i < drCount; i++) {
	gsl_vector_set(areaVector, i, theAffectedAreasVector[i]);
	}
	if (drCount!=thePopulationDensityList.count()) {
		qCritical() << "Error: The Size Of Two Lists Of Population And Affected Area Are Not The Same";
		return -1;
	}
	gsl_vector *popDensVector = gsl_vector_calloc(drCount);
	for (int i = 0; i < drCount; i++) {
	gsl_vector_set(popDensVector, i, thePopulationDensityList[i].data()->getCurrentValue());
	}

	int boundryInjCount = theInjuryDamageRatioThresholdsVector.count();
	gsl_vector *boundryInjVector = gsl_vector_calloc(boundryInjCount);
	for (int i = 0; i < boundryInjCount; i++) {
	gsl_vector_set(boundryInjVector, i, theInjuryDamageRatioThresholdsVector[i]);
	}

	int boundryFatCount = theFatalityDamageRatioThresholdsVector.count();
	gsl_vector *boundryFatVector = gsl_vector_calloc(boundryFatCount);
	for (int i = 0; i < boundryFatCount; i++) {
	gsl_vector_set(boundryFatVector, i, theFatalityDamageRatioThresholdsVector[i]);
	}

	
	//calculating number of Injuries caused by damage to the member
	double count=0.0;
	double numberOfInjuries = 0.0;
	double numberOfFatalities = 0.0;
	for (int i = 0; i < drCount; i++) {
		double dr=gsl_vector_get(drVector , i);
		RiFunction(gsl_vector_get(boundryInjVector,0), gsl_vector_get(boundryInjVector,1), dr, &count);
		numberOfInjuries = numberOfInjuries + count * injRate* gsl_vector_get(areaVector,i) 
			* gsl_vector_get(popDensVector,i);
	}
	if (fatRate!=0) {
	for (int i = 0; i < drCount; i++) {
		double dr=gsl_vector_get(drVector , i);
		RiFunction(gsl_vector_get(boundryFatVector,0), gsl_vector_get(boundryFatVector,1), dr, &count);
		numberOfFatalities = numberOfFatalities + count * fatRate* gsl_vector_get(areaVector,i) 
			* gsl_vector_get(popDensVector,i);
	}
	}
	
	//Calculating Loss due to injury and also fatality separately
	double injuryLoss;
	double fatalityLoss;
	injuryLoss = costOfOneInjury * numberOfInjuries;
	fatalityLoss = costOfOneFatality * numberOfFatalities;

	//Making memory free
	gsl_vector_free(drVector);
	gsl_vector_free(areaVector);
	gsl_vector_free(popDensVector);
	gsl_vector_free(boundryInjVector);
	gsl_vector_free(boundryFatVector);


	theInjuryLoss->setCurrentValue(injuryLoss);
	theFatalityLoss->setCurrentValue(fatalityLoss);
	theNumberOfInjuriesResponse->setCurrentValue(numberOfInjuries);
	theNumberOfFatalitiesResponse->setCurrentValue(numberOfFatalities);

	return 0;
	}
	 int RInjurySocialLossModel::RiFunction(double lq, double uq, double dr, double *count)
	 {
		 double PI = M_PI;
		 double a = 0.5; double d = 0.5;
		 double b = PI/(uq-lq);
		 double c = (3*PI)/2 -lq * b;

		if (dr <= lq) {
			*count = 0;
		}
		else if ((dr > lq)&&(dr < uq)) {
			*count = a * sin(b*dr+c) + d;
		}
		else {
			*count = 1;
		}
		 return -1;
	 }

	 bool RInjurySocialLossModel::canBeRunParallelly() {
		 return false;
	 }