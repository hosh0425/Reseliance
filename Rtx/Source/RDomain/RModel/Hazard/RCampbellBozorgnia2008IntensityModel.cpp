#include "RCampbellBozorgnia2008IntensityModel.h"	

#include <QCoreApplication>
#include <QMetaEnum>
	
#include "RParameter.h"	
#include "RResponse.h"	
#include "RLocation.h"
#include "RLocationResponse.h"
#include "RMainWindow.h"

#include <math.h>

#include <gsl/gsl_math.h>
	
RCampbellBozorgnia2008IntensityModel::RCampbellBozorgnia2008IntensityModel(QObject *parent, QString name)	
	: RModel(parent, name)
{	
	//theEpsilon = 0;
	theResponseType = RCampbellBozorgnia2008IntensityModel::Sa;
}	
	
RCampbellBozorgnia2008IntensityModel::~RCampbellBozorgnia2008IntensityModel()	
{	
	
}	

QString RCampbellBozorgnia2008IntensityModel::getMagnitudeList()	
{	
	return parameterListToString(theMagnitudeList);
}	

void RCampbellBozorgnia2008IntensityModel::setMagnitudeList(QString value)	
{	
	theMagnitudeList = stringToParameterList(value);
}

QString RCampbellBozorgnia2008IntensityModel::getHypocentreLocationList()	
{	
	return parameterListToString(theHypocentreLocationList);
}	

void RCampbellBozorgnia2008IntensityModel::setHypocentreLocationList(QString value)	
{	
	theHypocentreLocationList = stringToParameterList(value);

	// Checking if all the passed parameters are either RLocation or RLocationResponse
	parameterListToLocationList(theHypocentreLocationList);
}

QString RCampbellBozorgnia2008IntensityModel::getEpsilonUncertaintyList()	
{	
	return parameterListToString(theEpsilonUncertaintyList);
}	
	
void RCampbellBozorgnia2008IntensityModel::setEpsilonUncertaintyList(QString value)	
{	
	theEpsilonUncertaintyList = stringToParameterList(value);

	createResponses();
}	

QString RCampbellBozorgnia2008IntensityModel::getEtaUncertaintyList()	
{	
	return parameterListToString(theEtaUncertaintyList);
}	
	
void RCampbellBozorgnia2008IntensityModel::setEtaUncertaintyList(QString value)	
{	
	theEtaUncertaintyList = stringToParameterList(value);

	createResponses();
}	


QList<RParameter *> RCampbellBozorgnia2008IntensityModel::getPhysicalParameterList()
{
	QList<RParameter *> parmList;
	for (int i = 0; i < theEpsilonUncertaintyList.count(); i++){
		parmList << theEpsilonUncertaintyList[i].data();
	}
	for (int i = 0; i < theEtaUncertaintyList.count(); i++){	
		parmList <<  theEtaUncertaintyList[i].data();
	}
	return parmList;
}

QString RCampbellBozorgnia2008IntensityModel::getFaultTypes() const	
{
	QMetaEnum metaEnum = metaObject()->enumerator(metaObject()->indexOfEnumerator("RFaultType"));
	QString result = "";
	for (int i = 0; i < theFaultTypes.count(); i++) {
		QString temp = metaEnum.valueToKey(theFaultTypes[i]);
	    result += temp + "; ";
	}
	return result;
}	
	
void RCampbellBozorgnia2008IntensityModel::setFaultTypes(QString value)	
{	
	QString string = value;
	QMetaEnum metaEnum = metaObject()->enumerator(metaObject()->indexOfEnumerator("RFaultType"));

	QRegExp regExp = QRegExp(QString("[ ,;:\t\n\v\r%1%2]").arg(QChar(QChar::ParagraphSeparator)).arg(QChar(QChar::LineSeparator)));
	QStringList stringList = string.split(regExp, QString::SkipEmptyParts);
	string = "";
	QList<RCampbellBozorgnia2008IntensityModel::RFaultType> tempList;
	for (int i = 0; i < stringList.count(); i++) {
		QByteArray ba = stringList[i].toLocal8Bit();
		const char *tempChar = ba.data();
		int tempValue = metaEnum.keyToValue(tempChar);
		RCampbellBozorgnia2008IntensityModel::RFaultType temp = RFaultType(tempValue);
		if (temp < 0) {
			rCritical() << "Error in the Model" << objectName() << ": The fault type" << stringList[i] << "is not valid";
			return;
		}
		tempList.append(temp);
	}
	theFaultTypes = tempList;
}	

