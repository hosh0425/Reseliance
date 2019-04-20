#include "RChiouYoungs2008IntensityModel.h"	

#include <QCoreApplication>
#include <QMetaEnum>
	
#include "RParameter.h"	
#include "RResponse.h"	
#include "RLocation.h"
#include "RLocationResponse.h"
#include "RMainWindow.h"

#include <math.h>

#include <gsl/gsl_math.h>
	
RChiouYoungs2008IntensityModel::RChiouYoungs2008IntensityModel(QObject *parent, QString name)	
	: RModel(parent, name)
{	
	//theEpsilon = 0;
	theResponseType = RChiouYoungs2008IntensityModel::Sa;
	theShockType = RChiouYoungs2008IntensityModel::MainShock;
}	
	
RChiouYoungs2008IntensityModel::~RChiouYoungs2008IntensityModel()	
{	
	
}	

QString RChiouYoungs2008IntensityModel::getMagnitudeList()	
{	
	return parameterListToString(theMagnitudeList);
}	

void RChiouYoungs2008IntensityModel::setMagnitudeList(QString value)	
{	
	theMagnitudeList = stringToParameterList(value);
}

QString RChiouYoungs2008IntensityModel::getHypocentreLocationList()	
{	
	return parameterListToString(theHypocentreLocationList);
}	

void RChiouYoungs2008IntensityModel::setHypocentreLocationList(QString value)	
{	
	theHypocentreLocationList = stringToParameterList(value);

	// Checking if all the passed parameters are either RLocation or RLocationResponse
	parameterListToLocationList(theHypocentreLocationList);
}

QString RChiouYoungs2008IntensityModel::getEpsilonUncertaintyList()	
{	
	return parameterListToString(theEpsilonUncertaintyList);
}	
	
void RChiouYoungs2008IntensityModel::setEpsilonUncertaintyList(QString value)	
{	
	theEpsilonUncertaintyList = stringToParameterList(value);

	createResponses();
}	

QString RChiouYoungs2008IntensityModel::getEtaUncertaintyList()	
{	
	return parameterListToString(theEtaUncertaintyList);
}	
	
void RChiouYoungs2008IntensityModel::setEtaUncertaintyList(QString value)	
{	
	theEtaUncertaintyList = stringToParameterList(value);

	createResponses();
}	


QList<RParameter *> RChiouYoungs2008IntensityModel::getPhysicalParameterList()
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

QString RChiouYoungs2008IntensityModel::getFaultTypes() const	
{
	QMetaEnum metaEnum = metaObject()->enumerator(metaObject()->indexOfEnumerator("RFaultType"));
	QString result = "";
	for (int i = 0; i < theFaultTypes.count(); i++) {
		QString temp = metaEnum.valueToKey(theFaultTypes[i]);
	    result += temp + "; ";
	}
	return result;
}	
	
void RChiouYoungs2008IntensityModel::setFaultTypes(QString value)	
{	
	QString string = value;
	QMetaEnum metaEnum = metaObject()->enumerator(metaObject()->indexOfEnumerator("RFaultType"));

	QRegExp regExp = QRegExp(QString("[ ,;:\t\n\v\r%1%2]").arg(QChar(QChar::ParagraphSeparator)).arg(QChar(QChar::LineSeparator)));
	QStringList stringList = string.split(regExp, QString::SkipEmptyParts);
	string = "";
	QList<RChiouYoungs2008IntensityModel::RFaultType> tempList;
	for (int i = 0; i < stringList.count(); i++) {
		QByteArray ba = stringList[i].toLocal8Bit();
		const char *tempChar = ba.data();
		int tempValue = metaEnum.keyToValue(tempChar);
		RChiouYoungs2008IntensityModel::RFaultType temp = RFaultType(tempValue);
		if (temp < 0) {
			rCritical() << "Error in the Model" << objectName() << ": The fault type" << stringList[i] << "is not valid";
			return;
		}
		tempList.append(temp);
	}
	theFaultTypes = tempList;
}	

