#include "RAtkinson1997IntensityModel.h"	
	
#include "RParameter.h"	
#include "RResponse.h"	
#include "RLocation.h"
#include "RLocationResponse.h"
#include "RMainWindow.h"

#include <math.h>

#include <gsl/gsl_math.h>
	
RAtkinson1997IntensityModel::RAtkinson1997IntensityModel(QObject *parent, QString name)	
	: RModel(parent, name)
{	
	// Initializing the pointer data members
	//$$ theMagnitude = 0;
	theSoilType = RAtkinson1997IntensityModel::Rock;
	//$$ theEpsilon = 0;
	//$$ theHypocentreLocation = 0;

	// Model Error Parameters
	//$$ theC0Uncertainty = 0;
	//$$ theC1Uncertainty = 0;
	//$$ theC2Uncertainty = 0;
	//$$ theC3Uncertainty = 0;
	//$$ theCa3Uncertainty = 0;
	//$$ theCa4Uncertainty = 0;
	//$$ theCsigmaUncertainty = 0;
	//$$ theCsUncertainty = 0;
}	
	
RAtkinson1997IntensityModel::~RAtkinson1997IntensityModel()	
{	
	
}	
	
QString RAtkinson1997IntensityModel::getPeriodParameterList()
{
	return parameterListToString(thePeriodParameterList);
}

void RAtkinson1997IntensityModel::setPeriodParameterList(QString value)
{

	thePeriodParameterList = stringToParameterList(value);
}	
	
QObject * RAtkinson1997IntensityModel::getHypocentreLocation() const	
{	
	return theHypocentreLocation.data();
}	
	
void RAtkinson1997IntensityModel::setHypocentreLocation(QObject *value)	
{	

	theHypocentreLocation = qobject_cast<RParameter *>(value);

	// Verification that the user has given an RLocationResponse or an RLocation
	parameterToLocation(theHypocentreLocation.data());
}	
	
QObject * RAtkinson1997IntensityModel::getMagnitude() const	
{	
	return theMagnitude.data();
}	
	
void RAtkinson1997IntensityModel::setMagnitude(QObject *value)	
{	

	theMagnitude = qobject_cast<RParameter *>(value);
}	
	
RAtkinson1997IntensityModel::RSoilType RAtkinson1997IntensityModel::getSoilType() const	
{	
	return theSoilType;
}	
	
void RAtkinson1997IntensityModel::setSoilType(RAtkinson1997IntensityModel::RSoilType value)	
{	
	theSoilType = value;
}	
	
QObject * RAtkinson1997IntensityModel::getEpsilon() const	
{	
	return theEpsilon.data();
}	
	
void RAtkinson1997IntensityModel::setEpsilon(QObject *value)	
{	
	theEpsilon = qobject_cast<RParameter *>(value);
}	

QString RAtkinson1997IntensityModel::getStructureLocationList()
{
	return parameterListToString(theStructureLocationList);
}

void RAtkinson1997IntensityModel::setStructureLocationList(QString value)
{
	theStructureLocationList = stringToParameterList(value);
	
	// Checking if all the passed parameters are either RLocation or RLocationResponse
	parameterListToLocationList(theStructureLocationList);
	
	// Removing old responses
	while (thePSAResponseList.count() > 0) {
		//thePSAResponseList[0]->setModel(0);
		delete thePSAResponseList.takeAt(0);
	}
	
	// Instantiating the response objects one for each structure
	for (int i = 0; i < theStructureLocationList.count(); i++) {
		theDomain->createObject(objectName() + theStructureLocationList[i].data()->objectName() + "Response", "RGenericResponse");
		RResponse *response = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
		response->setModel(this);
		thePSAResponseList << response;
	}

	// Update the domain tree
	RMainWindow *mainWindow = qobject_cast<RMainWindow *>(parent()->parent());
	if (mainWindow) {
		mainWindow->updateTree("Domain");
	}
}
	
QObject * RAtkinson1997IntensityModel::getC0Uncertainty() const	
{	
	return theC0Uncertainty.data();
}	
	
void RAtkinson1997IntensityModel::setC0Uncertainty(QObject *value)	
{	
	theC0Uncertainty = qobject_cast<RParameter *>(value);
}	
	
