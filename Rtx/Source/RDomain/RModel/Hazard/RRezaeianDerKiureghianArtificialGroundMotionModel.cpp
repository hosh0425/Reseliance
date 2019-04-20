#include "RRezaeianDerKiureghianArtificialGroundMotionModel.h"	

#include <QCoreApplication>
#include <QMetaEnum>
#include <QFile>
#include <QVector>
#include <QRegExp>
#include <QtCore/qmath.h>

#include "RParameter.h"	
#include "RResponse.h"	
#include "RLocation.h"
#include "RLocationResponse.h"
#include "RMainWindow.h"


#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_cdf.h>
#include <gsl/gsl_multiroots.h>
#include <gsl/gsl_sf_gamma.h>
#include <gsl/gsl_multifit_nlin.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

#include <iostream>     // std::cout, std::ios
#include <sstream>      // std::stringstream.
#include <fstream>


RRezaeianDerKiureghianArtificialGroundMotionModel::RRezaeianDerKiureghianArtificialGroundMotionModel(QObject *parent, QString name)	
	: RModel(parent, name)
{	
	// Initializing the data members
	theFaultType = RRezaeianDerKiureghianArtificialGroundMotionModel::Reverse;

	// RPOINTER_TAG
	/*theMomentMagnitude = 0;
	theRuptureLocation = 0;
	theShearWaveVelocity = 0;
	theV1IntereventOrTotalModelError = 0;
	theV2IntereventOrTotalModelError = 0;
	theV3IntereventOrTotalModelError = 0;
	theV4IntereventOrTotalModelError = 0;
	theV5IntereventOrTotalModelError = 0;
	theV6IntereventOrTotalModelError = 0;
	theV1IntraeventModelError = 0;
	theV2IntraeventModelError = 0;
	theV3IntraeventModelError = 0;
	theV4IntraeventModelError = 0;
	theV5IntraeventModelError = 0;
	theV6IntraeventModelError = 0;
	theStructureLocation = 0;*/

	theDuration = 0.0;
	theTimeStep = 0.02;
	
	theDomain->createObject(objectName() + "PGAResponse", "RGenericResponse");
	thePeakGroundAccelerationResponse = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	thePeakGroundAccelerationResponse->setModel(this);

	theDomain->createObject(objectName() + "RuptureDistanceResponse", "RGenericResponse");
	theRuptureDistanceResponse = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theRuptureDistanceResponse->setModel(this);
}	

RRezaeianDerKiureghianArtificialGroundMotionModel::~RRezaeianDerKiureghianArtificialGroundMotionModel()	
{	

}	

QString RRezaeianDerKiureghianArtificialGroundMotionModel::getWhiteNoiseVariableList()	
{	
	return parameterListToString(theWhiteNoiseVariableList);
}	

void RRezaeianDerKiureghianArtificialGroundMotionModel::setWhiteNoiseVariableList(QString value)	
{	
	theWhiteNoiseVariableList = stringToParameterList(value);
}	

RRezaeianDerKiureghianArtificialGroundMotionModel::RFaultType RRezaeianDerKiureghianArtificialGroundMotionModel::getFaultType() const	
{	
	return theFaultType;
}	

void RRezaeianDerKiureghianArtificialGroundMotionModel::setFaultType(RRezaeianDerKiureghianArtificialGroundMotionModel::RFaultType value)	
{	
	theFaultType = value;
}	

QObject * RRezaeianDerKiureghianArtificialGroundMotionModel::getMomentMagnitude() const	
{	
	return theMomentMagnitude.data();
}	

void RRezaeianDerKiureghianArtificialGroundMotionModel::setMomentMagnitude(QObject *value)	
{	
	theMomentMagnitude = qobject_cast<RParameter *>(value);
}	

QObject * RRezaeianDerKiureghianArtificialGroundMotionModel::getRuptureLocation() const	
{	
	return theRuptureLocation.data();
}	

void RRezaeianDerKiureghianArtificialGroundMotionModel::setRuptureLocation(QObject *value)	
{	
	theRuptureLocation = qobject_cast<RParameter *>(value);
}	

QObject * RRezaeianDerKiureghianArtificialGroundMotionModel::getShearWaveVelocity() const	
{	
	return theShearWaveVelocity.data();
}	

void RRezaeianDerKiureghianArtificialGroundMotionModel::setShearWaveVelocity(QObject *value)	
{	
	theShearWaveVelocity = qobject_cast<RParameter *>(value);
}	

QObject * RRezaeianDerKiureghianArtificialGroundMotionModel::getV1IntereventOrTotalModelError() const	
{	
	return theV1IntereventOrTotalModelError.data();
}	

void RRezaeianDerKiureghianArtificialGroundMotionModel::setV1IntereventOrTotalModelError(QObject *value)	
{	
	theV1IntereventOrTotalModelError = qobject_cast<RParameter *>(value);
}	

QObject * RRezaeianDerKiureghianArtificialGroundMotionModel::getV1IntraeventModelError() const	
{	
	return theV1IntraeventModelError.data();
}	

void RRezaeianDerKiureghianArtificialGroundMotionModel::setV1IntraeventModelError(QObject *value)	
{	
	theV1IntraeventModelError = qobject_cast<RParameter *>(value);
}	

QObject * RRezaeianDerKiureghianArtificialGroundMotionModel::getV2IntereventOrTotalModelError() const	
{	
	return theV2IntereventOrTotalModelError.data();
}	

void RRezaeianDerKiureghianArtificialGroundMotionModel::setV2IntereventOrTotalModelError(QObject *value)	
{	
	theV2IntereventOrTotalModelError = qobject_cast<RParameter *>(value);
}	

