#include "RBuildingResponseModel.h"

#include "RParameter.h"
#include "RResponse.h"
#include "RBuildingInformationModel.h"
#include "RConstant.h"

#include <gsl/gsl_math.h>

#include <math.h>
#include <qmutex.h>
#include <QMutexLocker>


RBuildingResponseModel::RBuildingResponseModel(QObject *parent, QString name)
	: RModel(parent, name)
{

/* //$$ 
	theBuildingInformationModel = 0;
	thePeriodInformation = 0;
	theSa = 0;

	theTheta1 = 0;
	theTheta2 = 0;
	theTheta3 = 0;
	theTheta4 = 0;
	theTheta5 = 0;
	theTheta6 = 0;
	theTheta7 = 0;
	theTheta8 = 0;
	theTheta9 = 0;
	theTheta10 = 0;
	theTheta11 = 0;
	theTheta12 = 0;
	theTheta13 = 0;
	theTheta14 = 0;
	theTheta15 = 0;
	theTheta16 = 0;
	theTheta17 = 0;
	theTheta18 = 0;
	theTheta19 = 0;
	theTheta20 = 0;

	theTheta21 = 0;
	theSigma1 = 0;
	theSigma2 = 0;
	theEpsilon1 = 0;
	theEpsilon2 = 0;

*/

	theDomain->createObject(objectName() + "DriftResponse", "RGenericResponse");
	thePeakDrift = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	thePeakDrift->setModel(this);	

	theDomain->createObject(objectName() + "AccelerationResponse", "RGenericResponse");
	thePeakAcc = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	thePeakAcc->setModel(this);

	upstreamParametersListLock = new QMutex();
}

RBuildingResponseModel::~RBuildingResponseModel()
{
	delete upstreamParametersListLock;
}

QObject * RBuildingResponseModel::getBuildingInformationModel() const	
{	
	return theBuildingInformationModel.data();
}	
	
void RBuildingResponseModel::setBuildingInformationModel(QObject *value)	
{	
	theBuildingInformationModel = qobject_cast<RModel *>(value);
}	

QObject * RBuildingResponseModel::getPeriodInformation() const	
{	
	return thePeriodInformation.data();
}	
	
void RBuildingResponseModel::setPeriodInformation(QObject *value)	
{	
	thePeriodInformation = qobject_cast<RParameter *>(value);
}	

QObject * RBuildingResponseModel::getSa() const	
{	
	return theSa.data();
}	
	
void RBuildingResponseModel::setSa(QObject *value)	
{	
	theSa = qobject_cast<RParameter *>(value);
}	

QObject * RBuildingResponseModel::getTheta1() const	
{	
	return theTheta1.data();
}	
	
void RBuildingResponseModel::setTheta1(QObject *value)	
{	
	theTheta1 = qobject_cast<RParameter *>(value);
}	
	
QObject * RBuildingResponseModel::getTheta2() const	
{	
	return theTheta2.data();
}	
	
void RBuildingResponseModel::setTheta2(QObject *value)	
{	
	theTheta2 = qobject_cast<RParameter *>(value);
}	
	
QObject * RBuildingResponseModel::getTheta3() const	
{	
	return theTheta3.data();
}	
	
void RBuildingResponseModel::setTheta3(QObject *value)	
{	
	theTheta3 = qobject_cast<RParameter *>(value);
}	
	
QObject * RBuildingResponseModel::getTheta4() const	
{	
	return theTheta4.data();
}	
	
void RBuildingResponseModel::setTheta4(QObject *value)	
{	
	theTheta4 = qobject_cast<RParameter *>(value);
}	
	
QObject * RBuildingResponseModel::getTheta5() const	
{	
	return theTheta5.data();
}	
	
void RBuildingResponseModel::setTheta5(QObject *value)	
{	
	theTheta5 = qobject_cast<RParameter *>(value);
}	
	
QObject * RBuildingResponseModel::getTheta6() const	
{	
	return theTheta6.data();
}	
	
void RBuildingResponseModel::setTheta6(QObject *value)	
{	
	theTheta6 = qobject_cast<RParameter *>(value);
}	
	
QObject * RBuildingResponseModel::getTheta7() const	
{	
	return theTheta7.data();
}	
	
void RBuildingResponseModel::setTheta7(QObject *value)	
{	
	theTheta7 = qobject_cast<RParameter *>(value);
}	
	
QObject * RBuildingResponseModel::getTheta8() const	
{	
	return theTheta8.data();
}	
	
void RBuildingResponseModel::setTheta8(QObject *value)	
{	
	theTheta8 = qobject_cast<RParameter *>(value);
}	
	