RCampbellBozorgnia2008IntensityModel::RResponseType RCampbellBozorgnia2008IntensityModel::getResponseType() const	
{	
	return theResponseType;
}	
	
void RCampbellBozorgnia2008IntensityModel::setResponseType(RCampbellBozorgnia2008IntensityModel::RResponseType value)	
{	
	theResponseType = value;
}

QString RCampbellBozorgnia2008IntensityModel::getPeriodList()	
{	
	return parameterListToString(thePeriodList);
}	
	
void RCampbellBozorgnia2008IntensityModel::setPeriodList(QString value)	
{	
	thePeriodList = stringToParameterList(value);

	createResponses();
}	
	
QString RCampbellBozorgnia2008IntensityModel::getStructureLocationList()	
{	
	return parameterListToString(theStructureLocationList);
}	
	
void RCampbellBozorgnia2008IntensityModel::setStructureLocationList(QString value)	
{	
	theStructureLocationList = stringToParameterList(value);

	// Checking if all the passed parameters are either RLocation or RLocationResponse
	parameterListToLocationList(theStructureLocationList);

	createResponses();
}	
	
QString RCampbellBozorgnia2008IntensityModel::getShearWaveVelocityList()	
{	
	return parameterListToString(theShearWaveVelocityList);
}	
	
void RCampbellBozorgnia2008IntensityModel::setShearWaveVelocityList(QString value)	
{	
	theShearWaveVelocityList = stringToParameterList(value);

	createResponses();
}	

