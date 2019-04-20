#include "RChiouYoungs2013IntensityModel.h"	


#include <QCoreApplication>
#include <QMetaEnum>
	
#include "RParameter.h"	
#include "RResponse.h"	
#include "RLocation.h"
#include "RLocationResponse.h"
#include "RMainWindow.h"

#include <math.h>

#include <gsl/gsl_math.h>
	
RChiouYoungs2013IntensityModel::RChiouYoungs2013IntensityModel(QObject *parent, QString name)	
	: RModel(parent, name)
{	
	//theEpsilon = 0;
	theResponseType = RChiouYoungs2013IntensityModel::Sa;
}	
	
RChiouYoungs2013IntensityModel::~RChiouYoungs2013IntensityModel()	
{	
	
}	

QString RChiouYoungs2013IntensityModel::getMagnitudeList()	
{	
	return parameterListToString(theMagnitudeList);
}	

void RChiouYoungs2013IntensityModel::setMagnitudeList(QString value)	
{	
	theMagnitudeList = stringToParameterList(value);
}

QString RChiouYoungs2013IntensityModel::getDepthToTopOfRuptureList() 	
{	
	return parameterListToString(theDepthToTopOfRuptureList);
}	
	
void RChiouYoungs2013IntensityModel::setDepthToTopOfRuptureList(QString value)	
{	
	theDepthToTopOfRuptureList = stringToParameterList(value);
}


QString RChiouYoungs2013IntensityModel::getHorizontalDistanceFromTopOfRuptureList() 	
{	
	return parameterListToString(theHorizontalDistanceFromTopOfRuptureList);
}	
	
void RChiouYoungs2013IntensityModel::setHorizontalDistanceFromTopOfRuptureList(QString value)	
{	
	theHorizontalDistanceFromTopOfRuptureList = stringToParameterList(value);
}


QString RChiouYoungs2013IntensityModel::getRuptureDistanceList() 	
{	
	return parameterListToString(theRuptureDistanceList);
}	
	
void RChiouYoungs2013IntensityModel::setRuptureDistanceList(QString value)	
{	
	theRuptureDistanceList = stringToParameterList(value);
}

QString RChiouYoungs2013IntensityModel::getDipAngles() 	
{	
	return doubleVectorToString(theDipAngleVector);
}	
	
void RChiouYoungs2013IntensityModel::setDipAngles(QString value)	
{	
	theDipAngleVector = stringToDoubleVector(value);;
}

QString RChiouYoungs2013IntensityModel::getVsFlags() 	
{	
	return doubleVectorToString(theVsFlagVector);
}	
	
void RChiouYoungs2013IntensityModel::setVsFlags(QString value)	
{	
	theVsFlagVector = stringToDoubleVector(value);;
}

QString RChiouYoungs2013IntensityModel::getHypocentreLocationList()	
{	
	return parameterListToString(theHypocentreLocationList);
}	

void RChiouYoungs2013IntensityModel::setHypocentreLocationList(QString value)	
{	
	theHypocentreLocationList = stringToParameterList(value);

	// Checking if all the passed parameters are either RLocation or RLocationResponse
	parameterListToLocationList(theHypocentreLocationList);
}

QString RChiouYoungs2013IntensityModel::getFaultTypes() const	
{
	QMetaEnum metaEnum = metaObject()->enumerator(metaObject()->indexOfEnumerator("RFaultType"));
	QString result = "";
	for (int i = 0; i < theFaultTypes.count(); i++) {
		QString temp = metaEnum.valueToKey(theFaultTypes[i]);
	    result += temp + "; ";
	}
	return result;
}	
	

void RChiouYoungs2013IntensityModel::setFaultTypes(QString value)	
{	
	QString string = value;
	QMetaEnum metaEnum = metaObject()->enumerator(metaObject()->indexOfEnumerator("RFaultType"));

	QRegExp regExp = QRegExp(QString("[ ,;:\t\n\v\r%1%2]").arg(QChar(QChar::ParagraphSeparator)).arg(QChar(QChar::LineSeparator)));
	QStringList stringList = string.split(regExp, QString::SkipEmptyParts);
	string = "";
	QList<RChiouYoungs2013IntensityModel::RFaultType> tempList;
	for (int i = 0; i < stringList.count(); i++) {
		QByteArray ba = stringList[i].toLocal8Bit();
		const char *tempChar = ba.data();
		int tempValue = metaEnum.keyToValue(tempChar);
		RChiouYoungs2013IntensityModel::RFaultType temp = RFaultType(tempValue);
		if (temp < 0) {
			rCritical() << "Error in the Model" << objectName() << ": The fault type" << stringList[i] << "is not valid";
			return;
		}
		tempList.append(temp);
	}
	theFaultTypes = tempList;
}	


QString RChiouYoungs2013IntensityModel::getWallTypes() const	
{
	QMetaEnum metaEnum = metaObject()->enumerator(metaObject()->indexOfEnumerator("RWallType"));
	QString result = "";
	for (int i = 0; i < theWallTypes.count(); i++) {
		QString temp = metaEnum.valueToKey(theWallTypes[i]);
	    result += temp + "; ";
	}
	return result;
}	
	