QObject * RBuildingResponseModel::getTheta9() const	
{	
	return theTheta9.data();
}	
	
void RBuildingResponseModel::setTheta9(QObject *value)	
{	
	theTheta9 = qobject_cast<RParameter *>(value);
}	
	
QObject * RBuildingResponseModel::getTheta10() const	
{	
	return theTheta10.data();
}	
	
void RBuildingResponseModel::setTheta10(QObject *value)	
{	
	theTheta10 = qobject_cast<RParameter *>(value);
}	
	
QObject * RBuildingResponseModel::getTheta11() const	
{	
	return theTheta11.data();
}	
	
void RBuildingResponseModel::setTheta11(QObject *value)	
{	
	theTheta11 = qobject_cast<RParameter *>(value);
}	
	
QObject * RBuildingResponseModel::getTheta12() const	
{	
	return theTheta12.data();
}	
	
void RBuildingResponseModel::setTheta12(QObject *value)	
{	
	theTheta12 = qobject_cast<RParameter *>(value);
}	
	
QObject * RBuildingResponseModel::getTheta13() const	
{	
	return theTheta13.data();
}	
	
void RBuildingResponseModel::setTheta13(QObject *value)	
{	
	theTheta13 = qobject_cast<RParameter *>(value);
}	
	
QObject * RBuildingResponseModel::getTheta14() const	
{	
	return theTheta14.data();
}	
	
void RBuildingResponseModel::setTheta14(QObject *value)	
{	
	theTheta14 = qobject_cast<RParameter *>(value);
}	
	
QObject * RBuildingResponseModel::getTheta15() const	
{	
	return theTheta15.data();
}	
	
void RBuildingResponseModel::setTheta15(QObject *value)	
{	
	theTheta15 = qobject_cast<RParameter *>(value);
}	
	
QObject * RBuildingResponseModel::getTheta16() const	
{	
	return theTheta16.data();
}	
	
void RBuildingResponseModel::setTheta16(QObject *value)	
{	
	theTheta16 = qobject_cast<RParameter *>(value);
}	
	
QObject * RBuildingResponseModel::getTheta17() const	
{	
	return theTheta17.data();
}	
	
void RBuildingResponseModel::setTheta17(QObject *value)	
{	
	theTheta17 = qobject_cast<RParameter *>(value);
}	
	
QObject * RBuildingResponseModel::getTheta18() const	
{	
	return theTheta18.data();
}	
	
void RBuildingResponseModel::setTheta18(QObject *value)	
{	
	theTheta18 = qobject_cast<RParameter *>(value);
}	
	
QObject * RBuildingResponseModel::getTheta19() const	
{	
	return theTheta19.data();
}	
	
void RBuildingResponseModel::setTheta19(QObject *value)	
{	
	theTheta19 = qobject_cast<RParameter *>(value);
}	
	
QObject * RBuildingResponseModel::getTheta20() const	
{	
	return theTheta20.data();
}	
	
void RBuildingResponseModel::setTheta20(QObject *value)	
{	
	theTheta20 = qobject_cast<RParameter *>(value);
}	

QObject * RBuildingResponseModel::getTheta21() const	
{	
	return theTheta21.data();
}	
	
void RBuildingResponseModel::setTheta21(QObject *value)	
{	
	theTheta21 = qobject_cast<RParameter *>(value);
}	
	
QObject * RBuildingResponseModel::getSigma1() const	
{	
	return theSigma1.data();
}	
	
void RBuildingResponseModel::setSigma1(QObject *value)	
{	
	theSigma1 = qobject_cast<RParameter *>(value);
}	
	
QObject * RBuildingResponseModel::getSigma2() const	
{	
	return theSigma2.data();
}	
	
void RBuildingResponseModel::setSigma2(QObject *value)	
{	
	theSigma2 = qobject_cast<RParameter *>(value);
}	
	
QObject * RBuildingResponseModel::getEpsilon1() const	
{	
	return theEpsilon1.data();
}	
	
void RBuildingResponseModel::setEpsilon1(QObject *value)	
{	
	theEpsilon1 = qobject_cast<RParameter *>(value);
}	
	
QObject * RBuildingResponseModel::getEpsilon2() const	
{	
	return theEpsilon2.data();
}	
	
void RBuildingResponseModel::setEpsilon2(QObject *value)	
{	
	theEpsilon2 = qobject_cast<RParameter *>(value);
}	

int RBuildingResponseModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
{	
	// Looking up the model parameters
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
	double theta16 = theTheta16.data()->getCurrentValue(threadID);
	double theta17 = theTheta17.data()->getCurrentValue(threadID);
	double theta18 = theTheta18.data()->getCurrentValue(threadID);
	double theta19 = theTheta19.data()->getCurrentValue(threadID);
	double theta20 = theTheta20.data()->getCurrentValue(threadID);
	double theta21 = theTheta21.data()->getCurrentValue(threadID);

	// Looking up the model error parameters
	double sigma1 = theSigma1.data()->getCurrentValue(threadID);
	double sigma2 = theSigma2.data()->getCurrentValue(threadID);
	double epsilon1 = theEpsilon1.data()->getCurrentValue(threadID) * sigma1;
	double epsilon2 = theEpsilon2.data()->getCurrentValue(threadID) * sigma2;

	//Looking up the demand parameter
	double Sa = theSa.data()->getCurrentValue(threadID);

	
	// Getting the information from the BIM
	RBuildingInformationModel *theBIM = qobject_cast<RBuildingInformationModel *>(theBuildingInformationModel.data());
	if (!theBIM) {
		rCritical(threadID) << "Error in model" << objectName() << ":" << theBuildingInformationModel.data()->objectName() << "is not a building information model.";
		return -1;
	}
	
	RBuildingInformationModel::RCodeLevel codeLevel = theBIM->getCodeLevel(threadID);
	double alpha = codeLevel + 1.0;
	double H = theBIM->getHeight();


	// Evaluating the sub-models
	double T = exp(-theta1) * pow(H, theta2);
	double V = exp(-theta3) * exp(-theta4 * H) * (2.0 + (alpha - 2.0) * (alpha - 1.0)) / 8.0;
	double gg = 9.81; //Gravity
	double deltaY = gsl_pow_2(T / 2.0 / M_PI) * V * gg / H;
	double Mu = exp(theta5) * pow(H, -theta6) * (10.0 + (alpha - 2.0) * (alpha - 1.0)) / 16.0;
	double deltaU = Mu * deltaY;
	double Kappa = exp(-theta7 * Sa * pow(alpha, -theta8));
	
	
	// Evaluating the drift model expression
	double logDrift = theta9 * log(deltaY) + theta10 * log(deltaU) - theta11 * log(V) - theta12 * log(Kappa) + theta13 * log(Sa) + theta14 * Sa - theta15 + epsilon1;
	double peakDrift = exp(logDrift);
	thePeakDrift->setCurrentValue(peakDrift, threadID);

	
	// Evaluating the acceleration model expression
	double logAcc = -theta16 * log(deltaY) + theta17 * log(V) - theta18 * log(Mu) + theta19 * log(Kappa) + theta20 * log(Sa) - theta21 + epsilon2;
	double peakAcc = exp(logAcc);
	thePeakAcc->setCurrentValue(peakAcc, threadID);

	if (evaluateDDM) {
		// IMPLEMENT
	}

	return 0;
}

int RBuildingResponseModel::setResponsesThatFlowUpstream(int threadID)
{	



	// Getting the information from the BIM
	RBuildingInformationModel *theBIM = qobject_cast<RBuildingInformationModel *>(theBuildingInformationModel.data());
	if (!theBIM) {
		rCritical(threadID) << "Error in model" << objectName() << ":" << theBuildingInformationModel.data()->objectName() << "is not a building information model.";
		return -1;
	}
	
	RBuildingInformationModel::RCodeLevel codeLevel = theBIM->getCodeLevel(threadID);
	double theta1 = theTheta1.data()->getCurrentValue(threadID);
	double theta2 = theTheta2.data()->getCurrentValue(threadID);
	double H = theBIM->getHeight();
	double T = exp(-theta1) * pow(H, theta2); 

	// Setting the value of the constant parameter (thePeriodInformation) to the calculated T. This constant will serve as input to upstream models such as the attenuation model
	thePeriodInformation.data()->setCurrentValue(T, threadID);
	//rDebug(threadID) <<"T=" << T;

	

	if (!upstreamParametersListHasBeenSet) {
		QMutexLocker locker(upstreamParametersListLock);

		//return 0;
		QList<RParameter *> parameterList;
		parameterList << theTheta1.data() << theTheta2.data();
		RConstant *constObject = qobject_cast<RConstant *>(thePeriodInformation.data());
		if (!constObject) {
			rCritical(threadID) << "Error in" << objectName() << ": The parameter" << thePeriodInformation.data()->objectName() << "that carries a response to an upstream model must be a Constant object";
			return -1;
		}
		constObject->setUpstreamParameterList(parameterList);

		upstreamParametersListHasBeenSet = true;
	}
	return 0;
}

bool RBuildingResponseModel::canBeRunParallelly(){
	return true;
}

void RBuildingResponseModel::reset() {
	upstreamParametersListHasBeenSet = false;
}