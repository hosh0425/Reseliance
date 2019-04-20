#include "RCampbellBozorgnia2013IntensityModel.h"	

#include <QCoreApplication>
#include <QMetaEnum>
	
#include "RParameter.h"	
#include "RResponse.h"	
#include "RLocation.h"
#include "RLocationResponse.h"
#include "RMainWindow.h"

#include <math.h>

#include <gsl/gsl_math.h>
	
RCampbellBozorgnia2013IntensityModel::RCampbellBozorgnia2013IntensityModel(QObject *parent, QString name)	
	: RModel(parent, name)
{	
	//theEpsilon = 0;
	theResponseType = RCampbellBozorgnia2013IntensityModel::Sa;
}	
	
RCampbellBozorgnia2013IntensityModel::~RCampbellBozorgnia2013IntensityModel()	
{	
	
}	

QString RCampbellBozorgnia2013IntensityModel::getMagnitudeList()	
{	
	return parameterListToString(theMagnitudeList);
}	

void RCampbellBozorgnia2013IntensityModel::setMagnitudeList(QString value)	
{	
	theMagnitudeList = stringToParameterList(value);
}

QString RCampbellBozorgnia2013IntensityModel::getDepthToTopOfRuptureList() 	
{	
	return parameterListToString(theDepthToTopOfRuptureList);
}	
	
void RCampbellBozorgnia2013IntensityModel::setDepthToTopOfRuptureList(QString value)	
{	
	theDepthToTopOfRuptureList = stringToParameterList(value);
}

QString RCampbellBozorgnia2013IntensityModel::getDownDipRupWidthList() 	
{	
	return parameterListToString(theDownDipRupWidthList);
}	
	
void RCampbellBozorgnia2013IntensityModel::setDownDipRupWidthList(QString value)	
{	
	theDownDipRupWidthList = stringToParameterList(value);
}

QString RCampbellBozorgnia2013IntensityModel::getHrzDistTopOfRupList() 	
{	
	return parameterListToString(theHrzDistTopOfRupList);
}	
	
void RCampbellBozorgnia2013IntensityModel::setHrzDistTopOfRupList(QString value)	
{	
	theHrzDistTopOfRupList = stringToParameterList(value);
}

QString RCampbellBozorgnia2013IntensityModel::getRupDistList() 	
{	
	return parameterListToString(theRupDistList);
}	
	
void RCampbellBozorgnia2013IntensityModel::setRupDistList(QString value)	
{	
	theRupDistList = stringToParameterList(value);
}

QString RCampbellBozorgnia2013IntensityModel::getDipAngles() 	
{	
	return doubleVectorToString(theDipAngleVector);
}	
	
void RCampbellBozorgnia2013IntensityModel::setDipAngles(QString value)	
{	
	theDipAngleVector = stringToDoubleVector(value);;
}

QString RCampbellBozorgnia2013IntensityModel::getSedimentFlags() 	
{	
	return doubleVectorToString(theSedimentFlagVector);
}	
	
void RCampbellBozorgnia2013IntensityModel::setSedimentFlags(QString value)	
{	
	theSedimentFlagVector = stringToDoubleVector(value);;
}

QString RCampbellBozorgnia2013IntensityModel::getHypocentreLocationList()	
{	
	return parameterListToString(theHypocentreLocationList);
}	

void RCampbellBozorgnia2013IntensityModel::setHypocentreLocationList(QString value)	
{	
	theHypocentreLocationList = stringToParameterList(value);

	// Checking if all the passed parameters are either RLocation or RLocationResponse
	parameterListToLocationList(theHypocentreLocationList);
}

QString RCampbellBozorgnia2013IntensityModel::getFaultTypes() const	
{
	QMetaEnum metaEnum = metaObject()->enumerator(metaObject()->indexOfEnumerator("RFaultType"));
	QString result = "";
	for (int i = 0; i < theFaultTypes.count(); i++) {
		QString temp = metaEnum.valueToKey(theFaultTypes[i]);
	    result += temp + "; ";
	}
	return result;
}	
	

void RCampbellBozorgnia2013IntensityModel::setFaultTypes(QString value)	
{	
	QString string = value;
	QMetaEnum metaEnum = metaObject()->enumerator(metaObject()->indexOfEnumerator("RFaultType"));

	QRegExp regExp = QRegExp(QString("[ ,;:\t\n\v\r%1%2]").arg(QChar(QChar::ParagraphSeparator)).arg(QChar(QChar::LineSeparator)));
	QStringList stringList = string.split(regExp, QString::SkipEmptyParts);
	string = "";
	QList<RCampbellBozorgnia2013IntensityModel::RFaultType> tempList;
	for (int i = 0; i < stringList.count(); i++) {
		QByteArray ba = stringList[i].toLocal8Bit();
		const char *tempChar = ba.data();
		int tempValue = metaEnum.keyToValue(tempChar);
		RCampbellBozorgnia2013IntensityModel::RFaultType temp = RFaultType(tempValue);
		if (temp < 0) {
			rCritical() << "Error in the Model" << objectName() << ": The fault type" << stringList[i] << "is not valid";
			return;
		}
		tempList.append(temp);
	}
	theFaultTypes = tempList;
}	

QString RCampbellBozorgnia2013IntensityModel::getWallTypes() const	
{
	QMetaEnum metaEnum = metaObject()->enumerator(metaObject()->indexOfEnumerator("RWallType"));
	QString result = "";
	for (int i = 0; i < theWallTypes.count(); i++) {
		QString temp = metaEnum.valueToKey(theWallTypes[i]);
	    result += temp + "; ";
	}
	return result;
}	
	

void RCampbellBozorgnia2013IntensityModel::setWallTypes(QString value)	
{	
	QString string = value;
	QMetaEnum metaEnum = metaObject()->enumerator(metaObject()->indexOfEnumerator("RWallType"));

	QRegExp regExp = QRegExp(QString("[ ,;:\t\n\v\r%1%2]").arg(QChar(QChar::ParagraphSeparator)).arg(QChar(QChar::LineSeparator)));
	QStringList stringList = string.split(regExp, QString::SkipEmptyParts);
	string = "";
	QList<RCampbellBozorgnia2013IntensityModel::RWallType> tempList;
	for (int i = 0; i < stringList.count(); i++) {
		QByteArray ba = stringList[i].toLocal8Bit();
		const char *tempChar = ba.data();
		int tempValue = metaEnum.keyToValue(tempChar);
		RCampbellBozorgnia2013IntensityModel::RWallType temp = RWallType(tempValue);
		if (temp < 0) {
			rCritical() << "Error in the Model" << objectName() << ": The wall type" << stringList[i] << "is not valid";
			return;
		}
		tempList.append(temp);
	}
	theWallTypes = tempList;
}	

