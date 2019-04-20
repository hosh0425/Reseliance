#include "RAbrahamsonSilva2008IntensityModel.h"	

#include <QCoreApplication>
#include <QMetaEnum>
	
#include "RParameter.h"	
#include "RResponse.h"	
#include "RLocation.h"
#include "RLocationResponse.h"
#include "RMainWindow.h"


#include <math.h>

#include <gsl/gsl_math.h>
	
RAbrahamsonSilva2008IntensityModel::RAbrahamsonSilva2008IntensityModel(QObject *parent, QString name)	
	: RModel(parent, name)
{	
	//theEpsilon = 0;
	theResponseType = RAbrahamsonSilva2008IntensityModel::Sa;
	theShockType = RAbrahamsonSilva2008IntensityModel::MainShock;
	//theSmoothness = RAbrahamsonSilva2008IntensityModel::Smoothed;
}	
	
RAbrahamsonSilva2008IntensityModel::~RAbrahamsonSilva2008IntensityModel()	
{	
	
}	

QString RAbrahamsonSilva2008IntensityModel::getMagnitudeList()	
{	
	return parameterListToString(theMagnitudeList);
}	

void RAbrahamsonSilva2008IntensityModel::setMagnitudeList(QString value)	
{	
	theMagnitudeList = stringToParameterList(value);
}

QString RAbrahamsonSilva2008IntensityModel::getHypocentreLocationList()	
{	
	return parameterListToString(theHypocentreLocationList);
}	

void RAbrahamsonSilva2008IntensityModel::setHypocentreLocationList(QString value)	
{	

	theHypocentreLocationList = stringToParameterList(value);

	// Checking if all the passed parameters are either RLocation or RLocationResponse
	parameterListToLocationList(theHypocentreLocationList);
}


QString RAbrahamsonSilva2008IntensityModel::getFaultTypes() const	
{
	QMetaEnum metaEnum = metaObject()->enumerator(metaObject()->indexOfEnumerator("RFaultType"));
	QString result = "";
	for (int i = 0; i < theFaultTypes.count(); i++) {
		QString temp = metaEnum.valueToKey(theFaultTypes[i]);
	    result += temp + "; ";
	}
	return result;
}	
	
void RAbrahamsonSilva2008IntensityModel::setFaultTypes(QString value)	
{	
	QString string = value;
	QMetaEnum metaEnum = metaObject()->enumerator(metaObject()->indexOfEnumerator("RFaultType"));

	QRegExp regExp = QRegExp(QString("[ ,;:\t\n\v\r%1%2]").arg(QChar(QChar::ParagraphSeparator)).arg(QChar(QChar::LineSeparator)));
	QStringList stringList = string.split(regExp, QString::SkipEmptyParts);
	string = "";
	QList<RAbrahamsonSilva2008IntensityModel::RFaultType> tempList;
	for (int i = 0; i < stringList.count(); i++) {
		QByteArray ba = stringList[i].toLocal8Bit();
		const char *tempChar = ba.data();
		int tempValue = metaEnum.keyToValue(tempChar);
		RAbrahamsonSilva2008IntensityModel::RFaultType temp = RFaultType(tempValue);
		if (temp < 0) {
			rCritical() << "Error in the Model" << objectName() << ": The fault type" << stringList[i] << "is not valid";
			return;
		}
		tempList.append(temp);
	}
	theFaultTypes = tempList;
}	

RAbrahamsonSilva2008IntensityModel::RResponseType RAbrahamsonSilva2008IntensityModel::getResponseType() const	
{	
	return theResponseType;
}	
	
void RAbrahamsonSilva2008IntensityModel::setResponseType(RAbrahamsonSilva2008IntensityModel::RResponseType value)	
{	
	theResponseType = value;
}

RAbrahamsonSilva2008IntensityModel::RShockType RAbrahamsonSilva2008IntensityModel::getShockType() const	
{	
	return theShockType;
}	
	
