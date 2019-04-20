#include "RFemaNibsBridgeDamageModel.h"

#include "RParameter.h"
#include "RResponse.h"
#include "RContinuousRandomVariable.h"

#include <math.h>

#include <gsl/gsl_math.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_cdf.h>

RFemaNibsBridgeDamageModel::RFemaNibsBridgeDamageModel(QObject *parent, QString name)
	: RModel(parent, name)
{
	// Initializing the pointer data members
	theAnalysisType = RFemaNibsBridgeDamageModel::GroundShaking;
	theBridgeType = RFemaNibsBridgeDamageModel::HWB1;
	//$$ theSa1 = 0;
	//$$ theSa2 = 0;
	theNumberOfSpans = 1;
	theAngleOfSkew = 0;
	//$$ thePGD = 0;
	theBridgeLength = 100;
	theSpanWidth = 10;
	//$$ theModelUncertainty = 0;
	
	// Instantiating the output response
	theDomain->createObject(objectName() + "PercentFunctionalResponse", "RGenericResponse");
	thePercentFunctional = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	thePercentFunctional->setModel(this);

	theDomain->createObject(objectName() + "DamageFactorResponse", "RGenericResponse");
	theDamageFactor = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theDamageFactor->setModel(this);
}

RFemaNibsBridgeDamageModel::~RFemaNibsBridgeDamageModel()
{

}

RFemaNibsBridgeDamageModel::RAnalysisType RFemaNibsBridgeDamageModel::getAnalysisType() const	
{	
	return theAnalysisType;
}	
	
void RFemaNibsBridgeDamageModel::setAnalysisType(RFemaNibsBridgeDamageModel::RAnalysisType value)	
{	
	theAnalysisType = value;
}	
	
RFemaNibsBridgeDamageModel::RBridgeType RFemaNibsBridgeDamageModel::getBridgeType() const	
{	
	return theBridgeType;
}	
	
void RFemaNibsBridgeDamageModel::setBridgeType(RFemaNibsBridgeDamageModel::RBridgeType value)	
{	
	theBridgeType = value;
}	
	
QObject * RFemaNibsBridgeDamageModel::getSa1() const	
{	
	return theSa1.data();
}	
	
void RFemaNibsBridgeDamageModel::setSa1(QObject *value)	
{	
	theSa1 = qobject_cast<RParameter *>(value);
}	
	
QObject * RFemaNibsBridgeDamageModel::getSa2() const	
{	
	return theSa2.data();
}	
	
void RFemaNibsBridgeDamageModel::setSa2(QObject *value)	
{	
	theSa2 = qobject_cast<RParameter *>(value);
}

QObject * RFemaNibsBridgeDamageModel::getTime() const	
{	
	return theTime.data();
}	
	
void RFemaNibsBridgeDamageModel::setTime(QObject *value)	
{	
	theTime = qobject_cast<RParameter *>(value);
}	
	
int RFemaNibsBridgeDamageModel::getNumberOfSpans() const	
{	
	return theNumberOfSpans;
}	
	
void RFemaNibsBridgeDamageModel::setNumberOfSpans(int value)	
{	
	theNumberOfSpans = value;
}	
	
double RFemaNibsBridgeDamageModel::getAngleOfSkew() const	
{	
	return theAngleOfSkew;
}	
	
void RFemaNibsBridgeDamageModel::setAngleOfSkew(double value)	
{	
	theAngleOfSkew = value;
}	
	
QObject * RFemaNibsBridgeDamageModel::getPGD() const	
{	
	return thePGD.data();
}	
	
void RFemaNibsBridgeDamageModel::setPGD(QObject *value)	
{	
	thePGD = qobject_cast<RParameter *>(value);
}	
	
double RFemaNibsBridgeDamageModel::getBridgeLength() const	
{	
	return theBridgeLength;
}	
	
void RFemaNibsBridgeDamageModel::setBridgeLength(double value)	
{	
	theBridgeLength = value;
}	
	