QString RCampbellBozorgnia2013IntensityModel::getRegions() const	
{
	QMetaEnum metaEnum = metaObject()->enumerator(metaObject()->indexOfEnumerator("RRegion"));
	QString result = "";
	for (int i = 0; i < theRegions.count(); i++) {
		QString temp = metaEnum.valueToKey(theRegions[i]);
	    result += temp + "; ";
	}
	return result;
}	
	

void RCampbellBozorgnia2013IntensityModel::setRegions(QString value)	
{	
	QString string = value;
	QMetaEnum metaEnum = metaObject()->enumerator(metaObject()->indexOfEnumerator("RRegion"));

	QRegExp regExp = QRegExp(QString("[ ,;:\t\n\v\r%1%2]").arg(QChar(QChar::ParagraphSeparator)).arg(QChar(QChar::LineSeparator)));
	QStringList stringList = string.split(regExp, QString::SkipEmptyParts);
	string = "";
	QList<RCampbellBozorgnia2013IntensityModel::RRegion> tempList;
	for (int i = 0; i < stringList.count(); i++) {
		QByteArray ba = stringList[i].toLocal8Bit();
		const char *tempChar = ba.data();
		int tempValue = metaEnum.keyToValue(tempChar);
		RCampbellBozorgnia2013IntensityModel::RRegion temp = RRegion(tempValue);
		if (temp < 0) {
			rCritical() << "Error in the Model" << objectName() << ": The region" << stringList[i] << "is not valid";
			return;
		}
		tempList.append(temp);
	}
	theRegions = tempList;
}	


RCampbellBozorgnia2013IntensityModel::RResponseType RCampbellBozorgnia2013IntensityModel::getResponseType() const	
{	
	return theResponseType;
}	
	
void RCampbellBozorgnia2013IntensityModel::setResponseType(RCampbellBozorgnia2013IntensityModel::RResponseType value)	
{	
	theResponseType = value;
}

RCampbellBozorgnia2013IntensityModel::RShockType RCampbellBozorgnia2013IntensityModel::getShockType() const	
{	
	return theShockType;
}	
	
void RCampbellBozorgnia2013IntensityModel::setShockType(RCampbellBozorgnia2013IntensityModel::RShockType value)	
{	
	theShockType = value;
}

QString RCampbellBozorgnia2013IntensityModel::getPeriodList()	
{	
	return parameterListToString(thePeriodList);
}	
	
void RCampbellBozorgnia2013IntensityModel::setPeriodList(QString value)	
{	
	thePeriodList = stringToParameterList(value);

	createResponses();
}	


QString RCampbellBozorgnia2013IntensityModel::getEpsilonUncertaintyList()	
{	
	return parameterListToString(theEpsilonUncertaintyList);
}	
	
void RCampbellBozorgnia2013IntensityModel::setEpsilonUncertaintyList(QString value)	
{	
	theEpsilonUncertaintyList = stringToParameterList(value);

	createResponses();
}	


QString RCampbellBozorgnia2013IntensityModel::getEtaUncertaintyList()	
{	
	return parameterListToString(theEtaUncertaintyList);
}	
	
void RCampbellBozorgnia2013IntensityModel::setEtaUncertaintyList(QString value)	
{	
	theEtaUncertaintyList = stringToParameterList(value);

	createResponses();
}	


QList<RParameter *> RCampbellBozorgnia2013IntensityModel::getPhysicalParameterList()
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

QString RCampbellBozorgnia2013IntensityModel::getStructureLocationList()	
{	
	return parameterListToString(theStructureLocationList);
}	
	
void RCampbellBozorgnia2013IntensityModel::setStructureLocationList(QString value)	
{	
	theStructureLocationList = stringToParameterList(value);

	// Checking if all the passed parameters are either RLocation or RLocationResponse
	parameterListToLocationList(theStructureLocationList);

	createResponses();
}	
	
QString RCampbellBozorgnia2013IntensityModel::getShearWaveVelocityList()	
{	
	return parameterListToString(theShearWaveVelocityList);
}	
	
void RCampbellBozorgnia2013IntensityModel::setShearWaveVelocityList(QString value)	
{	
	theShearWaveVelocityList = stringToParameterList(value);

	createResponses();
}	

QString RCampbellBozorgnia2013IntensityModel::getDepthToVs1List()	
{	
	return parameterListToString(theDepthToVs1List);
}	
	