void RAbrahamsonSilva2008IntensityModel::setShockType(RAbrahamsonSilva2008IntensityModel::RShockType value)	
{	
	theShockType = value;
}

QString RAbrahamsonSilva2008IntensityModel::getPeriodList()	
{	
	return parameterListToString(thePeriodList);
}	
	
void RAbrahamsonSilva2008IntensityModel::setPeriodList(QString value)	
{	
	thePeriodList = stringToParameterList(value);

	createResponses();
}	

QString RAbrahamsonSilva2008IntensityModel::getEpsilonUncertaintyList()	
{	
	return parameterListToString(theEpsilonUncertaintyList);
}	
	
void RAbrahamsonSilva2008IntensityModel::setEpsilonUncertaintyList(QString value)	
{	
	theEpsilonUncertaintyList = stringToParameterList(value);

	createResponses();
}	

QString RAbrahamsonSilva2008IntensityModel::getEtaUncertaintyList()	
{	
	return parameterListToString(theEtaUncertaintyList);
}	
	
void RAbrahamsonSilva2008IntensityModel::setEtaUncertaintyList(QString value)	
{	
	theEtaUncertaintyList = stringToParameterList(value);

	createResponses();
}	


QList<RParameter *> RAbrahamsonSilva2008IntensityModel::getPhysicalParameterList()
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

QString RAbrahamsonSilva2008IntensityModel::getStructureLocationList()	
{	
	return parameterListToString(theStructureLocationList);
}	
	
void RAbrahamsonSilva2008IntensityModel::setStructureLocationList(QString value)	
{	
	theStructureLocationList = stringToParameterList(value);

	// Checking if all the passed parameters are either RLocation or RLocationResponse
	parameterListToLocationList(theStructureLocationList);

	createResponses();
}	
	
QString RAbrahamsonSilva2008IntensityModel::getShearWaveVelocityList()	
{	
	return parameterListToString(theShearWaveVelocityList);
}	
	
void RAbrahamsonSilva2008IntensityModel::setShearWaveVelocityList(QString value)	
{	
	theShearWaveVelocityList = stringToParameterList(value);

	createResponses();
}	