QObject * RAtkinson1997IntensityModel::getC1Uncertainty() const	
{	
	return theC1Uncertainty.data();
}	
	
void RAtkinson1997IntensityModel::setC1Uncertainty(QObject *value)	
{	
	theC1Uncertainty = qobject_cast<RParameter *>(value);
}	
	
QObject * RAtkinson1997IntensityModel::getC2Uncertainty() const	
{	
	return theC2Uncertainty.data();
}	
	
void RAtkinson1997IntensityModel::setC2Uncertainty(QObject *value)	
{	
	theC2Uncertainty = qobject_cast<RParameter *>(value);
}	
	
QObject * RAtkinson1997IntensityModel::getC3Uncertainty() const	
{	
	return theC3Uncertainty.data();
}	
	
void RAtkinson1997IntensityModel::setC3Uncertainty(QObject *value)	
{	
	theC3Uncertainty = qobject_cast<RParameter *>(value);
}	
	
QObject * RAtkinson1997IntensityModel::getCa3Uncertainty() const	
{	
	return theCa3Uncertainty.data();
}	
	
void RAtkinson1997IntensityModel::setCa3Uncertainty(QObject *value)	
{	
	theCa3Uncertainty = qobject_cast<RParameter *>(value);
}	
	
QObject * RAtkinson1997IntensityModel::getCa4Uncertainty() const	
{	
	return theCa4Uncertainty.data();
}	
	
void RAtkinson1997IntensityModel::setCa4Uncertainty(QObject *value)	
{	
	theCa4Uncertainty = qobject_cast<RParameter *>(value);
}	
	
QObject * RAtkinson1997IntensityModel::getCsigmaUncertainty() const	
{	
	return theCsigmaUncertainty.data();
}	
	
void RAtkinson1997IntensityModel::setCsigmaUncertainty(QObject *value)	
{	
	theCsigmaUncertainty = qobject_cast<RParameter *>(value);
}	
	
QObject * RAtkinson1997IntensityModel::getCsUncertainty() const	
{	
	return theCsUncertainty.data();
}	
	
void RAtkinson1997IntensityModel::setCsUncertainty(QObject *value)	
{	
	theCsUncertainty = qobject_cast<RParameter *>(value);
}	

