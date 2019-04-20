#include "RIdriss2013IntensityModel.h"	

#include <QCoreApplication>
#include <QMetaEnum>
	
#include "RParameter.h"	
#include "RResponse.h"	
#include "RLocation.h"
#include "RLocationResponse.h"
#include "RMainWindow.h"

#include <math.h>

#include <gsl/gsl_math.h>
	
RIdriss2013IntensityModel::RIdriss2013IntensityModel(QObject *parent, QString name)	
	: RModel(parent, name)
{	
	//theEpsilon = 0;
	theResponseType = RIdriss2013IntensityModel::Sa;

}	
	
RIdriss2013IntensityModel::~RIdriss2013IntensityModel()	
{	
	
}	

QString RIdriss2013IntensityModel::getMagnitudeList()	
{	
	return parameterListToString(theMagnitudeList);
}	

void RIdriss2013IntensityModel::setMagnitudeList(QString value)	
{	
	theMagnitudeList = stringToParameterList(value);
}

QString RIdriss2013IntensityModel::getDepthToTopOfRuptureList() 	
{	
	return parameterListToString(theDepthToTopOfRuptureList);
}	
	
void RIdriss2013IntensityModel::setDepthToTopOfRuptureList(QString value)	
{	
	theDepthToTopOfRuptureList = stringToParameterList(value);
}

QString RIdriss2013IntensityModel::getHypocentreLocationList()	
{	
	return parameterListToString(theHypocentreLocationList);
}	

void RIdriss2013IntensityModel::setHypocentreLocationList(QString value)	
{	
	theHypocentreLocationList = stringToParameterList(value);

	// Checking if all the passed parameters are either RLocation or RLocationResponse
	parameterListToLocationList(theHypocentreLocationList);
}

QString RIdriss2013IntensityModel::getRupDistList() 	
{	
	return parameterListToString(theRupDistList);
}	
	
void RIdriss2013IntensityModel::setRupDistList(QString value)	
{	
	theRupDistList = stringToParameterList(value);
}


QString RIdriss2013IntensityModel::getEpsilonUncertaintyList()	
{	
	return parameterListToString(theEpsilonUncertaintyList);
}	
	
void RIdriss2013IntensityModel::setEpsilonUncertaintyList(QString value)	
{	
	theEpsilonUncertaintyList = stringToParameterList(value);

	createResponses();
}	

QList<RParameter *> RIdriss2013IntensityModel::getPhysicalParameterList()
{
	QList<RParameter *> parmList;
	for (int i = 0; i < theEpsilonUncertaintyList.count(); i++){
		parmList << theEpsilonUncertaintyList[i].data();
	}
	return parmList;
}

QString RIdriss2013IntensityModel::getFaultTypes() const	
{
	QMetaEnum metaEnum = metaObject()->enumerator(metaObject()->indexOfEnumerator("RFaultType"));
	QString result = "";
	for (int i = 0; i < theFaultTypes.count(); i++) {
		QString temp = metaEnum.valueToKey(theFaultTypes[i]);
	    result += temp + "; ";
	}
	return result;
}	
	
void RIdriss2013IntensityModel::setFaultTypes(QString value)	
{	
	QString string = value;
	QMetaEnum metaEnum = metaObject()->enumerator(metaObject()->indexOfEnumerator("RFaultType"));

	QRegExp regExp = QRegExp(QString("[ ,;:\t\n\v\r%1%2]").arg(QChar(QChar::ParagraphSeparator)).arg(QChar(QChar::LineSeparator)));
	QStringList stringList = string.split(regExp, QString::SkipEmptyParts);
	string = "";
	QList<RIdriss2013IntensityModel::RFaultType> tempList;
	for (int i = 0; i < stringList.count(); i++) {
		QByteArray ba = stringList[i].toLocal8Bit();
		const char *tempChar = ba.data();
		int tempValue = metaEnum.keyToValue(tempChar);
		RIdriss2013IntensityModel::RFaultType temp = RFaultType(tempValue);
		if (temp < 0) {
			rCritical() << "Error in the Model" << objectName() << ": The fault type" << stringList[i] << "is not valid";
			return;
		}
		tempList.append(temp);
	}
	theFaultTypes = tempList;
}	

RIdriss2013IntensityModel::RResponseType RIdriss2013IntensityModel::getResponseType() const	
{	
	return theResponseType;
}	
	