QObject * RRezaeianDerKiureghianArtificialGroundMotionModel::getV2IntraeventModelError() const	
{	
	return theV2IntraeventModelError.data();
}	

void RRezaeianDerKiureghianArtificialGroundMotionModel::setV2IntraeventModelError(QObject *value)	
{	
	theV2IntraeventModelError = qobject_cast<RParameter *>(value);
}	

QObject * RRezaeianDerKiureghianArtificialGroundMotionModel::getV3IntereventOrTotalModelError() const	
{	
	return theV3IntereventOrTotalModelError.data();
}	

void RRezaeianDerKiureghianArtificialGroundMotionModel::setV3IntereventOrTotalModelError(QObject *value)	
{	
	theV3IntereventOrTotalModelError = qobject_cast<RParameter *>(value);
}	

QObject * RRezaeianDerKiureghianArtificialGroundMotionModel::getV3IntraeventModelError() const	
{	
	return theV3IntraeventModelError.data();
}	

void RRezaeianDerKiureghianArtificialGroundMotionModel::setV3IntraeventModelError(QObject *value)	
{	
	theV3IntraeventModelError = qobject_cast<RParameter *>(value);
}	

QObject * RRezaeianDerKiureghianArtificialGroundMotionModel::getV4IntereventOrTotalModelError() const	
{	
	return theV4IntereventOrTotalModelError.data();
}	

void RRezaeianDerKiureghianArtificialGroundMotionModel::setV4IntereventOrTotalModelError(QObject *value)	
{	
	theV4IntereventOrTotalModelError = qobject_cast<RParameter *>(value);
}	

QObject * RRezaeianDerKiureghianArtificialGroundMotionModel::getV4IntraeventModelError() const	
{	
	return theV4IntraeventModelError.data();
}	

void RRezaeianDerKiureghianArtificialGroundMotionModel::setV4IntraeventModelError(QObject *value)	
{	
	theV4IntraeventModelError = qobject_cast<RParameter *>(value);
}	

QObject * RRezaeianDerKiureghianArtificialGroundMotionModel::getV5IntereventOrTotalModelError() const	
{	
	return theV5IntereventOrTotalModelError.data();
}	

void RRezaeianDerKiureghianArtificialGroundMotionModel::setV5IntereventOrTotalModelError(QObject *value)	
{	
	theV5IntereventOrTotalModelError = qobject_cast<RParameter *>(value);
}	

QObject * RRezaeianDerKiureghianArtificialGroundMotionModel::getV5IntraeventModelError() const	
{	
	return theV5IntraeventModelError.data();
}	

void RRezaeianDerKiureghianArtificialGroundMotionModel::setV5IntraeventModelError(QObject *value)	
{	
	theV5IntraeventModelError = qobject_cast<RParameter *>(value);
}	

QObject * RRezaeianDerKiureghianArtificialGroundMotionModel::getV6IntereventOrTotalModelError() const	
{	
	return theV6IntereventOrTotalModelError.data();
}	

void RRezaeianDerKiureghianArtificialGroundMotionModel::setV6IntereventOrTotalModelError(QObject *value)	
{	
	theV6IntereventOrTotalModelError = qobject_cast<RParameter *>(value);
}	

QObject * RRezaeianDerKiureghianArtificialGroundMotionModel::getV6IntraeventModelError() const	
{	
	return theV6IntraeventModelError.data();
}	

void RRezaeianDerKiureghianArtificialGroundMotionModel::setV6IntraeventModelError(QObject *value)	
{	
	theV6IntraeventModelError = qobject_cast<RParameter *>(value);
}	

QObject * RRezaeianDerKiureghianArtificialGroundMotionModel::getStructureLocation() const	
{	
	return theStructureLocation.data();
}	

void RRezaeianDerKiureghianArtificialGroundMotionModel::setStructureLocation(QObject *value)	
{	
	theStructureLocation = qobject_cast<RParameter *>(value);
}

double RRezaeianDerKiureghianArtificialGroundMotionModel::getDuration() const	
{	
	return theDuration;
}	

void RRezaeianDerKiureghianArtificialGroundMotionModel::setDuration(double value)	
{	
	theDuration = value;
}	

double RRezaeianDerKiureghianArtificialGroundMotionModel::getTimeStep() const	
{	
	return theTimeStep;
}	

void RRezaeianDerKiureghianArtificialGroundMotionModel::setTimeStep(double value)	
{	
	theTimeStep = value;
}	

QList<RParameter *> RRezaeianDerKiureghianArtificialGroundMotionModel::getPhysicalParameterList()
{
	QList<RParameter *> parmList;
	for (int i = 0; i < theWhiteNoiseVariableList.count(); i++){
		parmList << theWhiteNoiseVariableList[i].data();
	}
	
	return parmList;
}

