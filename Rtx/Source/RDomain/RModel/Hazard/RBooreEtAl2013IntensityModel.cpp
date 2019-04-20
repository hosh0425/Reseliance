#include "RBooreEtAl2013IntensityModel.h"	

#include <QCoreApplication>
#include <QMetaEnum>
	
#include "RParameter.h"	
#include "RResponse.h"	
#include "RLocation.h"
#include "RLocationResponse.h"
#include "RMainWindow.h"

#include <math.h>

#include <gsl/gsl_math.h>
	
RBooreEtAl2013IntensityModel::RBooreEtAl2013IntensityModel(QObject *parent, QString name)	
	: RModel(parent, name)
{	
	//theEpsilon = 0;
	theResponseType = RBooreEtAl2013IntensityModel::Sa;

}	
	
RBooreEtAl2013IntensityModel::~RBooreEtAl2013IntensityModel()	
{	
	
}	

QString RBooreEtAl2013IntensityModel::getMagnitudeList()	
{	
	return parameterListToString(theMagnitudeList);
}	

void RBooreEtAl2013IntensityModel::setMagnitudeList(QString value)	
{	
	theMagnitudeList = stringToParameterList(value);
}

QString RBooreEtAl2013IntensityModel::getHypocentreLocationList()	
{	
	return parameterListToString(theHypocentreLocationList);
}	

void RBooreEtAl2013IntensityModel::setHypocentreLocationList(QString value)	
{	
	theHypocentreLocationList = stringToParameterList(value);

	// Checking if all the passed parameters are either RLocation or RLocationResponse
	parameterListToLocationList(theHypocentreLocationList);
}

QString RBooreEtAl2013IntensityModel::getEpsilonUncertaintyList()	
{	
	return parameterListToString(theEpsilonUncertaintyList);
}	
	
void RBooreEtAl2013IntensityModel::setEpsilonUncertaintyList(QString value)	
{	
	theEpsilonUncertaintyList = stringToParameterList(value);

	createResponses();
}	


QString RBooreEtAl2013IntensityModel::getEtaUncertaintyList()	
{	
	return parameterListToString(theEtaUncertaintyList);
}	
	
void RBooreEtAl2013IntensityModel::setEtaUncertaintyList(QString value)	
{	
	theEtaUncertaintyList = stringToParameterList(value);

	createResponses();
}	


QList<RParameter *> RBooreEtAl2013IntensityModel::getPhysicalParameterList()
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

QString RBooreEtAl2013IntensityModel::getFaultTypes() const	
{
	QMetaEnum metaEnum = metaObject()->enumerator(metaObject()->indexOfEnumerator("RFaultType"));
	QString result = "";
	for (int i = 0; i < theFaultTypes.count(); i++) {
		QString temp = metaEnum.valueToKey(theFaultTypes[i]);
	    result += temp + "; ";
	}
	return result;
}	
	
void RBooreEtAl2013IntensityModel::setFaultTypes(QString value)	
{	
	QString string = value;
	QMetaEnum metaEnum = metaObject()->enumerator(metaObject()->indexOfEnumerator("RFaultType"));

	QRegExp regExp = QRegExp(QString("[ ,;:\t\n\v\r%1%2]").arg(QChar(QChar::ParagraphSeparator)).arg(QChar(QChar::LineSeparator)));
	QStringList stringList = string.split(regExp, QString::SkipEmptyParts);
	string = "";
	QList<RBooreEtAl2013IntensityModel::RFaultType> tempList;
	for (int i = 0; i < stringList.count(); i++) {
		QByteArray ba = stringList[i].toLocal8Bit();
		const char *tempChar = ba.data();
		int tempValue = metaEnum.keyToValue(tempChar);
		RBooreEtAl2013IntensityModel::RFaultType temp = RFaultType(tempValue);
		if (temp < 0) {
			rCritical() << "Error in the Model" << objectName() << ": The fault type" << stringList[i] << "is not valid";
			return;
		}
		tempList.append(temp);
	}
	theFaultTypes = tempList;
}	