RChiouYoungs2008IntensityModel::RResponseType RChiouYoungs2008IntensityModel::getResponseType() const	
{	
	return theResponseType;
}	
	
void RChiouYoungs2008IntensityModel::setResponseType(RChiouYoungs2008IntensityModel::RResponseType value)	
{	
	theResponseType = value;
}

RChiouYoungs2008IntensityModel::RShockType RChiouYoungs2008IntensityModel::getShockType() const	
{	
	return theShockType;
}	
	
void RChiouYoungs2008IntensityModel::setShockType(RChiouYoungs2008IntensityModel::RShockType value)	
{	
	theShockType = value;
}

QString RChiouYoungs2008IntensityModel::getPeriodList()	
{	
	return parameterListToString(thePeriodList);
}	
	
void RChiouYoungs2008IntensityModel::setPeriodList(QString value)	
{	
	thePeriodList = stringToParameterList(value);

	createResponses();
}	
	
QString RChiouYoungs2008IntensityModel::getStructureLocationList()	
{	
	return parameterListToString(theStructureLocationList);
}	
	
void RChiouYoungs2008IntensityModel::setStructureLocationList(QString value)	
{	
	theStructureLocationList = stringToParameterList(value);

	// Checking if all the passed parameters are either RLocation or RLocationResponse
	parameterListToLocationList(theStructureLocationList);

	createResponses();
}	
	
QString RChiouYoungs2008IntensityModel::getShearWaveVelocityList()	
{	
	return parameterListToString(theShearWaveVelocityList);
}	
	
void RChiouYoungs2008IntensityModel::setShearWaveVelocityList(QString value)	
{	
	theShearWaveVelocityList = stringToParameterList(value);

	createResponses();
}	

