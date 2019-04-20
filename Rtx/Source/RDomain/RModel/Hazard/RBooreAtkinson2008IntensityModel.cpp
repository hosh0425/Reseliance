#include "RBooreAtkinson2008IntensityModel.h"	

#include <QCoreApplication>
#include <QMetaEnum>
	
#include "RParameter.h"	
#include "RResponse.h"	
#include "RLocation.h"
#include "RLocationResponse.h"
#include "RMainWindow.h"

#include <math.h>

#include <gsl/gsl_math.h>
	
RBooreAtkinson2008IntensityModel::RBooreAtkinson2008IntensityModel(QObject *parent, QString name)	
	: RModel(parent, name)
{	
	//theEpsilon = 0;
	theResponseType = RBooreAtkinson2008IntensityModel::Sa;
	theSmoothness = RBooreAtkinson2008IntensityModel::Smoothed;
}	
	
RBooreAtkinson2008IntensityModel::~RBooreAtkinson2008IntensityModel()	
{	
	
}	

QString RBooreAtkinson2008IntensityModel::getMagnitudeList()	
{	
	return parameterListToString(theMagnitudeList);
}	

void RBooreAtkinson2008IntensityModel::setMagnitudeList(QString value)	
{	
	theMagnitudeList = stringToParameterList(value);
}

QString RBooreAtkinson2008IntensityModel::getHypocentreLocationList()	
{	
	return parameterListToString(theHypocentreLocationList);
}	

void RBooreAtkinson2008IntensityModel::setHypocentreLocationList(QString value)	
{	
	theHypocentreLocationList = stringToParameterList(value);

	// Checking if all the passed parameters are either RLocation or RLocationResponse
	parameterListToLocationList(theHypocentreLocationList);
}

QString RBooreAtkinson2008IntensityModel::getEpsilonUncertaintyList()	
{	
	return parameterListToString(theEpsilonUncertaintyList);
}	
	
void RBooreAtkinson2008IntensityModel::setEpsilonUncertaintyList(QString value)	
{	
	theEpsilonUncertaintyList = stringToParameterList(value);

	createResponses();
}	

QString RBooreAtkinson2008IntensityModel::getEtaUncertaintyList()	
{	
	return parameterListToString(theEtaUncertaintyList);
}	
	
void RBooreAtkinson2008IntensityModel::setEtaUncertaintyList(QString value)	
{	
	theEtaUncertaintyList = stringToParameterList(value);

	createResponses();
}	


QList<RParameter *> RBooreAtkinson2008IntensityModel::getPhysicalParameterList()
{
	QList<RParameter *> parmList;
	for (int i = 0; i < theEpsilonUncertaintyList.count(); i++){
		parmList << theEpsilonUncertaintyList[i].data()   ;
	}
	for (int i = 0; i < theEtaUncertaintyList.count(); i++){	
		parmList <<  theEtaUncertaintyList[i].data()   ;
	}
	return parmList;
}

QString RBooreAtkinson2008IntensityModel::getFaultTypes() const	
{
	QMetaEnum metaEnum = metaObject()->enumerator(metaObject()->indexOfEnumerator("RFaultType"));
	QString result = "";
	for (int i = 0; i < theFaultTypes.count(); i++) {
		QString temp = metaEnum.valueToKey(theFaultTypes[i]);
	    result += temp + "; ";
	}
	return result;
}	
	
void RBooreAtkinson2008IntensityModel::setFaultTypes(QString value)	
{	
	QString string = value;
	QMetaEnum metaEnum = metaObject()->enumerator(metaObject()->indexOfEnumerator("RFaultType"));

	QRegExp regExp = QRegExp(QString("[ ,;:\t\n\v\r%1%2]").arg(QChar(QChar::ParagraphSeparator)).arg(QChar(QChar::LineSeparator)));
	QStringList stringList = string.split(regExp, QString::SkipEmptyParts);
	string = "";
	QList<RBooreAtkinson2008IntensityModel::RFaultType> tempList;
	for (int i = 0; i < stringList.count(); i++) {
		QByteArray ba = stringList[i].toLocal8Bit();
		const char *tempChar = ba.data();
		int tempValue = metaEnum.keyToValue(tempChar);
		RBooreAtkinson2008IntensityModel::RFaultType temp = RFaultType(tempValue);
		if (temp < 0) {
			rCritical() << "Error in the Model" << objectName() << ": The fault type" << stringList[i] << "is not valid";
			return;
		}
		tempList.append(temp);
	}
	theFaultTypes = tempList;
}	

