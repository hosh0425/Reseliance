#include "RAbrahamsonEtAl2013IntensityModel.h"	


#include <QCoreApplication>
#include <QMetaEnum>
	
#include "RParameter.h"	
#include "RResponse.h"	
#include "RLocation.h"
#include "RLocationResponse.h"
#include "RMainWindow.h"


#include <math.h>

#include <gsl/gsl_math.h>
	
RAbrahamsonEtAl2013IntensityModel::RAbrahamsonEtAl2013IntensityModel(QObject *parent, QString name)	
	: RModel(parent, name)
{	
	//theEpsilon = 0;
	theResponseType = RAbrahamsonEtAl2013IntensityModel::Sa;
	theShockType = RAbrahamsonEtAl2013IntensityModel::MainShock;
	//theSmoothness = RAbrahamsonEtAl2013IntensityModel::Smoothed;
}	
	
RAbrahamsonEtAl2013IntensityModel::~RAbrahamsonEtAl2013IntensityModel()	
{	
	
}	

QString RAbrahamsonEtAl2013IntensityModel::getMagnitudeList()	
{	
	return parameterListToString(theMagnitudeList);
}	

void RAbrahamsonEtAl2013IntensityModel::setMagnitudeList(QString value)	
{	
	theMagnitudeList = stringToParameterList(value);
}

QString RAbrahamsonEtAl2013IntensityModel::getDepthToTopOfRuptureList() 	
{	
	return parameterListToString(theDepthToTopOfRuptureList);
}	
	
void RAbrahamsonEtAl2013IntensityModel::setDepthToTopOfRuptureList(QString value)	
{	
	theDepthToTopOfRuptureList = stringToParameterList(value);
}

QString RAbrahamsonEtAl2013IntensityModel::getDownDipRupWidthList() 	
{	
	return parameterListToString(theDownDipRupWidthList);
}	
	
void RAbrahamsonEtAl2013IntensityModel::setDownDipRupWidthList(QString value)	
{	
	theDownDipRupWidthList = stringToParameterList(value);
}

QString RAbrahamsonEtAl2013IntensityModel::getHrzDistTopOfRupList() 	
{	
	return parameterListToString(theHrzDistTopOfRupList);
}	
	
void RAbrahamsonEtAl2013IntensityModel::setHrzDistTopOfRupList(QString value)	
{	
	theHrzDistTopOfRupList = stringToParameterList(value);
}

QString RAbrahamsonEtAl2013IntensityModel::getHrzDistEndOfRupList() 	
{	
	return parameterListToString(theHrzDistEndOfRupList);
}	
	
void RAbrahamsonEtAl2013IntensityModel::setHrzDistEndOfRupList(QString value)	
{	
	theHrzDistEndOfRupList = stringToParameterList(value);
}

QString RAbrahamsonEtAl2013IntensityModel::getRupDistList() 	
{	
	return parameterListToString(theRupDistList);
}	
	
void RAbrahamsonEtAl2013IntensityModel::setRupDistList(QString value)	
{	
	theRupDistList = stringToParameterList(value);
}

QString RAbrahamsonEtAl2013IntensityModel::getDipAngles() 	
{	
	return doubleVectorToString(theDipAngleVector);
}	
	
void RAbrahamsonEtAl2013IntensityModel::setDipAngles(QString value)	
{	
	theDipAngleVector = stringToDoubleVector(value);;
}

QString RAbrahamsonEtAl2013IntensityModel::getVsFlags() 	
{	
	return doubleVectorToString(theVsFlagVector);
}	
	
void RAbrahamsonEtAl2013IntensityModel::setVsFlags(QString value)	
{	
	theVsFlagVector = stringToDoubleVector(value);;
}

QString RAbrahamsonEtAl2013IntensityModel::getHypocentreLocationList()	
{	
	return parameterListToString(theHypocentreLocationList);
}	

void RAbrahamsonEtAl2013IntensityModel::setHypocentreLocationList(QString value)	
{	
	theHypocentreLocationList = stringToParameterList(value);

	// Checking if all the passed parameters are either RLocation or RLocationResponse
	parameterListToLocationList(theHypocentreLocationList);
}


QString RAbrahamsonEtAl2013IntensityModel::getCentroidRJBList()	
{	
	return parameterListToString(theCentroidRJBList);
}	

void RAbrahamsonEtAl2013IntensityModel::setCentroidRJBList(QString value)	
{	
	theCentroidRJBList = stringToParameterList(value);
}


QString RAbrahamsonEtAl2013IntensityModel::getFaultTypes() const	
{
	QMetaEnum metaEnum = metaObject()->enumerator(metaObject()->indexOfEnumerator("RFaultType"));
	QString result = "";
	for (int i = 0; i < theFaultTypes.count(); i++) {
		QString temp = metaEnum.valueToKey(theFaultTypes[i]);
	    result += temp + "; ";
	}
	return result;
}	
	

void RAbrahamsonEtAl2013IntensityModel::setFaultTypes(QString value)	
{	
	QString string = value;
	QMetaEnum metaEnum = metaObject()->enumerator(metaObject()->indexOfEnumerator("RFaultType"));

	QRegExp regExp = QRegExp(QString("[ ,;:\t\n\v\r%1%2]").arg(QChar(QChar::ParagraphSeparator)).arg(QChar(QChar::LineSeparator)));
	QStringList stringList = string.split(regExp, QString::SkipEmptyParts);
	string = "";
	QList<RAbrahamsonEtAl2013IntensityModel::RFaultType> tempList;
	for (int i = 0; i < stringList.count(); i++) {
		QByteArray ba = stringList[i].toLocal8Bit();
		const char *tempChar = ba.data();
		int tempValue = metaEnum.keyToValue(tempChar);
		RAbrahamsonEtAl2013IntensityModel::RFaultType temp = RFaultType(tempValue);
		if (temp < 0) {
			rCritical() << "Error in the Model" << objectName() << ": The fault type" << stringList[i] << "is not valid";
			return;
		}
		tempList.append(temp);
	}
	theFaultTypes = tempList;
}	


QString RAbrahamsonEtAl2013IntensityModel::getWallTypes() const	
{
	QMetaEnum metaEnum = metaObject()->enumerator(metaObject()->indexOfEnumerator("RWallType"));
	QString result = "";
	for (int i = 0; i < theWallTypes.count(); i++) {
		QString temp = metaEnum.valueToKey(theWallTypes[i]);
	    result += temp + "; ";
	}
	return result;
}	
	

