#include "RDuWang2012IntensityModel.h"

#include <QCoreApplication>

#include "RParameter.h"
#include "RResponse.h"
#include "RLocation.h"
#include "RLocationResponse.h"
#include "RMainWindow.h"

#include <math.h>

#include <gsl/gsl_math.h>

RDuWang2012IntensityModel::RDuWang2012IntensityModel(QObject *parent, QString name)
	: RModel(parent, name)
{
	//theEpsilon=0;
	//$$ theIntereventSigma = 0;
	//$$ theIntraeventSigma = 0;
	theFaultType = RDuWang2012IntensityModel::Unspecified;
	theSiteClass = RDuWang2012IntensityModel::UnspecifiedClass;
	theResponseType = RDuWang2012IntensityModel::CAV;
	
	// Model Error Parameters
/* //$$ 
	theC8Uncertainty = 0;
	theC1Uncertainty = 0;
	theC2Uncertainty = 0;
	theC3Uncertainty = 0;
	theC4Uncertainty = 0;
	theC5Uncertainty = 0;
	theC6Uncertainty = 0;
	theC7Uncertainty = 0;
	theHUncertainty = 0;
*/
}

RDuWang2012IntensityModel::~RDuWang2012IntensityModel()
{

}

QString RDuWang2012IntensityModel::getMagnitudeList()
{
	return parameterListToString(theMagnitudeList);
}

void RDuWang2012IntensityModel::setMagnitudeList(QString value)
{
	theMagnitudeList = stringToParameterList(value);
}

QString RDuWang2012IntensityModel::getHypocentreLocationList()
{
	return parameterListToString(theHypocentreLocationList);
}

void RDuWang2012IntensityModel::setHypocentreLocationList(QString value)
{
	theHypocentreLocationList = stringToParameterList(value);

	// Checking if all the passed parameters are either RLocation or RLocationResponse
	parameterListToLocationList(theHypocentreLocationList);
}

//QObject * RDuWang2012IntensityModel::getEpsilon() const
//{
//	return theEpsilon;
//}

//void RDuWang2012IntensityModel::setEpsilon(QObject *value)
//{
//	theEpsilon = qobject_cast<RParameter *>(value);
//}

QObject * RDuWang2012IntensityModel::getIntraeventSigma() const
{
	return theIntraeventSigma.data();
}

void RDuWang2012IntensityModel::setIntraeventSigma(QObject *value)
{
	theIntraeventSigma = qobject_cast<RParameter *>(value);
}

QObject * RDuWang2012IntensityModel::getIntereventSigma() const
{
	return theIntereventSigma.data();
}

void RDuWang2012IntensityModel::setIntereventSigma(QObject *value)
{
	theIntereventSigma = qobject_cast<RParameter *>(value);
}

RDuWang2012IntensityModel::RFaultType RDuWang2012IntensityModel::getFaultType() const
{
	return theFaultType;
}

void RDuWang2012IntensityModel::setFaultType(RDuWang2012IntensityModel::RFaultType value)
{
	theFaultType = value;
}

RDuWang2012IntensityModel::RSiteClass RDuWang2012IntensityModel::getSiteClass() const
{
	return theSiteClass;
}

void RDuWang2012IntensityModel::setSiteClass(RDuWang2012IntensityModel::RSiteClass value)
{
	theSiteClass = value;
}

RDuWang2012IntensityModel::RResponseType RDuWang2012IntensityModel::getResponseType() const
{
	return theResponseType;
}

void RDuWang2012IntensityModel::setResponseType(RDuWang2012IntensityModel::RResponseType value)
{
	theResponseType = value;
}

QString RDuWang2012IntensityModel::getStructureLocationList()
{
	return parameterListToString(theStructureLocationList);
}

void RDuWang2012IntensityModel::setStructureLocationList(QString value)
{
	theStructureLocationList = stringToParameterList(value);

	// Checking if all the passed parameters are either RLocation or RLocationResponse
	parameterListToLocationList(theStructureLocationList);

	createResponses();
}

QObject * RDuWang2012IntensityModel::getC8Uncertainty() const	
{	
	return theC8Uncertainty.data();
}	
	
