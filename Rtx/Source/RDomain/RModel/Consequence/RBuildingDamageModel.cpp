#include "RBuildingDamageModel.h"

#include "RParameter.h"
#include "RResponse.h"
#include "RBuildingInformationModel.h"

#include <gsl/gsl_cdf.h>

#include <math.h>

RBuildingDamageModel::RBuildingDamageModel(QObject *parent, QString name)
	: RModel(parent, name)
{
	//$$ theBuildingInformationModel = 0;
	//$$ thePeakDriftRatio = 0;
	//$$ thePeakAcceleration = 0;

	//$$ theTheta1 = 0;
	//$$ theTheta2 = 0;
	//$$ theTheta3 = 0;
	//$$ theTheta4 = 0;
	//$$ theTheta5 = 0;
	//$$ theTheta6 = 0;
	//$$ theTheta7 = 0;
	//$$ theTheta8 = 0;
	//$$ theTheta9 = 0;
	//$$ theTheta10 = 0;
	//$$ theTheta11 = 0;
	//$$ theTheta12 = 0;
	//$$ theTheta13 = 0;
	//$$ theTheta14 = 0;
	//$$ theTheta15 = 0;

	//$$ theSigma1 = 0;
	//$$ theSigma2 = 0;
	//$$ theSigma3 = 0;
	//$$ theSigma4 = 0;
	//$$ theEpsilon1 = 0;
	//$$ theEpsilon2 = 0;
	//$$ theEpsilon3 = 0;
	//$$ theEpsilon4 = 0;

	
	theDomain->createObject(objectName() + "StructuralResponse", "RGenericResponse");
	theStructuralDamageRatio = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theStructuralDamageRatio->setModel(this);

	theDomain->createObject(objectName() + "NonstructuralDriftResponse", "RGenericResponse");
	theNonstructuralDriftDamageRatio = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theNonstructuralDriftDamageRatio->setModel(this);

	theDomain->createObject(objectName() + "NonstructuralAccelerationResponse", "RGenericResponse");
	theNonstructuralAccelerationDamageRatio = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theNonstructuralAccelerationDamageRatio->setModel(this);

	theDomain->createObject(objectName() + "ContentResponse", "RGenericResponse");
	theContentDamageRatio = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theContentDamageRatio->setModel(this);
}

RBuildingDamageModel::~RBuildingDamageModel()
{

}

QObject * RBuildingDamageModel::getBuildingInformationModel() const	
{	
	return theBuildingInformationModel.data();
}	
	
void RBuildingDamageModel::setBuildingInformationModel(QObject *value)	
{	
	theBuildingInformationModel = qobject_cast<RModel *>(value);
}	

QObject * RBuildingDamageModel::getPeakDriftRatio() const	
{	
	return thePeakDriftRatio.data();
}	
	
void RBuildingDamageModel::setPeakDriftRatio(QObject *value)	
{	
	thePeakDriftRatio = qobject_cast<RParameter *>(value);
}	
	
QObject * RBuildingDamageModel::getPeakAcceleration() const	
{	
	return thePeakAcceleration.data();
}	
	
void RBuildingDamageModel::setPeakAcceleration(QObject *value)	
{	
	thePeakAcceleration = qobject_cast<RParameter *>(value);
}	

QObject * RBuildingDamageModel::getTheta1() const	
{	
	return theTheta1.data();
}	
	
void RBuildingDamageModel::setTheta1(QObject *value)	
{	
	theTheta1 = qobject_cast<RParameter *>(value);
}	
	
QObject * RBuildingDamageModel::getTheta2() const	
{	
	return theTheta2.data();
}	
	
void RBuildingDamageModel::setTheta2(QObject *value)	
{	
	theTheta2 = qobject_cast<RParameter *>(value);
}	
	
QObject * RBuildingDamageModel::getTheta3() const	
{	
	return theTheta3.data();
}	
	
void RBuildingDamageModel::setTheta3(QObject *value)	
{	
	theTheta3 = qobject_cast<RParameter *>(value);
}	
	
QObject * RBuildingDamageModel::getTheta4() const	
{	
	return theTheta4.data();
}	
	
void RBuildingDamageModel::setTheta4(QObject *value)	
{	
	theTheta4 = qobject_cast<RParameter *>(value);
}	
	
QObject * RBuildingDamageModel::getTheta5() const	
{	
	return theTheta5.data();
}	
	
void RBuildingDamageModel::setTheta5(QObject *value)	
{	
	theTheta5 = qobject_cast<RParameter *>(value);
}	
	
QObject * RBuildingDamageModel::getTheta6() const	
{	
	return theTheta6.data();
}	
	
void RBuildingDamageModel::setTheta6(QObject *value)	
{	
	theTheta6 = qobject_cast<RParameter *>(value);
}	
	