void RCampbellBozorgnia2008IntensityModel::createResponses()
{
	// Removing old responses
	while (theSpectralResponseList.count() > 0) {
		delete theSpectralResponseList.takeAt(0);
	}

	if (theStructureLocationList.isEmpty() || theShearWaveVelocityList.isEmpty()) {
		return;
	}

	if ((theStructureLocationList.count() != thePeriodList.count() && theResponseType == RCampbellBozorgnia2008IntensityModel::Sa) || (theStructureLocationList.count() != theShearWaveVelocityList.count() && theShearWaveVelocityList.count() > 1) || (theStructureLocationList.count() != theEpsilonUncertaintyList.count() && theEpsilonUncertaintyList.count() > 1) || (theStructureLocationList.count() != theEtaUncertaintyList.count() && theEtaUncertaintyList.count() > 1)) {
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

int RCampbellBozorgnia2008IntensityModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
{	
	if (theStructureLocationList.isEmpty()) {
		rCritical(threadID) << "Error: There should be at least one structure location in the model" << objectName() << ".";
		return -1;
	}
	if ((theStructureLocationList.count() != thePeriodList.count() && theResponseType == RCampbellBozorgnia2008IntensityModel::Sa) || (theStructureLocationList.count() != theShearWaveVelocityList.count() && theShearWaveVelocityList.count() > 1)) {
		rCritical(threadID) << "Error: The number of input parameters for the period list, structure location list, and shear wave velocity list are not equal in the model" << objectName() << ".";
		return -1;
	}
	
	// Checking if the number of magnitudes is equal to the number of hypocentre locations
	if (theHypocentreLocationList.count() != theMagnitudeList.count()) {
		rCritical(threadID) << "Error: The number of magnitude parameters should be equal to the number of hypocentre locations in the model" << objectName() << ".";
		return -1;
	}


	// Converting the list of parameters to the list of locations
	QList<RLocation *> locationList = parameterListToLocationList(theStructureLocationList);


	

	// Period dependent coefficients
	QVector<double> periodTableVector;
	QVector<double> c0Vector;
	QVector<double> c1Vector;
	QVector<double> c2Vector;
	QVector<double> c3Vector;
	QVector<double> c4Vector;
	QVector<double> c5Vector;
	QVector<double> c6Vector;
	QVector<double> c7Vector;
	QVector<double> c8Vector;
	QVector<double> c9Vector;
	QVector<double> c10Vector;
	QVector<double> c11Vector;
	QVector<double> c12Vector;
	QVector<double> k1Vector;
	QVector<double> k2Vector;
	QVector<double> k3Vector;
	QVector<double> sigmalnyVector;
	QVector<double> sigmaTyVector;
	QVector<double> sigmacVector;
	QVector<double> rosVector;
	QVector<double> rotVector;
	periodTableVector << -2.00 << -1.00 << 0.00 << 0.01 << 0.02 << 0.03 << 0.05 << .075 << 0.1 << 0.15 << 0.2 << 0.25 << 0.3 << 0.4 << 0.5 << 0.75 << 1 << 1.5 << 2 << 3 << 4 << 5 << 7.5 << 10 << 999;
	c0Vector << -5.27 << 0.954 << -1.715 << -1.715 << -1.68 << -1.552 << -1.209 << -0.657 << -0.314 << -0.133 << -0.486 << -0.89 << -1.171 << -1.466 << -2.569 << -4.844 << -6.406 << -8.692 << -9.701 << -10.556 << -11.212 << -11.684 << -12.505 << -13.087 << -4.354;
	c1Vector << 1.6 << 0.696 << 0.5 << 0.5 << 0.5 << 0.5 << 0.5 << 0.5 << 0.5 << 0.5 << 0.5 << 0.5 << 0.5 << 0.5 << 0.656 << 0.972 << 1.196 << 1.513 << 1.6 << 1.6 << 1.6 << 1.6 << 1.6 << 1.6 << 0.942;
	c2Vector << -0.07 << -0.309 << -0.53 << -0.53 << -0.53 << -0.53 << -0.53 << -0.53 << -0.53 << -0.53 << -0.446 << -0.362 << -0.294 << -0.186 << -0.304 << -0.578 << -0.772 << -1.046 << -0.978 << -0.638 << -0.316 << -0.07 << -0.07 << -0.07 << -0.178;
	c3Vector << 0 << -0.019 << -0.262 << -0.262 << -0.262 << -0.262 << -0.267 << -0.302 << -0.324 << -0.339 << -0.398 << -0.458 << -0.511 << -0.592 << -0.536 << -0.406 << -0.314 << -0.185 << -0.236 << -0.491 << -0.77 << -0.986 << -0.656 << -0.422 << -0.346;
	c4Vector << -2 << -2.016 << -2.118 << -2.118 << -2.123 << -2.145 << -2.199 << -2.277 << -2.318 << -2.309 << -2.22 << -2.146 << -2.095 << -2.066 << -2.041 << -2 << -2 << -2 << -2 << -2 << -2 << -2 << -2 << -2 << -1.309;
	c5Vector << 0.17 << 0.17 << 0.17 << 0.17 << 0.17 << 0.17 << 0.17 << 0.17 << 0.17 << 0.17 << 0.17 << 0.17 << 0.17 << 0.17 << 0.17 << 0.17 << 0.17 << 0.17 << 0.17 << 0.17 << 0.17 << 0.17 << 0.17 << 0.17 << 0.087;
	c6Vector << 4 << 4 << 5.6 << 5.6 << 5.6 << 5.6 << 5.74 << 7.09 << 8.05 << 8.79 << 7.6 << 6.58 << 6.04 << 5.3 << 4.73 << 4 << 4 << 4 << 4 << 4 << 4 << 4 << 4 << 4 << 7.24;
	c7Vector << 0 << 0.245 << 0.28 << 0.28 << 0.28 << 0.28 << 0.28 << 0.28 << 0.28 << 0.28 << 0.28 << 0.28 << 0.28 << 0.28 << 0.28 << 0.28 << 0.255 << 0.161 << 0.094 << 0 << 0 << 0 << 0 << 0 << 0.111;
	c8Vector << 0 << 0 << -0.12 << -0.12 << -0.12 << -0.12 << -0.12 << -0.12 << -0.099 << -0.048 << -0.012 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << -0.108;
	c9Vector << 0 << 0.358 << 0.49 << 0.49 << 0.49 << 0.49 << 0.49 << 0.49 << 0.49 << 0.49 << 0.49 << 0.49 << 0.49 << 0.49 << 0.49 << 0.49 << 0.49 << 0.49 << 0.371 << 0.154 << 0 << 0 << 0 << 0 << 0.362;
	c10Vector << -0.82 << 1.694 << 1.058 << 1.058 << 1.102 << 1.174 << 1.272 << 1.438 << 1.604 << 1.928 << 2.194 << 2.351 << 2.46 << 2.587 << 2.544 << 2.133 << 1.571 << 0.406 << -0.456 << -0.82 << -0.82 << -0.82 << -0.82 << -0.82 << 2.549;
	c11Vector << 0.3 << 0.092 << 0.04 << 0.04 << 0.04 << 0.04 << 0.04 << 0.04 << 0.04 << 0.04 << 0.04 << 0.04 << 0.04 << 0.04 << 0.04 << 0.077 << 0.15 << 0.253 << 0.3 << 0.3 << 0.3 << 0.3 << 0.3 << 0.3 << 0.090;
	c12Vector << 1 << 1 << 0.61 << 0.61 << 0.61 << 0.61 << 0.61 << 0.61 << 0.61 << 0.61 << 0.61 << 0.7 << 0.75 << 0.85 << 0.883 << 1 << 1 << 1 << 1 << 1 << 1 << 1 << 1 << 1 << 1.277;
	k1Vector << 400 << 400 << 865 << 865 << 865 << 908 << 1054 << 1086 << 1032 << 878 << 748 << 654 << 587 << 503 << 457 << 410 << 400 << 400 << 400 << 400 << 400 << 400 << 400 << 400 << 400;
	k2Vector << 0 << -1.955 << -1.186 << -1.186 << -1.219 << -1.273 << -1.346 << -1.471 << -1.624 << -1.931 << -2.188 << -2.381 << -2.518 << -2.657 << -2.669 << -2.401 << -1.955 << -1.025 << -0.299 << 0 << 0 << 0 << 0 << 0 << -2.690;
	k3Vector << 2.744 << 1.929 << 1.839 << 1.839 << 1.84 << 1.841 << 1.843 << 1.845 << 1.847 << 1.852 << 1.856 << 1.861 << 1.865 << 1.874 << 1.883 << 1.906 << 1.929 << 1.974 << 2.019 << 2.11 << 2.2 << 2.291 << 2.517 << 2.744 << 1.0;
	sigmalnyVector << 0.667 << 0.484 << 0.478 << 0.478 << 0.48 << 0.489 << 0.51 << 0.52 << 0.531 << 0.532 << 0.534 << 0.534 << 0.544 << 0.541 << 0.55 << 0.568 << 0.568 << 0.564 << 0.571 << 0.558 << 0.576 << 0.601 << 0.628 << 0.667 << 0.371;
	sigmaTyVector << 0.485 << 0.203 << 0.219 << 0.219 << 0.219 << 0.235 << 0.258 << 0.292 << 0.286 << 0.28 << 0.249 << 0.24 << 0.215 << 0.217 << 0.214 << 0.227 << 0.255 << 0.296 << 0.296 << 0.326 << 0.297 << 0.359 << 0.428 << 0.485 << 0.196;
	sigmacVector << 0.29 << 0.19 << 0.166 << 0.166 << 0.166 << 0.165 << 0.162 << 0.158 << 0.17 << 0.18 << 0.186 << 0.191 << 0.198 << 0.206 << 0.208 << 0.221 << 0.225 << 0.222 << 0.226 << 0.229 << 0.237 << 0.237 << 0.271 << 0.29 << 0.089;
	rosVector << 1 << 0.999 << 0.989 << 0.963 << 0.922 << 0.898 << 0.89 << 0.871 << 0.852 << 0.831 << 0.785 << 0.735 << 0.628 << 0.534 << 0.411 << 0.331 << 0.289 << 0.261 << 0.2 << 0.174 << 0.174 << 1 << 0.691 << 0.174 << 0.735;
	rotVector << 1 << 0.994 << 0.979 << 0.927 << 0.88 << 0.871 << 0.885 << 0.913 << 0.873 << 0.848 << 0.756 << 0.631 << 0.442 << 0.29 << 0.29 << 0.29 << 0.29 << 0.29 << 0.29 << 0.29 << 0.29 << 1 << 0.538 << 0.29 << 0.0;
	double c = 1.88;
	double n = 1.18;

	
	double T;
	if (theResponseType == RCampbellBozorgnia2008IntensityModel::PGA) {
		T = 0.0;
	} else if (theResponseType == RCampbellBozorgnia2008IntensityModel::PGV) {
		T = -1.0;
	} else if (theResponseType == RCampbellBozorgnia2008IntensityModel::PGD) {
		T = -2.0;
	} else if (theResponseType == RCampbellBozorgnia2008IntensityModel::CAV) {
		T = 999;
	}


	QVector<double> spectralResponseVector;
	for (int i = 0; i < locationList.count(); i++) {
		spectralResponseVector << 0.0;
	}

	for (int index = 0; index < theMagnitudeList.count(); index++) {
	double FN = 0.0;
	double FR = 0.0;

	RCampbellBozorgnia2008IntensityModel::RFaultType theFaultType;
		if (theFaultTypes.count() == 1) {
			theFaultType = theFaultTypes[0];
		} else {
			theFaultType = theFaultTypes[index];
		}

	if (theFaultType == RCampbellBozorgnia2008IntensityModel::StrikeSlip) {
		FN = 0;
		FR = 0;
	} else if (theFaultType == RCampbellBozorgnia2008IntensityModel::NormalSlip) {
		FN = 1.0;
	} else if (theFaultType == RCampbellBozorgnia2008IntensityModel::ReverseSlip) {
		FR = 1.0;
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



		for (int i = 0; i < locationList.count(); i++) {
			double Rjb = theHypocentreLocationObject->computeSurfaceDistance(locationList[i],threadID);
			if (Rjb > 200.0) {
				if (getDisplayOutput()) {
					rCritical(threadID) << "Warning: The maximum allowable distance in the model" << objectName() << "is 200 km.";
				}
			}

			if (theResponseType == RCampbellBozorgnia2008IntensityModel::Sa) {
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
				if (Vs30 > 1300.0 || Vs30 < 180) {
					rCritical(threadID) << "Warning: The shear wave velocity in the model" << objectName() << "should be between 180 and 1300 m/s.";
				}
			}

			double eta;
			if (theEtaUncertaintyList.count() == 1) {
				eta = theEtaUncertaintyList[0].data()->getCurrentValue(threadID);
			} else {
				eta = theEtaUncertaintyList[i].data()->getCurrentValue(threadID);
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
				double c0 = c0Vector[j];
				double c1 = c1Vector[j];
				double c2 = c2Vector[j];
				double c3 = c3Vector[j];
				double c4 = c4Vector[j];
				double c5 = c5Vector[j];
				double c6 = c6Vector[j];
				double c7 = c7Vector[j];
				double c8 = c8Vector[j];
				double c9 = c9Vector[j];
				double c10 = c10Vector[j];
				double c11 = c11Vector[j];
				double c12 = c12Vector[j];
				double k1 = k1Vector[j];
				double k2 = k2Vector[j];
				double k3 = k3Vector[j];
				double sigmalny = sigmalnyVector[j];
				double sigmaTy = sigmaTyVector[j];
				double sigmac = sigmacVector[j];
				double ros = rosVector[j];
				double rot = rotVector[j];

				double Ztop ;
				
				if ( FN==0 && FR==0 ){
					Ztop = 5.63 + 0.68 * M ;
				}else if ( FN==1 || FR==1 ){
					Ztop = 11.24 - 0.2 * M ;
				}
				

				double R = sqrt( Rjb * Rjb + Ztop * Ztop) ;
				
				double FM ;
				if (M <= 5.5){
					FM = c0 + c1 * M;
				}else if (M > 5.5 && M<= 6.5){
					FM = c0 + c1 * M + c2 * ( M - 5.5) ;
				}else if (M > 6.5){
					FM = c0 + c1 * M + c2 * ( M - 5.5 ) + c3 * (M - 6.5) ;
				}
				if	(getDisplayOutput())	{				
					rDebug(threadID)	<<	"FM:"	<<	FM	<<" "	 ;
				}
				double FD =(c4 + c5 * M ) * log(sqrt( R * R + c6 * c6)) ; 
				if	(getDisplayOutput())	{				
					rDebug(threadID)	<<	"FD:"	<<	FD	<<" "	 ;
				}
				double Fl ;
				if (Ztop < 1){
					Fl = Ztop;
				}else if (Ztop >= 1){
					Fl = 1.0;
				}
				double FF =c7 * FR * Fl + c8 * FN ; 
				
				double lnY = FM + FD + FF;
				if (getDisplayOutput()) {
					rDebug(threadID) << "Fmag " << FM  << " .";
				}
				if (getDisplayOutput()) {
					rDebug(threadID) << "FDis " <<  FD  << " .";
				}
				if (getDisplayOutput()) {
					rDebug(threadID) << "Fflt" << FF  << " .";
				}


				
				double A1100;
				{
					
					double c0 = c0Vector[2];
					double c1 = c1Vector[2];
					double c2 = c2Vector[2];
					double c3 = c3Vector[2];
					double c4 = c4Vector[2];
					double c5 = c5Vector[2];
					double c6 = c6Vector[2];
					double c7 = c7Vector[2];
					double c8 = c8Vector[2];
					double c9 = c9Vector[2];
					double c10 = c10Vector[2];
					double c11 = c11Vector[2];
					double c12 = c12Vector[2];
					double k1 = k1Vector[2];
					double k2 = k2Vector[2];
					double k3 = k3Vector[2];

					if (M <= 5.5){
						FM = c0 + c1 * M;
					}else if (M > 5.5 && M<= 6.5){
						FM = c0 + c1 * M + c2 * (M-5.5) ;
					}else if (M > 6.5){
						FM = c0 + c1 * M + c2 * (M-5.5) + c3 * (M - 6.5) ;
					}
			
					double FD =(c4 + c5 *M) * log(sqrt(R*R + c6 * c6)) ; 
					

					A1100 = exp( FM + FD );
				}
				 c0 = c0Vector[j];
				 c1 = c1Vector[j];
				 c2 = c2Vector[j];
				 c3 = c3Vector[j];
				 c4 = c4Vector[j];
				 c5 = c5Vector[j];
				 c6 = c6Vector[j];
				 c7 = c7Vector[j];
				 c8 = c8Vector[j];
				 c9 = c9Vector[j];
				 c10 = c10Vector[j];
				 c11 = c11Vector[j];
				 c12 = c12Vector[j];
				 k1 = k1Vector[j];
				 k2 = k2Vector[j];
				 k3 = k3Vector[j];
				
				double FS ; 
				if (Vs30 < k1){
					FS = c10 * log(Vs30/k1) + k2 *( log(A1100+ c * pow((Vs30/k1) , n)) -log( A1100 + c));
				}else if( Vs30 >=k1 && Vs30<1100){
					FS = (c10 + k2 *n) * log(Vs30/k1);
				}else if (Vs30 >= 1100 ){
					FS = (c10 + k2 *n) * log(1100/k1);
				}
				
				if	(getDisplayOutput())	{			
					rDebug(threadID)	<<	"Fsite:"	<<	FS	<<" "	 ;
				}
				double sA =sigmalnyVector[2];
				double TuA=sigmaTyVector[2];
				double sAb = sqrt( sA * sA - 0.3 * 0.3);
				double sb  = sqrt( sigmalny * sigmalny - 0.3 * 0.3);

				double alfa ;
				if (Vs30 < k1){
					alfa =  k2 * A1100 *(pow(( A1100 + c * pow((Vs30/k1) , n)) ,-1.0 )- pow((A1100+c),-1.0));
				}else if( Vs30 >=k1){
					alfa = 0;
				}

				double stdvs;
				if (theResponseType == RCampbellBozorgnia2008IntensityModel::CAV) {

					stdvs = sigmalny*sigmalny + alfa * alfa * sAb * sAb + 2 * alfa * ros * sb * sAb + 0.3 * 0.3;
				}
				else {
					stdvs = sigmalny*sigmalny + alfa * alfa * sAb * sAb + 2 * alfa * ros * sb * sAb;
				}
				double stdvT;
				if (theResponseType == RCampbellBozorgnia2008IntensityModel::CAV) {
					stdvT = sigmaTy * sigmaTy;
				}
				else {
					stdvT = sigmaTy * sigmaTy + alfa * alfa * TuA * TuA + 2 * alfa * rot * sigmaTy * TuA;
				}

				lnY = lnY + FS + sqrt(stdvs) * epsilon + eta * sqrt(stdvT);
				lnYVector << lnY;
			}

			double lnY;
			if (lnYVector.count() == 1) {
				lnY = lnYVector[0];
			} else {//Is this right?////////////////////////////////////////////////////???????????????????????????????????????????????
				lnY = (lnYVector[1] - lnYVector[0]) / (1.0 / periodTableVector[upperIndex] - 1.0 / periodTableVector[lowerIndex]) * (1.0 / T - 1.0 / periodTableVector[lowerIndex]) + lnYVector[0];
			}

			double Y = exp(lnY); // The units of Y is g for Sa and PGA, and cm/s for PGV

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

bool RCampbellBozorgnia2008IntensityModel::canBeRunParallelly(){
	return true;
}