int RRezaeianDerKiureghianArtificialGroundMotionModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int ThreadID)
{	
	double magnitude = theMomentMagnitude.data()->getCurrentValue();
	double shVelocity = theShearWaveVelocity.data()->getCurrentValue();
	double v1IntereventOrTotalModelError = theV1IntereventOrTotalModelError.data()->getCurrentValue();
	double v2IntereventOrTotalModelError = theV2IntereventOrTotalModelError.data()->getCurrentValue();
	double v3IntereventOrTotalModelError = theV3IntereventOrTotalModelError.data()->getCurrentValue();
	double v4IntereventOrTotalModelError = theV4IntereventOrTotalModelError.data()->getCurrentValue();
	double v5IntereventOrTotalModelError = theV5IntereventOrTotalModelError.data()->getCurrentValue();
	double v6IntereventOrTotalModelError = theV6IntereventOrTotalModelError.data()->getCurrentValue();
	double v1IntraeventModelError = theV1IntraeventModelError.data()->getCurrentValue();
	double v2IntraeventModelError = theV2IntraeventModelError.data()->getCurrentValue();
	double v3IntraeventModelError = theV3IntraeventModelError.data()->getCurrentValue();
	double v4IntraeventModelError = theV4IntraeventModelError.data()->getCurrentValue();
	double v5IntraeventModelError = theV5IntraeventModelError.data()->getCurrentValue();
	double v6IntraeventModelError = theV6IntraeventModelError.data()->getCurrentValue();
	double durationMotion = theDuration;
	double TimeStep = theTimeStep;

	double ariasIntensity;
	double effectiveDuration;
	double midTimeOfStrongMotion;
	double omegaPrime;
	double omegaAtMidOfStrongMotion;
	double kisi;
	double pi = M_PI;
	double omegaC = 2.0*pi*0.1;  //NOTE: it can be as a variable


	if (magnitude!=0)
	{
	int accCount = theWhiteNoiseVariableList.count();
	gsl_vector *whiteNoiseVector = gsl_vector_calloc(accCount);
	for (int i = 0; i < accCount; i++) {
		gsl_vector_set(whiteNoiseVector, i, theWhiteNoiseVariableList[i].data()->getCurrentValue());
		/*if ((int)i / 10 = 0) {
		QCoreApplication::processEvents();
		}*/
	}
	gsl_vector *simulatedGroundMotion = gsl_vector_calloc(accCount); //ground motion acceleration proccess
	

	//beta matrix elements
	double b00 = -1.844; double b01 = -0.071; double b02 = 2.944;  double b03 = -1.356; double b04 = -0.265; //double s0=0.65;
	double b10 = -6.195; double b11 = -0.703; double b12 = 6.792;  double b13 = 0.219;  double b14 = -0.523; //double s1=0.73;
	double b20 = -5.011; double b21 = -0.345; double b22 = 4.638;  double b23 = 0.348;  double b24 = -0.185; //double s2=0.66;
	double b30 = 2.253;  double b31 = -0.081; double b32 = -1.810; double b33 = -0.211; double b34 = 0.012;  //double s3=1;
	double b40 = -2.489; double b41 = 0.044;  double b42 = 2.408;  double b43 = 0.065;  double b44 = -0.081; //double s4=0.96;
	double b50 = -0.258; double b51 = -0.477; double b52 = 0.905;  double b53 = -0.289; double b54 = 0.316;  //double s5=1.02;

	//correlations between error terms of model parameters. Note: It is assigned in input file if necessary
	//double c00=1; double c01=-0.36; double c02=0.01; double c03=-0.15; double c04=0.13;  double c05=-0.01;
	//			    double c11=1;     double c12=0.67; double c13=-0.13; double c14=-0.16; double c15=-0.20;
	//								  double c22=1;    double c23=-0.28; double c24=-0.20; double c25=-0.22;
	//											       double c33=1;     double c34=-0.20; double c35=0.28;
	//																     double c44=1;     double c45=-0.01;
	//																		  		  	   double c55=1;
	gsl_matrix *betaMatrix = gsl_matrix_calloc (6 , 5);
	gsl_matrix_set (betaMatrix, 0, 0, b00); 
	gsl_matrix_set (betaMatrix, 0, 1, b01); 
	gsl_matrix_set (betaMatrix, 0, 2, b02); 
	gsl_matrix_set (betaMatrix, 0, 3, b03); 
	gsl_matrix_set (betaMatrix, 0, 4, b04); 
	gsl_matrix_set (betaMatrix, 1, 0, b10); 
	gsl_matrix_set (betaMatrix, 1, 1, b11); 
	gsl_matrix_set (betaMatrix, 1, 2, b12); 
	gsl_matrix_set (betaMatrix, 1, 3, b13); 
	gsl_matrix_set (betaMatrix, 1, 4, b14); 
	gsl_matrix_set (betaMatrix, 2, 0, b20); 
	gsl_matrix_set (betaMatrix, 2, 1, b21); 
	gsl_matrix_set (betaMatrix, 2, 2, b22); 
	gsl_matrix_set (betaMatrix, 2, 3, b23); 
	gsl_matrix_set (betaMatrix, 2, 4, b24); 
	gsl_matrix_set (betaMatrix, 3, 0, b30); 
	gsl_matrix_set (betaMatrix, 3, 1, b31); 
	gsl_matrix_set (betaMatrix, 3, 2, b32); 
	gsl_matrix_set (betaMatrix, 3, 3, b33); 
	gsl_matrix_set (betaMatrix, 3, 4, b34); 
	gsl_matrix_set (betaMatrix, 4, 0, b40); 
	gsl_matrix_set (betaMatrix, 4, 1, b41); 
	gsl_matrix_set (betaMatrix, 4, 2, b42); 
	gsl_matrix_set (betaMatrix, 4, 3, b43); 
	gsl_matrix_set (betaMatrix, 4, 4, b44); 
	gsl_matrix_set (betaMatrix, 5, 0, b50); 
	gsl_matrix_set (betaMatrix, 5, 1, b51); 
	gsl_matrix_set (betaMatrix, 5, 2, b52); 
	gsl_matrix_set (betaMatrix, 5, 3, b53); 
	gsl_matrix_set (betaMatrix, 5, 4, b54); 

	//Verification that the user has given an RLocationResponse   NOTE: THIS IS THE RIGHT ONE, BUT FOR SIMPLIFICATION I USE THAT ONE. AFTER ALL STEPS IT SHOULD BE UNCOMMENTED
	RLocation *theRuptureLocationObject = parameterToLocation(theRuptureLocation.data());
	if (!theRuptureLocationObject) {
		qCritical() << "Error: The location response" << theRuptureLocation.data()->objectName() << "does not include any location object.";
		return -1;
	}

	RLocation *theStructureLocationObject = parameterToLocation(theStructureLocation.data());
	if (!theStructureLocationObject) {
		qCritical() << "Error: The location response" << theStructureLocation.data()->objectName() << "does not include any location object.";
		return -1;
	}
	//Calculating distance between site and rupture location
	double rupDistance = theRuptureLocationObject->computeSurfaceDistance(theStructureLocationObject, 0); //THREAD_ID


	int faultType;
	if ("RFaultType" == "StrikeSlip")
	{
		faultType=0;
	}
	else
	{
		faultType=1;
	}

	//First, 6 total errors should be calculated with summation of inter and intra-event errors, then, 6 neccessary parameters will be computed by using regression equations and transformations
	double v1Error = v1IntereventOrTotalModelError + v1IntraeventModelError;
	double v2Error = v2IntereventOrTotalModelError + v2IntraeventModelError;
	double v3Error = v3IntereventOrTotalModelError + v3IntraeventModelError;
	double v4Error = v4IntereventOrTotalModelError + v4IntraeventModelError;
	double v5Error = v5IntereventOrTotalModelError + v5IntraeventModelError;
	double v6Error = v6IntereventOrTotalModelError + v6IntraeventModelError;
	int result = calculatingParameters(magnitude, rupDistance, shVelocity, faultType, v1Error, v2Error, v3Error, v4Error, v5Error, v6Error, betaMatrix, &ariasIntensity, &effectiveDuration, &midTimeOfStrongMotion, &omegaPrime, &omegaAtMidOfStrongMotion, &kisi);
	if (result  < 0) {
		qCritical() << "Error: Neccessary parameters were not calculated successfully.";
		return -1;
	}

	//Now these 6 parameters enter this function. The result is a ground motion alceleration process, before high-pass filtering.
	result = simulate(TimeStep, accCount, ariasIntensity, effectiveDuration, midTimeOfStrongMotion, omegaPrime, omegaAtMidOfStrongMotion, kisi, whiteNoiseVector, simulatedGroundMotion);

	if (result  < 0) {
		qCritical() << "Error: Ground motion process could not be simulated";
		return -1;
	}

	//building output of GM before high-pass filtering
	/*QString fileName1 = QString("Simulated Ground Motion1.txt");
	QFile *output1 = new QFile(fileName1);
	if (output1->exists()) {
	output1->remove();
	}
	if (!output1->open(QIODevice::WriteOnly | QIODevice::Text)) {
	qCritical() << "Warning: Could not create the file" << fileName1;
	}

	QString tempString1;
	//for (int i = 0; i < accCount; i++) {
	//	double *cDV = qobject_cast<RContinuousDecisionVariable *>(theDecisionVariableList[i]);
	//if (cDV) {	
	//	tempString += QString("%1\n").arg(gsl_vector_get(simulatedGroundMotion,i), 0, 'g', 14);
	//}
	//}
	for (int i = 0; i < accCount; i++) {
	tempString1 += QString("%1\n").arg(gsl_vector_get(simulatedGroundMotion,i), 0, 'E', 10);
	}
	QByteArray byteArray1;
	byteArray1.append(tempString1);
	output1->write(byteArray1);
	output1->flush();
	output1->close();
	delete output1;*/



	//The simulated GM is high-pass filtered here.
	result = highPassFilter(accCount, TimeStep, simulatedGroundMotion, omegaC);
	if (result  < 0) {
		qCritical() << "Error: High-pass filtering has not been done successfully";
		return -1;
	}
	//Final result is simulatedGroundMotion

	//Writing simulatedGroundMotion in a text file
	QString fileName = QString("Simulated Ground Motion.txt");
	QFile *output = new QFile(fileName);
	if (output->exists()) {
		output->remove();
	}
	if (!output->open(QIODevice::WriteOnly | QIODevice::Text)) {
		qCritical() << "Warning: Could not create the file" << fileName;
	}

	QString tempString;
	for (int i = 0; i < accCount; i++) {
		tempString += QString("%1\n").arg(gsl_vector_get(simulatedGroundMotion,i), 0, 'E', 10);
	}
	QByteArray byteArray;
	byteArray.append(tempString);
	output->write(byteArray);
	output->flush();
	output->close();
	delete output;
	// END of the TEMPORARY code*/

	
	//Making memory free
	gsl_vector_free(whiteNoiseVector);
	gsl_matrix_free(betaMatrix);
	double maxValue = 0.0;
	//maxValue = gsl_vector_max(simulatedGroundMotion);

	//Finding PGA
	int index = gsl_blas_idamax(simulatedGroundMotion);
	maxValue = fabs(gsl_vector_get(simulatedGroundMotion ,index));
	gsl_vector_free(simulatedGroundMotion);
	thePeakGroundAccelerationResponse->setCurrentValue(maxValue);
	theRuptureDistanceResponse->setCurrentValue(rupDistance);
	}
	//It is for the situation in which this hazard is off and GM is not simulated, and its output is only PGA and rupture distance
	else {
	double maxValue=0.0;
	double rupDistance=0.0;
	thePeakGroundAccelerationResponse->setCurrentValue(maxValue);
	theRuptureDistanceResponse->setCurrentValue(rupDistance);
	}

	return 0;
}

