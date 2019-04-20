#include "RWaterStationDamageModel.h"	

#include "RParameter.h"	
#include "RResponse.h"	
#include "RWaterStationInformationModel.h"
#include <gsl/gsl_cdf.h>

#include <math.h>	

RWaterStationDamageModel::RWaterStationDamageModel(QObject *parent, QString name)
	: RModel(parent, name)
{
	// Initializing the data members
	//theWaterStationInformationModel = 0;
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

	//// Instantiating the response object Functionality
	//theDomain->createObject(objectName() + "FunctionalityResponse", "RGenericResponse");
	//theFunctionality = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	//theFunctionality->setModel(this);
	//theFunctionality->setCurrentValue(1);
	}

RWaterStationDamageModel::~RWaterStationDamageModel()
{

}

QObject * RWaterStationDamageModel::getWaterStationInformationModel() const
{
	return theWaterStationInformationModel.data();
}

void RWaterStationDamageModel::setWaterStationInformationModel(QObject *value)
{
	theWaterStationInformationModel = qobject_cast<RModel *>(value);
}

QObject * RWaterStationDamageModel::getIntensity() const
{
	return theIntensity.data();
}

void RWaterStationDamageModel::setIntensity(QObject *value)
{
	theIntensity = qobject_cast<RParameter *>(value);
}

QObject * RWaterStationDamageModel::getDamageStateUncertainty() const
{
	return theDamageStateUncertainty.data();
}

void RWaterStationDamageModel::setDamageStateUncertainty(QObject *value)
{
	theDamageStateUncertainty = qobject_cast<RParameter *>(value);
}

QObject * RWaterStationDamageModel::getDamageRatioUncertainty() const
{
	return theDamageRatioUncertainty.data();
}

void RWaterStationDamageModel::setDamageRatioUncertainty(QObject *value)
{
	theDamageRatioUncertainty = qobject_cast<RParameter *>(value);
}

int RWaterStationDamageModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
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


	// Getting the information from the WSIM
	RWaterStationInformationModel *theWSIM = qobject_cast<RWaterStationInformationModel *>(theWaterStationInformationModel.data());
	if (!theWSIM) {
		rCritical(threadID) << "Error in model" << objectName() << ":" << theWaterStationInformationModel.data()->objectName() << "is not a Water Station information model.";
		return -1;
	}

	double type = theWSIM->getType();
	double components = theWSIM->getComponents();
	double intensity = theIntensity.data()->getCurrentValue(threadID);

	//Tranform normal random numbers to uniform (0,1)
	double randNum1 = gsl_cdf_ugaussian_P(theDamageStateUncertainty.data()->getCurrentValue(threadID));
	double randNum2 = gsl_cdf_ugaussian_P(theDamageRatioUncertainty.data()->getCurrentValue(threadID));

	//Computing the damage ratio and damage state based on the FEMA

	//Vectors of median and beta of fragility curves
	QVector<double> medianVector;
	QVector<double> betaVector;

	//Parameters of fragility curves Table 8.6 & 8.7 of FEMA

	//Small
	if (type == RWaterStationInformationModel::Small) {
		if (components == RWaterStationInformationModel::Anchored) {
			medianVector << 0.15 << 0.36 << 0.66 << 1.50;
			betaVector << 0.70 << 0.65 << 0.65 << 0.80;
		}
		else if (components == RWaterStationInformationModel::Unanchored) {
			medianVector << 0.13 << 0.28 << 0.66 << 1.50;
			betaVector << 0.60 << 0.50 << 0.65 << 0.80;
		}
	}
	//Medium or Large
	else if ((type == RWaterStationInformationModel::Medium) || (type == RWaterStationInformationModel::Large)) {
		if (components == RWaterStationInformationModel::Anchored) {
			medianVector << 0.15 << 0.36 << 0.75 << 1.50;
			betaVector << 0.75 << 0.65 << 0.65 << 0.80;
		}
		else if (components == RWaterStationInformationModel::Unanchored) {
			medianVector << 0.13 << 0.28 << 0.77 << 1.50;
			betaVector << 0.60 << 0.50 << 0.65 << 0.80;
		}
	}

	//Mean Damage Ratio Table 15.25 FEMA
	QVector<double> minDamageRatioVector;
	QVector<double> maxDamageRatioVector;

	minDamageRatioVector << 0 << 0.01 << 0.15 << 0.40 << 0.8;
	maxDamageRatioVector << 0 << 0.15 << 0.4 << 0.80 << 1.0;

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

	////This model is based on engineering judgment and experts' opinion
	//double Functionality = 1.0;
	//if ((damageState>1) || (damageRatio>0.1)) {
	//	Functionality = 0.0;
	//}
	//theFunctionality->setCurrentValue(Functionality, threadID);

	return 0;
}

bool RWaterStationDamageModel::canBeRunParallelly() {
	return true;
}