void RChiouYoungs2008IntensityModel::createResponses()
{
	// Removing old responses
	while (theSpectralResponseList.count() > 0) {
		delete theSpectralResponseList.takeAt(0);
	}

	if (theStructureLocationList.isEmpty() || theShearWaveVelocityList.isEmpty()) {
		return;
	}

	if ((theStructureLocationList.count() != thePeriodList.count() && theResponseType == RChiouYoungs2008IntensityModel::Sa) || (theStructureLocationList.count() != theShearWaveVelocityList.count() && theShearWaveVelocityList.count() > 1) || (theStructureLocationList.count() != theEpsilonUncertaintyList.count() && theEpsilonUncertaintyList.count() > 1) || (theStructureLocationList.count() != theEtaUncertaintyList.count() && theEtaUncertaintyList.count() > 1)) {
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

int RChiouYoungs2008IntensityModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
{	
	if (theStructureLocationList.isEmpty()) {
		rCritical(threadID) << "Error: There should be at least one structure location in the model" << objectName() << ".";
		return -1;
	}
	if ((theStructureLocationList.count() != thePeriodList.count() && theResponseType == RChiouYoungs2008IntensityModel::Sa) || (theStructureLocationList.count() != theShearWaveVelocityList.count() && theShearWaveVelocityList.count() > 1)) {
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
	double c2 = 1.06;
	double c3 = 3.45;
	double c4 = -2.1;
	double c4a = -0.5;
	double crb = 50;
	double chm = 3;
	double cg3 = 4;
	double FAS = 0;

	if (theShockType == RChiouYoungs2008IntensityModel::AfterShock) {
		FAS = 1.0;
	} else if (theShockType == RChiouYoungs2008IntensityModel::MainShock) {
		FAS = 0.0;
	}

	// Period dependent coefficients
	QVector<double> periodTableVector;
	QVector<double> c1Vector;
	QVector<double> c1aVector;
	QVector<double> c1bVector;
	QVector<double> cnVector;
	QVector<double> cmVector;
	QVector<double> c5Vector;
	QVector<double> c6Vector;
	QVector<double> c7Vector;
	QVector<double> c7aVector;
	QVector<double> c9Vector;
	QVector<double> c9aVector;
	QVector<double> c10Vector;
	QVector<double> cg1Vector;
	QVector<double> cg2Vector;
	QVector<double> phi1Vector;
	QVector<double> phi2Vector;
	QVector<double> phi3Vector;
	QVector<double> phi4Vector;
	QVector<double> phi5Vector;
	QVector<double> phi6Vector;
	QVector<double> phi7Vector;
	QVector<double> phi8Vector;
	QVector<double> tau1Vector;
	QVector<double> tau2Vector;
	QVector<double> sig1Vector;
	QVector<double> sig2Vector;
	QVector<double> sig3Vector;
	QVector<double> sig4Vector;

	periodTableVector << -1.00 << 0.00 << 0.01 << 0.02 << 0.03 << 0.04 << 0.05 << .075 <<  0.1 << 0.15 <<  0.2 << 0.25 <<  0.3 <<  0.4 <<  0.5 << 0.75 <<  1 << 1.5 <<  2 <<  3 <<  4 <<  5 << 7.5 << 10 ;
	c1Vector  << 2.2884  << -1.2687  << -1.2687  << -1.2515  << -1.1744  << -1.0671  << -0.9464  << -0.7051  << -0.5747  << -0.5309  << -0.6352  << -0.7766  << -0.9278  << -1.2176  << -1.4695  << -1.9278  << -2.2453  << -2.7307  << -3.1413  << -3.7413  << -4.1814  << -4.5187  << -5.1224  << -5.5872 ;
	c1aVector  << 0.1094  << 0.1  << 0.1  << 0.1  << 0.1  << 0.1  << 0.1  << 0.1  << 0.1  << 0.1  << 0.1  << 0.1  << 0.0999  << 0.0997  << 0.0991  << 0.0936  << 0.0766  << 0.0022  << -0.0591  << -0.0931  << -0.0982  << -0.0994  << -0.0999  << -0.1 ;
	c1bVector  << -0.0626  << -0.255  << -0.255  << -0.255  << -0.255  << -0.255  << -0.255  << -0.254  << -0.253  << -0.25  << -0.2449  << -0.2382  << -0.2313  << -0.2146  << -0.1972  << -0.162  << -0.14  << -0.1184  << -0.11  << -0.104  << -0.102  << -0.101  << -0.101  << -0.1 ;
	cnVector  << 1.648  << 2.996  << 2.996  << 3.292  << 3.514  << 3.563  << 3.547  << 3.448  << 3.312  << 3.044  << 2.831  << 2.658  << 2.505  << 2.261  << 2.087  << 1.812  << 1.648  << 1.511  << 1.47  << 1.456  << 1.465  << 1.478  << 1.498  << 1.502 ;
	cmVector  << 4.2979  << 4.184  << 4.184  << 4.1879  << 4.1556  << 4.1226  << 4.1011  << 4.086  << 4.103  << 4.1717  << 4.2476  << 4.3184  << 4.3844  << 4.4979  << 4.5881  << 4.7571  << 4.882  << 5.0697  << 5.2173  << 5.4385  << 5.5977  << 5.7276  << 5.9891  << 6.193 ;
	c5Vector  << 5.17  << 6.16  << 6.16  << 6.158  << 6.155  << 6.1508  << 6.1441  << 6.12  << 6.085  << 5.9871  << 5.8699  << 5.7547  << 5.6527  << 5.4997  << 5.4029  << 5.29  << 5.248  << 5.2194  << 5.2099  << 5.204  << 5.202  << 5.201  << 5.2  << 5.2 ;
	c6Vector  << 0.4407  << 0.4893  << 0.4893  << 0.4892  << 0.489  << 0.4888  << 0.4884  << 0.4872  << 0.4854  << 0.4808  << 0.4755  << 0.4706  << 0.4665  << 0.4607  << 0.4571  << 0.4531  << 0.4517  << 0.4507  << 0.4504  << 0.4501  << 0.4501  << 0.45  << 0.45  << 0.45 ;
	c7Vector  << 0.0207  << 0.0512  << 0.0512  << 0.0512  << 0.0511  << 0.0508  << 0.0504  << 0.0495  << 0.0489  << 0.0479  << 0.0471  << 0.0464  << 0.0458  << 0.0445  << 0.0429  << 0.0387  << 0.035  << 0.028  << 0.0213  << 0.0106  << 0.0041  << 0.001  << 0  << 0 ;
	c7aVector  << 0.0437  << 0.086  << 0.086  << 0.086  << 0.086  << 0.086  << 0.086  << 0.086  << 0.086  << 0.086  << 0.086  << 0.086  << 0.086  << 0.085  << 0.083  << 0.069  << 0.045  << 0.0134  << 0.004  << 0.001  << 0  << 0  << 0  << 0 ;
	c9Vector  << 0.3079  << 0.79  << 0.79  << 0.8129  << 0.8439  << 0.874  << 0.8996  << 0.9442  << 0.9677  << 0.966  << 0.9334  << 0.8946  << 0.859  << 0.8019  << 0.7578  << 0.6788  << 0.6196  << 0.5101  << 0.3917  << 0.1244  << 0.0086  << 0  << 0  << 0 ;
	c9aVector  << 2.669  << 1.5005  << 1.5005  << 1.5028  << 1.5071  << 1.5138  << 1.523  << 1.5597  << 1.6104  << 1.7549  << 1.9157  << 2.0709  << 2.2005  << 2.3886  << 2.5  << 2.6224  << 2.669  << 2.6985  << 2.7085  << 2.7145  << 2.7164  << 2.7172  << 2.7177  << 2.718 ;
	c10Vector  << -0.1166  << -0.3218  << -0.3218  << -0.3323  << -0.3394  << -0.3453  << -0.3502  << -0.3579  << -0.3604  << -0.3565  << -0.347  << -0.3379  << -0.3314  << -0.3256  << -0.3189  << -0.2702  << -0.2059  << -0.0852  << 0.016  << 0.1876  << 0.3378  << 0.4579  << 0.7514  << 1.1856 ;
	cg1Vector  << -0.00275  << -0.00804  << -0.00804  << -0.00811  << -0.00839  << -0.00875  << -0.00912  << -0.00973  << -0.00975  << -0.00883  << -0.00778  << -0.00688  << -0.00612  << -0.00498  << -0.0042  << -0.00308  << -0.00246  << -0.0018  << -0.00147  << -0.00117  << -0.00107  << -0.00102  << -0.00096  << -0.00094 ;
	cg2Vector  << -0.00625  << -0.00785  << -0.00785  << -0.00792  << -0.00819  << -0.00855  << -0.00891  << -0.0095  << -0.00952  << -0.00862  << -0.00759  << -0.00671  << -0.00598  << -0.00486  << -0.0041  << -0.00301  << -0.00241  << -0.00176  << -0.00143  << -0.00115  << -0.00104  << -0.00099  << -0.00094  << -0.00091 ;
	phi1Vector  << -0.7861  << -0.4417  << -0.4417  << -0.434  << -0.4177  << -0.4  << -0.3903  << -0.404  << -0.4423  << -0.5162  << -0.5697  << -0.6109  << -0.6444  << -0.6931  << -0.7246  << -0.7708  << -0.799  << -0.8382  << -0.8663  << -0.9032  << -0.9231  << -0.9222  << -0.8346  << -0.7332 ;
	phi2Vector  << -0.0699  << -0.1417  << -0.1417  << -0.1364  << -0.1403  << -0.1591  << -0.1862  << -0.2538  << -0.2943  << -0.3113  << -0.2927  << -0.2662  << -0.2405  << -0.1975  << -0.1633  << -0.1028  << -0.0699  << -0.0425  << -0.0302  << -0.0129  << -0.0016  << 0  << 0  << 0 ;
	phi3Vector  << -0.008444  << -0.00701  << -0.00701  << -0.007279  << -0.007354  << -0.006977  << -0.006467  << -0.005734  << -0.005604  << -0.005845  << -0.006141  << -0.006439  << -0.006704  << -0.007125  << -0.007435  << -0.00812  << -0.008444  << -0.007707  << -0.004792  << -0.001828  << -0.001523  << -0.00144  << -0.001369  << -0.001361 ;
	phi4Vector  << 5.41  << 0.102151  << 0.102151  << 0.10836  << 0.119888  << 0.133641  << 0.148927  << 0.190596  << 0.230662  << 0.266468  << 0.255253  << 0.231541  << 0.207277  << 0.165464  << 0.133828  << 0.085153  << 0.058595  << 0.031787  << 0.019716  << 0.009643  << 0.005379  << 0.003223  << 0.001134  << 0.000515 ;
	phi5Vector  << 0.2899  << 0.2289  << 0.2289  << 0.2289  << 0.2289  << 0.2289  << 0.229  << 0.2292  << 0.2297  << 0.2326  << 0.2386  << 0.2497  << 0.2674  << 0.312  << 0.361  << 0.4353  << 0.4629  << 0.4756  << 0.4785  << 0.4796  << 0.4799  << 0.4799  << 0.48  << 0.48 ;
	phi6Vector  << 0.006718  << 0.014996  << 0.014996  << 0.014996  << 0.014996  << 0.014996  << 0.014996  << 0.014996  << 0.014996  << 0.014988  << 0.014964  << 0.014881  << 0.014639  << 0.013493  << 0.011133  << 0.006739  << 0.005749  << 0.005544  << 0.005521  << 0.005517  << 0.005517  << 0.005517  << 0.005517  << 0.005517 ;
	phi7Vector  << 459  << 580  << 580  << 580  << 580  << 579.9  << 579.9  << 579.6  << 579.2  << 577.2  << 573.9  << 568.5  << 560.5  << 540  << 512.9  << 441.9  << 391.8  << 348.1  << 332.5  << 324.1  << 321.7  << 320.9  << 320.3  << 320.1 ;
	phi8Vector  << 0.1138  << 0.07  << 0.07  << 0.0699  << 0.0701  << 0.0702  << 0.0701  << 0.0686  << 0.0646  << 0.0494  << -0.0019  << -0.0479  << -0.0756  << -0.096  << -0.0998  << -0.0765  << -0.0412  << 0.014  << 0.0544  << 0.1232  << 0.1859  << 0.2295  << 0.266  << 0.2682 ;
	tau1Vector  << 0.2539  << 0.3437  << 0.3437  << 0.3471  << 0.3603  << 0.3718  << 0.3848  << 0.3878  << 0.3835  << 0.3719  << 0.3601  << 0.3522  << 0.3438  << 0.3351  << 0.3353  << 0.3429  << 0.3577  << 0.3769  << 0.4023  << 0.4406  << 0.4784  << 0.5074  << 0.5328  << 0.5542 ;
	tau2Vector  << 0.2381  << 0.2637  << 0.2637  << 0.2671  << 0.2803  << 0.2918  << 0.3048  << 0.3129  << 0.3152  << 0.3128  << 0.3076  << 0.3047  << 0.3005  << 0.2984  << 0.3036  << 0.3205  << 0.3419  << 0.3703  << 0.4023  << 0.4406  << 0.4784  << 0.5074  << 0.5328  << 0.5542 ;
	sig1Vector  << 0.4496  << 0.4458  << 0.4458  << 0.4458  << 0.4535  << 0.4589  << 0.463  << 0.4702  << 0.4747  << 0.4798  << 0.4816  << 0.4815  << 0.4801  << 0.4758  << 0.471  << 0.4621  << 0.4581  << 0.4493  << 0.4459  << 0.4433  << 0.4424  << 0.442  << 0.4416  << 0.4414 ;
	sig2Vector  << 0.3554  << 0.3459  << 0.3459  << 0.3459  << 0.3537  << 0.3592  << 0.3635  << 0.3713  << 0.3769  << 0.3847  << 0.3902  << 0.3946  << 0.3981  << 0.4036  << 0.4079  << 0.4157  << 0.4213  << 0.4213  << 0.4213  << 0.4213  << 0.4213  << 0.4213  << 0.4213  << 0.4213 ;
	sig3Vector  << 0.7504  << 0.8  << 0.8  << 0.8  << 0.8  << 0.8  << 0.8  << 0.8  << 0.8  << 0.8  << 0.8  << 0.7999  << 0.7997  << 0.7988  << 0.7966  << 0.7792  << 0.7504  << 0.7136  << 0.7035  << 0.7006  << 0.7001  << 0.7  << 0.7  << 0.7 ;
	sig4Vector  << 0.0133  << 0.0663  << 0.0663  << 0.0663  << 0.0663  << 0.0663  << 0.0663  << 0.0663  << 0.0663  << 0.0612  << 0.053  << 0.0457  << 0.0398  << 0.0312  << 0.0255  << 0.0175  << 0.0133  << 0.009  << 0.0068  << 0.0045  << 0.0034  << 0.0027  << 0.0018  << 0.0014 ;

	
	double T;
	if (theResponseType == RChiouYoungs2008IntensityModel::PGA) {
		T = 0.0;
	} else if (theResponseType == RChiouYoungs2008IntensityModel::PGV) {
		T = -1.0;
	} 


	QVector<double> spectralResponseVector;
	for (int i = 0; i < locationList.count(); i++) {
		spectralResponseVector << 0.0;
	}

	for (int index = 0; index < theMagnitudeList.count(); index++) {
		double FN = 0 ;
		double FR = 0;
		
		RChiouYoungs2008IntensityModel::RFaultType theFaultType;
		if (theFaultTypes.count() == 1) {
			theFaultType = theFaultTypes[0];
		} else {
			theFaultType = theFaultTypes[index];
		}

		 if (theFaultType == RChiouYoungs2008IntensityModel::StrikeSlip) {
			FR = 0;
			FN = 0;
		} else if (theFaultType == RChiouYoungs2008IntensityModel::NormalSlip) {
			FN = 1.0;
		} else if (theFaultType == RChiouYoungs2008IntensityModel::ReverseSlip) {
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

			if (theResponseType == RChiouYoungs2008IntensityModel::Sa) {
				T = thePeriodList[i].data()->getCurrentValue(threadID);
			}
			if (T > periodTableVector.last() || T < periodTableVector.first()) {
				// RESTORE THE WARNING WHEN THE PAPER IS DONE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
				//rCritical() << "Warning: The period" << T << "from the parameter" << thePeriodList[i]->objectName() << "entered in the model" << objectName() << "is out of the range of" << periodTableVector[1] << "and" << periodTableVector.last() << "sec.";
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
				double c1 = c1Vector[j];
				double c1a = c1aVector[j];
				double c1b = c1bVector[j];
				double cn = cnVector[j];
				double cm = cmVector[j];
				double c5 = c5Vector[j];
				double c6 = c6Vector[j];
				double c7 = c7Vector[j];
				double c7a = c7aVector[j];
				double c9 = c9Vector[j];
				double c9a = c9aVector[j];
				double c10 = c10Vector[j];
				double cg1 = cg1Vector[j];
				double cg2 = cg2Vector[j];
				double phi1 = phi1Vector[j];
				double phi2 = phi2Vector[j];
				double phi3 = phi3Vector[j];
				double phi4 = phi4Vector[j];
				double phi5 = phi5Vector[j];
				double phi6 = phi6Vector[j];
				double phi7 = phi7Vector[j];
				double phi8 = phi8Vector[j];
				double tau1 = tau1Vector[j];
				double tau2 = tau2Vector[j];
				double sig1 = sig1Vector[j];
				double sig2 = sig2Vector[j];
				double sig3 = sig3Vector[j];
				double sig4 = sig4Vector[j];
				

				double Ztop ;
				if ( FN==0 && FR==0  ){
					Ztop = 5.63 + 0.68 * M ;
				}else if ( FN==1 || FR==1 ){
					Ztop = 11.24 - 0.2 * M ;
				}
				double Z1 = 28.5 - (3.82/8) * log(pow(Vs30,8) + pow(378.7,8) );
				double R = sqrt(Rjb * Rjb + Ztop * Ztop );
			
				double lnY = c1 + (c1a * FR + c1b * FN + c7 * (Ztop - 4 )) * ( 1 - FAS) ;
				if	(getDisplayOutput())	{				
					rDebug(threadID) <<	"LnY:"	<<	lnY <<" "	 ;
				}
				lnY = lnY + (c10 + c7a*(Ztop - 4))* FAS ;
				if	(getDisplayOutput())	{				
					rDebug(threadID)	<<	"LnY:"	<<	lnY <<" "	 ;
				}
				lnY = lnY + c2 * (M -6) + ((c2 - c3)/cn ) * log(1+exp(cn*(cm - M))) ;
				if	(getDisplayOutput())	{				
					rDebug(threadID)	<<	"LnY:"	<<	lnY <<" "	 ;
				}
				lnY = lnY + c4 * log( R + c5 * cosh(c6 * gsl_max(M-chm ,0)));
				if	(getDisplayOutput())	{				
					rDebug(threadID)	<<	"LnY:"	<<	lnY <<" "	 ;
				}
				lnY = lnY + (c4a - c4 ) * log(sqrt( R * R + crb * crb)); 
				if	(getDisplayOutput())	{				
					rDebug(threadID)	<<	"LnY:"	<<	lnY <<" "	 ;
				}
				lnY = lnY + (cg1 + ((cg2)/(cosh(gsl_max(M - cg3,0))))) * R ;
				if	(getDisplayOutput())	{				
					rDebug(threadID)	<<	"LnY:"	<<	lnY <<" "	 ;
				}

				lnY = lnY + phi1 * gsl_min (log(Vs30/1130),0)+ phi2 * (exp( phi3 * (gsl_min(Vs30,1130)-360))-exp( phi3 * (1130 - 360) )) * log((exp(lnY)+phi4)/phi4) ;
				if	(getDisplayOutput())	{				
					rDebug(threadID)	<<	"LnY:"	<<	lnY <<" "	 ;
				}
				lnY = lnY + phi5 *(1-(1/(cosh(phi6 * gsl_max(0, Z1 - phi7))))) + phi8 /(cosh(0.15* gsl_max(0, Z1 - 15)));
				if	(getDisplayOutput())	{				
					rDebug(threadID)	<<	"LnY:"	<<	lnY <<" "	 ;
				}
				double b;
				b = phi2 * (exp( phi3 * (gsl_min(Vs30,1130)-360))-exp( phi3 * (1130 - 360) ));
				
				double sigma ;
				double NL0 = b * ((exp(lnY))/(lnY + phi4 ))  ;
				double sNL0 = (sig1+((sig2-sig1)/2)*(gsl_min(gsl_max(M,5),7)-5)+sig4 * FAS)* sqrt(0.7 +(1+NL0)*(1+NL0))  ;
				double Tau =tau1+((tau2-tau1)/2)*(gsl_min(gsl_max(M,5),7)-5) ;

				sigma = eta * Tau * (1+NL0) + epsilon * sNL0;
				
				if	(getDisplayOutput())	{				
					rDebug(threadID)	<<	"sigma :"	<<	sigma <<" "	 ;
				}

				lnY = lnY + sigma   ;
				if	(getDisplayOutput())	{				
					rDebug(threadID)	<<	"LnY:"	<<	lnY <<" "	 ;
				}
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

bool RChiouYoungs2008IntensityModel::canBeRunParallelly(){
	return true;
}