double RFemaNibsBridgeDamageModel::getSpanWidth() const	
{	
	return theSpanWidth;
}	
	
void RFemaNibsBridgeDamageModel::setSpanWidth(double value)	
{	
	theSpanWidth = value;
}	
	
QObject * RFemaNibsBridgeDamageModel::getModelUncertainty() const	
{	
	return theModelUncertainty.data();
}	
	
void RFemaNibsBridgeDamageModel::setModelUncertainty(QObject *value)	
{	
	theModelUncertainty = qobject_cast<RParameter *>(value);

	// Check that the user has given a uniform random variable between 0 and 1:
	RContinuousRandomVariable *theRV = qobject_cast<RContinuousRandomVariable *>(value);  
	if (!theRV) {
		rCritical() << "Warning: The parameter given to the" << objectName() << "as the uncertainty must be a continuous standard normal random variable.";
		return;
	}

	QString type = theRV->getDistributionType();
	double mean = theRV->getParameter1Value(0);
	double stdv = theRV->getParameter2Value(0);
	if ((type != QString("Normal (mean, stdv)")) || (mean != 0.0) || (stdv != 1.0)) {
		rCritical() << "Warning: The random variable given to the" << objectName() << "as the uncertainty must be a standard normal random variable.";
	}
}	

int RFemaNibsBridgeDamageModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
{	
	double Sa1 = theSa1.data()->getCurrentValue(threadID);
	double Sa0_3 = theSa2.data()->getCurrentValue(threadID);
	double PGD = thePGD.data()->getCurrentValue(threadID);
	double time = theTime.data()->getCurrentValue(threadID);

	double A;
	int B;
	int IShape;
	double betaSa = 0.6;
	double betaPGD = 0.2;
	QVector<double> SaVector;
	QVector<double> PGDVector;
	double f1;
	double f2;
	double fSine = sin(M_PI / 180.0 * (theAngleOfSkew));
	double fLong = 0.5 * theBridgeLength / (theNumberOfSpans * theSpanWidth * sin(M_PI / 180.0 * (theAngleOfSkew)));

	// These data are f,rom the tables in chapter 7 of the FEMA-NIBS document
	switch (theBridgeType){

	case RFemaNibsBridgeDamageModel::HWB1 : {
		A = 0.25;
		B = 1;
		IShape = 0;

		SaVector << 0.4 << 0.5 << 0.7 << 0.9;
		PGDVector << 3.9 << 3.9 << 3.9 << 13.8;

		f1 = 1.0; // fSine  // fLong
		f2 = 1.0;

		break;
	} 

	case  RFemaNibsBridgeDamageModel::HWB2 : {
		
		A = 0.25;
		B = 1;
		IShape = 0;

		SaVector << 0.6 << 0.9 << 1.1 << 1.7;
		PGDVector << 3.9 << 3.9 << 3.9 << 13.8;

		f1 = 1.0;
		f2 = 1.0;

		break;
	} 

	case RFemaNibsBridgeDamageModel::HWB3 : {
		A = 0.25;
		B = 1;
		IShape = 1;

		SaVector << 0.8 << 1.0 << 1.2 << 1.7;
		PGDVector << 3.9 << 3.9 << 3.9 << 13.8;

		f1 = 1.0;
		f2 = 1.0;

		break;
	} 

	case RFemaNibsBridgeDamageModel::HWB4 : {
		A = 0.25;
		B = 1;
		IShape = 1;

		SaVector << 0.8 << 1.0 << 1.2 << 1.7;
		PGDVector << 3.9 << 3.9 << 3.9 << 13.8;

		f1 = 1.0;
		f2 = 1.0;

		break;
	} 
											 
	case RFemaNibsBridgeDamageModel::HWB5 : {
		A = 0.25;
		B = 1;
		IShape = 0;

		SaVector << 0.25 << 0.35 << 0.45 << 0.70;
		PGDVector << 3.9 << 3.9 << 3.9 << 13.8;

		f1 = fLong;
		f2 = fLong;

		break;
	} 
											 
	case RFemaNibsBridgeDamageModel::HWB6 : {
		A = 0.25;
		B = 1;
		IShape = 0;

		SaVector << 0.3 << 0.5 << 0.6 << 0.9;
		PGDVector << 3.9 << 3.9 << 3.9 << 13.8;

		f1 = fLong;
		f2 = fLong;

		break;
	} 
											 
	case RFemaNibsBridgeDamageModel::HWB7 : {
		A = 0.25;
		B = 1;
		IShape = 0;

		SaVector << 0.5 << 0.8 << 1.1 << 1.7 ;
		PGDVector << 3.9 << 3.9 << 3.9 << 13.8;

		f1 = fLong;
		f2 = fLong;

		break;
	} 
											 
	case RFemaNibsBridgeDamageModel::HWB8 : {
		A = 0.33;
		B = 0;
		IShape = 0;

		SaVector << 0.35 << 0.45 << 0.55 << 0.8 ;
		PGDVector << 3.9 << 3.9 << 3.9 << 13.8;

		f1 = 1.0;
		f2 = fSine;

		break;
	} 
											 
	case RFemaNibsBridgeDamageModel::HWB9 : {
		A = 0.33;
		B = 1;
		IShape = 0;

		SaVector << 0.6 << 0.9 << 1.3 << 1.6 ;
		PGDVector << 3.9 << 3.9 << 3.9 << 13.8;

		f1 = 1.0;
		f2 = fSine;

		break;
	} 
											 
	case RFemaNibsBridgeDamageModel::HWB10 : {
		A = 0.33;
		B = 0;
		IShape = 1;

		SaVector << 0.6 << 0.9 << 1.1 << 1.5 ;
		PGDVector << 3.9 << 3.9 << 3.9 << 13.8;

		f1 = 1.0;
		f2 = fSine;

		break;
	} 
											  
	case RFemaNibsBridgeDamageModel::HWB11 : {
		A = 0.33;
		B = 1;
		IShape = 1;

		SaVector << 0.9 << 0.9 << 1.1 << 1.5 ;
		PGDVector << 3.9 << 3.9 << 3.9 << 13.8;

		f1 = 1.0;
		f2 = fSine;

		break;
	} 
											  
	case RFemaNibsBridgeDamageModel::HWB12 : {
		A = 0.09;
		B = 1;
		IShape = 0;

		SaVector << 0.25 << 0.35 << 0.45 << 0.7 ;
		PGDVector << 3.9 << 3.9 << 3.9 << 13.8;

		f1 = fLong;
		f2 = fLong;

		break;
	} 
											  
	case RFemaNibsBridgeDamageModel::HWB13 : {
		A = 0.09;
		B = 1;
		IShape = 0;

		SaVector << 0.3 << 0.5 << 0.6 << 0.9 ;
		PGDVector << 3.9 << 3.9 << 3.9 << 13.8;

		f1 = fLong;
		f2 = fLong;

		break;
	} 
											 
	case RFemaNibsBridgeDamageModel::HWB14 : {
		A = 0.25;
		B = 1;
		IShape = 0;

		SaVector << 0.5 << 0.8 << 1.1 << 1.7 ;
		PGDVector << 3.9 << 3.9 << 3.9 << 13.8;

		f1 = fLong;
		f2 = fLong;

		break;
	} 
											 
	case RFemaNibsBridgeDamageModel::HWB15 : {
		A = 0.05;
		B = 0;
		IShape = 1;

		SaVector << 0.75 << 0.75 << 0.75 << 1.1 ;
		PGDVector << 3.9 << 3.9 << 3.9 << 13.8;

		f1 = 1.0;
		f2 = fSine;

		break;
	} 
											 
	case RFemaNibsBridgeDamageModel::HWB16 : {
		A = 0.33;
		B = 1;
		IShape = 1;

		SaVector << 0.9 << 0.9 << 1.1 << 1.5 ;
		PGDVector << 3.9 << 3.9 << 3.9 << 13.8;

		f1 = 1.0;
		f2 = fSine;

		break;
	} 
											 
	case RFemaNibsBridgeDamageModel::HWB17 : {
		A = 0.25;
		B = 1;
		IShape = 0;

		SaVector << 0.25 << 0.35 << 0.45 << 0.7 ;
		PGDVector << 3.9 << 3.9 << 3.9 << 13.8;

		f1 = fLong;
		f2 = fLong;

		break;
	} 
											 
	case RFemaNibsBridgeDamageModel::HWB18 : {
		A = 0.25;
		B = 1;
		IShape = 0;

		SaVector << 0.3 << 0.5 << 0.6 << 0.9 ;
		PGDVector << 3.9 << 3.9 << 3.9 << 13.8;

		f1 = fLong;
		f2 = fLong;

		break;
	} 
											 
	case RFemaNibsBridgeDamageModel::HWB19 : {
		A = 0.25;
		B = 1;
		IShape = 0;

		SaVector << 0.5 << 0.8 << 1.1 << 1.7 ;
		PGDVector << 3.9 << 3.9 << 3.9 << 13.8;

		f1 = fLong;
		f2 = fLong;

		break;
	} 
											 
	case RFemaNibsBridgeDamageModel::HWB20 : {
		A = 0.33;
		B = 0;
		IShape = 0;

		SaVector << 0.35 << 0.45 << 0.55 << 0.8 ;
		PGDVector << 3.9 << 3.9 << 3.9 << 13.8;

		f1 = 1.0;
		f2 = fSine;

		break;
	}
											 
	case RFemaNibsBridgeDamageModel::HWB21 : {
		A = 0.33;
		B = 1;
		IShape = 0;

		SaVector << 0.6 << 0.9 << 1.3 << 1.6 ;
		PGDVector << 3.9 << 3.9 << 3.9 << 13.8;

		f1 = 1.0;
		f2 = fSine;

		break;
	} 
											 
	case RFemaNibsBridgeDamageModel::HWB22 : {
		A = 0.33;
		B = 0;
		IShape = 1;

		SaVector << 0.6 << 0.9 << 1.1 << 1.5 ;
		PGDVector << 3.9 << 3.9 << 3.9 << 13.8;

		f1 = fLong;
		f2 = fLong;

		break;
	} 
											 
	case RFemaNibsBridgeDamageModel::HWB23 : {
		A = 0.33;
		B = 1;
		IShape = 1;

		SaVector << 0.9 << 0.9 << 1.1 << 1.5 ;
		PGDVector << 3.9 << 3.9 << 3.9 << 13.8;

		f1 = fLong;
		f2 = fLong;

		break;
	} 
											 
	case RFemaNibsBridgeDamageModel::HWB24 : {
		A = 0.2;
		B = 1;
		IShape = 0;

		SaVector << 0.25 << 0.35 << 0.45 << 0.7 ;
		PGDVector << 3.9 << 3.9 << 3.9 << 13.8;

		f1 = fLong;
		f2 = fLong;

		break;
	} 
											 
	case RFemaNibsBridgeDamageModel::HWB25 : {
		A = 0.2;
		B = 1;
		IShape = 0;

		SaVector << 0.3 << 0.5 << 0.6 << 0.9 ;
		PGDVector << 3.9 << 3.9 << 3.9 << 13.8;

		f1 = fLong;
		f2 = fLong;

		break;
	} 
											 
	case RFemaNibsBridgeDamageModel::HWB26 : {
		A = 0.1;
		B = 0;
		IShape = 1;

		SaVector << 0.75 << 0.75 << 0.75 << 1.1 ;
		PGDVector << 3.9 << 3.9 << 3.9 << 13.8;

		f1 = 1.0;
		f2 = fSine;

		break;
	}
											 
	case RFemaNibsBridgeDamageModel::HWB27 : {
		A = 0.1;
		B = 0;
		IShape = 1;

		SaVector << 0.75 << 0.75 << 0.75 << 1.1 ;
		PGDVector << 3.9 << 3.9 << 3.9 << 13.8;

		f1 = 1.0;
		f2 = fSine;

		break;
	} 
											 
	case RFemaNibsBridgeDamageModel::HWB28 : {
		// Attention: A, B, and IShape for this bridge type are not given in the FEMA-NIBS document
		A = 0.1;
		B = 0;
		IShape = 1;

		SaVector << 0.8 << 1.0 << 1.2 << 1.7 ;
		PGDVector << 3.9 << 3.9 << 3.9 << 13.8;

		f1 = 1.0;
		f2 = 1.0;

		break;
	}
    }	 


	// Calculating the modifiers
	double KSkew = sqrt(sin(M_PI / 180.0 * (90.0 - theAngleOfSkew)));
	double KShape = 2.5 * Sa1 / Sa0_3;
	double K3D = 1.0 + (double) (A / (theNumberOfSpans - B));

	double slightFactor;
	if (IShape == 0) {
		slightFactor = 1.0;
	} else {
		slightFactor = GSL_MIN(1.0, KShape);
	}

	SaVector[0] *= slightFactor;
	for (int i = 1; i < SaVector.count(); i++) {
		SaVector[i] *= K3D * KSkew;
	}

	PGDVector[3] *= f2;
	for (int i = 0; i < PGDVector.count() - 1; i++) {
		PGDVector[i] *= f1;
	}

	QVector<double> meanDaysVector;
	QVector<double> stdvDaysVector;
	meanDaysVector << 0.6 << 2.5 << 75.0 << 230.0;
	stdvDaysVector << 0.6 << 2.7 << 42.0 << 110.0;

	
	// Calculate the probability of each damage states
	double prob = 1.0;
	QVector<double> probabilityVector;
	for (int i = 0; i < SaVector.count(); i++) {
		double oldProb = prob;
		if (theAnalysisType == RFemaNibsBridgeDamageModel::GroundShaking) {			
			prob = gsl_cdf_gaussian_P(1.0 / betaSa * log(Sa1 / SaVector[i]), 1);

		} else if (theAnalysisType == RFemaNibsBridgeDamageModel::GroundFailure) {			
			prob = gsl_cdf_gaussian_P(1.0 / betaPGD * log(PGD / PGDVector[i]), 1);

		} else if (theAnalysisType == RFemaNibsBridgeDamageModel::Both) {			
			double PGDProb = gsl_cdf_gaussian_P(1.0 / betaPGD * log(PGD / PGDVector[i]), 1);
			double SaProb = gsl_cdf_gaussian_P(1.0 / betaSa * log(Sa1 / SaVector[i]), 1);
			prob = PGDProb + SaProb - PGDProb * SaProb;
		}
		probabilityVector << oldProb - prob;
	}
	probabilityVector << prob - 0.0;

	// Enter the central damage factors
	QVector<double> CDFVector;
	double completeCDF = 100.0;
	if (theNumberOfSpans > 2) {
		completeCDF = 2.0 / theNumberOfSpans * 100.0;
	}
	CDFVector << 0.0 << 3.0 << 8.0 << 25.0 << completeCDF;

	// Caluclate the mean damage factor and standard deviation
	double MDF = 0;
	double sigma2DF = 0;
	for (int i = 0; i < CDFVector.count(); i++) {
		MDF += CDFVector[i] * probabilityVector[i];
		sigma2DF += CDFVector[i] * CDFVector[i] * probabilityVector[i];
	}
	double sigmaDF = sqrt(sigma2DF - MDF * MDF);

	// Calculate the percent functional for each damage state at the "time" days after the earthquake
	QVector<double> percentFunctionalVector;
	percentFunctionalVector << 1.0;
	for (int i = 0; i < meanDaysVector.count(); i++) {
		percentFunctionalVector << gsl_cdf_gaussian_P(time - meanDaysVector[i], stdvDaysVector[i]);
	}

	// Caluclate the mean percent functional and standard deviation
	double MPF = 0;
	double sigma2PF = 0;
	for (int i = 0; i < percentFunctionalVector.count(); i++) {
		MPF += percentFunctionalVector[i] * probabilityVector[i];
		sigma2PF += percentFunctionalVector[i] * percentFunctionalVector[i] * probabilityVector[i];
	}
	double sigmaPF = sqrt(sigma2PF - MPF * MPF);
	if (qAbs(sigma2PF - MPF * MPF) < 1e-8) {
		sigmaPF = 0.0;
	}
	
	// Apply model error
	double modelUcertainty = theModelUncertainty.data()->getCurrentValue(threadID);
	double modelUcertaintyProb = gsl_cdf_gaussian_P(modelUcertainty, 1.0);

	
	// Calculating damage factor

	// Considering DF = LN(MDF, sigmaDF)
	double lognormalZetaDF = log(MDF)-0.5*log(1+pow((sigmaDF/MDF), 2.0));
	double lognormalSigmaDF = sqrt(log(pow((sigmaDF/MDF), 2.0) + 1));
	double damageFactor = gsl_cdf_lognormal_Pinv(modelUcertaintyProb, lognormalZetaDF, lognormalSigmaDF);

	//// Considering DF = BETA(MDF, sigmaDF, 0.0, 1.0)
	//RContinuousRandomVariable *betaRV = new RContinuousRandomVariable(0, QString("Beta"));
	//betaRV->setDistributionType(QString("Beta (a, b, min, max)"));
	//betaRV->setParameter3("0");
	//betaRV->setParameter4("100");
	//betaRV->setMean(QString("%1").arg(MDF));
	//betaRV->setStandardDeviation(QString("%1").arg(sigmaDF));
	//double damageFactor = betaRV->getInverseCDFValue(modelUcertaintyProb);
	//delete betaRV;

	damageFactor /= 100.0;

	

	// Calculating percent functional

	// Considering PF = LN(MPF, sigmaPF)
	double lognormalZetaPF = log(MPF)-0.5*log(1+pow((sigmaPF/MPF), 2.0));
	double lognormalSigmaPF = sqrt(log(pow((sigmaPF/MPF), 2.0) + 1));
	double percentFunctional = gsl_cdf_lognormal_Pinv(modelUcertaintyProb, lognormalZetaPF, lognormalSigmaPF);

	//// Considering PF = BETA(MPF, sigmaPF, 0.0, 1.0)
	//betaRV = new RContinuousRandomVariable(0, QString("Beta"));
	//betaRV->setDistributionType(QString("Beta (a, b, min, max)"));
	//betaRV->setParameter3("0");
	//betaRV->setParameter4("1");
	//betaRV->setMean(QString("%1").arg(MPF));
	//betaRV->setStandardDeviation(QString("%1").arg(sigmaPF));
	//double percentFunctional = betaRV->getInverseCDFValue(modelUcertaintyProb);
	//delete betaRV;



	// Adjusting damage factor
	if (damageFactor < 0) {
		damageFactor = 0;
	}
	if (damageFactor > 1.0) {
		damageFactor = 1.0;
	}

	// Adjusting percent functional
	if (percentFunctional < 0) {
		percentFunctional = 0;
	}
	if (percentFunctional > 1.0) {
		percentFunctional = 1.0;
	}
	
	// Updating the response current value
	thePercentFunctional->setCurrentValue(percentFunctional,threadID);
	theDamageFactor->setCurrentValue(damageFactor,threadID);

	return 0;
}


bool RFemaNibsBridgeDamageModel::canBeRunParallelly(){
	return true;
}