void RIdriss2013IntensityModel::setResponseType(RIdriss2013IntensityModel::RResponseType value)	
{	
	theResponseType = value;
}

QString RIdriss2013IntensityModel::getPeriodList()	
{	
	return parameterListToString(thePeriodList);
}	
	
void RIdriss2013IntensityModel::setPeriodList(QString value)	
{	
	thePeriodList = stringToParameterList(value);

	createResponses();
}	
	
QString RIdriss2013IntensityModel::getStructureLocationList()	
{	
	return parameterListToString(theStructureLocationList);
}	
	
void RIdriss2013IntensityModel::setStructureLocationList(QString value)	
{	
	theStructureLocationList = stringToParameterList(value);

	// Checking if all the passed parameters are either RLocation or RLocationResponse
	parameterListToLocationList(theStructureLocationList);

	createResponses();
}	
	
QString RIdriss2013IntensityModel::getShearWaveVelocityList()	
{	
	return parameterListToString(theShearWaveVelocityList);
}	
	
void RIdriss2013IntensityModel::setShearWaveVelocityList(QString value)	
{	
	theShearWaveVelocityList = stringToParameterList(value);

	createResponses();
}	

void RIdriss2013IntensityModel::createResponses()
{
	// Removing old responses
	while (theSpectralResponseList.count() > 0) {
		delete theSpectralResponseList.takeAt(0);
	}

	if (theStructureLocationList.isEmpty() || theShearWaveVelocityList.isEmpty()) {
		return;
	}

	if ((theStructureLocationList.count() != thePeriodList.count() && theResponseType == RIdriss2013IntensityModel::Sa) || (theStructureLocationList.count() != theShearWaveVelocityList.count() && theShearWaveVelocityList.count() > 1) || (theStructureLocationList.count() != theEpsilonUncertaintyList.count() && theEpsilonUncertaintyList.count() > 1)) {
		rCritical() << "Error: The number of input parameters for the period list, structure location list and uncertainty list are not equal in the model" << objectName() << ".";
		return;
	}
	
	// Instantiating the response objects one for each structure/location
	for (int i = 0; i < theStructureLocationList.count(); i++) {
		QString periodStr = "";
		if (!thePeriodList.isEmpty()) {
			periodStr = thePeriodList[i].data()->objectName();
		}
		theDomain->createObject(objectName() + theStructureLocationList[i].data()->objectName()+ periodStr + "Response", "RGenericResponse");
		RResponse *response = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
		response->setModel(this);
		theSpectralResponseList << response;

		// A line to keep the GUI responsive
		if (i % 10 == 0) {
			QCoreApplication::processEvents();
		}
	}

	// Update the domain tree
	RMainWindow *mainWindow = qobject_cast<RMainWindow *>(parent()->parent());
	if (mainWindow) {
		mainWindow->updateTree("Domain");
	}
}

