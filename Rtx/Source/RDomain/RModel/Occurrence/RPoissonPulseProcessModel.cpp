#include "RPoissonPulseProcessModel.h"

#include <QCoreApplication>

#include "RParameter.h"
#include "RResponse.h"

#include "RContinuousRandomVariable.h"

RPoissonPulseProcessModel::RPoissonPulseProcessModel(QObject *parent, QString name)
	: ROccurrenceModel(parent, name)
{
	//$$ theNonzeroOccurrenceRateParameter = 0;
	theNonzeroOccurrenceRateDouble = 0;
	theMeanDuration = 0;
	theTimeInstantGenerationScheme = RPoissonPulseProcessModel::Exact;
}

RPoissonPulseProcessModel::~RPoissonPulseProcessModel()
{

}
	
double RPoissonPulseProcessModel::getMeanDuration() const	
{	
	return theMeanDuration;
}	
	
void RPoissonPulseProcessModel::setMeanDuration(double value)	
{	
	theMeanDuration = value;
}	
	
RPoissonPulseProcessModel::RTimeInstantGenerationScheme RPoissonPulseProcessModel::getTimeInstantGenerationScheme() const	
{	
	return theTimeInstantGenerationScheme;
}	

void RPoissonPulseProcessModel::setTimeInstantGenerationScheme(RPoissonPulseProcessModel::RTimeInstantGenerationScheme value)	
{	
	theTimeInstantGenerationScheme = value;
}	

int RPoissonPulseProcessModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
{	
	return 0;
}

int RPoissonPulseProcessModel::establishTimeAxis(double T, gsl_rng *randomGenerator, int threadID)
{	 

	theStartTimeVector[threadID].clear();
	theEndTimeVector[threadID].clear();

	double lambda;

	if (theNonzeroOccurrenceRateParameter) {
		RContinuousRandomVariable *rv = qobject_cast<RContinuousRandomVariable *>(theNonzeroOccurrenceRateParameter.data());
		if (rv) {
			double lambdaProb = gsl_ran_flat(randomGenerator, 0, 1);
			lambda = rv->getInverseCDFValue(lambdaProb, threadID);
			rv->setCurrentValue(lambda, threadID);
		} else {
			lambda = theNonzeroOccurrenceRateParameter.data()->getCurrentValue(threadID);
		}
	} else {
		lambda = theNonzeroOccurrenceRateDouble;
	}

	double mud = theMeanDuration;
	double nu = 1 / mud;
	double p0 = lambda * mud;

	if (p0 > 1.0) {
		rCritical(threadID) << "Warning: The current realization of p0, i.e., the product of the Nonzero Occurrence Rate and Mean Duration, in the occurrence model" << this->objectName() << "is greater than 1.0.";
	}
 
	double t = 0;
	double on;
	long count = 0;

	if (theTimeInstantGenerationScheme == RPoissonPulseProcessModel::Exact) {
		while (t <= T) {
			on = gsl_ran_binomial(randomGenerator, p0, 1.0);
			double dt = gsl_ran_exponential(randomGenerator, mud);
			if (on == 1.0) {
				theStartTimeVector[threadID] << t;
				theEndTimeVector[threadID] << t + dt;
			}
			t += dt;

			count++;
			if ((threadID == 0) && (count % 100000 == 0)) {
				QCoreApplication::processEvents();
			}
		}
	} else if (theTimeInstantGenerationScheme == RPoissonPulseProcessModel::Approximate) {
		// Faster alternative implementation
		while (t <= T) {
			//double temp = gsl_ran_flat(randomGenerator, 0, 1);
			//rDebug(threadID) << temp;
			on = 0.0;
			double dt = gsl_ran_exponential(randomGenerator, 1.0 / lambda);
			t += dt;
			if (t > T) {
				break;
			}

			on == 1.0;
			dt = gsl_ran_exponential(randomGenerator, mud);
			theStartTimeVector[threadID] << t;
			theEndTimeVector[threadID] << t + dt;

			if (t + dt > T) {
				break;
			}
			//t += dt;
		}
	}

	
	if (on == 1.0) {
		theEndTimeVector[threadID].last() = T;
	}
	
	return 0;
}

bool RPoissonPulseProcessModel::isRareHazard()
{
	//double p0 = theNonzeroOccurrenceRate * theMeanDuration;
	if (getNonzeroOccurrenceRateValue() <= 1.0 / 365.0) { // Is a hazard with a lambda of 1.0 per year rare?
		return true;
	} else {
		return false;
	}
}

bool RPoissonPulseProcessModel::canBeRunParallelly(){
	return true;
}