RBooreAtkinson2008IntensityModel::RResponseType RBooreAtkinson2008IntensityModel::getResponseType() const	
{	
	return theResponseType;
}	
	
void RBooreAtkinson2008IntensityModel::setResponseType(RBooreAtkinson2008IntensityModel::RResponseType value)	
{	
	theResponseType = value;
}

RBooreAtkinson2008IntensityModel::RSmoothness RBooreAtkinson2008IntensityModel::getSmoothness() const	
{	
	return theSmoothness;
}	
	
void RBooreAtkinson2008IntensityModel::setSmoothness(RBooreAtkinson2008IntensityModel::RSmoothness value)	
{	
	theSmoothness = value;
}

QString RBooreAtkinson2008IntensityModel::getPeriodList()	
{	
	return parameterListToString(thePeriodList);
}	
	
void RBooreAtkinson2008IntensityModel::setPeriodList(QString value)	
{	
	thePeriodList = stringToParameterList(value);

	createResponses();
}	
	
QString RBooreAtkinson2008IntensityModel::getStructureLocationList()	
{	
	return parameterListToString(theStructureLocationList);
}	
	
void RBooreAtkinson2008IntensityModel::setStructureLocationList(QString value)	
{	
	theStructureLocationList = stringToParameterList(value);

	// Checking if all the passed parameters are either RLocation or RLocationResponse
	parameterListToLocationList(theStructureLocationList);

	createResponses();
}	
	
QString RBooreAtkinson2008IntensityModel::getShearWaveVelocityList()	
{	
	return parameterListToString(theShearWaveVelocityList);
}	
	
void RBooreAtkinson2008IntensityModel::setShearWaveVelocityList(QString value)	
{	
	theShearWaveVelocityList = stringToParameterList(value);

	createResponses();
}	