int RAtkinson1997IntensityModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
{	
	// For convenice, look up the values of the parameters first
	double M = theMagnitude.data()->getCurrentValue(threadID);
	double S = 0;
	if (theSoilType == RAtkinson1997IntensityModel::Rock) {
		S = 0;
	} else if (theSoilType == RAtkinson1997IntensityModel::FirmSoil) {
		S = 1;
	}

	double sigma = theEpsilon.data()->getCurrentValue(threadID);

	double c0Uncertainty = theC0Uncertainty.data()->getCurrentValue(threadID);
	double c1Uncertainty = theC1Uncertainty.data()->getCurrentValue(threadID);
	double c2Uncertainty = theC2Uncertainty.data()->getCurrentValue(threadID);
	double c3Uncertainty = theC3Uncertainty.data()->getCurrentValue(threadID);
	double ca3Uncertainty = theCa3Uncertainty.data()->getCurrentValue(threadID);
	double ca4Uncertainty = theCa4Uncertainty.data()->getCurrentValue(threadID);
	double csigmaUncertainty = theCsigmaUncertainty.data()->getCurrentValue(threadID);
	double csUncertainty = theCsUncertainty.data()->getCurrentValue(threadID);

	// Converting the list of parameters to the list of locations
	QList<RLocation *> theLocationList = parameterListToLocationList(theStructureLocationList);

	
	// Verification that the user has given an RLocationResponse
	RLocation *theHypocentreLocationObject = parameterToLocation(theHypocentreLocation.data());  
	if (!theHypocentreLocationObject) {
		rCritical(threadID) << "Error: The location response" << theHypocentreLocation.data()->objectName() << "does not include any location object.";
		return -1;
	}

	// Calculating h
	double h = qAbs(theHypocentreLocationObject->getAltitude(threadID));


	// Declare variables to be used below
	double logPSA;


	// First check whether the magnitude is greater than 8
	if (M>8.0) {
		M = 8.0;
	}

	for (int i = 0; i < theLocationList.count(); i++) {
		// Calculating R
		double R = theHypocentreLocationObject->computeSurfaceDistance(theLocationList[i],threadID);
		double T = thePeriodParameterList[i].data()->getCurrentValue(threadID);
		
		// Compute the natural frequency
		double f = 1.0/T;

		double PSA;

		/*// The algorithm to determine the PSA
		if (M<7.5) {

			if (f<0.5) {
				logPSA = 2.40+0.708*(M-6)-0.175*gsl_pow_2(M-6)-0.00066*h-1.0*log10(R)-(0+0.000034*h)*R+0.58*S+0.28*sigma;
			}
			else if (f<0.6) {
				double PSA1 = 2.40+0.708*(M-6)-0.175*gsl_pow_2(M-6)-0.00066*h-1.0*log10(R)-(0+0.000034*h)*R+0.58*S+0.28*sigma;
				double PSA2 = 2.49+0.671*(M-6)-0.166*gsl_pow_2(M-6)+0.00133*h-1.0*log10(R)-(0+0.000038*h)*R+0.60*S+0.27*sigma;

				logPSA = PSA1+(PSA2-PSA1)/(0.6-0.5)*(f-0.5);
			}

			else if (f<1.0) {
				double PSA1 = 2.49+0.671*(M-6)-0.166*gsl_pow_2(M-6)+0.00133*h-1.0*log10(R)-(0+0.000038*h)*R+0.60*S+0.27*sigma;
				double PSA2 = 2.70+0.605*(M-6)-0.115*gsl_pow_2(M-6)+0.00588*h-1.0*log10(R)-(0+0.000066*h)*R+0.59*S+0.25*sigma;

				logPSA = PSA1+(PSA2-PSA1)/(1.0-0.6)*(f-0.6);
			}

			else if (f<1.3) {
				double PSA1 = 2.70+0.605*(M-6)-0.115*gsl_pow_2(M-6)+0.00588*h-1.0*log10(R)-(0+0.000066*h)*R+0.59*S+0.25*sigma;
				double PSA2 = 2.94+0.539*(M-6)-0.126*gsl_pow_2(M-6)+0.00480*h-1.0*log10(R)-(0.0008+0.000054*h)*R+0.57*S+0.24*sigma;

				logPSA = PSA1+(PSA2-PSA1)/(1.3-1.0)*(f-1.0);
			}

			else if (f<1.6) {
				double PSA1 = 2.94+0.539*(M-6)-0.126*gsl_pow_2(M-6)+0.00480*h-1.0*log10(R)-(0.0008+0.000054*h)*R+0.57*S+0.24*sigma;
				double PSA2 = 3.04+0.501*(M-6)-0.111*gsl_pow_2(M-6)+0.00655*h-1.0*log10(R)-(0.0008+0.000068*h)*R+0.54*S+0.25*sigma;
			
				logPSA = PSA1+(PSA2-PSA1)/(1.6-1.3)*(f-1.3);
			}

			else if (f<2.0) {
				double PSA1 = 3.04+0.501*(M-6)-0.111*gsl_pow_2(M-6)+0.00655*h-1.0*log10(R)-(0.0008+0.000068*h)*R+0.54*S+0.25*sigma;
				double PSA2 = 3.24+0.473*(M-6)-0.112*gsl_pow_2(M-6)+0.00536*h-1.0*log10(R)-(0.0014+0.000058*h)*R+0.49*S+0.26*sigma;
				
				logPSA = PSA1+(PSA2-PSA1)/(2.0-1.6)*(f-1.6);
			}

			else if (f<2.5) {
				double PSA1 = 3.24+0.473*(M-6)-0.112*gsl_pow_2(M-6)+0.00536*h-1.0*log10(R)-(0.0014+0.000058*h)*R+0.49*S+0.26*sigma;
				double PSA2 = 3.29+0.462*(M-6)-0.118*gsl_pow_2(M-6)+0.00789*h-1.0*log10(R)-(0.0013+0.000068*h)*R+0.44*S+0.27*sigma;
				
				logPSA = PSA1+(PSA2-PSA1)/(2.5-2.0)*(f-2.0);
			}

			else if (f<3.2) {
				double PSA1 = 3.29+0.462*(M-6)-0.118*gsl_pow_2(M-6)+0.00789*h-1.0*log10(R)-(0.0013+0.000068*h)*R+0.44*S+0.27*sigma;
				double PSA2 = 3.35+0.434*(M-6)-0.125*gsl_pow_2(M-6)+0.00969*h-1.0*log10(R)-(0.0016+0.000072*h)*R+0.37*S+0.29*sigma;
				
				logPSA = PSA1+(PSA2-PSA1)/(3.2-2.5)*(f-2.5);
			}

			else if (f<4.0) {
				double PSA1 = 3.35+0.434*(M-6)-0.125*gsl_pow_2(M-6)+0.00969*h-1.0*log10(R)-(0.0016+0.000072*h)*R+0.37*S+0.29*sigma;
				double PSA2 = 3.63+0.413*(M-6)-0.128*gsl_pow_2(M-6)+0.00505*h-1.0*log10(R)-(0.0032+0.000030*h)*R+0.32*S+0.29*sigma;
				
				logPSA = PSA1+(PSA2-PSA1)/(4.0-3.2)*(f-3.2);
			}

			else if (f<5.0) {
				double PSA1 = 3.63+0.413*(M-6)-0.128*gsl_pow_2(M-6)+0.00505*h-1.0*log10(R)-(0.0032+0.000030*h)*R+0.32*S+0.29*sigma;
				double PSA2 = 3.62+0.417*(M-6)-0.124*gsl_pow_2(M-6)+0.00655*h-1.0*log10(R)-(0.0034+0.000022*h)*R+0.26*S+0.30*sigma;
				
				logPSA = PSA1+(PSA2-PSA1)/(5.0-4.0)*(f-4.0);
			}

			else if (f<7.9) {
				double PSA1 = 3.62+0.417*(M-6)-0.124*gsl_pow_2(M-6)+0.00655*h-1.0*log10(R)-(0.0034+0.000022*h)*R+0.26*S+0.30*sigma;
				double PSA2 = 3.45+0.377*(M-6)-0.119*gsl_pow_2(M-6)+0.01043*h-1.0*log10(R)-(0.0031+0.000042*h)*R+0.22*S+0.30*sigma;
				
				logPSA = PSA1+(PSA2-PSA1)/(7.9-5.0)*(f-5.0);
			}

			else if (f<10.0) {
				double PSA1 = 3.45+0.377*(M-6)-0.119*gsl_pow_2(M-6)+0.01043*h-1.0*log10(R)-(0.0031+0.000042*h)*R+0.22*S+0.30*sigma;
				double PSA2 = 3.38+0.364*(M-6)-0.112*gsl_pow_2(M-6)+0.01133*h-1.0*log10(R)-(0.0030+0.000052*h)*R+0.26*S+0.31*sigma;
				
				logPSA = PSA1+(PSA2-PSA1)/(10.0-7.9)*(f-7.9);
			}

			else {
				logPSA = 3.38+0.364*(M-6)-0.112*gsl_pow_2(M-6)+0.01133*h-1.0*log10(R)-(0.0030+0.000052*h)*R+0.26*S+0.31*sigma;
			}

		}
		else {

			if (f<0.5) {
				logPSA = 1.67+0.708*(M-6)-0.175*gsl_pow_2(M-6)-0.00066*h-0.5*log10(R)-(0.00014+0.0*h)*R+0.58*S+0.39*sigma;
			}

			else if (f<0.6) {
				double PSA1 = 1.67+0.708*(M-6)-0.175*gsl_pow_2(M-6)-0.00066*h-0.5*log10(R)-(0.00014+0.0*h)*R+0.58*S+0.39*sigma;
				double PSA2 = 1.77+0.671*(M-6)-0.166*gsl_pow_2(M-6)+0.00133*h-0.5*log10(R)-(0.00014+0.0*h)*R+0.60*S+0.35*sigma;
				
				logPSA = PSA1+(PSA2-PSA1)/(0.6-0.5)*(f-0.5);
			}

			else if (f<1.0) {
				double PSA1 = 1.77+0.671*(M-6)-0.166*gsl_pow_2(M-6)+0.00133*h-0.5*log10(R)-(0.00014+0.0*h)*R+0.60*S+0.35*sigma;
				double PSA2 = 1.98+0.605*(M-6)-0.115*gsl_pow_2(M-6)+0.00588*h-0.5*log10(R)-(0.00090+0.0*h)*R+0.59*S+0.33*sigma;
				
				logPSA = PSA1+(PSA2-PSA1)/(1.0-0.6)*(f-0.6);
			}

			else if (f<1.3) {
				double PSA1 = 1.98+0.605*(M-6)-0.115*gsl_pow_2(M-6)+0.00588*h-0.5*log10(R)-(0.00090+0.0*h)*R+0.59*S+0.33*sigma;
				double PSA2 = 2.21+0.539*(M-6)-0.126*gsl_pow_2(M-6)+0.00480*h-0.5*log10(R)-(0.00084+0.0*h)*R+0.57*S+0.33*sigma;
				
				logPSA = PSA1+(PSA2-PSA1)/(1.3-1.0)*(f-1.0);
			}

			else if (f<1.6) {
				double PSA1 = 2.21+0.539*(M-6)-0.126*gsl_pow_2(M-6)+0.00480*h-0.5*log10(R)-(0.00084+0.0*h)*R+0.57*S+0.33*sigma;
				double PSA2 = 2.31+0.501*(M-6)-0.111*gsl_pow_2(M-6)+0.00655*h-0.5*log10(R)-(0.00120+0.0*h)*R+0.54*S+0.32*sigma;
				
				logPSA = PSA1+(PSA2-PSA1)/(1.6-1.3)*(f-1.3);
			}

			else if (f<2.0) {
				double PSA1 = 2.31+0.501*(M-6)-0.111*gsl_pow_2(M-6)+0.00655*h-0.5*log10(R)-(0.00120+0.0*h)*R+0.54*S+0.32*sigma;
				double PSA2 = 2.52+0.473*(M-6)-0.112*gsl_pow_2(M-6)+0.00536*h-0.5*log10(R)-(0.00152+0.0*h)*R+0.49*S+0.28*sigma;
				
				logPSA = PSA1+(PSA2-PSA1)/(2.0-1.6)*(f-1.6);
			}

			else if (f<2.5) {
				double PSA1 = 2.52+0.473*(M-6)-0.112*gsl_pow_2(M-6)+0.00536*h-0.5*log10(R)-(0.00152+0.0*h)*R+0.49*S+0.28*sigma;
				double PSA2 = 2.56+0.462*(M-6)-0.118*gsl_pow_2(M-6)+0.00789*h-0.5*log10(R)-(0.00162+0.0*h)*R+0.44*S+0.27*sigma;
				
				logPSA = PSA1+(PSA2-PSA1)/(2.5-2.0)*(f-2.0);
			}

			else if (f<3.2) {
				double PSA1 = 2.56+0.462*(M-6)-0.118*gsl_pow_2(M-6)+0.00789*h-0.5*log10(R)-(0.00162+0.0*h)*R+0.44*S+0.27*sigma;
				double PSA2 = 2.62+0.434*(M-6)-0.125*gsl_pow_2(M-6)+0.00969*h-0.5*log10(R)-(0.00150+0.0*h)*R+0.37*S+0.25*sigma;
				
				logPSA = PSA1+(PSA2-PSA1)/(3.2-2.5)*(f-2.5);
			}

			else if (f<4.0) {
				double PSA1 = 2.62+0.434*(M-6)-0.125*gsl_pow_2(M-6)+0.00969*h-0.5*log10(R)-(0.00150+0.0*h)*R+0.37*S+0.25*sigma;
				double PSA2 = 2.91+0.413*(M-6)-0.128*gsl_pow_2(M-6)+0.00505*h-0.5*log10(R)-(0.00186+0.0*h)*R+0.32*S+0.24*sigma;
				
				logPSA = PSA1+(PSA2-PSA1)/(4.0-3.2)*(f-3.2);
			}

			else if (f<5.0) {
				double PSA1 = 2.91+0.413*(M-6)-0.128*gsl_pow_2(M-6)+0.00505*h-0.5*log10(R)-(0.00186+0.0*h)*R+0.32*S+0.24*sigma;
				double PSA2 = 2.90+0.417*(M-6)-0.124*gsl_pow_2(M-6)+0.00655*h-0.5*log10(R)-(0.00184+0.0*h)*R+0.26*S+0.25*sigma;
				
				logPSA = PSA1+(PSA2-PSA1)/(5.0-4.0)*(f-4.0);
			}

			else if (f<7.9) {
				double PSA1 = 2.90+0.417*(M-6)-0.124*gsl_pow_2(M-6)+0.00655*h-0.5*log10(R)-(0.00184+0.0*h)*R+0.26*S+0.25*sigma;
				double PSA2 = 2.73+0.377*(M-6)-0.119*gsl_pow_2(M-6)+0.01043*h-0.5*log10(R)-(0.00154+0.0*h)*R+0.22*S+0.27*sigma;
				
				logPSA = PSA1+(PSA2-PSA1)/(7.9-5.0)*(f-5.0);
			}

			else if (f<10.0) {
				double PSA1 = 2.73+0.377*(M-6)-0.119*gsl_pow_2(M-6)+0.01043*h-0.5*log10(R)-(0.00154+0.0*h)*R+0.22*S+0.27*sigma;
				double PSA2 = 2.66+0.364*(M-6)-0.112*gsl_pow_2(M-6)+0.01133*h-0.5*log10(R)-(0.00174+0.0*h)*R+0.26*S+0.28*sigma;
				
				logPSA = PSA1+(PSA2-PSA1)/(10.0-7.9)*(f-7.9);
			}

			else {
				logPSA = 2.66+0.364*(M-6)-0.112*gsl_pow_2(M-6)+0.01133*h-0.5*log10(R)-(0.00174+0.0*h)*R+0.26*S+0.28*sigma;
			}


		}

		// Assign the PSA value to the response object of this model (converted to g)
		PSA = pow(10.0, logPSA) / 981.0;*/

		double c0 = -0.035721735 * f*f + 0.450333079 * f + (-0.363220796*M+4.504677765) + c0Uncertainty * 0.114018678;
		double c1 = 0.006940234 * f*f - 0.099194239 * f + 0.691458651 + c1Uncertainty * 0.039043779;
		double c2 = -0.00087448 * f*f + 0.01172348 * f - 0.149527434 + c2Uncertainty * 0.017587579;
		double c3 = -2.75373e-5 * f*f*f*f + 0.000620298 * f*f*f - 0.004613233 * f*f + 0.013309472 *f - 0.005367818 + c3Uncertainty * 0.00124175;
		double ca3 = (1.65694e-5*M-0.000179439) * f*f + (-0.00028233*M+0.002859206) * f + (0.000457772*M-0.003509723) + ca3Uncertainty *  (0.00033395 + 0.000266524) / 2.0;
		double ca4 = (1.08039e-7*M-8.64311e-7) * f*f*f*f + (-2.47397e-6*M+1.97918e-5) * f*f*f + (1.8202e-5*M-0.000145616) * f*f + (-4.54508e-5*M+0.000363606) *f + (1.72283e-6*M-1.37826e-5) + ca4Uncertainty * (8.14736E-06 + 0.0) / 2.0;
		double csigma = (-0.007509084*M+0.051049657) * f + (0.035196642*M+0.044869466) + csigmaUncertainty * (0.013917106 + 0.037899501) / 2.0;
		double cs = 0.007649599 * f*f - 0.119314869 * f + 0.683941418 + csUncertainty * 0.021367993;
		double ca1 = -0.25*M+2.5;

		logPSA = c0 + c1*(M - 6) + c2*gsl_pow_2(M-6) + c3*h - ca1*log10(R) - (ca3 + ca4*h)*R + cs*S + csigma*sigma;//???????????????????????

		// Assign the PSA value to the response object of this model (converted to g)
		PSA = pow(10.0, logPSA) / 981.0;

		thePSAResponseList[i]->setCurrentValue(PSA,threadID);
	}

	return 0;
}

bool RAtkinson1997IntensityModel::canBeRunParallelly(){
	return true;
}