QString RBooreEtAl2013IntensityModel::getRegions() const	
{
	QMetaEnum metaEnum = metaObject()->enumerator(metaObject()->indexOfEnumerator("RRegion"));
	QString result = "";
	for (int i = 0; i < theRegions.count(); i++) {
		QString temp = metaEnum.valueToKey(theRegions[i]);
	    result += temp + "; ";
	}
	return result;
}	
	

void RBooreEtAl2013IntensityModel::setRegions(QString value)	
{	
	QString string = value;
	QMetaEnum metaEnum = metaObject()->enumerator(metaObject()->indexOfEnumerator("RRegion"));

	QRegExp regExp = QRegExp(QString("[ ,;:\t\n\v\r%1%2]").arg(QChar(QChar::ParagraphSeparator)).arg(QChar(QChar::LineSeparator)));
	QStringList stringList = string.split(regExp, QString::SkipEmptyParts);
	string = "";
	QList<RBooreEtAl2013IntensityModel::RRegion> tempList;
	for (int i = 0; i < stringList.count(); i++) {
		QByteArray ba = stringList[i].toLocal8Bit();
		const char *tempChar = ba.data();
		int tempValue = metaEnum.keyToValue(tempChar);
		RBooreEtAl2013IntensityModel::RRegion temp = RRegion(tempValue);
		if (temp < 0) {
			rCritical() << "Error in the Model" << objectName() << ": The region" << stringList[i] << "is not valid";
			return;
		}
		tempList.append(temp);
	}
	theRegions = tempList;
}	

RBooreEtAl2013IntensityModel::RResponseType RBooreEtAl2013IntensityModel::getResponseType() const	
{	
	return theResponseType;
}	
	
void RBooreEtAl2013IntensityModel::setResponseType(RBooreEtAl2013IntensityModel::RResponseType value)	
{	
	theResponseType = value;
}


QString RBooreEtAl2013IntensityModel::getPeriodList()	
{	
	return parameterListToString(thePeriodList);
}	
	
void RBooreEtAl2013IntensityModel::setPeriodList(QString value)	
{	
	thePeriodList = stringToParameterList(value);

	createResponses();
}	
	
QString RBooreEtAl2013IntensityModel::getStructureLocationList()	
{	
	return parameterListToString(theStructureLocationList);
}	
	
void RBooreEtAl2013IntensityModel::setStructureLocationList(QString value)	
{	
	theStructureLocationList = stringToParameterList(value);

	// Checking if all the passed parameters are either RLocation or RLocationResponse
	parameterListToLocationList(theStructureLocationList);

	createResponses();
}	
	
QString RBooreEtAl2013IntensityModel::getShearWaveVelocityList()	
{	
	return parameterListToString(theShearWaveVelocityList);
}	
	
void RBooreEtAl2013IntensityModel::setShearWaveVelocityList(QString value)	
{	
	theShearWaveVelocityList = stringToParameterList(value);

	createResponses();
}	

QString RBooreEtAl2013IntensityModel::getDepthToVs1List()	
{	
	return parameterListToString(theDepthToVs1List);
}	
	
void RBooreEtAl2013IntensityModel::setDepthToVs1List(QString value)	
{	
	theDepthToVs1List = stringToParameterList(value);

	createResponses();
}