//[I,D,Tmid,omegM,omegPr,kis]=fn_getPARAM(Rrup,Vs30,F,M,beta,sigma);
int RRezaeianDerKiureghianArtificialGroundMotionModel::calculatingParameters(double magn, double rupDist, double shVel, int ftType, double v1Err, double v2Err, double v3Err, double v4Err, double v5Err, double v6Err, gsl_matrix *beta, double *ariasInt, double *effDur, double *midStrong, double *omegP, double *omegM, double *kis)
{
	//This vector is mean of vi that is computed with regression equations
	gsl_vector *meanOfV = gsl_vector_calloc(6);
	//regressors of the regression equations 2 to 6
	gsl_vector *vectorOfRegressors=gsl_vector_calloc(5);
	gsl_vector_set(vectorOfRegressors, 0, 1);  
	gsl_vector_set(vectorOfRegressors, 1, ftType); //it should be enum number
	gsl_vector_set(vectorOfRegressors, 2, magn/7);  
	gsl_vector_set(vectorOfRegressors, 3, rupDist/25);
	gsl_vector_set(vectorOfRegressors, 4, shVel/750);

	//Mn(1)=beta(1,:)*[1;F;(M/7);log(Rrup/25);log(Vs30/750)];
	//Mn(2:6)=beta(2:6,:)*[1;F;(M/7);(Rrup/25);(Vs30/750)];
	int gsl_status=gsl_blas_dgemv(CblasNoTrans, 1.0, beta, vectorOfRegressors, 0.0, meanOfV);
	if (gsl_status) {  
		qCritical() << "Error in GSL under calculatingParameters function:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_vector_set(meanOfV, 0, gsl_matrix_get(beta,0,0)*1+gsl_matrix_get(beta,0,1)*ftType+(gsl_matrix_get(beta,0,2)*magn)/7+gsl_matrix_get(beta,0,3)*log((rupDist/25))+gsl_matrix_get(beta,0,4)*log((shVel/750))); 


	//Here, computed mean of v is added to viErrors--which were computed as random numbers at the beginning of the program.
	gsl_vector *generatedV = gsl_vector_calloc(6);
	gsl_vector_set(generatedV, 0, gsl_vector_get(meanOfV,0)+v1Err); 
	gsl_vector_set(generatedV, 1, gsl_vector_get(meanOfV,1)+v2Err);
	gsl_vector_set(generatedV, 2, gsl_vector_get(meanOfV,2)+v3Err); 
	gsl_vector_set(generatedV, 3, gsl_vector_get(meanOfV,3)+v4Err);
	gsl_vector_set(generatedV, 4, gsl_vector_get(meanOfV,4)+v5Err); 
	gsl_vector_set(generatedV, 5,gsl_vector_get(meanOfV,5)+v6Err);

	double zeta; double sigma;
	zeta = -4.354999117683; sigma = 1.60818347313;
	*ariasInt = gsl_cdf_lognormal_Pinv(gsl_cdf_ugaussian_P(gsl_vector_get(generatedV,0)),zeta,sigma); //Arias Intensity

	double max=45.0; double min=5.0;
	double mD=(17.3-5.0)/40.0; double sD=9.31/(40.0); double aD=(gsl_pow_2(mD)-gsl_pow_3(mD)-mD*gsl_pow_2(sD))/(gsl_pow_2(sD)); double bD=aD*(1-mD)/mD;
	*effDur=gsl_cdf_beta_Pinv(gsl_cdf_ugaussian_P(gsl_vector_get(generatedV,1)),aD,bD) * (max-min) + min; //Effective Duration

	max=40.0; min=0.5;
	double mT=(12.4-0.5)/(max-min); double sT=7.44/(max-min); double aT=(gsl_pow_2(mT)-gsl_pow_3(mT)-mT*gsl_pow_2(sT))/(gsl_pow_2(sT)); double bT=aT*(1.0-mT)/mT;
	*midStrong=gsl_cdf_beta_Pinv(gsl_cdf_ugaussian_P(gsl_vector_get(generatedV,2)),aT,bT) * (max-min) + min; //time at the middle of the strong-motion phase

	double pi=M_PI;

	double mOM=5.87; double sOM=3.11; double aOM=gsl_pow_2(mOM)/gsl_pow_2(sOM); double bOM=gsl_pow_2(sOM)/mOM;
	*omegM = 2.0*pi*gsl_cdf_gamma_Pinv(gsl_cdf_ugaussian_P(gsl_vector_get(generatedV,3)),aOM,bOM); //Filter frequency at Tmid

	double omeg = gsl_cdf_ugaussian_P(gsl_vector_get(generatedV,4)); 
	if (omeg>((4.85/6.77)*(1-exp(-2.0*6.77))))						//Calculating the rate of change of the filter...
	{																//frequency with the time of application of the pulse
		*omegP=2.0*pi*log((omeg-((4.85/6.77)*(1-exp(-2.0*6.77)))-4.85/17.1*1.0)/(4.85/(-17.1)))/(-17.1);
	}
	else if (omeg<((4.85/6.77)*(1-exp(-2.0*6.77)))&&omeg>0.0)
	{
		*omegP=2.0*pi*log(((omeg+4.85/6.77*exp(-2.0*6.77))*6.77)/4.85)/6.77;
	}
	else
	{qCritical() << "Error: value is not in the bounds 0 & 1";
	return -1;
	}

	double mK=(0.213-0.02)/0.98; double sK=0.143/0.98; double aK=(gsl_pow_2(mK)-gsl_pow_3(mK)-mK*gsl_pow_2(sK))/(gsl_pow_2(sK)); double bK=aK*(1.0-mK)/mK; //damping
	*kis=gsl_cdf_beta_Pinv(gsl_cdf_ugaussian_P(gsl_vector_get(generatedV,5)),aK,bK) * 0.98 + 0.02;

	//Making memory free
	gsl_vector_free(meanOfV);
	gsl_vector_free(vectorOfRegressors);
	gsl_vector_free(generatedV);

	return 0;
}

//[gmb]=fn_getSim(tn,dt,I,D,Tmid,omegM,omegPr,kis);
int RRezaeianDerKiureghianArtificialGroundMotionModel::simulate(double dt, int accN, double ariasInt, double effDur, double midStrong, double omegP, double omegM, double kis, gsl_vector *whiteNoiseVec, gsl_vector *simulatedGroundMotion)
{
	double alfa2;
	double alfa3;
	int result = solve(midStrong, effDur, &alfa2, &alfa3);
	if (result  < 0) {
		qCritical() << "Error: The Alfa equation was not solved successfully";
		return -1;
	}
	double alfa1 = sqrt(ariasInt*(qPow((2*alfa3),(2*alfa2-1)))/gsl_sf_gamma(2*alfa2-1));
	
	double time = 0.00;
	double sigm = 0.00;
	double q = 0.0;

	//Here, this equation is being computed    x(t)=q(t,alfa)*SIGMA(Si(t,lambda(ti))*ui)  ref:Equation number 4, Rezaeian & Der Kureghian 2010
	for (int i = 2; i < (accN+1); i++) {
		time = (i)*dt;
		int result = sigmaSU(i, dt, time, midStrong, omegP, omegM, kis, whiteNoiseVec, &sigm);
		if (result < 0) {
			qCritical() << "Error: The sigma was not computed successfully";
			return -1;
		}
		q = alfa1*qPow(time,(alfa2-1))*exp(-1*alfa3*time);
		gsl_vector_set(simulatedGroundMotion, (i-1), q*sigm);
		
	}
	
	return 0;
}

//sigm=fn_sigmSU(i,dt,t,u,kis,omegM,omegPr,Tmid); %defining alfas
int RRezaeianDerKiureghianArtificialGroundMotionModel::sigmaSU(int m, double dT, double t, double tMid, double omegP, double omegM, double kis, gsl_vector *whiteNoiseVec, double *sigma)
{
	double sigmDenominator = 0.0;
	double omega = 0.0;
	double hFun = 0.0;
	int result;
	double sig=0.0;

	//Here, this equation is being computed		SIGMA(Si(t,lambda(ti))*ui)  ref:Equation number 4 Rezaeian 2010
	//Which Si is				Si(t,lambda(ti))=(h[t-ti,lambda(ti)])/sqrt(SIGMA((h[t-tj,lambda(tj)])^2))
	for (int n = 1; n < (m+1); n++) {
		omega = omegM+omegP*((n)*dT-tMid);
		result = pseudoAccResponseSDOF(t, (n)*dT, kis, omega, &hFun);
		sigmDenominator = sigmDenominator+gsl_pow_2(hFun);
	}

	if (result  < 0) {
		qCritical() << "Error: The pseudoAccResponseSDOF was not computed successfully when it is called in sigmaSU";
		return -1;
	}
	
	for (int k = 1; k < (m+1); k++) {
		omega = omegM+omegP*(k*dT-tMid);
		result = pseudoAccResponseSDOF(t, k*dT, kis, omega, &hFun);
		sig = sig+hFun/sqrt(sigmDenominator)*gsl_vector_get(whiteNoiseVec, (k-1));
	}
	*sigma=sig;
	if (result  < 0) {
		qCritical() << "Error: The pseudoAccResponseSDOF was not computed successfully when it is called in sigmaSU";
		return -1;
	}

	return 0;
}

//h=fn_h(t,ti,kis,omega)
int RRezaeianDerKiureghianArtificialGroundMotionModel::pseudoAccResponseSDOF(double totTime, double time,double kis,double omega, double *h)
{
	*h=omega/sqrt(1-gsl_pow_2(kis))*exp(-1*kis*omega*(totTime-time))*sin(omega*sqrt(1-gsl_pow_2(kis))*(totTime-time));

	return 0;
}

//gmbf=fn_hpf(dt,gmb,omeg)
int RRezaeianDerKiureghianArtificialGroundMotionModel::highPassFilter(int n, double dt, gsl_vector *simGM, double omC)
{
	double beta=0.25; 
	double gama=0.5;
	gsl_vector *temp = gsl_vector_calloc(n); //p vector
	int result = gsl_vector_memcpy(temp, simGM);
	if (result  < 0) {
		qCritical() << "Error: gsl_Error in High-pass filtering";
		return -1;
	}

	double ud0=0.0; 
	double u0=0.0; 
	double m=1.0; 
	double c=2.0*omC; 
	double k=gsl_pow_2(omC); 
	double udd0=0.0; //simGM=gsl_vector_calloc(n);
	gsl_vector *ud=gsl_vector_calloc(n); 
	gsl_vector *u=gsl_vector_calloc(n);
	gsl_vector_set(simGM, 0, udd0); //for making memory using less, I used simGM as udd
	gsl_vector_set(ud, 0, ud0); 
	gsl_vector_set(u, 0, u0);

	//a1=(1/beta/dt^2)*m+(gama/beta/dt)*c   a2=(1/beta/dt)*m+(gama/beta-1)*c   a3=(1/2/beta-1)*m+dt*(gama/2/beta-1)*c
	double a1=m/beta/(gsl_pow_2(dt))+gama*c/beta/dt; 
	double a2=m/beta/dt+(gama/beta-1.0)*c; 
	double a3=(1.0/2.0/beta-1.0)*m+dt*(gama/2.0/beta-1.0)*c;
	double p_hat=0.0;
	double k_hat=a1+k;
	for (int i = 0; i < n-1; i++) {
		//p_hat(i+1)=p(i+1)+a1*u(i)+a2*ud(i)+a3*udd(i);
		//u(i+1)=p_hat(i+1)/k_hat;
		//ud(i+1)=G/B/dt*(u(i+1)-u(i))+(1-G/B)*ud(i)+dt*(1-G/2/B)*udd(i);
		//udd(i+1)=1/B/(dt^2)*(u(i+1)-u(i))-1/B/dt*ud(i)-(1/2/B-1)*udd(i);
		p_hat=gsl_vector_get(temp, i+1)+a1*gsl_vector_get(u,i)+a2*gsl_vector_get(ud,i)+a3*gsl_vector_get(simGM,i);
		gsl_vector_set(u,(i+1),p_hat/k_hat);
		gsl_vector_set(ud,i+1,gama/beta/dt*(gsl_vector_get(u,i+1)-gsl_vector_get(u,i))+(1.0-gama/beta)*gsl_vector_get(ud,i)+dt*(1.0-gama/2.0/beta)*gsl_vector_get(simGM,i));
		gsl_vector_set(simGM,i+1,1.0/beta/(gsl_pow_2(dt))*(gsl_vector_get(u,i+1)-gsl_vector_get(u,i))-1.0/beta/dt*gsl_vector_get(ud,i)-(1.0/2.0/beta-1.0)*gsl_vector_get(simGM,i));
	}

	gsl_vector_free(temp);
	gsl_vector_free(ud);
	gsl_vector_free(u);

	return 0;
}

/*/struct rparams
{
double a;
double b;
};

int
alfaFinding_f (const gsl_vector * x, void *params, gsl_vector * f) {
double a = ((struct rparams *) params)->a;
double b = ((struct rparams *) params)->b;

double x0 = gsl_vector_get (x, 0);
double x1 = gsl_vector_get (x, 1);
double x2 = gsl_vector_get (x, 2);
//F=[icdf('gamma',0.95,2*x(1)-1,1/(2*x(2)))-icdf('gamma',0.05,2*x(1)-1,1/(2*x(2)))-D;
//icdf('gamma',0.45,2*x(1)-1,1/(2*x(2)))-Tmid];
//double y0 = gsl_cdf_gamma_Pinv(0.95,2*x0-1,1/(2*x1))-gsl_cdf_gamma_Pinv(0.05,2*x0-1,1/(2*x1))-b;
//double y1 = gsl_cdf_gamma_Pinv(0.45,2*x0-1,1/(2*x1))-a;
double y0 = gsl_cdf_gamma_P(x0, 2*x1-1, 1/(2*x2))-0.05;
double y1 = gsl_cdf_gamma_P(x0+b, 2*x1-1, 1/(2*x2))-gsl_cdf_gamma_P(x0, 2*x1-1, 1/(2*x2))-0.9;
double y2 = gsl_cdf_gamma_P(a, 2*x1-1, 1/(2*x2))-0.45;
//const double y0 = a * (1 - x0);
//const double y1 = b * (x1 - x0 * x0);

gsl_vector_set (f, 0, y0);
gsl_vector_set (f, 1, y1);
gsl_vector_set (f, 2, y2);

return GSL_SUCCESS;
}

int RRezaeianDerKiureghianArtificialGroundMotionModel::solve(double tMid, double duration, double *alf2, double *alf3)
{
const gsl_multiroot_fsolver_type *T;
gsl_multiroot_fsolver *s;

int status;
size_t i, iter = 0;

const size_t n = 3;
struct rparams p = {tMid, duration};
//struct rparams p = {1, 10};
gsl_multiroot_function f = {&alfaFinding_f, n, &p};

double x_init[3] = {4, 10, 10};
gsl_vector *x = gsl_vector_alloc (n);

gsl_vector_set (x, 0, x_init[0]);
gsl_vector_set (x, 1, x_init[1]);
gsl_vector_set (x, 2, x_init[2]);

T = gsl_multiroot_fsolver_hybrid;
//T = gsl_multiroot_fsolver_dnewton;
//T = gsl_multiroot_fsolver_broyden;
s = gsl_multiroot_fsolver_alloc (T, 3);
gsl_multiroot_fsolver_set(s, &f, x);

//print_state (iter, s);

do
{
iter++;
status = gsl_multiroot_fsolver_iterate (s);

//print_state (iter, s);

if (status)   /* check if solver is stuck /
break;

status = 
gsl_multiroot_test_residual (s->f, 1e-7);
}
while (status == GSL_CONTINUE && iter < 1000);
//status = gsl_multiroot_fsolver_iterate(s);
//gsl_multiroot_test_residual (s->f, 1e-7);
printf ("status = %s\n", gsl_strerror (status));
*alf2=gsl_vector_get (s->x, 1);
*alf3=gsl_vector_get (s->x, 2);


gsl_multiroot_fsolver_free (s);
gsl_vector_free (x);
return 0; 
}*/

struct rparams
{
	double a;
	double b;
	//double *sigma;
};

int
	alfaFinding_f (const gsl_vector * x, void *params, gsl_vector * f) {
		double a = ((struct rparams *) params)->a;
		double b = ((struct rparams *) params)->b;
		//double *sigma = ((struct rparams *) params)->sigma;

		double x0 = gsl_vector_get (x, 0);
		double x1 = gsl_vector_get (x, 1);

		//F=[icdf('gamma',0.95,2*x(1)-1,1/(2*x(2)))-icdf('gamma',0.05,2*x(1)-1,1/(2*x(2)))-D;
		//icdf('gamma',0.45,2*x(1)-1,1/(2*x(2)))-Tmid];
		double y0 = gsl_cdf_gamma_Pinv(0.95,2.0*x0-1.0,1.0/(2.0*x1))-gsl_cdf_gamma_Pinv(0.05,2.0*x0-1.0,1.0/(2.0*x1));
		double y1 = gsl_cdf_gamma_Pinv(0.45,2.0*x0-1.0,1.0/(2.0*x1));
		//const double y0 = a * (1 - x0);
		//const double y1 = b * (x1 - x0 * x0);

		gsl_vector_set (f, 0, (y0-b)/0.01);
		gsl_vector_set (f, 1, (y1-a)/0.01);

		return GSL_SUCCESS;
}

int
	alfaFinding_df (const gsl_vector * x, void *params,
	gsl_matrix * J)
{
	//double *sigma = ((struct rparams *) params)->sigma;
	double x0 = gsl_vector_get (x, 0);
	double x1 = gsl_vector_get (x, 1);

	gsl_matrix_set(J,0,0, (gsl_cdf_gamma_Pinv(0.95,2.0*(1.0001*x0)-1.0,1.0/(2.0*x1))-gsl_cdf_gamma_Pinv(0.05,2.0*(1.0001*x0)-1.0,1.0/(2.0*x1))-gsl_cdf_gamma_Pinv(0.95,2.0*x0-1.0,1.0/(2.0*x1))+gsl_cdf_gamma_Pinv(0.05,2.0*x0-1.0,1.0/(2.0*x1)))/(0.0001*x0*0.01));
	gsl_matrix_set(J,0,1, (gsl_cdf_gamma_Pinv(0.95,2.0*x0-1.0,1.0/(2.0*(1.0001*x1)))-gsl_cdf_gamma_Pinv(0.05,2.0*x0-1.0,1.0/(2.0*(1.0001*x1)))-gsl_cdf_gamma_Pinv(0.95,2.0*x0-1.0,1.0/(2.0*x1))+gsl_cdf_gamma_Pinv(0.05,2.0*x0-1.0,1.0/(2.0*x1)))/(0.0001*x1*0.01));
	gsl_matrix_set(J,1,0, (gsl_cdf_gamma_Pinv(0.45,2.0*(1.0001*x0)-1.0,1.0/(2.0*x1))-gsl_cdf_gamma_Pinv(0.45,2.0*x0-1.0,1.0/(2.0*x1)))/(0.0001*x0*0.01));
	gsl_matrix_set(J,1,1, (gsl_cdf_gamma_Pinv(0.45,2.0*x0-1.0,1.0/(2.0*(1.0001*x1)))-gsl_cdf_gamma_Pinv(0.45,2.0*x0-1.0,1.0/(2.0*x1)))/(0.0001*x1*0.01));

	return GSL_SUCCESS;
}

int
	alfaFinding_fdf (const gsl_vector * x, void *params,
	gsl_vector * f, gsl_matrix * J)
{
	alfaFinding_f (x, params, f);
	alfaFinding_df (x, params, J);
	return GSL_SUCCESS;
}

//void print_state (size_t iter, gsl_multifit_fdfsolver * s);


int RRezaeianDerKiureghianArtificialGroundMotionModel::solve(double tMid, double duration, double *alf2, double *alf3)
{

	const gsl_multifit_fdfsolver_type *T;
	gsl_multifit_fdfsolver *s;
	int status;
	size_t i, iter = 0;
	const size_t n = 2;
	gsl_matrix *covar = gsl_matrix_alloc (n, n);

	//double sigma[n];
	struct rparams p = {tMid, duration};
	gsl_multifit_function_fdf f;
	double x_init[2] = { 1.0, 1.0};
	gsl_vector_view x = gsl_vector_view_array (x_init, n);
	const gsl_rng_type * type;
	gsl_rng * r;

	gsl_rng_env_setup();

	type = gsl_rng_default;
	r = gsl_rng_alloc (type);

	f.f = &alfaFinding_f;
	f.df = &alfaFinding_df;
	f.fdf = &alfaFinding_fdf;
	f.n = n;
	f.p = n;
	f.params = &p;

	T = gsl_multifit_fdfsolver_lmsder;
	s = gsl_multifit_fdfsolver_alloc (T, n, n);
	gsl_multifit_fdfsolver_set (s, &f, &x.vector);
	//print_state (iter, s);

	do
	{
		iter++;
		status = gsl_multifit_fdfsolver_iterate (s);
		printf ("status = %s\n", gsl_strerror (status));
		//print_state (iter, s);
		if (status)
			break;
		status = gsl_multifit_test_delta (s->dx, s->x,
			1e-4, 1e-4);
	}
	while (status == GSL_CONTINUE && iter < 500);

	//printf ("status = %s\n", gsl_strerror (status));
	*alf2=gsl_vector_get (s->x, 0);
	*alf3=gsl_vector_get (s->x, 1);


	gsl_matrix_free(covar);
	gsl_multifit_fdfsolver_free(s);
	gsl_rng_free(r);
	return 0; 
};

bool RRezaeianDerKiureghianArtificialGroundMotionModel::canBeRunParallelly() {	
	return false;
}