void RDuWang2012IntensityModel::setC8Uncertainty(QObject *value)	
{	
	theC8Uncertainty = qobject_cast<RParameter *>(value);
}	
	
QObject * RDuWang2012IntensityModel::getC1Uncertainty() const	
{	
	return theC1Uncertainty.data();
}	
	
void RDuWang2012IntensityModel::setC1Uncertainty(QObject *value)	
{	
	theC1Uncertainty = qobject_cast<RParameter *>(value);
}	
	
QObject * RDuWang2012IntensityModel::getC2Uncertainty() const	
{	
	return theC2Uncertainty.data();
}	
	
void RDuWang2012IntensityModel::setC2Uncertainty(QObject *value)	
{	
	theC2Uncertainty = qobject_cast<RParameter *>(value);
}	
	
QObject * RDuWang2012IntensityModel::getC3Uncertainty() const	
{	
	return theC3Uncertainty.data();
}	
	
void RDuWang2012IntensityModel::setC3Uncertainty(QObject *value)	
{	
	theC3Uncertainty = qobject_cast<RParameter *>(value);
}	
	
QObject * RDuWang2012IntensityModel::getC4Uncertainty() const	
{	
	return theC4Uncertainty.data();
}	
	
void RDuWang2012IntensityModel::setC4Uncertainty(QObject *value)	
{	
	theC4Uncertainty = qobject_cast<RParameter *>(value);
}	
	
QObject * RDuWang2012IntensityModel::getC5Uncertainty() const	
{	
	return theC5Uncertainty.data();
}	
	
void RDuWang2012IntensityModel::setC5Uncertainty(QObject *value)	
{	
	theC5Uncertainty = qobject_cast<RParameter *>(value);
}	
	
QObject * RDuWang2012IntensityModel::getC6Uncertainty() const	
{	
	return theC6Uncertainty.data();
}	
	
void RDuWang2012IntensityModel::setC6Uncertainty(QObject *value)	
{	
	theC6Uncertainty = qobject_cast<RParameter *>(value);
}	
	
QObject * RDuWang2012IntensityModel::getC7Uncertainty() const	
{	
	return theC7Uncertainty.data();
}	
	
void RDuWang2012IntensityModel::setC7Uncertainty(QObject *value)	
{	
	theC7Uncertainty = qobject_cast<RParameter *>(value);
}	

QObject * RDuWang2012IntensityModel::getHUncertainty() const	
{	
	return theHUncertainty.data();
}	

void RDuWang2012IntensityModel::setHUncertainty(QObject *value)	
{	
	theHUncertainty = qobject_cast<RParameter *>(value);
}	


void RDuWang2012IntensityModel::createResponses()
{
	// Removing old responses
	while (theSpectralResponseList.count() > 0) {
		delete theSpectralResponseList.takeAt(0);
	}


	// Instantiating the response objects one for each structure
	for (int i = 0; i < theStructureLocationList.count(); i++) {
		theDomain->createObject(objectName() + theStructureLocationList[i].data()->objectName() + "Response", "RGenericResponse");
		RResponse *response = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
		response->setModel(this);
		theSpectralResponseList << response;
	}

	// Update the domain tree
	RMainWindow *mainWindow = qobject_cast<RMainWindow *>(parent()->parent());
	if (mainWindow) {
		mainWindow->updateTree("Domain");
	}
}