void RCampbellBozorgnia2013IntensityModel::setDepthToVs1List(QString value)	
{	
	theDepthToVs1List = stringToParameterList(value);

	createResponses();
}	
void RCampbellBozorgnia2013IntensityModel::createResponses()
{
	// Removing old responses
	while (theSpectralResponseList.count() > 0) {
		delete theSpectralResponseList.takeAt(0);
	}

	if (theStructureLocationList.isEmpty() || theShearWaveVelocityList.isEmpty()) {
		return;
	}

	if ((theStructureLocationList.count() != thePeriodList.count() && theResponseType == RCampbellBozorgnia2013IntensityModel::Sa) || (theStructureLocationList.count() != theShearWaveVelocityList.count() && theShearWaveVelocityList.count() > 1) || (theStructureLocationList.count() != theEpsilonUncertaintyList.count() && theEpsilonUncertaintyList.count() > 1) || (theStructureLocationList.count() != theEtaUncertaintyList.count() && theEtaUncertaintyList.count() > 1)) {
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

int RCampbellBozorgnia2013IntensityModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
{	
	if (theStructureLocationList.isEmpty()) {
		rCritical(threadID) << "Error: There should be at least one structure location in the model" << objectName() << ".";
		return -1;
	}
	if ((theStructureLocationList.count() != thePeriodList.count() && theResponseType == RCampbellBozorgnia2013IntensityModel::Sa) || (theStructureLocationList.count() != theShearWaveVelocityList.count() && theShearWaveVelocityList.count() > 1)) {
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
	QVector<double> c13Vector;
	QVector<double> c14Vector;
	QVector<double> c15Vector;
	QVector<double> c16Vector;
	QVector<double> c17Vector;
	QVector<double> c18Vector;
	QVector<double> c19Vector;
	QVector<double> c20Vector;
	QVector<double> Deltac20CAVector;
	QVector<double> Deltac20JPVector;
	QVector<double> Deltac20CHVector;
	QVector<double> a2Vector;
	QVector<double> h1Vector;
	QVector<double> h2Vector;
	QVector<double> h3Vector;
	QVector<double> h4Vector;
	QVector<double> h5Vector;
	QVector<double> h6Vector;
	QVector<double> k1Vector;
	QVector<double> k2Vector;
	QVector<double> k3Vector;
	QVector<double> phi1Vector;
	QVector<double> phi2Vector;
	QVector<double> tau1Vector;
	QVector<double> tau2Vector;
	QVector<double> phicVector;
	QVector<double> sigmaM45Vector;
	QVector<double> sigmaM55Vector;
	QVector<double> rholnPGAlnYVector;

	periodTableVector << -1 << 0 << 0.01 << 0.02 << 0.03 << 0.05 << 0.075 << 0.1 << 0.15 << 0.2 << 0.25 << 0.3 << 0.4 << 0.5 << 0.75 << 1 << 1.5 << 2 << 3 << 4 << 5 << 7.5 << 10;
	c0Vector  << -2.895 << -4.416 << -4.365 << -4.348 << -4.024 << -3.479 << -3.293 << -3.666 << -4.866 << -5.411 << -5.962 << -6.403 << -7.566 << -8.379 << -9.841 << -11.011 << -12.469 << -12.969 << -13.306 << -14.02 << -14.558 << -15.509 << -15.975;
	c1Vector  << 1.51 << 0.984 << 0.977 << 0.976 << 0.931 << 0.887 << 0.902 << 0.993 << 1.267 << 1.366 << 1.458 << 1.528 << 1.739 << 1.872 << 2.021 << 2.18 << 2.27 << 2.271 << 2.15 << 2.132 << 2.116 << 2.223 << 2.132;
	c2Vector  << 0.27 << 0.537 << 0.533 << 0.549 << 0.628 << 0.674 << 0.726 << 0.698 << 0.51 << 0.447 << 0.274 << 0.193 << -0.02 << -0.121 << -0.042 << -0.069 << 0.047 << 0.149 << 0.368 << 0.726 << 1.027 << 0.169 << 0.367 ;
	c3Vector  << -1.299 << -1.499 << -1.485 << -1.488 << -1.494 << -1.388 << -1.469 << -1.572 << -1.669 << -1.75 << -1.711 << -1.77 << -1.594 << -1.577 << -1.757 << -1.707 << -1.621 << -1.512 << -1.315 << -1.506 << -1.721 << -0.756 << -0.8 ;
	c4Vector  << -0.453 << -0.496 << -0.499 << -0.501 << -0.517 << -0.615 << -0.596 << -0.536 << -0.49 << -0.451 << -0.404 << -0.321 << -0.426 << -0.44 << -0.443 << -0.527 << -0.63 << -0.768 << -0.89 << -0.885 << -0.878 << -1.077 << -1.282;
	c5Vector  << -2.466 << -2.773 << -2.773 << -2.772 << -2.782 << -2.791 << -2.745 << -2.633 << -2.458 << -2.421 << -2.392 << -2.376 << -2.303 << -2.296 << -2.232 << -2.158 << -2.063 << -2.104 << -2.051 << -1.986 << -2.021 << -2.179 << -2.244;
	c6Vector  << 0.204 << 0.248 << 0.248 << 0.247 << 0.246 << 0.24 << 0.227 << 0.21 << 0.183 << 0.182 << 0.189 << 0.195 << 0.185 << 0.186 << 0.186 << 0.169 << 0.158 << 0.158 << 0.148 << 0.135 << 0.135 << 0.165 << 0.18;
	c7Vector  << 5.837 << 6.768 << 6.753 << 6.502 << 6.291 << 6.317 << 6.861 << 7.294 << 8.031 << 8.385 << 7.534 << 6.99 << 7.012 << 6.902 << 5.522 << 5.65 << 5.795 << 6.632 << 6.759 << 7.978 << 8.538 << 8.468 << 6.564;
	c8Vector  << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0;
	c9Vector  << -0.168 << -0.212 << -0.214 << -0.208 << -0.213 << -0.244 << -0.266 << -0.229 << -0.211 << -0.163 << -0.15 << -0.131 << -0.159 << -0.153 << -0.09 << -0.105 << -0.058 << -0.028 << 0 << 0 << 0 << 0 << 0;
	c10Vector  << 0.305 << 0.72 << 0.72 << 0.73 << 0.759 << 0.826 << 0.815 << 0.831 << 0.749 << 0.764 << 0.716 << 0.737 << 0.738 << 0.718 << 0.795 << 0.556 << 0.48 << 0.401 << 0.206 << 0.105 << 0 << 0 << 0;
	c11Vector  << 1.713 << 1.09 << 1.094 << 1.149 << 1.29 << 1.449 << 1.535 << 1.615 << 1.877 << 2.069 << 2.205 << 2.306 << 2.398 << 2.355 << 1.995 << 1.447 << 0.33 << -0.514 << -0.848 << -0.793 << -0.748 << -0.664 << -0.576;
	c12Vector  << 2.602 << 2.186 << 2.191 << 2.189 << 2.164 << 2.138 << 2.446 << 2.969 << 3.544 << 3.707 << 3.343 << 3.334 << 3.544 << 3.016 << 2.616 << 2.47 << 2.108 << 1.327 << 0.601 << 0.568 << 0.356 << 0.075 << -0.027;
	c13Vector  << 2.457 << 1.42 << 1.416 << 1.453 << 1.476 << 1.549 << 1.772 << 1.916 << 2.161 << 2.465 << 2.766 << 3.011 << 3.203 << 3.333 << 3.054 << 2.562 << 1.453 << 0.657 << 0.367 << 0.306 << 0.268 << 0.374 << 0.297;
	c14Vector  << 0.106 << -0.0064 << -0.007 << -0.0167 << -0.0422 << -0.0663 << -0.0794 << -0.0294 << 0.0642 << 0.0968 << 0.1441 << 0.1597 << 0.141 << 0.1474 << 0.1764 << 0.2593 << 0.2881 << 0.3112 << 0.3478 << 0.3747 << 0.3382 << 0.3754 << 0.3506;
	c15Vector  << 0.332 << -0.202 << -0.207 << -0.199 << -0.202 << -0.339 << -0.404 << -0.416 << -0.407 << -0.311 << -0.172 << -0.084 << 0.085 << 0.233 << 0.411 << 0.479 << 0.566 << 0.562 << 0.534 << 0.522 << 0.477 << 0.321 << 0.174;
	c16Vector  << 0.585 << 0.393 << 0.39 << 0.387 << 0.378 << 0.295 << 0.322 << 0.384 << 0.417 << 0.404 << 0.466 << 0.528 << 0.54 << 0.638 << 0.776 << 0.771 << 0.748 << 0.763 << 0.686 << 0.691 << 0.67 << 0.757 << 0.621;
	c17Vector  << 0.0517 << 0.0977 << 0.0981 << 0.1009 << 0.1095 << 0.1226 << 0.1165 << 0.0998 << 0.076 << 0.0571 << 0.0437 << 0.0323 << 0.0209 << 0.0092 << -0.0082 << -0.0131 << -0.0187 << -0.0258 << -0.0311 << -0.0413 << -0.0281 << -0.0205 << 0.0009;
	c18Vector  << 0.0327 << 0.0333 << 0.0334 << 0.0327 << 0.0331 << 0.027 << 0.0288 << 0.0325 << 0.0388 << 0.0437 << 0.0463 << 0.0508 << 0.0432 << 0.0405 << 0.042 << 0.0426 << 0.038 << 0.0252 << 0.0236 << 0.0102 << 0.0034 << 0.005 << 0.0099;
	c19Vector  << 0.00613 << 0.00757 << 0.00755 << 0.00759 << 0.0079 << 0.00803 << 0.00811 << 0.00744 << 0.00716 << 0.00688 << 0.00556 << 0.00458 << 0.00401 << 0.00388 << 0.0042 << 0.00409 << 0.00424 << 0.00448 << 0.00345 << 0.00603 << 0.00805 << 0.0028 << 0.00458;
	c20Vector  << -0.0017 << -0.0055 << -0.0055 << -0.0055 << -0.0057 << -0.0063 << -0.007 << -0.0073 << -0.0069 << -0.006 << -0.0055 << -0.0049 << -0.0037 << -0.0027 << -0.0016 << -0.0006 << 0 << 0 << 0 << 0 << 0 << 0 << 0;
	Deltac20CAVector  << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0;
	Deltac20JPVector  << -0.0006 << -0.0035 << -0.0035 << -0.0035 << -0.0034 << -0.0037 << -0.0037 << -0.0034 << -0.003 << -0.0031 << -0.0033 << -0.0035 << -0.0034 << -0.0034 << -0.0032 << -0.003 << -0.0019 << -0.0005 << 0 << 0 << 0 << 0 << 0;
	Deltac20CHVector  << 0.0017 << 0.0036 << 0.0036 << 0.0036 << 0.0037 << 0.004 << 0.0039 << 0.0042 << 0.0042 << 0.0041 << 0.0036 << 0.0031 << 0.0028 << 0.0025 << 0.0016 << 0.0006 << 0 << 0 << 0 << 0 << 0 << 0 << 0;
	a2Vector  << 0.596 << 0.167 << 0.168 << 0.166 << 0.167 << 0.173 << 0.198 << 0.174 << 0.198 << 0.204 << 0.185 << 0.164 << 0.16 << 0.184 << 0.216 << 0.596 << 0.596 << 0.596 << 0.596 << 0.596 << 0.596 << 0.596 << 0.596;
	h1Vector  << 0.117 << 0.241 << 0.242 << 0.244 << 0.246 << 0.251 << 0.26 << 0.259 << 0.254 << 0.237 << 0.206 << 0.21 << 0.226 << 0.217 << 0.154 << 0.117 << 0.117 << 0.117 << 0.117 << 0.117 << 0.117 << 0.117 << 0.117;
	h2Vector  << 1.616 << 1.474 << 1.471 << 1.467 << 1.467 << 1.449 << 1.435 << 1.449 << 1.461 << 1.484 << 1.581 << 1.586 << 1.544 << 1.554 << 1.626 << 1.616 << 1.616 << 1.616 << 1.616 << 1.616 << 1.616 << 1.616 << 1.616;
	h3Vector  << -0.733 << -0.715 << -0.714 << -0.711 << -0.713 << -0.701 << -0.695 << -0.708 << -0.715 << -0.721 << -0.787 << -0.795 << -0.77 << -0.77 << -0.78 << -0.733 << -0.733 << -0.733 << -0.733 << -0.733 << -0.733 << -0.733 << -0.733;
	h4Vector  << 1 << 1 << 1 << 1 << 1 << 1 << 1 << 1 << 1 << 1 << 1 << 1 << 1 << 1 << 1 << 1 << 1 << 1 << 1 << 1 << 1 << 1 << 1;
	h5Vector  << -0.128 << -0.337 << -0.336 << -0.339 << -0.338 << -0.338 << -0.347 << -0.391 << -0.449 << -0.393 << -0.339 << -0.447 << -0.525 << -0.407 << -0.371 << -0.128 << -0.128 << -0.128 << -0.128 << -0.128 << -0.128 << -0.128 << -0.128;
	h6Vector  << -0.756 << -0.27 << -0.27 << -0.263 << -0.259 << -0.263 << -0.219 << -0.201 << -0.099 << -0.198 << -0.21 << -0.121 << -0.086 << -0.281 << -0.285 << -0.756 << -0.756 << -0.756 << -0.756 << -0.756 << -0.756 << -0.756 << -0.756;
	k1Vector  << 400 << 865 << 865 << 865 << 908 << 1054 << 1086 << 1032 << 878 << 748 << 654 << 587 << 503 << 457 << 410 << 400 << 400 << 400 << 400 << 400 << 400 << 400 << 400;
	k2Vector  << -1.955 << -1.186 << -1.186 << -1.219 << -1.273 << -1.346 << -1.471 << -1.624 << -1.931 << -2.188 << -2.381 << -2.518 << -2.657 << -2.669 << -2.401 << -1.955 << -1.025 << -0.299 << 0 << 0 << 0 << 0 << 0;
	k3Vector  << 1.929 << 1.839 << 1.839 << 1.84 << 1.841 << 1.843 << 1.845 << 1.847 << 1.852 << 1.856 << 1.861 << 1.865 << 1.874 << 1.883 << 1.906 << 1.929 << 1.974 << 2.019 << 2.11 << 2.2 << 2.291 << 2.517 << 2.744;
	phi1Vector  << 0.655 << 0.734 << 0.734 << 0.738 << 0.747 << 0.777 << 0.782 << 0.769 << 0.769 << 0.761 << 0.744 << 0.727 << 0.69 << 0.663 << 0.606 << 0.579 << 0.541 << 0.529 << 0.527 << 0.521 << 0.502 << 0.457 << 0.441;
	phi2Vector  << 0.494 << 0.492 << 0.492 << 0.496 << 0.503 << 0.52 << 0.535 << 0.543 << 0.543 << 0.552 << 0.545 << 0.568 << 0.593 << 0.611 << 0.633 << 0.628 << 0.603 << 0.588 << 0.578 << 0.559 << 0.551 << 0.546 << 0.543;
	tau1Vector  << 0.317 << 0.409 << 0.404 << 0.417 << 0.446 << 0.508 << 0.504 << 0.445 << 0.382 << 0.339 << 0.34 << 0.34 << 0.356 << 0.379 << 0.43 << 0.47 << 0.497 << 0.499 << 0.5 << 0.543 << 0.534 << 0.523 << 0.466;
	tau2Vector  << 0.297 << 0.322 << 0.325 << 0.326 << 0.344 << 0.377 << 0.418 << 0.426 << 0.387 << 0.338 << 0.316 << 0.3 << 0.264 << 0.263 << 0.326 << 0.353 << 0.399 << 0.4 << 0.417 << 0.393 << 0.421 << 0.438 << 0.438;
	phicVector  << 0.19 << 0.166 << 0.166 << 0.166 << 0.165 << 0.162 << 0.158 << 0.17 << 0.18 << 0.186 << 0.191 << 0.198 << 0.206 << 0.208 << 0.221 << 0.225 << 0.222 << 0.226 << 0.229 << 0.237 << 0.237 << 0.271 << 0.29;
	sigmaM45Vector  << 0.728 << 0.84 << 0.838 << 0.848 << 0.87 << 0.928 << 0.93 << 0.888 << 0.859 << 0.833 << 0.818 << 0.803 << 0.776 << 0.764 << 0.743 << 0.746 << 0.735 << 0.727 << 0.726 << 0.753 << 0.733 << 0.695 << 0.642;
	sigmaM55Vector  << 0.576 << 0.588 << 0.59 << 0.594 << 0.609 << 0.642 << 0.679 << 0.69 << 0.667 << 0.647 << 0.63 << 0.642 << 0.649 << 0.665 << 0.712 << 0.72 << 0.723 << 0.711 << 0.713 << 0.683 << 0.693 << 0.7 << 0.698;
	rholnPGAlnYVector  << 0.684 << 1 << 1 << 0.998 << 0.986 << 0.938 << 0.887 << 0.87 << 0.876 << 0.87 << 0.85 << 0.819 << 0.743 << 0.684 << 0.562 << 0.467 << 0.364 << 0.298 << 0.234 << 0.202 << 0.184 << 0.176 << 0.154;
	
	double c = 1.88;
	double n = 1.18;
	double philnAF = 0.3;

	
	double T;
	if (theResponseType == RCampbellBozorgnia2013IntensityModel::PGA) {
		T = 0.0;
	} else if (theResponseType == RCampbellBozorgnia2013IntensityModel::PGV) {
		T = -1.0;
	} 


	QVector<double> spectralResponseVector;
	for (int i = 0; i < locationList.count(); i++) {
		spectralResponseVector << 0.0;
	}

	for (int index = 0; index < theMagnitudeList.count(); index++) {
		
		
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

		double FN = 0.0;
		double FRV = 0.0;
		double WUSGS;
		RCampbellBozorgnia2013IntensityModel::RFaultType theFaultType;
			if (theFaultTypes.count() == 1) {
				theFaultType = theFaultTypes[0];
			} else {
				theFaultType = theFaultTypes[index];
			}
		if (theFaultType == RCampbellBozorgnia2013IntensityModel::StrikeSlip) {
			FN = 0;
			FRV = 0;
			WUSGS = pow(10.0 , (-0.76 + 0.27* M) );
		} else if (theFaultType == RCampbellBozorgnia2013IntensityModel::NormalSlip) {
			FN = 1.0;
			WUSGS = pow(10.0 , (-1.14 + 0.35* M) );
		} else if (theFaultType == RCampbellBozorgnia2013IntensityModel::ReverseSlip) {
			FRV = 1.0;
			WUSGS = pow(10.0 , (-1.61 + 0.41* M) );
		}	
	
		

		for (int i = 0; i < locationList.count(); i++) {
			double Rjb = theHypocentreLocationObject->computeSurfaceDistance(locationList[i], threadID);
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
			double Ztori;
			if (FRV == 1){
				Ztori = pow( std::max(2.704 - 1.226 * std::max(M-5.849 , 0.0) , 0.0) , 2.0);
			}else{
				Ztori = pow(std::max(2.673 - 1.136 * std::max(M-4.970 , 0.0) , 0.0) , 2.0);
			}
			if (Ztor == 999){//Baker's code...
			Ztor = Ztori;
			}
			/* //USGS recomendation:
			if (Ztor == 999){
				if ( FN==0 && FRV==0 ){
					Ztor = 5.63 + 0.68 * M ;
				}else if ( FN==1 || FRV==1 ){
					Ztor = 11.24 - 0.2 * M ;
				}
			}
			*/		
			double W;
			if (theDownDipRupWidthList.count () == 1){
				W = theDownDipRupWidthList[0].data()->getCurrentValue(threadID);
			}else {
				W = theDownDipRupWidthList[index].data()->getCurrentValue(threadID);
			}
			double ZHYP;
			double W_ind = (W == 999);
			if (W == 999){
				// W recomended in the 2014 paper:
				double Zbot = 15; //The default value of NGA-W2 excel code....!!!
				W = std::min( (sqrt( pow(10.0, ((M - 4.07) / 0.98) ))),((Zbot - Ztori) / sin(3.1416 / 180 * dip)));   // !:It needs to get Zbot as iput.   
				ZHYP = 9;//Baker's code...
				//W = std::min(18.0 / sin( 3.1416 / 180.0 * dip),pow(10.0 ,(-1.75 + 0.45 * M)) ); // !: ***Copy of the formula of ASK14 code ... (Is this right?)
			} /*else if (W == -999 || theDownDipRupWidthList.isEmpty () == 1){
				W = WUSGS;
			}
			*/

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





			if (theResponseType == RCampbellBozorgnia2013IntensityModel::Sa) {
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
			
			double SedFlag;
			if (theSedimentFlagVector.count() == 1){
				SedFlag = theSedimentFlagVector[0];
			}else {
				SedFlag = theSedimentFlagVector[i];
			}


			RCampbellBozorgnia2013IntensityModel::RRegion theRegion;
			if (theRegions.count() == 1) {
				theRegion = theRegions[0];
			} else {
				theRegion = theRegions[i];
			}

			
			RCampbellBozorgnia2013IntensityModel::RWallType theWallType;
			if (theWallTypes.count() == 1) {
				theWallType = theWallTypes[0];
			} else {
				theWallType = theWallTypes[index];
			}

			double FHW;
			if (theWallTypes.isEmpty() == 1){
				FHW =0;
			}
			if (theWallType == RCampbellBozorgnia2013IntensityModel::HangingWall){
				FHW = 1;
			} else {
				FHW = 0;
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
				double c13 = c13Vector[j];
				double c14 = c14Vector[j];
				double c15 = c15Vector[j];
				double c16 = c16Vector[j];
				double c17 = c17Vector[j];
				double c18 = c18Vector[j];
				double c19 = c19Vector[j];
				double c20 = c20Vector[j];
				double Deltac20CA = Deltac20CAVector[j];
				double Deltac20JP = Deltac20JPVector[j];
				double Deltac20CH = Deltac20CHVector[j];
				double a2 = a2Vector[j];
				double h1 = h1Vector[j];
				double h2 = h2Vector[j];
				double h3 = h3Vector[j];
				double h4 = h4Vector[j];
				double h5 = h5Vector[j];
				double h6 = h6Vector[j];
				double k1 = k1Vector[j];
				double k2 = k2Vector[j];
				double k3 = k3Vector[j];
				double phi1 = phi1Vector[j];
				double phi2 = phi2Vector[j];
				double tau1 = tau1Vector[j];
				double tau2 = tau2Vector[j];
				double phic = phicVector[j];
				double rholnPGAlnY = rholnPGAlnYVector[j];
				
				double sigma;
				if (M <= 4.5) {
					sigma = sigmaM45Vector[j];
				} else if (M >= 5.5) {
					sigma = sigmaM55Vector[j];
				} else {
					sigma = ( (sigmaM55Vector[j] - sigmaM45Vector[j]) / 1) * (M - 4.5) + sigmaM45Vector[j] ;
				}


				double SJ = 0;
				double Deltac20;
				if (theRegion == RCampbellBozorgnia2013IntensityModel::Japan){
					Deltac20 = Deltac20JP;
					SJ = 1;
				} else if (theRegion == RCampbellBozorgnia2013IntensityModel::China){
					Deltac20 = Deltac20CH;
				} else {
					Deltac20 = Deltac20CA;
				}
				
				double FM ;
				if (M <= 4.5){
					FM = c0 + c1 * M;
				}else if (M > 4.5 && M<= 5.5){
					FM = c0 + c1 * M + c2 * ( M - 4.5) ;
				}else if (M > 5.5 && M<= 6.5){
					FM = c0 + c1 * M + c2 * ( M - 4.5) + c3 * (M-5.5) ;
				}else if (M > 6.5){
					FM = c0 + c1 * M + c2 * ( M - 4.5 ) + c3 * (M - 5.5) + c4 * (M - 6.5) ;
				}
				double FD;
				FD = (c5 + c6 * M) * log( sqrt(Rrup*Rrup + c7*c7) );

				double FfltM;
				if (M <= 4.5) {
					FfltM = 0;
				} else if (4.5 < M && M <= 5.5) {
					FfltM = M-4.5;
				} else {
					FfltM = 1;
				}
				double FfltF = c8 * FRV + c9 * FN;
				double FF = FfltF * FfltM;

				double Fhngdelta = (90 - dip) / 45;
				double FhngZ;
				if (Ztor <= 16.66) {
					FhngZ = 1 - 0.06 * Ztor;
				} else {
					FhngZ = 0;
				}
				double FhngM;
				if (M <= 5.5) {
					FhngM = 0;
				} else if (5.5 < M && M <= 6.5) {
					FhngM = (M - 5.5) * (1 + a2 * (M - 6.5) );
				} else {
					FhngM = 1 + a2 * (M - 6.5);
				}
				double FhngRrup;
				if (Rrup == 0) {
					FhngRrup = 1;
				} else if (Rrup > 0) {
					FhngRrup = (Rrup - Rjb) / Rrup;
				}
				double R2 = 62 * M -350;
				double R1 = W * cos(dip * 3.1416 /180);
				double F2 = h4 + h5 * ( (Rx - R1) / (R2 - R1) ) + h6 * ( (Rx - R1) / (R2 - R1) ) * ( (Rx - R1) / (R2 - R1) );
				double F1 = h1 + h2* (Rx / R1) + h3 * (Rx / R1) * (Rx / R1);
				double FhngRx;
				if (Rx < 0 || FHW == 0) {
					FhngRx = 0;
				} else if (0 <= Rx && Rx < R1){
					FhngRx = F1;
				} else {
					if (F2 >= 0){
						FhngRx = F2;
					} else {
						FhngRx = 0;
					}
				}
				double Fhng = c10 * FhngRx * FhngRrup * FhngM * FhngZ * Fhngdelta;


				//California has been considered as default:
				double Z25;
				if (theRegion == RCampbellBozorgnia2013IntensityModel::Japan){
					Z25 = exp(5.359 - 1.102 * log(Vs30) );
				} else {
					Z25 = exp(7.089 - 1.144 * log(Vs30) );
				}

				if (W_ind == 0){
					double FDeltaZM;
					if (M < 6.75) {
						FDeltaZM = -4.317 + 0.984 * M;
					} else {
						FDeltaZM = 2.325;
					}
					double FDeltaZdelta;
					if ( dip <= 40) {
						FDeltaZdelta = 0.0445 * (dip - 40);
					} else {
						FDeltaZdelta = 0;
					}

					double Zbor = Ztori + W * sin(3.1416 /180 * dip);

					double lnDeltaZ;
					lnDeltaZ = std::min( (FDeltaZM + FDeltaZdelta) , log(0.9 * (Zbor - Ztori)));
				
					ZHYP = Ztori + exp(lnDeltaZ);
				}

				double Fsed;
				if (SedFlag != 0){
					if (Z25 <= 1) { 
						Fsed = (c14 +c15 * SJ) * (Z25 -1);
					} else if (1 < Z25 && Z25 <= 3) {
						Fsed = 0;
					} else {
						Fsed = c16 * k3 * exp(-0.75) * (1 - exp(-0.25 * (Z25 - 3) ) );
					}
				} else {
					Fsed = 0;
				}

				double FhypM;
				if (M <= 5.5) {
					FhypM = c17;
				} else if (5.5 < M && M <= 6.5) {
					FhypM = c17 + (c18 - c17) * (M - 5.5);
				} else {
					FhypM = c18;
				}
				double FhypH;
				if (ZHYP <= 7) {
					FhypH = 0;
				} else if (7 < ZHYP && ZHYP <= 20) {
					FhypH =ZHYP -7;
				} else {
					FhypH = 13;
				}
				double Fhyp = FhypH * FhypM;

				
				double Fdip;
				if (M <= 4.5) { 
					Fdip = c19 * dip;
				} else if (4.5 < M && M <= 5.5){
					Fdip = c19 * (5.5 - M) * dip;
				} else {
					Fdip = 0;
				}


				double Fatn;
				if (Rrup > 80){
					Fatn = (c20 +Deltac20) * (Rrup - 80);
				} else {
					Fatn = 0;
				}


				double lnY = FM + FD + FF + Fhng + Fsed + Fhyp + Fdip + Fatn;


				double taulnY;
				double philnY;
				double taulnPGA;
				double philnPGA;
				if (M <= 4.5){
					taulnY = tau1;
					philnY = phi1;
					taulnPGA = tau1Vector[1];
					philnPGA = phi1Vector[1];
				} else if (4.5 < M && M < 5.5){
					taulnY = tau2 + (tau1 - tau2) * (5.5 - M);
					philnY = phi2 + (phi1 - phi2) * (5.5 - M);
					taulnPGA = tau2Vector[1] + (tau1Vector[1] - tau2Vector[1]) * (5.5 - M);
					philnPGA = phi2Vector[1] + (phi1Vector[1] - phi2Vector[1]) * (5.5 - M);
				} else {
					taulnY = tau2;
					philnY = phi2;
					taulnPGA = tau2Vector[1];
					philnPGA = phi2Vector[1];
				}

				


				//////////////////A1100:
				double A1100 = 0.1758;	
				 c0 = c0Vector[1];
				 c1 = c1Vector[1];
				 c2 = c2Vector[1];
				 c3 = c3Vector[1];
				 c4 = c4Vector[1];
				 c5 = c5Vector[1];
				 c6 = c6Vector[1];
				 c7 = c7Vector[1];
				 c8 = c8Vector[1];
				 c9 = c9Vector[1];
				 c10 = c10Vector[1];
				 c11 = c11Vector[1];
				 c12 = c12Vector[1];
				 c13 = c13Vector[1];
				 c14 = c14Vector[1];
				 c15 = c15Vector[1];
				 c16 = c16Vector[1];
				 c17 = c17Vector[1];
				 c18 = c18Vector[1];
				 c19 = c19Vector[1];
				 c20 = c20Vector[1];
				 Deltac20CA = Deltac20CAVector[1];
				 Deltac20JP = Deltac20JPVector[1];
				 Deltac20CH = Deltac20CHVector[1];
				 a2 = a2Vector[1];
				 h1 = h1Vector[1];
				 h2 = h2Vector[1];
				 h3 = h3Vector[1];
				 h4 = h4Vector[1];
				 h5 = h5Vector[1];
				 h6 = h6Vector[1];
				 k1 = k1Vector[1];
				 k2 = k2Vector[1];
				 k3 = k3Vector[1];
				 phi1 = phi1Vector[1];
				 phi2 = phi2Vector[1];
				 tau1 = tau1Vector[1];
				 tau2 = tau2Vector[1];
				 phic = phicVector[1];
				 rholnPGAlnY = rholnPGAlnYVector[1];
						
				if (M <= 4.5){
					FM = c0 + c1 * M;
				}else if (M > 4.5 && M<= 5.5){
					FM = c0 + c1 * M + c2 * ( M - 4.5) ;
				}else if (M > 5.5 && M<= 6.5){
					FM = c0 + c1 * M + c2 * ( M - 4.5) + c3 * (M-5.5) ;
				}else if (M > 6.5){
					FM = c0 + c1 * M + c2 * ( M - 4.5 ) + c3 * (M - 5.5) + c4 * (M - 6.5) ;
				}

				FD = (c5 + c6 * M) * log( sqrt(Rrup*Rrup + c7*c7) );
			
				if (M <= 4.5) {
					FfltM = 0;
				} else if (4.5 < M && M <= 5.5) {
					FfltM = M-4.5;
				} else {
					FfltM = 1;
				}
				 FfltF = c8 * FRV + c9 * FN;
				 FF = FfltF * FfltM;
				
				 Fhngdelta = (90 - dip) / 45;
				
				if (Ztor <= 16.66) {
					FhngZ = 1 - 0.06 * Ztor;
				} else {
					FhngZ = 0;
				}
				if (M <= 5.5) {
					FhngM = 0;
				} else if (5.5 < M && M <= 6.5) {
					FhngM = (M - 5.5) * (1 + a2 * (M - 6.5) );
				} else {
					FhngM = 1 + a2 * (M - 6.5);
				}
				
				if (Rrup == 0) {
					FhngRrup = 1;
				} else if (Rrup > 0) {
					FhngRrup = (Rrup - Rjb) / Rrup;
				}
				 R2 = 62 * M -350;
				 R1 = W * cos(dip *3.1416 /180);
				 F2 = h4 + h5 * ( (Rx - R1) / (R2 - R1) ) + h6 * ( (Rx - R1) / (R2 - R1) ) * ( (Rx - R1) / (R2 - R1) );
				 F1 = h1 + h2* (Rx / R1) + h3 * (Rx / R1)*(Rx / R1);
				 FhngRx;
				if (Rx < 0  || FHW == 0) {
					FhngRx = 0;
				} else if (0 <= Rx && Rx < R1){
					FhngRx = F1;
				} else {
					if (F2 >= 0){
						FhngRx = F2;
					} else {
						FhngRx = 0;
					}
				}
				 Fhng = c10 * FhngRx * FhngRrup * FhngM * FhngZ * Fhngdelta;

				 //California has been considered as default:
				if (theRegion == RCampbellBozorgnia2013IntensityModel::Japan){
					Z25 = exp(5.359 - 1.102 * log(1100.0) );
				} else {
					Z25 = exp(7.089 - 1.144 * log(1100.0) );
				}

				 Fsed;
				if (Z25 <= 1) { 
					Fsed = (c14 +c15 * SJ) * (Z25);
				} else if (1 < Z25 && Z25 <= 3) {
					Fsed = 0;
				} else {
					Fsed = c16 * k3 * exp(-0.75) * (1 - exp(-0.25 * (Z25 - 3) ) );
				}

				 FhypM;
				if (M <= 5.5) {
					FhypM = c17;
				} else if (5.5 < M && M <= 6.5) {
					FhypM = c17 + (c18 - c17) * (M - 5.5);
				} else {
					FhypM = c18;
				}
				 FhypH;
				if (ZHYP <= 7) {
					FhypH = 0;
				} else if (7 < ZHYP && ZHYP <= 20) {
					FhypH =ZHYP -7;
				} else {
					FhypH = 13;
				}
				 Fhyp = FhypH * FhypM;

				 Fdip;
				if (M <= 4.5) { 
					Fdip = c19 * dip;
				} else if (4.5 < M && M <= 5.5){
					Fdip = c19 * (5.5 - M) * dip;
				} else {
					Fdip = 0;
				}


				 Fatn;
				if (Rrup > 80){
					Fatn = (c20 +Deltac20) * (Rrup - 80);
				} else {
					Fatn = 0;
				}
	
				A1100 = exp( FM + FD + FF + Fhng + Fsed + Fhyp + Fdip + Fatn );
			    //////////A1100 End.

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
				 c13 = c13Vector[j];
				 c14 = c14Vector[j];
				 c15 = c15Vector[j];
				 c16 = c16Vector[j];
				 c17 = c17Vector[j];
				 c18 = c18Vector[j];
				 c19 = c19Vector[j];
				 c20 = c20Vector[j];
				 Deltac20CA = Deltac20CAVector[j];
				 Deltac20JP = Deltac20JPVector[j];
				 Deltac20CH = Deltac20CHVector[j];
				 a2 = a2Vector[j];
				 h1 = h1Vector[j];
				 h2 = h2Vector[j];
				 h3 = h3Vector[j];
				 h4 = h4Vector[j];
				 h5 = h5Vector[j];
				 h6 = h6Vector[j];
				 k1 = k1Vector[j];
				 k2 = k2Vector[j];
				 k3 = k3Vector[j];
				 phi1 = phi1Vector[j];
				 phi2 = phi2Vector[j];
				 tau1 = tau1Vector[j];
				 tau2 = tau2Vector[j];
				 phic = phicVector[j];
				 rholnPGAlnY = rholnPGAlnYVector[j];
				if (M <= 4.5) {
					sigma = sigmaM45Vector[j];
				} else if (M >= 5.5) {
					sigma = sigmaM55Vector[j];
				} else {
					sigma = ( (sigmaM55Vector[j] - sigmaM45Vector[j]) / 1) * (M - 4.5) + sigmaM45Vector[j] ;
				}


				double FsiteJ;
				if (Vs30 <= 200){
					FsiteJ = (c12 + k2 * n) * (log(Vs30 / k1) - log(200.0 / k1) );
				} else {
					FsiteJ = (c13 + k2 * n) * log(Vs30 / k1);
				}
				double FsiteG;
				if ( Vs30 <= k1) {
					FsiteG = c11 * log(Vs30 / k1) + k2 * (log(A1100 + c * pow( (Vs30 / k1) , n) ) - log(A1100 + c) );
				} else {
					FsiteG = (c11 + k2 * n) * log(Vs30 / k1);
				}
				double Fsite = FsiteG + SJ * FsiteJ;

				

				double alpha ;
				if (Vs30 < k1){
					alpha =  k2 * A1100 *(pow(( A1100 + c * pow((Vs30 / k1) , n) ) ,-1.0 )- pow((A1100 + c),-1.0));
				}else if( Vs30 >=k1){
					alpha = 0;
				}

				double taulnYB = taulnY;
				double taulnPGAB = taulnPGA;
				double philnYB = sqrt(philnY * philnY - philnAF * philnAF);
				double philnPGAB = sqrt(philnPGA * philnPGA - philnAF * philnAF);
				double tau = sqrt( taulnYB * taulnYB + alpha * alpha * taulnPGAB * taulnPGAB + 2 * alpha * rholnPGAlnY * taulnYB * taulnPGAB );
				double phi = sqrt( philnYB * philnYB + philnAF * philnAF + alpha * alpha * philnPGAB * philnPGAB + 2 * alpha * rholnPGAlnY * philnYB * philnPGAB );


				
				lnY = lnY + Fsite + epsilon * phi + eta * tau;
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

bool RCampbellBozorgnia2013IntensityModel::canBeRunParallelly(){
	return true;
}