void RBooreEtAl2013IntensityModel::createResponses()
{
	// Removing old responses
	while (theSpectralResponseList.count() > 0) {
		delete theSpectralResponseList.takeAt(0);
	}

	if (theStructureLocationList.isEmpty() || theShearWaveVelocityList.isEmpty()) {
		return;
	}

	if ((theStructureLocationList.count() != thePeriodList.count() && theResponseType == RBooreEtAl2013IntensityModel::Sa) || (theStructureLocationList.count() != theShearWaveVelocityList.count() && theShearWaveVelocityList.count() > 1) || (theStructureLocationList.count() != theEpsilonUncertaintyList.count() && theEpsilonUncertaintyList.count() > 1) || (theStructureLocationList.count() != theEtaUncertaintyList.count() && theEtaUncertaintyList.count() > 1)) {
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

int RBooreEtAl2013IntensityModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
{	
	if (theStructureLocationList.isEmpty()) {
		rCritical(threadID) << "Error: There should be at least one structure location in the model" << objectName() << ".";
		return -1;
	}
	if ((theStructureLocationList.count() != thePeriodList.count() && theResponseType == RBooreEtAl2013IntensityModel::Sa) || (theStructureLocationList.count() != theShearWaveVelocityList.count() && theShearWaveVelocityList.count() > 1)) {
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
	double Mref = 4.5;
	double Rref = 1.0; // km
	double deltac3gCATW = 0; // For globalCATW
	double Vref = 760; // m/s
	double f1 = 0; 
	double f3 = 0.1;
	double V1 = 225; // m/s
	double V2 = 300; // m/s

	// Period dependent coefficients
	QVector<double> periodTableVector;
	periodTableVector   << -1 <<  0 <<  0.01 <<  0.02 <<  0.03 <<  0.05 <<  0.075 <<  0.1 <<  0.15 <<  0.2 <<  0.25 <<  0.3 <<  0.4 <<  0.5 <<  0.75 <<  1 <<  1.5 <<  2 <<  3 <<  4 <<  5 <<  7.5 <<  10 ;

	QVector<double> c1Vector;
	c1Vector  << -1.243 <<  -1.134 <<  -1.134 <<  -1.1394 <<  -1.1421 <<  -1.1159 <<  -1.0831 <<  -1.0652 <<  -1.0532 <<  -1.0607 <<  -1.0773 <<  -1.0948 <<  -1.1243 <<  -1.1459 <<  -1.1777 <<  -1.193 <<  -1.2063 <<  -1.2159 <<  -1.2179 <<  -1.2162 <<  -1.2189 <<  -1.2543 <<  -1.3253 ;
	QVector<double> c2Vector;
	c2Vector  << 0.1489 <<  0.1917 <<  0.1916 <<  0.18962 <<  0.18842 <<  0.18709 <<  0.18225 <<  0.17203 <<  0.15401 <<  0.14489 <<  0.13925 <<  0.13388 <<  0.12512 <<  0.12015 <<  0.11054 <<  0.10248 <<  0.09645 <<  0.09636 <<  0.09764 <<  0.10218 <<  0.10353 <<  0.12507 <<  0.15183 ;
	QVector<double> c3Vector;
	c3Vector  << -0.00344 <<  -0.008088 <<  -0.008088 <<  -0.008074 <<  -0.008336 <<  -0.009819 <<  -0.01058 <<  -0.0102 <<  -0.008977 <<  -0.007717 <<  -0.006517 <<  -0.005475 <<  -0.004053 <<  -0.00322 <<  -0.001931 <<  -0.00121 <<  -0.000365 <<  0 <<  0 <<  -0.000052 <<  0 <<  0 <<  0 ;
	QVector<double> hVector;
	hVector  << 5.3 <<  4.5 <<  4.5 <<  4.5 <<  4.49 <<  4.2 <<  4.04 <<  4.13 <<  4.39 <<  4.61 <<  4.78 <<  4.93 <<  5.16 <<  5.34 <<  5.6 <<  5.74 <<  6.18 <<  6.54 <<  6.93 <<  7.32 <<  7.78 <<  9.48 <<  9.66 ;

	QVector<double> e0Vector;
	e0Vector  << 5.037 <<  0.4473 <<  0.4534 <<  0.48598 <<  0.56916 <<  0.75436 <<  0.96447 <<  1.1268 <<  1.3095 <<  1.3255 <<  1.2766 <<  1.2217 <<  1.1046 <<  0.96991 <<  0.66903 <<  0.3932 <<  -0.14954 <<  -0.58669 <<  -1.1898 <<  -1.6388 <<  -1.966 <<  -2.5865 <<  -3.0702 ;
	QVector<double> e1Vector;
	e1Vector  << 5.078 <<  0.4856 <<  0.4916 <<  0.52359 <<  0.6092 <<  0.79905 <<  1.0077 <<  1.1669 <<  1.3481 <<  1.359 <<  1.3017 <<  1.2401 <<  1.1214 <<  0.99106 <<  0.69737 <<  0.4218 <<  -0.11866 <<  -0.55003 <<  -1.142 <<  -1.5748 <<  -1.8882 <<  -2.4874 <<  -2.9537 ;
	QVector<double> e2Vector;
	e2Vector  << 4.849 <<  0.2459 <<  0.2519 <<  0.29707 <<  0.40391 <<  0.60652 <<  0.77678 <<  0.8871 <<  1.0648 <<  1.122 <<  1.0828 <<  1.0246 <<  0.89765 <<  0.7615 <<  0.47523 <<  0.207 <<  -0.3138 <<  -0.71466 <<  -1.23 <<  -1.6673 <<  -2.0245 <<  -2.8176 <<  -3.3776 ;
	QVector<double> e3Vector;
	e3Vector  << 5.033 <<  0.4539 <<  0.4599 <<  0.48875 <<  0.55783 <<  0.72726 <<  0.9563 <<  1.1454 <<  1.3324 <<  1.3414 <<  1.3052 <<  1.2653 <<  1.1552 <<  1.012 <<  0.69173 <<  0.4124 <<  -0.1437 <<  -0.60658 <<  -1.2664 <<  -1.7516 <<  -2.0928 <<  -2.6854 <<  -3.1726 ;
	QVector<double> e4Vector;
	e4Vector  << 1.073 <<  1.431 <<  1.421 <<  1.4331 <<  1.4261 <<  1.3974 <<  1.4174 <<  1.4293 <<  1.2844 <<  1.1349 <<  1.0166 <<  0.95676 <<  0.96766 <<  1.0384 <<  1.2871 <<  1.5004 <<  1.7622 <<  1.9152 <<  2.1323 <<  2.204 <<  2.2299 <<  2.1187 <<  1.8837 ;
	QVector<double> e5Vector;
	e5Vector  << -0.1536 <<  0.05053 <<  0.04932 <<  0.053388 <<  0.061444 <<  0.067357 <<  0.073549 <<  0.055231 <<  -0.042065 <<  -0.11096 <<  -0.16213 <<  -0.1959 <<  -0.22608 <<  -0.23522 <<  -0.21591 <<  -0.18983 <<  -0.1467 <<  -0.11237 <<  -0.04332 <<  -0.014642 <<  -0.014855 <<  -0.081606 <<  -0.15096 ;
	QVector<double> e6Vector;
	e6Vector  << 0.2252 <<  -0.1662 <<  -0.1659 <<  -0.16561 <<  -0.1669 <<  -0.18082 <<  -0.19665 <<  -0.19838 <<  -0.18234 <<  -0.15852 <<  -0.12784 <<  -0.092855 <<  -0.023189 <<  0.029119 <<  0.10829 <<  0.17895 <<  0.33896 <<  0.44788 <<  0.62694 <<  0.76303 <<  0.87314 <<  1.0121 <<  1.0651 ;

	QVector<double> MhVector;
	MhVector  << 6.2 <<  5.5 <<  5.5 <<  5.5 <<  5.5 <<  5.5 <<  5.5 <<  5.54 <<  5.74 <<  5.92 <<  6.05 <<  6.14 <<  6.2 <<  6.2 <<  6.2 <<  6.2 <<  6.2 <<  6.2 <<  6.2 <<  6.2 <<  6.2 <<  6.2 <<  6.2 ;

	// deltac3 for China Turkey
	QVector<double> deltac3ChTrVector;
	deltac3ChTrVector  << 0.00435 <<  0.00286 <<  0.00282 <<  0.00278 <<  0.00276 <<  0.00296 <<  0.00296 <<  0.00288 <<  0.00279 <<  0.00261 <<  0.00244 <<  0.0022 <<  0.00211 <<  0.00235 <<  0.00269 <<  0.00292 <<  0.00304 <<  0.00292 <<  0.00262 <<  0.00261 <<  0.0026 <<  0.0026 <<  0.00303 ;
	// deltac3 for Italy Japan
	QVector<double> deltac3ItJaVector;
	deltac3ItJaVector  << -0.00033 <<  -0.00255 <<  -0.00244 <<  -0.00234 <<  -0.00217 <<  -0.00199 <<  -0.00216 <<  -0.00244 <<  -0.00271 <<  -0.00297 <<  -0.00314 <<  -0.0033 <<  -0.00321 <<  -0.00291 <<  -0.00253 <<  -0.00209 <<  -0.00152 <<  -0.00117 <<  -0.00119 <<  -0.00108 <<  -0.00057 <<  0.00038 <<  0.00149 ;

	QVector<double> cVector;
	cVector  << -0.84 <<  -0.6 <<  -0.6037 <<  -0.5739 <<  -0.5341 <<  -0.458 <<  -0.4441 <<  -0.4872 <<  -0.5796 <<  -0.6876 <<  -0.7718 <<  -0.8417 <<  -0.9109 <<  -0.9693 <<  -1.0154 <<  -1.05 <<  -1.0454 <<  -1.0392 <<  -1.0112 <<  -0.9694 <<  -0.9195 <<  -0.7766 <<  -0.6558 ;
	QVector<double> VcVector;
	VcVector  << 1300 <<  1500 <<  1500.2 <<  1500.36 <<  1502.95 <<  1501.42 <<  1494 <<  1479.12 <<  1442.85 <<  1392.61 <<  1356.21 <<  1308.47 <<  1252.66 <<  1203.91 <<  1147.59 <<  1109.95 <<  1072.39 <<  1009.49 <<  922.43 <<  844.48 <<  793.13 <<  771.01 <<  775 ;

	QVector<double> f4Vector;
	f4Vector  << -0.1 <<  -0.15 <<  -0.14833 <<  -0.1471 <<  -0.15485 <<  -0.192 <<  -0.235 <<  -0.24916 <<  -0.25713 <<  -0.24658 <<  -0.23574 <<  -0.21912 <<  -0.19582 <<  -0.175 <<  -0.13866 <<  -0.10521 <<  -0.062 <<  -0.036136 <<  -0.013577 <<  -0.0032123 <<  -0.0002548 <<  -0.0000546 <<  0 ;
	QVector<double> f5Vector;
	f5Vector  << -0.00844 <<  -0.00701 <<  -0.00701 <<  -0.00728 <<  -0.00735 <<  -0.00647 <<  -0.00573 <<  -0.0056 <<  -0.00585 <<  -0.00614 <<  -0.00644 <<  -0.0067 <<  -0.00713 <<  -0.00744 <<  -0.00812 <<  -0.00844 <<  -0.00771 <<  -0.00479 <<  -0.00183 <<  -0.00152 <<  -0.00144 <<  -0.00137 <<  -0.00136 ;
	QVector<double> f6Vector;
	f6Vector  << -9.9 <<  -9.9 <<  -9.9 <<  -9.9 <<  -9.9 <<  -9.9 <<  -9.9 <<  -9.9 <<  -9.9 <<  -9.9 <<  -9.9 <<  -9.9 <<  -9.9 <<  -9.9 <<  0.092 <<  0.367 <<  0.638 <<  0.871 <<  1.135 <<  1.271 <<  1.329 <<  1.329 <<  1.183 ;
	QVector<double> f7Vector;
	f7Vector  << -9.9 <<  -9.9 <<  -9.9 <<  -9.9 <<  -9.9 <<  -9.9 <<  -9.9 <<  -9.9 <<  -9.9 <<  -9.9 <<  -9.9 <<  -9.9 <<  -9.9 <<  -9.9 <<  0.059 <<  0.208 <<  0.309 <<  0.382 <<  0.516 <<  0.629 <<  0.738 <<  0.809 <<  0.703 ;

	QVector<double> R1Vector;
	R1Vector  << 105 <<  110 <<  111.67 <<  113.1 <<  112.13 <<  97.93 <<  85.99 <<  79.59 <<  81.33 <<  90.91 <<  97.04 <<  103.15 <<  106.02 <<  105.54 <<  108.39 <<  116.39 <<  125.38 <<  130.37 <<  130.36 <<  129.49 <<  130.22 <<  130.72 <<  130 ;
	QVector<double> R2Vector;
	R2Vector  << 272 <<  270 <<  270 <<  270 <<  270 <<  270 <<  270.04 <<  270.09 <<  270.16 <<  270 <<  269.45 <<  268.59 <<  266.54 <<  265 <<  266.51 <<  270 <<  262.41 <<  240.14 <<  195 <<  199.45 <<  230 <<  250.39 <<  210 ;

	QVector<double> deltaphiRVector;
	deltaphiRVector  << 0.082 <<  0.1 <<  0.096 <<  0.092 <<  0.081 <<  0.063 <<  0.064 <<  0.087 <<  0.12 <<  0.136 <<  0.141 <<  0.138 <<  0.122 <<  0.109 <<  0.1 <<  0.098 <<  0.104 <<  0.105 <<  0.088 <<  0.07 <<  0.061 <<  0.058 <<  0.06 ;
	QVector<double> deltaphiVVector;
	deltaphiVVector  << 0.08 <<  0.07 <<  0.07 <<  0.03 <<  0.029 <<  0.03 <<  0.022 <<  0.014 <<  0.015 <<  0.045 <<  0.055 <<  0.05 <<  0.049 <<  0.06 <<  0.07 <<  0.02 <<  0.01 <<  0.008 <<  0 <<  0 <<  0 <<  0 <<  0 ;

	QVector<double> phi1Vector;
	phi1Vector  << 0.644 <<  0.695 <<  0.698 <<  0.702 <<  0.721 <<  0.753 <<  0.745 <<  0.728 <<  0.72 <<  0.711 <<  0.698 <<  0.675 <<  0.643 <<  0.615 <<  0.581 <<  0.553 <<  0.532 <<  0.526 <<  0.534 <<  0.536 <<  0.528 <<  0.512 <<  0.51 ;
	QVector<double> phi2Vector;
	phi2Vector  << 0.552 <<  0.495 <<  0.499 <<  0.502 <<  0.514 <<  0.532 <<  0.542 <<  0.541 <<  0.537 <<  0.539 <<  0.547 <<  0.561 <<  0.58 <<  0.599 <<  0.622 <<  0.625 <<  0.619 <<  0.618 <<  0.619 <<  0.616 <<  0.622 <<  0.634 <<  0.604 ;

	QVector<double> tau1Vector;
	tau1Vector  << 0.401 <<  0.398 <<  0.402 <<  0.409 <<  0.445 <<  0.503 <<  0.474 <<  0.415 <<  0.354 <<  0.344 <<  0.35 <<  0.363 <<  0.381 <<  0.41 <<  0.457 <<  0.498 <<  0.525 <<  0.532 <<  0.537 <<  0.543 <<  0.532 <<  0.511 <<  0.487 ;
	QVector<double> tau2Vector;
	tau2Vector  << 0.346 <<  0.348 <<  0.345 <<  0.346 <<  0.364 <<  0.426 <<  0.466 <<  0.458 <<  0.388 <<  0.309 <<  0.266 <<  0.229 <<  0.21 <<  0.224 <<  0.266 <<  0.298 <<  0.315 <<  0.329 <<  0.344 <<  0.349 <<  0.335 <<  0.27 <<  0.239 ;


	double T;
	if (theResponseType == RBooreEtAl2013IntensityModel::PGA) {
		T = 0.0;
	} else if (theResponseType == RBooreEtAl2013IntensityModel::PGV) {
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
		
		RBooreEtAl2013IntensityModel::RFaultType theFaultType;
		if (theFaultTypes.count() == 1) {
			theFaultType = theFaultTypes[0];
		} else {
			theFaultType = theFaultTypes[index];
		}

		if (theFaultType == RBooreEtAl2013IntensityModel::Unspecified) {
			U = 1.0;
		} else if (theFaultType == RBooreEtAl2013IntensityModel::StrikeSlip) {
			SS = 1.0;
		} else if (theFaultType == RBooreEtAl2013IntensityModel::NormalSlip) {
			NS = 1.0;
		} else if (theFaultType == RBooreEtAl2013IntensityModel::ReverseSlip) {
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
			double Rjb = theHypocentreLocationObject->computeSurfaceDistance(locationList[i], threadID);
			
			if (Rjb > 200.0) {
				if (getDisplayOutput()) {
					rCritical(threadID) << "Warning: The maximum allowable distance in the model" << objectName() << "is 200 km.";
				}
			}

			if (theResponseType == RBooreEtAl2013IntensityModel::Sa) {
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

			double z1;
			if (theDepthToVs1List.count() == 1) {
				z1 = theDepthToVs1List[0].data()->getCurrentValue(threadID);
			} else {
				z1 = theDepthToVs1List[i].data()->getCurrentValue(threadID);
			}

			RBooreEtAl2013IntensityModel::RRegion theRegion;
			if (theRegions.count() == 1) {
				theRegion = theRegions[0];
			} else {
				theRegion = theRegions[i];
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
				double Tj = periodTableVector[j];
				double e0 = e0Vector[j];
				double e1 = e1Vector[j];
				double e2 = e2Vector[j];
				double e3 = e3Vector[j];
				double e4 = e4Vector[j];
				double e5 = e5Vector[j];
				double e6 = e6Vector[j];
				double Mh = MhVector[j];

				double FE;			
				if (M <= Mh) {
					FE = e0 * U + e1 * SS + e2 * NS + e3 * RS + e4 * (M - Mh) + e5 * pow((M - Mh) , 2.0);
				} else {
					FE = e0 * U + e1 * SS + e2 * NS + e3 * RS + e6 * (M - Mh);
				}
				
				double h = hVector[j];
				double c1 = c1Vector[j];
				double c2 = c2Vector[j];
				double c3 = c3Vector[j];
				double deltac3 = 0;
				if (theRegion == RBooreEtAl2013IntensityModel::California || theRegion == RBooreEtAl2013IntensityModel:: Taiwan || theRegion == RBooreEtAl2013IntensityModel:: Other){
					deltac3 = deltac3gCATW;
				} else if (theRegion == RBooreEtAl2013IntensityModel:: Japan || theRegion == RBooreEtAl2013IntensityModel:: Italy){
					deltac3 = deltac3ItJaVector[j];
				} else if (theRegion == RBooreEtAl2013IntensityModel:: China ||theRegion == RBooreEtAl2013IntensityModel:: Turkey){
					deltac3 = deltac3ChTrVector[j];
				}
			

				double R = sqrt(Rjb * Rjb + h * h);
				
				double Fpb = (c1 + c2 * (M - Mref)) * log(R / Rref) + (c3 ) * (R-Rref);
				double Fp = Fpb + deltac3 * (R - Rref);
				
				double Vc = VcVector[j];
				double c = cVector[j];
				double f4 = f4Vector[j];
				double f5 = f5Vector[j];
				double f6 = f6Vector[j];
				double f7 = f7Vector[j];
				double lnFlin;
				if (Vs30 <= Vc) {
					lnFlin = c * log(Vs30/Vref);
				} else {
					lnFlin = c * log(Vc/Vref);
				}

				double f2;
				f2 = f4 * (exp(f5 * (std::min(Vs30 , 760.0) - 360)) - exp(f5 * (760 - 360)));
			
				double muz1;
				double deltaz1;
				double Fdeltaz1; 
				if (z1 != 999) {
					if (theRegion == RBooreEtAl2013IntensityModel:: Japan) {
						// muz1 for Japan
						muz1 = exp( (-5.23/2) * log((pow(Vs30, 2.0) + pow(412.39, 2.0)) / (pow(1360.0, 2.0) + pow(412.39, 2.0))) ) / 1000;
					} else {
						muz1 =exp( (-7.15/4) * log((pow(Vs30, 4.0) + pow(570.94, 4.0)) / (pow(1360.0, 4.0) + pow(570.94, 4.0))) ) / 1000;
					}
					deltaz1 = z1 - muz1;
				} else {
					deltaz1 = 0;
				}
				if (z1 != 999){
					if (Tj < 0.65) {
						Fdeltaz1 = 0;
					} else if (Tj >= 0.65 && abs(deltaz1) <= (f7 / f6)) {
						Fdeltaz1 = f6 * deltaz1;
					} else {
						Fdeltaz1 = f7;
					}
				} else {
					Fdeltaz1 = 0;
				}

				double tau1=tau1Vector[j];
				double tau2=tau2Vector[j];
				double phi1=phi1Vector[j];
				double phi2=phi2Vector[j];
				double phiM;
				double tau;
				if (M <= 4.5) {
					tau = tau1;
					phiM = phi1;
				} else if (M > 4.5 && M < 5.5) {
					tau = tau1 + (tau2 - tau1)*(M - 4.5);
					phiM = phi1 + (phi2 - phi1)*(M - 4.5);
				} else {
					tau = tau2;
					phiM = phi2;
				}


				double R1 = R1Vector[j];
				double R2 = R2Vector[j];
				double deltaphiR = deltaphiRVector[j];
				double deltaphiV = deltaphiVVector[j];
				
				double phiMRjb;
				if (Rjb <= R1) {
					phiMRjb = phiM;
				} else if (Rjb > R1 && Rjb <= R2) {
					phiMRjb = phiM + deltaphiR * log(Rjb/R1) / log(R2/R1);
				} else if (Rjb > R2) {
					phiMRjb = phiM + deltaphiR;
				}

				double phiMRjbVs30;
				if (Vs30 >= V2) {
					phiMRjbVs30 = phiMRjb;
				} else if (Vs30 >= V1 && Vs30 < V2) {
					phiMRjbVs30 = phiMRjb - deltaphiV * log(V2/Vs30) / log(V2/V1);
				} else if (Vs30 < V1) {
					phiMRjbVs30 = phiMRjb - deltaphiV;
				}
				
				/* This region is originally from the paper (but not needed):
				double sigma; 
				sigma = sqrt(phiMRjbVs30 * phiMRjbVs30 + tau * tau);
				*/

				double lnY = FE + Fp;

				// PGAr /////////////////////////////////
				e0 = e0Vector[1];
				e1 = e1Vector[1];
				e2 = e2Vector[1];
				e3 = e3Vector[1];
				e4 = e4Vector[1];
				e5 = e5Vector[1];
				e6 = e6Vector[1];
				Mh = MhVector[1];
				double Vs30PGA = 760;
				if (M <= Mh) {
					FE = e0 * U + e1 * SS + e2 * NS + e3 * RS + e4 * (M - Mh) + e5 * pow((M - Mh) , 2.0);
				} else {
					FE = e0 * U + e1 * SS + e2 * NS + e3 * RS + e6 * (M - Mh);
				}
				
				h = hVector[1];
				c1 = c1Vector[1];
				c2 = c2Vector[1];
				c3 = c3Vector[1];
				
				R = sqrt(Rjb * Rjb + h * h);
				
				Fpb = (c1 + c2 * (M - Mref)) * log(R / Rref) + (c3 ) * (R-Rref);
				Fp = Fpb + deltac3 * (R - Rref);
				
				/*
				Vc = VcVector[1];
				c = cVector[1];
				f4 = f4Vector[1];
				f5 = f5Vector[1];
				f6 = f6Vector[1];
				f7 = f7Vector[1];
				double lnFlinp;
				if (Vs30 <= Vc) {
					lnFlinp = c * log(Vs30/Vref);
				} else {
					lnFlinp = c * log(Vc/Vref);
				}

				double FdeltaZ1p = 0; //It is the result for PGAr 

				double lnFnlp = f1;
				double Fsbp = lnFlinp + lnFnlp;
				double Fsp = Fsbp + FdeltaZ1p;
				*/
				double lnPGAr = FE + Fp ;//+ Fsp;
				double PGAr = exp(lnPGAr);
				// PGAr //////////////////////////////////////////////////

				double lnFnl = f1 + f2 * log((PGAr + f3)/f3);
				double Fsb = lnFlin + lnFnl;
				double Fs = Fsb + Fdeltaz1;

				lnY = lnY + Fs + phiMRjbVs30 * epsilon + tau * eta;
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

bool RBooreEtAl2013IntensityModel::canBeRunParallelly(){
	return true;
}