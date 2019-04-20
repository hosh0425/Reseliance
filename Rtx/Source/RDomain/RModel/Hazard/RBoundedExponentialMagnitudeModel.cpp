#include "RBoundedExponentialMagnitudeModel.h"

#include "RParameter.h"
#include "RResponse.h"

#include <gsl/gsl_randist.h>
#include <gsl/gsl_cdf.h>
#include <gsl/gsl_math.h>

#include <math.h>

RBoundedExponentialMagnitudeModel::RBoundedExponentialMagnitudeModel(QObject *parent, QString name)
	: RModel(parent, name)
{
	//$$ theMinimumMagnitude = 0;
	//$$ theMaximumMagnitude = 0;
	//$$ theBeta = 0;
	//$$ theTheta = 0;
	
	theDomain->createObject(objectName() + "Response", "RGenericResponse");
	theMagnitude = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theMagnitude->setModel(this);
}

RBoundedExponentialMagnitudeModel::~RBoundedExponentialMagnitudeModel()
{

}

QObject * RBoundedExponentialMagnitudeModel::getMinimumMagnitude() const	
{	
	return theMinimumMagnitude.data();
}	
	
void RBoundedExponentialMagnitudeModel::setMinimumMagnitude(QObject *value)	
{	
	theMinimumMagnitude = qobject_cast<RParameter *>(value);
}	
	
QObject * RBoundedExponentialMagnitudeModel::getMaximumMagnitude() const	
{	
	return theMaximumMagnitude.data();
}	
	
void RBoundedExponentialMagnitudeModel::setMaximumMagnitude(QObject *value)	
{	
	theMaximumMagnitude = qobject_cast<RParameter *>(value);
}	
	
QObject * RBoundedExponentialMagnitudeModel::getBeta() const	
{	
	return theBeta.data();
}	
	
void RBoundedExponentialMagnitudeModel::setBeta(QObject *value)	
{	
	theBeta = qobject_cast<RParameter *>(value);
}	
	
QObject * RBoundedExponentialMagnitudeModel::getTheta() const	
{	
	return theTheta.data();
}	
	
void RBoundedExponentialMagnitudeModel::setTheta(QObject *value)	
{	
	theTheta = qobject_cast<RParameter *>(value);
}	

QList<RParameter *> RBoundedExponentialMagnitudeModel::getPhysicalParameterList()
{
	QList<RParameter *> paramList;
	paramList << theTheta.data();//?????????????????????????????????????????????????????????????????????????
	return paramList;
}

int RBoundedExponentialMagnitudeModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
{	
	double Mmin = theMinimumMagnitude.data()->getCurrentValue(threadID);
	double Mmax = theMaximumMagnitude.data()->getCurrentValue(threadID);
	double beta = theBeta.data()->getCurrentValue(threadID);
	double theta = theTheta.data()->getCurrentValue(threadID);
	
	double cdf = gsl_cdf_gaussian_P(theta, 1.0);
	double AA = exp(-beta * (Mmax - Mmin));
	double BB = 1.0 - (1.0 - AA) * cdf;
	double magnitude = -1.0 / beta * log(BB) + Mmin;

	// Updating the response current value
	theMagnitude->setCurrentValue(magnitude,threadID);

	if (evaluateDDM) {
		double dMagnitude_dMmin = -cdf * AA / BB + 1.0;
		double dMagnitude_dMmax = cdf * AA / BB ;
		double dMagnitude_dBeta = log(BB) / gsl_pow_2(beta) + cdf / beta * (Mmax - Mmin) * AA / BB;
		double dMagnitude_dTheta = gsl_ran_gaussian_pdf(theta, 1.0) / beta * (1.0 - AA) / BB;

		QMap<RParameter *, double> theMagnitudeGradientMap;
		theMagnitudeGradientMap.insertMulti(theMinimumMagnitude.data(), dMagnitude_dMmin);
		theMagnitudeGradientMap.insertMulti(theMaximumMagnitude.data(), dMagnitude_dMmax);
		theMagnitudeGradientMap.insertMulti(theBeta.data(), dMagnitude_dBeta);
		theMagnitudeGradientMap.insertMulti(theTheta.data(), dMagnitude_dTheta);

		theDDMMap.insertMulti(theMagnitude, theMagnitudeGradientMap);
	}

	return 1;
}	

bool RBoundedExponentialMagnitudeModel::canBeRunParallelly(){
	return true;
}