void RChiouYoungs2013IntensityModel::setWallTypes(QString value)	
{	
	QString string = value;
	QMetaEnum metaEnum = metaObject()->enumerator(metaObject()->indexOfEnumerator("RWallType"));

	QRegExp regExp = QRegExp(QString("[ ,;:\t\n\v\r%1%2]").arg(QChar(QChar::ParagraphSeparator)).arg(QChar(QChar::LineSeparator)));
	QStringList stringList = string.split(regExp, QString::SkipEmptyParts);
	string = "";
	QList<RChiouYoungs2013IntensityModel::RWallType> tempList;
	for (int i = 0; i < stringList.count(); i++) {
		QByteArray ba = stringList[i].toLocal8Bit();
		const char *tempChar = ba.data();
		int tempValue = metaEnum.keyToValue(tempChar);
		RChiouYoungs2013IntensityModel::RWallType temp = RWallType(tempValue);
		if (temp < 0) {
			rCritical() << "Error in the Model" << objectName() << ": The Wall type" << stringList[i] << "is not valid";
			return;
		}
		tempList.append(temp);
	}
	theWallTypes = tempList;
}	


QString RChiouYoungs2013IntensityModel::getRegions() const	
{
	QMetaEnum metaEnum = metaObject()->enumerator(metaObject()->indexOfEnumerator("RRegion"));
	QString result = "";
	for (int i = 0; i < theRegions.count(); i++) {
		QString temp = metaEnum.valueToKey(theRegions[i]);
	    result += temp + "; ";
	}
	return result;
}	
	

void RChiouYoungs2013IntensityModel::setRegions(QString value)	
{	
	QString string = value;
	QMetaEnum metaEnum = metaObject()->enumerator(metaObject()->indexOfEnumerator("RRegion"));

	QRegExp regExp = QRegExp(QString("[ ,;:\t\n\v\r%1%2]").arg(QChar(QChar::ParagraphSeparator)).arg(QChar(QChar::LineSeparator)));
	QStringList stringList = string.split(regExp, QString::SkipEmptyParts);
	string = "";
	QList<RChiouYoungs2013IntensityModel::RRegion> tempList;
	for (int i = 0; i < stringList.count(); i++) {
		QByteArray ba = stringList[i].toLocal8Bit();
		const char *tempChar = ba.data();
		int tempValue = metaEnum.keyToValue(tempChar);
		RChiouYoungs2013IntensityModel::RRegion temp = RRegion(tempValue);
		if (temp < 0) {
			rCritical() << "Error in the Model" << objectName() << ": The region" << stringList[i] << "is not valid";
			return;
		}
		tempList.append(temp);
	}
	theRegions = tempList;
}	


RChiouYoungs2013IntensityModel::RResponseType RChiouYoungs2013IntensityModel::getResponseType() const	
{	
	return theResponseType;
}	
	
void RChiouYoungs2013IntensityModel::setResponseType(RChiouYoungs2013IntensityModel::RResponseType value)	
{	
	theResponseType = value;
}

QString RChiouYoungs2013IntensityModel::getPeriodList()	
{	
	return parameterListToString(thePeriodList);
}	
	
void RChiouYoungs2013IntensityModel::setPeriodList(QString value)	
{	
	thePeriodList = stringToParameterList(value);

	createResponses();
}	


QString RChiouYoungs2013IntensityModel::getEpsilonUncertaintyList()	
{	
	return parameterListToString(theEpsilonUncertaintyList);
}	
	
void RChiouYoungs2013IntensityModel::setEpsilonUncertaintyList(QString value)	
{	
	theEpsilonUncertaintyList = stringToParameterList(value);

	createResponses();
}	


QString RChiouYoungs2013IntensityModel::getEtaUncertaintyList()	
{	
	return parameterListToString(theEtaUncertaintyList);
}	
	
void RChiouYoungs2013IntensityModel::setEtaUncertaintyList(QString value)	
{	
	theEtaUncertaintyList = stringToParameterList(value);

	createResponses();
}	


QList<RParameter *> RChiouYoungs2013IntensityModel::getPhysicalParameterList()
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

QString RChiouYoungs2013IntensityModel::getStructureLocationList()	
{	
	return parameterListToString(theStructureLocationList);
}	
	
void RChiouYoungs2013IntensityModel::setStructureLocationList(QString value)	
{	
	theStructureLocationList = stringToParameterList(value);

	// Checking if all the passed parameters are either RLocation or RLocationResponse
	parameterListToLocationList(theStructureLocationList);

	createResponses();
}	
	
QString RChiouYoungs2013IntensityModel::getShearWaveVelocityList()	
{	
	return parameterListToString(theShearWaveVelocityList);
}	
	
void RChiouYoungs2013IntensityModel::setShearWaveVelocityList(QString value)	
{	
	theShearWaveVelocityList = stringToParameterList(value);

	createResponses();
}	

QString RChiouYoungs2013IntensityModel::getDepthToVs1List()	
{	
	return parameterListToString(theDepthToVs1List);
}	
	