int RDuWang2012IntensityModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
{
	if (theStructureLocationList.isEmpty()) {
		rCritical(threadID) << "Error: There should be at least one structure location in the model" << objectName() << ".";
		return -1;
	}

	// Checking if the number of magnitudes is equal to the number of hypocentre locations
	if (theHypocentreLocationList.count() != theMagnitudeList.count()) {
		rCritical(threadID) << "Error: The number of magnitude parameters should be equal to the number of hypocentre locations in the model" << objectName() << ".";
		return -1;
	}

	//double epsilon = theEpsilon->getCurrentValue();
	double intereventSigma = theIntereventSigma.data()->getCurrentValue(threadID);
	double intraeventSigma = theIntraeventSigma.data()->getCurrentValue(threadID);
	double c8Uncertainty = theC8Uncertainty.data()->getCurrentValue(threadID);
	double c1Uncertainty = theC1Uncertainty.data()->getCurrentValue(threadID);
	double c2Uncertainty = theC2Uncertainty.data()->getCurrentValue(threadID);
	double c3Uncertainty = theC3Uncertainty.data()->getCurrentValue(threadID);
	double c4Uncertainty = theC4Uncertainty.data()->getCurrentValue(threadID);
	double c5Uncertainty = theC5Uncertainty.data()->getCurrentValue(threadID);
	double c6Uncertainty = theC6Uncertainty.data()->getCurrentValue(threadID);
	double c7Uncertainty = theC7Uncertainty.data()->getCurrentValue(threadID);
	double hUncertainty = theHUncertainty.data()->getCurrentValue(threadID);

	// Converting the list of parameters to the list of locations
	QList<RLocation *> locationList = parameterListToLocationList(theStructureLocationList);


	double FN = 0.0;
	double FR = 0.0;
	if (theFaultType == RDuWang2012IntensityModel::Unspecified) {
		FN = 0.0;
		FR = 0.0;
	} else if (theFaultType == RDuWang2012IntensityModel::Normal) {
		FN = 1.0;
		FR = 0.0;
	} else if (theFaultType == RDuWang2012IntensityModel::Reverse) {
		FN = 0.0;
		FR = 1.0;
	} else if (theFaultType == RDuWang2012IntensityModel::ReverseOblique) {
		FN = 0.0;
		FR = 0.5;
	}
	double SC = 0.0;
	double SD = 0.0;
	if (theSiteClass == RDuWang2012IntensityModel::UnspecifiedClass) {
		SC = 0.0;
		SD = 0.0;
	} else if (theSiteClass == RDuWang2012IntensityModel::SGSSiteClassC) {
		SC = 1.0;
		SD = 0.0;
	} else if (theSiteClass == RDuWang2012IntensityModel::SGSSiteClassD) {
		SC = 0.0;
		SD = 1.0;
	}


	QVector<double> spectralResponseVector;
	for (int i = 0; i < locationList.count(); i++) {
		spectralResponseVector << 0.0;
	}

	for (int index = 0; index < theMagnitudeList.count(); index++) {
		// For convenience, look up the values of the parameters first
		double M = theMagnitudeList[index].data()->getCurrentValue(threadID);

		// IMPORTANT: If M = 0, it means that this hazard is off, so the resulting PGA is Zero
		if (M == 0.0) {
			continue;
		}
		if (getDisplayOutput()) {
			if (M > 8.0 || M < 4.5) {
				rCritical(threadID) << "Warning: The magnitude in the model" << objectName() << "should be between 4.5 and 8.";
			}
		}
		if (getDisplayOutput()) {
				rCritical(threadID) << "Magnitude" << M << " ";
		}

		// Verification that the user has given an RLocationResponse
		RLocation *theHypocentreLocationObject = parameterToLocation(theHypocentreLocationList[index].data());
		if (!theHypocentreLocationObject) {
			rCritical(threadID) << "Error: The location response" << theHypocentreLocationList[index].data()->objectName() << "does not include any location object.";
			return -1;
		}



		for (int i = 0; i < locationList.count(); i++) {
			double Rjb = theHypocentreLocationObject->computeSurfaceDistance(locationList[i],threadID);
			if (Rjb > 200.0) {
				if (getDisplayOutput()) {
					rCritical(threadID) << "Warning: The maximum allowable distance in the model" << objectName() << "is 200 km.";
				}
			}
			if (getDisplayOutput()) {
				rCritical(threadID) << "Rjb :" << Rjb << " ";
		}

			double lnY;
			double lnY2;
			double C1 = 1.826 + c1Uncertainty * 0.157;// c1 = to meann + standard normal realization * stdv
			double C2 = 0 -0.130 + c2Uncertainty * 0.017;// Like c1
			double C3 = 0 -1.403 + c3Uncertainty * 0.134;// Like c1
			double C4 = 0.098 + c4Uncertainty * 0.019;// Like c1
			double C5 = 0.286 + c5Uncertainty * 0.037;// Like c1
			double C6 = 0.481 + c6Uncertainty * 0.033 ;// Like c1
			double C7 = 0 -0.155 + c7Uncertainty * 0.116;// Like c1
			double C8 = 0.095 + c8Uncertainty  * 0.108;// Like c1
			double h = 8.455 + hUncertainty * 0.166;// Like c1
			double Inter = 0.247  ;// is a constant TU* standard normal realization
			double Intra = 0;//will be evaluated after CAV

			//OUTPUT 
			if (getDisplayOutput()) {
				rCritical(threadID) << "C1 :" << C1 << " ";
			}
			if (getDisplayOutput()) {
				rCritical(threadID) << "C2 :" << C2 << " ";
			}
			if (getDisplayOutput()) {
				rCritical(threadID) << "C3 :" << C3 << " ";
			}
			if (getDisplayOutput()) {
				rCritical(threadID) << "C4 :" << C4 << " ";
			}
			if (getDisplayOutput()) {
				rCritical(threadID) << "C5 :" << C5 << " ";
			}
			if (getDisplayOutput()) {
				rCritical(threadID) << "C6 :" << C6 << " ";
			}
			if (getDisplayOutput()) {
				rCritical(threadID) << "C7 :" << C7 << " ";
			}
			if (getDisplayOutput()) {
				rCritical(threadID) << "C8 :" << C8 << " ";
			}
			if (getDisplayOutput()) {
				rCritical(threadID) << "H :" << h << " ";
			}
			if (getDisplayOutput()) {
				rCritical(threadID) << "Inter-event :" << Inter << " ";
			}
			if (getDisplayOutput()) {
				rCritical(threadID) << "SD :" << SD << " ";
			}
			if (getDisplayOutput()) {
				rCritical(threadID) << "SC :" << SC << " ";
			}
			if (getDisplayOutput()) {
				rCritical(threadID) << "FN :" << FN << " ";
			}
			if (getDisplayOutput()) {
				rCritical(threadID) << "FR :" << FR << " ";
			}


			lnY = C1 + C2 * ( 8.5 - M ) * ( 8.5 - M ) + (C3 + C4 * M) * log(sqrt( Rjb*Rjb + h*h )) + C5 * SC + C6 * SD + C7 * FN + C8 * FR ;
			double Y = exp(lnY); // The units of Y is g.s for CAV
			if (getDisplayOutput()) {
				rCritical(threadID) << "CAV Output without Intra-event AND Inter-event Uncertainty  :" << Y << " ";
			}
			//Intra-event non-linear Uncertainty Evaluation
			if (SC == 0 && SD == 0) {
				Intra= 0;
			}else if (SC = 1.0) {
				if ( Y <= 0.15){
					Intra= 0.45;
				}
				else if (0.15 < Y && Y < 1 ){
					Intra= 0.45 - (0.45 - 0.37 )/(log10( 1 / 0.15 ) ) * log10( Y / 0.15 ) ;
				}
				else if (Y >= 1){
					Intra= 0.37;
				}
			} else if (SD = 1.0) {
				if ( Y <= 0.15){
					Intra= 0.38;
				}
				else if (0.15 < Y && Y < 1 ){
					Intra= 0.38 - (0.38 - 0.34 )/(log10( 1 / 0.15 ) ) * log10( Y / 0.15 )  ;
				}
				else if (Y >= 1){
					Intra= 0.34;
				}
			} 

			if (getDisplayOutput()) {
				rCritical(threadID) << "Intra-event :" << Intra << " ";
			}
			lnY2 = lnY +  Intra * intraeventSigma + Inter * intereventSigma  ;
			
			

			Y = exp(lnY2); // The units of Y is g.s for CAV
			if (getDisplayOutput()) {
				rCritical(threadID) << "CAV Output with Intra-event AND Inter-event Uncertainty  :" << Y << " ";
			}

			// SRSS the intensities from different sources if seismicity; Stage 1: Compute sum of squares
			spectralResponseVector[i] += gsl_pow_2(Y);
		}
	}

	// Update responses
	for (int i = 0; i < locationList.count(); i++) {
		// SRSS the intensities from different sources if seismicity; Stage 2: Compute the square root of sum of squares
		spectralResponseVector[i] = sqrt(spectralResponseVector[i]);
		theSpectralResponseList[i]->setCurrentValue(spectralResponseVector[i],threadID);
	}

	return 0;
}

bool RDuWang2012IntensityModel::canBeRunParallelly(){
	return true;
}