int RIdriss2013IntensityModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
{	
	if (theStructureLocationList.isEmpty()) {
		rCritical(threadID) << "Error: There should be at least one structure location in the model" << objectName() << ".";
		return -1;
	}
	if ((theStructureLocationList.count() != thePeriodList.count() && theResponseType == RIdriss2013IntensityModel::Sa) || (theStructureLocationList.count() != theShearWaveVelocityList.count() && theShearWaveVelocityList.count() > 1)) {
		rCritical(threadID) << "Error: The number of input parameters for the period list, structure location list, and shear wave velocity list are not equal in the model" << objectName() << ".";
		return -1;
	}
	
	// Checking if the number of magnitudes is equal to the number of hypocentre locations
	if (theHypocentreLocationList.count() != theMagnitudeList.count()) {
		rCritical(threadID) << "Error: In the model" << objectName() << "the number of magnitude parameters should be equal to the number of hypocentre locations.";
		return -1;
	}

	// Checking if the number of magnitudes is equal to the number of hypocentre locations
	if ((theFaultTypes.count() != theMagnitudeList.count()) && (theFaultTypes.count() != 1)) {
		rCritical(threadID) << "Error: In the model" << objectName() << "the number of fault types should be either equal to 1 or to the number of magnitude parameters and hypocenter locations.";
		return -1;
	}



	// Converting the list of parameters to the list of locations
	QList<RLocation *> locationList = parameterListToLocationList(theStructureLocationList);


	// Some constants
	

	// Period dependent coefficients
	QVector<double> periodTableVector;
	periodTableVector  << 0 << 0.01 << 0.02 << 0.03 << 0.05 << 0.075 << 0.1 << 0.15 << 0.2 << 0.25 << 0.3 << 0.4 << 0.5 << 0.75 << 1 << 1.5 << 2 << 3 << 4 << 5 << 7.5 << 10;

	QVector<double> alfa1ML675Vector;
	alfa1ML675Vector << 7.0887 << 7.0887 << 7.1157 << 7.2087 << 6.2638 << 5.9051 << 7.5791 << 8.019 << 9.2812 << 9.5804 << 9.8912 << 9.5342 << 9.2142 << 8.3517 << 7.0453 << 5.1307 << 3.361 << 0.1784 << -2.4301 << -4.357 << -7.8275 << -9.2857;
	QVector<double> alfa2ML675Vector;
	alfa2ML675Vector   << 0.2058 << 0.2058 << 0.2058 << 0.2058 << 0.0625 << 0.1128 << 0.0848 << 0.1713 << 0.1041 << 0.0875 << 0.0003 << 0.0027 << 0.0399 << 0.0689 << 0.16 << 0.2429 << 0.3966 << 0.756 << 0.9283 << 1.1209 << 1.4016 << 1.5574;
	
	QVector<double> beta1ML675Vector;
	beta1ML675Vector   << 2.9935 << 2.9935 << 2.9935 << 2.9935 << 2.8664 << 2.9406 << 3.019 << 2.7871 << 2.8611 << 2.8289 << 2.8423 << 2.83 << 2.856 << 2.7544 << 2.7339 << 2.68 << 2.6837 << 2.6907 << 2.5782 << 2.5468 << 2.4478 << 2.3922;
	QVector<double> beta2ML675Vector;
	beta2ML675Vector  << -0.2287 << -0.2287 << -0.2287 << -0.2287 << -0.2418 << -0.2513 << -0.2516 << -0.2236 << -0.2229 << -0.22 << -0.2284 << -0.2318 << -0.2337 << -0.2392 << -0.2398 << -0.2417 << -0.245 << -0.2389 << -0.2514 << -0.2541 << -0.2593 << -0.2586;

	QVector<double> alfa1MG675Vector;
	alfa1MG675Vector  << 9.0138 << 9.0138 << 9.0408 << 9.1338 << 7.9837 << 7.756 << 9.4252 << 9.6242 << 11.13 << 11.3629 << 11.7818 << 11.6097 << 11.4484 << 10.9065 << 9.8565 << 8.3363 << 6.8656 << 4.1178 << 1.8102 << 0.0977 << -3.0563 << -4.4387;
	QVector<double> alfa2MG675Vector;
	alfa2MG675Vector  << -0.0794 << -0.0794 << -0.0794 << -0.0794 << -0.1923 << -0.1614 << -0.1887 << -0.0665 << -0.1698 << -0.1766 << -0.2798 << -0.3048 << -0.2911 << -0.3097 << -0.2565 << -0.232 << -0.1226 << 0.1724 << 0.3001 << 0.4609 << 0.6948 << 0.8393;

	QVector<double> beta1MG675Vector;
	beta1MG675Vector  << 2.9935 << 2.9935 << 2.9935 << 2.9935 << 2.7995 << 2.8143 << 2.8131 << 2.4091 << 2.4938 << 2.3773 << 2.3772 << 2.3413 << 2.3477 << 2.2042 << 2.1493 << 2.0408 << 2.0013 << 1.9408 << 1.7763 << 1.703 << 1.5212 << 1.4195;
	QVector<double> beta2MG675Vector;
	beta2MG675Vector  << -0.2287 << -0.2287 << -0.2287 << -0.2287 << -0.2319 << -0.2326 << -0.2211 << -0.1676 << -0.1685 << -0.1531 << -0.1595 << -0.1594 << -0.1584 << -0.1577 << -0.1532 << -0.147 << -0.1439 << -0.1278 << -0.1326 << -0.1291 << -0.122 << -0.1145;

	QVector<double> alfa3Vector;
	alfa3Vector  << 0.0589 << 0.0589 << 0.0589 << 0.0589 << 0.0417 << 0.0527 << 0.0442 << 0.0329 << 0.0188 << 0.0095 << -0.0039 << -0.0133 << -0.0224 << -0.0267 << -0.0198 << -0.0367 << -0.0291 << -0.0214 << -0.024 << -0.0202 << -0.0219 << -0.0035;

	QVector<double> ksiVector;
	ksiVector  << -0.854 << -0.854 << -0.854 << -0.854 << -0.631 << -0.591 << -0.757 << -0.911 << -0.998 << -1.042 << -1.03 << -1.019 << -1.023 << -1.056 << -1.009 << -0.898 << -0.851 << -0.761 << -0.675 << -0.629 << -0.531 << -0.586;

	QVector<double> gammaVector;
	gammaVector  << -0.0027 << -0.0027 << -0.0027 << -0.0027 << -0.0061 << -0.0056 << -0.0042 << -0.0046 << -0.003 << -0.0028 << -0.0029 << -0.0028 << -0.0021 << -0.0029 << -0.0032 << -0.0033 << -0.0032 << -0.0031 << -0.0051 << -0.0059 << -0.0057 << -0.0061;

	QVector<double> phiVector;
	phiVector  << 0.08 << 0.08 << 0.08 << 0.08 << 0.08 << 0.08 << 0.08 << 0.08 << 0.08 << 0.08 << 0.08 << 0.08 << 0.08 << 0.08 << 0.06 << 0.04 << 0.02 << 0.02 << 0 << 0 << 0 << 0;

	


	double T;
	if (theResponseType == RIdriss2013IntensityModel::PGA) {
		T = 0.0;
	} 


	QVector<double> spectralResponseVector;
	for (int i = 0; i < locationList.count(); i++) {
		spectralResponseVector << 0.0;
	}

	for (int index = 0; index < theMagnitudeList.count(); index++) {
		
		RIdriss2013IntensityModel::RFaultType theFaultType;
		if (theFaultTypes.count() == 1) {
			theFaultType = theFaultTypes[0];
		} else {
			theFaultType = theFaultTypes[index];
		}

		double F = 0;
		if (theFaultType == ReverseSlip){
		F = 1;
		}

		// For convenience, look up the values of the parameters first
		double M = theMagnitudeList[index].data()->getCurrentValue(threadID);
		
		// IMPORTANT: If M = 0, it means that this hazard is off, so the resulting PGA is Zero
		if (M == 0.0) {
			continue;
		}
		if (getDisplayOutput()) {
			if (M > 8.0 || M < 5.0) {
				rCritical(threadID) << "Warning: The magnitude in the model" << objectName() << "should be between 5 and 8.";
			}
		}

		// Verification that the user has given an RLocationResponse
		RLocation *theHypocentreLocationObject = parameterToLocation(theHypocentreLocationList[index].data());  
		if (!theHypocentreLocationObject) {
			rCritical(threadID) << "Error: The location response" << theHypocentreLocationList[index].data()->objectName() << "does not include any location object.";
			return -1;
		}

		double Ztor;
		if (theDepthToTopOfRuptureList.count () == 1){
			Ztor = theDepthToTopOfRuptureList[0].data()->getCurrentValue(threadID);
		}else {
			Ztor = theDepthToTopOfRuptureList[index].data()->getCurrentValue(threadID);
		}
		if (Ztor == 999){
			if (F == 1) {
				Ztor = pow(std::max(2.704 - 1.226 * std::max((M - 5.849) , 0.0) , 0.0) ,2.0);
			} else {
				Ztor =pow(std::max(2.673 - 1.136 * std::max(M-4.970 , 0.0) , 0.0) , 2.0);
			}
		}


		for (int i = 0; i < locationList.count(); i++) {
			double Rjb = theHypocentreLocationObject->computeSurfaceDistance(locationList[i], threadID);
			if (Rjb > 200.0) {
				if (getDisplayOutput()) {
					rCritical(threadID) << "Warning: The maximum allowable distance in the model" << objectName() << "is 200 km.";
				}
			}

			if (theResponseType == RIdriss2013IntensityModel::Sa) {
				T = thePeriodList[i].data()->getCurrentValue(threadID);
			}
			if (T > periodTableVector.last() || T < periodTableVector.first()) {
				// RESTORE THE WARNING WHEN THE PAPER IS DONE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
				//rCritical(threadID) << "Warning: The period" << T << "from the parameter" << thePeriodList[i]->objectName() << "entered in the model" << objectName() << "is out of the range of" << periodTableVector[1] << "and" << periodTableVector.last() << "sec.";
				if (T > periodTableVector.last()) {
					T = periodTableVector.last();
				}
				//return -1;
			}

			
			double Vs30;
			if (theShearWaveVelocityList.count() == 1) {
				Vs30 = theShearWaveVelocityList[0].data()->getCurrentValue(threadID);
			} else {
				Vs30 = theShearWaveVelocityList[i].data()->getCurrentValue(threadID);
			}
			if (getDisplayOutput()) {
				if (Vs30 > 1200 || Vs30 < 450) {
					rCritical(threadID) << "Warning: The shear wave velocity in the model" << objectName() << "should be between 450 and 1200 m/s.";
				}
			}

			
		
			double Rrup;
			double irrup;
			if (theRupDistList.count() == 1){
				irrup = 0;
			}else{
				irrup = i;
			}
			if (theRupDistList.isEmpty() == 1 || theRupDistList[irrup].data()->getCurrentValue(threadID) == 999){
				Rrup = sqrt(Rjb * Rjb + Ztor * Ztor);
			} else {
				Rrup = theRupDistList[irrup].data()->getCurrentValue(threadID);
			}

			double epsilon;
			if (theEpsilonUncertaintyList.count() == 1) {
				epsilon = theEpsilonUncertaintyList[0].data()->getCurrentValue(threadID);
			} else {
				epsilon = theEpsilonUncertaintyList[i].data()->getCurrentValue(threadID);
			}
	

			int lowerIndex = periodTableVector.indexOf(T);
			double *upperIndexIterator;
			int upperIndex;
			if (lowerIndex == -1) {
				upperIndexIterator = qUpperBound(periodTableVector.begin(), periodTableVector.end(), T);
				upperIndex = periodTableVector.indexOf(*upperIndexIterator);
				lowerIndex = upperIndex - 1;
			} else {
				upperIndex = lowerIndex;
			}


			QVector<double> lnYVector;
			for (int j = lowerIndex; j <= upperIndex; j++) { // At max, there will be 2 iterations
				
				double alfa1;
				double alfa2;
				double beta1;
				double beta2;
				
				if (M <= 6.75){
				alfa1 = alfa1ML675Vector[j];
				alfa2 = alfa2ML675Vector[j];
				beta1 = beta1ML675Vector[j];
				beta2 = beta2ML675Vector[j];
				} else{
				alfa1 = alfa1MG675Vector[j];
				alfa2 = alfa2MG675Vector[j];
				beta1 = beta1MG675Vector[j];
				beta2 = beta2MG675Vector[j];
				}

				double alfa3 = alfa3Vector[j];
	
				double ksi = ksiVector[j];
	
				double gamma = gammaVector[j];
	
				double phi = phiVector[j];

				
				double lnPSA = alfa1 + alfa2 * M + alfa3 * (8.5 - M) * (8.5 - M) - (beta1 + beta2 * M) * log(Rrup + 10.0) + ksi * log(Vs30) + gamma * Rrup + phi * F; 

				double SE;
				double Ms;
				Ms = std::max(std::min(M , 7.50) , 5.0);
				if (T < 0.05){
					SE = 1.18 + 0.035 * log( 0.05) - 0.06 * Ms;
				}else if (T > 3) {
					SE = 1.18 + 0.035 * log( 3.0) - 0.06 * Ms;
				}else{
					SE = 1.18 + 0.035 * log( T) - 0.06 * Ms;
				}
				
				
				double lnY = lnPSA + SE * epsilon;
				lnYVector << lnY;
			}

			double lnY;
			if (lnYVector.count() == 1) {
				lnY = lnYVector[0];
			} else {//Is this right?////////////////////////////////////////////////////???????????????????????????????????????????????
				lnY = (lnYVector[1] - lnYVector[0]) / (1.0 / periodTableVector[upperIndex] - 1.0 / periodTableVector[lowerIndex]) * (1.0 / T - 1.0 / periodTableVector[lowerIndex]) + lnYVector[0];
			}

			double Y = exp(lnY); // The units of Y is g for Sa and PGA

			// SRSS the intensities from different sources if seismicity; Stage 1: Compute sum of squares
			spectralResponseVector[i] += gsl_pow_2(Y);
		}
	}

	// Update responses
	for (int i = 0; i < locationList.count(); i++) {
		// SRSS the intensities from different sources if seismicity; Stage 2: Compute the square root of sum of squares
		spectralResponseVector[i] = sqrt(spectralResponseVector[i]);
		theSpectralResponseList[i]->setCurrentValue(spectralResponseVector[i], threadID);
	}

	return 0;
}

bool RIdriss2013IntensityModel::canBeRunParallelly(){
	return true;
}