void RBooreAtkinson2008IntensityModel::createResponses()
{
	// Removing old responses
	while (theSpectralResponseList.count() > 0) {
		delete theSpectralResponseList.takeAt(0);
	}

	if (theStructureLocationList.isEmpty() || theShearWaveVelocityList.isEmpty()) {
		return;
	}

	if ((theStructureLocationList.count() != thePeriodList.count() && theResponseType == RBooreAtkinson2008IntensityModel::Sa) || (theStructureLocationList.count() != theShearWaveVelocityList.count() && theShearWaveVelocityList.count() > 1) || (theStructureLocationList.count() != theEpsilonUncertaintyList.count() && theEpsilonUncertaintyList.count() > 1) || (theStructureLocationList.count() != theEtaUncertaintyList.count() && theEtaUncertaintyList.count() > 1)) {
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

int RBooreAtkinson2008IntensityModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
{	
	if (theStructureLocationList.isEmpty()) {
		rCritical(threadID) << "Error: There should be at least one structure location in the model" << objectName() << ".";
		return -1;
	}
	if ((theStructureLocationList.count() != thePeriodList.count() && theResponseType == RBooreAtkinson2008IntensityModel::Sa) || (theStructureLocationList.count() != theShearWaveVelocityList.count() && theShearWaveVelocityList.count() > 1)) {
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
	double Vref = 760; // m/s
	double V1 = 180; // m/s
	double V2 = 300; // m/s
	double a1 = 0.03; // g
	double a2 = 0.09; // g
	double pga_low = 0.06; // g
	double deltaX = log(a2 / a1);
	double Mref = 4.5;
	double Rref = 1.0; // km

	// Period dependent coefficients
	QVector<double> periodTableVector;
	periodTableVector << -1 << 0 << 0.01 << 0.02 << 0.03 << 0.05 << 0.075 << 0.1 << 0.15 << 0.2 << 0.25 << 0.3 << 0.4 << 0.5 << 0.75 << 1 << 1.5 << 2 << 3 << 4 << 5 << 7.5 << 10;

	QVector<double> blinVector;
	blinVector << -0.6 << -0.36 << -0.36 << -0.34 << -0.33 << -0.29 << -0.23 << -0.25 << -0.28 << -0.31 << -0.39 << -0.44 << -0.5 << -0.6 << -0.69 << -0.7 << -0.72 << -0.73 << -0.74 << -0.75 << -0.75 << -0.692 << -0.65;
	QVector<double> b1Vector;
	b1Vector << -0.5 << -0.64 << -0.64 << -0.63 << -0.62 << -0.64 << -0.64 << -0.6 << -0.53 << -0.52 << -0.52 << -0.52 << -0.51 << -0.5 << -0.47 << -0.44 << -0.4 << -0.38 << -0.34 << -0.31 << -0.291 << -0.247 << -0.215;
	QVector<double> b2Vector;
	b2Vector << -0.06 << -0.14 << -0.14 << -0.12 << -0.11 << -0.11 << -0.11 << -0.13 << -0.18 << -0.19 << -0.16 << -0.14 << -0.1 << -0.06 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0;

	QVector<double> c1Vector;
	c1Vector << -0.8737 << -0.6605 << -0.6622 << -0.666 << -0.6901 << -0.717 << -0.7205 << -0.7081 << -0.6961 << -0.583 << -0.5726 << -0.5543 << -0.6443 << -0.6914 << -0.7408 << -0.8183 << -0.8303 << -0.8285 << -0.7844 << -0.6854 << -0.5096 << -0.3724 << -0.09824;
	QVector<double> c2Vector;
	c2Vector << 0.1006 << 0.1197 << 0.12 << 0.1228 << 0.1283 << 0.1317 << 0.1237 << 0.1117 << 0.09884 << 0.04273 << 0.02977 << 0.01955 << 0.04394 << 0.0608 << 0.07518 << 0.1027 << 0.09793 << 0.09432 << 0.07282 << 0.03758 << -0.02391 << -0.06568 << -0.138;
	QVector<double> c3Vector;
	c3Vector << -0.00334 << -0.01151 << -0.01151 << -0.01151 << -0.01151 << -0.01151 << -0.01151 << -0.01151 << -0.01113 << -0.00952 << -0.00837 << -0.0075 << -0.00626 << -0.0054 << -0.00409 << -0.00334 << -0.00255 << -0.00217 << -0.00191 << -0.00191 << -0.00191 << -0.00191 << -0.00191;
	QVector<double> hVector;
	hVector << 2.54 << 1.35 << 1.35 << 1.35 << 1.35 << 1.35 << 1.55 << 1.68 << 1.86 << 1.98 << 2.07 << 2.14 << 2.24 << 2.32 << 2.46 << 2.54 << 2.66 << 2.73 << 2.83 << 2.89 << 2.93 << 3 << 3.04;

	QVector<double> e1Vector;
	e1Vector << 5.00121 << -0.53804 << -0.52883 << -0.52192 << -0.45285 << -0.28476 << 0.00767 << 0.20109 << 0.46128 << 0.5718 << 0.51884 << 0.43825 << 0.3922 << 0.18957 << -0.21338 << -0.46896 << -0.86271 << -1.22652 << -1.82979 << -2.24656 << -1.28408 << -1.43145 << -2.15446;
	QVector<double> e2Vector;
	e2Vector << 5.04727 << -0.5035 << -0.49429 << -0.48508 << -0.41831 << -0.25022 << 0.04912 << 0.23102 << 0.48661 << 0.59253 << 0.53496 << 0.44516 << 0.40602 << 0.19878 << -0.19496 << -0.43443 << -0.79593 << -1.15514 << -1.7469 << -2.15906 << -1.2127 << -1.31632 << -2.16137;
	QVector<double> e3Vector;
	e3Vector << 4.63188 << -0.75472 << -0.74551 << -0.73906 << -0.66722 << -0.48462 << -0.20578 << 0.03058 << 0.30185 << 0.4086 << 0.3388 << 0.25356 << 0.21398 << 0.00967 << -0.49176 << -0.78465 << -1.20902 << -1.57697 << -2.22584 << -2.58228 << -1.50904 << -1.81022 << -2.53323;
	QVector<double> e4Vector;
	e4Vector << 5.0821 << -0.5097 << -0.49966 << -0.48895 << -0.42229 << -0.26092 << 0.02706 << 0.22193 << 0.49328 << 0.61472 << 0.57747 << 0.5199 << 0.4608 << 0.26337 << -0.10813 << -0.3933 << -0.88085 << -1.27669 << -1.91814 << -2.38168 << -1.41093 << -1.59217 << -2.14635;
	QVector<double> e5Vector;
	e5Vector << 0.18322 << 0.28805 << 0.28897 << 0.25144 << 0.17976 << 0.06369 << 0.0117 << 0.04697 << 0.1799 << 0.52729 << 0.6088 << 0.64472 << 0.7861 << 0.76837 << 0.75179 << 0.6788 << 0.70689 << 0.77989 << 0.77966 << 1.24961 << 0.14271 << 0.52407 << 0.40387;
	QVector<double> e6Vector;
	e6Vector << -0.12736 << -0.10164 << -0.10019 << -0.11006 << -0.12858 << -0.15752 << -0.17051 << -0.15948 << -0.14539 << -0.12964 << -0.13843 << -0.15694 << -0.07843 << -0.09054 << -0.14053 << -0.18257 << -0.2595 << -0.29657 << -0.45384 << -0.35874 << -0.39006 << -0.37578 << -0.48492;
	QVector<double> e7Vector;
	e7Vector << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0.00102 << 0.08607 << 0.10601 << 0.02262 << 0 << 0.10302 << 0.05393 << 0.19082 << 0.29888 << 0.67466 << 0.79508 << 0 << 0 << 0;
	QVector<double> MhVector;
	MhVector << 8.5 << 6.75 << 6.75 << 6.75 << 6.75 << 6.75 << 6.75 << 6.75 << 6.75 << 6.75 << 6.75 << 6.75 << 6.75 << 6.75 << 6.75 << 6.75 << 6.75 << 6.75 << 6.75 << 6.75 << 8.5 << 8.5 << 8.5;

	QVector<double> TUVector;
	TUVector << 0.286 << 0.265 << 0.267 << 0.267 << 0.276 << 0.286 << 0.322 << 0.313 << 0.288 << 0.283 << 0.267 << 0.272 << 0.267 << 0.265 << 0.311 << 0.318 << 0.382 << 0.398 << 0.41 << 0.394 << 0.414 << 0.465 << 0.355 ;
	QVector<double> sigmaVector;
	sigmaVector << 0.5 << 0.502 << 0.502 << 0.502 << 0.507 << 0.516 << 0.513 << 0.52 << 0.518 << 0.523 << 0.527 << 0.546 << 0.541 << 0.555 << 0.571 << 0.573 << 0.566 << 0.58 << 0.566 << 0.583 << 0.601 << 0.626 << 0.645 ;
	QVector<double> TMVector;
	TMVector << 0.256 << 0.26 << 0.262 << 0.262 << 0.274 << 0.286 << 0.32 << 0.318 << 0.29 << 0.288 << 0.267 << 0.269 << 0.267 << 0.265 << 0.299 << 0.302 << 0.373 << 0.389 << 0.401 << 0.385 << 0.437 << 0.477 << 0.477 ;
	
	double T;
	if (theResponseType == RBooreAtkinson2008IntensityModel::PGA) {
		T = 0.0;
	} else if (theResponseType == RBooreAtkinson2008IntensityModel::PGV) {
		T = -1.0;
	} 


	QVector<double> spectralResponseVector;
	for (int i = 0; i < locationList.count(); i++) {
		spectralResponseVector << 0.0;
	}

	for (int index = 0; index < theMagnitudeList.count(); index++) {
		double U = 0.0;
		double SS = 0.0;
		double NS = 0.0;
		double RS = 0.0;
		
		RBooreAtkinson2008IntensityModel::RFaultType theFaultType;
		if (theFaultTypes.count() == 1) {
			theFaultType = theFaultTypes[0];
		} else {
			theFaultType = theFaultTypes[index];
		}

		if (theFaultType == RBooreAtkinson2008IntensityModel::Unspecified) {
			U = 1.0;
		} else if (theFaultType == RBooreAtkinson2008IntensityModel::StrikeSlip) {
			SS = 1.0;
		} else if (theFaultType == RBooreAtkinson2008IntensityModel::NormalSlip) {
			NS = 1.0;
		} else if (theFaultType == RBooreAtkinson2008IntensityModel::ReverseSlip) {
			RS = 1.0;
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

			if (theResponseType == RBooreAtkinson2008IntensityModel::Sa) {
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
				double e1 = e1Vector[j];
				double e2 = e2Vector[j];
				double e3 = e3Vector[j];
				double e4 = e4Vector[j];
				double e5 = e5Vector[j];
				double e6 = e6Vector[j];
				double e7 = e7Vector[j];
				double Mh = MhVector[j];

				double FM;
				if (theSmoothness == RBooreAtkinson2008IntensityModel::Smoothed) {
					// Continuous
					double MEpsilon = 0.3;
					if (M <= Mh - MEpsilon) {
						FM = e1 * U + e2 * SS + e3 * NS + e4 * RS + e5 * (M - Mh) + e6 * gsl_pow_2(M - Mh);
					}
					else if ((M >= Mh - MEpsilon) && (M <= Mh + MEpsilon)) {
						// Smoothing
						int result = cubicSmooth(Mh, MEpsilon, e5 * (-MEpsilon) + e6 * gsl_pow_2(-MEpsilon), e7 * (MEpsilon), e5 + 2 * e6 * (-MEpsilon), e7, M, &FM);
						FM += e1 * U + e2 * SS + e3 * NS + e4 * RS;
					}
					else {
						FM = e1 * U + e2 * SS + e3 * NS + e4 * RS + e7 * (M - Mh);
					}
				} else {					
					// Discontinuity
					if (M <= Mh) {
						FM = e1 * U + e2 * SS + e3 * NS + e4 * RS + e5 * (M - Mh) + e6 * gsl_pow_2(M - Mh);
					} else {
						FM = e1 * U + e2 * SS + e3 * NS + e4 * RS + e7 * (M - Mh);
					}
				}


				double h = hVector[j];
				double c1 = c1Vector[j];
				double c2 = c2Vector[j];
				double c3 = c3Vector[j];

				double R = sqrt(Rjb * Rjb + h * h);
				double FD = (c1 + c2 * (M - Mref)) * log(R / Rref) + c3 * (R-Rref);



				double blin = blinVector[j];
				double b1 = b1Vector[j];
				double b2 = b2Vector[j];

				double bnl;
				if (theSmoothness == RBooreAtkinson2008IntensityModel::Smoothed) {
					// Continuous form
					double Vs30Epsilon = 40; // m/s

					if (Vs30 <= V1 - Vs30Epsilon) {
						bnl = b1;
					} 
					else if (Vs30 > V1 - Vs30Epsilon && Vs30 <= V1 + Vs30Epsilon) {
						// Smoothing
						int result = cubicSmooth(V1, Vs30Epsilon, b1, (b1 - b2) * log((V1 + Vs30Epsilon) / V2) / log(V1/V2) + b2, 0.0, (b1 - b2) / (V1 + Vs30Epsilon) / log(V1/V2), Vs30, &bnl);						
					} 
					else if (Vs30 > V1 + Vs30Epsilon && Vs30 <= V2 - Vs30Epsilon) {
						bnl = (b1 - b2) * log(Vs30 / V2) / log(V1/V2) + b2;
					} 
					else if (Vs30 > V2 - Vs30Epsilon && Vs30 <= V2 + Vs30Epsilon) {
						// Smoothing
						int result = cubicSmooth(V2, Vs30Epsilon, (b1 - b2) * log((V2 - Vs30Epsilon) / V2) / log(V1/V2) + b2, b2 * log((V2 + Vs30Epsilon) / Vref) / log(V2/Vref), (b1 - b2) / (V2 - Vs30Epsilon) / log(V1/V2), b2 / (V2 + Vs30Epsilon) / log(V2/Vref), Vs30, &bnl);
					} 
					else if (Vs30 > V2 + Vs30Epsilon && Vs30 <= Vref - Vs30Epsilon) {
						bnl = b2 * log(Vs30 / Vref) / log(V2/Vref);
					} 
					else if (Vs30 > Vref - Vs30Epsilon && Vs30 <= Vref + Vs30Epsilon) {
						// Smoothing
						int result = cubicSmooth(Vref, Vs30Epsilon, b2 * log((Vref - Vs30Epsilon) / Vref) / log(V2/Vref), 0.0, b2 / (Vref - Vs30Epsilon) / log(V2/Vref), 0.0, Vs30, &bnl);
					} 
					else if (Vs30 > Vref + Vs30Epsilon) {
						bnl = 0.0;
					}
				} else {
					// Discontinuity
					if (Vs30 <= V1) {
						bnl = b1;
					} else if (Vs30 > V1 && Vs30 <= V2) {
						bnl = (b1 - b2) * log(Vs30 / V2) / log(V1/V2) + b2;
					} else if (Vs30 > V2 && Vs30 < Vref) {
						bnl = b2 * log(Vs30 / Vref) / log(V2/Vref);
					} else if (Vs30 >= Vref) {
						bnl = 0.0;
					}
				}


				double deltaY = bnl * log(a2 / pga_low);
				double cc = (3 * deltaY - bnl * deltaX) / gsl_pow_2(deltaX);
				double dd = -(2 * deltaY - bnl * deltaX) / gsl_pow_3(deltaX);

				double pga4nl;
				{
					// For pga4nl, calculate the PGA for FS = 0.0 and epsilon = 0.0 and Rref = 5.0 km;
					double e1 = e1Vector[1];
					double e2 = e2Vector[1];
					double e3 = e3Vector[1];
					double e4 = e4Vector[1];
					double e5 = e5Vector[1];
					double e6 = e6Vector[1];
					double e7 = e7Vector[1];
					double Mh = MhVector[1];
					
					
					double FM;
					if (theSmoothness == RBooreAtkinson2008IntensityModel::Smoothed) {
						// Continuous
						double MEpsilon = 0.3;
						if (M <= Mh - MEpsilon) {
							FM = e1 * U + e2 * SS + e3 * NS + e4 * RS + e5 * (M - Mh) + e6 * gsl_pow_2(M - Mh);
						}
						else if ((M >= Mh - MEpsilon) && (M <= Mh + MEpsilon)) {
							// Smoothing
							int result = cubicSmooth(Mh, MEpsilon, e5 * (-MEpsilon) + e6 * gsl_pow_2(-MEpsilon), e7 * (MEpsilon), e5 + 2 * e6 * (-MEpsilon), e7, M, &FM);
							FM += e1 * U + e2 * SS + e3 * NS + e4 * RS;
						}
						else {
							FM = e1 * U + e2 * SS + e3 * NS + e4 * RS + e7 * (M - Mh);
						}
					} else {
						// Discontinuity
						if (M <= Mh) {
							FM = e1 * U + e2 * SS + e3 * NS + e4 * RS + e5 * (M - Mh) + e6 * gsl_pow_2(M - Mh);
						} else {
							FM = e1 * U + e2 * SS + e3 * NS + e4 * RS + e7 * (M - Mh);
						}
					}

					
					double h = hVector[1];
					double c1 = c1Vector[1];
					double c2 = c2Vector[1];
					double c3 = c3Vector[1];
					double R = sqrt(Rjb * Rjb + h * h);
					double Rref = 5.0;//Is this right?////////////////////////////////////////////////////???????????????????????????????????????????????
					double FD = (c1 + c2 * (M - Mref)) * log(R / Rref) + c3 * (R-Rref);

					pga4nl = exp(FM + FD);
				}

				double Fnl;
				if (theSmoothness == RBooreAtkinson2008IntensityModel::Smoothed) {
					// Continuous
					double pga4nlEpsilon = 0.01;
					if (pga4nl < a1 - pga4nlEpsilon) {
						Fnl = bnl * log(pga_low / 0.1);
					}
					else if (pga4nl >= a1 - pga4nlEpsilon && pga4nl < a1 + pga4nlEpsilon) {
						// Smoothing
						int result = cubicSmooth(pga4nl, pga4nlEpsilon, bnl * log(pga_low / 0.1), bnl * log(pga_low / 0.1) + cc * gsl_pow_2(log((a1 + pga4nlEpsilon) / a1)) + dd * gsl_pow_3(log((a1 + pga4nlEpsilon) / a1)), 0.0, 2 * cc / pga4nl * log(pga4nl / a1) + 3 * dd / pga4nl * gsl_pow_2(log(pga4nl / a1)), pga4nl, &Fnl);
					}
					else if (pga4nl >= a1 + pga4nlEpsilon && pga4nl < a2 - pga4nlEpsilon) {
						Fnl = bnl * log(pga_low / 0.1) + cc * gsl_pow_2(log(pga4nl / a1)) + dd * gsl_pow_3(log(pga4nl / a1));
					}
					else if (pga4nl >= a2 - pga4nlEpsilon && pga4nl < a2 + pga4nlEpsilon) {
						// Smoothing
						int result = cubicSmooth(pga4nl, pga4nlEpsilon, bnl * log(pga_low / 0.1) + cc * gsl_pow_2(log((a1 + pga4nlEpsilon) / a1)) + dd * gsl_pow_3(log((a1 + pga4nlEpsilon) / a1)), bnl * log(pga4nl / 0.1), 2 * cc / pga4nl * log(pga4nl / a1) + 3 * dd / pga4nl * gsl_pow_2(log(pga4nl / a1)), bnl / pga4nl, pga4nl, &Fnl);				
					}
					else if (pga4nl >= a2 + pga4nlEpsilon) {
						Fnl = bnl * log(pga4nl / 0.1);
					}
				} else {
					// Discontinuity
					if (pga4nl <= a1) {
						Fnl = bnl * log(pga_low / 0.1);
					} else if (pga4nl > a1 && pga4nl <= a2) {
						Fnl = bnl * log(pga_low / 0.1) + cc * gsl_pow_2(log(pga4nl / a1)) + dd * gsl_pow_3(log(pga4nl / a1));
					} else if (pga4nl > a2) {
						Fnl = bnl * log(pga4nl / 0.1);
					}
				}



				double Flin = blin * log(Vs30 / Vref);
				double FS = Flin + Fnl;

				
				double sigma = sigmaVector[j]; 
				double tau;
				if (theFaultType == RBooreAtkinson2008IntensityModel::Unspecified) {
					tau = TUVector[j];
				} else {
					tau = TMVector[j];
				}

				
				
				double lnY = FM + FD + FS + sigma * epsilon + tau * eta;
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

bool RBooreAtkinson2008IntensityModel::canBeRunParallelly(){
	return true;
}