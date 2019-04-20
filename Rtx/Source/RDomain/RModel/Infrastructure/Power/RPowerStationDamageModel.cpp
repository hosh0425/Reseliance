#include "RPowerStationDamageModel.h"	

#include "RParameter.h"	
#include "RResponse.h"	
#include "RPowerStationInformationModel.h"
#include <gsl/gsl_cdf.h>

#include <math.h>	

RPowerStationDamageModel::RPowerStationDamageModel(QObject *parent, QString name)
	: RModel(parent, name)
{
	// Initializing the data members
	//thePowerStationInformationModel = 0;
	//theIntensity = 0;
	//theDamageStateUncertainty = 0;
	//theDamageRatioUncertainty = 0;

	// Instantiating the response object
	theDomain->createObject(objectName() + "RatioResponse", "RGenericResponse");
	theDamageRatio = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theDamageRatio->setModel(this);

	theDomain->createObject(objectName() + "StateResponse", "RGenericResponse");
	theDamageState = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theDamageState->setModel(this);

	//// Instantiating the response object Serviceability
	//theDomain->createObject(objectName() + "ServiceabilityResponse", "RGenericResponse");
	//theServiceability = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	//theServiceability->setModel(this);
	//theServiceability->setCurrentValue(1);
}

RPowerStationDamageModel::~RPowerStationDamageModel()
{

}

QObject * RPowerStationDamageModel::getPowerStationInformationModel() const
{
	return thePowerStationInformationModel.data();
}

void RPowerStationDamageModel::setPowerStationInformationModel(QObject *value)
{
	thePowerStationInformationModel = qobject_cast<RModel *>(value);
}

QObject * RPowerStationDamageModel::getIntensity() const
{
	return theIntensity.data();
}

void RPowerStationDamageModel::setIntensity(QObject *value)
{
	theIntensity = qobject_cast<RParameter *>(value);
}

QObject * RPowerStationDamageModel::getDamageStateUncertainty() const
{
	return theDamageStateUncertainty.data();
}

void RPowerStationDamageModel::setDamageStateUncertainty(QObject *value)
{
	theDamageStateUncertainty = qobject_cast<RParameter *>(value);
}

QObject * RPowerStationDamageModel::getDamageRatioUncertainty() const
{
	return theDamageRatioUncertainty.data();
}

void RPowerStationDamageModel::setDamageRatioUncertainty(QObject *value)
{
	theDamageRatioUncertainty = qobject_cast<RParameter *>(value);
}

int RPowerStationDamageModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
{

	// Checking if all pointer properties are given by the user
	int status = this->checkProperties();
	// Error handling
	if (status < 0) {
		// Printing an error message to the output pane
		rCritical(threadID) << "Error in" << this->objectName() << ":"
			<< "Could not find all the passed objects";
		// Returning the error code
		return -1;
	}


	// Getting the information from the PSIM
	RPowerStationInformationModel *thePSIM = qobject_cast<RPowerStationInformationModel *>(thePowerStationInformationModel.data());
	if (!thePSIM) {
		rCritical(threadID) << "Error in model" << objectName() << ":" << thePowerStationInformationModel.data()->objectName() << "is not a Power Station information model.";
		return -1;
	}

	double type = thePSIM->getType();
	double components = thePSIM->getComponents();
	double intensity = theIntensity.data()->getCurrentValue(threadID);

	//Tranform normal random numbers to uniform (0,1)
	double randNum1 = gsl_cdf_ugaussian_P(theDamageStateUncertainty.data()->getCurrentValue(threadID));
	double randNum2 = gsl_cdf_ugaussian_P(theDamageRatioUncertainty.data()->getCurrentValue(threadID));

	//Computing the damage ratio and damage state based on the FEMA

	//Vectors of median and beta of fragility curves
	QVector<double> medianVector;
	QVector<double> betaVector;

	//Parameters of fragility curves Table 8.23 & 8.24 of FEMA

	//Low Voltage
	if (type == RPowerStationInformationModel::LowVoltage) {
		if (components == RPowerStationInformationModel::Anchored) {
			medianVector << 0.15 << 0.29 << 0.45 << 0.90;
			betaVector << 0.70 << 0.55 << 0.45 << 0.45;
		}
		else if (components == RPowerStationInformationModel::Unanchored) {
			medianVector << 0.13 << 0.26 << 0.34 << 0.74;
			betaVector << 0.65 << 0.50 << 0.40 << 0.40;
		}
	}
	//Medium Voltage
	else if (type == RPowerStationInformationModel::MediumVoltage) {
		if (components == RPowerStationInformationModel::Anchored) {
			medianVector << 0.15 << 0.25 << 0.35 << 0.70;
			betaVector << 0.60 << 0.50 << 0.40 << 0.40;
		}
		else if (components == RPowerStationInformationModel::Unanchored) {
			medianVector << 0.10 << 0.20 << 0.30 << 0.50;
			betaVector << 0.60 << 0.50 << 0.40 << 0.40;
		}
	}
	//High Voltage
	else if (type == RPowerStationInformationModel::HighVoltage) {
		if (components == RPowerStationInformationModel::Anchored) {
			;
			medianVector << 0.11 << 0.15 << 0.20 << 0.47;
			betaVector << 0.50 << 0.45 << 0.35 << 0.40;
		}
		else if (components == RPowerStationInformationModel::Unanchored) {
			medianVector << 0.09 << 0.13 << 0.17 << 0.38;
			betaVector << 0.50 << 0.40 << 0.35 << 0.35;
		}
	}

	//Mean Damage Ratio Table 15.28 FEMA
	QVector<double> minDamageRatioVector;
	QVector<double> maxDamageRatioVector;

	minDamageRatioVector << 0 << 0.01 << 0.15 << 0.40 << 0.8;
	maxDamageRatioVector << 0 << 0.15 << 0.4 << 0.80 << 1.0;

	//double SlightMinDR=0.01;
	//double SlightMaxDR=0.15;

	//double ModerateMinDR=0.15;
	//double ModerateMaxDR=0.4;
	//
	//double ExtensiveMinDR=0.40;
	//double ExtensiveMaxDR=0.80;
	//
	//double CompleteMinDR=0.8;
	//double CompleteMaxDR=1.0;

	////Mean Replacement Value : Table 15.17 FEMA
	//double LowAnchRepValue = 10000;
	//double LowUnanchRepValue = 10000;

	//double MediumAnchRepValue = 20000;
	//double MediumUnanchRepValue = 20000;

	//double HighAnchRepValue = 50000;
	//double HighUnanchRepValue = 50000;


	//THE ALGORITHM
	//QString a = this->objectName();
	//Compute the probability for each damage state

	//compute P(DS>=dsi)
	QVector <double> CumProbDamageVector;
	QVector <double> ProbDamageVector;

	for (int i = 0; i<4; i++) {
		CumProbDamageVector << gsl_cdf_ugaussian_P(1 / betaVector[i] * log(intensity / medianVector[i]));
	}
	CumProbDamageVector << 0.0;
	CumProbDamageVector.prepend(1.0);

	for (int i = CumProbDamageVector.count() - 2; i > 0; i--) {
		if (CumProbDamageVector[i] < CumProbDamageVector[i + 1]) {
			CumProbDamageVector[i] = 0.5*(CumProbDamageVector[i - 1] + CumProbDamageVector[i + 1]);
		}
	}
	CumProbDamageVector.remove(0);

	ProbDamageVector << (1 - CumProbDamageVector[0]);
	for (int i = 1; i<CumProbDamageVector.count(); i++) {
		ProbDamageVector << (CumProbDamageVector[i - 1] - CumProbDamageVector[i]);
	}

	double damageRatio = 0.0;
	int damageState = 0;

	//Computing the damage state
	for (int i = 1; i<CumProbDamageVector.count(); i++) {
		if ((randNum1 <= CumProbDamageVector[i - 1]) && (randNum1 > CumProbDamageVector[i])) {
			damageState = i;
		};
	}
	theDamageState->setCurrentValue(damageState, threadID);

	//Computing the damage ratio
	/*for (int i=0; i<5; i++) {
	if (damageState==i){
	damageRatio = randNum2*(maxDamageRatioVector[i]-minDamageRatioVector[i]) + minDamageRatioVector[i];
	break;
	}
	}*/
	damageRatio = randNum2*(maxDamageRatioVector[damageState] - minDamageRatioVector[damageState]) + minDamageRatioVector[damageState];
	if (damageRatio<0) {
		damageRatio = 0.0;
	}

	/*switch (damageState){
	case 0:
	damageRatio = 0;
	break;
	case 1:
	damageRatio=randNum2*(SlightMaxDR-SlightMinDR)+SlightMinDR;
	break;
	case 2:
	damageRatio=randNum2*(ModerateMaxDR-ModerateMinDR)+ModerateMinDR;
	break;
	case 3:
	damageRatio=randNum2*(ExtensiveMaxDR-ExtensiveMinDR)+ExtensiveMinDR;
	break;
	case 4:
	damageRatio=randNum2*(CompleteMaxDR-CompleteMinDR)+CompleteMinDR;
	break;
	}*/

	theDamageRatio->setCurrentValue(damageRatio, threadID);

	//double serviceability = 1.0;
	//if ((damageState>1) || (damageRatio>0.1)) {
	//	serviceability = 0.0;
	//}
	//theServiceability->setCurrentValue(serviceability, threadID);


	return 0;
}

bool RPowerStationDamageModel::canBeRunParallelly() {
	return true;
}