QObject * RBuildingDamageModel::getTheta7() const	
{	
	return theTheta7.data();
}	
	
void RBuildingDamageModel::setTheta7(QObject *value)	
{	
	theTheta7 = qobject_cast<RParameter *>(value);
}	
	
QObject * RBuildingDamageModel::getTheta8() const	
{	
	return theTheta8.data();
}	
	
void RBuildingDamageModel::setTheta8(QObject *value)	
{	
	theTheta8 = qobject_cast<RParameter *>(value);
}	
	
QObject * RBuildingDamageModel::getTheta9() const	
{	
	return theTheta9.data();
}	
	
void RBuildingDamageModel::setTheta9(QObject *value)	
{	
	theTheta9 = qobject_cast<RParameter *>(value);
}	
	
QObject * RBuildingDamageModel::getTheta10() const	
{	
	return theTheta10.data();
}	
	
void RBuildingDamageModel::setTheta10(QObject *value)	
{	
	theTheta10 = qobject_cast<RParameter *>(value);
}	
	
QObject * RBuildingDamageModel::getTheta11() const	
{	
	return theTheta11.data();
}	
	
void RBuildingDamageModel::setTheta11(QObject *value)	
{	
	theTheta11 = qobject_cast<RParameter *>(value);
}	
	
QObject * RBuildingDamageModel::getTheta12() const	
{	
	return theTheta12.data();
}	
	
void RBuildingDamageModel::setTheta12(QObject *value)	
{	
	theTheta12 = qobject_cast<RParameter *>(value);
}	
	
QObject * RBuildingDamageModel::getTheta13() const	
{	
	return theTheta13.data();
}	
	
void RBuildingDamageModel::setTheta13(QObject *value)	
{	
	theTheta13 = qobject_cast<RParameter *>(value);
}	
	
QObject * RBuildingDamageModel::getTheta14() const	
{	
	return theTheta14.data();
}	
	
void RBuildingDamageModel::setTheta14(QObject *value)	
{	
	theTheta14 = qobject_cast<RParameter *>(value);
}	
	
QObject * RBuildingDamageModel::getTheta15() const	
{	
	return theTheta15.data();
}	
	
void RBuildingDamageModel::setTheta15(QObject *value)	
{	
	theTheta15 = qobject_cast<RParameter *>(value);
}	

QObject * RBuildingDamageModel::getSigma1() const	
{	
	return theSigma1.data();
}	
	
void RBuildingDamageModel::setSigma1(QObject *value)	
{	
	theSigma1 = qobject_cast<RParameter *>(value);
}	
	
QObject * RBuildingDamageModel::getSigma2() const	
{	
	return theSigma2.data();
}	
	
void RBuildingDamageModel::setSigma2(QObject *value)	
{	
	theSigma2 = qobject_cast<RParameter *>(value);
}	
	
QObject * RBuildingDamageModel::getSigma3() const	
{	
	return theSigma3.data();
}	
	
void RBuildingDamageModel::setSigma3(QObject *value)	
{	
	theSigma3 = qobject_cast<RParameter *>(value);
}	
	
QObject * RBuildingDamageModel::getSigma4() const	
{	
	return theSigma4.data();
}	
	
void RBuildingDamageModel::setSigma4(QObject *value)	
{	
	theSigma4 = qobject_cast<RParameter *>(value);
}	
	
QObject * RBuildingDamageModel::getEpsilon1() const	
{	
	return theEpsilon1.data();
}	
	
void RBuildingDamageModel::setEpsilon1(QObject *value)	
{	
	theEpsilon1 = qobject_cast<RParameter *>(value);
}	
	
QObject * RBuildingDamageModel::getEpsilon2() const	
{	
	return theEpsilon2.data();
}	
	
void RBuildingDamageModel::setEpsilon2(QObject *value)	
{	
	theEpsilon2 = qobject_cast<RParameter *>(value);
}	
	
QObject * RBuildingDamageModel::getEpsilon3() const	
{	
	return theEpsilon3.data();
}	
	
void RBuildingDamageModel::setEpsilon3(QObject *value)	
{	
	theEpsilon3 = qobject_cast<RParameter *>(value);
}	
	
QObject * RBuildingDamageModel::getEpsilon4() const	
{	
	return theEpsilon4.data();
}	
	
void RBuildingDamageModel::setEpsilon4(QObject *value)	
{	
	theEpsilon4 = qobject_cast<RParameter *>(value);
}	

int RBuildingDamageModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
{	
	// Looking up model parameters
	double theta1 = theTheta1.data()->getCurrentValue(threadID);
	double theta2 = theTheta2.data()->getCurrentValue(threadID);
	double theta3 = theTheta3.data()->getCurrentValue(threadID);
	double theta4 = theTheta4.data()->getCurrentValue(threadID);
	double theta5 = theTheta5.data()->getCurrentValue(threadID);
	double theta6 = theTheta6.data()->getCurrentValue(threadID);
	double theta7 = theTheta7.data()->getCurrentValue(threadID);
	double theta8 = theTheta8.data()->getCurrentValue(threadID);
	double theta9 = theTheta9.data()->getCurrentValue(threadID);
	double theta10 = theTheta10.data()->getCurrentValue(threadID);
	double theta11 = theTheta11.data()->getCurrentValue(threadID);
	double theta12 = theTheta12.data()->getCurrentValue(threadID);
	double theta13 = theTheta13.data()->getCurrentValue(threadID);
	double theta14 = theTheta14.data()->getCurrentValue(threadID);
	double theta15 = theTheta15.data()->getCurrentValue(threadID);

	// Looking up the model error parameters
	double sigma1 = theSigma1.data()->getCurrentValue(threadID);
	double sigma2 = theSigma2.data()->getCurrentValue(threadID);
	double sigma3 = theSigma3.data()->getCurrentValue(threadID);
	double sigma4 = theSigma4.data()->getCurrentValue(threadID);
	double epsilon1 = theEpsilon1.data()->getCurrentValue(threadID) * sigma1;
	double epsilon2 = theEpsilon2.data()->getCurrentValue(threadID) * sigma2;
	double epsilon3 = theEpsilon3.data()->getCurrentValue(threadID) * sigma3;
	double epsilon4 = theEpsilon4.data()->getCurrentValue(threadID) * sigma4;

	// Looking up the drift and acceleration
	double drift = thePeakDriftRatio.data()->getCurrentValue(threadID);
	double acceleration = thePeakAcceleration.data()->getCurrentValue(threadID);

	
	// Getting the information from the BIM
	RBuildingInformationModel *theBIM = qobject_cast<RBuildingInformationModel *>(theBuildingInformationModel.data());
	if (!theBIM) {
		rCritical(threadID) << "Error in model" << objectName() << ":" << theBuildingInformationModel.data()->objectName() << "is not a building information model.";
		return -1;
	}
	
	RBuildingInformationModel::RCodeLevel codeLevel = theBIM->getCodeLevel(threadID);
	double alpha = codeLevel + 1.0;
	double H = theBIM->getHeight();


	// Look up for irregularities
	double Ivi = 0;
	double Ipi = 0;
	double Iss = 0;
	double Isc = 0;
	double Ip = 0;
	if (theBIM->getVerticalIrregularity()) {
		Ivi = 1.0;
	}
	if (theBIM->getVerticalIrregularity()) {
		Ipi = 1.0;
	}
	if (theBIM->getSoftStorey()) {
		Iss = 1.0;
	}
	if (theBIM->getShortColumn()) {
		Isc = 1.0;
	}
	if (theBIM->getPounding()) {
		Ip = 1.0;
	}


	// Evaluating the model expression of the structural damage model
	double damageRatioS = gsl_cdf_gaussian_P(theta1 * log(drift) + theta2 * log(H) - theta3 * alpha + theta4, 1.0) * exp(theta5 * Ivi + theta6 * Ipi + theta7 * Iss + theta8 * Isc + theta9 * Ip);
	theStructuralDamageRatio->setCurrentValue(damageRatioS,threadID);


	// Evaluating the model expression of the non-structural drift-sensitive damage model
	double damageRatioND = gsl_cdf_gaussian_P(theta10 * log(drift) + theta11, 1.0);
	theNonstructuralDriftDamageRatio->setCurrentValue(damageRatioND,threadID);


	// Evaluating the model expression of the non-structural acecleration-sensitive damage model
	double damageRatioNA = gsl_cdf_gaussian_P(theta12 * log(acceleration) - theta13 * alpha, 1.0);
	theNonstructuralAccelerationDamageRatio->setCurrentValue(damageRatioNA,threadID);


	// Evaluating the model expression of the content damage model
	double damageRatioC = gsl_cdf_gaussian_P(theta14 * log(acceleration) - theta15 * alpha, 1.0);
	theContentDamageRatio->setCurrentValue(damageRatioC,threadID);

	if (evaluateDDM) {
		// IMPLEMENT
	}

	return 0;
}

void RBuildingDamageModel::updateHistoryVariables(int threadID)
{
	if (theStructuralDamageRatio->getCurrentValue(threadID) > 0.5) {
		theBuildingInformationModel.data()->updateHistoryVariables(threadID);
	}
}

void RBuildingDamageModel::resetHistoryVariables(int threadID)
{
	theBuildingInformationModel.data()->resetHistoryVariables(threadID);
}

bool RBuildingDamageModel::canBeRunParallelly(){
	return true;
}