void RAbrahamsonEtAl2013IntensityModel::setWallTypes(QString value)	
{	
	QString string = value;
	QMetaEnum metaEnum = metaObject()->enumerator(metaObject()->indexOfEnumerator("RWallType"));

	QRegExp regExp = QRegExp(QString("[ ,;:\t\n\v\r%1%2]").arg(QChar(QChar::ParagraphSeparator)).arg(QChar(QChar::LineSeparator)));
	QStringList stringList = string.split(regExp, QString::SkipEmptyParts);
	string = "";
	QList<RAbrahamsonEtAl2013IntensityModel::RWallType> tempList;
	for (int i = 0; i < stringList.count(); i++) {
		QByteArray ba = stringList[i].toLocal8Bit();
		const char *tempChar = ba.data();
		int tempValue = metaEnum.keyToValue(tempChar);
		RAbrahamsonEtAl2013IntensityModel::RWallType temp = RWallType(tempValue);
		if (temp < 0) {
			rCritical() << "Error in the Model" << objectName() << ": The Wall type" << stringList[i] << "is not valid";
			return;
		}
		tempList.append(temp);
	}
	theWallTypes = tempList;
}	

QString RAbrahamsonEtAl2013IntensityModel::getRegions() const	
{
	QMetaEnum metaEnum = metaObject()->enumerator(metaObject()->indexOfEnumerator("RRegion"));
	QString result = "";
	for (int i = 0; i < theRegions.count(); i++) {
		QString temp = metaEnum.valueToKey(theRegions[i]);
	    result += temp + "; ";
	}
	return result;
}	
	

void RAbrahamsonEtAl2013IntensityModel::setRegions(QString value)	
{	
	QString string = value;
	QMetaEnum metaEnum = metaObject()->enumerator(metaObject()->indexOfEnumerator("RRegion"));

	QRegExp regExp = QRegExp(QString("[ ,;:\t\n\v\r%1%2]").arg(QChar(QChar::ParagraphSeparator)).arg(QChar(QChar::LineSeparator)));
	QStringList stringList = string.split(regExp, QString::SkipEmptyParts);
	string = "";
	QList<RAbrahamsonEtAl2013IntensityModel::RRegion> tempList;
	for (int i = 0; i < stringList.count(); i++) {
		QByteArray ba = stringList[i].toLocal8Bit();
		const char *tempChar = ba.data();
		int tempValue = metaEnum.keyToValue(tempChar);
		RAbrahamsonEtAl2013IntensityModel::RRegion temp = RRegion(tempValue);
		if (temp < 0) {
			rCritical() << "Error in the Model" << objectName() << ": The region" << stringList[i] << "is not valid";
			return;
		}
		tempList.append(temp);
	}
	theRegions = tempList;
}	


RAbrahamsonEtAl2013IntensityModel::RResponseType RAbrahamsonEtAl2013IntensityModel::getResponseType() const	
{	
	return theResponseType;
}	
	
void RAbrahamsonEtAl2013IntensityModel::setResponseType(RAbrahamsonEtAl2013IntensityModel::RResponseType value)	
{	
	theResponseType = value;
}

RAbrahamsonEtAl2013IntensityModel::RShockType RAbrahamsonEtAl2013IntensityModel::getShockType() const	
{	
	return theShockType;
}	
	
void RAbrahamsonEtAl2013IntensityModel::setShockType(RAbrahamsonEtAl2013IntensityModel::RShockType value)	
{	
	theShockType = value;
}

QString RAbrahamsonEtAl2013IntensityModel::getPeriodList()	
{	
	return parameterListToString(thePeriodList);
}	
	
void RAbrahamsonEtAl2013IntensityModel::setPeriodList(QString value)	
{	
	thePeriodList = stringToParameterList(value);

	createResponses();
}	


QString RAbrahamsonEtAl2013IntensityModel::getEpsilonUncertaintyList()	
{	
	return parameterListToString(theEpsilonUncertaintyList);
}	
	
void RAbrahamsonEtAl2013IntensityModel::setEpsilonUncertaintyList(QString value)	
{	
	theEpsilonUncertaintyList = stringToParameterList(value);

	createResponses();
}	


QString RAbrahamsonEtAl2013IntensityModel::getEtaUncertaintyList()	
{	
	return parameterListToString(theEtaUncertaintyList);
}	
	
void RAbrahamsonEtAl2013IntensityModel::setEtaUncertaintyList(QString value)	
{	
	theEtaUncertaintyList = stringToParameterList(value);

	createResponses();
}	


QList<RParameter *> RAbrahamsonEtAl2013IntensityModel::getPhysicalParameterList()
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

QString RAbrahamsonEtAl2013IntensityModel::getStructureLocationList()	
{	
	return parameterListToString(theStructureLocationList);
}	
	
void RAbrahamsonEtAl2013IntensityModel::setStructureLocationList(QString value)	
{	
	theStructureLocationList = stringToParameterList(value);

	// Checking if all the passed parameters are either RLocation or RLocationResponse
	parameterListToLocationList(theStructureLocationList);

	createResponses();
}	
	
QString RAbrahamsonEtAl2013IntensityModel::getShearWaveVelocityList()	
{	
	return parameterListToString(theShearWaveVelocityList);
}	
	
void RAbrahamsonEtAl2013IntensityModel::setShearWaveVelocityList(QString value)	
{	
	theShearWaveVelocityList = stringToParameterList(value);

	createResponses();
}	

QString RAbrahamsonEtAl2013IntensityModel::getDepthToVs1List()	
{	
	return parameterListToString(theDepthToVs1List);
}	
	
void RAbrahamsonEtAl2013IntensityModel::setDepthToVs1List(QString value)	
{	
	theDepthToVs1List = stringToParameterList(value);

	createResponses();
}	