void RAbrahamsonSilva2008IntensityModel::createResponses()
{
	// Removing old responses
	while (theSpectralResponseList.count() > 0) {
		delete theSpectralResponseList.takeAt(0);
	}

	if (theStructureLocationList.isEmpty() || theShearWaveVelocityList.isEmpty()) {
		return;
	}

	if ((theStructureLocationList.count() != thePeriodList.count() && theResponseType == RAbrahamsonSilva2008IntensityModel::Sa) || (theStructureLocationList.count() != theShearWaveVelocityList.count() && theShearWaveVelocityList.count() > 1) || (theStructureLocationList.count() != theEpsilonUncertaintyList.count() && theEpsilonUncertaintyList.count() > 1) || (theStructureLocationList.count() != theEtaUncertaintyList.count() && theEtaUncertaintyList.count() > 1)) {
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


int RAbrahamsonSilva2008IntensityModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
{	
	if (theStructureLocationList.isEmpty()) {
		rCritical(threadID) << "Error: There should be at least one structure location in the model" << objectName() << ".";
		return -1;
	}
	if ((theStructureLocationList.count() != thePeriodList.count() && theResponseType == RAbrahamsonSilva2008IntensityModel::Sa) || (theStructureLocationList.count() != theShearWaveVelocityList.count() && theShearWaveVelocityList.count() > 1)) {
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


	// Some constants
	double c1 = 6.75;
	double c4 = 4.5;
	double a3 = 0.265;
	double a4 = -0.231;
	double a5 = -0.398;
	double n = 1.18;
	double c = 1.88;
	double c2 = 50;
	double FAS = 0;
	
	if (theShockType == RAbrahamsonSilva2008IntensityModel::AfterShock) {
		FAS = 1.0;
	} else if (theShockType == RAbrahamsonSilva2008IntensityModel::MainShock) {
		FAS = 0.0;
	}

	// Period dependent coefficients
	QVector<double> periodTableVector;
	periodTableVector << -1.00 << 0.00 << 0.010  << 0.020  << 0.030  << 0.040  << 0.050  << 0.075  << 0.100  << 0.150  << 0.200  << 0.250  << 0.300  << 0.400  << 0.500  << 0.750  << 1.000  << 1.500  << 2.000  << 3.000  << 4.000  << 5.000  << 7.500  << 10.000  ; 

	QVector<double> VlinVector;
	QVector<double> bVector;
	QVector<double> a1Vector;
	QVector<double> a2Vector;
	QVector<double> a8Vector;
	QVector<double> a10Vector;
	QVector<double> a12Vector;
	QVector<double> a13Vector;
	QVector<double> a14Vector;
	QVector<double> a15Vector;
	QVector<double> a16Vector;
	QVector<double> a17Vector;
	QVector<double> a18Vector;
	QVector<double> s1Vector;
	QVector<double> s2Vector;
	QVector<double> s3Vector;
	QVector<double> s4Vector;


	VlinVector  << 400  << 865.1  << 865.1  << 865.1  << 907.8  << 994.5  << 1053.5  << 1085.7  << 1032.5  << 877.6  << 748.2  << 654.3  << 587.1  << 503  << 456.6  << 410.5  << 400  << 400  << 400  << 400  << 400  << 400  << 400  << 400 ;
	bVector  << -1.955  << -1.186  << -1.186  << -1.219  << -1.273  << -1.308  << -1.346  << -1.471  << -1.624  << -1.931  << -2.188  << -2.381  << -2.518  << -2.657  << -2.669  << -2.401  << -1.955  << -1.025  << -0.299  << 0  << 0  << 0  << 0  << 0 ;
	a1Vector  << 5.772  << 0.725  << 0.731  << 0.797  << 0.929  << 1.033  << 1.138  << 1.393  << 1.566  << 1.667  << 1.64  << 1.56  << 1.466  << 1.372  << 1.28  << 1.054  << 0.816  << 0.496  << 0.187  << -0.225  << -0.544  << -0.823  << -1.416  << -1.865 ;
	a2Vector  << -0.91  << -0.968  << -0.968  << -0.9818  << -1.0095  << -1.0367  << -1.0593  << -1.0921  << -1.0941  << -1.0431  << -0.9837  << -0.9305  << -0.8834  << -0.8601  << -0.8421  << -0.8093  << -0.7946  << -0.7946  << -0.7946  << -0.7946  << -0.7946  << -0.7946  << -0.7946  << -0.7946 ;
	a8Vector  << -0.1  << 0  << 0  << 0  << 0  << 0.0038  << 0.0102  << 0.0218  << 0.0266  << 0.0206  << 0.0018  << -0.0128  << -0.0247  << -0.0435  << -0.0581  << -0.088  << -0.1092  << -0.1391  << -0.1603  << -0.1936  << -0.2173  << -0.2357  << -0.2691  << -0.2927 ;
	a10Vector  << 1.587  << 0.9485  << 0.9485  << 0.9874  << 1.0511  << 1.0924  << 1.1373  << 1.2848  << 1.4653  << 1.8107  << 2.08  << 2.2814  << 2.4216  << 2.5516  << 2.5395  << 2.1643  << 1.5581  << 0.348  << -0.5887  << -0.9415  << -0.9415  << -0.9118  << -0.7675  << -0.6651 ;
	a12Vector  << -0.05  << -0.12  << -0.12  << -0.12  << -0.12  << -0.12  << -0.12  << -0.12  << -0.12  << -0.1077  << -0.0989  << -0.0921  << -0.0866  << -0.0779  << -0.0711  << -0.0587  << -0.05  << -0.05  << -0.05  << -0.05  << -0.05  << -0.05  << -0.05  << -0.05 ;
	a13Vector  << -0.2  << -0.05  << -0.05  << -0.05  << -0.05  << -0.05  << -0.05  << -0.05  << -0.05  << -0.05  << -0.05  << -0.05  << -0.05  << -0.05  << -0.05  << -0.0778  << -0.1136  << -0.1642  << -0.2  << -0.2  << -0.2  << -0.2  << -0.2  << -0.2 ;
	a14Vector  << 0.7  << 1.08  << 1.08  << 1.08  << 1.1331  << 1.1708  << 1.2  << 1.2  << 1.2  << 1.1683  << 1.1274  << 1.0956  << 1.0697  << 1.0288  << 0.9971  << 0.9395  << 0.8985  << 0.8409  << 0.8  << 0.4793  << 0.2518  << 0.0754  << 0  << 0 ;
	a15Vector  << -0.2841  << -0.405  << -0.4018  << -0.4098  << -0.4295  << -0.4478  << -0.4724  << -0.5036  << -0.5178  << -0.5198  << -0.4407  << -0.3824  << -0.3942  << -0.3412  << -0.3763  << -0.353  << -0.2591  << -0.2651  << -0.1746  << -0.0883  << -0.0089  << -0.1573  << -0.0282  << 0.0516 ;
	a16Vector  << 0.46  << 0.65  << 0.65  << 0.65  << 0.65  << 0.65  << 0.65  << 0.65  << 0.65  << 0.65  << 0.65  << 0.65  << 0.65  << 0.65  << 0.65  << 0.5233  << 0.4333  << 0.3066  << 0.2167  << 0.0899  << 0  << 0  << 0  << 0 ;
	a17Vector  << 0.35  << 0.6  << 0.6  << 0.6  << 0.6  << 0.6  << 0.6  << 0.6  << 0.6  << 0.6  << 0.6  << 0.5506  << 0.5102  << 0.4464  << 0.397  << 0.3071  << 0.2434  << 0.1536  << 0.0898  << 0  << 0  << 0  << 0  << 0 ;
	a18Vector  << 0  << -0.0067  << -0.0067  << -0.0067  << -0.0067  << -0.0067  << -0.0076  << -0.0093  << -0.0093  << -0.0093  << -0.0083  << -0.0069  << -0.0057  << -0.0039  << -0.0025  << 0  << 0  << 0  << 0  << 0  << 0  << 0  << 0  << 0 ;
	s1Vector  << 0.704  << 0.562  << 0.562  << 0.561  << 0.568  << 0.573  << 0.577  << 0.584  << 0.59  << 0.599  << 0.605  << 0.609  << 0.613  << 0.617  << 0.62  << 0.624  << 0.625  << 0.625  << 0.624  << 0.626  << 0.626  << 0.629  << 0.617  << 0.619 ;
	s2Vector  << 0.711  << 0.438  << 0.438  << 0.437  << 0.445  << 0.45  << 0.454  << 0.461  << 0.467  << 0.476  << 0.48  << 0.484  << 0.487  << 0.491  << 0.493  << 0.496  << 0.494  << 0.488  << 0.482  << 0.491  << 0.503  << 0.527  << 0.491  << 0.493 ;
	s3Vector  << 0.419  << 0.479  << 0.479  << 0.479  << 0.485  << 0.489  << 0.493  << 0.499  << 0.503  << 0.509  << 0.513  << 0.517  << 0.519  << 0.523  << 0.526  << 0.531  << 0.533  << 0.536  << 0.538  << 0.541  << 0.542  << 0.543  << 0.545  << 0.546 ;
	s4Vector  << 0.237  << 0.286  << 0.286  << 0.286  << 0.292  << 0.296  << 0.3  << 0.307  << 0.311  << 0.316  << 0.319  << 0.321  << 0.322  << 0.325  << 0.327  << 0.329  << 0.331  << 0.333  << 0.334  << 0.336  << 0.336  << 0.336  << 0.337  << 0.337 ;

	
	double T;
	if (theResponseType == RAbrahamsonSilva2008IntensityModel::PGA) {
		T = 0.0;
	} else if (theResponseType == RAbrahamsonSilva2008IntensityModel::PGV) {
		T = -1.0;
	} 


	QVector<double> spectralResponseVector;
	for (int i = 0; i < locationList.count(); i++) {
		spectralResponseVector << 0.0;
	}

	for (int index = 0; index < theMagnitudeList.count(); index++) {
		
		double FRV = 0;
		double FN = 0;
		RAbrahamsonSilva2008IntensityModel::RFaultType theFaultType;
		if (theFaultTypes.count() == 1) {
			theFaultType = theFaultTypes[0];
		} else {
			theFaultType = theFaultTypes[index];
		}

		if (theFaultType == RAbrahamsonSilva2008IntensityModel::StrikeSlip) {
			FRV = 0;
			FN = 0;
		} else if (theFaultType == RAbrahamsonSilva2008IntensityModel::ReverseSlip) {
			FRV = 1.0;
		} else if (theFaultType == RAbrahamsonSilva2008IntensityModel::NormalSlip) {
			FN = 1.0;
		}
		
		// For convenience, look up the values of the parameters first
		double M = theMagnitudeList[index].data()->getCurrentValue(threadID);
		
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
			double Rjb = theHypocentreLocationObject->computeSurfaceDistance(locationList[i],threadID);
			if (Rjb > 200.0) {
				if (getDisplayOutput()) {
					rCritical(threadID) << "Warning: The maximum allowable distance in the model" << objectName() << "is 200 km.";
				}
			}

			if (theResponseType == RAbrahamsonSilva2008IntensityModel::Sa) {
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
				double Vlin = VlinVector[j];
				double b = bVector[j];
				double a1 = a1Vector[j];
				double a2 = a2Vector[j];
				double a8 = a8Vector[j];
				double a10 = a10Vector[j];
				double a12 = a12Vector[j];
				double a13 = a13Vector[j];
				double a14 = a14Vector[j];
				double a15 = a15Vector[j];
				double a16 = a16Vector[j];
				double a17 = a17Vector[j];
				double a18 = a18Vector[j];
				double s1 = s1Vector[j];
				double s2 = s2Vector[j];
				double s3 = s3Vector[j];
				double s4 = s4Vector[j];

				double Ztop ;
				if ( FN==0 && FRV==0  ){
					Ztop = 5.63 + 0.68 * M ;
				}else if ( FN==1 || FRV==1 ){
					Ztop = 11.24 - 0.2 * M ;
				}
				double R11 = sqrt ( Rjb * Rjb + Ztop * Ztop);
				double R =  sqrt ( R11 * R11 + c4 * c4);
			
				double F1;
					if ( M <= c1){
					F1 = a1 + a4 * ( M - c1 ) + a8 * ( 8.5 - M ) * ( 8.5 - M ) + ( a2 + a3 * (M - c1 ))* log(R) ;
					} else if ( M > c1){
					F1 = a1 + a5 * ( M - c1 ) + a8 * ( 8.5 - M ) * ( 8.5 - M ) + ( a2 + a3 * (M - c1 ))* log(R) ;
					}
				if	(getDisplayOutput())	{
					rDebug(threadID)	<<	"M:"	<<	M	<<" "	 ;
				}
				if	(getDisplayOutput())	{
					rDebug(threadID)	<<	"R:"	<<	R	<<" "	 ;
				}
				if	(getDisplayOutput())	{				
					rDebug(threadID)	<<	"a1:"	<<	a1	<<" "	 ;
				}
				if	(getDisplayOutput())	{			
					rDebug(threadID)	<<	"a2:"	<<	a2	<<" "	 ;
				}
				if	(getDisplayOutput())	{			
					rDebug(threadID)	<<	"a3:"	<<	a3	<<" "	 ;
				}
				if	(getDisplayOutput())	{
					rDebug(threadID)	<<	"a4:"	<<	a4	<<" "	 ;
				}
				if	(getDisplayOutput())	{
					rDebug(threadID)	<<	"a8:"	<<	a8	<<" "	 ;
				}
				
				double F6;
				if ( Ztop <= 2 ){
					F6 = 0;
				}else if (Ztop > 2 && Ztop < 5){
					F6 = a16 * ( Ztop - 2 ) / (3);
				}else if (Ztop >= 5){
					F6 = a16;
				}
				
				double F7;
				if ( Ztop <= 0 ){
					F7 = 0;
				}else if (Ztop > 2 && Ztop < 10){
					F7 = a17 * ( Ztop - 2 ) / (8);
				}else if (Ztop >= 10){
					F7 = a17;
				}
				
				double T6;
				if ( M <= 5.5 ){
					T6 = 1;
				}else if (M > 5.5 && M < 6.5){
					T6 = 6.5 - M ;
				}else if ( M >6.5 ){
					T6 = 0;
				}
				
				double F8;
				if ( R < 100 ){
					F8 = 0;
				}else if (R >= 100){
					F8 = a18 * ( R - 100 ) * T6;
				}
				
				double a22;
				if (T < 2){
					a22 = 0;
				}else if ( T >= 2){
					a22 = 0.0625 * (T -2);
				}
				
			//	double e2;
			//	if (T < 0.35 || Vs30>1000  ){
			//		e2 = 0;
			//	}else if ( T >= 0.35 && T<= 2){
			//		e2 = 0.25 * log(Vs30/1000) * log(T/0.35);
			//	}else if ( T> 2){
			//		e2 = 0.25 * log(Vs30/1000) * log(2/0.35);
			//	}
				
				double v1;
				if (T <= 0.5){
					v1 = 1500;
				}else if (T > 0.5 && T <= 1){
					v1 = exp (8.0 - 0.795 * log(T/0.21));
				}else if (T > 1 && T < 2){
					v1 = exp (6.76 - 0.279 * log(T));
				}else if (T >= 2.0 ){
					v1 = 700;
				}else if (T = -1){
					v1 = 862;
				}
				
				double Vsstar30;
				if (Vs30< v1){
					Vsstar30 = Vs30;
				}else if (Vs30 >= v1){
					Vsstar30 = v1;
				}

				double lnzb;
				if (Vs30<180){
					lnzb=6.745;
				}else if (Vs30<= 180 && Vs30<= 500){
					lnzb=6.745 - 1.35 * log(Vs30/180);
				}else if(Vs30>500){
					lnzb=5.394 - 4.48 * log(Vs30/180);
				}

			//	double F10;
			//	if (exp(lnzb) >= 200){
			//		F10 = a22 * log(exp(lnzb)/200);
			//	}else if (exp(lnzb) < 200){
			//		F10 = 0;
			//	}
			//OUTPUT
			if	(getDisplayOutput())	{	
				rDebug(threadID)	<<	"F1:"	<<	F1	<<" "	 ;
			}												
			if	(getDisplayOutput())	{				
				rDebug(threadID)	<<	"F6:"	<<	F6	<<" "	 ;
			}						
			if	(getDisplayOutput())	{				
				rDebug(threadID)	<<	"F7:"	<<	F7	<<" "	 ;
			}						
			if	(getDisplayOutput())	{
				rDebug(threadID)	<<	"F8:"	<<	F8	<<" "	 ;
			}						
		//	if	(getDisplayOutput())	{				
		//	rDebug()	<<	"F10:"	<<	F10	<<" "	 ;
		//	}						
 

				double lnY = F1 + a12 * FRV + a13 * FN + a15 * FAS + FRV * F6 + (1 - FRV) * F7 + F8 ;//+F10
				double A1100;
				{
					// For pga4nl, calculate the PGA for FS = 0.0 and epsilon = 0.0 and Rref = 5.0 km;
					double Vlin = VlinVector[1];
					double b = bVector[1];
					double a1 = a1Vector[1];
					double a2 = a2Vector[1];
					double a8 = a8Vector[1];
					double a10 = a10Vector[1];
					double a12 = a12Vector[1];
					double a13 = a13Vector[1];
					double a14 = a14Vector[1];
					double a15 = a15Vector[1];
					double a16 = a16Vector[1];
					double a17 = a17Vector[1];
					double a18 = a18Vector[1];
				Vs30 = 1100;				
				double F1;
					if ( M <= c1){
					F1 = a1 + a4 * ( M - c1 ) + a8 * ( 8.5 - M ) * ( 8.5 - M ) + ( a2 + a3 * (M - c1 ))* log(R) ;
					} else if ( M > c1){
					F1 = a1 + a5 * ( M - c1 ) + a8 * ( 8.5 - M ) * ( 8.5 - M ) + ( a2 + a3 * (M - c1 ))* log(R) ;
					}
				
				double Ztop = 11.24 - 0.2 * M;
				
				double F6;
				if ( Ztop <= 2 ){
					F6 = 0;
				}else if (Ztop > 2 && Ztop < 5){
					F6 = a16 * ( Ztop - 2 ) / (3);
				}else if (Ztop >= 5){
					F6 = a16;
				}
				
				double F7;
				if ( Ztop <= 0 ){
					F7 = 0;
				}else if (Ztop > 2 && Ztop < 10){
					F7 = a17 * ( Ztop - 2 ) / (8);
				}else if (Ztop >= 10){
					F7 = a17;
				}
				
				double T6;
				if ( M <= 5.5 ){
					T6 = 1;
				}else if (M > 5.5 && M < 6.5){
					T6 = 6.5 - M ;
				}else if ( M >6.5 ){
					T6 = 0;
				}
				
				double F8;
				if ( R < 100 ){
					F8 = 0;
				}else if (R >= 100){
					F8 = a18 * ( R - 100 ) * T6;
				}
				
				double a22;
				if (T < 2){
					a22 = 0;
				}else if ( T >= 2){
					a22 = 0.0625 * (T -2);
				}
				
			//	double e2;
			//	if (T < 0.35 || Vs30>1000  ){
			//		e2 = 0;
			//	}else if ( T >= 0.35 && T<= 2){
			//		e2 = 0.25 * log(Vs30/1000) * log(T/0.35);
			//	}else if ( T> 2){
			//		e2 = 0.25 * log(Vs30/1000) * log(2/0.35);
			//	}
				
				double v1;
				if (T <= 0.5){
					v1 = 1500;
				}else if (T > 0.5 && T <= 1){
					v1 = exp (8.0 - 0.795 * log(T/0.21));
				}else if (T > 1 && T < 2){
					v1 = exp (6.76 - 0.279 * log(T));
				}else if (T >= 2.0 ){
					v1 = 700;
				}else if (T = -1){
					v1 = 862;
				}
				
				double Vsstar30;
				if (Vs30< v1){
					Vsstar30 = Vs30;
				}else if (Vs30 >= v1){
					Vsstar30 = v1;
				}

				double lnzb;
				if (Vs30<180){
					lnzb=6.745;
				}else if (Vs30<= 180 && Vs30<= 500){
					lnzb=6.745 - 1.35 * log(Vs30/180);
				}else if(Vs30>500){
					lnzb=5.394 - 4.48 * log(Vs30/180);
				}

				double F10;
				if (exp(lnzb) >= 200){
					F10 = a22 * log(exp(lnzb)/200);
				}else if (exp(lnzb) < 200){
					F10 = 0;
				}
					
					double R = sqrt(Rjb * Rjb + c4 * c4 );
					
					A1100 = exp( F1 + a12 * FRV + a13 * FN + a15 * FAS + (a10 + b * n) * log( 1100.0 / 1500.0) + FRV * F6 + (1 - FRV) * F7 + F8 + F10);
				}

			double F5;
			
			if (theShearWaveVelocityList.count() == 1) {
				Vs30 = theShearWaveVelocityList[0].data()->getCurrentValue(threadID);
			} else {
				Vs30 = theShearWaveVelocityList[i].data()->getCurrentValue(threadID);
			}
			if (theResponseType == RAbrahamsonSilva2008IntensityModel::PGA) {
				T = 0.0;
			} else if (theResponseType == RAbrahamsonSilva2008IntensityModel::PGV) {
				T = -1.0;
			} 
			if (theResponseType == RAbrahamsonSilva2008IntensityModel::Sa) {
				T = thePeriodList[i].data()->getCurrentValue(threadID);
			}
				if (T <= 0.5){
					v1 = 1500;
				}else if (T > 0.5 && T <= 1){
					v1 = exp (8.0 - 0.795 * log(T/0.21));
				}else if (T > 1 && T < 2){
					v1 = exp (6.76 - 0.279 * log(T));
				}else if (T >= 2.0 ){
					v1 = 700;
				}else if (T = -1){
					v1 = 862;
				}
				if (Vs30< v1){
					Vsstar30 = Vs30;
				}else if (Vs30 >= v1){
					Vsstar30 = v1;
				}
				a10 = a10Vector[j];
				Vlin = VlinVector[j];
				b = bVector[j];
				 if(Vlin > Vs30){
					 F5 = a10 * log (Vsstar30/Vlin) - b * log(A1100 + c) + b * log(A1100 + c * (Vsstar30/Vlin)) ;
				 }else if(Vlin <= Vs30){
					 F5 = (a10 + b* n)* log(Vsstar30/Vlin);
				 }
				//OUTPUT
				 if	(getDisplayOutput())	{		
					rDebug(threadID)	<<	"F5:"	<<	F5	<<" "	 ;
				}

				 double sigma0;
				 double sigmab;
				 double Tub;
				 double sigma;

				 if (M<5){
					 Tub = s3;
				 }else if(M>=5 && M<=7){
					 Tub = s3+(s4-s3)/(2)*(M - 5);
				 }else if(M>7){
					 Tub = s4;
				 }

				 if (M<5){
					 sigma0 = s1;
				 }else if(M>=5 && M<=7){
					 sigma0 = s1+(s2-s1)/(2)*(M - 5);
				 }else if(M>7){
					 sigma0 = s2;
				 }

				 if(Vlin > Vs30){
					 sigmab = sqrt(sigma0*sigma0 - 0.3*0.3);
				 }else if(Vlin <= Vs30){
					 sigmab = sigma0;
				 }


				sigma = Tub * eta + sigmab * epsilon;

				if	(getDisplayOutput())	{		
					rDebug(threadID)	<<	"Sigma:"	<<	sigma	<<" "	 ;
				}

				lnY = lnY +  F5 + sigma ;
				lnYVector << lnY;

			}

			double lnY;
			if (lnYVector.count() == 1) {
				lnY = lnYVector[0];
			} else {//Is this right?////////////////////////////////////////////////////???????????????????????????????????????????????
				lnY = (lnYVector[1] - lnYVector[0]) / (1.0 / periodTableVector[upperIndex] - 1.0 / periodTableVector[lowerIndex]) * (1.0 / T - 1.0 / periodTableVector[lowerIndex]) + lnYVector[0];
			}

			double Y = exp(lnY); // The units of Y is g for Sa and PGA, and cm/s for PGV
			if	(getDisplayOutput())	{		
				rDebug(threadID)	<<	"Y:"	<<	Y	<<" "	 ;
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

bool RAbrahamsonSilva2008IntensityModel::canBeRunParallelly(){
	return true;
}