void RChiouYoungs2013IntensityModel::setDepthToVs1List(QString value)	
{	
	theDepthToVs1List = stringToParameterList(value);

	createResponses();
}	
void RChiouYoungs2013IntensityModel::createResponses()
{
	// Removing old responses
	while (theSpectralResponseList.count() > 0) {
		delete theSpectralResponseList.takeAt(0);
	}

	if (theStructureLocationList.isEmpty() || theShearWaveVelocityList.isEmpty()) {
		return;
	}

	if ((theStructureLocationList.count() != thePeriodList.count() && theResponseType == RChiouYoungs2013IntensityModel::Sa) || (theStructureLocationList.count() != theShearWaveVelocityList.count() && theShearWaveVelocityList.count() > 1) || (theStructureLocationList.count() != theEpsilonUncertaintyList.count() && theEpsilonUncertaintyList.count() > 1) || (theStructureLocationList.count() != theEtaUncertaintyList.count() && theEtaUncertaintyList.count() > 1)) {
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

int RChiouYoungs2013IntensityModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
{	
	if (theStructureLocationList.isEmpty()) {
		rCritical(threadID) << "Error: There should be at least one structure location in the model" << objectName() << ".";
		return -1;
	}
	if ((theStructureLocationList.count() != thePeriodList.count() && theResponseType == RChiouYoungs2013IntensityModel::Sa) || (theStructureLocationList.count() != theShearWaveVelocityList.count() && theShearWaveVelocityList.count() > 1)) {
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
	QVector<double> c8Vector;
	QVector<double> c1Vector;
	QVector<double> c1aVector;
	QVector<double> c1bVector;
	QVector<double> c1cVector;
	QVector<double> c1dVector;
	QVector<double> cnVector;
	QVector<double> cMVector;
	QVector<double> c3Vector;
	QVector<double> c5Vector;
	QVector<double> cHMVector;
	QVector<double> c6Vector;
	QVector<double> c7Vector;
	QVector<double> c7bVector;
	QVector<double> c8bVector;
	QVector<double> c9Vector;
	QVector<double> c9aVector;
	QVector<double> c9bVector;
	QVector<double> c11bVector;
	QVector<double> cg1Vector;
	QVector<double> cg2Vector;
	QVector<double> cg3Vector;
	QVector<double> phi1Vector;
	QVector<double> phi2Vector;
	QVector<double> phi3Vector;
	QVector<double> phi4Vector;
	QVector<double> phi5Vector;
	QVector<double> phi6Vector;
	QVector<double> tau1Vector;
	QVector<double> tau2Vector;
	QVector<double> gammaJPITVector;
	QVector<double> gammaWnVector;
	QVector<double> sigma1Vector;
	QVector<double> sigma2Vector;
	QVector<double> sigma3Vector;
	QVector<double> sigma2JPVector;
	QVector<double> phi1JPVector;
	QVector<double> phi5JPVector;
	

	periodTableVector  << -1 << 0 << 0.01 << 0.02 << 0.03 << 0.04 << 0.05 << 0.075 << 0.1 << 0.12 << 0.15 << 0.17 << 0.2 << 0.25 << 0.3 << 0.4 << 0.5 << 0.75 << 1 << 1.5 << 2 << 3 << 4 << 5 << 7.5 << 10 ;
	c8Vector  << 0.2154 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0.0991 << 0.1982 << 0.2154 << 0.2154 << 0.2154 << 0.2154 << 0.2154 << 0.2154 << 0.2154 << 0.2154;
	c1Vector  << 2.3549 << -1.5065 << -1.5065 << -1.4798 << -1.2972 << -1.1007 << -0.9292 << -0.658 << -0.5613 << -0.5342 << -0.5462 << -0.5858 << -0.6798 << -0.8663 << -1.0514 << -1.3794 << -1.6508 << -2.1511 << -2.5365 << -3.0686 << -3.4148 << -3.9013 << -4.2466 << -4.5143 << -5.0009 << -5.3461;
	c1aVector  << 0.165 << 0.165 << 0.165 << 0.165 << 0.165 << 0.165 << 0.165 << 0.165 << 0.165 << 0.165 << 0.165 << 0.165 << 0.165 << 0.165 << 0.165 << 0.165 << 0.165 << 0.165 << 0.165 << 0.165 << 0.1645 << 0.1168 << 0.0732 << 0.0484 << 0.022 << 0.0124;
	c1bVector  << -0.0626 << -0.255 << -0.255 << -0.255 << -0.255 << -0.255 << -0.255 << -0.254 << -0.253 << -0.252 << -0.25 << -0.248 << -0.2449 << -0.2382 << -0.2313 << -0.2146 << -0.1972 << -0.162 << -0.14 << -0.1184 << -0.11 << -0.104 << -0.102 << -0.101 << -0.101 << -0.1;
	c1cVector  << -0.165 << -0.165 << -0.165 << -0.165 << -0.165 << -0.165 << -0.165 << -0.165 << -0.165 << -0.165 << -0.165 << -0.165 << -0.165 << -0.165 << -0.165 << -0.165 << -0.165 << -0.165 << -0.165 << -0.165 << -0.1645 << -0.1168 << -0.0732 << -0.0484 << -0.022 << -0.0124;
	c1dVector  << 0.0626 << 0.255 << 0.255 << 0.255 << 0.255 << 0.255 << 0.255 << 0.254 << 0.253 << 0.252 << 0.25 << 0.248 << 0.2449 << 0.2382 << 0.2313 << 0.2146 << 0.1972 << 0.162 << 0.14 << 0.1184 << 0.11 << 0.104 << 0.102 << 0.101 << 0.101 << 0.1;
	cnVector  << 3.3024 << 16.0875 << 16.0875 << 15.7118 << 15.8819 << 16.4556 << 17.6453 << 20.1772 << 19.9992 << 18.7106 << 16.6246 << 15.3709 << 13.7012 << 11.2667 << 9.1908 << 6.5459 << 5.2305 << 3.7896 << 3.3024 << 2.8498 << 2.5417 << 2.1488 << 1.8957 << 1.7228 << 1.5737 << 1.5265;
	cMVector  << 5.423 << 4.9993 << 4.9993 << 4.9993 << 4.9993 << 4.9993 << 4.9993 << 5.0031 << 5.0172 << 5.0315 << 5.0547 << 5.0704 << 5.0939 << 5.1315 << 5.167 << 5.2317 << 5.2893 << 5.4109 << 5.5106 << 5.6705 << 5.7981 << 5.9983 << 6.1552 << 6.2856 << 6.5428 << 6.7415;
	c3Vector  << 2.3152 << 1.9636 << 1.9636 << 1.9636 << 1.9636 << 1.9636 << 1.9636 << 1.9636 << 1.9636 << 1.9795 << 2.0362 << 2.0823 << 2.1521 << 2.2574 << 2.344 << 2.4709 << 2.5567 << 2.6812 << 2.7474 << 2.8161 << 2.8514 << 2.8875 << 2.9058 << 2.9169 << 2.932 << 2.9396;
	c5Vector  << 5.8096 << 6.4551 << 6.4551 << 6.4551 << 6.4551 << 6.4551 << 6.4551 << 6.4551 << 6.8305 << 7.1333 << 7.3621 << 7.4365 << 7.4972 << 7.5416 << 7.56 << 7.5735 << 7.5778 << 7.5808 << 7.5814 << 7.5817 << 7.5818 << 7.5818 << 7.5818 << 7.5818 << 7.5818 << 7.5818 ;
	cHMVector  << 3.0514 << 3.0956 << 3.0956 << 3.0963 << 3.0974 << 3.0988 << 3.1011 << 3.1094 << 3.2381 << 3.3407 << 3.43 << 3.4688 << 3.5146 << 3.5746 << 3.6232 << 3.6945 << 3.7401 << 3.7941 << 3.8144 << 3.8284 << 3.833 << 3.8361 << 3.8369 << 3.8376 << 3.838 << 3.838 ;
	c6Vector  << 0.4407 << 0.4908 << 0.4908 << 0.4925 << 0.4992 << 0.5037 << 0.5048 << 0.5048 << 0.5048 << 0.5048 << 0.5045 << 0.5036 << 0.5016 << 0.4971 << 0.4919 << 0.4807 << 0.4707 << 0.4575 << 0.4522 << 0.4501 << 0.45 << 0.45 << 0.45 << 0.45 << 0.45 << 0.45 ;
	c7Vector  << 0.0324 << 0.0352 << 0.0352 << 0.0352 << 0.0352 << 0.0352 << 0.0352 << 0.0352 << 0.0352 << 0.0352 << 0.0352 << 0.0352 << 0.0352 << 0.0352 << 0.0352 << 0.0352 << 0.0352 << 0.0352 << 0.0352 << 0.0352 << 0.0352 << 0.016 << 0.0062 << 0.0029 << 0.0007 << 0.0003 ;
	c7bVector  << 0.0097 << 0.0462 << 0.0462 << 0.0472 << 0.0533 << 0.0596 << 0.0639 << 0.063 << 0.0532 << 0.0452 << 0.0345 << 0.0283 << 0.0202 << 0.009 << -0.0004 << -0.0155 << -0.0278 << -0.0477 << -0.0559 << -0.063 << -0.0665 << -0.0516 << -0.0448 << -0.0424 << -0.0348 << -0.0253 ;
	c8bVector  << 5 << 0.4833 << 0.4833 << 1.2144 << 1.6421 << 1.9456 << 2.181 << 2.6087 << 2.9122 << 3.1045 << 3.3399 << 3.4719 << 3.6434 << 3.8787 << 4.0711 << 4.3745 << 4.6099 << 5.0376 << 5.3411 << 5.7688 << 6.0723 << 6.5 << 6.8035 << 7.0389 << 7.4666 << 7.77 ;
	c9Vector  << 0.3079 << 0.9228 << 0.9228 << 0.9296 << 0.9396 << 0.9661 << 0.9794 << 1.026 << 1.0177 << 1.0008 << 0.9801 << 0.9652 << 0.9459 << 0.9196 << 0.8829 << 0.8302 << 0.7884 << 0.6754 << 0.6196 << 0.5101 << 0.3917 << 0.1244 << 0.0086 << 0 << 0 << 0 ;
	c9aVector  << 0.1 << 0.1202 << 0.1202 << 0.1217 << 0.1194 << 0.1166 << 0.1176 << 0.1171 << 0.1146 << 0.1128 << 0.1106 << 0.115 << 0.1208 << 0.1208 << 0.1175 << 0.106 << 0.1061 << 0.1 << 0.1 << 0.1 << 0.1 << 0.1 << 0.1 << 0.1 << 0.1 << 0.1 ;
	c9bVector  << 6.5 << 6.8607 << 6.8607 << 6.8697 << 6.9113 << 7.0271 << 7.0959 << 7.3298 << 7.2588 << 7.2372 << 7.2109 << 7.2491 << 7.2988 << 7.3691 << 6.8789 << 6.5334 << 6.526 << 6.5 << 6.5 << 6.5 << 6.5 << 6.5 << 6.5 << 6.5 << 6.5 << 6.5 ;
	c11bVector  << -0.3834 << -0.4536 << -0.4536 << -0.4536 << -0.4536 << -0.4536 << -0.4536 << -0.4536 << -0.4536 << -0.4536 << -0.4536 << -0.4536 << -0.444 << -0.3539 << -0.2688 << -0.1793 << -0.1428 << -0.1138 << -0.1062 << -0.102 << -0.1009 << -0.1003 << -0.1001 << -0.1001 << -0.1 << -0.1 ;
	cg1Vector  << -0.001852 << -0.007146 << -0.007146 << -0.007249 << -0.007869 << -0.008316 << -0.008743 << -0.009537 << -0.00983 << -0.009913 << -0.009896 << -0.009787 << -0.009505 << -0.008918 << -0.008251 << -0.007267 << -0.006492 << -0.005147 << -0.004277 << -0.002979 << -0.002301 << -0.001344 << -0.001084 << -0.00101 << -0.000964 << -0.00095 ;
	cg2Vector  << -0.007403 << -0.006758 << -0.006758 << -0.006758 << -0.006758 << -0.006758 << -0.006758 << -0.00619 << -0.005332 << -0.004732 << -0.003806 << -0.00328 << -0.00269 << -0.002128 << -0.001812 << -0.001274 << -0.001074 << -0.001115 << -0.001197 << -0.001675 << -0.002349 << -0.003306 << -0.003566 << -0.00364 << -0.003686 << -0.0037 ;
	cg3Vector  << 4.3439 << 4.2542 << 4.2542 << 4.2386 << 4.2519 << 4.296 << 4.3578 << 4.5455 << 4.7603 << 4.8963 << 5.0644 << 5.1371 << 5.188 << 5.2164 << 5.1954 << 5.0899 << 4.7854 << 4.3304 << 4.1667 << 4.0029 << 3.8949 << 3.7928 << 3.7443 << 3.709 << 3.6632 << 3.623 ;
	phi1Vector  << -0.7936 << -0.521 << -0.521 << -0.5055 << -0.4368 << -0.3752 << -0.3469 << -0.3747 << -0.444 << -0.4895 << -0.5477 << -0.5922 << -0.6693 << -0.7766 << -0.8501 << -0.9431 << -1.0044 << -1.0602 << -1.0941 << -1.1142 << -1.1154 << -1.1081 << -1.0603 << -0.9872 << -0.8274 << -0.7053 ;
	phi2Vector  << -0.0699 << -0.1417 << -0.1417 << -0.1364 << -0.1403 << -0.1591 << -0.1862 << -0.2538 << -0.2943 << -0.3077 << -0.3113 << -0.3062 << -0.2927 << -0.2662 << -0.2405 << -0.1975 << -0.1633 << -0.1028 << -0.0699 << -0.0425 << -0.0302 << -0.0129 << -0.0016 << 0 << 0 << 0 ;
	phi3Vector  << -0.008444 << -0.00701 << -0.00701 << -0.007279 << -0.007354 << -0.006977 << -0.006467 << -0.005734 << -0.005604 << -0.005696 << -0.005845 << -0.005959 << -0.006141 << -0.006439 << -0.006704 << -0.007125 << -0.007435 << -0.00812 << -0.008444 << -0.007707 << -0.004792 << -0.001828 << -0.001523 << -0.00144 << -0.001369 << -0.001361 ;
	phi4Vector  << 5.41 << 0.102151 << 0.102151 << 0.10836 << 0.119888 << 0.133641 << 0.148927 << 0.190596 << 0.230662 << 0.253169 << 0.266468 << 0.26506 << 0.255253 << 0.231541 << 0.207277 << 0.165464 << 0.133828 << 0.085153 << 0.058595 << 0.031787 << 0.019716 << 0.009643 << 0.005379 << 0.003223 << 0.001134 << 0.000515 ;
	phi5Vector  << 0.0202 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0.001 << 0.004 << 0.01 << 0.034 << 0.067 << 0.143 << 0.203 << 0.277 << 0.309 << 0.321 << 0.329 << 0.33 ;
	phi6Vector  << 300 << 300 << 300 << 300 << 300 << 300 << 300 << 300 << 300 << 300 << 300 << 300 << 300 << 300 << 300 << 300 << 300 << 300 << 300 << 300 << 300 << 300 << 300 << 300 << 300 << 300 ;
	tau1Vector  << 0.3894 << 0.4 << 0.4 << 0.4026 << 0.4063 << 0.4095 << 0.4124 << 0.4179 << 0.4219 << 0.4244 << 0.4275 << 0.4292 << 0.4313 << 0.4341 << 0.4363 << 0.4396 << 0.4419 << 0.4459 << 0.4484 << 0.4515 << 0.4534 << 0.4558 << 0.4574 << 0.4584 << 0.4601 << 0.4612 ;
	tau2Vector  << 0.2578 << 0.26 << 0.26 << 0.2637 << 0.2689 << 0.2736 << 0.2777 << 0.2855 << 0.2913 << 0.2949 << 0.2993 << 0.3017 << 0.3047 << 0.3087 << 0.3119 << 0.3165 << 0.3199 << 0.3255 << 0.3291 << 0.3335 << 0.3363 << 0.3398 << 0.3419 << 0.3435 << 0.3459 << 0.3474 ;
	gammaJPITVector  << 2.2306 << 1.5817 << 1.5817 << 1.574 << 1.5544 << 1.5502 << 1.5391 << 1.4804 << 1.4094 << 1.3682 << 1.3241 << 1.3071 << 1.2931 << 1.315 << 1.3514 << 1.4051 << 1.4402 << 1.528 << 1.6523 << 1.8872 << 2.1348 << 3.5752 << 3.8646 << 3.7292 << 2.3763 << 1.7679 ;
	gammaWnVector  << 0.335 << 0.7594 << 0.7594 << 0.7606 << 0.7642 << 0.7676 << 0.7739 << 0.7956 << 0.7932 << 0.7768 << 0.7437 << 0.7219 << 0.6922 << 0.6579 << 0.6362 << 0.6049 << 0.5507 << 0.3582 << 0.2003 << 0.0356 << 0 << 0 << 0 << 0 << 0 << 0 ;
	sigma1Vector  << 0.4785 << 0.4912 << 0.4912 << 0.4904 << 0.4988 << 0.5049 << 0.5096 << 0.5179 << 0.5236 << 0.527 << 0.5308 << 0.5328 << 0.5351 << 0.5377 << 0.5395 << 0.5422 << 0.5433 << 0.5294 << 0.5105 << 0.4783 << 0.4681 << 0.4617 << 0.4571 << 0.4535 << 0.4471 << 0.4426 ;
	sigma2Vector  << 0.36292 << 0.3762 << 0.3762 << 0.3762 << 0.3849 << 0.391 << 0.3957 << 0.4043 << 0.4104 << 0.4143 << 0.4191 << 0.4217 << 0.4252 << 0.4299 << 0.4338 << 0.4399 << 0.4446 << 0.4533 << 0.4594 << 0.468 << 0.4681 << 0.4617 << 0.4571 << 0.4535 << 0.4471 << 0.4426 ;
	sigma3Vector  << 0.7504 << 0.8 << 0.8 << 0.8 << 0.8 << 0.8 << 0.8 << 0.8 << 0.8 << 0.8 << 0.8 << 0.8 << 0.8 << 0.7999 << 0.7997 << 0.7988 << 0.7966 << 0.7792 << 0.7504 << 0.7136 << 0.7035 << 0.7006 << 0.7001 << 0.7 << 0.7 << 0.7 ;
	sigma2JPVector  << 0.3918 << 0.4528 << 0.4528 << 0.4551 << 0.4571 << 0.4642 << 0.4716 << 0.5022 << 0.523 << 0.5278 << 0.5304 << 0.531 << 0.5312 << 0.5309 << 0.5307 << 0.531 << 0.5313 << 0.5309 << 0.5302 << 0.5276 << 0.5167 << 0.4917 << 0.4682 << 0.4517 << 0.4167 << 0.3755 ;
	phi1JPVector  << -0.7966 << -0.6846 << -0.6846 << -0.6681 << -0.6314 << -0.5855 << -0.5457 << -0.4685 << -0.4985 << -0.5603 << -0.6451 << -0.6981 << -0.7653 << -0.8469 << -0.8999 << -0.9618 << -0.9945 << -1.0225 << -1.0002 << -0.9245 << -0.8626 << -0.7882 << -0.7195 << -0.656 << -0.5202 << -0.4068 ;
	phi5JPVector  << 0.9488 << 0.459 << 0.459 << 0.458 << 0.462 << 0.453 << 0.436 << 0.383 << 0.375 << 0.377 << 0.379 << 0.38 << 0.384 << 0.393 << 0.408 << 0.462 << 0.524 << 0.658 << 0.78 << 0.96 << 1.11 << 1.291 << 1.387 << 1.433 << 1.46 << 1.464 ;

	double c2 = 1.06;
	double c4 = -2.1;
	double c4a = -0.5;
	double cRB = 50;
	double c8a = 0.2695;
	double c11 = 0;
	double phi6JP = 800;

	
	double T;
	if (theResponseType == RChiouYoungs2013IntensityModel::PGA) {
		T = 0.0;
	} else if (theResponseType == RChiouYoungs2013IntensityModel::PGV) {
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
		
		RChiouYoungs2013IntensityModel::RFaultType theFaultType;
			if (theFaultTypes.count() == 1) {
				theFaultType = theFaultTypes[0];
			} else {
				theFaultType = theFaultTypes[index];
			}

		if (theFaultType == RChiouYoungs2013IntensityModel::StrikeSlip) {
			FN = 0;
			FRV = 0;
			
		} else if (theFaultType == RChiouYoungs2013IntensityModel::NormalSlip) {
			FN = 1.0;
			
		} else if (theFaultType == RChiouYoungs2013IntensityModel::ReverseSlip) {
			FRV = 1.0;
			
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
			double EZtor;
			double DeltaZtor;
			if (FRV == 1) {
				EZtor  = pow( std::max(2.704 - 1.226 * std::max(M - 5.849 , 0.0) , 0.0) , 2.0);
			} else { 
				EZtor  = pow( std::max(2.673 - 1.136 * std::max(M - 4.970 , 0.0) , 0.0) , 2.0);
			}
			if (Ztor == 999){
				Ztor = EZtor;
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
			DeltaZtor = Ztor - EZtor;		
			


			double Rrup;
			double irrup;
			if (theRuptureDistanceList.count() == 1){
				irrup = 0;
			}else{
				irrup = i;
			}
			if (theRuptureDistanceList.isEmpty() == 1 || theRuptureDistanceList[irrup].data()->getCurrentValue(threadID) == 999){
				Rrup = sqrt(Rjb * Rjb + Ztor * Ztor);
			} else {
				Rrup = theRuptureDistanceList[irrup].data()->getCurrentValue(threadID);
			}
				

			double Rx;
			double irx;
			if (theHorizontalDistanceFromTopOfRuptureList.count() == 1){
				irx = 0;
			}else{
				irx = i;
			}
			if (theHorizontalDistanceFromTopOfRuptureList.isEmpty() == 1 || theHorizontalDistanceFromTopOfRuptureList[irx].data()->getCurrentValue(threadID) == 999){
			Rx = Rjb;
			} else {
				Rx = theHorizontalDistanceFromTopOfRuptureList[irx].data()->getCurrentValue(threadID);
			}





			if (theResponseType == RChiouYoungs2013IntensityModel::Sa) {
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

			double Vs30Flag;
			if (theVsFlagVector.count() == 1){
				Vs30Flag = theVsFlagVector[0];
			}else {
				Vs30Flag = theVsFlagVector[i];
			}

			double Z1;
			if (theDepthToVs1List.count() == 1) {
				Z1 = theDepthToVs1List[0].data()->getCurrentValue(threadID);
			} else {
				Z1 = theDepthToVs1List[i].data()->getCurrentValue(threadID);
			}
			

			RChiouYoungs2013IntensityModel::RRegion theRegion;
			if (theRegions.count() == 1) {
				theRegion = theRegions[0];
			} else {
				theRegion = theRegions[i];
			}

			
			RChiouYoungs2013IntensityModel::RWallType theWallType;
			if (theWallTypes.count() == 1) {
				theWallType = theWallTypes[0];
			} else {
				theWallType = theWallTypes[index];
			}

			double FHW;
			if (theWallTypes.isEmpty() == 1){
				FHW =0;
			}
			if (theWallType == RChiouYoungs2013IntensityModel::HangingWall){
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

				double c8 = c8Vector[j];
				double c1 = c1Vector[j];
				double c1a = c1aVector[j];
				double c1b = c1bVector[j];
				double c1c = c1cVector[j];
				double c1d = c1dVector[j];
				double cn = cnVector[j];
				double cM = cMVector[j];
				double c3 = c3Vector[j];
				double c5 = c5Vector[j];
				double cHM = cHMVector[j];
				double c6 = c6Vector[j];
				double c7 = c7Vector[j];
				double c7b = c7bVector[j];
				double c8b = c8bVector[j];
				double c9 = c9Vector[j];
				double c9a = c9aVector[j];
				double c9b = c9bVector[j];
				double c11b = c11bVector[j];
				double cg1 = cg1Vector[j];
				double cg2 = cg2Vector[j];
				double cg3 = cg3Vector[j];
				double phi1 = phi1Vector[j];
				double phi2 = phi2Vector[j];
				double phi3 = phi3Vector[j];
				double phi4 = phi4Vector[j];
				double phi5 = phi5Vector[j];
				double phi6 = phi6Vector[j];
				double tau1 = tau1Vector[j];
				double tau2 = tau2Vector[j];
				double gammaJPIT = gammaJPITVector[j];
				double gammaWn = gammaWnVector[j];
				double sigma1 = sigma1Vector[j];
				double sigma2 = sigma2Vector[j];
				double sigma3 = sigma3Vector[j];
				double sigma2JP = sigma2JPVector[j];
				double phi1JP = phi1JPVector[j];
				double phi5JP = phi5JPVector[j];
				
				
				double DeltaDPP;
				DeltaDPP = 0; ///////// : It refers to directivity effects. If you want to consider directivity, You have to get it as an input, 0 or 1;

				if (theRegion == RChiouYoungs2013IntensityModel::Japan){
					sigma2 = sigma2JP;
					phi1 = phi1JP;
					phi5 = phi5JP;
					phi6 = phi6JP;
				}

				/*
				//California has been considered as default:
				double Z25;
				if (theRegion == RChiouYoungs2013IntensityModel::Japan){
					Z25 = exp(5.359 - 1.102 * log(Vs30) );
				} else {
					Z25 = exp(7.089 - 1.144 * log(Vs30) );
				}
				*/
				
				double Finferred = 0;
				double Fmeasured = 0;
				if (Vs30Flag == 0){
					Finferred = 1;
				}else {
					Fmeasured = 1;
				}


				double Z1ref;
				double DeltaZ1;
				if (theRegion == RChiouYoungs2013IntensityModel::Japan) {
					Z1ref = 1/1000.0 * exp(-5.23/2 *log ( (pow(Vs30 , 2) + pow(412.0 , 2)) / (pow(1360.0 , 2) + pow(412.0 , 2)) ) );
				} else {
					Z1ref = 1/1000.0 * exp(-7.67/4 *log ( (pow(Vs30 , 4) + pow(610.0 , 4)) / (pow(1360.0 , 4) + pow(610.0 , 4)) ) );
				}
				if (Z1 == 999){
					Z1 = Z1ref;
					DeltaZ1 = 0;
				} else {
					DeltaZ1 = (Z1 - Z1ref) * 1000.0; //m
				}


				double tempTerm1 = (cg1 + cg2 / cosh(std::max(M - cg3 , 0.0))) * Rrup;
				if ( (theRegion == RChiouYoungs2013IntensityModel::Japan || theRegion == RChiouYoungs2013IntensityModel::Italy) && (6 < M && M < 6.9) ){
					tempTerm1 = tempTerm1 * gammaJPIT;
				} else if (theRegion == RChiouYoungs2013IntensityModel::China) {
					tempTerm1 = tempTerm1 * gammaWn;
				}

				double lnYref = c1 + (c1a + c1c / cosh(2.0 * std::max(M - 4.5 , 0.0)) ) * FRV;
				lnYref = lnYref + (c1b + c1d / cosh(2.0 * std::max(M - 4.5 , 0.0))) * FN;
				lnYref = lnYref + (c7 + c7b / cosh(2.0 * std::max(M - 4.5 , 0.0))) * DeltaZtor;
				lnYref = lnYref + (c11 + c11b / cosh(2.0 * std::max(M - 4.5 , 0.0))) * pow(cos(dip / 180 * 3.1416) , 2.0);
				lnYref = lnYref + c2 * (M - 6) + (c2 - c3) / cn * log(1.0 + exp(cn * (cM - M)));
				lnYref = lnYref + c4 * log(Rrup + c5 * cosh(c6 * std::max(M - cHM , 0.0)));
				lnYref = lnYref + (c4a - c4) * log(sqrt(Rrup*Rrup + cRB*cRB));
				lnYref = lnYref + tempTerm1;
				lnYref = lnYref + c8 * std::max((1.0 - std::max(Rrup - 40 , 0.0) / 30.0) , 0.0) * std::min((std::max(M - 5.5 , 0.0) / 8.0) , 1.0) * exp(-c8a * pow(M - c8b , 2.0)) * DeltaDPP;
				lnYref = lnYref + c9 * FHW * cos(dip / 180 * 3.1416) * (c9a + (1 - c9a) * tanh(Rx / c9b)) * (1 - sqrt(Rjb*Rjb + Ztor*Ztor) / (Rrup + 1));
				double yref = exp(lnYref);

				double NL0 = phi2 * (exp(phi3 * (std::min(Vs30 , 1130.0) - 360)) - exp(phi3 *(1130.0 * 360.0))) * (yref / (yref + phi4));
				double sigmaNL0 = (sigma1 + (sigma2 - sigma1) / 1.5 * (std::min(std::max(M , 5.0) , 6.5) - 5.0) ) * sqrt(sigma3 * Finferred + 0.7 * Fmeasured + pow(1+NL0 , 2.0));
				double tau = tau1 + (tau2 - tau1) / 1.5 * (std::min(std::max(M , 5.0) , 6.5) - 5.0);
				double sigmaT = pow((1 + NL0) * tau , 2.0) + pow(sigmaNL0 , 2.0);


				double lnY;
				lnY = lnYref + phi1 * std::min(log(Vs30 / 1130), 0.0);
				lnY = lnY + phi2 * (exp(phi3 * (std::min(Vs30 , 1130.0) - 360)) - exp(phi3 * (1130 - 360) ) ) * log((yref * exp(eta * tau) + phi4) / phi4);
				lnY = lnY + phi5 * (1 - exp(-DeltaZ1 / phi6));


				
				lnY = lnY + epsilon * sigmaT + eta * tau;
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

bool RChiouYoungs2013IntensityModel::canBeRunParallelly(){
	return true;
}