void RAbrahamsonEtAl2013IntensityModel::createResponses()
{
	// Removing old responses
	while (theSpectralResponseList.count() > 0) {
		delete theSpectralResponseList.takeAt(0);
	}

	if (theStructureLocationList.isEmpty() || theShearWaveVelocityList.isEmpty()) {
		return;
	}

	if ((theStructureLocationList.count() != thePeriodList.count() && theResponseType == RAbrahamsonEtAl2013IntensityModel::Sa) || (theStructureLocationList.count() != theShearWaveVelocityList.count() && theShearWaveVelocityList.count() > 1) || (theStructureLocationList.count() != theEpsilonUncertaintyList.count() && theEpsilonUncertaintyList.count() > 1) || (theStructureLocationList.count() != theEtaUncertaintyList.count() && theEtaUncertaintyList.count() > 1)) {
		rCritical() << "Error: The number of input parameters for the period list, structure location list and  uncertainty list are not equal in the model" << objectName() << ".";
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

int RAbrahamsonEtAl2013IntensityModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
{	
	if (theStructureLocationList.isEmpty()) {
		rCritical(threadID) << "Error: There should be at least one structure location in the model" << objectName() << ".";
		return -1;
	}
	if ((theStructureLocationList.count() != thePeriodList.count() && theResponseType == RAbrahamsonEtAl2013IntensityModel::Sa) || (theStructureLocationList.count() != theShearWaveVelocityList.count() && theShearWaveVelocityList.count() > 1)) {
		rCritical(threadID) << "Error: The number of input parameters for the period list, structure location list, and shear wave velocity list are not equal in the model" << objectName() << ".";
		return -1;
	}
	
	// Checking if the number of magnitudes is equal to the number of hypocentre locations
	if (theHypocentreLocationList.count() != theMagnitudeList.count() /*|| theCentroidRJBList.count() != theMagnitudeList.count() */) {
		rCritical(threadID) << "Error: The number of magnitude parameters should be equal to the number of hypocentre locations in the model" << objectName() << ".";
		return -1;
	}


	// Converting the list of parameters to the list of locations
	QList<RLocation *> locationList = parameterListToLocationList(theStructureLocationList);


	// Some constants
	
	double M2 = 5;
	double a4 = -0.1;
	double a5 = -0.49;
	double a7 = 0;
	double a39 = 0;
	double n = 1.5;
	double a2HW = 0.2;
	
	

	// Period dependent coefficients
	QVector<double> periodTableVector;
	periodTableVector  << -1 << 0 << 0.01 << 0.02 << 0.03 << 0.05 << 0.075 << 0.1 << 0.15 << 0.2 << 0.25 << 0.3 << 0.4 << 0.5 << 0.75 << 1 << 1.5 << 2 << 3 << 4 << 5 << 6 << 7.5 << 10 ;

	QVector<double> VlinVector;
	QVector<double> bVector;
	QVector<double> c4Vector;
	QVector<double> M1Vector;
	QVector<double> a1Vector;
	QVector<double> a2Vector;
	QVector<double> a3Vector;
	QVector<double> a6Vector;
	QVector<double> a8Vector;
	QVector<double> a10Vector;
	QVector<double> a11Vector;
	QVector<double> a12Vector;
	QVector<double> a13Vector;
	QVector<double> a14Vector;
	QVector<double> a15Vector;
	QVector<double> a17Vector;
	QVector<double> a25Vector;
	QVector<double> a28Vector;
	QVector<double> a29Vector;
	QVector<double> a31Vector;
	QVector<double> a36Vector;
	QVector<double> a37Vector;
	QVector<double> a38Vector;
	QVector<double> a40Vector;
	QVector<double> a41Vector;
	QVector<double> a42Vector;
	QVector<double> a43Vector;
	QVector<double> a44Vector;
	QVector<double> a45Vector;
	QVector<double> a46Vector;
	QVector<double> s1Vector;
	QVector<double> s2Vector;
	QVector<double> s3Vector;
	QVector<double> s4Vector;
	QVector<double> s5JPVector;
	QVector<double> s6JPVector;
	QVector<double> s1KnownVsVector;
	QVector<double> s2KnownVsVector;

	VlinVector  << 330 << 660 << 660 << 680 << 770 << 915 << 960 << 910 << 740 << 590 << 495 << 430 << 360 << 340 << 330 << 330 << 330 << 330 << 330 << 330 << 330 << 330 << 330 << 330 ;
	bVector  << -2.02 << -1.47 << -1.47 << -1.459 << -1.39 << -1.219 << -1.152 << -1.23 << -1.587 << -2.012 << -2.411 << -2.757 << -3.278 << -3.599 << -3.8 << -3.5 << -2.4 << -1 << 0 << 0 << 0 << 0 << 0 << 0 ;
	c4Vector  << 4.5 << 4.5 << 4.5 << 4.5 << 4.5 << 4.5 << 4.5 << 4.5 << 4.5 << 4.5 << 4.5 << 4.5 << 4.5 << 4.5 << 4.5 << 4.5 << 4.5 << 4.5 << 4.5 << 4.5 << 4.5 << 4.5 << 4.5 << 4.5 ;
	M1Vector  << 6.75 << 6.75 << 6.75 << 6.75 << 6.75 << 6.75 << 6.75 << 6.75 << 6.75 << 6.75 << 6.75 << 6.75 << 6.75 << 6.75 << 6.75 << 6.75 << 6.75 << 6.75 << 6.82 << 6.92 << 7 << 7.06 << 7.145 << 7.25 ;
	a1Vector  << 5.975 << 0.587 << 0.587 << 0.598 << 0.602 << 0.707 << 0.973 << 1.169 << 1.442 << 1.637 << 1.701 << 1.712 << 1.662 << 1.571 << 1.299 << 1.043 << 0.665 << 0.329 << -0.06 << -0.299 << -0.562 << -0.875 << -1.303 << -1.928 ;
	a2Vector  << -0.919 << -0.79 << -0.79 << -0.79 << -0.79 << -0.79 << -0.79 << -0.79 << -0.79 << -0.79 << -0.79 << -0.79 << -0.79 << -0.79 << -0.79 << -0.79 << -0.79 << -0.79 << -0.79 << -0.79 << -0.765 << -0.711 << -0.634 << -0.529 ;
	a3Vector  << 0.275 << 0.275 << 0.275 << 0.275 << 0.275 << 0.275 << 0.275 << 0.275 << 0.275 << 0.275 << 0.275 << 0.275 << 0.275 << 0.275 << 0.275 << 0.275 << 0.275 << 0.275 << 0.275 << 0.275 << 0.275 << 0.275 << 0.275 << 0.275 ;
	a6Vector  << 2.3657 << 2.1541 << 2.1541 << 2.1461 << 2.1566 << 2.0845 << 2.0285 << 2.0408 << 2.1208 << 2.2241 << 2.3124 << 2.3383 << 2.4688 << 2.5586 << 2.6821 << 2.763 << 2.8355 << 2.8973 << 2.9061 << 2.8888 << 2.8984 << 2.8955 << 2.87 << 2.8431 ;
	a8Vector  << -0.094 << -0.015 << -0.015 << -0.015 << -0.015 << -0.015 << -0.015 << -0.015 << -0.022 << -0.03 << -0.038 << -0.045 << -0.055 << -0.065 << -0.095 << -0.11 << -0.124 << -0.138 << -0.172 << -0.197 << -0.218 << -0.235 << -0.255 << -0.285 ;
	a10Vector  << 2.36 << 1.735 << 1.735 << 1.718 << 1.615 << 1.358 << 1.258 << 1.31 << 1.66 << 2.22 << 2.77 << 3.25 << 3.99 << 4.45 << 4.75 << 4.3 << 2.6 << 0.55 << -0.95 << -0.95 << -0.93 << -0.91 << -0.87 << -0.8 ;
	a11Vector  << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 ;
	a12Vector  << -0.1 << -0.1 << -0.1 << -0.1 << -0.1 << -0.1 << -0.1 << -0.1 << -0.1 << -0.1 << -0.1 << -0.1 << -0.1 << -0.1 << -0.1 << -0.1 << -0.1 << -0.1 << -0.1 << -0.1 << -0.1 << -0.2 << -0.2 << -0.2 ;
	a13Vector  << 0.25 << 0.6 << 0.6 << 0.6 << 0.6 << 0.6 << 0.6 << 0.6 << 0.6 << 0.6 << 0.6 << 0.6 << 0.58 << 0.56 << 0.53 << 0.5 << 0.42 << 0.35 << 0.2 << 0 << 0 << 0 << 0 << 0 ;
	a14Vector  << 0.22 << -0.3 << -0.3 << -0.3 << -0.3 << -0.3 << -0.3 << -0.3 << -0.3 << -0.3 << -0.24 << -0.19 << -0.11 << -0.04 << 0.07 << 0.15 << 0.27 << 0.35 << 0.46 << 0.54 << 0.61 << 0.65 << 0.72 << 0.8 ;
	a15Vector  << 0.3 << 1.1 << 1.1 << 1.1 << 1.1 << 1.1 << 1.1 << 1.1 << 1.1 << 1.1 << 1.1 << 1.03 << 0.92 << 0.84 << 0.68 << 0.57 << 0.42 << 0.31 << 0.16 << 0.05 << -0.04 << -0.11 << -0.19 << -0.3 ;
	a17Vector  << -0.0005 << -0.0072 << -0.0072 << -0.0073 << -0.0075 << -0.008 << -0.0089 << -0.0095 << -0.0095 << -0.0086 << -0.0074 << -0.0064 << -0.0043 << -0.0032 << -0.0025 << -0.0025 << -0.0022 << -0.0019 << -0.0015 << -0.001 << -0.001 << -0.001 << -0.001 << -0.001 ;
	a25Vector  << -0.0001 << -0.0015 << -0.0015 << -0.0015 << -0.0016 << -0.002 << -0.0027 << -0.0033 << -0.0035 << -0.0033 << -0.0029 << -0.0027 << -0.0023 << -0.002 << -0.001 << -0.0005 << -0.0004 << -0.0002 << 0 << 0 << 0 << 0 << 0 << 0 ;
	a28Vector  << 0.0005 << 0.0025 << 0.0025 << 0.0024 << 0.0023 << 0.0027 << 0.0032 << 0.0036 << 0.0033 << 0.0027 << 0.0024 << 0.002 << 0.001 << 0.0008 << 0.0007 << 0.0007 << 0.0006 << 0.0003 << 0 << 0 << 0 << 0 << 0 << 0 ;
	a29Vector  << -0.0037 << -0.0034 << -0.0034 << -0.0033 << -0.0034 << -0.0033 << -0.0029 << -0.0025 << -0.0025 << -0.0031 << -0.0036 << -0.0039 << -0.0048 << -0.005 << -0.0041 << -0.0032 << -0.002 << -0.0017 << -0.002 << -0.002 << -0.002 << -0.002 << -0.002 << -0.002 ;
	a31Vector  << -0.1462 << -0.1503 << -0.1503 << -0.1479 << -0.1447 << -0.1326 << -0.1353 << -0.1128 << 0.0383 << 0.0775 << 0.0741 << 0.2548 << 0.2136 << 0.1542 << 0.0787 << 0.0476 << -0.0163 << -0.1203 << -0.2719 << -0.2958 << -0.2718 << -0.2517 << -0.14 << -0.0216 ;
	a36Vector  << 0.377 << 0.265 << 0.265 << 0.255 << 0.249 << 0.202 << 0.126 << 0.022 << -0.136 << -0.078 << 0.037 << -0.091 << 0.129 << 0.31 << 0.505 << 0.358 << 0.131 << 0.123 << 0.109 << 0.135 << 0.189 << 0.215 << 0.15 << 0.092 ;
	a37Vector  << 0.212 << 0.337 << 0.337 << 0.328 << 0.32 << 0.289 << 0.275 << 0.256 << 0.162 << 0.224 << 0.248 << 0.203 << 0.232 << 0.252 << 0.208 << 0.208 << 0.108 << 0.068 << -0.023 << 0.028 << 0.031 << 0.024 << -0.07 << -0.159 ;
	a38Vector  << 0.157 << 0.188 << 0.188 << 0.184 << 0.18 << 0.167 << 0.173 << 0.189 << 0.108 << 0.115 << 0.122 << 0.096 << 0.123 << 0.134 << 0.129 << 0.152 << 0.118 << 0.119 << 0.093 << 0.084 << 0.058 << 0.065 << 0 << -0.05 ;
	a40Vector  << 0.095 << 0.088 << 0.088 << 0.088 << 0.093 << 0.133 << 0.186 << 0.16 << 0.068 << 0.048 << 0.055 << 0.073 << 0.143 << 0.16 << 0.158 << 0.145 << 0.131 << 0.083 << 0.07 << 0.101 << 0.095 << 0.133 << 0.151 << 0.124 ;
	a41Vector  << -0.038 << -0.196 << -0.196 << -0.194 << -0.175 << -0.09 << 0.09 << 0.006 << -0.156 << -0.274 << -0.248 << -0.203 << -0.154 << -0.159 << -0.141 << -0.144 << -0.126 << -0.075 << -0.021 << 0.072 << 0.205 << 0.285 << 0.329 << 0.301 ;
	a42Vector  << 0.065 << 0.044 << 0.044 << 0.061 << 0.162 << 0.451 << 0.506 << 0.335 << -0.084 << -0.178 << -0.187 << -0.159 << -0.023 << -0.029 << 0.061 << 0.062 << 0.037 << -0.143 << -0.028 << -0.097 << 0.015 << 0.104 << 0.299 << 0.243 ;
	a43Vector  << 0.28 << 0.1 << 0.1 << 0.1 << 0.1 << 0.1 << 0.1 << 0.1 << 0.1 << 0.1 << 0.1 << 0.1 << 0.1 << 0.1 << 0.14 << 0.17 << 0.22 << 0.26 << 0.34 << 0.41 << 0.51 << 0.55 << 0.49 << 0.42 ;
	a44Vector  << 0.15 << 0.05 << 0.05 << 0.05 << 0.05 << 0.05 << 0.05 << 0.05 << 0.05 << 0.05 << 0.05 << 0.05 << 0.07 << 0.1 << 0.14 << 0.17 << 0.21 << 0.25 << 0.3 << 0.32 << 0.32 << 0.32 << 0.275 << 0.22 ;
	a45Vector  << 0.09 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0.03 << 0.06 << 0.1 << 0.14 << 0.17 << 0.2 << 0.22 << 0.23 << 0.23 << 0.22 << 0.2 << 0.17 << 0.14 ;
	a46Vector  << 0.07 << -0.05 << -0.05 << -0.05 << -0.05 << -0.05 << -0.05 << -0.05 << -0.05 << -0.03 << 0 << 0.03 << 0.06 << 0.09 << 0.13 << 0.14 << 0.16 << 0.16 << 0.16 << 0.14 << 0.13 << 0.1 << 0.09 << 0.08 ;
	s1Vector  << 0.662 << 0.754 << 0.754 << 0.76 << 0.781 << 0.81 << 0.81 << 0.81 << 0.801 << 0.789 << 0.77 << 0.74 << 0.699 << 0.676 << 0.631 << 0.609 << 0.578 << 0.555 << 0.548 << 0.527 << 0.505 << 0.477 << 0.457 << 0.429 ;
	s2Vector  << 0.51 << 0.52 << 0.52 << 0.52 << 0.52 << 0.53 << 0.54 << 0.55 << 0.56 << 0.565 << 0.57 << 0.58 << 0.59 << 0.6 << 0.615 << 0.63 << 0.64 << 0.65 << 0.64 << 0.63 << 0.63 << 0.63 << 0.63 << 0.63 ;
	s3Vector  << 0.38 << 0.47 << 0.47 << 0.47 << 0.47 << 0.47 << 0.47 << 0.47 << 0.47 << 0.47 << 0.47 << 0.47 << 0.47 << 0.47 << 0.47 << 0.47 << 0.47 << 0.47 << 0.47 << 0.47 << 0.47 << 0.47 << 0.47 << 0.47 ;
	s4Vector  << 0.38 << 0.36 << 0.36 << 0.36 << 0.36 << 0.36 << 0.36 << 0.36 << 0.36 << 0.36 << 0.36 << 0.36 << 0.36 << 0.36 << 0.36 << 0.36 << 0.36 << 0.36 << 0.36 << 0.36 << 0.36 << 0.36 << 0.36 << 0.36 ;
	s5JPVector  << 0.58 << 0.54 << 0.54 << 0.54 << 0.55 << 0.56 << 0.57 << 0.57 << 0.58 << 0.59 << 0.61 << 0.63 << 0.66 << 0.69 << 0.73 << 0.77 << 0.8 << 0.8 << 0.8 << 0.76 << 0.72 << 0.7 << 0.67 << 0.64 ;
	s6JPVector  << 0.53 << 0.63 << 0.63 << 0.63 << 0.63 << 0.65 << 0.69 << 0.7 << 0.7 << 0.7 << 0.7 << 0.7 << 0.7 << 0.7 << 0.69 << 0.68 << 0.66 << 0.62 << 0.55 << 0.52 << 0.5 << 0.5 << 0.5 << 0.5 ;
	s1KnownVsVector  << 0.66 << 0.741 << 0.741 << 0.747 << 0.769 << 0.798 << 0.798 << 0.795 << 0.773 << 0.753 << 0.729 << 0.693 << 0.644 << 0.616 << 0.566 << 0.541 << 0.506 << 0.48 << 0.472 << 0.447 << 0.425 << 0.395 << 0.378 << 0.359 ;
	s2KnownVsVector  << 0.51 << 0.501 << 0.501 << 0.501 << 0.501 << 0.512 << 0.522 << 0.527 << 0.519 << 0.514 << 0.513 << 0.519 << 0.524 << 0.532 << 0.548 << 0.565 << 0.576 << 0.587 << 0.576 << 0.565 << 0.568 << 0.571 << 0.575 << 0.585 ;

	double T;
	if (theResponseType == RAbrahamsonEtAl2013IntensityModel::PGA) {
		T = 0.0;
	} else if (theResponseType == RAbrahamsonEtAl2013IntensityModel::PGV) {
		T = -1.0;
	} 

	double c;
	if (theResponseType == RAbrahamsonEtAl2013IntensityModel::PGV) {
		c = 2400;
	} else {
		c = 2.4;
	}

	QVector<double> spectralResponseVector;
	for (int i = 0; i < locationList.count(); i++) {
		spectralResponseVector << 0.0;
	}

	for (int index = 0; index < theMagnitudeList.count(); index++) {
		
		double FRV = 0;
		double FN = 0;
		RAbrahamsonEtAl2013IntensityModel::RFaultType theFaultType;
		if (theFaultTypes.count() == 1) {
			theFaultType = theFaultTypes[0];
		} else {
			theFaultType = theFaultTypes[index];
		}	

		if (theFaultType == RAbrahamsonEtAl2013IntensityModel::StrikeSlip) {
			FRV = 0;
			FN = 0;
		} else if (theFaultType == RAbrahamsonEtAl2013IntensityModel::ReverseSlip || theFaultType == RAbrahamsonEtAl2013IntensityModel::ReverseObliqueSlip) {
			FRV = 1.0;
		} else if (theFaultType == RAbrahamsonEtAl2013IntensityModel::NormalSlip) {
			FN = 1.0;
		}

		
	
		// For convenience, look up the values of the parameters first
		double M = theMagnitudeList[index].data()->getCurrentValue(threadID);

		
		double dip;
		if ( theDipAngleVector.count() == 1){
			dip = theDipAngleVector[0];
		}else {
			dip = theDipAngleVector[index];
		}


		// IMPORTANT: If M = 0, it means that this hazard is off, so the resulting PGA is Zero
		if (M == 0.0) {
			continue;
		}
		if (getDisplayOutput()) {
			if (M > 8.0 || M < 4.75) {
				rCritical(threadID) << "Warning: The magnitude in the model" << objectName() << "should be between 4.75 and 8.";
			}
		}

		// Verification that the user has given an RLocationResponse
		RLocation *theHypocentreLocationObject = parameterToLocation(theHypocentreLocationList[index].data());  
		if (!theHypocentreLocationObject) {
			rCritical(threadID) << "Error: The location response" << theHypocentreLocationList[index].data()->objectName() << "does not include any location object.";
			return -1;
		}



		for (int i = 0; i < locationList.count(); i++) {
			double Rjb = theHypocentreLocationObject->computeSurfaceDistance(locationList[i], threadID);
			
			double CRjb;
			if (theCentroidRJBList.isEmpty() == 1){
			CRjb = 999;
			} else{ 
				if (theCentroidRJBList.count() == 1){
					CRjb = theCentroidRJBList[0].data()->getCurrentValue(threadID);
				} else {
					CRjb = theCentroidRJBList[i].data()->getCurrentValue(threadID);
				}

			}
			double FAS = 0;
			if (CRjb < 15 || theShockType == RAbrahamsonEtAl2013IntensityModel::AfterShock) {
				FAS = 1.0;
			} else {
				FAS = 0.0;
			}


			if (Rjb > 200.0) {
				if (getDisplayOutput()) {
					rCritical(threadID) << "Warning: The maximum allowable distance in the model" << objectName() << "is 200 km.";
				}
			}

			double Ztor;
			if (theDepthToTopOfRuptureList.count () == 1){
				Ztor = theDepthToTopOfRuptureList[0].data()->getCurrentValue(threadID);
			}else {
				Ztor = theDepthToTopOfRuptureList[index].data()->getCurrentValue(threadID);
			}
			if (Ztor == 999){
				if (FRV == 1) {
					Ztor = pow(std::max(2.704 - 1.226 * std::max((M - 5.849) , 0.0) , 0.0) ,2.0);
				} else {
					Ztor =pow(std::max(2.673 - 1.136 * std::max(M-4.970 , 0.0) , 0.0) , 2.0);
				}
			}
			
			double W;
			if (theDownDipRupWidthList.count () == 1){
				W = theDownDipRupWidthList[0].data()->getCurrentValue(threadID);
			}else {
				W = theDownDipRupWidthList[index].data()->getCurrentValue(threadID);
			}
			if (W == 999 || theDownDipRupWidthList.isEmpty () == 1){
				 W = std::min(18.0 / sin( 3.1416 / 180.0 * dip),pow(10.0 ,(-1.75 + 0.45 * M)) );
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
	
		
			double Rx;
			double irx;
			if (theHrzDistTopOfRupList.count() == 1){
				irx = 0;
			}else{
				irx = i;
			}
			if (theHrzDistTopOfRupList.isEmpty() == 1 || theHrzDistTopOfRupList[irx].data()->getCurrentValue(threadID) == 999){
			Rx = Rjb;
			} else {
				Rx = theHrzDistTopOfRupList[irx].data()->getCurrentValue(threadID);
			}

			double Ry0;
			double iry0;
			if (theHrzDistEndOfRupList.count() == 1){
				iry0 = 0;
			}else{
				iry0 = index;
			}
			if (theHrzDistEndOfRupList.isEmpty() == 1 ||  theHrzDistEndOfRupList[iry0].data()->getCurrentValue(threadID) == 999){
			Ry0 = 999;
			} else {
				Ry0 = theHrzDistEndOfRupList[iry0].data()->getCurrentValue(threadID);
			}

			

			if (theResponseType == RAbrahamsonEtAl2013IntensityModel::Sa) {
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

			RAbrahamsonEtAl2013IntensityModel::RRegion theRegion;
			if (theRegions.count() == 1) {
				theRegion = theRegions[0];
			} else {
				theRegion = theRegions[i];
			}


			double Z1;
			if (theDepthToVs1List.count() == 1) {
				Z1 = theDepthToVs1List[0].data()->getCurrentValue(threadID);
			} else {
				Z1 = theDepthToVs1List[i].data()->getCurrentValue(threadID);
			}
			

			
			double Vs30Flag;
			if (theVsFlagVector.count() == 1){
				Vs30Flag = theVsFlagVector[0];
			}else {
				Vs30Flag = theVsFlagVector[i];
			}

			double FJP = 0;
			double FTW = 0;
			double FCN = 0;
			if (theRegion == RAbrahamsonEtAl2013IntensityModel::Japan){
				FJP = 1;
			} else if (theRegion == RAbrahamsonEtAl2013IntensityModel::Taiwan){
				FTW = 1;
			} else if (theRegion == RAbrahamsonEtAl2013IntensityModel::China){
				FCN = 1;
			}
		
			RAbrahamsonEtAl2013IntensityModel::RWallType theWallType;
			if (theWallTypes.count() == 1) {
				theWallType = theWallTypes[0];
			} else {
				theWallType = theWallTypes[i];
			}

			double FHW;
			if (theWallType == RAbrahamsonEtAl2013IntensityModel::HangingWall){
				FHW = 1;
			} else {
				FHW = 0;
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
                double Vlin = VlinVector[j];
                double 	b = 	bVector[j];
                double 	c4 = 	c4Vector[j];
                double 	M1 = 	M1Vector[j];
                double 	a1 = 	a1Vector[j];
                double 	a2 = 	a2Vector[j];
                double 	a3 = 	a3Vector[j];
                double 	a6 = 	a6Vector[j];
                double 	a8 = 	a8Vector[j];
                double 	a10 = 	a10Vector[j];
                double 	a11 = 	a11Vector[j];
                double 	a12 = 	a12Vector[j];
                double 	a13 = 	a13Vector[j];
                double 	a14 = 	a14Vector[j];
                double 	a15 = 	a15Vector[j];
                double 	a17 = 	a17Vector[j];
                double 	a25 = 	a25Vector[j];
                double 	a28 = 	a28Vector[j];
                double 	a29 = 	a29Vector[j];
                double 	a31 = 	a31Vector[j];
                double 	a36 = 	a36Vector[j];
                double 	a37 = 	a37Vector[j];
                double 	a38 = 	a38Vector[j];
                double 	a40 = 	a40Vector[j];
                double 	a41 = 	a41Vector[j];
                double 	a42 = 	a42Vector[j];
                double 	a43 = 	a43Vector[j];
                double 	a44 = 	a44Vector[j];
                double 	a45 = 	a45Vector[j];
                double 	a46 = 	a46Vector[j];
                double 	s1 = 	s1Vector[j];
                double 	s2 = 	s2Vector[j];
                double 	s3 = 	s3Vector[j];
                double 	s4 = 	s4Vector[j];
				double  s5JP = s5JPVector[j];
				double  s6JP = s6JPVector[j];
				double  s1KnownVs = s1KnownVsVector[j];
				double  s2KnownVs = s2KnownVsVector[j];

				/*double Ztop ; // USGS recomendation:
				if ( FN==0 && FRV==0  ){
					Ztop = 5.63 + 0.68 * M ;
				}else if ( FN==1 || FRV==1 ){
					Ztop = 11.24 - 0.2 * M ;
				}
				*/

				
				if (Vs30Flag == 1){
					s1 = s1KnownVs;
					s2 = s2KnownVs;
				} // For measured and known Vs30


				double c4M;
				if (M > 5) {
					c4M = c4;
				} else if (M > 4 && M <= 5) {
					c4M = c4 - (c4 - 1) * (5.0 - M);
				} else {
					c4M = 1;
				}


				
				double R =  sqrt ( Rrup * Rrup + c4M * c4M);
			
				double F1;
					if ( M < M2){
					F1 = a1 + a4 * ( M2 - M1 ) + a8 * ( 8.5 - M2 ) * ( 8.5 - M2 ) + a6 * (M - M2) + a7 * (M - M2) * (M - M2) + ( a2 + a3 * (M2 - M1 ))* log(R) + a17 * Rrup;
					} else if ( M1 > M && M2 <= M){
					F1 = a1 + a4 * (M - M1) + a8 * (8.5 - M) * (8.5 - M) + (a2 + a3 * (M - M1)) * log(R) + a17 * Rrup;
					} else {	 
					F1 = a1 + a5 * ( M - M1 ) + a8 * ( 8.5 - M ) * ( 8.5 - M ) + ( a2 + a3 * (M - M1 ))* log(R) + a17 * Rrup;
					}
				
				double F7;
				if ( M <= 4 ){
					F7 = 0;
				}else if (M >= 4 && M <= 5){
					F7 = a11 * ( M - 4 ) ;
				}else {
					F7 = a11;
				}
				
				double F8;
				if ( M > 5 ){
					F8 = a12;
				}else if (M >= 4 && M <= 5){
					F8 = a12 * (M - 4 );
				} else {
					F8 = 0;
				}

				double V1;
				if (T <= 0.5) {
					V1 = 1500.0;
				} else if (T > 0.5 && T <= 3) {
					V1 = exp(-0.35 * log(T / 0.5) + log(1500.0));
				} else {
					V1 = 800;
				}

				// Vsstar30 stands for V*s30 in the paper.
				double Vsstar30;
				if (Vs30 < V1) {
					Vsstar30 = Vs30;
				} else {
					Vsstar30 = V1; 
				}

				
				double Vsstar30_1180;
				if (V1 < 1180 ){
					Vsstar30_1180 = V1;
				} else{
					Vsstar30_1180 = 1180;
				}

				
				double F11;
				if (CRjb <= 5.0){
					F11 = a11;
				} else if (5 < CRjb && CRjb < 15){
					F11 = a14 * (1- (CRjb-5)/10.0);
				} else {
					F11 = 0;
				}
				

				double R1;
				R1 = W * cos(dip /180.0 * 3.1416);

				double R2;
				R2 = 3* R1;
				
				double Ry1;
				Ry1 = Rx * tan(20/180 * 3.1416);

				double h1;
				h1= 0.25;
				
				double h2;
				h2 = 1.5;

				double h3;
				h3 = -0.75;

				double T1;
				if (dip > 30) {
					T1 = (90 - dip) / 45;
				} else {
					T1 = 60.0 / 45.0;
				}

				double T2;
				if (M >= 6.5) {
					T2 = 1 + a2HW * (M - 6.5);
				} else if (M > 5.5 && M <= 6.5) {
					T2 = 1 + a2HW * (M  - 6.5) - (1 - a2HW) * (M - 6.5) * (M - 6.5);
				} else {
					T2 = 0;
				}

				double T3;
				if (Rx < R1) {
					T3 = h1 + h2 * (Rx / R1) + h3 * (Rx / R1)* (Rx / R1);
				} else if (R1 <= Rx && Rx <= R2) {
					T3 = 1 - (Rx - R1) / (R2 - R1);
				} else {
					T3 = 0;
				}

				double T4;
				if (Ztor <= 10) {
					T4 = 1 - Ztor* Ztor / 100.0;
				} else {
					T4 = 0;
				}

				double T5;
				if (Ry0 != 999 ) {
					if (Ry0 < Ry1) {
					T5 = 1;
					} else if ( (Ry0 - Ry1) < 5.0 && (Ry0 - Ry1) >= 0) {
					T5 = 1 - (Ry0 - Ry1) / 5.0;
					} else if ( (Ry0 - Ry1) >= 5.0) {
					T5 = 0;
					}
				} else if (Ry0 == 999) {
					if (Rjb = 0){
						T5 = 1;
					} else if (Rjb < 30) {
						T5 = 1 - Rjb / 30.0;
					} else if (Rjb >= 30) {
						T5 = 0;
					}
				}

				double F4;
				F4 = a13 * T1 *T2 * T3 * T4 * T5;


				double F6;
				if (Ztor < 20) {
					F6 = a15* Ztor /20.0;
				} else {
					F6 = a15;
				}

				double Z1ref;
				if (theRegion == RAbrahamsonEtAl2013IntensityModel::Japan) {
					Z1ref = 1/1000.0 * exp(-5.23/2 *log ( (pow(Vs30 , 2) + pow(412.0 , 2)) / (pow(1360.0 , 2) + pow(412.0 , 2)) ) );
				} else {
					Z1ref = 1/1000.0 * exp(-7.67/4 *log ( (pow(Vs30 , 4) + pow(610.0 , 4)) / (pow(1360.0 , 4) + pow(610.0 , 4)) ) );
				}
				if (Z1 == 999){
					Z1 = Z1ref;
				}

				double F10;
				/* !!! : Dont delete this region, this is original formula of paper 2014! But we use smoothed one...
				if (Vs30 <= 200.0) {
					F10 = a43 * log((Z1+ 0.01) / (Z1ref + 0.01));
				} else if (200 < Vs30 && Vs30 <= 300) { 
					F10 = a44 * log((Z1+ 0.01) / (Z1ref + 0.01));
				} else if (300.0 < Vs30 && Vs30 <= 500) {
					F10 = a45 * log((Z1+ 0.01) / (Z1ref + 0.01));
				} else {
					F10 = a46 * log((Z1+ 0.01) / (Z1ref + 0.01));
				}
				*/

				//Smoothed:
				double y1z;
				double y2z;
				double x1z;
				double x2z;
				if (Vs30 <= 150) {
					y1z = a43;
					y2z = a43;
					x1z = 50;
					x2z = 150;
				}else if ( Vs30 <= 250){
					y1z = a43;
					y2z = a44;
					x1z = 150;
					x2z = 250;
				}else if (Vs30 <= 400){ 
					y1z = a44;
					y2z = a45;
					x1z = 250;
					x2z = 400;
				}else if (Vs30 <= 700 ){
					y1z = a45;
					y2z = a46;
					x1z = 400;
					x2z = 700;
				}else{
					y1z = a46;
					y2z = a46;
					x1z = 700;
					x2z = 1000;
				}
				F10 = (y1z + (Vs30 - x1z) * (y2z - y1z) / (x2z - x1z)) * log((Z1 + 0.01) / (Z1ref + 0.01));


				double F12;
				F12 = a31 * log(Vsstar30 / Vlin);
				double F12_1180 = a31 * log(Vsstar30_1180 / Vlin);

				double F13;
				/* !!!: Dont delete this region, this is original formula of paper 2014! But we use smoothed one...
				if (Vs30 < 200) { 
					F13 = a36;
				} else if (200 <= Vs30 && Vs30 < 300) {
					F13 = a37;
				} else if (300 <= Vs30 && Vs30 < 400) {
					F13 = a38;
				} else if (400 <= Vs30 && Vs30 < 500) {
					F13 = a39;
				} else if (500 <= Vs30 && Vs30 < 700) {
					F13 = a40;
				} else if (700 <= Vs30 && Vs30 < 1000) {
					F13 = a41;
				} else if (1000 <= Vs30) {
					F13 = a41;
				}
				*/

				//Smoothness:
				double y1;
				double y2;
				double x1;
				double x2;
				if (Vs30 < 150) { 
					y1 = a36;
					y2 = a36;
					x1 = 50;
					x2 = 150;
				}else if (Vs30 < 250) { 
					y1 = a36;
					y2 = a37;
					x1 = 150;
					x2 = 250;
				}else if (Vs30 < 350) { 
					y1 = a37;
					y2 = a38;
					x1 = 250;
					x2 = 350;
				}else if (Vs30 < 450) {
					y1 = a38;
					y2 = a39;
					x1 = 350;
					x2 = 450;
				}else if (Vs30 < 600) { 
					y1 = a39;
					y2 = a40;
					x1 = 450;
					x2 = 600;
				} else if (Vs30 < 850) {
					y1 = a40;
					y2 = a41;
					x1 = 600;
					x2 = 850;
				}else if (Vs30 < 1150 ){
					y1 = a41;
					y2 = a42;
					x1 = 850;
					x2 = 1150;
				}else{
					y1 = a42;
					y2 = a42;
					x1 = 1150;
					x2 = 3000;
				}
				F13 = y1 + (y2 - y1) / (x2 - x1) * (Vs30 - x1);


				double Regional;
				Regional = FTW * (F12 + a25* Rrup) + FCN * (a28 * Rrup) + FJP * (F13 + a29 * Rrup);
				double Regional1180 =  FTW * (F12_1180 + a25* Rrup) + FCN * (a28 * Rrup) + FJP * (F13 + a29 * Rrup);


				double Sahat1180;
				Sahat1180 = exp(F1 + FRV * F7 + FN* F8 +((a10 + b* n) * log(Vsstar30_1180 / Vlin)) + FHW * F4 + F6 +Regional1180);


				double F5;
				if (Vs30 < Vlin) {
					F5 = a10 * log(Vsstar30 / Vlin) - b * log(Sahat1180 + c) + b * log(Sahat1180 + c * pow((Vsstar30 / Vlin) , n));
				} else {
					F5 = (a10 + b* n) * log(Vsstar30 / Vlin);
				}


				double phiAL;
				if (M < 4){
					phiAL = s1;
				}else if (M >= 4 && M <= 6){
					 phiAL = s1 + (s2 - s1) / 2 * (M - 4);
				}else{
					phiAL = s2;
				}

				double tauAL;
				if (M < 5){
					tauAL = s3;
				}else if (M <= 7){
					tauAL = s3 + (s4 - s3) / 2 *(M - 5);
				}else {
					tauAL = s4;
				}
				double tauB = tauAL;

				if (FJP == 1){
					if (Rrup < 30 ){
						phiAL = s5JP;
					}else if (Rrup <= 80){
						phiAL = s5JP + (s6JP - s5JP)/50 * (Rrup - 30);
					}else {
						phiAL = s6JP;
					}
				}
				double phiamp = 0.4;
				double phiB = sqrt(phiAL * phiAL - phiamp * phiamp);
				double dln;
				if (Vs30 >= Vlin){
					dln = 0;
				}else{
					dln = -b * Sahat1180/(Sahat1180 + c) + b*Sahat1180/(Sahat1180 + c * pow((Vs30/Vlin) , n));
				}

				double phi = sqrt(phiB*phiB * (1 + dln)*(1 + dln) + phiamp*phiamp);

				double tau = tauB*(1+ dln);

				double lnY;
				lnY = F1 + FRV * F7 + FN * F8 + FAS * F11 + F5 + FHW * F4 + F6 + F10 + Regional + tau *eta + epsilon * phi ;
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
		theSpectralResponseList[i]->setCurrentValue(spectralResponseVector[i], threadID);
	}

	return 0;
}

bool RAbrahamsonEtAl2013IntensityModel::canBeRunParallelly(){
	return true;
}
