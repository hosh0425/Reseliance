#include "RSoilStructureInteractionModel.h"

#include "RParameter.h"
#include "RResponse.h"
#include "RRandomVariable.h"
#include "RMainWindow.h"


#include <QFile>
#include <QVector>
#include <QRegExp>
#include <QtCore/qmath.h>
#include <QCoreApplication>

#include <gsl/gsl_eigen.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_roots.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_complex.h>
#include <gsl/gsl_complex_math.h>
#include <gsl/gsl_fft_complex.h>
#include <gsl/gsl_fft_real.h>
#include <gsl/gsl_fft_halfcomplex.h>

#include <math.h>
#include <cmath>
#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

RSoilStructureInteractionModel::RSoilStructureInteractionModel(QObject *parent, QString name)
: RModel(parent, name)
{
	// Initializing the podoubleer data members
	// theFixedBasePeriod = 0;
	theNumberOfStories = 1;
	//theStructureDamping = 0;
	//theSoilDamping = 0;
	//thePoissonRatio  = 0;
	//theAspectRatio = 0;
	//theLastStoryStiffnessRatioIntercept = 0;
	//theLastStoryStiffnessRatioSlope = 0;
	//theStiffnessVariationExponent = 0;
	theRadius = 1;
	theSsASCE=1.25;	  //LOCATION=Los Angeles, CA, USA
	theS1ASCE=0.6;	//LOCATION=Los Angeles, CA, USA
	theOverStrength =1;
	theOverStrengthNEHRP =1;
	//theEmbedmentRatio = 0;
	//theStructureToSoilMassRatio = 0;
	//theFoundationToStructureMassRatio = 0;
	//theFoundationCentroidRatio = 0;
	//theDimensionlessFrequencyOrShearWaveVelocity = 0;
	//theStrengthReductionFactorOrTargetDuctility = 0;
	//theRecordSelectionUncertainty = 0;
	//theTargetDuctility = 0;
	//theIntensityMeasure=0;
	theScaleFactorLowerBound = 0.5;
	theScaleFactorUpperBound = 2;

	theMDOFOrSDOF = RSoilStructureInteractionModel::SDOF;
	//************************************************************** Runmode 5
	/*theSoftOrStiff = RSoilStructureInteractionModel::Soft;
	theCoefficientTeta1 = 0;
	theCoefficientTeta2 = 0;
	theCoefficientTeta3 = 0;
	theCoefficientTeta4 = 0;*/
	//**************************************************************
	//theProgramComputesNumberOfStories= RSoilStructureInteractionModel::No;
	theSSISeverityMeasure = RSoilStructureInteractionModel::DimensionlessFrequency;
	theSoilType = RSoilStructureInteractionModel::SoilTypeD;
	theApplyShearWaveVelocityDegradationFactor = true;
	theShearDistributionMethod = RSoilStructureInteractionModel::ASCE7;
	theRunMode = RSoilStructureInteractionModel::GivenRSameR;
	theRecordScalingMethod = RSoilStructureInteractionModel::PGA;
	theRecordScaling = RSoilStructureInteractionModel::Adaptive;

	// Instantiating the output responses 
	// Setting Responses podoubleer 
	// Setting the "model" property of Responses to be "this" object


	theDomain->createObject(objectName() + "T_SSIResponse", "RGenericResponse");
	theT_SSIResponse = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theT_SSIResponse->setModel(this);

	theDomain->createObject(objectName() + "VhatRatioResponse", "RGenericResponse");
	theVhatRatioResponse = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theVhatRatioResponse->setModel(this);

	theDomain->createObject(objectName() + "ElasticDisplacementResponse", "RGenericResponse");
	theElasticDisplacementResponse = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theElasticDisplacementResponse->setModel(this);

	theDomain->createObject(objectName() + "ElasticAccelarationResponse", "RGenericResponse");
	theElasticAccelarationResponse = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theElasticAccelarationResponse->setModel(this);

	theDomain->createObject(objectName() + "ElasticDisplacement_IntensityResponse", "RGenericResponse");
	theElasticDisplacement_IntensityResponse = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theElasticDisplacement_IntensityResponse->setModel(this);

	theDomain->createObject(objectName() + "ElasticAccelaration_IntensityResponse", "RGenericResponse");
	theElasticAccelaration_IntensityResponse = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theElasticAccelaration_IntensityResponse->setModel(this);

	theDomain->createObject(objectName() + "TotalElasticDisplacementResponse", "RGenericResponse");
	theTotalElasticDisplacementResponse = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theTotalElasticDisplacementResponse->setModel(this);

	theDomain->createObject(objectName() + "TotalElasticAccelarationResponse", "RGenericResponse");
	theTotalElasticAccelarationResponse = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theTotalElasticAccelarationResponse->setModel(this);

	theDomain->createObject(objectName() + "TotalElasticDisplacement_IntensityResponse", "RGenericResponse");
	theTotalElasticDisplacement_IntensityResponse = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theTotalElasticDisplacement_IntensityResponse->setModel(this);

	theDomain->createObject(objectName() + "TotalElasticAccelaration_IntensityResponse", "RGenericResponse");
	theTotalElasticAccelaration_IntensityResponse = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theTotalElasticAccelaration_IntensityResponse->setModel(this);

	theDomain->createObject(objectName() + "InelasticDisplacementResponse", "RGenericResponse");
	theInelasticDisplacementResponse = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theInelasticDisplacementResponse->setModel(this);

	theDomain->createObject(objectName() + "InelasticAccelarationResponse", "RGenericResponse");
	theInelasticAccelarationResponse = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theInelasticAccelarationResponse->setModel(this);

	theDomain->createObject(objectName() + "InelasticDisplacementResponse_Intensity", "RGenericResponse");
	theInelasticDisplacement_IntensityResponse = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theInelasticDisplacement_IntensityResponse->setModel(this);

	theDomain->createObject(objectName() + "InelasticAccelarationResponse_Intensity", "RGenericResponse");
	theInelasticAccelaration_IntensityResponse = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theInelasticAccelaration_IntensityResponse->setModel(this);

	theDomain->createObject(objectName() + "TotalInelasticDisplacementResponse", "RGenericResponse");
	theTotalInelasticDisplacementResponse= qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theTotalInelasticDisplacementResponse->setModel(this);

	theDomain->createObject(objectName() + "TotalInelasticAccelarationResponse", "RGenericResponse");
	theTotalInelasticAccelarationResponse= qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theTotalInelasticAccelarationResponse->setModel(this);

	theDomain->createObject(objectName() + "TotalInelasticDisplacementResponse_Intensity", "RGenericResponse");
	theTotalInelasticDisplacement_IntensityResponse= qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theTotalInelasticDisplacement_IntensityResponse->setModel(this);

	theDomain->createObject(objectName() + "TotalInelasticAccelarationResponse_Intensity", "RGenericResponse");
	theTotalInelasticAccelaration_IntensityResponse= qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theTotalInelasticAccelaration_IntensityResponse->setModel(this);

	theDomain->createObject(objectName() + "DuctilityResponse", "RGenericResponse");
	theDuctilityResponse= qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theDuctilityResponse->setModel(this);

	theDomain->createObject(objectName() + "SystemDuctilityResponse", "RGenericResponse");
	theSystemDuctilityResponse= qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theSystemDuctilityResponse->setModel(this);

	theDomain->createObject(objectName() + "FixedDuctilityResponse", "RGenericResponse");
	theFixedDuctilityResponse= qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theFixedDuctilityResponse->setModel(this);

	theDomain->createObject(objectName() + "FixedTotalAccelarationResponse", "RGenericResponse");
	theFixedTotalAccelarationResponse= qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theFixedTotalAccelarationResponse->setModel(this);

	theDomain->createObject(objectName() + "FixedDisplacementResponse", "RGenericResponse");
	theFixedDisplacementResponse= qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theFixedDisplacementResponse->setModel(this);

	theDomain->createObject(objectName() + "FixedDesignDuctilityResponse", "RGenericResponse");
	theFixedDesignDuctilityResponse= qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theFixedDesignDuctilityResponse->setModel(this);

	theDomain->createObject(objectName() + "FixedDesignTotalAccelarationResponse", "RGenericResponse");
	theFixedDesignTotalAccelarationResponse= qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theFixedDesignTotalAccelarationResponse->setModel(this);

	theDomain->createObject(objectName() + "FixedDesignDisplacementResponse", "RGenericResponse");
	theFixedDesignDisplacementResponse= qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theFixedDesignDisplacementResponse->setModel(this);


	theDomain->createObject(objectName() + "SpringYieldForce_mIntensityResponse", "RGenericResponse");
	theSpringYieldForce_mIntensityResponse= qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theSpringYieldForce_mIntensityResponse->setModel(this);

	theDomain->createObject(objectName() + "ElasticSpringForce_mIntensityResponse", "RGenericResponse");
	theElasticSpringForce_mIntensityResponse= qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theElasticSpringForce_mIntensityResponse->setModel(this);

	theDomain->createObject(objectName() + "FixedStrengthReductionFactorResponse", "RGenericResponse");
	theFixedStrengthReductionFactorResponse= qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theFixedStrengthReductionFactorResponse->setModel(this);

	theDomain->createObject(objectName() + "SSIStrengthReductionFactorResponse", "RGenericResponse");
	theSSIStrengthReductionFactorResponse= qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theSSIStrengthReductionFactorResponse->setModel(this);

	theDomain->createObject(objectName() + "MaximumDuctilityResponse", "RGenericResponse");
	theMaximumDuctilityResponse= qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theMaximumDuctilityResponse->setModel(this);

	theDomain->createObject(objectName() + "MaximumDuctilityStoryResponse", "RGenericResponse");
	theMaximumDuctilityStoryResponse= qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theMaximumDuctilityStoryResponse->setModel(this);

	theDomain->createObject(objectName() + "FixedMaximumDuctilityResponse", "RGenericResponse");
	theFixedMaximumDuctilityResponse= qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theFixedMaximumDuctilityResponse->setModel(this);

	theDomain->createObject(objectName() + "FixedMaximumDuctilityStoryResponse", "RGenericResponse");
	theFixedMaximumDuctilityStoryResponse= qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theFixedMaximumDuctilityStoryResponse->setModel(this);

	theDomain->createObject(objectName() + "FixedStrengthReductionFactorMDOFResponse", "RGenericResponse");
	theFixedStrengthReductionFactorMDOFResponse= qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theFixedStrengthReductionFactorMDOFResponse->setModel(this);

	theDomain->createObject(objectName() + "SSIStrengthReductionFactorMDOFResponse", "RGenericResponse");
	theSSIStrengthReductionFactorMDOFResponse= qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theSSIStrengthReductionFactorMDOFResponse->setModel(this);

	theDomain->createObject(objectName() + "FixedStrengthReductionFactorSDOFResponse", "RGenericResponse");
	theFixedStrengthReductionFactorSDOFResponse= qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theFixedStrengthReductionFactorSDOFResponse->setModel(this);

	theDomain->createObject(objectName() + "SSIStrengthReductionFactorSDOFResponse", "RGenericResponse");
	theSSIStrengthReductionFactorSDOFResponse= qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theSSIStrengthReductionFactorSDOFResponse->setModel(this);

	theDomain->createObject(objectName() + "SSIModificationFactorAlpha_RResponse", "RGenericResponse");
	theSSIModificationFactorAlpha_RResponse= qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theSSIModificationFactorAlpha_RResponse->setModel(this);

	theDomain->createObject(objectName() + "FixedModificationFactorAlpha_RResponse", "RGenericResponse");
	theFixedModificationFactorAlpha_RResponse= qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theFixedModificationFactorAlpha_RResponse->setModel(this);

	theDomain->createObject(objectName() + "SSIModificationFactorAlpha_VResponse", "RGenericResponse");
	theSSIModificationFactorAlpha_VResponse= qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theSSIModificationFactorAlpha_VResponse->setModel(this);

	theDomain->createObject(objectName() + "FixedModificationFactorAlpha_VResponse", "RGenericResponse");
	theFixedModificationFactorAlpha_VResponse= qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theFixedModificationFactorAlpha_VResponse->setModel(this);

	theDomain->createObject(objectName() + "FixedElasticBaseShearRatioResponse", "RGenericResponse");
	theFixedElasticBaseShearRatioResponse= qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theFixedElasticBaseShearRatioResponse->setModel(this);
	
	theDomain->createObject(objectName() + "SSIElasticBaseShearRatioResponse", "RGenericResponse");
	theSSIElasticBaseShearRatioResponse= qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theSSIElasticBaseShearRatioResponse->setModel(this);

	theDomain->createObject(objectName() + "MaximumDuctilityMDOFfyfixSDOFResponse", "RGenericResponse");
	theMaximumDuctilityMDOFfyfixSDOFResponse= qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theMaximumDuctilityMDOFfyfixSDOFResponse->setModel(this);

	theDomain->createObject(objectName() + "MaximumDuctilityStoryMDOFfyfixSDOFResponse", "RGenericResponse");
	theMaximumDuctilityStoryMDOFfyfixSDOFResponse= qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theMaximumDuctilityStoryMDOFfyfixSDOFResponse->setModel(this);

	theDomain->createObject(objectName() + "DuctilitySDOFfyfixSDOFResponse", "RGenericResponse");
	theDuctilitySDOFfyfixSDOFResponse= qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theDuctilitySDOFfyfixSDOFResponse->setModel(this);

	theDomain->createObject(objectName() + "SSIStrengthReductionFactorSDOFfyfixSDOFResponse", "RGenericResponse");
	theSSIStrengthReductionFactorSDOFfyfixSDOFResponse= qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theSSIStrengthReductionFactorSDOFfyfixSDOFResponse->setModel(this);
}

RSoilStructureInteractionModel::~RSoilStructureInteractionModel()
{
}
QObject * RSoilStructureInteractionModel::getFixedBasePeriod() const	
{	
	return theFixedBasePeriod.data();
}	
void RSoilStructureInteractionModel::setFixedBasePeriod(QObject *value)	
{	
	theFixedBasePeriod = value;
}	
//****************************************** Runmode 5
/*
RSoilStructureInteractionModel::RSoftOrStiff RSoilStructureInteractionModel::getSoftOrStiff() const
{
	return theSoftOrStiff;
}
void RSoilStructureInteractionModel::setSoftOrStiff(RSoilStructureInteractionModel::RSoftOrStiff value)
{
	theSoftOrStiff = value;
}

QObject * RSoilStructureInteractionModel::getCoefficientTeta1() const	
{	
	return theCoefficientTeta1;
}	
void RSoilStructureInteractionModel::setCoefficientTeta1(QObject *value)	
{	
	theCoefficientTeta1 = qobject_cast<RParameter *>(value);
}	
QObject * RSoilStructureInteractionModel::getCoefficientTeta2() const	
{	
	return theCoefficientTeta2;
}	
void RSoilStructureInteractionModel::setCoefficientTeta2(QObject *value)	
{	
	theCoefficientTeta2 = qobject_cast<RParameter *>(value);
}
QObject * RSoilStructureInteractionModel::getCoefficientTeta3() const	
{	
	return theCoefficientTeta3;
}	
void RSoilStructureInteractionModel::setCoefficientTeta3(QObject *value)	
{	
	theCoefficientTeta3 = qobject_cast<RParameter *>(value);
}
QObject * RSoilStructureInteractionModel::getCoefficientTeta4() const	
{	
	return theCoefficientTeta4;
}	
void RSoilStructureInteractionModel::setCoefficientTeta4(QObject *value)	
{	
	theCoefficientTeta4 = qobject_cast<RParameter *>(value);
}
//******************************************
*/
int RSoilStructureInteractionModel::getNumberOfStories() const	
{	
	return theNumberOfStories;
}	

void RSoilStructureInteractionModel::setNumberOfStories(int value)	
{	
	theNumberOfStories = value;
	
	int numStories = 0;
	if (theNumberOfStories) {
		numStories = theNumberOfStories;
	}

	//------------------------------------------------------ Responses of MDOF buildings for each story
	//Ductility_MDOF
	// Removing old responses
	while (theductility_MDOFResponseList.count() > 0) {
		delete theductility_MDOFResponseList.takeAt(0);
	}

	RMainWindow *mainWindow = qobject_cast<RMainWindow *>(parent()->parent());

	if (numStories <= 1) {
		// Update the domain tree
		if (mainWindow) {
			mainWindow->updateTree("Domain");
		}
		return;
	}
	
	// Instantiating the response objects one for each story
	for (int i = 0; i < numStories; i++) {
		theDomain->createObject(objectName() + QString("Story%1").arg(i+1) + "DuctilityResponse", "RGenericResponse");
		RResponse *response = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
		response->setModel(this);
		theductility_MDOFResponseList << response;

		// A line to keep the GUI responsive
		if (i % 10 == 0) {
			QCoreApplication::processEvents();
		}
	}

	// Update the domain tree
	if (mainWindow) {
		mainWindow->updateTree("Domain");
	}

	//Ductility_MDOF2
	// Removing old responses
	while (theductility_MDOF2ResponseList.count() > 0) {
		delete theductility_MDOF2ResponseList.takeAt(0);
	}

	if (numStories <= 1) {
		// Update the domain tree
		if (mainWindow) {
			mainWindow->updateTree("Domain");
		}
		return;
	}
	
	// Instantiating the response objects one for each story
	for (int i = 0; i < numStories; i++) {
		theDomain->createObject(objectName() + QString("Story%1").arg(i+1) + "DuctilityMDOFfyfixSDOFResponse", "RGenericResponse");
		RResponse *response = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
		response->setModel(this);
		theductility_MDOF2ResponseList << response;

		// A line to keep the GUI responsive
		if (i % 10 == 0) {
			QCoreApplication::processEvents();
		}
	}

	// Update the domain tree
	if (mainWindow) {
		mainWindow->updateTree("Domain");
	}

	//Dessi_MDOF
	// Removing old responses
	while (theDessi_MDOFResponseList.count() > 0) {
		delete theDessi_MDOFResponseList.takeAt(0);
	}

	if (numStories <= 1) {
		// Update the domain tree
		if (mainWindow) {
			mainWindow->updateTree("Domain");
		}
		return;
	}
	
	// Instantiating the response objects one for each story
	for (int i = 0; i < numStories; i++) {
		theDomain->createObject(objectName() + QString("Story%1").arg(i+1) + "ElasticDisplacementResponse", "RGenericResponse");
		RResponse *response = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
		response->setModel(this);
		theDessi_MDOFResponseList << response;

		// A line to keep the GUI responsive
		if (i % 10 == 0) {
			QCoreApplication::processEvents();
		}
	}

	// Update the domain tree
	if (mainWindow) {
		mainWindow->updateTree("Domain");
	}

	//Aessi_MDOF
	// Removing old responses
	while (theAessi_MDOFResponseList.count() > 0) {
		delete theAessi_MDOFResponseList.takeAt(0);
	}


	if (numStories <= 1) {
		// Update the domain tree
		if (mainWindow) {
			mainWindow->updateTree("Domain");
		}
		return;
	}
	
	// Instantiating the response objects one for each story
	for (int i = 0; i < numStories; i++) {
		theDomain->createObject(objectName() + QString("Story%1").arg(i+1) + "ElasticAccelarationResponse", "RGenericResponse");
		RResponse *response = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
		response->setModel(this);
		theAessi_MDOFResponseList << response;

		// A line to keep the GUI responsive
		if (i % 10 == 0) {
			QCoreApplication::processEvents();
		}
	}

	// Update the domain tree
	if (mainWindow) {
		mainWindow->updateTree("Domain");
	}

	//De_tot_MDOF
	// Removing old responses
	while (theDe_tot_MDOFResponseList.count() > 0) {
		delete theDe_tot_MDOFResponseList.takeAt(0);
	}


	if (numStories <= 1) {
		// Update the domain tree
		if (mainWindow) {
			mainWindow->updateTree("Domain");
		}
		return;
	}
	
	// Instantiating the response objects one for each story
	for (int i = 0; i < numStories; i++) {
		theDomain->createObject(objectName() + QString("Story%1").arg(i+1) + "TotalElasticDisplacementResponse", "RGenericResponse");
		RResponse *response = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
		response->setModel(this);
		theDe_tot_MDOFResponseList << response;

		// A line to keep the GUI responsive
		if (i % 10 == 0) {
			QCoreApplication::processEvents();
		}
	}

	// Update the domain tree
	if (mainWindow) {
		mainWindow->updateTree("Domain");
	}

	//Ae_tot_MDOF
	// Removing old responses
	while (theAe_tot_MDOFResponseList.count() > 0) {
		delete theAe_tot_MDOFResponseList.takeAt(0);
	}


	if (numStories <= 1) {
		// Update the domain tree
		if (mainWindow) {
			mainWindow->updateTree("Domain");
		}
		return;
	}
	
	// Instantiating the response objects one for each story
	for (int i = 0; i < numStories; i++) {
		theDomain->createObject(objectName() + QString("Story%1").arg(i+1) + "TotalElasticAccelarationResponse", "RGenericResponse");
		RResponse *response = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
		response->setModel(this);
		theAe_tot_MDOFResponseList << response;

		// A line to keep the GUI responsive
		if (i % 10 == 0) {
			QCoreApplication::processEvents();
		}
	}

	// Update the domain tree
	if (mainWindow) {
		mainWindow->updateTree("Domain");
	}

	//Dm_MDOF
	// Removing old responses
	while (theDm_MDOFResponseList.count() > 0) {
		delete theDm_MDOFResponseList.takeAt(0);
	}


	if (numStories <= 1) {
		// Update the domain tree
		if (mainWindow) {
			mainWindow->updateTree("Domain");
		}
		return;
	}
	
	// Instantiating the response objects one for each story
	for (int i = 0; i < numStories; i++) {
		theDomain->createObject(objectName() + QString("Story%1").arg(i+1) + "InelasticDisplacementResponse", "RGenericResponse");
		RResponse *response = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
		response->setModel(this);
		theDm_MDOFResponseList << response;

		// A line to keep the GUI responsive
		if (i % 10 == 0) {
			QCoreApplication::processEvents();
		}
	}

	// Update the domain tree
	if (mainWindow) {
		mainWindow->updateTree("Domain");
	}

	//Am_MDOF
	// Removing old responses
	while (theAm_MDOFResponseList.count() > 0) {
		delete theAm_MDOFResponseList.takeAt(0);
	}


	if (numStories <= 1) {
		// Update the domain tree
		if (mainWindow) {
			mainWindow->updateTree("Domain");
		}
		return;
	}
	
	// Instantiating the response objects one for each story
	for (int i = 0; i < numStories; i++) {
		theDomain->createObject(objectName() + QString("Story%1").arg(i+1) + "InelasticAccelarationResponse", "RGenericResponse");
		RResponse *response = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
		response->setModel(this);
		theAm_MDOFResponseList << response;

		// A line to keep the GUI responsive
		if (i % 10 == 0) {
			QCoreApplication::processEvents();
		}
	}

	// Update the domain tree
	if (mainWindow) {
		mainWindow->updateTree("Domain");
	}

	//Dm_tot_MDOF
	// Removing old responses
	while (theDm_tot_MDOFResponseList.count() > 0) {
		delete theDm_tot_MDOFResponseList.takeAt(0);
	}


	if (numStories <= 1) {
		// Update the domain tree
		if (mainWindow) {
			mainWindow->updateTree("Domain");
		}
		return;
	}
	
	// Instantiating the response objects one for each story
	for (int i = 0; i < numStories; i++) {
		theDomain->createObject(objectName() + QString("Story%1").arg(i+1) + "TotalInelasticDisplacementResponse", "RGenericResponse");
		RResponse *response = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
		response->setModel(this);
		theDm_tot_MDOFResponseList << response;

		// A line to keep the GUI responsive
		if (i % 10 == 0) {
			QCoreApplication::processEvents();
		}
	}

	// Update the domain tree
	if (mainWindow) {
		mainWindow->updateTree("Domain");
	}

	//Am_tot_MDOF
	// Removing old responses
	while (theAm_tot_MDOFResponseList.count() > 0) {
		delete theAm_tot_MDOFResponseList.takeAt(0);
	}


	if (numStories <= 1) {
		// Update the domain tree
		if (mainWindow) {
			mainWindow->updateTree("Domain");
		}
		return;
	}
	
	// Instantiating the response objects one for each story
	for (int i = 0; i < numStories; i++) {
		theDomain->createObject(objectName() + QString("Story%1").arg(i+1) + "TotalInelasticAccelarationResponse", "RGenericResponse");
		RResponse *response = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
		response->setModel(this);
		theAm_tot_MDOFResponseList << response;

		// A line to keep the GUI responsive
		if (i % 10 == 0) {
			QCoreApplication::processEvents();
		}
	}

	// Update the domain tree
	if (mainWindow) {
		mainWindow->updateTree("Domain");
	}

	//FixedDuctility_MDOF
	// Removing old responses
	while (theFixedDuctility_MDOFResponseList.count() > 0) {
		delete theFixedDuctility_MDOFResponseList.takeAt(0);
	}


	if (numStories <= 1) {
		// Update the domain tree
		if (mainWindow) {
			mainWindow->updateTree("Domain");
		}
		return;
	}
	
	// Instantiating the response objects one for each story
	for (int i = 0; i < numStories; i++) {
		theDomain->createObject(objectName() + QString("Story%1").arg(i+1) + "FixedDuctilityResponse", "RGenericResponse");
		RResponse *response = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
		response->setModel(this);
		theFixedDuctility_MDOFResponseList << response;

		// A line to keep the GUI responsive
		if (i % 10 == 0) {
			QCoreApplication::processEvents();
		}
	}

	// Update the domain tree
	if (mainWindow) {
		mainWindow->updateTree("Domain");
	}

	//Dm_fix_MDOF
	// Removing old responses
	while (theDm_fix_MDOFResponseList.count() > 0) {
		delete theDm_fix_MDOFResponseList.takeAt(0);
	}


	if (numStories <= 1) {
		// Update the domain tree
		if (mainWindow) {
			mainWindow->updateTree("Domain");
		}
		return;
	}
	
	// Instantiating the response objects one for each story
	for (int i = 0; i < numStories; i++) {
		theDomain->createObject(objectName() + QString("Story%1").arg(i+1) + "FixedInelasticDisplacementResponse", "RGenericResponse");
		RResponse *response = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
		response->setModel(this);
		theDm_fix_MDOFResponseList << response;

		// A line to keep the GUI responsive
		if (i % 10 == 0) {
			QCoreApplication::processEvents();
		}
	}

	// Update the domain tree
	if (mainWindow) {
		mainWindow->updateTree("Domain");
	}

	//Am_tot_fix_MDOF
	// Removing old responses
	while (theAm_tot_fix_MDOFResponseList.count() > 0) {
		delete theAm_tot_fix_MDOFResponseList.takeAt(0);
	}


	if (numStories <= 1) {
		// Update the domain tree
		if (mainWindow) {
			mainWindow->updateTree("Domain");
		}
		return;
	}
	
	// Instantiating the response objects one for each story
	for (int i = 0; i < numStories; i++) {
		theDomain->createObject(objectName() + QString("Story%1").arg(i+1) + "FixedTotalInelasticAccelerationResponse", "RGenericResponse");
		RResponse *response = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
		response->setModel(this);
		theAm_tot_fix_MDOFResponseList << response;

		// A line to keep the GUI responsive
		if (i % 10 == 0) {
			QCoreApplication::processEvents();
		}
	}

	// Update the domain tree
	if (mainWindow) {
		mainWindow->updateTree("Domain");
	}

}	

RSoilStructureInteractionModel::RMDOFOrSDOF RSoilStructureInteractionModel::getMDOFOrSDOF() const
{
	return theMDOFOrSDOF;
}
void RSoilStructureInteractionModel::setMDOFOrSDOF(RSoilStructureInteractionModel::RMDOFOrSDOF value)
{
	theMDOFOrSDOF = value;
}
/*RSoilStructureInteractionModel::RProgramComputesNumberOfStories RSoilStructureInteractionModel::getProgramComputesNumberOfStories() const	
{	
	return theProgramComputesNumberOfStories;
}		
void RSoilStructureInteractionModel::setProgramComputesNumberOfStories(RSoilStructureInteractionModel::RProgramComputesNumberOfStories value)	
{	
	theProgramComputesNumberOfStories = value;
}*/	
QObject * RSoilStructureInteractionModel::getStructureDamping() const	
{	
	return theStructureDamping.data();
}	
void RSoilStructureInteractionModel::setStructureDamping(QObject *value)	
{	
	theStructureDamping = value;
}	
QObject * RSoilStructureInteractionModel::getSoilDamping() const	
{	
	return theSoilDamping.data();
}	
void RSoilStructureInteractionModel::setSoilDamping(QObject *value)	
{	
	theSoilDamping = qobject_cast<RParameter *>(value);
}	
QObject * RSoilStructureInteractionModel::getPoissonRatio () const	
{	
	return thePoissonRatio.data() ;
}	
void RSoilStructureInteractionModel::setPoissonRatio (QObject *value)	
{	
	thePoissonRatio  = value;
}	
QObject * RSoilStructureInteractionModel::getAspectRatio() const	
{	
	return theAspectRatio.data();
}	
void RSoilStructureInteractionModel::setAspectRatio(QObject *value)	
{	
	theAspectRatio = qobject_cast<RParameter *>(value);
}	
double RSoilStructureInteractionModel::getRadius() const	
{	
	return theRadius;
}	

void RSoilStructureInteractionModel::setRadius(double value)	
{	
	theRadius = value;
}	

QObject * RSoilStructureInteractionModel::getEmbedmentRatio() const	
{	
	return theEmbedmentRatio.data();
}	
void RSoilStructureInteractionModel::setEmbedmentRatio(QObject *value)	
{	
	theEmbedmentRatio = qobject_cast<RParameter *>(value);
}	
QObject * RSoilStructureInteractionModel::getStructureToSoilMassRatio() const	
{	
	return theStructureToSoilMassRatio.data();
}
void RSoilStructureInteractionModel::setStructureToSoilMassRatio(QObject *value)	
{	
	theStructureToSoilMassRatio = qobject_cast<RParameter *>(value);
}	
QObject * RSoilStructureInteractionModel::getFoundationToStructureMassRatio() const	
{	
	return theFoundationToStructureMassRatio.data();
}
void RSoilStructureInteractionModel::setFoundationToStructureMassRatio(QObject *value)	
{	
	theFoundationToStructureMassRatio = qobject_cast<RParameter *>(value);
}	

QObject * RSoilStructureInteractionModel::getFoundationCentroidRatio() const	
{	
	return theFoundationCentroidRatio.data();
}
void RSoilStructureInteractionModel::setFoundationCentroidRatio(QObject *value)	
{	
	theFoundationCentroidRatio = qobject_cast<RParameter *>(value);
}	

/*QObject * RSoilStructureInteractionModel::getLastStoryStiffnessRatioIntercept() const
{
	return theLastStoryStiffnessRatioIntercept;
}
void RSoilStructureInteractionModel::setLastStoryStiffnessRatioIntercept(QObject *value)
{
	theLastStoryStiffnessRatioIntercept = qobject_cast<RParameter *>(value);
}
QObject * RSoilStructureInteractionModel::getLastStoryStiffnessRatioSlope() const
{
	return theLastStoryStiffnessRatioSlope;
}
void RSoilStructureInteractionModel::setLastStoryStiffnessRatioSlope(QObject *value)
{
	theLastStoryStiffnessRatioSlope = qobject_cast<RParameter *>(value);
}
QObject * RSoilStructureInteractionModel::getStiffnessVariationExponent() const
{
	return theStiffnessVariationExponent;
}
void RSoilStructureInteractionModel::setStiffnessVariationExponent(QObject *value)
{
	theStiffnessVariationExponent = qobject_cast<RParameter *>(value);
}*/

QObject * RSoilStructureInteractionModel::getRecordSelectionUncertainty() const	
{	
	return theRecordSelectionUncertainty.data();
}
void RSoilStructureInteractionModel::setRecordSelectionUncertainty(QObject *value)	
{	
	theRecordSelectionUncertainty = qobject_cast<RParameter *>(value);
}	
RSoilStructureInteractionModel::RSSISeverityMeasure RSoilStructureInteractionModel::getSSISeverityMeasure() const	
{	
	return theSSISeverityMeasure;
}		
void RSoilStructureInteractionModel::setSSISeverityMeasure(RSoilStructureInteractionModel::RSSISeverityMeasure value)	
{	
	theSSISeverityMeasure = value;
}	

RSoilStructureInteractionModel::RSoilType RSoilStructureInteractionModel::getSoilType() const	
{	
	return theSoilType;
}		
void RSoilStructureInteractionModel::setSoilType(RSoilStructureInteractionModel::RSoilType value)	
{	
	theSoilType = value;
}	
bool RSoilStructureInteractionModel::getApplyShearWaveVelocityDegradationFactor()
{
	return theApplyShearWaveVelocityDegradationFactor;
}

void RSoilStructureInteractionModel::setApplyShearWaveVelocityDegradationFactor(bool value)
{
	theApplyShearWaveVelocityDegradationFactor = value;
}

QObject * RSoilStructureInteractionModel::getDimensionlessFrequencyOrShearWaveVelocity() const	
{	
	return theDimensionlessFrequencyOrShearWaveVelocity.data();
}	
void RSoilStructureInteractionModel::setDimensionlessFrequencyOrShearWaveVelocity(QObject *value)	
{	
	theDimensionlessFrequencyOrShearWaveVelocity = qobject_cast<RParameter *>(value);
}	
double RSoilStructureInteractionModel::getSsASCE() const	
{	
	return theSsASCE;
}	

void RSoilStructureInteractionModel::setSsASCE(double value)	
{	
	theSsASCE = value;
}	
double RSoilStructureInteractionModel::getS1ASCE() const	
{	
	return theS1ASCE;
}	

void RSoilStructureInteractionModel::setS1ASCE(double value)	
{	
	theS1ASCE = value;
}

QObject * RSoilStructureInteractionModel::getStrengthReductionFactorOrTargetDuctility() const	
{	
	return theStrengthReductionFactorOrTargetDuctility.data();
}
void RSoilStructureInteractionModel::setStrengthReductionFactorOrTargetDuctility(QObject *value)	
{	
	theStrengthReductionFactorOrTargetDuctility = qobject_cast<RParameter *>(value);
}
double RSoilStructureInteractionModel::getOverStrength() const	
{	
	return theOverStrength;
}	

void RSoilStructureInteractionModel::setOverStrength(double value)	
{	
	theOverStrength = value;
}	
double RSoilStructureInteractionModel::getOverStrengthNEHRP() const	
{	
	return theOverStrengthNEHRP;
}	

void RSoilStructureInteractionModel::setOverStrengthNEHRP(double value)	
{	
	theOverStrengthNEHRP = value;
}

/*
QObject * RSoilStructureInteractionModel::getTargetDuctility() const	
{	
return theTargetDuctility;
}
void RSoilStructureInteractionModel::setTargetDuctility(QObject *value)	
{	
theTargetDuctility = qobject_cast<RParameter *>(value);
}	
*/

QString RSoilStructureInteractionModel::getRecords() const	
{	
	return theRecords.join(";");
}	
void RSoilStructureInteractionModel::setRecords(QString value)	
{	
	QRegExp regExp = QRegExp(QString("[,;\t\n\v\r%1%2]").arg(QChar(QChar::ParagraphSeparator)).arg(QChar(QChar::LineSeparator)));
	theRecords = value.split(regExp, QString::SkipEmptyParts);
	IntensityVector.clear();
	recordAccVectorList.clear();

}	
QString RSoilStructureInteractionModel::getRecordTimeSteps()
{	
	QString result = doubleVectorToString(theRecordTimeStepsVector);
	return result;
}
void RSoilStructureInteractionModel::setRecordTimeSteps(QString value)	
{	
	theRecordTimeStepsVector = stringToDoubleVector(value);
}	
RSoilStructureInteractionModel::RRecordScalingMethod RSoilStructureInteractionModel::getRecordScalingMethod() const	
{	
	return theRecordScalingMethod;
}		

void RSoilStructureInteractionModel::setRecordScalingMethod(RSoilStructureInteractionModel::RRecordScalingMethod value)	
{	
	theRecordScalingMethod = value;
	if(theRecordScalingMethod == RSoilStructureInteractionModel:: Sa){
		rDebug() << "Note: To use defined Sa based on average value of Tn just input zero as Intensity measure ";
	}
	if(theRecordScalingMethod == RSoilStructureInteractionModel:: Spectrum){
		rCritical() << "Warning: In this scaling method the given Intensity measure will not be used ";
	}
	IntensityVector.clear();
	recordAccVectorList.clear();
}	

QObject * RSoilStructureInteractionModel::getIntensityMeasure() const	
{	
	return theIntensityMeasure.data();
}	
void RSoilStructureInteractionModel::setIntensityMeasure(QObject *value)	
{	
	theIntensityMeasure = qobject_cast<RParameter *>(value);
}

RSoilStructureInteractionModel::RShearDistributionMethod RSoilStructureInteractionModel::getShearDistributionMethod() const	
{	
	return theShearDistributionMethod;
}		
void RSoilStructureInteractionModel::setShearDistributionMethod(RSoilStructureInteractionModel::RShearDistributionMethod value)	
{	
	theShearDistributionMethod = value;
}
RSoilStructureInteractionModel::RRunMode RSoilStructureInteractionModel::getRunMode() const	
{	
	return theRunMode;
}		
void RSoilStructureInteractionModel::setRunMode(RSoilStructureInteractionModel::RRunMode value)	
{	
	theRunMode = value;
	if((theRunMode == RSoilStructureInteractionModel:: GivenRSameR)|| (theRunMode == RSoilStructureInteractionModel:: GivenDucSameR) || (theRunMode == RSoilStructureInteractionModel:: GivenDucComputeModificationFactor) || (theRunMode == RSoilStructureInteractionModel:: GivenDucSameRorFy) || (theRunMode == RSoilStructureInteractionModel:: GivenRFixSameFy) || (theRunMode == RSoilStructureInteractionModel:: GivenDucFixSameFy) || (theRunMode == RSoilStructureInteractionModel:: GivenDucSSISameFy) || (theRunMode == RSoilStructureInteractionModel:: GivenRReducedFyATC)){
		rDebug() << "Note: The values that have assigned for SsASVE and S1ASCE will be ignored in analyses  ";
	}
	if((theRunMode == RSoilStructureInteractionModel:: GivenRReducedFyASCE_05)|| (theRunMode == RSoilStructureInteractionModel:: GivenRReducedFyASCE_10) || (theRunMode == RSoilStructureInteractionModel:: GivenRReducedFyNEHRP)){
		rDebug() << "Note: The values that have primarily assigned for SsASVE and S1ASCE belong to Los Angeles, CA, USA ";
	}

}	
RSoilStructureInteractionModel::RRecordScaling RSoilStructureInteractionModel::getRecordScaling() const	
{	
	return theRecordScaling;
}		
void RSoilStructureInteractionModel::setRecordScaling(RSoilStructureInteractionModel::RRecordScaling value)	
{	
	theRecordScaling = value;
}	

double RSoilStructureInteractionModel::getScaleFactorLowerBound() const	
{	
	return theScaleFactorLowerBound;
}	
void RSoilStructureInteractionModel::setScaleFactorLowerBound(double value)	
{	
	theScaleFactorLowerBound = value;
}	

double RSoilStructureInteractionModel::getScaleFactorUpperBound() const	
{	
	return theScaleFactorUpperBound;
}	
void RSoilStructureInteractionModel::setScaleFactorUpperBound(double value)	
{	
	theScaleFactorUpperBound = value;
}	

// ---------------------------------------- Main Code
int RSoilStructureInteractionModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
{	// Code Start
	// Setting Input Variables for Model
	double pi = M_PI;
	double ms = 1.0;			// Mass of Structure for SDOF
	double Tn = theFixedBasePeriod.data()->getCurrentValue(threadID);
	int Num;
	if (theMDOFOrSDOF == RSoilStructureInteractionModel::MDOF){
		Num = theNumberOfStories;
	} else {
		Num =1;
	}
	//********************************************************************* Runmode 5
	/*double I=0.0;
	if (theSoftOrStiff == RSoilStructureInteractionModel::Soft){
		I = 2.0;
	} else {
		I=1.0;
	}
	double teta1 = theCoefficientTeta1->getCurrentValue(threadID);
	double teta2 = theCoefficientTeta2->getCurrentValue(threadID);
	double teta3 = theCoefficientTeta3->getCurrentValue(threadID);
	double teta4 = theCoefficientTeta4->getCurrentValue(threadID);*/
	//********************************************************************* 
	double wn = 2*pi/Tn;		// Natural Frequency of The Structure
	double xi_s = theStructureDamping.data()->getCurrentValue(threadID);;
	double xi_0 =  theSoilDamping.data()->getCurrentValue(threadID);
	double v =  thePoissonRatio.data()->getCurrentValue(threadID);
	double h_r0 =  theAspectRatio.data()->getCurrentValue(threadID);
	double r0=  theRadius;
	r0=1;
	double h=h_r0*r0;
	double e_r0 =  theEmbedmentRatio.data()->getCurrentValue(threadID);
	double mbar =  theStructureToSoilMassRatio.data()->getCurrentValue(threadID);
	double mratio =  theFoundationToStructureMassRatio.data()->getCurrentValue(threadID); //
	double fcr = theFoundationCentroidRatio.data()->getCurrentValue(threadID); // Foundation Center of Mass Position: previously considerd as 1/2
	double a0orvs =  theDimensionlessFrequencyOrShearWaveVelocity.data()->getCurrentValue(threadID);
	double SsASCE= theSsASCE;
	double S1ASCE= theS1ASCE;
	double ROrDuc =  theStrengthReductionFactorOrTargetDuctility.data()->getCurrentValue(threadID);
	double Omega0=  theOverStrength;
	double Omega0NEHRP=  theOverStrengthNEHRP;
	//double target_ductility =  theTargetDuctility->getCurrentValue(threadID);
	double sflb = theScaleFactorLowerBound;
	double sfub = theScaleFactorUpperBound;
	double landam=0;
	double landah=0;
	int n;
	double dt;  
	QVector<double> d2uff ; 
	double R_primary=0;
	double R=0;
	double target_ductility=0;
	double randrec = theRecordSelectionUncertainty.data()->getCurrentValue(threadID);
	if ((theRunMode == RSoilStructureInteractionModel::GivenRSameR) || (theRunMode == RSoilStructureInteractionModel::GivenRReducedFyATC) || (theRunMode == RSoilStructureInteractionModel::GivenRFixSameFy) || (theRunMode == RSoilStructureInteractionModel::GivenRReducedFyASCE_05) || (theRunMode == RSoilStructureInteractionModel::GivenRReducedFyASCE_10) || (theRunMode == RSoilStructureInteractionModel::GivenRReducedFyNEHRP)){
		R_primary=ROrDuc;
		R = R_primary/Omega0;
	}else{
		target_ductility=ROrDuc;
	}
	// Define Average value of the initial parameters which are used for calculationg Base Shear according to ASCE-10 & ATC:
	//------------------ Tn, wn, ks:
	double meanTn;
	RRandomVariable *TnRV = qobject_cast<RRandomVariable *>(theFixedBasePeriod.data());
	if (TnRV) {
		meanTn = TnRV->getMeanValue(threadID);
	} else {
		meanTn = Tn; 
	}
	double meanwn = 2*pi/meanTn;	        //Natural Frequency of the structure for mean Tn
	double meanks = ms*meanwn*meanwn;		// Stiffness of The Structure
	//---------------- h_r0, e_r0:
	double meanh_r0;
	RRandomVariable *h_r0RV = qobject_cast<RRandomVariable *>(theAspectRatio.data());
	if (h_r0RV) {
		meanh_r0 = h_r0RV->getMeanValue(threadID);
	} else {
		meanh_r0 = h_r0; 
	}
	double meane_r0;
	RRandomVariable *e_r0RV = qobject_cast<RRandomVariable *>(theEmbedmentRatio.data());
	if (e_r0RV) {
		meane_r0 = e_r0RV->getMeanValue(threadID);
	} else {
		meane_r0 = e_r0; 
	}
	//--------------- v:
	double meanv;
	RRandomVariable *vRV = qobject_cast<RRandomVariable *>(thePoissonRatio.data());
	if (vRV) {
		meanv = vRV->getMeanValue(threadID);
	} else {
		meanv = v; 
	}
	//-------------- mbar:
	double meanmbar;
	RRandomVariable *mbarRV = qobject_cast<RRandomVariable *>(theStructureToSoilMassRatio.data());
	if (mbarRV) {
		meanmbar = mbarRV->getMeanValue(threadID);
	} else {
		meanmbar = mbar; 
	}
	//--------------- a0orvs:
	double meana0orvs;
	RRandomVariable *a0orvsRV = qobject_cast<RRandomVariable *>(theDimensionlessFrequencyOrShearWaveVelocity.data());
	if (a0orvsRV) {
		meana0orvs = a0orvsRV->getMeanValue(threadID);
	} else {
		meana0orvs = a0orvs; 
	}
	//-----------------------------------" End of definition "
	// calculation of raw_vs and raw_a0fix:
	double raw_a0fix;
	double raw_vs=0;
	if (theSSISeverityMeasure == RSoilStructureInteractionModel::DimensionlessFrequency){
		raw_a0fix=a0orvs;
		raw_vs=wn*h/raw_a0fix;     // Shear Velocity with Soil Hysteretic Material Damping
	} else {
		raw_vs=a0orvs;
		raw_a0fix=wn*h/raw_vs;
	}
	//------------------------------------- Calculating Sa due to meanTn based on ASCE-10:
	//---- Calculating FaASCE and FvASCE: ( Considering Same Fa and Fv for both ASCE and NEHRP to have same fix desinged structures)
	//Importing Table of Fa:
	gsl_matrix *Fatable = gsl_matrix_calloc(3,5);
	gsl_matrix_set(Fatable,0,0,1.20);
	gsl_matrix_set(Fatable,0,1,1.20);
	gsl_matrix_set(Fatable,0,2,1.10);
	gsl_matrix_set(Fatable,0,3,1.00);
	gsl_matrix_set(Fatable,0,4,1.00);
	gsl_matrix_set(Fatable,1,0,1.60);
	gsl_matrix_set(Fatable,1,1,1.40);
	gsl_matrix_set(Fatable,1,2,1.20);
	gsl_matrix_set(Fatable,1,3,1.10);
	gsl_matrix_set(Fatable,1,4,1.00);
	gsl_matrix_set(Fatable,2,0,2.50);
	gsl_matrix_set(Fatable,2,1,1.70);
	gsl_matrix_set(Fatable,2,2,1.20);
	gsl_matrix_set(Fatable,2,3,0.90);
	gsl_matrix_set(Fatable,2,4,0.90);
	//Importing Table of Fv:
	gsl_matrix *Fvtable = gsl_matrix_calloc(3,5);
	gsl_matrix_set(Fvtable,0,0,1.70);
	gsl_matrix_set(Fvtable,0,1,1.60);
	gsl_matrix_set(Fvtable,0,2,1.50);
	gsl_matrix_set(Fvtable,0,3,1.40);
	gsl_matrix_set(Fvtable,0,4,1.30);
	gsl_matrix_set(Fvtable,1,0,2.40);
	gsl_matrix_set(Fvtable,1,1,2.00);
	gsl_matrix_set(Fvtable,1,2,1.80);
	gsl_matrix_set(Fvtable,1,3,1.60);
	gsl_matrix_set(Fvtable,1,4,1.50);
	gsl_matrix_set(Fvtable,2,0,3.50);
	gsl_matrix_set(Fvtable,2,1,3.20);
	gsl_matrix_set(Fvtable,2,2,2.80);
	gsl_matrix_set(Fvtable,2,3,2.40);
	gsl_matrix_set(Fvtable,2,4,2.40);
	// Numbering Different Soil Types:
	double soilnum;
	if (theSoilType == RSoilStructureInteractionModel::SoilTypeC) 
		soilnum=0.0;
	if (theSoilType == RSoilStructureInteractionModel::SoilTypeD) 
		soilnum=1.0;
	if (theSoilType == RSoilStructureInteractionModel::SoilTypeE) 
		soilnum=2.0;	
	//Importing SsVector:
	QVector<double> SsASCEVector;
	SsASCEVector << 0.25 << 0.50 << 0.75 << 1.00 << 1.25;
	//Calculating Fa:
	double FaASCE;
	if(SsASCE<= SsASCEVector[0])
		FaASCE=gsl_matrix_get(Fatable,soilnum,0);
	for( int i=1; i<5 ; i++){
		if((SsASCEVector[i-1]<SsASCE) && (SsASCE<= SsASCEVector[i])){
			double Fa_2= gsl_matrix_get(Fatable,soilnum,i);
			double Ss_2= SsASCEVector[i];
			double Fa_1= gsl_matrix_get(Fatable,soilnum,i-1);
			double Ss_1=SsASCEVector[i-1];
			FaASCE= ((Fa_2-Fa_1)/(Ss_2-Ss_1))*(SsASCE-Ss_1)+ Ss_1;
		}
	}
	if(SsASCE>=SsASCEVector[4])
		FaASCE=gsl_matrix_get(Fatable,soilnum,4);
	//Importing S1Vector:
	QVector<double> S1ASCEVector;
	S1ASCEVector << 0.1 << 0.2 << 0.3 << 0.4 << 0.5;
	//Calculating Fv:
	double FvASCE;
	if(S1ASCE<= S1ASCEVector[0])
		FvASCE=gsl_matrix_get(Fvtable,soilnum,0);
	for( int i=1 ; i<5 ; i++){
		if((S1ASCEVector[i-1]<S1ASCE) && (S1ASCE<= S1ASCEVector[i])){
			double Fv_2= gsl_matrix_get(Fvtable,soilnum,i);
			double S1_2= S1ASCEVector[i];
			double Fv_1= gsl_matrix_get(Fvtable,soilnum,i-1);
			double S1_1= S1ASCEVector[i-1];
			FvASCE = ((Fv_2-Fv_1)/(S1_2-S1_1))*(S1ASCE-S1_1)+ Fv_1;
		}
	}
	if(S1ASCE>S1ASCEVector[4])
		FvASCE=gsl_matrix_get(Fvtable,soilnum,4);
	//Calculating Sds and Sdi
	double SmsASCE=FaASCE*SsASCE;
	double SmiASCE=FvASCE*S1ASCE;
	double SdsASCE=(2.0*SmsASCE)/3.0;
	double SdiASCE=(2.0*SmiASCE)/3.0;
	
	// ----Calculating RF(degradation Factor for shear wave velosity:
	double RF;
	if (theApplyShearWaveVelocityDegradationFactor) {
	//Importating table:
		gsl_matrix *RFtable = gsl_matrix_calloc(3,3);
		gsl_matrix_set(RFtable,0,0,0.97);
		gsl_matrix_set(RFtable,0,1,0.87);
		gsl_matrix_set(RFtable,0,2,0.77);
		gsl_matrix_set(RFtable,1,0,0.95);
		gsl_matrix_set(RFtable,1,1,0.71);
		gsl_matrix_set(RFtable,1,2,0.32);
		gsl_matrix_set(RFtable,2,0,0.77);
		gsl_matrix_set(RFtable,2,1,0.22);
		gsl_matrix_set(RFtable,2,2,0.10);
		//Importing Sds/2.5 Table:
		QVector<double> SdsASCEVector;
		SdsASCEVector << 0.1 << 0.4 << 0.8;
		//Calculating RF:
		if(SdsASCE/2.5 <= SdsASCEVector[0])
			RF=gsl_matrix_get(RFtable,soilnum,0);	
		for(int i=1 ; i<3 ; i++){
			if((SdsASCEVector[i-1]<SdsASCE/2.5) && (SdsASCE/2.5<= SsASCEVector[i])){
			double RF_2= gsl_matrix_get(RFtable,soilnum,i);
			double Sds_2= SdsASCEVector[i];
			double RF_1= gsl_matrix_get(RFtable,soilnum,i-1);
			double Sds_1=SdsASCEVector[i-1];
			RF = ((RF_2-RF_1)/(Sds_2-Sds_1))*(SdsASCE/2.5-Sds_1)+ RF_1;
			}
		}
		if(SdsASCE/2.5 > SdsASCEVector[2]){
			RF=gsl_matrix_get(RFtable,soilnum,2);
		}
		} else {
			RF = 1;
		}

	
//--------------------------------------------------------------------------

	double T0=(0.2*SdiASCE)/SdsASCE;
	double Ts=(SdiASCE)/SdsASCE;
	double SaASCE;
	if (meanTn<T0){
		SaASCE=SdsASCE*(0.4+0.6*(meanTn/T0));
	}
	if (T0<meanTn && meanTn<Ts){
		SaASCE=SdsASCE;
	}
	if (Ts<meanTn){
		SaASCE=(SdiASCE/meanTn);
	}
	// calculation of a0 and reduced vs:
	double a0fix=0;
	double vs=0;
	if (theSSISeverityMeasure == RSoilStructureInteractionModel::DimensionlessFrequency){
		a0fix=raw_a0fix;
		vs=raw_vs;     // Shear Velocity with Soil Hysteretic Material Damping
	} else {
		vs=RF*raw_vs;
		a0fix=(wn)*(h)/vs;
	}
	double Intensity =  theIntensityMeasure.data()->getCurrentValue(threadID);
	if ((Intensity == 0) && (theRecordScalingMethod == RSoilStructureInteractionModel::Sa) ) {	// when user what to use defined Sa based on average of given Tn
			Intensity = SaASCE;
	}
	int dof;
	if (a0fix==0){	//Fixed base
		dof=Num;
	}
	else{			//SSI effect 
		dof=3+Num;
	}
	if (getDisplayOutput()) {
		rDebug(threadID) << "------------------------------------------------" << endl;	
		rDebug(threadID) << "Tn = " << Tn << endl;
		rDebug(threadID) << "xi_s = " << xi_s << endl;
		rDebug(threadID) << "xi_0 = " << xi_0 << endl;
		rDebug(threadID) << "Poisson = " << v << endl;
		rDebug(threadID) << "h_r0 = " << h_r0 << endl;
		rDebug(threadID) << "r0 = " << r0 << endl;
		rDebug(threadID) << "e_r0 = " << e_r0 << endl;
		rDebug(threadID) << "mbar = " << mbar << endl;
		rDebug(threadID) << "mratio = " << mratio << endl;
		if (theSSISeverityMeasure == RSoilStructureInteractionModel::DimensionlessFrequency)
			rDebug(threadID) << "a0fix = " << a0fix << endl;
		else
			rDebug(threadID) << "Vs = " << a0orvs << endl;
		if (theRecordScalingMethod == RSoilStructureInteractionModel::PGA)
			rDebug(threadID) << "pga = " << Intensity << endl;


		if ((theRunMode == RSoilStructureInteractionModel::GivenRSameR) || (theRunMode == RSoilStructureInteractionModel::GivenRReducedFyATC) || (theRunMode == RSoilStructureInteractionModel::GivenRFixSameFy)  || (theRunMode == RSoilStructureInteractionModel::GivenRReducedFyASCE_05) || (theRunMode == RSoilStructureInteractionModel::GivenRReducedFyASCE_10) || (theRunMode == RSoilStructureInteractionModel::GivenRReducedFyNEHRP))
			rDebug(threadID) << "R = " << R << endl;
		else 
			rDebug(threadID) << "Target Ductility = " << target_ductility << endl;			

	}


	if (theRecords.count() != theRecordTimeStepsVector.count()){
		rCritical(threadID) << "Error: Records and Time Steps do not match in number";
		return -1;
	}

	// Clearing data from previous run
	//recordAccVectorList.clear();
	//IntensityVector.clear();
	//dtVector.clear();
	//-------------------------Record Scaleing based on PGA:
	if(theRecordScalingMethod == RSoilStructureInteractionModel::PGA){
		// Reading acceleration data files and sorting based on PGA	
		if (recordAccVectorList.isEmpty()==true) {
			for (int i = 0; i < theRecords.count(); i++) {

				QFile file(theRecords[i]);
				if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
					rCritical(threadID) << "Error: Could not open the file" << theRecords[i];
					return -1;
				}
				QTextStream data(&file);		
				QRegExp regExp = QRegExp("[ ,;\t]");
				int lineNumber = 0;
				double recpga = 0.0;
				QVector<double> recordAccVector;
				while (!data.atEnd()) {
					lineNumber++;
					QString line = data.readLine();
					QStringList rowList = line.split(regExp, QString::SkipEmptyParts);
					for (int j = 0; j < rowList.count(); j++) {
						bool ok;
						double doubleValue = rowList[j].toDouble(&ok);
						if (ok) {
							recordAccVector << doubleValue;
							if (qAbs(doubleValue) > recpga) {
								recpga = qAbs(doubleValue);
							}

						} else {
							rCritical(threadID) << "Error in model" << objectName() << ": Could not read acceleration data file: " << theRecords[i] << " at line: " << lineNumber << " and column: " << j+1;
							return -1;
						}
					}
				} // end while (!data.atEnd())

				int j=0; bool findplace = false;
				while ( (j<IntensityVector.count()) && (findplace==false)){ 
					if (recpga > IntensityVector[j])
						j++;
					else
						findplace=true;
				}
				IntensityVector.insert(j, recpga);
				recordAccVectorList.insert(j, recordAccVector);
				dtVector.insert(j, theRecordTimeStepsVector[i]);
			} // end or i
		} // end if (recordAccVectorList.isEmpty()==true)

		// Selecting the record based on pga ( Randomly selecting a record with a pga between 0.5*PGA and 2*PGA )
		int recnum = 0;
		int first_rec = 0;
		int last_rec = IntensityVector.count()-1;
		bool flag=true;
		if (theRecordScaling == RSoilStructureInteractionModel::Adaptive){
			if (sflb*IntensityVector[0]>Intensity){
				Intensity = sflb*IntensityVector[0];
				if (getDisplayOutput()) 
					rDebug(threadID) << "Warning: The given PGA is too small: Rt is setting PGA = ScaleFactorLowerBound * minimum of record's PGAs = " << Intensity;
			}
			if (sfub*IntensityVector[last_rec]<Intensity){
				Intensity = sfub*IntensityVector[last_rec];
				if (getDisplayOutput()) 
					rDebug(threadID) << "Warning: The given PGA is too large: Rt is setting PGA = ScaleFactorUpperBound * maximum of record's PGAs = " << Intensity;
			}
			while((recnum <= last_rec)&&(flag==true)){
				if ( sfub*IntensityVector[recnum] >= Intensity ){
					first_rec=recnum;
					flag=false;
				}
				else{
					recnum++;
				}
			}
			flag=true;	
			while((recnum <= last_rec)&&(flag==true)){
				if ( sflb*IntensityVector[recnum] > Intensity ){
					last_rec=recnum-1;
					flag=false;
				}
				else{
					recnum++;
				}
			}
			if (last_rec < first_rec){
				if ((Intensity-sfub*IntensityVector[last_rec])<= (sflb*IntensityVector[first_rec]-Intensity)){
					Intensity = sfub*IntensityVector[last_rec];
					first_rec=last_rec;
					while((first_rec > 0)&&(IntensityVector[first_rec-1]==IntensityVector[last_rec]))
						first_rec=first_rec-1;
				}
				else {
					Intensity = sflb*IntensityVector[first_rec];
					last_rec=first_rec;
					while((last_rec < IntensityVector.count()-1)&&(IntensityVector[last_rec+1]==IntensityVector[first_rec]))
						last_rec++;
				}
				if (getDisplayOutput()) 
					rDebug(threadID) << "Warning: There is no record with a PGA in the appropriate range: Rt is setting PGA = nearest appropriate PGA = " << Intensity;
			}
		}
		/// select record
		/*srand (time(NULL));
		recnum = rand() % (last_rec-first_rec+1) + first_rec;*/
		recnum = qFloor(randrec*(last_rec-first_rec)+first_rec);

		if (getDisplayOutput()) {
			rDebug(threadID) << "Selected Record Number = " << (recnum+1) << " => Selected Record is: " << theRecords[recnum] << endl;	

			if (theRecordScaling == RSoilStructureInteractionModel::Unscaled)
				rDebug(threadID) << "Unscaled Record PGA = " << IntensityVector[recnum] << endl;
			else 
				rDebug(threadID) << "Given PGA = " << Intensity << endl;

		}

		d2uff = recordAccVectorList[recnum];
		n = d2uff.count();
		dt = dtVector[recnum];   

		// Scaling the record to the given PGA -- [ acceleration data file (g) --> d2uff (m/s) ] 
		if (theRecordScaling == RSoilStructureInteractionModel::Unscaled){
			Intensity=IntensityVector[recnum];
			for (int i=0; i<d2uff.count() ;i++) {
				d2uff[i] = d2uff[i] * 9.81;
			}	
		} else{
			for (int i=0; i<d2uff.count() ;i++) {
				d2uff[i] = d2uff[i] * (Intensity/IntensityVector[recnum]) * 9.81;
			}	
		}
	}// end for based on given pga

	//-----------------------------------Record scaling based on Sa:

	if(theRecordScalingMethod == RSoilStructureInteractionModel::Sa){
		if (theMDOFOrSDOF == RSoilStructureInteractionModel::SDOF){	//*********************SDOF systems
			// Reading acceleration data files and sorting based on Maximum Accelaration on given Tn:	
			if (recordAccVectorList.isEmpty()==true) {
				rDebug(threadID) << "test";
				int gsl_status;
				int la_status;
				double ks = ms*meanwn*meanwn;		// Stiffness of The Structure
				double cs = 2*0.05*ms*wn;	// Damping of The Structure
				gsl_matrix *Kfix = gsl_matrix_calloc(1,1);
				gsl_matrix *Cfix = gsl_matrix_calloc(1,1);
				gsl_matrix *Mfix = gsl_matrix_calloc(1,1);
				gsl_matrix *Lfix = gsl_matrix_calloc(1,2);
				gsl_matrix_set(Mfix,0,0,ms);
				gsl_matrix_set(Cfix,0,0,cs);
				gsl_matrix_set(Kfix,0,0,ks);
				gsl_matrix_set(Lfix,0,0,ms);
				gsl_matrix_set(Lfix,0,1,0);
				for (int i = 0; i < theRecords.count(); i++) {
					QFile file(theRecords[i]);
					if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
						rCritical(threadID) << "Error: Could not open the file" << theRecords[i];
						return -1;
					}
					QTextStream data(&file);		
					QRegExp regExp = QRegExp("[ ,;\t]");
					int lineNumber = 0;
					QVector<double> recordAccVector;
					while (!data.atEnd()) {
						lineNumber++;
						QString line = data.readLine();
						QStringList rowList = line.split(regExp, QString::SkipEmptyParts);
						for (int j = 0; j < rowList.count(); j++) {
							bool ok;
							double doubleValue = rowList[j].toDouble(&ok);
							if (ok) {
								recordAccVector << doubleValue;

							} else {
								rCritical(threadID) << "Error in model" << objectName() << ": Could not read acceleration data file: " << theRecords[i] << " at line: " << lineNumber << " and column: " << j+1;
								return -1;
							}
						}
					} // end while (!data.atEnd())

					//finding maximum suedo accelaration for record considering 0.05 damping ratio
					n=recordAccVector.count();
					dt=theRecordTimeStepsVector[i];
					QVector<double> FIMhfix(n);	// Horizontal Foundation Input Motion
					QVector<double> FIMrfix(n);	// Rotational Foundation Input Motion
					FIMhfix=recordAccVector;
					FIMrfix.fill(0.0);
					double maxD=0;	//Maximum Elastic Accelaration
					double maxDtot=0;
					double maxA;
					double maxAtot;
					double Acc=0;	//  suedo Accelaration 
					//linear Analysis for calculationg maxD ( considering Fix base )
					la_status = linearAnalysisSDOF(0.0, Kfix, Cfix, Mfix, Lfix, FIMhfix, FIMrfix, n, dt, 1.0 , &maxD, &maxDtot, &maxA, &maxAtot,threadID);
					if (la_status) {  
						rCritical(threadID) << "Error in linearAnalysis function";
						return -1;
					}
					Acc=maxD*meanwn*meanwn; //suedo accelaration
					//Sorting records due to their suedo accelarations:
					int j=0; bool findplace = false;
					while ( (j<IntensityVector.count()) && (findplace==false)){ 
						if (Acc > IntensityVector[j])
							j++;
						else
							findplace=true;
					}
					IntensityVector.insert(j, Acc);
					recordAccVectorList.insert(j, recordAccVector);
					dtVector.insert(j, dt);
				} // end of: for (int i = 0; i < theRecords.count(); i++)
				// Freeing allocated memory
				gsl_matrix_free(Mfix);
				gsl_matrix_free(Kfix);
				gsl_matrix_free(Cfix);
				gsl_matrix_free(Lfix);
			} // end of: if (recordAccVectorList.isEmpty()==true)

			// Selecting the record based on Sa 
			int recnum = 0;
			int first_rec = 0;
			int last_rec = IntensityVector.count()-1;
			bool flag=true;
			// Adaptive: Randomly selecting a record with a Acc between 0.5*Sa and 2*Sa 
			if (theRecordScaling == RSoilStructureInteractionModel::Adaptive){
				if (sflb*IntensityVector[0]>Intensity){
					Intensity = sflb*IntensityVector[0];
					if (getDisplayOutput()) 
						rDebug(threadID) << "Warning: The Sa is too small: Rt is setting Sa = ScaleFactorLowerBound * minimum of record's Sas = " << Intensity;
				}
				if (sfub*IntensityVector[last_rec]<Intensity){
					Intensity = sfub*IntensityVector[last_rec];
					if (getDisplayOutput()) 
						rDebug(threadID) << "Warning: The SaASCE is too large: Rt is setting Sa = ScaleFactorUpperBound * maximum of record's Sas = " << Intensity;
				}
				while((recnum <= last_rec)&&(flag==true)){
					if ( sfub*IntensityVector[recnum] >= Intensity ){
						first_rec=recnum;
						flag=false;
					}
					else{
						recnum++;
					}
				}
				flag=true;	
				while((recnum <= last_rec)&&(flag==true)){
					if ( sflb*IntensityVector[recnum] > Intensity){
						last_rec=recnum-1;
						flag=false;
					}
					else{
						recnum++;
					}
				}
				if (last_rec < first_rec){
					if ((Intensity-sfub*IntensityVector[last_rec])<= (sflb*IntensityVector[first_rec]-Intensity)){
						Intensity = sfub*IntensityVector[last_rec];
						first_rec=last_rec;
						while((first_rec > 0)&&(IntensityVector[first_rec-1]==IntensityVector[last_rec]))
							first_rec=first_rec-1;
					}
					else {
						Intensity = sflb*IntensityVector[first_rec];
						last_rec=first_rec;
						while((last_rec < IntensityVector.count()-1)&&(IntensityVector[last_rec+1]==IntensityVector[first_rec]))
							last_rec++;
					}
					if (getDisplayOutput()) 
						rDebug(threadID) << "Warning: There is no record with a SaASCE in the appropriate range: Rt is setting Sa = nearest appropriate Sa = " << Intensity;
				}
			} //End of: if (theRecordScaling == RSoilStructureInteractionModel::Adaptive)
			//Randomly selecting a record:
			srand (time(NULL));
			recnum = rand() % (last_rec-first_rec+1) + first_rec;

			if (getDisplayOutput()) {
				rDebug(threadID) << "Selected Record Number = " << (recnum+1) << " => Selected Record is: " << theRecords[recnum] << endl;	

				if (theRecordScaling == RSoilStructureInteractionModel::Unscaled)
					rDebug(threadID) << "Unscaled Record Sa = " << IntensityVector[recnum] << endl;
				else 
					rDebug(threadID) << "Given Sa = " << Intensity << endl;

			}
			d2uff = recordAccVectorList[recnum];
			n = d2uff.count();
			dt = dtVector[recnum];   
			// Scaling the record to the given Sa -- [ acceleration data file (g) --> d2uff (m/s) ] 
			if (theRecordScaling == RSoilStructureInteractionModel::Unscaled){
				for (int i=0; i<d2uff.count() ;i++) {
					d2uff[i] = d2uff[i] * 9.81;
				}	
			} else{
				for (int i=0; i<d2uff.count() ;i++) {
					d2uff[i] = d2uff[i] * (Intensity/IntensityVector[recnum]) * 9.81;
				}	
			}
		}//end of SDOF
		else //************************************************* MDOF systems
		{
		// Reading acceleration data files and sorting based on Maximum Accelaration on given Tn:	
			if (recordAccVectorList.isEmpty()==true) {
				int gsl_status;
				int la_status;
				int SDMatrix_status;
				double Mtotal=ms;	//Consider that all of mass will participate in the first mode
				double roh;
				double htotal;
				double h_story;
				gsl_vector *kstr = gsl_vector_calloc(Num);	//[k1/k1;k2/k1;...;ki/k1;...]
				gsl_matrix *Kfix = gsl_matrix_calloc(Num,Num);
				gsl_matrix *Cfix = gsl_matrix_calloc(Num,Num);
				gsl_matrix *Mfix = gsl_matrix_calloc(Num,Num);
				gsl_matrix *Lfix = gsl_matrix_calloc(Num,2);
				SDMatrix_status = stiffnessAndDampingMatrixMDOF(Tn,vs,wn,h_r0,mbar,xi_s,Num,kstr,Kfix,Mfix,Cfix,&h_story,&htotal,Mtotal,&r0,&roh,&landam,&landah,threadID);
				if (SDMatrix_status) {  
					rCritical(threadID) << "Error in StiffnessAndDampingMatrixMDOF function";
					return -1;
		     	}
				double e=e_r0*r0;
				gsl_matrix_set_all(Lfix,1);
				for (int i = 0; i < theRecords.count(); i++) {
					QFile file(theRecords[i]);
					if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
						rCritical(threadID) << "Error: Could not open the file" << theRecords[i];
						return -1;
					}
					QTextStream data(&file);		
					QRegExp regExp = QRegExp("[ ,;\t]");
					int lineNumber = 0;
					QVector<double> recordAccVector;
					while (!data.atEnd()) {
						lineNumber++;
						QString line = data.readLine();
						QStringList rowList = line.split(regExp, QString::SkipEmptyParts);
						for (int j = 0; j < rowList.count(); j++) {
							bool ok;
							double doubleValue = rowList[j].toDouble(&ok);
							if (ok) {
								recordAccVector << doubleValue;

							} else {
								rCritical(threadID) << "Error in model" << objectName() << ": Could not read acceleration data file: " << theRecords[i] << " at line: " << lineNumber << " and column: " << j+1;
								return -1;
							}
						}
					} // end while (!data.atEnd())

					//finding maximum suedo accelaration for record considering 0.05 damping ratio
					n=recordAccVector.count();
					dt=theRecordTimeStepsVector[i];
					QVector<double> FIMhfix(n);	// Horizontal Foundation Input Motion
					QVector<double> FIMrfix(n);	// Rotational Foundation Input Motion
					FIMhfix=recordAccVector;
					FIMrfix.fill(0.0);
					gsl_vector *maxD_v=gsl_vector_calloc(Num);	//Maximum Elastic Accelaration
					gsl_vector *maxDtot_v=gsl_vector_calloc(Num);
					gsl_vector *maxA_v=gsl_vector_calloc(Num);
					gsl_vector *maxAtot_v=gsl_vector_calloc(Num);
					double Acc=0;	//  suedo Accelaration 
					double VeFix=0;
					//linear Analysis for calculationg maxD ( considering Fix base )
					la_status = linearAnalysisMDOF(h_story,e,kstr,Kfix, Cfix, Mfix, Lfix, FIMhfix, FIMrfix, n, dt, Num, Num, maxD_v, maxDtot_v, maxA_v, maxAtot_v,&VeFix,threadID);
					if (la_status) {  
						rCritical(threadID) << "Error in linearAnalysis function";
						return -1;
					}
					double maxD=gsl_vector_max(maxD_v);
					Acc=maxD*meanwn*meanwn; //suedo accelaration
					//Sorting records due to their suedo accelarations:
					int j=0; bool findplace = false;
					while ( (j<IntensityVector.count()) && (findplace==false)){ 
						if (Acc > IntensityVector[j])
							j++;
						else
							findplace=true;
					}
					IntensityVector.insert(j, Acc);
					recordAccVectorList.insert(j, recordAccVector);
					dtVector.insert(j, dt);
				} // end of: for (int i = 0; i < theRecords.count(); i++)
				// Freeing allocated memory
				gsl_matrix_free(Mfix);
				gsl_matrix_free(Kfix);
				gsl_matrix_free(Cfix);
				gsl_matrix_free(Lfix);
				gsl_vector_free(kstr);
			} // end of: if (recordAccVectorList.isEmpty()==true)

			// Selecting the record based on Sa 
			int recnum = 0;
			int first_rec = 0;
			int last_rec = IntensityVector.count()-1;
			bool flag=true;
			// Adaptive: Randomly selecting a record with a Acc between 0.5*Sa and 2*Sa 
			if (theRecordScaling == RSoilStructureInteractionModel::Adaptive){
				if (sflb*IntensityVector[0]>Intensity){
					Intensity = sflb*IntensityVector[0];
					if (getDisplayOutput()) 
						rDebug(threadID) << "Warning: The Sa is too small: Rt is setting Sa = ScaleFactorLowerBound * minimum of record's Sas = " << Intensity;
				}
				if (sfub*IntensityVector[last_rec]<Intensity){
					Intensity = sfub*IntensityVector[last_rec];
					if (getDisplayOutput()) 
						rDebug(threadID) << "Warning: The SaASCE is too large: Rt is setting Sa = ScaleFactorUpperBound * maximum of record's Sas = " << Intensity;
				}
				while((recnum <= last_rec)&&(flag==true)){
					if ( sfub*IntensityVector[recnum] >= Intensity ){
						first_rec=recnum;
						flag=false;
					}
					else{
						recnum++;
					}
				}
				flag=true;	
				while((recnum <= last_rec)&&(flag==true)){
					if ( sflb*IntensityVector[recnum] > Intensity){
						last_rec=recnum-1;
						flag=false;
					}
					else{
						recnum++;
					}
				}
				if (last_rec < first_rec){
					if ((Intensity-sfub*IntensityVector[last_rec])<= (sflb*IntensityVector[first_rec]-Intensity)){
						Intensity = sfub*IntensityVector[last_rec];
						first_rec=last_rec;
						while((first_rec > 0)&&(IntensityVector[first_rec-1]==IntensityVector[last_rec]))
							first_rec=first_rec-1;
					}
					else {
						Intensity = sflb*IntensityVector[first_rec];
						last_rec=first_rec;
						while((last_rec < IntensityVector.count()-1)&&(IntensityVector[last_rec+1]==IntensityVector[first_rec]))
							last_rec++;
					}
					if (getDisplayOutput()) 
						rDebug(threadID) << "Warning: There is no record with a SaASCE in the appropriate range: Rt is setting Sa = nearest appropriate Sa = " << Intensity;
				}
			} //End of: if (theRecordScaling == RSoilStructureInteractionModel::Adaptive)
			//Randomly selecting a record:
			srand (time(NULL));
			recnum = rand() % (last_rec-first_rec+1) + first_rec;

			if (getDisplayOutput()) {
				rDebug(threadID) << "Selected Record Number = " << (recnum+1) << " => Selected Record is: " << theRecords[recnum] << endl;	

				if (theRecordScaling == RSoilStructureInteractionModel::Unscaled)
					rDebug(threadID) << "Unscaled Record Sa = " << IntensityVector[recnum] << endl;
				else 
					rDebug(threadID) << "Given Sa = " << Intensity << endl;

			}
			d2uff = recordAccVectorList[recnum];
			n = d2uff.count();
			dt = dtVector[recnum];   
			// Scaling the record to the given Sa -- [ acceleration data file (g) --> d2uff (m/s) ] 
			if (theRecordScaling == RSoilStructureInteractionModel::Unscaled){
				for (int i=0; i<d2uff.count() ;i++) {
					d2uff[i] = d2uff[i] * 9.81;
				}	
			} else{
				for (int i=0; i<d2uff.count() ;i++) {
					d2uff[i] = d2uff[i] * (Intensity/IntensityVector[recnum]) * 9.81;
				}	
			}
		}//end of MDOF
	}	//end of Basedon_Sa

	//---------------------------Record Scaling Based on Spectrum:

	if(theRecordScalingMethod == RSoilStructureInteractionModel:: Spectrum){
		if(theMDOFOrSDOF == RSoilStructureInteractionModel:: SDOF){	//***************************SDOF systems
			// Reading acceleration data files and sorting based on Spectrum
			if (recordAccVectorList.isEmpty()==true) {
				double lowerTn = 0.2 * meanTn;
				double upperTn = 1.5 * meanTn;
				double dtn=(upperTn-lowerTn)/10;
				QVector<double> SaASCEVector;
				QVector<double> TnASCEVector;
				for ( int i=0; i<11; i++){
					TnASCEVector.insert(i, lowerTn+i*dtn);
					if (TnASCEVector[i]<T0)
						SaASCEVector.insert(i,SdsASCE*(0.4+0.6*(TnASCEVector[i]/T0)));
					if (T0<TnASCEVector[i] && TnASCEVector[i]<Ts)
						SaASCEVector.insert(i, SdsASCE);
					if (Ts<TnASCEVector[i])
						SaASCEVector.insert(i,SdiASCE/TnASCEVector[i]);
				} // end of for ( int i=0; i<11; i++)
				double ks = ms*meanwn*meanwn;		// Stiffness of The Structure
				double cs = 2*0.05*ms*meanwn;	// Damping of The Structure
				int gsl_status;
				int la_status;
				gsl_matrix *Kfix = gsl_matrix_calloc(1,1);
				gsl_matrix *Cfix = gsl_matrix_calloc(1,1);
				gsl_matrix *Mfix = gsl_matrix_calloc(1,1);
				gsl_matrix *Lfix = gsl_matrix_calloc(1,2);
				gsl_matrix *Kfix_l = gsl_matrix_calloc(1,1);
				gsl_matrix *Cfix_l = gsl_matrix_calloc(1,1);
				gsl_matrix *Mfix_l = gsl_matrix_calloc(1,1);
				gsl_matrix *Lfix_l = gsl_matrix_calloc(1,2);
				gsl_matrix_set(Mfix,0,0,ms);
				gsl_matrix_set(Cfix,0,0,cs);
				gsl_matrix_set(Kfix,0,0,ks);
				gsl_matrix_set(Lfix,0,0,ms);
				gsl_matrix_set(Lfix,0,1,0);
				for (int i = 0; i < theRecords.count(); i++) {
					QFile file(theRecords[i]);
					if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
						rCritical(threadID) << "Error: Could not open the file" << theRecords[i];
						return -1;
					}
					QTextStream data(&file);		
					QRegExp regExp = QRegExp("[ ,;\t]");
					int lineNumber = 0;
					QVector<double> recordAccVector;
					while (!data.atEnd()) {
						lineNumber++;
						QString line = data.readLine();
						QStringList rowList = line.split(regExp, QString::SkipEmptyParts);
						for (int j = 0; j < rowList.count(); j++) {
							bool ok;
							double doubleValue = rowList[j].toDouble(&ok);
							if (ok) {
								recordAccVector << doubleValue;

							} else {
								rCritical(threadID) << "Error in model" << objectName() << ": Could not read acceleration data file: " << theRecords[i] << " at line: " << lineNumber << " and column: " << j+1;
								return -1;
							}
						}
					} // end while (!data.atEnd())
					//finding maximum suedo accelaration for record considering 0.05 damping ratio
					n=recordAccVector.count();
					dt=theRecordTimeStepsVector[i];
					QVector<double> FIMhfix(n);	// Horizontal Foundation Input Motion
					QVector<double> FIMrfix(n);	// Rotational Foundation Input Motion
					FIMhfix=recordAccVector;
					FIMrfix.fill(0.0);
					double maxD=0;	//Maximum Elastic Accelaration
					double maxDtot=0;
					double maxA;
					double maxAtot;
					double Acc=0;
					la_status = linearAnalysisSDOF(0.0, Kfix, Cfix, Mfix, Lfix, FIMhfix, FIMrfix, n, dt, 1.0 , &maxD, &maxDtot, &maxA, &maxAtot,threadID);
					if (la_status) {  
						rCritical(threadID) << "Error in linearAnalysis function";
						return -1;
		     		}
					Acc=maxD*meanwn*meanwn; //maximum suedo accelaration

					//scaling each record based on Acc in order to prepare it for furthur calculations in order preiod rang
					for (int l=0; l<recordAccVector.count() ;l++) {
						recordAccVector[l] = recordAccVector[l] * (SaASCE/Acc);
					}
					QVector<double> FIMhfix_l(n);	// Horizontal Foundation Input Motion
					QVector<double> FIMrfix_l(n);	// Rotational Foundation Input Motion
					FIMhfix_l=recordAccVector;
					FIMrfix_l.fill(0.0);
					int l=0; bool delta = true;
					while ((l<TnASCEVector.count()) && (delta==true )) {
						double wn_l = 2*pi/TnASCEVector[l];		// Natural Frequency of The Structure
						double ks_l = ms*wn_l*wn_l;		// Stiffness of The Structure
						double cs_l = 2*0.05*ms*wn_l;	// Damping of The Structure
						gsl_matrix_set(Mfix_l,0,0,ms);
						gsl_matrix_set(Cfix_l,0,0,cs_l);
						gsl_matrix_set(Kfix_l,0,0,ks_l);
						gsl_matrix_set(Lfix_l,0,0,ms);
						gsl_matrix_set(Lfix_l,0,1,0);
						double maxD_l=0;	//Maximum Elastic Accelaration
						double maxDtot_l=0;
						double maxA_l;
						double maxAtot_l;
						double Acc_l=0;	//  Maximum Elastic suedo Accelaration 
						//linear Analysis for calculationg maxD ( considering Fix base )
						la_status = linearAnalysisSDOF(0.0, Kfix_l, Cfix_l, Mfix_l, Lfix_l, FIMhfix_l, FIMrfix_l, n, dt, 1.0 , &maxD_l, &maxDtot_l, &maxA_l, &maxAtot_l,threadID);
						if (la_status) {  
							rCritical(threadID) << "Error in linearAnalysis function";
							return -1;
						}
						Acc_l=wn_l*wn_l*maxD_l; //maximum suedo acceleration

						// Freeing allocated memory ( Linear Analysis )

						double delta_Sa =  (Acc_l-SaASCEVector[l])/SaASCEVector[l];
						if (qAbs(delta_Sa)>0.2){
							delta=false;
						} else {
							l++;
						}
					} 	// End of: while ((l<TnASCEVector.count()) && (delta==true ) 
					//all 10 checked point is ok then:
					if (delta==true){
						// returnig to unscaled mode:
						for (int l=0; l<recordAccVector.count() ;l++) {
							recordAccVector[l] = recordAccVector[l] * (Acc/SaASCE);
						}
						int j=0; bool findplace = false;
						while ( (j<IntensityVector.count()) && (findplace==false)){ 
							if (Acc > IntensityVector[j])
								j++;
							else
								findplace=true;
						}
						IntensityVector.insert(j, Acc);
						recordAccVectorList.insert(j, recordAccVector);
						dtVector.insert(j, dt);
					} //end for if (delta==true)
				} // end or i
				// Freeing allocated memories:
				gsl_matrix_free(Mfix);
				gsl_matrix_free(Kfix);
				gsl_matrix_free(Cfix);
				gsl_matrix_free(Lfix);
				gsl_matrix_free(Mfix_l);
				gsl_matrix_free(Kfix_l);
				gsl_matrix_free(Cfix_l);
				gsl_matrix_free(Lfix_l);
			} // end if (recordAccVectorList.isEmpty()==true)

			if (recordAccVectorList.isEmpty()==true) {
				rCritical(threadID) << "Error: No record matches the scaling criteria ";
				return -1;
			}

			// Selecting the record based on Sa between the authorized records ( Randomly selecting a record with a Acc between 0.5*Sa and 2*Sa )
			int recnum = 0;
			int first_rec = 0;
			int last_rec = IntensityVector.count()-1;
			bool flag=true;
			if (theRecordScaling == RSoilStructureInteractionModel::Adaptive){
				if (sflb*IntensityVector[0]>SaASCE){
					SaASCE = sflb*IntensityVector[0];
					if (getDisplayOutput()) 
						rDebug(threadID) << "Warning: The Sa is too small: Rt is setting Sa = ScaleFactorLowerBound * minimum of record's Sas = " << SaASCE;
				}
				if (	sfub*IntensityVector[last_rec]<SaASCE){
					SaASCE = sfub*IntensityVector[last_rec];
					if (getDisplayOutput()) 
						rDebug(threadID) << "Warning: The SaASCE is too large: Rt is setting Sa = ScaleFactorUpperBound * maximum of record's Sas = " << SaASCE;
				}
				while((recnum <= last_rec)&&(flag==true)){
					if ( sfub*IntensityVector[recnum] >= SaASCE ){
						first_rec=recnum;
						flag=false;
					}
					else{
						recnum++;
					}
				}
				flag=true;	
				while((recnum <= last_rec)&&(flag==true)){
					if ( sflb*IntensityVector[recnum] > SaASCE){
						last_rec=recnum-1;
						flag=false;
					}
					else{
						recnum++;
					}
				}
				if (last_rec < first_rec){
					if ((SaASCE-sfub*IntensityVector[last_rec])<= (sflb*IntensityVector[first_rec]-SaASCE)){
						SaASCE = sfub*IntensityVector[last_rec];
						first_rec=last_rec;
						while((first_rec > 0)&&(IntensityVector[first_rec-1]==IntensityVector[last_rec]))
							first_rec=first_rec-1;
					}
					else {
						SaASCE = sflb*IntensityVector[first_rec];
						last_rec=first_rec;
						while((last_rec < IntensityVector.count()-1)&&(IntensityVector[last_rec+1]==IntensityVector[first_rec]))
							last_rec++;
					}
					if (getDisplayOutput()) 
						rDebug(threadID) << "Warning: There is no record with a SaASCE in the appropriate range: Rt is setting Sa = nearest appropriate Sa = " << SaASCE;
				}
			}
			/// select record
			srand (time(NULL));
			recnum = rand() % (last_rec-first_rec+1) + first_rec;
	
			if (getDisplayOutput()) {
				rDebug(threadID) << "Selected Record Number = " << (recnum+1) << " => Selected Record is: " << theRecords[recnum] << endl;	

			if (theRecordScaling == RSoilStructureInteractionModel::Unscaled)
				rDebug(threadID) << "Unscaled Record Sa = " << IntensityVector[recnum] << endl;
			else 
				rDebug(threadID) << "Given Sa = " << SaASCE << endl;

			}

			d2uff = recordAccVectorList[recnum];
			n = d2uff.count();
			dt = dtVector[recnum];   

			// Scaling the record to the given Sa -- [ acceleration data file (g) --> d2uff (m/s) ] 
			if (theRecordScaling == RSoilStructureInteractionModel::Unscaled){
				for (int i=0; i<d2uff.count() ;i++) {
					d2uff[i] = d2uff[i] * 9.81;
				}	
			} else{
				for (int i=0; i<d2uff.count() ;i++) {
					d2uff[i] = d2uff[i] * (SaASCE/IntensityVector[recnum]) * 9.81;
				}	
			}
		}//end of SDOF
		else{ //******************************************************* MDOF systems
			// Reading acceleration data files and sorting based on Spectrum
			if (recordAccVectorList.isEmpty()==true) {
				double lowerTn = 0.2 * meanTn;
				double upperTn = 1.5 * meanTn;
				double dtn=(upperTn-lowerTn)/10;
				QVector<double> SaASCEVector;
				QVector<double> TnASCEVector;
				for ( int i=0; i<11; i++){
					TnASCEVector.insert(i, lowerTn+i*dtn);
					if (TnASCEVector[i]<T0)
						SaASCEVector.insert(i,SdsASCE*(0.4+0.6*(TnASCEVector[i]/T0)));
					if (T0<TnASCEVector[i] && TnASCEVector[i]<Ts)
						SaASCEVector.insert(i, SdsASCE);
					if (Ts<TnASCEVector[i])
						SaASCEVector.insert(i,SdiASCE/TnASCEVector[i]);
				} // end of for ( int i=0; i<11; i++)
				int gsl_status;
				int la_status;
				int SDMatrix_status;
				double Mtotal=ms;	//Consider that all of mass will participate in the first mode
				double roh;
				double htotal;
				double h_story=3.2;
				gsl_vector *kstr = gsl_vector_calloc(Num);	//[k1/k1;k2/k1;...;ki/k1;...]
				gsl_matrix *Kfix = gsl_matrix_calloc(Num,Num);
				gsl_matrix *Cfix = gsl_matrix_calloc(Num,Num);
				gsl_matrix *Mfix = gsl_matrix_calloc(Num,Num);
				gsl_matrix *Lfix = gsl_matrix_calloc(Num,2);
				gsl_matrix *Kfix_l = gsl_matrix_calloc(Num,Num);
				gsl_matrix *Cfix_l = gsl_matrix_calloc(Num,Num);
				gsl_matrix *Mfix_l = gsl_matrix_calloc(Num,Num);
				gsl_matrix *Lfix_l = gsl_matrix_calloc(Num,2);
				SDMatrix_status = stiffnessAndDampingMatrixMDOF(Tn,vs,wn,h_r0,mbar,xi_s,Num,kstr,Kfix,Mfix,Cfix,&h_story,&htotal,Mtotal,&r0,&roh,&landam,&landah,threadID);
				if (SDMatrix_status) {  
					rCritical(threadID) << "Error in StiffnessAndDampingMatrixMDOF function";
					return -1;
		     	}
				double e=e_r0*r0;
				gsl_matrix_set_all(Lfix,1);
				for (int i = 0; i < theRecords.count(); i++) {
					QFile file(theRecords[i]);
					if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
						rCritical(threadID) << "Error: Could not open the file" << theRecords[i];
						return -1;
					}
					QTextStream data(&file);		
					QRegExp regExp = QRegExp("[ ,;\t]");
					int lineNumber = 0;
					QVector<double> recordAccVector;
					while (!data.atEnd()) {
						lineNumber++;
						QString line = data.readLine();
						QStringList rowList = line.split(regExp, QString::SkipEmptyParts);
						for (int j = 0; j < rowList.count(); j++) {
							bool ok;
							double doubleValue = rowList[j].toDouble(&ok);
							if (ok) {
								recordAccVector << doubleValue;

							} else {
								rCritical(threadID) << "Error in model" << objectName() << ": Could not read acceleration data file: " << theRecords[i] << " at line: " << lineNumber << " and column: " << j+1;
								return -1;
							}
						}
					} // end while (!data.atEnd())
					//finding maximum suedo accelaration for record considering 0.05 damping ratio
					n=recordAccVector.count();
					dt=theRecordTimeStepsVector[i];
					QVector<double> FIMhfix(n);	// Horizontal Foundation Input Motion
					QVector<double> FIMrfix(n);	// Rotational Foundation Input Motion
					FIMhfix=recordAccVector;
					FIMrfix.fill(0.0);
					gsl_vector *maxD_v=gsl_vector_calloc(Num);	//Maximum Elastic Accelaration
					gsl_vector *maxDtot_v=gsl_vector_calloc(Num);
					gsl_vector *maxA_v=gsl_vector_calloc(Num);
					gsl_vector *maxAtot_v=gsl_vector_calloc(Num);
					double Acc=0;
					double VeFix=0;
					la_status = linearAnalysisMDOF(h_story,e,kstr,Kfix, Cfix, Mfix, Lfix, FIMhfix, FIMrfix, n, dt, Num, Num , maxD_v, maxDtot_v, maxA_v, maxAtot_v,&VeFix,threadID);
					if (la_status) {  
						rCritical(threadID) << "Error in linearAnalysis function";
						return -1;
		     		}
					double maxD=gsl_vector_max(maxD_v);
					Acc=maxD*meanwn*meanwn; //maximum suedo accelaration

					//scaling each record based on Acc in order to prepare it for furthur calculations in order preiod rang
					for (int l=0; l<recordAccVector.count() ;l++) {
						recordAccVector[l] = recordAccVector[l] * (SaASCE/Acc);
					}
					QVector<double> FIMhfix_l(n);	// Horizontal Foundation Input Motion
					QVector<double> FIMrfix_l(n);	// Rotational Foundation Input Motion
					FIMhfix_l=recordAccVector;
					FIMrfix_l.fill(0.0);
					int l=0; bool delta = true;
					while ((l<TnASCEVector.count()) && (delta==true )) {
						double wn_l = 2*pi/TnASCEVector[l];		// Natural Frequency of The Structure
						SDMatrix_status = stiffnessAndDampingMatrixMDOF(TnASCEVector[l],vs,wn_l,h_r0,mbar,xi_s,Num,kstr,Kfix_l,Mfix_l,Cfix_l,&h_story,&htotal,Mtotal,&r0,&roh,&landam,&landah,threadID);
						if (SDMatrix_status) {  
							rCritical(threadID) << "Error in StiffnessAndDampingMatrixMDOF function";
							return -1;
		     			}
						gsl_matrix_set_all(Lfix_l,1);
						gsl_vector *maxD_v_l=gsl_vector_calloc(Num);	//Maximum Elastic Accelaration
						gsl_vector *maxDtot_v_l=gsl_vector_calloc(Num);
						gsl_vector *maxA_v_l=gsl_vector_calloc(Num);
						gsl_vector *maxAtot_v_l=gsl_vector_calloc(Num);
						double Acc_l=0;	//  Maximum Elastic suedo Accelaration 
						double VeFix=0;
						//linear Analysis for calculationg maxD ( considering Fix base )
						la_status = linearAnalysisMDOF(h_story,e,kstr,Kfix_l, Cfix_l, Mfix_l, Lfix_l, FIMhfix_l, FIMrfix_l, n, dt, Num, Num , maxD_v_l, maxDtot_v_l, maxA_v_l, maxAtot_v_l,&VeFix,threadID);
						if (la_status) {  
							rCritical(threadID) << "Error in linearAnalysis function";
							return -1;
						}
						double maxD_l=gsl_vector_max(maxD_v_l);
						Acc_l=wn_l*wn_l*maxD_l; //maximum suedo acceleration

						// Freeing allocated memory ( Linear Analysis )

						double delta_Sa =  (Acc_l-SaASCEVector[l])/SaASCEVector[l];
						if (qAbs(delta_Sa)>0.2){
							delta=false;
						} else {
							l++;
						}
					} 	// End of: while ((l<TnASCEVector.count()) && (delta==true ) 
					//all 10 checked point is ok then:
					if (delta==true){
						// returnig to unscaled mode:
						for (int l=0; l<recordAccVector.count() ;l++) {
							recordAccVector[l] = recordAccVector[l] * (Acc/SaASCE);
						}
						int j=0; bool findplace = false;
						while ( (j<IntensityVector.count()) && (findplace==false)){ 
							if (Acc > IntensityVector[j])
								j++;
							else
								findplace=true;
						}
						IntensityVector.insert(j, Acc);
						recordAccVectorList.insert(j, recordAccVector);
						dtVector.insert(j, dt);
					} //end for if (delta==true)
				} // end or i
				// Freeing allocated memories:
				gsl_matrix_free(Mfix);
				gsl_matrix_free(Kfix);
				gsl_matrix_free(Cfix);
				gsl_matrix_free(Lfix);
				gsl_matrix_free(Mfix_l);
				gsl_matrix_free(Kfix_l);
				gsl_matrix_free(Cfix_l);
				gsl_matrix_free(Lfix_l);
				gsl_vector_free(kstr);
			} // end if (recordAccVectorList.isEmpty()==true)

			if (recordAccVectorList.isEmpty()==true) {
				rCritical(threadID) << "Error: No record matches the scaling criteria ";
				return -1;
			}

			// Selecting the record based on Sa between the authorized records ( Randomly selecting a record with a Acc between 0.5*Sa and 2*Sa )
			int recnum = 0;
			int first_rec = 0;
			int last_rec = IntensityVector.count()-1;
			bool flag=true;
			if (theRecordScaling == RSoilStructureInteractionModel::Adaptive){
				if (sflb*IntensityVector[0]>SaASCE){
					SaASCE = sflb*IntensityVector[0];
					if (getDisplayOutput()) 
						rDebug(threadID) << "Warning: The Sa is too small: Rt is setting Sa = ScaleFactorLowerBound * minimum of record's Sas = " << SaASCE;
				}
				if (	sfub*IntensityVector[last_rec]<SaASCE){
					SaASCE = sfub*IntensityVector[last_rec];
					if (getDisplayOutput()) 
						rDebug(threadID) << "Warning: The SaASCE is too large: Rt is setting Sa = ScaleFactorUpperBound * maximum of record's Sas = " << SaASCE;
				}
				while((recnum <= last_rec)&&(flag==true)){
					if ( sfub*IntensityVector[recnum] >= SaASCE ){
						first_rec=recnum;
						flag=false;
					}
					else{
						recnum++;
					}
				}
				flag=true;	
				while((recnum <= last_rec)&&(flag==true)){
					if ( sflb*IntensityVector[recnum] > SaASCE){
						last_rec=recnum-1;
						flag=false;
					}
					else{
						recnum++;
					}
				}
				if (last_rec < first_rec){
					if ((SaASCE-sfub*IntensityVector[last_rec])<= (sflb*IntensityVector[first_rec]-SaASCE)){
						SaASCE = sfub*IntensityVector[last_rec];
						first_rec=last_rec;
						while((first_rec > 0)&&(IntensityVector[first_rec-1]==IntensityVector[last_rec]))
							first_rec=first_rec-1;
					}
					else {
						SaASCE = sflb*IntensityVector[first_rec];
						last_rec=first_rec;
						while((last_rec < IntensityVector.count()-1)&&(IntensityVector[last_rec+1]==IntensityVector[first_rec]))
							last_rec++;
					}
					if (getDisplayOutput()) 
						rDebug(threadID) << "Warning: There is no record with a SaASCE in the appropriate range: Rt is setting Sa = nearest appropriate Sa = " << SaASCE;
				}
			}
			/// select record
			srand (time(NULL));
			recnum = rand() % (last_rec-first_rec+1) + first_rec;
	
			if (getDisplayOutput()) {
				rDebug(threadID) << "Selected Record Number = " << (recnum+1) << " => Selected Record is: " << theRecords[recnum] << endl;	

			if (theRecordScaling == RSoilStructureInteractionModel::Unscaled)
				rDebug(threadID) << "Unscaled Record Sa = " << IntensityVector[recnum] << endl;
			else 
				rDebug(threadID) << "Given Sa = " << SaASCE << endl;

			}

			d2uff = recordAccVectorList[recnum];
			n = d2uff.count();
			dt = dtVector[recnum];   

			// Scaling the record to the given Sa -- [ acceleration data file (g) --> d2uff (m/s) ] 
			if (theRecordScaling == RSoilStructureInteractionModel::Unscaled){
				for (int i=0; i<d2uff.count() ;i++) {
					d2uff[i] = d2uff[i] * 9.81;
				}	
			} else{
				for (int i=0; i<d2uff.count() ;i++) {
					d2uff[i] = d2uff[i] * (SaASCE/IntensityVector[recnum]) * 9.81;
				}	
			}
		}//end of MDOF
	}//end of spectrum

	// Shortening record's time step if required
	double TnScale = 25;         // According to Bispec Program
	if (dt>(Tn/TnScale)){
		int divide = qCeil(dt/(Tn/TnScale));
		QVector<double> d2uff_new;
		for (int i=0; i <= n-2; i++){
			for (int j=0; j <= divide-1; j++){
				d2uff_new << (d2uff[i]+(d2uff[i+1]-d2uff[i])*j/divide);
			}
		}

		d2uff_new << d2uff[n-1];
		n=divide*(n-1)+1;
		dt=dt/divide;
		d2uff.resize(n);
		d2uff = d2uff_new;
	}

	// Make Records length even by adding zero at the end of it for fast fourier transform
	if (qFloor(n/2) != (n/2)){
		n=n+1;
		d2uff << 0;
	}
	// ----------------------------- Main Code -----------------------------

	// Defining required variables
	int gsl_status;
	int dofeq;
	if (dof==Num){
		dofeq=1;
	}else{
		dofeq=4;
	}
	gsl_matrix *K = gsl_matrix_calloc(dof,dof);
	gsl_matrix *C = gsl_matrix_calloc(dof,dof);
	gsl_matrix *M = gsl_matrix_calloc(dof,dof);
	gsl_matrix *L = gsl_matrix_calloc(dof,2);
	gsl_matrix *K_eq = gsl_matrix_calloc(dofeq,dofeq);
	gsl_matrix *C_eq = gsl_matrix_calloc(dofeq,dofeq);
	gsl_matrix *M_eq = gsl_matrix_calloc(dofeq,dofeq);
	gsl_matrix *L_eq = gsl_matrix_calloc(dofeq,2);

	double ks;
	double cs;
	double he;
	double he_eq;
	double Mtotal=ms;	//Consider that all of mass will participate in the first mode
	double roh=0;
	double htotal=0;
	QVector<double> FIMh(n);	// Horizontal Foundation Input Motion
	QVector<double> FIMr(n);	// Rotational Foundation Input Motion
	QVector<double> FIMhfix(n);	// Horizontal Foundation Input Motion
	QVector<double> FIMrfix(n);	// Rotational Foundation Input Motion
	//Equivalent SDOF
	QVector<double> FIMh_eq(n);	// Horizontal Foundation Input Motion
	QVector<double> FIMr_eq(n);	// Rotational Foundation Input Motion

	double THatAtc;
	double meanTHatAtc;
	double meanTHatASCE;
	double meanTHatNEHRP=meanTn;
	gsl_matrix *Kfix = gsl_matrix_calloc(Num,Num);
	gsl_matrix *Cfix = gsl_matrix_calloc(Num,Num);
	gsl_matrix *Mfix = gsl_matrix_calloc(Num,Num);
	gsl_matrix *Lfix = gsl_matrix_calloc(Num,2);
	gsl_matrix_set_all(Lfix,0);
	for (int  j=1;j<=Num;j++){
		gsl_matrix_set(Lfix,j-1,0,1);
	}
	FIMhfix=d2uff;
	FIMrfix.fill(0.0);
	//double h_story=3.2;
	double h_story;
	double e;
	double e_eq;
	double mbar_eq;
	double mratio_eq;
	double h_story_eq;
	double htotal_eq=0;
	double Mtotal_eq=Mtotal;	//Consider that all of mass will participate in the first mode
	double r0_eq;
	double roh_eq;
	double meanG, meanvs, meanhtotal, meanKr, meanKh;	//These parameteres should be defined here because they are used in NEHRP, so the definition shoulb be global
	gsl_vector *kstr = gsl_vector_calloc(Num);	//[kn/k1;k(n-1)/k1;...;ki/k1;...;k1/k1]
	gsl_vector *kstr_eq = gsl_vector_calloc(1);
	gsl_matrix *Kfix_eq = gsl_matrix_calloc(1,1);
	gsl_matrix *Cfix_eq = gsl_matrix_calloc(1,1);
	gsl_matrix *Mfix_eq = gsl_matrix_calloc(1,1);
	gsl_matrix *Lfix_eq = gsl_matrix_calloc(1,2);
	gsl_matrix_set(Lfix_eq,0,0,1);
	gsl_matrix_set(Lfix_eq,0,1,0);

	if (theMDOFOrSDOF == RSoilStructureInteractionModel::MDOF){ //MDOF
		int SDMatrix_status;
		//In MDOF, mbar is based on Mtotal and htotal. mratio is based on Mtotal too.
		SDMatrix_status = stiffnessAndDampingMatrixMDOF(Tn,vs,wn,h_r0,mbar,xi_s,Num,kstr,Kfix,Mfix,Cfix,&h_story,&htotal,Mtotal,&r0,&roh,&landam,&landah,threadID);
		if (SDMatrix_status) {  
			rCritical(threadID) << "Error in StiffnessAndDampingMatrixMDOF function";
			return -1;
		}
		e=e_r0*r0;
		//SDOFeq
		mbar_eq=mbar*landam/landah;
		mratio_eq=mratio/landam;
		SDMatrix_status = stiffnessAndDampingMatrixMDOF(Tn,vs,wn,h_r0,mbar_eq,xi_s,1,kstr_eq,Kfix_eq,Mfix_eq,Cfix_eq,&h_story_eq,&htotal_eq,Mtotal_eq,&r0_eq,&roh_eq,&landam,&landah,threadID);
		if (SDMatrix_status) {  
			rCritical(threadID) << "Error in StiffnessAndDampingMatrixMDOF function";
			return -1;
		}
		if ((landam!=1) || (landah!=1)){
			rCritical(threadID) << "Error in finding equivalent SDOF";
			return -1;
		}
	} else {        //SDOF
		ks = ms*wn*wn;		// Stiffness of the structure based on SDOF
		cs = 2*xi_s*ms*wn;	// Damping of the structure based on SDOF
		htotal=h_r0*r0;						// Effective Height of the Structure
		roh=(1/mbar)*ms/(htotal*r0*r0);       // Density of The Soil
		Mtotal=ms;						//Mass of total structure
		gsl_matrix_set(Mfix,0,0,ms);
		gsl_matrix_set(Cfix,0,0,cs);
		gsl_matrix_set(Kfix,0,0,ks);
	}//end of if theMDOFOrSDOF
	
	if (dof==Num){		  // Fixed Base Structure
		gsl_matrix_memcpy(M,Mfix);
		gsl_matrix_memcpy(C,Cfix);
		gsl_matrix_memcpy(K,Kfix);
		gsl_matrix_memcpy(L,Lfix);
		gsl_matrix_memcpy(M_eq,Mfix_eq);
		gsl_matrix_memcpy(C_eq,Cfix_eq);
		gsl_matrix_memcpy(K_eq,Kfix_eq);
		gsl_matrix_memcpy(L_eq,Lfix_eq);
		he=0.0;
		FIMh=d2uff;
		FIMr.fill(0.0);
		FIMh_eq=d2uff;
		FIMr_eq.fill(0.0);
	}
	else{   // if (dof==Num+3)      // Flexible Base Structure
		gsl_matrix_set(L,dof-3,0,1); //L for flexible base
		gsl_matrix_set(L,dof-2,1,1);
		gsl_matrix_set(L,dof-1,1,1);
		gsl_matrix_set(L_eq,1,0,1); //L for flexible base
		gsl_matrix_set(L_eq,2,1,1);
		gsl_matrix_set(L_eq,3,1,1);

		// Definition and calculation of mean of initial parameteres which are used to design based on regulations
		double meanhtotal;
		double meanroh;
		if (theMDOFOrSDOF == RSoilStructureInteractionModel::SDOF){
			meanhtotal = meanh_r0 * r0;
			meanroh=(1/meanmbar)*ms/(meanhtotal*r0*r0);	       // Density of The Soil
		}
		else{
			meanhtotal = htotal;
			meanroh = roh;	       // Density of The Soil
		}
		double meane= meane_r0 * r0;
		double meana0fix;
		if (theSSISeverityMeasure == RSoilStructureInteractionModel::DimensionlessFrequency){
			meana0fix=meana0orvs;
			meanvs=meanwn*meanhtotal/meana0fix;     // Shear Velocity with Soil Hysteretic Material Damping
		} else {
			meanvs=RF*meana0orvs;
			meana0fix=(meanwn)*(meanhtotal)/meanvs;
		}
		meanG=meanroh*meanvs*meanvs;                    // Shear Modulus of The Soil 
		// Definition and calculation of initial parameteres which are used to in our SSI code
		double w0, xi0_w0, e, mf, m0r, c0h, c0r, c1r, fc, k0h, meank0h, k0r, meank0r, fk, Is, If, I1r; 
		double G, z0h, vp, z0r, Kh, Kr, df;

		mf=mratio*Mtotal;       // Mass of The Foundation
		e=e_r0*r0;          // Foundation Embedment
		he=htotal+e;             // Effective Height with Respect to the Center of Rotation							
		Is=0.25*Mtotal*r0*r0;          // Mass of Inertia of The Structure
		If=0.25*mf*r0*r0;               // Mass of Inertia of The Foundation
		G=roh*vs*vs;                             // Shear Modulus of The Soil 
		z0h=r0*pi/8*(2.0-v);                      // Aspect Ratio of Translational Cone

		if (v<1/3){
			vp=vs*qSqrt(2*(1.0-v)/(1.0-2*v));        // Dilatational Velocity
			z0r=r0*9*pi/16*(1.0-v)*(1.0-v)/(1.0-2*v);     // Apect Ratio of Rocking Cone
		}
		else{ //if (v>=1/3)
			vp=2*vs;
			z0r=r0*9*pi/8*(1.0-v);
		}
		Kh=8*G*r0/(2.0-v);                // Translational Static Stiffness  baraye paye sathi
		meanKh=8*meanG*r0/(2.0-meanv);
		Kr=8*G*r0*r0*r0/(3*(1.0-v));          // Rocking Static Stiffness baraye peye sathi
		meanKr=8*meanG*r0*r0*r0/(3*(1.0-meanv));
		if (e!=0){ //Embedded Foundation && dof=Num+3

			// d2uwff=fft(d2uff)/n;        // Free Field Acceleration in Freq. Domain
			df=1.0/(n*dt);         // Frequency Interval for Fourier Transform
			gsl_vector_complex *d2uwff=gsl_vector_complex_alloc(n);		
			gsl_complex tempc;
			double *data=new double[2*n];
			gsl_fft_complex_workspace *work = gsl_fft_complex_workspace_alloc (n);
			gsl_fft_complex_wavetable *wave = gsl_fft_complex_wavetable_alloc (n);
			for (int i = 0; i < n; i++){ 
				data[2*i] = d2uff[i];
				data[2*i+1] = 0.0;
			}
			gsl_fft_complex_forward(data,1,n,wave,work);
			for(int i=0; i<n ; i++){
				tempc = gsl_complex_rect(data[2*i],data[2*i+1]);
				tempc = gsl_complex_div_real(tempc,n);
				gsl_vector_complex_set(d2uwff,i,tempc);
			}

			double a0max = 4;
			int ndisk = qFloor(5/pi*e_r0*a0max+1)+1;       // Number of Required Disks,Thickness of each layer is less than 0.1 wave length
			double de=e/(ndisk-1);

			gsl_vector_complex *ee = gsl_vector_complex_alloc(ndisk);
			for (int i=0 ; i<ndisk ; i++){
				tempc = gsl_complex_rect(de*(ndisk-1-i),0.0);
				gsl_vector_complex_set(ee,i,tempc);       // Depth of Each Layer
			}

			gsl_vector_complex *one = gsl_vector_complex_alloc(ndisk);
			tempc = gsl_complex_rect(1.0,0.0);
			gsl_vector_complex_set_all(one,tempc);

			gsl_matrix_complex *A = gsl_matrix_complex_alloc(ndisk,2);       // Transformation Matrix
			gsl_status = gsl_matrix_complex_set_col(A,0,one);	//A(:,1)=one;
			if (gsl_status) {  
				rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
				return -1;
			}
			gsl_status = gsl_matrix_complex_set_col (A, 1, ee);		//A(:,2)=ee;
			if (gsl_status) {  
				rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
				return -1;
			}

			gsl_vector_complex *ufHarm = gsl_vector_complex_alloc(ndisk);          // Harmonic Motion Vector at Each Depth              
			gsl_vector_complex *d2ugw = gsl_vector_complex_alloc(n);               // Horizontal FIM in Freq. Domain
			gsl_vector_complex *d2phigw = gsl_vector_complex_alloc(n);             // Rocking FIM in Freq. Domain
			gsl_vector_complex *d2u0gw = gsl_vector_complex_alloc(2);
			gsl_matrix_complex *Gh = gsl_matrix_complex_alloc(ndisk,ndisk);      // Horizontal Green's Function
			gsl_matrix_complex *Gr = gsl_matrix_complex_calloc(ndisk,ndisk);      // Rocking Green's Function
			gsl_matrix_complex *S00g = gsl_matrix_complex_calloc(2,2);
			gsl_matrix_complex *Sffr = gsl_matrix_complex_alloc(ndisk,ndisk);      // Rocking Inverse Green's Function	
			gsl_matrix_complex *Sffh = gsl_matrix_complex_alloc(ndisk,ndisk);      // Horizontal Inverse Green's Function	

			for (int i=0 ; i <= n/2-1 ; i++){
				double w=2*pi*df*i;
				double b0h=w*z0h/vs;
				double b0r=w*z0r/vp;

				for (int j=0; j < ndisk ; j++){
					for (int k=0; k < ndisk ; k++){
						double dist1 = qAbs(j-k)*de;
						double dist2 = (j+k)*de;
						gsl_complex tempc2, tempc3;

						//Gh(j,k)=1/(2*Kh)*1/(1+sqrt(-1)*b0h)*(exp(-sqrt(-1)*w*dist1/vs)/(1+dist1/z0h)+exp(-sqrt(-1)*w*dist2/vs)/(1+dist2/z0h));
						tempc = gsl_complex_rect(0.0,-1*w*dist2/vs);
						tempc = gsl_complex_exp(tempc);
						tempc = gsl_complex_div_real(tempc,(1+dist2/z0h));
						tempc2 = gsl_complex_rect(0.0,-1*w*dist1/vs);
						tempc2 = gsl_complex_exp(tempc2);
						tempc2 = gsl_complex_div_real(tempc2,(1+dist1/z0h));
						tempc = gsl_complex_add(tempc,tempc2);
						tempc2 = gsl_complex_rect(1.0,b0h);
						tempc = gsl_complex_div(tempc,tempc2);
						tempc = gsl_complex_div_real(tempc,2*Kh);
						gsl_matrix_complex_set(Gh,j,k,tempc);	

						//Gr(j,k)=1/(2*Kr)*1/(1+sqrt(-1)*b0r-b0r^2/3)*(exp(-sqrt(-1)*w*dist1/vp)*(1/(1+dist1/z0r)^3+sqrt(-1)*b0r*1/(1+dist1/z0r)^2)+exp(-sqrt(-1)*w*dist2/vp)*(1/(1+dist2/z0r)^3+sqrt(-1)*b0r*1/(1+dist2/z0r)^2));
						tempc = gsl_complex_rect(1.0/gsl_pow_3(1+dist2/z0r),b0r/gsl_pow_2(1.0+dist2/z0r));
						tempc2 = gsl_complex_rect(0.0,-1*w*dist2/vp);
						tempc2 = gsl_complex_exp(tempc2);
						tempc = gsl_complex_mul(tempc,tempc2);
						tempc2 = gsl_complex_rect(1.0/gsl_pow_3(1+dist1/z0r),b0r/gsl_pow_2(1.0+dist1/z0r));
						tempc3 = gsl_complex_rect(0.0,-1*w*dist1/vp);
						tempc3 = gsl_complex_exp(tempc3);
						tempc2 = gsl_complex_mul(tempc2,tempc3);
						tempc = gsl_complex_add(tempc,tempc2);
						tempc2 = gsl_complex_rect(1.0-b0r*b0r/3,b0r);
						tempc = gsl_complex_div(tempc,tempc2);
						tempc = gsl_complex_div_real(tempc,2*Kr);	
						gsl_matrix_complex_set(Gr,j,k,tempc);
					} // end for k
					gsl_vector_complex_set(ufHarm,j, gsl_complex_rect(qCos(w*j*de/vs),0.0));
				} // end for j

				//Sffh=inv(Gh);
				int s;
				gsl_permutation *p = gsl_permutation_calloc(ndisk);
				gsl_status = gsl_linalg_complex_LU_decomp(Gh,p,&s);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}
				gsl_status = gsl_linalg_complex_LU_invert (Gh, p, Sffh);				
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}

				//Sffr=inv(Gr);
				gsl_status = gsl_linalg_complex_LU_decomp(Gr,p,&s);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}
				gsl_status = gsl_linalg_complex_LU_invert (Gr, p, Sffr);				
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}
				gsl_permutation_free(p);

				double mdisk = roh*pi*r0*r0*e;
				gsl_complex tempc2;
				gsl_complex one_c = gsl_complex_rect(1.0,0.0);
				gsl_complex zero_c = gsl_complex_rect(0.0,0.0);
				gsl_vector_complex *tempvc = gsl_vector_complex_alloc(ndisk);

				//S00g(1,1) = one'*Sffh*one + w^2*mdisk;
				gsl_status = gsl_blas_zgemv(CblasNoTrans,one_c,Sffh,one,zero_c,tempvc);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}
				gsl_status = gsl_blas_zdotu(one,tempvc,&tempc);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}
				tempc = gsl_complex_add_real(tempc,w*w*mdisk);
				gsl_matrix_complex_set(S00g,0,0,tempc);

				// S00g(2,2) = ee'*Sffh*ee + one'*Sffr*one + w^2*mdisk*e^2/3+w^2*mdisk*r0^2/4;
				gsl_status = gsl_blas_zgemv(CblasNoTrans,one_c,Sffh,ee,zero_c,tempvc);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}	
				gsl_status = gsl_blas_zdotu(ee,tempvc,&tempc);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}
				gsl_status = gsl_blas_zgemv(CblasNoTrans,one_c,Sffr,one,zero_c,tempvc);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}
				gsl_status = gsl_blas_zdotu(one,tempvc,&tempc2);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}
				tempc = gsl_complex_add(tempc, tempc2);
				tempc = gsl_complex_add_real(tempc, w*w*mdisk*e*e/3+w*w*mdisk*r0*r0/4);
				gsl_matrix_complex_set(S00g,1,1,tempc);

				// S00g(1,2) = ee'*Sffh*one + w^2*mdisk*e;	S00g(2,1)=S00g(1,2);
				gsl_status = gsl_blas_zgemv(CblasNoTrans,one_c,Sffh,one,zero_c,tempvc);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}
				gsl_status = gsl_blas_zdotu(ee,tempvc,&tempc);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}
				tempc = gsl_complex_add_real(tempc,w*w*mdisk*e);
				gsl_matrix_complex_set(S00g,0,1,tempc);
				gsl_matrix_complex_set(S00g,1,0,tempc);

				//d2u0gw=inv(S00g)*A'*Sffh*ufHarm;
				p = gsl_permutation_calloc(2);
				gsl_matrix_complex *invS00g = gsl_matrix_complex_alloc(2,2);
				gsl_status = gsl_linalg_complex_LU_decomp(S00g,p,&s);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}
				gsl_status = gsl_linalg_complex_LU_invert (S00g, p, invS00g);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}
				gsl_permutation_free(p);

				gsl_matrix_complex *tempmc = gsl_matrix_complex_calloc(2,ndisk);				
				gsl_status = gsl_blas_zgemm(CblasNoTrans,CblasTrans,one_c,invS00g,A,zero_c,tempmc);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}
				gsl_matrix_complex *tempmc2 = gsl_matrix_complex_calloc(2,ndisk);				
				gsl_status = gsl_blas_zgemm(CblasNoTrans,CblasNoTrans,one_c,tempmc,Sffh,zero_c,tempmc2);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}
				gsl_status = gsl_blas_zgemv(CblasNoTrans,one_c,tempmc2,ufHarm,zero_c,d2u0gw);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}

				//d2ugw(i+1)=d2u0gw(1)*d2uwff(i+1);
				tempc=gsl_vector_complex_get(d2u0gw,0);
				tempc2=gsl_vector_complex_get(d2uwff,i);
				tempc = gsl_complex_mul(tempc,tempc2);
				gsl_vector_complex_set(d2ugw,i,tempc);

				//d2phigw(i+1)=d2u0gw(2)*d2uwff(i+1);
				tempc=gsl_vector_complex_get(d2u0gw,1);
				tempc2=gsl_vector_complex_get(d2uwff,i);
				tempc = gsl_complex_mul(tempc,tempc2);
				gsl_vector_complex_set(d2phigw,i,tempc);

				gsl_matrix_complex_free(invS00g);
				gsl_matrix_complex_free(tempmc);
				gsl_matrix_complex_free(tempmc2);
				gsl_vector_complex_free(tempvc);
			} // end for i

			gsl_matrix_complex_free(A);
			gsl_vector_complex_free(ee);
			gsl_vector_complex_free(one);
			gsl_matrix_complex_free(Gh);
			gsl_matrix_complex_free(Gr);
			gsl_matrix_complex_free(Sffh);
			gsl_matrix_complex_free(Sffr);
			gsl_matrix_complex_free(S00g);

			//	FIMh=ifft(d2ugw,'symmetric')*n;
			data[0]=GSL_REAL(gsl_vector_complex_get(d2ugw,0));
			data[1]=0.0;
			for (int i = 1; i <= n/2-1; i++){ 
				tempc = gsl_vector_complex_get(d2ugw,i);
				data[2*i] = GSL_REAL(tempc);
				data[2*(n-i)] = data[2*i];
				data[2*i+1] = GSL_IMAG(tempc);
				data[2*(n-i)+1] = (-1)*data[2*i+1];
			}
			data[n]=data[n-2];
			data[n+1]=0.0;
			gsl_fft_complex_inverse(data,1,n,wave,work);
			for(int i=0; i<n ; i++){
				FIMh[i] = data[2*i]*n;
			}

			//	FIMr=ifft(d2phigw,'symmetric')*n;
			data[0]=GSL_REAL(gsl_vector_complex_get(d2phigw,0));
			data[1]=0.0;
			for (int i = 1; i <= n/2-1; i++){ 
				tempc = gsl_vector_complex_get(d2phigw,i);
				data[2*i] = GSL_REAL(tempc);
				data[2*(n-i)] = data[2*i];
				data[2*i+1] = GSL_IMAG(tempc);
				data[2*(n-i)+1] = (-1)*data[2*i+1];
			}
			data[n]=data[n-2];
			data[n+1]=0.0;
			gsl_fft_complex_inverse(data,1,n,wave,work);
			for(int i=0; i<n ; i++){
				FIMr[i] = data[2*i]*n;
			}

			gsl_fft_complex_workspace_free(work);
			gsl_fft_complex_wavetable_free(wave);
			delete[] data;

			gsl_vector_complex_free(d2u0gw);
			gsl_vector_complex_free(d2ugw);
			gsl_vector_complex_free(d2phigw);
			gsl_vector_complex_free(ufHarm);
			gsl_vector_complex_free(d2uwff);
		}
		else {//if (e==0) Surface Foundtion && dof=Num+3
			FIMh=d2uff;
			FIMr.fill(0.0);
		}
		if (e==0){	// Surface Foundation
			double gama0h=0.78-0.4*v;
			double gama1r=0.42-0.3*v*v;
			double mu0r;
			if (v<1/3){
				mu0r=0;
			}
			else{ //if (v>=1/3)
				mu0r=0.16*(v-1/3);
			}
			double mu1r=0.34-0.2*v*v;
			k0h=Kh;
			meank0h=meanKh;
			k0r=Kr;
			meank0r=meanKr;
			c0h=r0/vs*gama0h*k0h;
			c0r=0;
			c1r=r0/vs*gama1r*k0r;
			m0r=gsl_pow_2(r0/vs)*mu0r*k0r;
			I1r=gsl_pow_2(r0/vs)*mu1r*k0r;
			fk=0;
			fc=0;
		}
		else { //if (e!=0)	// Embedded Foundation
			double gama0h=0.68+0.57*qSqrt(e_r0);
			double gama0r=0.15631*e_r0-0.08906*gsl_pow_2(e_r0)-0.00874*gsl_pow_3(e_r0);
			double gama1r=0.4+0.03*gsl_pow_2(e_r0);
			double mu1r=0.33+0.1*gsl_pow_2(e_r0);
			fk=e/4;
			fc=0.32*e+0.03*e*gsl_pow_2(e_r0);
			k0h=Kh*(1+e_r0);
			meank0h=meanKh*(1+meane_r0);
			double kr_embedded=Kr*(1+2.3*e_r0+0.58*gsl_pow_3(e_r0));
			double meankr_embedded=meanKr*(1+2.3*meane_r0+0.58*gsl_pow_3(meane_r0));
			k0r=kr_embedded-G*gsl_pow_3(r0)/(2*(2.0-v))*(1+e_r0)*gsl_pow_2(e_r0);
			meank0r=meankr_embedded-meanG*gsl_pow_3(r0)/(2*(2.0-meanv))*(1+meane_r0)*gsl_pow_2(meane_r0);
			c0h=r0/vs*gama0h*k0h;
			c0r=r0/vs*gama0r*kr_embedded;
			c1r=r0/vs*gama1r*kr_embedded;       // Damping of Internal Degree of Freedom
			m0r=0;
			I1r=gsl_pow_2(r0/vs)*mu1r*kr_embedded;     // Mass of Inertia Internal Degree of Freedom
		}
		double THat=0.0;
		if (theMDOFOrSDOF == RSoilStructureInteractionModel::SDOF){
			//------------- Computing Average of TSSI Using ATC Formultion
			double meankyatc=(1+2*meane/(3*r0))*8*meanG*r0/(2-meanv);	// Ky for using in ATC formulation
			double meanktatc=(1+2*meane/r0)*8*meanG*r0*r0*r0/(3-3*meanv);	// Kteta for using in ATC formulation
			meanTHatAtc =  meanTn*qSqrt(1+meanks/meankyatc*(1+meankyatc*meanhtotal*meanhtotal/meanktatc));	// T using ATC formulation
			//------------- Computing TSSI Using ASCE and NEHRP Formultions
			meanTHatASCE =  meanTn*qSqrt(1+meanks/meank0h*(1+meank0h*meanhtotal*meanhtotal/meank0r));	// T using ASCE formulation
			meanTHatNEHRP =  meanTn*qSqrt(1+meanks/meank0h*(1+meank0h*meanhtotal*meanhtotal/meank0r));	// T using ASCE formulation
			THat =  Tn*qSqrt(1+ks/k0h*(1+k0h*h*h/k0r));	// THat based on ConeModel		
		}else{
			/*ks=(2*pi/Tn)*(2*pi/Tn)*Mtotal;
			double kyatc=(1+2*e/(3*r0))*8*G*r0/(2-v);	// Ky for using in ATC formulation
			double ktatc=(1+2*e/r0)*8*G*r0*r0*r0/(3-3*v);	// Kteta for using in ATC formulation
			THatAtc =  Tn*qSqrt(1+ks/kyatc*(1+kyatc*h*h/ktatc));	// T using ATC formulation
			THat = THatAtc;*/
			//ASCE7-10 (19.2-5)
			double al;
			al = mbar_eq/pi;
			double ateta=1.0;
			double f=r0/(vs*Tn);
			if (f<0.05){
				ateta=1.0;
			}
			if ((f>=0.05) && (f<=0.15)){
				ateta=-1.5*f+1.075;
			}
			if ((f>=0.15) && (f<=0.35)){
				ateta=-0.75*f+0.9625;
			}
			if ((f>=0.35) && (f<=0.5)){
				ateta=0.6+(-2*f+1)/3;
			}
			if (f>0.5){
				ateta=0.6;
			}
			THat=Tn*qSqrt(1+((25*al*r0*h)/(vs*vs*Tn*Tn))*(1+(1.12*r0*h*h)/(ateta*r0*r0*r0)));
		}
		
		w0=vs/r0;
		double xi_0_modified=xi_0*(THat/Tn)*h_r0/a0fix;
		xi0_w0=xi_0_modified/w0;

	
		if (theMDOFOrSDOF == RSoilStructureInteractionModel::SDOF){	//SDOF
			//M=[ms ms ms*he 0;
			//ms ms+mf+c0h*xi0_w0 ms*he+mf*e*fcr+c0h*xi0_w0*fc 0;
			//ms*he ms*he+mf*e*fcr+c0h*xi0_w0*fc ms*he^2+mf*e^2/4+c0h*xi0_w0*fc^2+c0r*xi0_w0+c1r*xi0_w0+Is+If+m0r -c1r*xi0_w0;
			//0 0 -c1r*xi0_w0 c1r*xi0_w0+I1r];		
			gsl_matrix_set(M,0,0,ms);
			gsl_matrix_set(M,0,1,ms);
			gsl_matrix_set(M,0,2,ms*he);
			gsl_matrix_set(M,0,3,0.0);

			gsl_matrix_set(M,1,0,ms);
			gsl_matrix_set(M,1,1,ms+mf+c0h*xi0_w0);
			gsl_matrix_set(M,1,2,ms*he+mf*e*fcr+c0h*xi0_w0*fc);
			gsl_matrix_set(M,1,3,0.0);

			gsl_matrix_set(M,2,0,ms*he);
			gsl_matrix_set(M,2,1,ms*he+mf*e*fcr+c0h*xi0_w0*fc);
			gsl_matrix_set(M,2,2,ms*he*he+mf*e*e/4+c0h*xi0_w0*fc*fc+c0r*xi0_w0+c1r*xi0_w0+Is+If+m0r);
			gsl_matrix_set(M,2,3,-1*c1r*xi0_w0);

			gsl_matrix_set(M,3,0,0.0);
			gsl_matrix_set(M,3,1,0.0);
			gsl_matrix_set(M,3,2,-1*c1r*xi0_w0);
			gsl_matrix_set(M,3,3,c1r*xi0_w0+I1r);

			//C=[cs 0 0 0;
			//0 c0h+2*k0h*xi0_w0 c0h*fc+2*k0h*xi0_w0*fk 0;
			//0 c0h*fc+2*k0h*xi0_w0*fk c0h*fc^2+2*k0h*xi0_w0*fk^2+c0r+2*k0r*xi0_w0+c1r -c1r;
			//0 0 -c1r c1r];
			gsl_matrix_set(C,0,0,cs);
			gsl_matrix_set(C,0,1,0.0);
			gsl_matrix_set(C,0,2,0.0);
			gsl_matrix_set(C,0,3,0.0);

			gsl_matrix_set(C,1,0,0.0);
			gsl_matrix_set(C,1,1,c0h+2*k0h*xi0_w0);
			gsl_matrix_set(C,1,2,c0h*fc+2*k0h*xi0_w0*fk);
			gsl_matrix_set(C,1,3,0.0);

			gsl_matrix_set(C,2,0,0.0);
			gsl_matrix_set(C,2,1,c0h*fc+2*k0h*xi0_w0*fk);
			gsl_matrix_set(C,2,2,c0h*fc*fc+2*k0h*xi0_w0*fk*fk+c0r+2*k0r*xi0_w0+c1r);
			gsl_matrix_set(C,2,3,-1*c1r);

			gsl_matrix_set(C,3,0,0.0);
			gsl_matrix_set(C,3,1,0.0);
			gsl_matrix_set(C,3,2,-1*c1r);
			gsl_matrix_set(C,3,3,c1r);

			//K=[ks 0 0 0;
			//0 k0h k0h*fk 0;
			//0 k0h*fk k0h*fk^2+k0r 0;
			//0 0 0 0];
			gsl_matrix_set(K,0,0,ks);
			gsl_matrix_set(K,0,1,0.0);
			gsl_matrix_set(K,0,2,0.0);
			gsl_matrix_set(K,0,3,0.0);

			gsl_matrix_set(K,1,0,0);
			gsl_matrix_set(K,1,1,k0h);
			gsl_matrix_set(K,1,2,k0h*fk);
			gsl_matrix_set(K,1,3,0.0);

			gsl_matrix_set(K,2,0,0.0);
			gsl_matrix_set(K,2,1,k0h*fk);
			gsl_matrix_set(K,2,2,k0h*fk*fk+k0r);
			gsl_matrix_set(K,2,3,0.0);

			gsl_matrix_set(K,3,0,0.0);
			gsl_matrix_set(K,3,1,0.0);
			gsl_matrix_set(K,3,2,0.0);
			gsl_matrix_set(K,3,3,0.0);

		}
		else{	//MDOF
			for (int j=1;j<=Num;j++){
				for (int i=1;i<=Num;i++){
					gsl_matrix_set(M,i-1,j-1,gsl_matrix_get(Mfix,i-1,j-1));	//Mss
					gsl_matrix_set(K,i-1,j-1,gsl_matrix_get(Kfix,i-1,j-1));	//Kss
					gsl_matrix_set(C,i-1,j-1,gsl_matrix_get(Cfix,i-1,j-1));	//Css
				}
			}

			//Mso,Mos
			for (int i=1;i<=Num;i++){
				gsl_matrix_set(M,i-1,Num,gsl_matrix_get(Mfix,i-1,i-1));
				gsl_matrix_set(M,i-1,Num+1,gsl_matrix_get(Mfix,i-1,i-1)*(e+h_story*(Num-i+1)));//[mn*(Hn+e)]
				gsl_matrix_set(M,Num,i-1,gsl_matrix_get(Mfix,i-1,i-1));
				gsl_matrix_set(M,Num+1,i-1,gsl_matrix_get(Mfix,i-1,i-1)*(e+h_story*(Num-i+1)));
			}
			//Moo
			double sigmaMi=0;
			double sigmaMiHi=0;
			double sigmaMiHi2=0;
			for (int i=1;i<=Num;i++){
				sigmaMi=sigmaMi+gsl_matrix_get(Mfix,i-1,i-1);
				sigmaMiHi=sigmaMiHi+gsl_matrix_get(Mfix,i-1,i-1)*(h_story*(Num-i+1));
				sigmaMiHi2=sigmaMiHi2+gsl_matrix_get(Mfix,i-1,i-1)*(h_story*(Num-i+1)+e)*(h_story*(Num-i+1)+e);
			}
			gsl_matrix_set(M,Num,Num,sigmaMi+mf+c0h*xi0_w0);
			gsl_matrix_set(M,Num,Num+1,sigmaMiHi+sigmaMi*e+mf*e*fcr+c0h*xi0_w0*fc);

			gsl_matrix_set(M,Num+1,Num,sigmaMiHi+sigmaMi*e+mf*e*fcr+c0h*xi0_w0*fc);
			gsl_matrix_set(M,Num+1,Num+1,sigmaMiHi2+mf*e*e/4+Is+If+c0h*xi0_w0*fc*fc+(c0r+c1r)*xi0_w0+m0r);
			gsl_matrix_set(M,Num+1,Num+2,-1*c1r*xi0_w0);

			gsl_matrix_set(M,Num+2,Num+1,-1*c1r*xi0_w0);
			gsl_matrix_set(M,Num+2,Num+2,c1r*xi0_w0+I1r);

			//Coo
			gsl_matrix_set(C,Num,Num,c0h+2*k0h*xi0_w0);
			gsl_matrix_set(C,Num,Num+1,c0h*fc+2*k0h*xi0_w0*fk);
			
			gsl_matrix_set(C,Num+1,Num,c0h*fc+2*k0h*xi0_w0*fk);
			gsl_matrix_set(C,Num+1,Num+1,c0h*fc*fc+2*k0h*xi0_w0*fk*fk+c0r+2*k0r*xi0_w0+c1r);
			gsl_matrix_set(C,Num+1,Num+2,-1*c1r);

			gsl_matrix_set(C,Num+2,Num+1,-1*c1r);
			gsl_matrix_set(C,Num+2,Num+2,c1r);

			//Koo
			gsl_matrix_set(K,Num,Num,k0h);
			gsl_matrix_set(K,Num,Num+1,k0h*fk);

			gsl_matrix_set(K,Num+1,Num,k0h*fk);
			gsl_matrix_set(K,Num+1,Num+1,k0h*fk*fk+k0r);
		}

		if (theMDOFOrSDOF == RSoilStructureInteractionModel::MDOF){	//Equivalent SDOF
			if (dofeq==4){
				// Definition and calculation of mean of initial parameteres which are used to design based on regulations
				double meanhtotal_eq = htotal_eq;
				double meanroh_eq = roh_eq;	 // Density of The Soil      
				double meane_eq= meane_r0 * r0_eq;
				double meanvs_eq, meana0fix_eq;
				if (theSSISeverityMeasure == RSoilStructureInteractionModel::DimensionlessFrequency){
					meana0fix_eq=meana0orvs;
					meanvs_eq=meanwn*meanhtotal_eq/meana0fix_eq;     // Shear Velocity with Soil Hysteretic Material Damping
				} else {
					meanvs_eq=RF*meana0orvs;
					meana0fix_eq=(meanwn)*(meanhtotal)/meanvs;
				}
				double meanG_eq=meanroh_eq*meanvs_eq*meanvs_eq;                    // Shear Modulus of The Soil 
				// Definition and calculation of initial parameteres which are used to in our SSI code
				double w0_eq, xi0_w0_eq, mf_eq, m0r_eq, c0h_eq, c0r_eq, c1r_eq, fc_eq, k0h_eq, k0r_eq, fk_eq, Is_eq, If_eq, I1r_eq; 
				double G_eq, z0h_eq, vp_eq, z0r_eq, Kh_eq, Kr_eq, df_eq;

				mf_eq=mratio_eq*Mtotal_eq;       // Mass of The Foundation
				e_eq=e_r0*r0_eq;          // Foundation Embedment
				he_eq=htotal_eq+e_eq;             // Effective Height with Respect to the Center of Rotation							
				Is_eq=0.25*Mtotal_eq*r0_eq*r0_eq;          // Mass of Inertia of The Structure
				If_eq=0.25*mf_eq*r0_eq*r0_eq;               // Mass of Inertia of The Foundation
				G_eq=roh_eq*vs*vs;                             // Shear Modulus of The Soil 
				z0h_eq=r0_eq*pi/8*(2.0-v);                      // Aspect Ratio of Translational Cone

				if (v<1/3){
					vp_eq=vs*qSqrt(2*(1.0-v)/(1.0-2*v));        // Dilatational Velocity
					z0r_eq=r0_eq*9*pi/16*(1.0-v)*(1.0-v)/(1.0-2*v);     // Apect Ratio of Rocking Cone
				}
				else{ //if (v>=1/3)
					vp_eq=2*vs;
					z0r_eq=r0*9*pi/8*(1.0-v);
				}
				Kh_eq=8*G_eq*r0_eq/(2.0-v);                // Translational Static Stiffness
				Kr_eq=8*G_eq*r0_eq*r0_eq*r0_eq/(3*(1.0-v));          // Rocking Static Stiffness
				/*double ks_eq=(2*pi/Tn)*(2*pi/Tn)*Mtotal_eq;
				double kyatc_eq=(1+2*e_eq/(3*r0_eq))*8*G_eq*r0_eq/(2-v);	// Ky for using in ATC formulation
				double ktatc_eq=(1+2*e_eq/r0_eq)*8*G_eq*r0_eq*r0_eq*r0_eq/(3-3*v);	// Kteta for using in ATC formulation
				double THatAtc_eq =  Tn*qSqrt(1+ks_eq/kyatc_eq*(1+kyatc_eq*meanhtotal_eq*meanhtotal_eq/ktatc_eq));	// T using ATC formulation
				double THat_eq = THatAtc_eq;*/
				double al;
				al = mbar_eq/pi;
				double ateta=1.0;
				double f=r0_eq/(vs*Tn);
				if (f<0.05){
					ateta=1.0;
				}
				if ((f>=0.05) && (f<=0.15)){
					ateta=-1.5*f+1.075;
				}
				if ((f>=0.15) && (f<=0.35)){
					ateta=-0.75*f+0.9625;
				}
				if ((f>=0.35) && (f<=0.5)){
					ateta=0.6+(-2*f+1)/3;
				}
				if (f>0.5){
					ateta=0.6;
				}
				double THat_eq=Tn*qSqrt(1+((25*al*r0_eq*h)/(vs*vs*Tn*Tn))*(1+(1.12*r0_eq*h*h)/(ateta*r0_eq*r0_eq*r0_eq)));

				//Foundation input motion for equivalent SDOF
				if (e_eq!=0){ //Embedded Foundation && dof=Num+3

					// d2uwff=fft(d2uff)/n;        // Free Field Acceleration in Freq. Domain
					df_eq=1.0/(n*dt);         // Frequency Interval for Fourier Transform
					gsl_vector_complex *d2uwff=gsl_vector_complex_alloc(n);		
					gsl_complex tempc;
					double *data=new double[2*n];
					gsl_fft_complex_workspace *work = gsl_fft_complex_workspace_alloc (n);
					gsl_fft_complex_wavetable *wave = gsl_fft_complex_wavetable_alloc (n);
					for (int i = 0; i < n; i++){ 
						data[2*i] = d2uff[i];
						data[2*i+1] = 0.0;
					}
					gsl_fft_complex_forward(data,1,n,wave,work);
					for(int i=0; i<n ; i++){
						tempc = gsl_complex_rect(data[2*i],data[2*i+1]);
						tempc = gsl_complex_div_real(tempc,n);
						gsl_vector_complex_set(d2uwff,i,tempc);
					}

					double a0max = 4;
					int ndisk = qFloor(5/pi*e_r0*a0max+1)+1;       // Number of Required Disks,Thickness of each layer is less than 0.1 wave length
					double de=e_eq/(ndisk-1);

					gsl_vector_complex *ee = gsl_vector_complex_alloc(ndisk);
					for (int i=0 ; i<ndisk ; i++){
						tempc = gsl_complex_rect(de*(ndisk-1-i),0.0);
						gsl_vector_complex_set(ee,i,tempc);       // Depth of Each Layer
					}

					gsl_vector_complex *one = gsl_vector_complex_alloc(ndisk);
					tempc = gsl_complex_rect(1.0,0.0);
					gsl_vector_complex_set_all(one,tempc);

					gsl_matrix_complex *A = gsl_matrix_complex_alloc(ndisk,2);       // Transformation Matrix
					gsl_status = gsl_matrix_complex_set_col(A,0,one);	//A(:,1)=one;
					if (gsl_status) {  
						rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
						return -1;
					}
					gsl_status = gsl_matrix_complex_set_col (A, 1, ee);		//A(:,2)=ee;
					if (gsl_status) {  
						rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
						return -1;
					}

					gsl_vector_complex *ufHarm = gsl_vector_complex_alloc(ndisk);          // Harmonic Motion Vector at Each Depth              
					gsl_vector_complex *d2ugw = gsl_vector_complex_alloc(n);               // Horizontal FIM in Freq. Domain
					gsl_vector_complex *d2phigw = gsl_vector_complex_alloc(n);             // Rocking FIM in Freq. Domain
					gsl_vector_complex *d2u0gw = gsl_vector_complex_alloc(2);
					gsl_matrix_complex *Gh = gsl_matrix_complex_alloc(ndisk,ndisk);      // Horizontal Green's Function
					gsl_matrix_complex *Gr = gsl_matrix_complex_calloc(ndisk,ndisk);      // Rocking Green's Function
					gsl_matrix_complex *S00g = gsl_matrix_complex_calloc(2,2);
					gsl_matrix_complex *Sffr = gsl_matrix_complex_alloc(ndisk,ndisk);      // Rocking Inverse Green's Function	
					gsl_matrix_complex *Sffh = gsl_matrix_complex_alloc(ndisk,ndisk);      // Horizontal Inverse Green's Function	

					for (int i=0 ; i <= n/2-1 ; i++){
						double w=2*pi*df_eq*i;
						double b0h=w*z0h_eq/vs;
						double b0r=w*z0r_eq/vp_eq;

						for (int j=0; j < ndisk ; j++){
							for (int k=0; k < ndisk ; k++){
								double dist1 = qAbs(j-k)*de;
								double dist2 = (j+k)*de;
								gsl_complex tempc2, tempc3;

								//Gh(j,k)=1/(2*Kh_eq)*1/(1+sqrt(-1)*b0h)*(exp(-sqrt(-1)*w*dist1/vs)/(1+dist1/z0h_eq)+exp(-sqrt(-1)*w*dist2/vs)/(1+dist2/z0h_eq));
								tempc = gsl_complex_rect(0.0,-1*w*dist2/vs);
								tempc = gsl_complex_exp(tempc);
								tempc = gsl_complex_div_real(tempc,(1+dist2/z0h_eq));
								tempc2 = gsl_complex_rect(0.0,-1*w*dist1/vs);
								tempc2 = gsl_complex_exp(tempc2);
								tempc2 = gsl_complex_div_real(tempc2,(1+dist1/z0h_eq));
								tempc = gsl_complex_add(tempc,tempc2);
								tempc2 = gsl_complex_rect(1.0,b0h);
								tempc = gsl_complex_div(tempc,tempc2);
								tempc = gsl_complex_div_real(tempc,2*Kh_eq);
								gsl_matrix_complex_set(Gh,j,k,tempc);	

								//Gr(j,k)=1/(2*Kr_eq)*1/(1+sqrt(-1)*b0r-b0r^2/3)*(exp(-sqrt(-1)*w*dist1/vp_eq)*(1/(1+dist1/z0r_eq)^3+sqrt(-1)*b0r*1/(1+dist1/z0r_eq)^2)+exp(-sqrt(-1)*w*dist2/vp_eq)*(1/(1+dist2/z0r_eq)^3+sqrt(-1)*b0r*1/(1+dist2/z0r_eq)^2));
								tempc = gsl_complex_rect(1.0/gsl_pow_3(1+dist2/z0r_eq),b0r/gsl_pow_2(1.0+dist2/z0r_eq));
								tempc2 = gsl_complex_rect(0.0,-1*w*dist2/vp_eq);
								tempc2 = gsl_complex_exp(tempc2);
								tempc = gsl_complex_mul(tempc,tempc2);
								tempc2 = gsl_complex_rect(1.0/gsl_pow_3(1+dist1/z0r_eq),b0r/gsl_pow_2(1.0+dist1/z0r_eq));
								tempc3 = gsl_complex_rect(0.0,-1*w*dist1/vp_eq);
								tempc3 = gsl_complex_exp(tempc3);
								tempc2 = gsl_complex_mul(tempc2,tempc3);
								tempc = gsl_complex_add(tempc,tempc2);
								tempc2 = gsl_complex_rect(1.0-b0r*b0r/3,b0r);
								tempc = gsl_complex_div(tempc,tempc2);
								tempc = gsl_complex_div_real(tempc,2*Kr_eq);	
								gsl_matrix_complex_set(Gr,j,k,tempc);
							} // end for k
							gsl_vector_complex_set(ufHarm,j, gsl_complex_rect(qCos(w*j*de/vs),0.0));
						} // end for j

						//Sffh=inv(Gh);
						int s;
						gsl_permutation *p = gsl_permutation_calloc(ndisk);
						gsl_status = gsl_linalg_complex_LU_decomp(Gh,p,&s);
						if (gsl_status) {  
							rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
							return -1;
						}
						gsl_status = gsl_linalg_complex_LU_invert (Gh, p, Sffh);				
						if (gsl_status) {  
							rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
							return -1;
						}

						//Sffr=inv(Gr);
						gsl_status = gsl_linalg_complex_LU_decomp(Gr,p,&s);
						if (gsl_status) {  
							rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
							return -1;
						}
						gsl_status = gsl_linalg_complex_LU_invert (Gr, p, Sffr);				
						if (gsl_status) {  
							rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
							return -1;
						}
						gsl_permutation_free(p);

						double mdisk = roh_eq*pi*r0_eq*r0_eq*e_eq;
						gsl_complex tempc2;
						gsl_complex one_c = gsl_complex_rect(1.0,0.0);
						gsl_complex zero_c = gsl_complex_rect(0.0,0.0);
						gsl_vector_complex *tempvc = gsl_vector_complex_alloc(ndisk);

						//S00g(1,1) = one'*Sffh*one + w^2*mdisk;
						gsl_status = gsl_blas_zgemv(CblasNoTrans,one_c,Sffh,one,zero_c,tempvc);
						if (gsl_status) {  
							rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
							return -1;
						}
						gsl_status = gsl_blas_zdotu(one,tempvc,&tempc);
						if (gsl_status) {  
							rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
							return -1;
						}
						tempc = gsl_complex_add_real(tempc,w*w*mdisk);
						gsl_matrix_complex_set(S00g,0,0,tempc);

						// S00g(2,2) = ee'*Sffh*ee + one'*Sffr*one + w^2*mdisk*e^2/3+w^2*mdisk*r0^2/4;
						gsl_status = gsl_blas_zgemv(CblasNoTrans,one_c,Sffh,ee,zero_c,tempvc);
						if (gsl_status) {  
							rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
							return -1;
						}	
						gsl_status = gsl_blas_zdotu(ee,tempvc,&tempc);
						if (gsl_status) {  
							rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
							return -1;
						}
						gsl_status = gsl_blas_zgemv(CblasNoTrans,one_c,Sffr,one,zero_c,tempvc);
						if (gsl_status) {  
							rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
							return -1;
						}
						gsl_status = gsl_blas_zdotu(one,tempvc,&tempc2);
						if (gsl_status) {  
							rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
							return -1;
						}
						tempc = gsl_complex_add(tempc, tempc2);
						tempc = gsl_complex_add_real(tempc, w*w*mdisk*e_eq*e_eq/3+w*w*mdisk*r0_eq*r0_eq/4);
						gsl_matrix_complex_set(S00g,1,1,tempc);

						// S00g(1,2) = ee'*Sffh*one + w^2*mdisk*e;	S00g(2,1)=S00g(1,2);
						gsl_status = gsl_blas_zgemv(CblasNoTrans,one_c,Sffh,one,zero_c,tempvc);
						if (gsl_status) {  
							rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
							return -1;
						}
						gsl_status = gsl_blas_zdotu(ee,tempvc,&tempc);
						if (gsl_status) {  
							rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
							return -1;
						}
						tempc = gsl_complex_add_real(tempc,w*w*mdisk*e_eq);
						gsl_matrix_complex_set(S00g,0,1,tempc);
						gsl_matrix_complex_set(S00g,1,0,tempc);

						//d2u0gw=inv(S00g)*A'*Sffh*ufHarm;
						p = gsl_permutation_calloc(2);
						gsl_matrix_complex *invS00g = gsl_matrix_complex_alloc(2,2);
						gsl_status = gsl_linalg_complex_LU_decomp(S00g,p,&s);
						if (gsl_status) {  
							rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
							return -1;
						}
						gsl_status = gsl_linalg_complex_LU_invert (S00g, p, invS00g);
						if (gsl_status) {  
							rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
							return -1;
						}
						gsl_permutation_free(p);

						gsl_matrix_complex *tempmc = gsl_matrix_complex_calloc(2,ndisk);				
						gsl_status = gsl_blas_zgemm(CblasNoTrans,CblasTrans,one_c,invS00g,A,zero_c,tempmc);
						if (gsl_status) {  
							rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
							return -1;
						}
						gsl_matrix_complex *tempmc2 = gsl_matrix_complex_calloc(2,ndisk);				
						gsl_status = gsl_blas_zgemm(CblasNoTrans,CblasNoTrans,one_c,tempmc,Sffh,zero_c,tempmc2);
						if (gsl_status) {  
							rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
							return -1;
						}
						gsl_status = gsl_blas_zgemv(CblasNoTrans,one_c,tempmc2,ufHarm,zero_c,d2u0gw);
						if (gsl_status) {  
							rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
							return -1;
						}

						//d2ugw(i+1)=d2u0gw(1)*d2uwff(i+1);
						tempc=gsl_vector_complex_get(d2u0gw,0);
						tempc2=gsl_vector_complex_get(d2uwff,i);
						tempc = gsl_complex_mul(tempc,tempc2);
						gsl_vector_complex_set(d2ugw,i,tempc);

						//d2phigw(i+1)=d2u0gw(2)*d2uwff(i+1);
						tempc=gsl_vector_complex_get(d2u0gw,1);
						tempc2=gsl_vector_complex_get(d2uwff,i);
						tempc = gsl_complex_mul(tempc,tempc2);
						gsl_vector_complex_set(d2phigw,i,tempc);

						gsl_matrix_complex_free(invS00g);
						gsl_matrix_complex_free(tempmc);
						gsl_matrix_complex_free(tempmc2);
						gsl_vector_complex_free(tempvc);
					} // end for i

					gsl_matrix_complex_free(A);
					gsl_vector_complex_free(ee);
					gsl_vector_complex_free(one);
					gsl_matrix_complex_free(Gh);
					gsl_matrix_complex_free(Gr);
					gsl_matrix_complex_free(Sffh);
					gsl_matrix_complex_free(Sffr);
					gsl_matrix_complex_free(S00g);

					//	FIMh_eq=ifft(d2ugw,'symmetric')*n;
					data[0]=GSL_REAL(gsl_vector_complex_get(d2ugw,0));
					data[1]=0.0;
					for (int i = 1; i <= n/2-1; i++){ 
						tempc = gsl_vector_complex_get(d2ugw,i);
						data[2*i] = GSL_REAL(tempc);
						data[2*(n-i)] = data[2*i];
						data[2*i+1] = GSL_IMAG(tempc);
						data[2*(n-i)+1] = (-1)*data[2*i+1];
					}
					data[n]=data[n-2];
					data[n+1]=0.0;
					gsl_fft_complex_inverse(data,1,n,wave,work);
					for(int i=0; i<n ; i++){
						FIMh_eq[i] = data[2*i]*n;
					}

					//	FIMr_eq=ifft(d2phigw,'symmetric')*n;
					data[0]=GSL_REAL(gsl_vector_complex_get(d2phigw,0));
					data[1]=0.0;
					for (int i = 1; i <= n/2-1; i++){ 
						tempc = gsl_vector_complex_get(d2phigw,i);
						data[2*i] = GSL_REAL(tempc);
						data[2*(n-i)] = data[2*i];
						data[2*i+1] = GSL_IMAG(tempc);
						data[2*(n-i)+1] = (-1)*data[2*i+1];
					}
					data[n]=data[n-2];
					data[n+1]=0.0;
					gsl_fft_complex_inverse(data,1,n,wave,work);
					for(int i=0; i<n ; i++){
						FIMr_eq[i] = data[2*i]*n;
					}

					gsl_fft_complex_workspace_free(work);
					gsl_fft_complex_wavetable_free(wave);
					delete[] data;

					gsl_vector_complex_free(d2u0gw);
					gsl_vector_complex_free(d2ugw);
					gsl_vector_complex_free(d2phigw);
					gsl_vector_complex_free(ufHarm);
					gsl_vector_complex_free(d2uwff);
				}
				else {//if (e==0) Surface Foundtion && dof=Num+3
					FIMh_eq=d2uff;
					FIMr_eq.fill(0.0);
				}
				if (e_eq==0){	// Surface Foundation
					double gama0h=0.78-0.4*v;
					double gama1r=0.42-0.3*v*v;
					double mu0r;
					if (v<1/3){
						mu0r=0;
					}
					else{ //if (v>=1/3)
						mu0r=0.16*(v-1/3);
					}
					double mu1r=0.34-0.2*v*v;
					k0h_eq=Kh_eq;
					k0r_eq=Kr_eq;
					c0h_eq=r0_eq/vs*gama0h*k0h_eq;
					c0r_eq=0;
					c1r_eq=r0_eq/vs*gama1r*k0r_eq;
					m0r_eq=gsl_pow_2(r0_eq/vs)*mu0r*k0r_eq;
					I1r_eq=gsl_pow_2(r0_eq/vs)*mu1r*k0r_eq;
					fk_eq=0;
					fc_eq=0;
				}
				else { //if (e_eq!=0)	// Embedded Foundation
					double gama0h=0.68+0.57*qSqrt(e_r0);
					double gama0r=0.15631*e_r0-0.08906*gsl_pow_2(e_r0)-0.00874*gsl_pow_3(e_r0);
					double gama1r=0.4+0.03*gsl_pow_2(e_r0);
					double mu1r=0.33+0.1*gsl_pow_2(e_r0);
					fk_eq=e_eq/4;
					fc_eq=0.32*e_eq+0.03*e_eq*gsl_pow_2(e_r0);
					k0h_eq=Kh_eq*(1+e_r0);
					double kr_embedded=Kr_eq*(1+2.3*e_r0+0.58*gsl_pow_3(e_r0));
					k0r_eq=kr_embedded-G_eq*gsl_pow_3(r0_eq)/(2*(2.0-v))*(1+e_r0)*gsl_pow_2(e_r0);
					c0h_eq=r0_eq/vs*gama0h*k0h_eq;
					c0r_eq=r0_eq/vs*gama0r*kr_embedded;
					c1r_eq=r0_eq/vs*gama1r*kr_embedded;       // Damping of Internal Degree of Freedom
					m0r_eq=0;
					I1r_eq=gsl_pow_2(r0_eq/vs)*mu1r*kr_embedded;     // Mass of Inertia Internal Degree of Freedom
				}

				w0_eq=vs/r0_eq;
				double xi_0_modified=xi_0*(THat_eq/Tn)*h_r0/a0fix;
				xi0_w0_eq=xi_0_modified/w0_eq;
		
				//MDOF
				for (int j=1;j<=1;j++){
					for (int i=1;i<=1;i++){
						gsl_matrix_set(M_eq,i-1,j-1,gsl_matrix_get(Mfix_eq,i-1,j-1));	//Mss
						gsl_matrix_set(K_eq,i-1,j-1,gsl_matrix_get(Kfix_eq,i-1,j-1));	//Kss
						gsl_matrix_set(C_eq,i-1,j-1,gsl_matrix_get(Cfix_eq,i-1,j-1));	//Css
					}
				}

				//Mso,Mos
				for (int i=1;i<=1;i++){
					gsl_matrix_set(M_eq,i-1,1,gsl_matrix_get(Mfix_eq,i-1,i-1));
					gsl_matrix_set(M_eq,i-1,2,gsl_matrix_get(Mfix_eq,i-1,i-1)*(e_eq+h_story_eq*(1-i+1)));//[mn*(Hn+e)]
					gsl_matrix_set(M_eq,1,i-1,gsl_matrix_get(Mfix_eq,i-1,i-1));
					gsl_matrix_set(M_eq,2,i-1,gsl_matrix_get(Mfix_eq,i-1,i-1)*(e_eq+h_story_eq*(1-i+1)));
				}
				//Moo
				double sigmaMi=0;
				double sigmaMiHi=0;
				double sigmaMiHi2=0;
				for (int i=1;i<=1;i++){
					sigmaMi=sigmaMi+gsl_matrix_get(Mfix_eq,i-1,i-1);
					sigmaMiHi=sigmaMiHi+gsl_matrix_get(Mfix_eq,i-1,i-1)*(h_story_eq*(1-i+1));
					sigmaMiHi2=sigmaMiHi2+gsl_matrix_get(Mfix_eq,i-1,i-1)*(h_story_eq*(1-i+1)+e_eq)*(h_story_eq*(1-i+1)+e_eq);
				}
				gsl_matrix_set(M_eq,1,1,sigmaMi+mf_eq+c0h_eq*xi0_w0_eq);
				gsl_matrix_set(M_eq,1,1+1,sigmaMiHi+sigmaMi*e_eq+mf_eq*e_eq*fcr+c0h_eq*xi0_w0_eq*fc_eq);
				gsl_matrix_set(M_eq,1+1,1,sigmaMiHi+sigmaMi*e_eq+mf_eq*e_eq*fcr+c0h_eq*xi0_w0_eq*fc_eq);
				gsl_matrix_set(M_eq,1+1,1+1,sigmaMiHi2+mf_eq*e_eq*e_eq/4+Is_eq+If_eq+c0h_eq*xi0_w0_eq*fc_eq*fc_eq+(c0r_eq+c1r_eq)*xi0_w0_eq+m0r_eq);
				gsl_matrix_set(M_eq,1+1,1+2,-1*c1r_eq*xi0_w0_eq);
				gsl_matrix_set(M_eq,1+2,1+1,-1*c1r_eq*xi0_w0_eq);
				gsl_matrix_set(M_eq,1+2,1+2,c1r_eq*xi0_w0_eq+I1r_eq);
				//Coo
				gsl_matrix_set(C_eq,1,1,c0h_eq+2*k0h_eq*xi0_w0_eq);
				gsl_matrix_set(C_eq,1,1+1,c0h_eq*fc_eq+2*k0h_eq*xi0_w0_eq*fk_eq);
			
				gsl_matrix_set(C_eq,1+1,1,c0h_eq*fc_eq+2*k0h_eq*xi0_w0_eq*fk_eq);
				gsl_matrix_set(C_eq,1+1,1+1,c0h_eq*fc_eq*fc_eq+2*k0h_eq*xi0_w0_eq*fk_eq*fk_eq+c0r_eq+2*k0r_eq*xi0_w0_eq+c1r_eq);
				gsl_matrix_set(C_eq,1+1,1+2,-1*c1r_eq);

				gsl_matrix_set(C_eq,1+2,1+1,-1*c1r_eq);
				gsl_matrix_set(C_eq,1+2,1+2,c1r_eq);

				//Koo
				gsl_matrix_set(K_eq,1,1,k0h_eq);
				gsl_matrix_set(K_eq,1,1+1,k0h_eq*fk_eq);

				gsl_matrix_set(K_eq,1+1,1,k0h_eq*fk_eq);
				gsl_matrix_set(K_eq,1+1,1+1,k0h_eq*fk_eq*fk_eq+k0r_eq);
			}
		}	//end of equivalent SDOF
		
	}	//end for fixed or flexible
	/*RDomain *theDomain = qobject_cast<RDomain *>(this->parent());
	rDebug(threadID) << "K = " << endl;
	theDomain->print(K);
	rDebug(threadID) << "M = " << endl;
	theDomain->print(M);
	rDebug(threadID) << "C = " << endl;
	theDomain->print(C);
	rDebug(threadID) << "roh = " << roh << endl;
	rDebug(threadID) << "Vs = " << vs << endl;*/
	// ------------------------------------------- Linear Analysis --------------------------------------------------
	int la_status=0;		// Newmark function status
	//---------------------- MDOF Parameters
	gsl_vector *Defix_MDOF = gsl_vector_calloc(Num);			// Maximum Elastic Displacement (Fixed Base)
	gsl_vector *De_tot_fix_MDOF = gsl_vector_calloc(Num);
	gsl_vector *Dessi_MDOF = gsl_vector_calloc(Num);		// Maximum Elastic Displacement (SSI)
	gsl_vector *De_tot_MDOF = gsl_vector_calloc(Num);	// Total Maximum Elastic Displacement
	gsl_vector *Aefix_MDOF = gsl_vector_calloc(Num);		// Maximum Elastic Accelaration (Fixed Base)
	gsl_vector *Ae_tot_fix_MDOF = gsl_vector_calloc(Num);// Total Maximum Elastic Accelaration (Fixed Base)
	gsl_vector *Aessi_MDOF = gsl_vector_calloc(Num);		// Maximum Elastic Accelaration (SSI)
	gsl_vector *Ae_tot_MDOF = gsl_vector_calloc(Num);	// Total Maximum Elastic Accelaration
	double VeFix=0;
	double VeSSI=0;

	//---------------------- Equivalent SDOF Parameters
	gsl_vector *Defix_eq = gsl_vector_calloc(1);			// Maximum Elastic Displacement (Fixed Base)
	gsl_vector *De_tot_fix_eq = gsl_vector_calloc(1);
	gsl_vector *Dessi_eq = gsl_vector_calloc(1);		// Maximum Elastic Displacement (SSI)
	gsl_vector *De_tot_eq = gsl_vector_calloc(1);	// Total Maximum Elastic Displacement
	gsl_vector *Aefix_eq = gsl_vector_calloc(1);		// Maximum Elastic Accelaration (Fixed Base)
	gsl_vector *Ae_tot_fix_eq = gsl_vector_calloc(1);// Total Maximum Elastic Accelaration (Fixed Base)
	gsl_vector *Aessi_eq = gsl_vector_calloc(1);		// Maximum Elastic Accelaration (SSI)
	gsl_vector *Ae_tot_eq = gsl_vector_calloc(1);	// Total Maximum Elastic Accelaration
	double VeFix_eq=0;
	double VeSSI_eq=0;

	//---------------------- SDOF Parameters
	double Defix = 0;			// Maximum Elastic Displacement (Fixed Base)
	double De_tot_fix=0;
	double Dessi = 0;		// Maximum Elastic Displacement (SSI)
	double De_tot = 0;	// Total Maximum Elastic Displacement
	double Aefix = 0;			// Maximum Elastic Accelaration (Fixed Base)
	double Ae_tot_fix=0;	// Total Maximum Elastic Accelaration (Fixed Base)
	double Aessi = 0;		// Maximum Elastic Accelaration (SSI)
	double Ae_tot = 0;	// Total Maximum Elastic Accelaration

	if (theMDOFOrSDOF == RSoilStructureInteractionModel::SDOF){	//--------------------------SDOF
		
		// ------------------------------- Linear Analysis of FixedBase Structure
		la_status = linearAnalysisSDOF(0.0, Kfix, Cfix, Mfix, Lfix, FIMhfix, FIMrfix, n, dt, 1.0, &Defix, &De_tot_fix, &Aefix, &Ae_tot_fix,threadID);
		if (la_status) {  
			rCritical(threadID) << "Error in linearAnalysis function";
			return -1;
		}
		if (dof==4){
			// --------------------------------- Linear Analysis of Soil-Structure System
			la_status = linearAnalysisSDOF(he, K, C, M, L, FIMh, FIMr, n, dt, dof , &Dessi, &De_tot, &Aessi, &Ae_tot,threadID);
			if (la_status) {  
				rCritical(threadID) << "Error in linearAnalysis function";
				return -1;
			}
		}
		else
		{ // if dof==1
			Dessi=Defix;
			De_tot=Defix;
			Aessi=Aefix;
			Ae_tot=Ae_tot_fix;
		}
	}
	else{	//-------------------------------------------------------MDOF

		// ------------------------------- Linear Analysis of FixedBase Structure
		la_status = linearAnalysisMDOF(0.0,0.0,kstr,Kfix, Cfix, Mfix, Lfix, FIMhfix, FIMrfix, n, dt, Num, Num , Defix_MDOF, De_tot_fix_MDOF, Aefix_MDOF, Ae_tot_fix_MDOF,&VeFix,threadID);
		if (la_status) {  
			rCritical(threadID) << "Error in linearAnalysis function";
			return -1;
		}
		//Equivalent SDOF
		la_status = linearAnalysisMDOF(0.0,0.0,kstr_eq,Kfix_eq, Cfix_eq, Mfix_eq, Lfix_eq, FIMhfix, FIMrfix, n, dt, 1, 1 , Defix_eq, De_tot_fix_eq, Aefix_eq, Ae_tot_fix_eq,&VeFix_eq,threadID);
		if (la_status) {  
			rCritical(threadID) << "Error in linearAnalysis function";
			return -1;
		}
		if (dof==Num+3){
		// --------------------------------- Linear Analysis of Soil-Structure System
			la_status = linearAnalysisMDOF(h_story,e,kstr,K, C, M, L, FIMh, FIMr, n, dt, dof, Num , Dessi_MDOF, De_tot_MDOF, Aessi_MDOF, Ae_tot_MDOF,&VeSSI,threadID);
			if (la_status) {  
				rCritical(threadID) << "Error in linearAnalysis function";
				return -1;
			}
		}else{ // if dof==Num
			gsl_vector_memcpy(Dessi_MDOF,Defix_MDOF);
			gsl_vector_memcpy(De_tot_MDOF,Defix_MDOF);
			gsl_vector_memcpy(Aessi_MDOF,Aefix_MDOF);
			gsl_vector_memcpy(Ae_tot_MDOF,Ae_tot_fix_MDOF);
			VeSSI=VeFix;
		}
		//Equivalent SDOF
		if ( (theRunMode == GivenDucComputeModificationFactor) || (theRunMode == GivenDucSameRorFy) /*|| (theRunMode == GivenDucSameAlpha)*/){
			if (dofeq==4){
			// --------------------------------- Linear Analysis of Soil-Structure System
				la_status = linearAnalysisMDOF(h_story_eq,e_eq,kstr_eq,K_eq, C_eq, M_eq, L_eq, FIMh_eq, FIMr_eq, n, dt, dofeq, 1, Dessi_eq, De_tot_eq, Aessi_eq, Ae_tot_eq,&VeSSI_eq,threadID);
				if (la_status) {  
					rCritical(threadID) << "Error in linearAnalysis function";
					return -1;
				}
			}else{ // if dofeq==1
				gsl_vector_memcpy(Dessi_eq,Defix_eq);
				gsl_vector_memcpy(De_tot_eq,Defix_eq);
				gsl_vector_memcpy(Aessi_eq,Aefix_eq);
				gsl_vector_memcpy(Ae_tot_eq,Ae_tot_fix_eq);
				VeSSI_eq=VeFix_eq;
			}
		}
		/*RDomain *theDomain = qobject_cast<RDomain *>(this->parent());
		rDebug(threadID) << "De = " <<endl;
		theDomain->print(Dessi_MDOF);
		rDebug(threadID) << "Defix = " << endl;
		theDomain->print(Defix_MDOF);*/

		
	}
	// --------------------------------- Linear Analysis End -------------------------------------------------------------

	// ------------------------------ Nonlinear Analysis
	// ----------- MDOF Parameters
	gsl_vector *Dm_MDOF = gsl_vector_calloc(Num);           // Maximum Inelastic Displacement
	gsl_vector *Dm_tot_MDOF = gsl_vector_calloc(Num);   // Maximum displacement of structure with respect to the base of foundation
	gsl_vector *Dm_fix_MDOF = gsl_vector_calloc(Num);
	gsl_vector *Dm_tot_fix_MDOF = gsl_vector_calloc(Num);
	gsl_vector *Am_MDOF = gsl_vector_calloc(Num);           // Maximum Inelastic Accelaration
	gsl_vector *Am_tot_MDOF = gsl_vector_calloc(Num);  // Maximum Accelaration of structure with respect to the base of foundation
	gsl_vector *Am_fix_MDOF = gsl_vector_calloc(Num);
	gsl_vector *Am_tot_fix_MDOF = gsl_vector_calloc(Num);
	gsl_vector *ductility_MDOF = gsl_vector_calloc(Num);
	// MDOF parameters for Runmode 4: MDOF flexible base with fy of SDOF fixed base
	gsl_vector *Dm_MDOF2 = gsl_vector_calloc(Num);           // Maximum Inelastic Displacement
	gsl_vector *Dm_tot_MDOF2 = gsl_vector_calloc(Num);   // Maximum displacement of structure with respect to the base of foundation
	gsl_vector *Am_MDOF2 = gsl_vector_calloc(Num);           // Maximum Inelastic Accelaration
	gsl_vector *Am_tot_MDOF2 = gsl_vector_calloc(Num);  // Maximum Accelaration of structure with respect to the base of foundation
	gsl_vector *ductility_MDOF2 = gsl_vector_calloc(Num);

	gsl_vector *De_Intensity_MDOF = gsl_vector_calloc(Num);
	gsl_vector *Ae_Intensity_MDOF = gsl_vector_calloc(Num);
	gsl_vector *De_tot_Intensity_MDOF = gsl_vector_calloc(Num);
	gsl_vector *Ae_tot_Intensity_MDOF = gsl_vector_calloc(Num);
	gsl_vector *Dm_Intensity_MDOF = gsl_vector_calloc(Num);
	gsl_vector *Am_Intensity_MDOF = gsl_vector_calloc(Num);
	gsl_vector *Dm_tot_Intensity_MDOF = gsl_vector_calloc(Num);
	gsl_vector *Am_tot_Intensity_MDOF = gsl_vector_calloc(Num);
	gsl_vector *Fy_mIntensity_MDOF = gsl_vector_calloc(Num);
	gsl_vector *Fo_mIntensity_MDOF = gsl_vector_calloc(Num);
	gsl_vector *FixedDuc_MDOF = gsl_vector_calloc(Num);
	double ductility_max=0.0;
	int ductility_maxNumber=0;
	double ductility_max2=0.0;
	int ductility_maxNumber2=0;
	double Fixductility_max=0.0;
	int Fixductility_maxNumber=0;
	double RfixMDOF=1.0;
	double RssiMDOF=1.0;
	double Alpha_RFixed=1.0;
	double Alpha_VFixed=1.0;
	double Vfixratio = 1.0;
	double Vssiratio = 1.0;
	
	// ----------- Equivalent SDOF Parameters
	gsl_vector *Dm_eq = gsl_vector_calloc(1);           // Maximum Inelastic Displacement
	gsl_vector *Dm_tot_eq = gsl_vector_calloc(1);   // Maximum displacement of structure with respect to the base of foundation
	gsl_vector *Dm_fix_eq = gsl_vector_calloc(1);
	gsl_vector *Dm_tot_fix_eq = gsl_vector_calloc(1);
	gsl_vector *Am_eq = gsl_vector_calloc(1);           // Maximum Inelastic Accelaration
	gsl_vector *Am_tot_eq = gsl_vector_calloc(1);  // Maximum Accelaration of structure with respect to the base of foundation
	gsl_vector *Am_fix_eq = gsl_vector_calloc(1);
	gsl_vector *Am_tot_fix_eq = gsl_vector_calloc(1);
	gsl_vector *ductility_eq = gsl_vector_calloc(1);
	gsl_vector *FixedDuc_eq = gsl_vector_calloc(1);
	gsl_vector *Dm_eq2 = gsl_vector_calloc(1);           // Maximum Inelastic Displacement
	gsl_vector *Dm_tot_eq2 = gsl_vector_calloc(1);   // Maximum displacement of structure with respect to the base of foundation
	gsl_vector *Am_eq2 = gsl_vector_calloc(1);           // Maximum Inelastic Accelaration
	gsl_vector *Am_tot_eq2 = gsl_vector_calloc(1);  // Maximum Accelaration of structure with respect to the base of foundation
	gsl_vector *ductility_eq2 = gsl_vector_calloc(1);
	double ductility_max_eq=0.0;
	int ductility_maxNumber_eq=0;
	double ductility_max_eq2=0.0;
	int ductility_maxNumber_eq2=0;
	double Fixductility_max_eq=0.0;
	int Fixductility_maxNumber_eq=0;
	double RfixSDOF=1.0;
	double RssiSDOF=1.0;
	double RssiSDOF2=1.0;
	double Alpha_RSSI=1.0;
	double Alpha_VSSI=1.0;

	// ----------- SDOF Parameters
	double Dm = 0.0;           // Maximum Inelastic Displacement
	double Dm_tot = 0.0;   // Maximum displacement of structure with respect to the base of foundation
	double Dm_fix=0.0;
	double Dm_tot_fix = 0.0;
	double Am = 0.0;           // Maximum Inelastic Accelaration
	double Am_tot = 0.0;   // Maximum Accelaration of structure with respect to the base of foundation
	double Am_fix = 0.0;
	double Am_tot_fix = 0.0;
	double ductility = 0.0;
	double De_Intensity;
	double Ae_Intensity;
	double De_tot_Intensity;
	double Ae_tot_Intensity;
	double Dm_Intensity;
	double Am_Intensity;
	double Dm_tot_Intensity;
	double Am_tot_Intensity;
	double SystemDuc;
	double Fy_mIntensity;
	double Fo_mIntensity;
	double VhatRatio;
	double FixedDuc = 0.0;
	double Duc_B;
	double Dm_B;
	double Am_tot_B;
	double T_SSI;
	double Rssi=1.0;
	double Rfix=1.0;

	if (theMDOFOrSDOF == RSoilStructureInteractionModel::SDOF){	//SDOF
		int nla_status;		// nonlinearAnalysis function status
		double alpha = 0;		 // k2(after yielding)/k1(before yielding)
		double fbar_y;
		double DeUsed;
		double fy;
		double fy_fix;

		//double dof1 = 1;
		//Runmodes: 1)GivenRSameR, 2)GivenDucSameR, 3)GivenRFixSameFy, 4)GivenDucFixSameFy, 5)GivenDucSSISameFy, 6)GivenRReducedFyATC 7)GivenRReducedFyASC_E05  8)GivenRReducedFyNEHRP

		//-------------------------------------
		if (theRunMode == RSoilStructureInteractionModel::GivenRSameR){

			// 1) Given Rfix -> Rfix=Rssi -> Ducssi
			DeUsed=Dessi; //Same R
			Rfix = R;
			Rssi = R;
			fbar_y = 1/R;
			fy_fix = ks* Defix* fbar_y;
			fy = ks* DeUsed* fbar_y;
			if (R==1){
				ductility=1;
				Dm_tot=De_tot;
				Dm=Dessi;
				Am_tot=Ae_tot;
				Am=Aessi;
			} 
			else{

				// Nonlinear Analysis of FixedBase Structure: FixedDuc
				nla_status = nonlinearAnalysisSDOF(he, fy_fix, alpha, ks, Kfix, Cfix, Mfix, Lfix, FIMhfix, FIMrfix, n, dt, 1 , &FixedDuc, &Dm_tot_fix, &Am_fix, &Am_tot_fix,threadID);
				if (nla_status) {  
					rCritical(threadID) << "Error in NonlinearAnalysis function";
					return -1;
				}

				// Nonlinear Analysis of Flexible Base Structure: ductility
				if (dof==4){
					nla_status = nonlinearAnalysisSDOF(he,fy,alpha,ks,K,C,M,L, FIMh,FIMr,n,dt,dof , &ductility, &Dm_tot, &Am, &Am_tot,threadID);
					if (nla_status) {  
						rCritical(threadID) << "Error in NonlinearAnalysis function";
						return -1;
					}
				}
				else
				{
					ductility=FixedDuc;
					Dm_tot=Dm_tot_fix;
					Am_tot=Am_tot_fix;
					Am=Am_fix;
				}

				Dm = ductility*DeUsed*fbar_y;
			}

			if (ductility < 1){
				//ductility=1;
			}

		} //end if (theRunMode == 1)

		//--------------------------------------

		if (theRunMode == RSoilStructureInteractionModel::GivenDucSameR){ 
			// 2) Given target_ducssi -> Rssi = Rfix
			DeUsed=Dessi; //Same R

			//NonLinear Analysis of Soil Structure System : Fyssi
			fbar_y=1;
			ductility=1;
			double multiplier=0.85;
			while (ductility<=target_ductility){
				fbar_y=fbar_y*multiplier;
				fy = ks* DeUsed* fbar_y;
				nla_status = nonlinearAnalysisSDOF(he,fy,alpha,ks,K,C,M,L,FIMh,FIMr,n,dt,dof, &ductility, &Dm_tot, &Am, &Am_tot,threadID);

				if (nla_status) {  
					rCritical(threadID) << "Error in NonlinearAnalysis function";
					return -1;
				}
			}

			//first=min(fbar_y/(0.85^5),1); 
			double first=1;
			if ( fbar_y/gsl_pow_5(0.85) < 1)
				first = fbar_y/gsl_pow_5(0.85);
			double last=fbar_y;
			int start;
			QVector<double> fbar;
			for (int i=0; i<=10 ; i++){
				fbar.insert(i, first+i*(last-first)/10);
			}
			QVector<double> mubar(fbar.count());
			mubar.fill(0.0);
			if (first==1){
				mubar[0]=1;
				start=1;
			}
			else{
				start=0;
			}
			for (int i=start; i <= fbar.count()-1 ; i++){
				fy = ks* DeUsed* fbar[i];
				nla_status = nonlinearAnalysisSDOF(he,fy,alpha,ks,K,C,M,L,FIMh,FIMr,n,dt,dof, &mubar[i], &Dm_tot, &Am, &Am_tot,threadID);
				if (nla_status) {  
					rCritical(threadID) << "Error in NonlinearAnalysis function";
					return -1;
				}	
			}
			fbar_y=0;
			fy=0;
			double interp;
			int index=0;
			for (int i=0; i <= fbar.count()-2 ; i++ ){
				if ( (target_ductility - mubar[i])*(target_ductility-mubar[i+1]) < 0 ){
					interp = qExp((qLn(fbar[i+1])-qLn(fbar[i]))/(qLn(mubar[i+1])-qLn(mubar[i]))*(qLn(target_ductility)-qLn(mubar[i+1]))+qLn(fbar[i+1]));
					if (interp>fbar_y){
						fbar_y=interp;
						index=i;
					}
				}
			}
			fy = ks* fbar_y* DeUsed;
			nla_status = nonlinearAnalysisSDOF(he,fy,alpha,ks,K,C,M,L,FIMh,FIMr,n,dt,dof, &ductility, &Dm_tot, &Am, &Am_tot,threadID);
			if (nla_status) {  
				rCritical(threadID) << "Error in NonlinearAnalysis function";
				return -1;
			}
			double f1=fbar[index];
			double f2=fbar[index+1];
			double mu1=mubar[index];
			double mu2=mubar[index+1];
			int trial=0;
			while ( (qAbs(ductility-target_ductility)>=0.01) && (trial<20) ){
				trial=trial+1;
				fbar_y=qExp((qLn(f2)-qLn(f1))/(qLn(mu2)-qLn(mu1))*(qLn(target_ductility)-qLn(mu2))+qLn(f2));
				fy = ks* fbar_y* DeUsed;
				nla_status = nonlinearAnalysisSDOF(he,fy,alpha,ks,K,C,M,L,FIMh,FIMr,n,dt,dof, &ductility, &Dm_tot, &Am, &Am_tot,threadID);
				if (nla_status) {  
					rCritical(threadID) << "Error in NonlinearAnalysis function";
					return -1;
				}
				mu1=mu2;
				f1=f2;
				mu2=ductility;
				f2=fbar_y;
			}
			if (trial==20){
				first=fbar[index];
				last=fbar[index+1];
				for (int i=0; i<=fbar.count()-1 ; i++){
					fbar[i] = first + i*(last-first)/10;
				}
				mubar.fill(0.0);
				if (first==1){
					mubar[0]=1;
					start=1;
				}
				else{
					start=0;
				}
				for (int i=start; i<=fbar.count()-1 ; i++){
					fy = ks* fbar[i]* DeUsed;
					nla_status = nonlinearAnalysisSDOF(he,fy,alpha,ks,K,C,M,L,FIMh,FIMr,n,dt,dof, &mubar[i], &Dm_tot, &Am, &Am_tot,threadID);
					if (nla_status) {  
						rCritical(threadID) << "Error in NonlinearAnalysis function";
						return -1;
					}
				}
				fbar_y = 0;
				for (int i=0 ; i<=fbar.count()-2 ; i++){
					if ((target_ductility-mubar[i])*(target_ductility-mubar[i+1])<0){
						interp=qExp((qLn(fbar[i+1])-qLn(fbar[i]))/(qLn(mubar[i+1])-qLn(mubar[i]))*(qLn(target_ductility)-qLn(mubar[i+1]))+qLn(fbar[i+1]));
						if (interp>fbar_y){
							fbar_y=interp;
						}
					}
				}
				if (fbar_y==0){
					fbar_y=qExp( ((qLn(fbar[10])-qLn(fbar[0]))/(qLn(mubar[10])-qLn(mubar[0])))*(qLn(target_ductility)-qLn(mubar[10]))+qLn(fbar[10]) );
				}
				fy = ks* fbar_y* DeUsed;
				nla_status = nonlinearAnalysisSDOF(he,fy,alpha,ks,K,C,M,L,FIMh,FIMr,n,dt,dof, &ductility, &Dm_tot, &Am, &Am_tot,threadID);
				if (nla_status) {  
					rCritical(threadID) << "Error in NonlinearAnalysis function";
					return -1;
				}
			} // end if (trial == 20)

			R=1/fbar_y;
			Rssi = R;
			Rfix = R;
			Dm =ductility*DeUsed*fbar_y;
			if (ductility < 1){
				//ductility=1;
			}
		} // end if (theRunMode == 2)

		//-------------------------------------------------

		if (theRunMode == RSoilStructureInteractionModel::GivenRFixSameFy){

			// 3) Given Rfix -> Fyfix=Fyssi -> Ducssi
			DeUsed=Defix; //Same Fy	
			fbar_y = 1/R;
			Rfix = R;
			fy = ks* DeUsed* fbar_y;

			// Nonlinear Analysis of FixedBase Structure: FixedDuc
			nla_status = nonlinearAnalysisSDOF(he,fy,alpha,ks,Kfix,Cfix,Mfix,Lfix, FIMhfix,FIMrfix,n,dt,1 , &FixedDuc, &Dm_tot_fix, &Am_fix, &Am_tot_fix,threadID);
			if (nla_status) {  
				rCritical(threadID) << "Error in NonlinearAnalysis function";
				return -1;
			}

			// Nonlinear Analysis of Flexible Base Structure: ductility
			if (dof==4){
				nla_status = nonlinearAnalysisSDOF(he,fy,alpha,ks,K,C,M,L, FIMh,FIMr,n,dt,dof , &ductility, &Dm_tot, &Am, &Am_tot,threadID);
				if (nla_status) {  
					rCritical(threadID) << "Error in NonlinearAnalysis function";
					return -1;
				}
				Rssi = (ks*Dessi)/fy; 
			}
			else
			{
				ductility=FixedDuc;
				Dm_tot=Dm_tot_fix;
				Am_tot=Am_tot_fix;
				Am=Am_fix;
				Rssi = Rfix;
			}

			Dm = ductility*DeUsed*fbar_y; // Dessi*fbar_yssi = Defix*fbar_yfix = Fy/ks
			if (ductility < 1){
				//ductility=1;
			}
		} //end if RunMode==3

		//--------------------------------------

		if (theRunMode == RSoilStructureInteractionModel::GivenDucFixSameFy){ 
			// 4)Given target_ducfix -> Fyfix = Fyssi -> Ducssi
			DeUsed=Defix; //Same Fy
			//NonLinear Analysis of FixedBase Structure: Fyfix
			fbar_y=1;
			ductility=1;
			double multiplier=0.85;
			while (ductility<=target_ductility){
				fbar_y=fbar_y*multiplier;
				fy = ks* fbar_y* Defix;
				nla_status = nonlinearAnalysisSDOF(he,fy,alpha,ks,Kfix,Cfix,Mfix,Lfix,FIMhfix,FIMrfix,n,dt,1, &ductility, &Dm_tot, &Am, &Am_tot,threadID);

				if (nla_status) {  
					rCritical(threadID) << "Error in NonlinearAnalysis function";
					return -1;
				}
			}

			//first=min(fbar_y/(0.85^5),1); 
			double first=1;
			if ( fbar_y/gsl_pow_5(0.85) < 1)
				first = fbar_y/gsl_pow_5(0.85);
			double last=fbar_y;
			int start;
			QVector<double> fbar;
			for (int i=0; i<=10 ; i++){
				fbar.insert(i, first+i*(last-first)/10);
			}
			QVector<double> mubar(fbar.count());
			mubar.fill(0.0);
			if (first==1){
				mubar[0]=1;
				start=1;
			}
			else{
				start=0;
			}
			for (int i=start; i <= fbar.count()-1 ; i++){
				fy = ks* fbar[i]* Defix;
				nla_status = nonlinearAnalysisSDOF(he,fy,alpha,ks,Kfix,Cfix,Mfix,Lfix,FIMhfix,FIMrfix,n,dt,1, &mubar[i], &Dm_tot, &Am, &Am_tot,threadID);
				if (nla_status) {  
					rCritical(threadID) << "Error in NonlinearAnalysis function";
					return -1;
				}	
			}
			fbar_y=0;
			double interp;
			int index=0;
			for (int i=0; i <= fbar.count()-2 ; i++ ){
				if ( (target_ductility - mubar[i])*(target_ductility-mubar[i+1]) < 0 ){
					interp = qExp((qLn(fbar[i+1])-qLn(fbar[i]))/(qLn(mubar[i+1])-qLn(mubar[i]))*(qLn(target_ductility)-qLn(mubar[i+1]))+qLn(fbar[i+1]));
					if (interp>fbar_y){
						fbar_y=interp;
						index=i;
					}
				}
			}
			fy = ks* fbar_y* Defix;
			nla_status = nonlinearAnalysisSDOF(he,fy,alpha,ks,Kfix,Cfix,Mfix,Lfix,FIMhfix,FIMrfix,n,dt,1, &ductility, &Dm_tot, &Am, &Am_tot,threadID);
			if (nla_status) {  
				rCritical(threadID) << "Error in NonlinearAnalysis function";
				return -1;
			}
			double f1=fbar[index];
			double f2=fbar[index+1];
			double mu1=mubar[index];
			double mu2=mubar[index+1];
			int trial=0;
			while ( (qAbs(ductility-target_ductility)>=0.01) && (trial<20) ){
				trial=trial+1;
				fbar_y=qExp((qLn(f2)-qLn(f1))/(qLn(mu2)-qLn(mu1))*(qLn(target_ductility)-qLn(mu2))+qLn(f2));
				fy = ks* fbar_y* Defix;
				nla_status = nonlinearAnalysisSDOF(he,fy,alpha,ks,Kfix,Cfix,Mfix,Lfix,FIMhfix,FIMrfix,n,dt,1, &ductility, &Dm_tot, &Am, &Am_tot,threadID);
				if (nla_status) {  
					rCritical(threadID) << "Error in NonlinearAnalysis function";
					return -1;
				}
				mu1=mu2;
				f1=f2;
				mu2=ductility;
				f2=fbar_y;
			}
			if (trial==20){
				first=fbar[index];
				last=fbar[index+1];
				for (int i=0; i<=fbar.count()-1 ; i++){
					fbar[i] = first + i*(last-first)/10;
				}
				mubar.fill(0.0);
				if (first==1){
					mubar[0]=1;
					start=1;
				}
				else{
					start=0;
				}
				for (int i=start; i<=fbar.count()-1 ; i++){
					fy = ks* fbar[i]* Defix;
					nla_status = nonlinearAnalysisSDOF(he,fy,alpha,ks,Kfix,Cfix,Mfix,Lfix,FIMhfix,FIMrfix,n,dt,1, &mubar[i], &Dm_tot, &Am, &Am_tot,threadID);
					if (nla_status) {  
						rCritical(threadID) << "Error in NonlinearAnalysis function";
						return -1;
					}
				}
				fbar_y=0;
				for (int i=0 ; i<=fbar.count()-2 ; i++){
					if ((target_ductility-mubar[i])*(target_ductility-mubar[i+1])<0){
						interp=qExp((qLn(fbar[i+1])-qLn(fbar[i]))/(qLn(mubar[i+1])-qLn(mubar[i]))*(qLn(target_ductility)-qLn(mubar[i+1]))+qLn(fbar[i+1]));
						if (interp>fbar_y){
							fbar_y=interp;
						}
					}
				}
				if (fbar_y==0){
					fbar_y=qExp( ((qLn(fbar[10])-qLn(fbar[0]))/(qLn(mubar[10])-qLn(mubar[0])))*(qLn(target_ductility)-qLn(mubar[10]))+qLn(fbar[10]) );
				}
				fy = ks* fbar_y* Defix;
				nla_status = nonlinearAnalysisSDOF(he,fy,alpha,ks,Kfix,Cfix,Mfix,Lfix,FIMhfix,FIMrfix,n,dt,1, &ductility, &Dm_tot, &Am, &Am_tot,threadID);
				if (nla_status) {  
					rCritical(threadID) << "Error in NonlinearAnalysis function";
					return -1;
				}
			} // end if (trial == 20)

			R=1/fbar_y;
			Rfix = R;
			if (dof==4) {
				fy = ks* fbar_y* DeUsed;
				nla_status = nonlinearAnalysisSDOF(he,fy,alpha,ks,K,C,M,L,FIMh,FIMr,n,dt,dof, &ductility, &Dm_tot, &Am,&Am_tot,threadID);
				if (nla_status) {  
					rCritical(threadID) << "Error in NonlinearAnalysis function";
					return -1;
				}
				Rssi = ks*Dessi/fy;
			}
			Dm = ductility*DeUsed*fbar_y;
			if (ductility < 1){
				//ductility=1;
			}

		} // end if (theRunMode == 4)

		//-------------------------------------------------

		if (theRunMode == RSoilStructureInteractionModel::GivenDucSSISameFy){ 
			// 5) Given target_ducssi -> Fyssi = Fyfix -> Rfix
			DeUsed=Defix; //Same Fy
			//NonLinear Analysis of Soil Structure System : Fyssi
			fbar_y=1;
			ductility=1;
			double multiplier=0.85;
			while (ductility<=target_ductility){
				fbar_y=fbar_y*multiplier;
				fy = ks* fbar_y* DeUsed;
				nla_status = nonlinearAnalysisSDOF(he,fy,alpha,ks,K,C,M,L,FIMh,FIMr,n,dt,dof, &ductility, &Dm_tot, &Am, &Am_tot,threadID);

				if (nla_status) {  
					rCritical(threadID) << "Error in NonlinearAnalysis function";
					return -1;
				}
			}

			//first=min(fbar_y/(0.85^5),1); 
			double first=1;
			if ( fbar_y/gsl_pow_5(0.85) < 1)
				first = fbar_y/gsl_pow_5(0.85);
			double last=fbar_y;
			int start;
			QVector<double> fbar;
			for (int i=0; i<=10 ; i++){
				fbar.insert(i, first+i*(last-first)/10);
			}
			QVector<double> mubar(fbar.count());
			mubar.fill(0.0);
			if (first==1){
				mubar[0]=1;
				start=1;
			}
			else{
				start=0;
			}
			for (int i=start; i <= fbar.count()-1 ; i++){
				fy = ks* fbar[i]* DeUsed;
				nla_status = nonlinearAnalysisSDOF(he,fy,alpha,ks,K,C,M,L,FIMh,FIMr,n,dt,dof, &mubar[i], &Dm_tot, &Am,&Am_tot,threadID);
				if (nla_status) {  
					rCritical(threadID) << "Error in NonlinearAnalysis function";
					return -1;
				}	
			}
			fbar_y=0;
			double interp;
			int index=0;
			for (int i=0; i <= fbar.count()-2 ; i++ ){
				if ( (target_ductility - mubar[i])*(target_ductility-mubar[i+1]) < 0 ){
					interp = qExp((qLn(fbar[i+1])-qLn(fbar[i]))/(qLn(mubar[i+1])-qLn(mubar[i]))*(qLn(target_ductility)-qLn(mubar[i+1]))+qLn(fbar[i+1]));
					if (interp>fbar_y){
						fbar_y=interp;
						index=i;
					}
				}
			}
			fy = ks* fbar_y* DeUsed;
			nla_status = nonlinearAnalysisSDOF(he,fy,alpha,ks,K,C,M,L,FIMh,FIMr,n,dt,dof, &ductility, &Dm_tot, &Am, &Am_tot,threadID);
			if (nla_status) {  
				rCritical(threadID) << "Error in NonlinearAnalysis function";
				return -1;
			}
			double f1=fbar[index];
			double f2=fbar[index+1];
			double mu1=mubar[index];
			double mu2=mubar[index+1];
			int trial=0;
			while ( (qAbs(ductility-target_ductility)>=0.01) && (trial<20) ){
				trial=trial+1;
				fbar_y=qExp((qLn(f2)-qLn(f1))/(qLn(mu2)-qLn(mu1))*(qLn(target_ductility)-qLn(mu2))+qLn(f2));
				fy = ks* fbar_y* DeUsed;
				nla_status = nonlinearAnalysisSDOF(he,fy,alpha,ks,K,C,M,L,FIMh,FIMr,n,dt,dof, &ductility, &Dm_tot, &Am, &Am_tot,threadID);
				if (nla_status) {  
					rCritical(threadID) << "Error in NonlinearAnalysis function";
					return -1;
				}
				mu1=mu2;
				f1=f2;
				mu2=ductility;
				f2=fbar_y;
			}
			if (trial==20){
				first=fbar[index];
				last=fbar[index+1];
				for (int i=0; i<=fbar.count()-1 ; i++){
					fbar[i] = first + i*(last-first)/10;
				}
				mubar.fill(0.0);
				if (first==1){
					mubar[0]=1;
					start=1;
				}
				else{
					start=0;
				}
				for (int i=start; i<=fbar.count()-1 ; i++){
					fy = ks* fbar[i]* DeUsed;
					nla_status = nonlinearAnalysisSDOF(he,fy,alpha,ks,K,C,M,L,FIMh,FIMr,n,dt,dof, &mubar[i], &Dm_tot, &Am, &Am_tot,threadID);
					if (nla_status) {  
						rCritical(threadID) << "Error in NonlinearAnalysis function";
						return -1;
					}
				}
				fbar_y=0;
				for (int i=0 ; i<=fbar.count()-2 ; i++){
					if ((target_ductility-mubar[i])*(target_ductility-mubar[i+1])<0){
						interp=qExp((qLn(fbar[i+1])-qLn(fbar[i]))/(qLn(mubar[i+1])-qLn(mubar[i]))*(qLn(target_ductility)-qLn(mubar[i+1]))+qLn(fbar[i+1]));
						if (interp>fbar_y){
							fbar_y=interp;
						}
					}
				}
				if (fbar_y==0){
					fbar_y=qExp( ((qLn(fbar[10])-qLn(fbar[0]))/(qLn(mubar[10])-qLn(mubar[0])))*(qLn(target_ductility)-qLn(mubar[10]))+qLn(fbar[10]) );
				}
				fy = ks* fbar_y* DeUsed;
				nla_status = nonlinearAnalysisSDOF(he,fy,alpha,ks,K,C,M,L,FIMh,FIMr,n,dt,dof, &ductility, &Dm_tot, &Am,&Am_tot,threadID);
				if (nla_status) {  
					rCritical(threadID) << "Error in NonlinearAnalysis function";
					return -1;
				}
			} // end if (trial == 20)

			R=1/fbar_y;
			Rfix = R;
			Rssi = ks*Dessi/fy;
			Dm =ductility*DeUsed*fbar_y;
			if (ductility < 1){
				//ductility=1;
			}

		} // end if (theRunMode == 5)

		//-------------------------------------------------
		double Duc_B=0;
		double Dm_B=0;
		double Dm_tot_B=0;
		double Am_B=0;
		double Am_tot_B=0;
		double VhatRatio=1.0;
		T_SSI=0;
		if (theRunMode == RSoilStructureInteractionModel::GivenRReducedFyATC){

			// 6) Given Rfix -> Fyfix -> FyReduced(ATC) -> Ducssi
			T_SSI=meanTHatAtc;
			double VAtc,VhatAtc,CsAtc,CsHatAtc,TRatioAtc,Beta0Atc,BetaHatAtc,DVAtc;	
			double AvAtc=0.2;
			double AaAtc=AvAtc;   //???????
			double SAtc=1.2;                        //Corresponding to Soil Profile S2
			double RAtc=R;                          //Corresponding to Dual Structural System with Braced Frames
			CsAtc=1.2*AvAtc*SAtc/(RAtc*qPow(meanTn,2/3));
			if (CsAtc>2.5*AaAtc/RAtc)	//Base Shear Limit Corresponding to Soil Profile S2
				CsAtc=2.5*AaAtc/RAtc;       
			VAtc=CsAtc*ms*9.81;		//fy=VAtc	
			// Nonlinear Analysis of FixedBase Structure: FixedDuc (System A)
			nla_status = nonlinearAnalysisSDOF(he, VAtc, alpha, ks, Kfix, Cfix, Mfix, Lfix, FIMhfix, FIMrfix, n, dt, 1, &FixedDuc, &Dm_tot_fix, &Am_fix,&Am_tot_fix,threadID);
			if (nla_status) {  
				rCritical(threadID) << "Error in NonlinearAnalysis function";
				return -1;
			}
			Dm_fix = (FixedDuc*VAtc)/ks;	

			if (dof==4) // Modify Damping according to ATC regulations:
			{	
				// System B
				nla_status = nonlinearAnalysisSDOF(he, VAtc, alpha, ks, K, C, M, L, FIMh, FIMr, n, dt, dof , &Duc_B, &Dm_tot_B, &Am_B, &Am_tot_B,threadID);
				if (nla_status) {  
					rCritical(threadID) << "Error in NonlinearAnalysis function";
					return -1;
				}
				Dm_B = (Duc_B*VAtc)/ks;

				// System C
				TRatioAtc=meanTHatAtc/meanTn;
				if (meanh_r0<=1) 
					Beta0Atc= -0.2531*TRatioAtc*TRatioAtc + 1.0401*TRatioAtc - 0.7889;    //Corresponding to Av>=0.2
				if ((meanh_r0>1) && (meanh_r0<2))  
					Beta0Atc= -0.123*TRatioAtc*TRatioAtc + 0.5454*TRatioAtc - 0.4235;    
				if ((meanh_r0>=2) && (meanh_r0<3)) 
					Beta0Atc= -0.0737*TRatioAtc*TRatioAtc + 0.3398*TRatioAtc - 0.266;      
				if ((meanh_r0>=3) && (meanh_r0<4)) 
					Beta0Atc= -0.0653*TRatioAtc*TRatioAtc + 0.2688*TRatioAtc - 0.2024;    
				if (meanh_r0>=5) 
					Beta0Atc= -0.0563*TRatioAtc*TRatioAtc + 0.2269*TRatioAtc - 0.1702;   

				BetaHatAtc=Beta0Atc+0.05/gsl_pow_3(TRatioAtc);
				if (BetaHatAtc<0.05) 
					BetaHatAtc=0.05;

				CsHatAtc=1.2*AvAtc*SAtc/(RAtc*qPow(meanTHatAtc,2/3));
				if (CsHatAtc>2.5*AaAtc/RAtc)
					CsHatAtc=2.5*AaAtc/RAtc;
				DVAtc=(CsAtc-CsHatAtc*qPow((0.05/BetaHatAtc),0.4))* 0.7 * ms*9.81;
				VhatAtc=VAtc-DVAtc;
				if (VhatAtc<0.7*VAtc)			//fy=VhatAtc
					VhatAtc=0.7*VAtc;
				VhatRatio=VhatAtc/VAtc;
				nla_status = nonlinearAnalysisSDOF(he, VhatAtc,alpha,ks,K,C,M,L, FIMh,FIMr,n,dt,dof , &ductility, &Dm_tot, &Am, &Am_tot,threadID);
				if (nla_status) {  
					rCritical(threadID) << "Error in NonlinearAnalysis function";
					return -1;
				}

			}
			else{ //if dof==1
				ductility=FixedDuc;
				Duc_B=FixedDuc;
				Dm_tot=Dm_tot_fix;
				Am_tot=Am_tot_fix;
				Am=Am_fix;
			}

			Dm = (ductility*VhatAtc)/ks;

			//if (ductility < 1){
			//	ductility=1;
			//}
		} //end if (theRunMode == 6)

		//-------------------------------------

		/* ATC Mahsuli Code

		//ATC3-06 Fixed Base Shear
		AvAtc:=0.2;
		SAtc:=1.2;                        //Corresponding to Soil Profile S2
		RAtc:=6;                          //Corresponding to Dual Structural System with Braced Frames
		CsAtc:=1.2*AvAtc*SAtc/(RAtc*Power(Tn,2/3));

		AaAtc:=AvAtc;   ////??????
		if CsAtc>2.5*AaAtc/RAtc then CsAtc:=2.5*AaAtc/RAtc;       //Base Shear Limit Corresponding to Soil Profile S2
		VAtc:=CsAtc*ms*ge;



		kyAtc:=8*G*r0/(2-v)*(1+2/3*e_r0);
		ktetaAtc:=8*G*r0*r0*r0/(3*(1-v))*(1+2*e_r0);
		THatAtc:=Tn*sqrt(1+ks/kyAtc*(1+kyAtc*h*h/ktetaAtc));
		TRatioAtc:=THatAtc/Tn;
		if h_r0<=1 then Beta0Atc:=-0.2531*TRatioAtc*TRatioAtc+1.0401*TRatioAtc-0.7889;    //Corresponding to Av>=0.2
		if h_r0=1.5 then Beta0Atc:=-0.123*TRatioAtc*TRatioAtc+0.5454*TRatioAtc-0.4235;    //Corresponding to Av>=0.2
		if h_r0=2 then Beta0Atc:=-0.0737*TRatioAtc*TRatioAtc+0.3398*TRatioAtc-0.266;      //Corresponding to Av>=0.2
		if h_r0=3 then Beta0Atc:=-0.0653*TRatioAtc*TRatioAtc+0.2688*TRatioAtc-0.2024;     //Corresponding to Av>=0.2
		if h_r0=5 then Beta0Atc:=-0.0563*TRatioAtc*TRatioAtc+0.2269*TRatioAtc-0.1702;     //Corresponding to Av>=0.2
		BetaHatAtc:=Beta0Atc+0.05/Power(TRatioAtc,3);
		if BetaHatAtc<0.05 then BetaHatAtc:=0.05;

		CsHatAtc:=1.2*AvAtc*SAtc/(RAtc*Power(THatAtc,2/3));
		if CsHatAtc>2.5*AaAtc/RAtc then CsHatAtc:=2.5*AaAtc/RAtc;
		DVAtc:=(CsAtc-CsHatAtc*Power((0.05/BetaHatAtc),0.4))* 0.7 * ms*ge;
		VhatAtc:=VAtc-DVAtc;
		if VhatAtc<0.7*VAtc then VhatAtc:=0.7*VAtc;
		*/

		//-------------------------------------------------Run mode 7(GivenRReducedFyASCE_05)

		if (theRunMode == RSoilStructureInteractionModel::GivenRReducedFyASCE_05){

			// 7) Given Rfix -> Fyfix -> FyReduced(ASCE-05) -> Ducssi
			T_SSI=meanTHatASCE;
			double VASCE,VhatASCE,CsASCE,CsHatASCE,TRatioASCE,Beta0ASCE,BetaHatASCE,DVASCE;	
			double IeASCE=1.0;
			double RASCE=R;                          //Corresponding to Dual Structural System with Braced Frames 

			CsASCE=SdsASCE/(RASCE/IeASCE);
			if (CsASCE>SdiASCE/((RASCE*meanTn)/IeASCE))	//not exceed 
				CsASCE=SdiASCE/((RASCE*meanTn)/IeASCE);  
			double minCsASCE=0.044*SdsASCE*IeASCE; //minimum of CsASCE
			if (minCsASCE<0.01)
				minCsASCE=0.01;
			if (CsASCE<minCsASCE)
				CsASCE=minCsASCE;

			VASCE=CsASCE*ms*9.81;
			// Nonlinear Analysis of FixedBase Structure: FixedDuc (System A)
			nla_status = nonlinearAnalysisSDOF(he,VASCE,alpha,ks,Kfix,Cfix,Mfix,Lfix, FIMhfix,FIMrfix,n,dt,1 , &FixedDuc, &Dm_tot_fix, &Am_fix,&Am_tot_fix,threadID);
			if (nla_status) {  
				rCritical(threadID) << "Error in NonlinearAnalysis function";
				return -1;
			}

			Dm_fix = (FixedDuc*VASCE)/ks;

			if (dof==4) // Modify Damping according to ASCE regulations:
			{	
				// System B
				nla_status = nonlinearAnalysisSDOF(he,VASCE,alpha,ks,K,C,M,L, FIMh,FIMr,n,dt,dof , &Duc_B, &Dm_tot_B, &Am_B, &Am_tot_B,threadID);
				if (nla_status) {  
					rCritical(threadID) << "Error in NonlinearAnalysis function";
					return -1;
				}
				Dm_B = (Duc_B*VASCE)/ks;

				// System C
				TRatioASCE=meanTHatASCE/meanTn;
				if (TRatioASCE<=2){
				if (meanh_r0<=1) 
					Beta0ASCE= -0.2531*TRatioASCE*TRatioASCE + 1.0401*TRatioASCE - 0.7889;    //Corresponding to Sds/2.5>=0.2
				if ((meanh_r0>1) && (meanh_r0<2))  
					Beta0ASCE= -0.123*TRatioASCE*TRatioASCE + 0.5454*TRatioASCE - 0.4235;    
				if ((meanh_r0>=2) && (meanh_r0<3)) 
					Beta0ASCE= -0.0737*TRatioASCE*TRatioASCE + 0.3398*TRatioASCE - 0.266;      
				if ((meanh_r0>=3) && (meanh_r0<4)) 
					Beta0ASCE= -0.0653*TRatioASCE*TRatioASCE + 0.2688*TRatioASCE - 0.2024;    
				if (meanh_r0>=5) 
					Beta0ASCE= -0.0563*TRatioASCE*TRatioASCE + 0.2269*TRatioASCE - 0.1702;   
				}else if (TRatioASCE>2){	// In this step, if TRatio>2 then we assume TRato=2! (Our judgment)
				if (meanh_r0<=1) 
					Beta0ASCE= -0.2531*2.0*2.0 + 1.0401*2.0 - 0.7889;    //Corresponding to Sds/2.5>=0.2
				if ((meanh_r0>1) && (meanh_r0<2))  
					Beta0ASCE= -0.123*2.0*2.0 + 0.5454*2.0 - 0.4235;    
				if ((meanh_r0>=2) && (meanh_r0<3)) 
					Beta0ASCE= -0.0737*2.0*2.0 + 0.3398*2 - 0.266;      
				if ((meanh_r0>=3) && (meanh_r0<4)) 
					Beta0ASCE= -0.0653*2.0*2.0 + 0.2688*2.0 - 0.2024;    
				if (meanh_r0>=5) 
					Beta0ASCE= -0.0563*2.0*2.0 + 0.2269*2.0 - 0.1702;
				}

				BetaHatASCE=Beta0ASCE+0.05/gsl_pow_3(TRatioASCE);

				if (BetaHatASCE<0.05) 
					BetaHatASCE=0.05;

				CsHatASCE=SdsASCE/(RASCE);
				if (CsHatASCE>SdiASCE/(RASCE*meanTHatASCE))	//not exceed 
					CsHatASCE=SdiASCE/(RASCE*meanTHatASCE); 
				if (CsHatASCE<minCsASCE)
					CsHatASCE=minCsASCE;

				DVASCE=(CsASCE-CsHatASCE*qPow((0.05/BetaHatASCE),0.4))* 0.7 * ms*9.81;
				VhatASCE=VASCE-DVASCE;
				if (VhatASCE<0.7*VASCE)			//fy=VhatAtc
					VhatASCE=0.7*VASCE;
				VhatRatio=VhatASCE/VASCE;
				nla_status = nonlinearAnalysisSDOF(he,VhatASCE,alpha,ks,K,C,M,L, FIMh,FIMr,n,dt,dof , &ductility, &Dm_tot, &Am, &Am_tot,threadID);
				if (nla_status) {  
					rCritical(threadID) << "Error in NonlinearAnalysis function";
					return -1;
				}
			}
			else{ //if dof==1
				ductility=FixedDuc;
				Duc_B=FixedDuc;
				Dm_tot=Dm_tot_fix;
				Am_tot=Am_tot_fix;
				Am=Am_fix;
			}

			Dm = ( ductility * VhatASCE )/ks;

			//if (ductility < 1){
			//	ductility=1;
			//}
		} //end if (theRunMode == 7)
		//-------------------------------------------------Run mode 8(GivenRReducedFyASCE_10)

		if (theRunMode == RSoilStructureInteractionModel::GivenRReducedFyASCE_10){

			// 8) Given Rfix -> Fyfix -> FyReduced(ASCE-10) -> Ducssi
			T_SSI=meanTHatASCE;
			double VASCE,VhatASCE,CsASCE,CsHatASCE,TRatioASCE,Beta0ASCE,BetaHatASCE,DVASCE;	
			double IeASCE=1.0;
			double RASCE=R;                          //Corresponding to Dual Structural System with Braced Frames 

			CsASCE=SdsASCE/(RASCE/IeASCE);
			if (CsASCE>SdiASCE/((RASCE*meanTn)/IeASCE))	//not exceed 
				CsASCE=SdiASCE/((RASCE*meanTn)/IeASCE);  
			double minCsASCE=0.044*SdsASCE*IeASCE; //minimum of CsASCE
			if (minCsASCE<0.01)
				minCsASCE=0.01;
			if (CsASCE<minCsASCE)
				CsASCE=minCsASCE;

			VASCE=CsASCE*ms*9.81;	//fy=VASCE
			// Nonlinear Analysis of FixedBase Structure: FixedDuc (System A)
			nla_status = nonlinearAnalysisSDOF(he,VASCE,alpha,ks,Kfix,Cfix,Mfix,Lfix, FIMhfix,FIMrfix,n,dt,1 , &FixedDuc, &Dm_tot_fix, &Am_fix,&Am_tot_fix,threadID);
			if (nla_status) {  
				rCritical(threadID) << "Error in NonlinearAnalysis function";
				return -1;
			}

			Dm_fix = (FixedDuc*VASCE)/ks;

			if (dof==4) // Modify Damping according to ASCE regulations:
			{	
				// System B
				nla_status = nonlinearAnalysisSDOF(he,VASCE,alpha,ks,K,C,M,L, FIMh,FIMr,n,dt,dof , &Duc_B, &Dm_tot_B, &Am_B, &Am_tot_B,threadID);
				if (nla_status) {  
					rCritical(threadID) << "Error in NonlinearAnalysis function";
					return -1;
				}
				Dm_B = (Duc_B*VASCE)/ks;
				// System C
				TRatioASCE=meanTHatASCE/meanTn;

				if (TRatioASCE<=2){
					if (meanh_r0<=0.75) 
						Beta0ASCE= -0.0757*TRatioASCE*TRatioASCE*TRatioASCE*TRatioASCE+0.7583*TRatioASCE*TRatioASCE*TRatioASCE-2.6898*TRatioASCE*TRatioASCE + 4.0774*TRatioASCE - 2.0703;    //Corresponding to Sds/2.5>=0.2
					if ((meanh_r0>=0.75) && (meanh_r0<1.5))  
						Beta0ASCE= -0.1800*TRatioASCE*TRatioASCE + 0.682*TRatioASCE - 0.5020;    
					if ((meanh_r0>=1.5) && (meanh_r0<2.5)) 
						Beta0ASCE= -0.0694*TRatioASCE*TRatioASCE + 0.2736*TRatioASCE - 0.2042;        
					if (meanh_r0>=2.5) // Based on our discussion with Dr.Ghannad ( It is similar to the 2800's approach.)
						Beta0ASCE= 0;  
				}else if(TRatioASCE>2){ // In this step, if TRatio>2 then we assume TRato=2! (Our judgement)
					if (meanh_r0<=0.75) 
						Beta0ASCE= -0.0757*2.0*2.0*2.0*2.0+0.7583*2.0*2.0*2.0-2.6898*2.0*2.0 + 4.0774*2.0 - 2.0703;    //Corresponding to Sds/2.5>=0.2
					if ((meanh_r0>=0.75) && (meanh_r0<1.5))  
						Beta0ASCE= -0.1800*2.0*2.0 + 0.682*2.0 - 0.5020;    
					if ((meanh_r0>=1.5) && (meanh_r0<2.5)) 
						Beta0ASCE= -0.0694*2.0*2.0 + 0.2736*2.0 - 0.2042;        
					if (meanh_r0>=2.5) // Based on our discussion with Dr.Ghannad ( It is similar to the 2800's approach.)
						Beta0ASCE= 0;
				}
				BetaHatASCE=Beta0ASCE+0.05/gsl_pow_3(TRatioASCE);

				if (BetaHatASCE<0.05) 
					BetaHatASCE=0.05;

				CsHatASCE=SdsASCE/(RASCE);
				if (CsHatASCE>SdiASCE/(RASCE*meanTHatASCE))	//not exceed 
					CsHatASCE=SdiASCE/(RASCE*meanTHatASCE); 
				if (CsHatASCE<minCsASCE)
					CsHatASCE=minCsASCE;

				DVASCE=(CsASCE-CsHatASCE*qPow((0.05/BetaHatASCE),0.4))* 0.7 * ms*9.81;
				VhatASCE=VASCE-DVASCE;
				if (VhatASCE<0.7*VASCE)
					VhatASCE=0.7*VASCE;		//fy=VhatAtc
				VhatRatio=VhatASCE/VASCE;
				nla_status = nonlinearAnalysisSDOF(he, VhatASCE, alpha, ks, K, C, M, L, FIMh, FIMr, n, dt, dof , &ductility, &Dm_tot, &Am, &Am_tot,threadID);
				if (nla_status) {  
					rCritical(threadID) << "Error in NonlinearAnalysis function";
					return -1;
				}

			}
			else{ //if dof==1
				ductility=FixedDuc;
				Duc_B=FixedDuc;
				Dm_tot=Dm_tot_fix;
				Am_tot=Am_tot_fix;
				Am=Am_fix;
			}

			Dm = (ductility*VhatASCE)/ks;

			//if (ductility < 1){
			//	ductility=1;
			//}
		} //end if (theRunMode == 8)
		//-------------------------------------------------Run mode 9(GivenRReducedNEHRP)

		if (theRunMode == RSoilStructureInteractionModel::GivenRReducedFyNEHRP){

			// 9) Given Rfix -> Fyfix -> FyReduced(NEHRP) -> Ducssi
			T_SSI=meanTHatNEHRP;
			double VNEHRP,VhatNEHRP,CsNEHRP,CsHatNEHRP,TRatioNEHRP,DVNEHRP;	
			double IeNEHRP=1.0;
			double RNEHRP=R;                          //Corresponding to Dual Structural System with Braced Frames 
			CsNEHRP=SdsASCE/(RNEHRP/IeNEHRP);
			if (CsNEHRP>SdiASCE/((RNEHRP*meanTn)/IeNEHRP))	//not exceed 
				CsNEHRP=SdiASCE/((RNEHRP*meanTn)/IeNEHRP);  
			double minCsNEHRP=0.044*SdsASCE*IeNEHRP; //minimum of CsASCE
			if (minCsNEHRP<0.01)
				minCsNEHRP=0.01;
			if (CsNEHRP<minCsNEHRP)
				CsNEHRP=minCsNEHRP;
			VNEHRP=CsNEHRP*ms*9.81;	//fy=VNEHRP
			// Nonlinear Analysis of FixedBase Structure: FixedDuc (System A)
			nla_status = nonlinearAnalysisSDOF(he,VNEHRP,alpha,ks,Kfix,Cfix,Mfix,Lfix, FIMhfix,FIMrfix,n,dt,1 , &FixedDuc, &Dm_tot_fix, &Am_fix,&Am_tot_fix,threadID);
			if (nla_status) {  
				rCritical(threadID) << "Error in NonlinearAnalysis function";
				return -1;
			}
			Dm_fix = (FixedDuc*VNEHRP)/ks;

			if (dof==4) // Modify Damping according to ASCE regulations:
			{	
				// System B
				nla_status = nonlinearAnalysisSDOF(he,VNEHRP,alpha,ks,K,C,M,L, FIMh,FIMr,n,dt,dof , &Duc_B, &Dm_tot_B, &Am_B, &Am_tot_B,threadID);
				if (nla_status) {  
					rCritical(threadID) << "Error in NonlinearAnalysis function";
					return -1;
				}
				Dm_B = (Duc_B*VNEHRP)/ks;
				// System C
				TRatioNEHRP=meanTHatNEHRP/meanTn;
				double miyu=R_primary/Omega0NEHRP;
				double TRatioNEHRP_eff= qSqrt(1+(1/miyu)*(TRatioNEHRP*TRatioNEHRP-1.0));

				CsHatNEHRP=SdsASCE/(RNEHRP);
				if (CsHatNEHRP>SdiASCE/(RNEHRP*meanTHatNEHRP))	//not exceed 
					CsHatNEHRP=SdiASCE/(RNEHRP*meanTHatNEHRP); 
				if (CsHatNEHRP<minCsNEHRP)
					CsHatNEHRP=minCsNEHRP;
				double alfa;
				if (R_primary <= 3.0)
					alfa=0.7;
				if ((3.0 < R_primary)&&(R_primary < 6.0))
					alfa=0.5+R_primary/15;
				if (R_primary >= 6.0)
					alfa=0.9;		
				// Calculationg B0 and B_SSI
				double beta = 0.05;
				//-----beta_s:
				//Importating table:
				gsl_matrix *beta_stable = gsl_matrix_calloc(3,4);
				gsl_matrix_set(beta_stable,0,0,0.01);
				gsl_matrix_set(beta_stable,0,1,0.01);
				gsl_matrix_set(beta_stable,0,2,0.03);
				gsl_matrix_set(beta_stable,0,3,0.05);
				gsl_matrix_set(beta_stable,1,0,0.01);
				gsl_matrix_set(beta_stable,1,1,0.02);
				gsl_matrix_set(beta_stable,1,2,0.07);
				gsl_matrix_set(beta_stable,1,3,0.15);
				gsl_matrix_set(beta_stable,2,0,0.01);
				gsl_matrix_set(beta_stable,2,1,0.05);
				gsl_matrix_set(beta_stable,2,2,0.20);
				gsl_matrix_set(beta_stable,2,3,0.40);

				//Importing Sds/2.5 Table:
				QVector<double> SdsNEHRPVector;
				SdsNEHRPVector << 0.0 << 0.10 << 0.40 << 0.80;
				//Calculating RF:
				double beta_s;	
				for(int i=1; i<4; i++){
					if((SdsNEHRPVector[i-1]<SdsASCE/2.5) && (SdsASCE/2.5<= SdsNEHRPVector[i])){
						double beta_s2= gsl_matrix_get(beta_stable,soilnum,i);
						double Sds_2= SdsNEHRPVector[i];
						double beta_s1= gsl_matrix_get(beta_stable,soilnum,i-1);
						double Sds_1=SdsNEHRPVector[i-1];
						beta_s = ((beta_s2-beta_s1)/(Sds_2-Sds_1))*(SdsASCE/2.5-Sds_1)+ beta_s1;
					}
				}
				if(SdsASCE/2.5 > SdsNEHRPVector[3])
					beta_s=gsl_matrix_get(beta_stable,soilnum,3);

				//-----beta_r:
				double a0NEHRP=(2.0*pi*r0)/(meanTHatNEHRP*meanvs);
				double alfa_rr=1.0 -((0.35 * a0NEHRP * a0NEHRP)/(1.0 + a0NEHRP * a0NEHRP));
				double kesayNEHRP = qSqrt((2.0*(1.0-meanv))/(1.0 - 2.0 * meanv));
				if (kesayNEHRP > 2.5)
					kesayNEHRP = 2.5;
				double T_r = 2.0 * pi * qSqrt(ms/meanKh);
				double T_rr = 2.0 * pi * qSqrt((ms * meanhtotal * meanhtotal)/(alfa_rr * meanKr));
				double beta_r1=(pi/(meanKh/(meanG*r0)))*(a0NEHRP/2.0);
				double beta_rr=(((pi*kesayNEHRP/4.0)*a0NEHRP*a0NEHRP)/((meanKr/(meanG*r0*r0*r0))*(2.0+a0NEHRP*a0NEHRP)))*(a0NEHRP/(2.0*alfa_rr));
				double beta_r=(1.0/gsl_pow_2(meanTHatNEHRP/T_r))*beta_r1+(1.0/gsl_pow_2(meanTHatNEHRP/T_rr))*beta_rr;
				//-----beta_f
				double beta_f=((gsl_pow_2(TRatioNEHRP)-1)/gsl_pow_2(TRatioNEHRP))*beta_s+beta_r;
				//-----beta0 
				double beta0=beta_f + beta/gsl_pow_2(TRatioNEHRP_eff);
				if (beta0>0.2)
					beta0=0.2;
				//-----B_SSI 
				double B_SSI=4.0/(5.6-log(100*beta0));
				//-----Calculating delta_V
				DVNEHRP=(CsNEHRP-CsHatNEHRP/B_SSI)*9.81*ms;
				VhatNEHRP=VNEHRP-DVNEHRP;
				if (VhatNEHRP<=alfa*VNEHRP)
					VhatNEHRP=alfa*VNEHRP;	//fy=VhatNEHRP
				VhatRatio=VhatNEHRP/VNEHRP;
				nla_status = nonlinearAnalysisSDOF(he, VhatNEHRP, alpha, ks, K, C, M, L, FIMh, FIMr, n, dt, dof , &ductility, &Dm_tot, &Am, &Am_tot,threadID);
				if (nla_status) {  
					rCritical(threadID) << "Error in NonlinearAnalysis function";
					return -1;
				}
			}
			else{ //if dof==1
				ductility=FixedDuc;
				Duc_B=FixedDuc;
				Dm_tot=Dm_tot_fix;
				Am_tot=Am_tot_fix;
				Am=Am_fix;
			}

			Dm = (ductility*VhatNEHRP)/ks;

			//if (ductility < 1){
			//	ductility=1;
			//}
		} //end if (theRunMode == 9)

		//This runmodes are working for MDOF only
		if (theRunMode == RSoilStructureInteractionModel::GivenDucComputeModificationFactor){
			rDebug(threadID) << "This runmode does not work for SDOF systems!";
			return -1;
		}
			
		if (theRunMode == RSoilStructureInteractionModel::GivenDucSameRorFy){
			rDebug(threadID) << "This runmode does not work for SDOF systems!";
			return -1;
		}

		/*if (theRunMode == RSoilStructureInteractionModel::GivenDucSameAlpha){
			rDebug(threadID) << "This runmode does not work for SDOF systems!";
			return -1;
		}*/

		if (dof==1){
			Dessi=Defix;
			De_tot=Defix;
			Dm_tot=Dm;
			meanTHatAtc = meanTn;	
		}

		double De_Intensity=Dessi/Intensity;
		double Ae_Intensity=Aessi/(Intensity*9.81);
		double De_tot_Intensity=De_tot/Intensity;
		double Ae_tot_Intensity=Ae_tot/Intensity;
		double Dm_Intensity=Dm/Intensity;
		double Am_Intensity=Am/(Intensity*9.81);
		double Dm_tot_Intensity=Dm_tot/Intensity;
		double Am_tot_Intensity=Am_tot/(Intensity*9.81);
		double SystemDuc=Dm_tot*R/De_tot;
		double Fy_mIntensity=ks*Defix/(R*ms*Intensity);
		double Fo_mIntensity=ks*Dessi/(ms*Intensity);


	// -------------------------------------------------------------------------------- Nonlinear Analysis for SDOF End	

	// -------------------------------------------------------------------------------- Nonlinear Analysis for MDOF
	} else {//if MDOF
		int nla_status;		// nonlinearAnalysis function status
		double alpha = 0;		 // k2(after yielding)/k1(before yielding)
		double fbar_y;
		gsl_vector *fy = gsl_vector_calloc(Num);
		gsl_vector *fy_fix = gsl_vector_calloc(Num);
		gsl_vector *fy_eq = gsl_vector_calloc(1);
		gsl_vector *fy_fix_eq = gsl_vector_calloc(1);

		//Runmodes: 1)GivenRSameR, 2)GivenDucSameR, 3)GivenDucComputeModificationFactor, 4)GivenDucSameRorFy, 5)GivenRFixSameFy, 6)GivenDucFixSameFy, 7)GivenDucSSISameFy

		//-------------------------------------
		if (theRunMode == RSoilStructureInteractionModel::GivenRSameR){

			// 1) Given Rfix -> Rfix=Rssi -> Ducssi
			// Base shear distribution --> fy
			gsl_vector *tempfy = gsl_vector_calloc(Num);
			gsl_vector *tempfy_fix = gsl_vector_calloc(Num);
			fbar_y = 1/R;
			int latdist;
			gsl_vector *dist = gsl_vector_calloc(Num);
			latdist = lateralforcedistribution(Tn,Num,dist,threadID);
			if (latdist){
				rCritical(threadID) <<"Error in lateral force distribution function";
				return -1;
			}
			for (int i=1;i<=Num;i++){
				gsl_vector_set(tempfy,Num-i,VeSSI*fbar_y*gsl_vector_get(dist,i-1));
				gsl_vector_set(tempfy_fix,Num-i,VeFix*fbar_y*gsl_vector_get(dist,i-1));
			}
			double sp=0;
			double spf=0;
			for (int i=1;i<=Num;i++){
				sp=sp+gsl_vector_get(tempfy,i-1);
				gsl_vector_set(fy,i-1,sp);
				spf=spf+gsl_vector_get(tempfy_fix,i-1);
				gsl_vector_set(fy_fix,i-1,spf);
			}
			//gsl_vector *Dy = gsl_vector_calloc(Num);
			//for (int i=1;i<=Num;i++){
			//	gsl_vector_set(Dy,i-1,gsl_vector_get(fy,i-1)/gsl_vector_get(kstr,i-1));
			//}

			if (R==1){
				gsl_vector_set_all(ductility_MDOF,1);
				gsl_status = gsl_vector_memcpy(Dm_tot_MDOF,De_tot_MDOF);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL4:" << gsl_strerror(gsl_status);
					return -1;
				}
				gsl_status = gsl_vector_memcpy(Dm_MDOF,Dessi_MDOF);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL5:" << gsl_strerror(gsl_status);
					return -1;
				}
				gsl_status = gsl_vector_memcpy(Am_tot_MDOF,Ae_tot_MDOF);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL6:" << gsl_strerror(gsl_status);
					return -1;
				}
				gsl_status = gsl_vector_memcpy(Am_MDOF,Aessi_MDOF);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL7:" << gsl_strerror(gsl_status);
					return -1;
				}
				ductility_max=gsl_vector_max(ductility_MDOF);
				ductility_maxNumber=Num-gsl_vector_max_index(ductility_MDOF)+1;
			} 
			else{
				// Nonlinear Analysis of FixedBase Structure: FixedDuc
				nla_status = nonlinearAnalysisMDOF(h_story,e,fy_fix,alpha,kstr,Kfix,Kfix,Cfix,Mfix,Lfix,FIMhfix,FIMrfix,n,dt,Num,Num,FixedDuc_MDOF,&Fixductility_max,&Fixductility_maxNumber,Dm_fix_MDOF,Dm_tot_fix_MDOF,Am_fix_MDOF,Am_tot_fix_MDOF,threadID);
				if (nla_status) {  
					rCritical(threadID) << "Error in NonlinearAnalysis function";
					return -1;
				}

				// Nonlinear Analysis of Flexible Base Structure: ductility
				if (dof==Num+3){
					nla_status = nonlinearAnalysisMDOF(h_story,e,fy,alpha,kstr,Kfix,K,C,M,L,FIMh,FIMr,n,dt,dof,Num,ductility_MDOF,&ductility_max,&ductility_maxNumber,Dm_MDOF,Dm_tot_MDOF,Am_MDOF,Am_tot_MDOF,threadID);
					if (nla_status) {  
						rCritical(threadID) << "Error in NonlinearAnalysis function";
						return -1;
					}
				}
				else
				{
					gsl_status = gsl_vector_memcpy(Dm_MDOF,Dm_fix_MDOF);
					if (gsl_status) {  
						rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
						return -1;
					}
					gsl_status = gsl_vector_memcpy(Dm_tot_MDOF,Dm_tot_fix_MDOF);
					if (gsl_status) {  
						rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
						return -1;
					}
					gsl_status = gsl_vector_memcpy(Am_tot_MDOF,Am_tot_fix_MDOF);
					if (gsl_status) {  
						rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
						return -1;
					}
					gsl_status = gsl_vector_memcpy(Am_MDOF,Am_fix_MDOF);
					if (gsl_status) {  
						rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
						return -1;
					}
					gsl_status = gsl_vector_memcpy(ductility_MDOF,FixedDuc_MDOF);
					if (gsl_status) {  
						rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
						return -1;
					}
					ductility_max=Fixductility_max;
					ductility_maxNumber=Fixductility_maxNumber;

				}
			}
			/*RDomain *theDomain = qobject_cast<RDomain *>(this->parent());
			rDebug(threadID) << "fy = " << endl;
			theDomain->print(fy);
			rDebug(threadID) << "tempfy = " << endl;
			theDomain->print(tempfy);
			rDebug(threadID) << "dist = " << endl;
			theDomain->print(dist);
			rDebug(threadID) << "fy_fix = " << endl;
			theDomain->print(fy_fix);
			rDebug(threadID)<<"VfixE = "<<vfixMDOF;*/
			
			gsl_vector_free(dist);
			//gsl_vector_free(Dy);
			gsl_vector_free(tempfy);
			gsl_vector_free(tempfy_fix);

		} //end if (theRunMode == 1)

	//--------------------------------------

		if (theRunMode == RSoilStructureInteractionModel::GivenDucSameR){ 
			// 2) Given target_ducssi -> Rssi = Rfix
			int R2_status;
			R2_status = RunmodeGivenDuc(K,M,C,Kfix,Mfix,Cfix,L,Lfix,kstr,VeFix,VeSSI,FIMh,FIMr,n,dt,e,alpha,target_ductility,Tn,vs,h_story,dof,Num,fy,ductility_MDOF,&ductility_max,&ductility_maxNumber,&R,Dm_MDOF,Dm_tot_MDOF,Am_MDOF,Am_tot_MDOF,threadID);
			if (R2_status) {  
				rCritical(threadID) << "Error in Runmode2: Given Duc Same R";
				return -1;
			}
			RssiMDOF = R;
			RfixMDOF = R;

		}	//end if (theRunMode ==2)

	//--------------------------------------

		if (theRunMode == RSoilStructureInteractionModel::GivenDucComputeModificationFactor){
			// 3) Given ductility -> Computing modification factor
			int R3_status;
			//---------------------------------------------------------Fixed base system
			R3_status = RunmodeGivenDuc(Kfix,Mfix,Cfix,Kfix,Mfix,Cfix,Lfix,Lfix,kstr,VeFix,VeFix,FIMhfix,FIMrfix,n,dt,e,alpha,target_ductility,Tn,vs,h_story,Num,Num,fy_fix,FixedDuc_MDOF,&Fixductility_max,&Fixductility_maxNumber,&RfixMDOF,Dm_fix_MDOF,Dm_tot_fix_MDOF,Am_fix_MDOF,Am_tot_fix_MDOF,threadID);
			if (R3_status) {  
				rCritical(threadID) << "Error in Runmode3: Modification Factor Fixed-MDOF";
				return -1;
			}
			//Equivalent SDOF
			R3_status = RunmodeGivenDuc(Kfix_eq,Mfix_eq,Cfix_eq,Kfix_eq,Mfix_eq,Cfix_eq,Lfix_eq,Lfix_eq,kstr_eq,VeFix_eq,VeFix_eq,FIMhfix,FIMrfix,n,dt,e_eq,alpha,target_ductility,Tn,vs,h_story_eq,1,1,fy_fix_eq,FixedDuc_eq,&Fixductility_max_eq,&Fixductility_maxNumber_eq,&RfixSDOF,Dm_fix_eq,Dm_tot_fix_eq,Am_fix_eq,Am_tot_fix_eq,threadID);
			if (R3_status) {  
				rCritical(threadID) << "Error in Runmode3: Modification Factor Fixed-SDOFeq";
				return -1;
			}
			//alpha_r = Rs/Rm
			Alpha_RFixed = RfixSDOF/RfixMDOF;
			//alpha_v=Vm/Vs
			Vfixratio = VeFix/VeFix_eq;	//Vm/Vs for elastic
			Alpha_VFixed = gsl_vector_get(fy_fix,Num-1)/gsl_vector_get(fy_fix_eq,0);
			//rDebug(threadID) << "base shear fixMDOF: " << vfixMDOF << endl;
			//rDebug(threadID) << "base shearfix SDOF: " <<  vfixSDOF << endl;

			//Flexible base system
			if (dof==Num+3) {
				R3_status = RunmodeGivenDuc(K,M,C,Kfix,Mfix,Cfix,L,Lfix,kstr,VeFix,VeSSI,FIMh,FIMr,n,dt,e,alpha,target_ductility,Tn,vs,h_story,dof,Num,fy,ductility_MDOF,&ductility_max,&ductility_maxNumber,&RssiMDOF,Dm_MDOF,Dm_tot_MDOF,Am_MDOF,Am_tot_MDOF,threadID);
				if	(R3_status) {  
					rCritical(threadID) << "Error in Runmode3: Modification Factor Flexible-MDOF";
					return -1;
				}
			}
			else{
				gsl_status = gsl_vector_memcpy(Dm_MDOF,Dm_fix_MDOF);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}
				gsl_status = gsl_vector_memcpy(Dm_tot_MDOF,Dm_tot_fix_MDOF);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}
				gsl_status = gsl_vector_memcpy(Am_MDOF,Am_fix_MDOF);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}
				gsl_status = gsl_vector_memcpy(Am_tot_MDOF,Am_tot_fix_MDOF);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}
				gsl_status = gsl_vector_memcpy(ductility_MDOF,FixedDuc_MDOF);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}
				gsl_status = gsl_vector_memcpy(fy,fy_fix);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}
				ductility_max = Fixductility_max;
				ductility_maxNumber = Fixductility_maxNumber;
				RssiMDOF = RfixMDOF;
			}
			//Equivalent SDOF
			if (dofeq==4) {
				R3_status = RunmodeGivenDuc(K_eq,M_eq,C_eq,Kfix_eq,Mfix_eq,Cfix_eq,L_eq,Lfix_eq,kstr_eq,VeFix_eq,VeSSI_eq,FIMh_eq,FIMr_eq,n,dt,e_eq,alpha,target_ductility,Tn,vs,h_story_eq,dofeq,1,fy_eq,ductility_eq,&ductility_max_eq,&ductility_maxNumber_eq,&RssiSDOF,Dm_eq,Dm_tot_eq,Am_eq,Am_tot_eq,threadID);
				if	(R3_status) {  
					rCritical(threadID) << "Error in Runmode3: Modification Factor Flexible-SDOFeq";
					return -1;
				}
			}
			else{
				gsl_status = gsl_vector_memcpy(Dm_eq,Dm_fix_eq);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}
				gsl_status = gsl_vector_memcpy(Dm_tot_eq,Dm_tot_fix_eq);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}
				gsl_status = gsl_vector_memcpy(Am_eq,Am_fix_eq);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}
				gsl_status = gsl_vector_memcpy(Am_tot_eq,Am_tot_fix_eq);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}
				gsl_status = gsl_vector_memcpy(ductility_eq,FixedDuc_eq);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}
				gsl_status = gsl_vector_memcpy(fy_eq,fy_fix_eq);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}
				gsl_status = gsl_vector_memcpy(fy_eq,fy_fix_eq);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}
				ductility_max_eq = Fixductility_max_eq;
				ductility_maxNumber_eq = Fixductility_maxNumber_eq;
				RssiSDOF = RfixSDOF;
			}
			Alpha_RSSI = RssiSDOF/RssiMDOF;

			//alpha_v=Vm/Vs
			Vssiratio = VeSSI/VeSSI_eq;
			Alpha_VSSI = gsl_vector_get(fy,Num-1)/gsl_vector_get(fy_eq,0);
			/*RDomain *theDomain = qobject_cast<RDomain *>(this->parent());
			rDebug(threadID) << "DmSDOF = " << endl;
			theDomain->print(Dm_fix_eq);
			rDebug(threadID) << "DeSDOF = " << endl;
			theDomain->print(Defix_eq);
			rDebug(threadID) << "fy = " << endl;
			theDomain->print(fy);
			rDebug(threadID) << "fy_fix = " << endl;
			theDomain->print(fy_fix);
			rDebug(threadID) << "fy_eq = " << endl;
			theDomain->print(fy_eq);
			rDebug(threadID) << "fy_fix_eq = " << endl;
			theDomain->print(fy_fix_eq);*/
			
		} // end if (theRunMode == 3)

	//-------------------------------------------------

		if (theRunMode == RSoilStructureInteractionModel::GivenDucSameRorFy){
			// 4) Given Ductility -> 1: Run SDOF and find fy then use it for MDOF in fixed base -- 2: Same as 1 but for flexible base -- 3: Run SDOFfix and find base shear then use it for SDOFssi and MDOFssi
			//SDOF fixed base with target ductility
			int R4_status;
			R4_status = RunmodeGivenDuc(Kfix_eq,Mfix_eq,Cfix_eq,Kfix_eq,Mfix_eq,Cfix_eq,Lfix_eq,Lfix_eq,kstr_eq,VeFix_eq,VeSSI_eq,FIMhfix,FIMrfix,n,dt,e_eq,alpha,target_ductility,Tn,vs,h_story_eq,1,1,fy_fix_eq,FixedDuc_eq,&Fixductility_max_eq,&Fixductility_maxNumber_eq,&RfixSDOF,Dm_fix_eq,Dm_tot_fix_eq,Am_fix_eq,Am_tot_fix_eq,threadID);
			if (R4_status) {  
				rCritical(threadID) << "Error in Runmode4: Given Duc SDOFeq fixed base";
				return -1;
			}
			//SDOF flexible base with target ductility
			if (dofeq==4) {
				R4_status = RunmodeGivenDuc(K_eq,M_eq,C_eq,Kfix_eq,Mfix_eq,Cfix_eq,L_eq,Lfix_eq,kstr_eq,VeFix_eq,VeSSI_eq,FIMh_eq,FIMr_eq,n,dt,e_eq,alpha,target_ductility,Tn,vs,h_story_eq,dofeq,1,fy_eq,ductility_eq,&ductility_max_eq,&ductility_maxNumber_eq,&RssiSDOF,Dm_eq,Dm_tot_eq,Am_eq,Am_tot_eq,threadID);
				if	(R4_status) {  
					rCritical(threadID) << "Error in Runmode4: Given Duc SDOFeq flexible base";
					return -1;
				}
			}
			else{
				gsl_status = gsl_vector_memcpy(Dm_eq,Dm_fix_eq);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}
				gsl_status = gsl_vector_memcpy(Dm_tot_eq,Dm_tot_fix_eq);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}
				gsl_status = gsl_vector_memcpy(Am_eq,Am_fix_eq);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}
				gsl_status = gsl_vector_memcpy(Am_tot_eq,Am_tot_fix_eq);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}
				gsl_status = gsl_vector_memcpy(ductility_eq,FixedDuc_eq);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}
				gsl_status = gsl_vector_memcpy(fy_eq,fy_fix_eq);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}
				ductility_max_eq = Fixductility_max_eq;
				ductility_maxNumber_eq = Fixductility_maxNumber_eq;
				RssiSDOF = RfixSDOF;
			}
			//SDOF flexible base with base shear of fixed base
			if (dofeq==4){
				nla_status = nonlinearAnalysisMDOF(h_story_eq,e_eq,fy_fix_eq,alpha,kstr_eq,Kfix_eq,K_eq,C_eq,M_eq,L_eq,FIMh_eq,FIMr_eq,n,dt,dofeq,1,ductility_eq2,&ductility_max_eq2,&ductility_maxNumber_eq2,Dm_eq2,Dm_tot_eq2,Am_eq2,Am_tot_eq2,threadID);
				if (nla_status) {  
					rCritical(threadID) << "Error in NonlinearAnalysis function: SDOFeq flexible base with fixed base shear";
					return -1;
				}
				RssiSDOF2=VeSSI_eq/gsl_vector_get(fy_fix_eq,0);
			}else{
				gsl_status = gsl_vector_memcpy(Dm_eq2,Dm_fix_eq);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}
				gsl_status = gsl_vector_memcpy(Dm_tot_eq2,Dm_tot_fix_eq);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}
				gsl_status = gsl_vector_memcpy(Am_eq2,Am_fix_eq);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}
				gsl_status = gsl_vector_memcpy(Am_tot_eq2,Am_tot_fix_eq);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}
				gsl_status = gsl_vector_memcpy(ductility_eq2,FixedDuc_eq);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}
				ductility_max_eq2 = Fixductility_max_eq;
				ductility_maxNumber_eq2 = Fixductility_maxNumber_eq;
				RssiSDOF2 = RfixSDOF;
			}

			//MDOF
			//Base shear distribution pattern
			int latdist;
			gsl_vector *dist = gsl_vector_calloc(Num);
			latdist = lateralforcedistribution(Tn,Num,dist,threadID);
			if (latdist){
				rCritical(threadID) <<"Error in lateral force distribution function";
				return -1;
			}
			//fy of MDOF systems
			gsl_vector *tempfy = gsl_vector_calloc(Num);	//Use fy of SDOF SSI for MDOF SSI
			gsl_vector *tempfy_fix = gsl_vector_calloc(Num);	//Use fy of SDOF fixed for MDOF fixed and flexible
			for (int i=1;i<=Num;i++){
				gsl_vector_set(tempfy,Num-i,gsl_vector_get(fy_eq,0)*gsl_vector_get(dist,i-1));
				gsl_vector_set(tempfy_fix,Num-i,gsl_vector_get(fy_fix_eq,0)*gsl_vector_get(dist,i-1));
			}
			double sp=0.0;
			double spf=0.0;
			for (int i=1;i<=Num;i++){
				sp=sp+gsl_vector_get(tempfy,i-1);
				gsl_vector_set(fy,i-1,sp);
				spf=spf+gsl_vector_get(tempfy_fix,i-1);
				gsl_vector_set(fy_fix,i-1,spf);
			}
			// Nonlinear Analysis of FixedBase Structure with SDOF fixed base fy
			nla_status = nonlinearAnalysisMDOF(h_story,e,fy_fix,alpha,kstr,Kfix,Kfix,Cfix,Mfix,Lfix,FIMhfix,FIMrfix,n,dt,Num,Num,FixedDuc_MDOF,&Fixductility_max,&Fixductility_maxNumber,Dm_fix_MDOF,Dm_tot_fix_MDOF,Am_fix_MDOF,Am_tot_fix_MDOF,threadID);
			if (nla_status) {  
				rCritical(threadID) << "Error in NonlinearAnalysis function";
				return -1;
			}
			// Nonlinear Analysis of Flexible Base Structure with SDOF flexible base fy
			if (dof==Num+3){
				nla_status = nonlinearAnalysisMDOF(h_story,e,fy,alpha,kstr,Kfix,K,C,M,L,FIMh,FIMr,n,dt,dof,Num,ductility_MDOF,&ductility_max,&ductility_maxNumber,Dm_MDOF,Dm_tot_MDOF,Am_MDOF,Am_tot_MDOF,threadID);
				if (nla_status) {  
					rCritical(threadID) << "Error in NonlinearAnalysis function";
					return -1;
				}
				// Nonlinear Analysis of Flexible Base Structure with SDOF fixed base fy
				nla_status = nonlinearAnalysisMDOF(h_story,e,fy_fix,alpha,kstr,Kfix,K,C,M,L,FIMh,FIMr,n,dt,dof,Num,ductility_MDOF2,&ductility_max2,&ductility_maxNumber2,Dm_MDOF2,Dm_tot_MDOF2,Am_MDOF2,Am_tot_MDOF2,threadID);
				if (nla_status) {  
					rCritical(threadID) << "Error in NonlinearAnalysis function";
					return -1;
				}
			}
			else
			{
				gsl_status = gsl_vector_memcpy(Dm_MDOF,Dm_fix_MDOF);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}
				gsl_status = gsl_vector_memcpy(Dm_tot_MDOF,Dm_tot_fix_MDOF);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}
				gsl_status = gsl_vector_memcpy(Am_tot_MDOF,Am_tot_fix_MDOF);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}
				gsl_status = gsl_vector_memcpy(Am_MDOF,Am_fix_MDOF);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}
				gsl_status = gsl_vector_memcpy(ductility_MDOF,FixedDuc_MDOF);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}
				ductility_max=Fixductility_max;
				ductility_maxNumber=Fixductility_maxNumber;
				gsl_status = gsl_vector_memcpy(Dm_MDOF2,Dm_fix_MDOF);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}
				gsl_status = gsl_vector_memcpy(Dm_tot_MDOF2,Dm_tot_fix_MDOF);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}
				gsl_status = gsl_vector_memcpy(Am_tot_MDOF2,Am_tot_fix_MDOF);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}
				gsl_status = gsl_vector_memcpy(Am_MDOF2,Am_fix_MDOF);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}
				gsl_status = gsl_vector_memcpy(ductility_MDOF2,FixedDuc_MDOF);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}
				ductility_max2=Fixductility_max;
				ductility_maxNumber2=Fixductility_maxNumber;
			}
			gsl_vector_free(dist);
			gsl_vector_free(tempfy);
			gsl_vector_free(tempfy_fix);
			/*RDomain *theDomain = qobject_cast<RDomain *>(this->parent());
			rDebug(threadID) << "Dm2 = " << endl;
			theDomain->print(Dm_MDOF2);
			rDebug(threadID) << "Am2 = " << endl;
			theDomain->print(Am_MDOF2);
			rDebug(threadID) << "ductility_MDOF2 = " << endl;
			theDomain->print(ductility_MDOF2);
			rDebug(threadID) << "ducmax= " << ductility_max2 << endl;
			rDebug(threadID) << "ducmaxNum= " << ductility_maxNumber2 << endl;
			rDebug(threadID) << "RssiSDOF2= " << RssiSDOF2 << endl;
			rDebug(threadID) << "RssiSDOF= " << RssiSDOF << endl;*/

		}	// end if (theRunMode == 4)

	//-------------------------------------------------

		/*if (theRunMode == RSoilStructureInteractionModel::GivenDucSameAlpha){
			// 5) Given DucSSI-> Run SDOFssi with Duc -> Use alphav and then apply the shear to MDOF
			//SDOF flexible base with target ductility
			int R5_status;
			R5_status = RunmodeGivenDuc(Kfix_eq,Mfix_eq,Cfix_eq,Kfix_eq,Mfix_eq,Cfix_eq,Lfix_eq,Lfix_eq,kstr_eq,VeFix_eq, VeFix_eq,FIMh,FIMr,n,dt,e_eq,alpha,target_ductility,Tn,vs,h_story_eq,1,1,fy_fix_eq,FixedDuc_eq,&Fixductility_max_eq,&Fixductility_maxNumber_eq,&RfixSDOF,Dm_fix_eq,Dm_tot_fix_eq,Am_fix_eq,Am_tot_fix_eq,threadID);
			if (R5_status) {  
				rCritical(threadID) << "Error in Runmode4: Given Duc SDOFeq fixed base";
				return -1;
			}
			if (dofeq==4) {
				R5_status = RunmodeGivenDuc(K_eq,M_eq,C_eq,Kfix_eq,Mfix_eq,Cfix_eq,L_eq,Lfix_eq,kstr_eq,VeFix_eq,VeSSI_eq,FIMh_eq,FIMr_eq,n,dt,e_eq,alpha,target_ductility,Tn,vs,h_story_eq,dofeq,1,fy_eq,ductility_eq,&ductility_max_eq,&ductility_maxNumber_eq,&RssiSDOF,Dm_eq,Dm_tot_eq,Am_eq,Am_tot_eq,threadID);
				if	(R5_status) {  
					rCritical(threadID) << "Error in Runmode4: Given Duc SDOFeq flexible base";
					return -1;
				}
			}
			else{
				gsl_status = gsl_vector_memcpy(Dm_eq,Dm_fix_eq);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}
				gsl_status = gsl_vector_memcpy(Dm_tot_eq,Dm_tot_fix_eq);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}
				gsl_status = gsl_vector_memcpy(Am_eq,Am_fix_eq);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}
				gsl_status = gsl_vector_memcpy(Am_tot_eq,Am_tot_fix_eq);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}
				gsl_status = gsl_vector_memcpy(ductility_eq,FixedDuc_eq);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}
				gsl_status = gsl_vector_memcpy(fy_eq,fy_fix_eq);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}
				ductility_max_eq = Fixductility_max_eq;
				ductility_maxNumber_eq = Fixductility_maxNumber_eq;
				RssiSDOF = RfixSDOF;
			}
			//MDOF
			//Base shear distribution pattern
			int latdist;
			gsl_vector *dist = gsl_vector_calloc(Num);
			latdist = lateralforcedistribution(Tn,Num,dist,threadID);
			if (latdist){
				rCritical(threadID) <<"Error in lateral force distribution function";
				return -1;
			}
			//fy of MDOF systems
			gsl_vector *tempfy = gsl_vector_calloc(Num);	//Use fy of SDOF SSI for MDOF SSI
			gsl_vector *tempfy_fix = gsl_vector_calloc(Num);	//Use fy of SDOF fixed for MDOF fixed and flexible
			
			//************************************************************************* Regression Formula
			double Vmfix = gsl_vector_get(fy_fix_eq,0)*((pow(M_E,teta1)*pow(M_E,Num*teta2)*pow(M_E,target_ductility*teta3)*pow(M_E,I*teta4))-1); 
			double Vmssi = gsl_vector_get(fy_eq,0)*((pow(M_E,teta1)*pow(M_E,Num*teta2)*pow(M_E,target_ductility*teta3)*pow(M_E,I*teta4))-1);
			//*************************************************************************

			for (int i=1;i<=Num;i++){
				gsl_vector_set(tempfy,Num-i,Vmssi*gsl_vector_get(dist,i-1));
				gsl_vector_set(tempfy_fix,Num-i,Vmfix*gsl_vector_get(dist,i-1));
			}
			double sp=0.0;
			double spf=0.0;
			for (int i=1;i<=Num;i++){
				sp=sp+gsl_vector_get(tempfy,i-1);
				gsl_vector_set(fy,i-1,sp);
				spf=spf+gsl_vector_get(tempfy_fix,i-1);
				gsl_vector_set(fy_fix,i-1,spf);
			}
			// Nonlinear Analysis of FixedBase Structure with SDOF fixed base fy
			nla_status = nonlinearAnalysisMDOF(h_story,e,fy_fix,alpha,kstr,Kfix,Kfix,Cfix,Mfix,Lfix,FIMhfix,FIMrfix,n,dt,Num,Num,FixedDuc_MDOF,&Fixductility_max,&Fixductility_maxNumber,Dm_fix_MDOF,Dm_tot_fix_MDOF,Am_fix_MDOF,Am_tot_fix_MDOF,threadID);
			if (nla_status) {  
				rCritical(threadID) << "Error in NonlinearAnalysis function";
				return -1;
			}
			// Nonlinear Analysis of Flexible Base Structure with SDOF flexible base fy
			if (dof==Num+3){
				nla_status = nonlinearAnalysisMDOF(h_story,e,fy,alpha,kstr,Kfix,K,C,M,L,FIMh,FIMr,n,dt,dof,Num,ductility_MDOF,&ductility_max,&ductility_maxNumber,Dm_MDOF,Dm_tot_MDOF,Am_MDOF,Am_tot_MDOF,threadID);
				if (nla_status) {  
					rCritical(threadID) << "Error in NonlinearAnalysis function";
					return -1;
				}
			}
			else
			{
				gsl_status = gsl_vector_memcpy(Dm_MDOF,Dm_fix_MDOF);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}
				gsl_status = gsl_vector_memcpy(Dm_tot_MDOF,Dm_tot_fix_MDOF);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}
				gsl_status = gsl_vector_memcpy(Am_tot_MDOF,Am_tot_fix_MDOF);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}
				gsl_status = gsl_vector_memcpy(Am_MDOF,Am_fix_MDOF);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}
				gsl_status = gsl_vector_memcpy(ductility_MDOF,FixedDuc_MDOF);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}
				ductility_max=Fixductility_max;
				ductility_maxNumber=Fixductility_maxNumber;
			}
			gsl_vector_free(dist);
			gsl_vector_free(tempfy);
			gsl_vector_free(tempfy_fix);

		}	// end if (the RunMode == 5)*/
	//-------------------------------------------------
		if (theRunMode == RSoilStructureInteractionModel::GivenRFixSameFy){

			// 6) Given Rfix -> Fyfix=Fyssi -> Ducssi	// Dessi*fbar_yssi = Defix*fbar_yfix = Fy/ks
			//Compute fy for each story
			gsl_vector *tempfy = gsl_vector_calloc(Num);
			gsl_vector *tempfy_fix = gsl_vector_calloc(Num);
			fbar_y = 1/R;
			int latdist;
			gsl_vector *dist = gsl_vector_calloc(Num);
			latdist = lateralforcedistribution(Tn,Num,dist,threadID);
			if (latdist){
				rCritical(threadID) <<"Error in lateral force distribution function";
				return -1;
			}
			for (int i=1;i<=Num;i++){
				gsl_vector_set(tempfy,Num-i,VeFix*fbar_y*gsl_vector_get(dist,i-1));
				gsl_vector_set(tempfy_fix,Num-i,VeFix*fbar_y*gsl_vector_get(dist,i-1));
			}
			double sp=0.0;
			double spf=0.0;
			for (int i=1;i<=Num;i++){
				sp=sp+gsl_vector_get(tempfy,i-1);
				gsl_vector_set(fy,i-1,sp);
				spf=spf+gsl_vector_get(tempfy_fix,i-1);
				gsl_vector_set(fy_fix,i-1,spf);
			}
			/*gsl_vector *Dy = gsl_vector_calloc(Num);
			for (int i=1;i<=Num;i++){
				gsl_vector_set(Dy,i-1,gsl_vector_get(fy,i-1)/gsl_vector_get(kstr,i-1));
			}
			RDomain *theDomain = qobject_cast<RDomain *>(this->parent());
			rDebug(threadID) << "Dy of springs = " << endl;
			theDomain->print(Dy);
			rDebug(threadID) << "fy = " << endl;
			theDomain->print(fy);*/
			
			// Nonlinear Analysis of FixedBase Structure: FixedDuc
			nla_status = nonlinearAnalysisMDOF(h_story,e,fy,alpha,kstr,Kfix,Kfix,Cfix,Mfix,Lfix, FIMhfix,FIMrfix,n,dt,Num,Num,FixedDuc_MDOF, &Fixductility_max,&Fixductility_maxNumber,Dm_fix_MDOF,Dm_tot_fix_MDOF, Am_fix_MDOF, Am_tot_fix_MDOF,threadID);
			if (nla_status) {  
				rCritical(threadID) << "Error in NonlinearAnalysis function";
				return -1;
			}
			RfixMDOF = R;
			// Nonlinear Analysis of Flexible Base Structure: ductility
			if (dof==Num+3){
				nla_status = nonlinearAnalysisMDOF(h_story,e,fy,alpha,kstr,Kfix,K,C,M,L, FIMh,FIMr,n,dt,dof,Num,ductility_MDOF,&ductility_max,&ductility_maxNumber,Dm_MDOF,Dm_tot_MDOF,Am_MDOF, Am_tot_MDOF,threadID);
				if (nla_status) {  
					rCritical(threadID) << "Error in NonlinearAnalysis function";
					return -1;
				}
				RssiMDOF = VeSSI/gsl_vector_get(fy,Num-1);
			}
			else
			{
				gsl_status = gsl_vector_memcpy(Dm_MDOF,Dm_fix_MDOF);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}
				gsl_status = gsl_vector_memcpy(ductility_MDOF,FixedDuc_MDOF);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}
				gsl_status = gsl_vector_memcpy(Dm_tot_MDOF,Dm_tot_fix_MDOF);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}
				gsl_status = gsl_vector_memcpy(Am_tot_MDOF,Am_tot_fix_MDOF);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}
				gsl_status = gsl_vector_memcpy(Am_MDOF,Am_fix_MDOF);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}
				ductility_max = Fixductility_max;
				ductility_maxNumber = Fixductility_maxNumber;
				RssiMDOF = RfixMDOF;
			}

			gsl_vector_free(tempfy);
			gsl_vector_free(tempfy_fix);
			gsl_vector_free(dist);

		} //end if RunMode==6

	//--------------------------------------

		if (theRunMode == RSoilStructureInteractionModel::GivenDucFixSameFy){ 
			// 7)Given target_ducfix -> Fyfix = Fyssi -> Ducssi
			//Analysis of fixed-base: DucFix --> Fyfix
			int R6_status;
			R6_status =  RunmodeGivenDuc(Kfix,Mfix,Cfix,Kfix,Mfix,Cfix,Lfix,Lfix,kstr,VeFix,VeFix,FIMhfix,FIMrfix,n,dt,e,alpha,target_ductility,Tn,vs,h_story,Num,Num,fy_fix,FixedDuc_MDOF,&Fixductility_max,&Fixductility_maxNumber,&RfixMDOF,Dm_fix_MDOF,Dm_tot_fix_MDOF,Am_fix_MDOF,Am_tot_fix_MDOF,threadID);
			if (R6_status) {  
				rCritical(threadID) << "Error in Runmode4: Given Duc MDOF fixed base";
				return -1;
			}
			//Analysis of flexible-base system with Fyfix
			gsl_status = gsl_vector_memcpy(fy,fy_fix); //Same Fy
			if (gsl_status) {  
				rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
				return -1;
			}
			if (dof==Num+3){
				nla_status = nonlinearAnalysisMDOF(h_story,e,fy,alpha,kstr,Kfix,K,C,M,L,FIMh,FIMr,n,dt,dof, Num,ductility_MDOF,&ductility_max,&ductility_maxNumber,Dm_MDOF,Dm_tot_MDOF,Am_MDOF, Am_tot_MDOF,threadID);
				if (nla_status) {  
					rCritical(threadID) << "Error in NonlinearAnalysis function";
					return -1;
				}
				RssiMDOF = VeSSI/gsl_vector_get(fy,Num-1);
			} else {
				gsl_status = gsl_vector_memcpy(Dm_MDOF,Dm_fix_MDOF);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}
				gsl_status = gsl_vector_memcpy(ductility_MDOF,FixedDuc_MDOF);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}
				gsl_status = gsl_vector_memcpy(Dm_tot_MDOF,Dm_tot_fix_MDOF);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}
				gsl_status = gsl_vector_memcpy(Am_tot_MDOF,Am_tot_fix_MDOF);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}
				gsl_status = gsl_vector_memcpy(Am_MDOF,Am_fix_MDOF);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}
				ductility_max = Fixductility_max;
				ductility_maxNumber = Fixductility_maxNumber;
				RssiMDOF = RfixMDOF;
			}

			/*RDomain *theDomain = qobject_cast<RDomain *>(this->parent());
			rDebug(threadID) << "Dy of springs = " << endl;
			theDomain->print(Dy);
			rDebug(threadID) << "fy = " << endl;
			theDomain->print(fy);
			rDebug(threadID) << "R = " <<R<< endl;
			rDebug(threadID) << "Ducmax = " <<ductility_max<< endl;
			rDebug(threadID) << "DucmaxNumber = " <<ductility_maxNumber<< endl;*/

		} // end if (theRunMode == 7)

	//-------------------------------------------------

		if (theRunMode == RSoilStructureInteractionModel::GivenDucSSISameFy){ 
			// 8) Given target_ducssi -> Fyssi = Fyfix -> Rfix
			if (dof==Num) {  
				rCritical(threadID) << "Error in Runmode: Given DucSSI Same Fy: SSI effect should be considered. Please input a proper value for a0";
				return -1;
			} else {
				int R7_status;
				R7_status = RunmodeGivenDuc(K,M,C,Kfix,Mfix,Cfix,L,Lfix,kstr,VeFix,VeSSI,FIMh,FIMr,n,dt,e,alpha,target_ductility,Tn,vs,h_story,dof,Num,fy,ductility_MDOF,&ductility_max,&ductility_maxNumber,&RssiMDOF,Dm_MDOF,Dm_tot_MDOF,Am_MDOF,Am_tot_MDOF,threadID);
				if (R7_status) {  
					rCritical(threadID) << "Error in Runmode7: Given DucSSI Same Fy";
					return -1;
				}
				gsl_status = gsl_vector_memcpy(fy_fix,fy);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}
				RfixMDOF = VeFix/gsl_vector_get(fy_fix,Num-1);
			}
			/*RDomain *theDomain = qobject_cast<RDomain *>(this->parent());
			rDebug(threadID) << "Dy of springs = " << endl;
			theDomain->print(Dy);
			rDebug(threadID) << "fy = " << endl;
			theDomain->print(fy);
			rDebug(threadID) << "R = " <<R<< endl;
			rDebug(threadID) << "Ducmax = " <<ductility_max<< endl;
			rDebug(threadID) << "DucmaxNumber = " <<ductility_maxNumber<< endl;*/

		} // end if (theRunMode == 8)

		//This runmodes are working for SDOF only
		if (theRunMode == RSoilStructureInteractionModel::GivenRReducedFyATC){
			rDebug(threadID) << "This runmode does not work for MDOF systems!";
			return -1;
		}
			
		if (theRunMode == RSoilStructureInteractionModel::GivenRReducedFyASCE_05){
			rDebug(threadID) << "This runmode does not work for MDOF systems!";
			return -1;
		}

		if (theRunMode == RSoilStructureInteractionModel::GivenRReducedFyASCE_10){
			rDebug(threadID) << "This runmode does not work for MDOF systems!";
			return -1;
		}
		if (theRunMode == RSoilStructureInteractionModel::GivenRReducedFyNEHRP){
			rDebug(threadID) << "This runmode does not work for MDOF systems!";
			return -1;
		}
		if (dof==Num){
			gsl_status = gsl_vector_memcpy(Dessi_MDOF,Defix_MDOF);	//Dessi=Defix;
			if (gsl_status) {  
				rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
				return -1;
			}
			gsl_status = gsl_vector_memcpy(De_tot_MDOF,Defix_MDOF);	//De_tot=Defix;
			if (gsl_status) {  
				rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
				return -1;
			}
			gsl_status = gsl_vector_memcpy(Dm_tot_MDOF,Dm_MDOF);	//Dm_tot=Dm;;
			if (gsl_status) {  
				rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
				return -1;
			}

		}
/*		for (int j=1;j<=Num;j++){
			gsl_vector_set(De_Intensity_MDOF,j-1,gsl_vector_get(Dessi_MDOF,j-1)/Intensity);
			gsl_vector_set(Ae_Intensity_MDOF,j-1,gsl_vector_get(Aessi_MDOF,j-1)/(Intensity*9.81));
			gsl_vector_set(De_tot_Intensity_MDOF,j-1,gsl_vector_get(De_tot_MDOF,j-1)/Intensity);
			gsl_vector_set(Ae_tot_Intensity_MDOF,j-1,gsl_vector_get(Ae_tot_MDOF,j-1)/Intensity);
			gsl_vector_set(Dm_Intensity_MDOF,j-1,gsl_vector_get(Dm_MDOF,j-1)/Intensity);
			gsl_vector_set(Am_Intensity_MDOF,j-1,gsl_vector_get(Am_MDOF,j-1)/(Intensity*9.81));
			gsl_vector_set(Dm_tot_Intensity_MDOF,j-1,gsl_vector_get(Dm_tot_MDOF,j-1)/Intensity);
			gsl_vector_set(Am_tot_Intensity_MDOF,j-1,gsl_vector_get(Am_tot_MDOF,j-1)/(Intensity*9.81));
		}//end for j
		gsl_status = gsl_blas_dgemv(CblasNoTrans,1/(R*Mtotal*Intensity),Kfix,Defix_MDOF,0.0,Fy_mIntensity_MDOF);
		if (gsl_status) {  
			rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
			return -1;
		}
		gsl_status = gsl_blas_dgemv(CblasNoTrans,1/(Mtotal*Intensity),Kfix,Dessi_MDOF,0.0,Fo_mIntensity_MDOF);
		if (gsl_status) {  
			rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
			return -1;
		}*/
		/*RDomain *theDomain = qobject_cast<RDomain *>(this->parent());

		rDebug(threadID) << "Acc_inelastic = " << endl;
		theDomain->print(Am_MDOF);
		rDebug(threadID) << "Acc_inelastic_total = " << endl;
		theDomain->print(Am_tot_MDOF);
		rDebug(threadID) << "Disp_inelastic = " << endl;
		theDomain->print(Dm_MDOF);
		rDebug(threadID) << "Disp_inelastic_total = " << endl;
		theDomain->print(Dm_tot_MDOF);
		rDebug(threadID) << "Ductility = " << endl;
		theDomain->print(ductility_MDOF);*/
		
		/*FILE *outDe;
		FILE *outDm;
		FILE *outAe;
		FILE *outAm;
		FILE *outAetot;
		FILE *outAmtot;
		FILE *outDetot;
		FILE *outDmtot;
		FILE *duc;
		outDe = fopen ("De.txt","wt");
		outAe = fopen ("Ae.txt","wt");
		outDm = fopen ("Dm.txt","wt");
		outAm = fopen ("Am.txt","wt");
		outDetot = fopen ("Detotal.txt","wt");
		outDmtot = fopen ("Dmtotal.txt","wt");
		outAmtot = fopen ("Amtotal.txt","wt");
		outAetot = fopen ("Aetotal.txt","wt");
		duc = fopen("Ductility.txt","wt");
		for (int i=1;i<(Num+1);i++){
			fprintf(outDm,"%e\n",gsl_vector_get(Dm_MDOF,i-1));
			fprintf(outAm,"%e\n",gsl_vector_get(Am_MDOF,i-1));
			fprintf(outDmtot,"%e\n",gsl_vector_get(Dm_tot_MDOF,i-1));
			fprintf(outAmtot,"%e\n",gsl_vector_get(Am_tot_MDOF,i-1));
			fprintf(duc,"%e\n",gsl_vector_get(ductility_MDOF,i-1));
			fprintf(outAe,"%e\n",gsl_vector_get(Aessi_MDOF,i-1));
			fprintf(outDe,"%e\n",gsl_vector_get(Dessi_MDOF,i-1));
			fprintf(outDetot,"%e\n",gsl_vector_get(De_tot_MDOF,i-1));
			fprintf(outAetot,"%e\n",gsl_vector_get(Ae_tot_MDOF,i-1));
		}*/
		gsl_vector_free(fy);
		gsl_vector_free(fy_fix);
		gsl_vector_free(fy_eq);
		gsl_vector_free(fy_fix_eq);



	}//end if MDOF
	
	// ------------------------------------------------------------------------------ Nonlinear Analysis for MDOF End	

	// ------------------------------------------------------------------------------ Nonlinear Analysis End	
	
	//	Updating the response current value
	if (theMDOFOrSDOF == RSoilStructureInteractionModel::SDOF){
		//-------------------- SDOF System Responses
		theT_SSIResponse->setCurrentValue(T_SSI, threadID);
		theVhatRatioResponse->setCurrentValue(VhatRatio, threadID);
		theElasticDisplacementResponse->setCurrentValue(Dessi, threadID);
		theElasticAccelarationResponse->setCurrentValue(Aessi, threadID);
		theTotalElasticDisplacementResponse->setCurrentValue(De_tot, threadID);
		theTotalElasticAccelarationResponse->setCurrentValue(Ae_tot, threadID);
		theInelasticDisplacementResponse->setCurrentValue(Dm, threadID);
		theInelasticAccelarationResponse->setCurrentValue(Am, threadID);
		theTotalInelasticDisplacementResponse->setCurrentValue(Dm_tot, threadID);
		theTotalInelasticAccelarationResponse->setCurrentValue(Am_tot, threadID);
		theDuctilityResponse->setCurrentValue(ductility, threadID);
		theFixedStrengthReductionFactorResponse->setCurrentValue(Rfix, threadID);
		theSSIStrengthReductionFactorResponse->setCurrentValue(Rssi, threadID);
		if (theRunMode == RSoilStructureInteractionModel::GivenRReducedFyATC || theRunMode == RSoilStructureInteractionModel::GivenRReducedFyASCE_05 || theRunMode == RSoilStructureInteractionModel::GivenRReducedFyNEHRP){
			theFixedStrengthReductionFactorResponse->setCurrentValue(R, threadID);
		}

		theElasticDisplacement_IntensityResponse->setCurrentValue(De_Intensity, threadID);
		theElasticAccelaration_IntensityResponse->setCurrentValue(Ae_Intensity, threadID);
		theTotalElasticDisplacement_IntensityResponse->setCurrentValue(De_tot_Intensity, threadID);
		theTotalElasticAccelaration_IntensityResponse->setCurrentValue(Ae_tot_Intensity, threadID);
		theInelasticDisplacement_IntensityResponse->setCurrentValue(Dm_Intensity, threadID);
		theInelasticAccelaration_IntensityResponse->setCurrentValue(Am_Intensity, threadID);
		theTotalInelasticDisplacement_IntensityResponse->setCurrentValue(Dm_tot_Intensity, threadID);
		theTotalInelasticAccelaration_IntensityResponse->setCurrentValue(Am_tot_Intensity, threadID);
		theSystemDuctilityResponse->setCurrentValue(SystemDuc, threadID);
		theFixedDuctilityResponse->setCurrentValue(FixedDuc, threadID);
		theFixedDisplacementResponse->setCurrentValue(Dm_fix, threadID);
		theFixedTotalAccelarationResponse->setCurrentValue(Am_tot_fix, threadID);
		theFixedDesignDuctilityResponse->setCurrentValue(Duc_B, threadID);
		theFixedDesignDisplacementResponse->setCurrentValue(Dm_B, threadID);
		theFixedDesignTotalAccelarationResponse->setCurrentValue(Am_tot_B, threadID);
		theSpringYieldForce_mIntensityResponse->setCurrentValue(Fy_mIntensity, threadID);
		theElasticSpringForce_mIntensityResponse->setCurrentValue(Fo_mIntensity, threadID);
	}else{	
		//-------------------- MDOF System Responses
		if (Num>1){
			for (int j=1;j<=Num;j++){
				theductility_MDOFResponseList[j-1]->setCurrentValue(gsl_vector_get(ductility_MDOF,Num-j), threadID);
				theductility_MDOF2ResponseList[j-1]->setCurrentValue(gsl_vector_get(ductility_MDOF2,Num-j), threadID);
				theDessi_MDOFResponseList[j-1]->setCurrentValue(gsl_vector_get(Dessi_MDOF,Num-j), threadID);
				theAessi_MDOFResponseList[j-1]->setCurrentValue(gsl_vector_get(Aessi_MDOF,Num-j), threadID);
				theDe_tot_MDOFResponseList[j-1]->setCurrentValue(gsl_vector_get(De_tot_MDOF,Num-j), threadID);
				theAe_tot_MDOFResponseList[j-1]->setCurrentValue(gsl_vector_get(Ae_tot_MDOF,Num-j), threadID);
				theDm_MDOFResponseList[j-1]->setCurrentValue(gsl_vector_get(Dm_MDOF,Num-j), threadID);
				theAm_MDOFResponseList[j-1]->setCurrentValue(gsl_vector_get(Am_MDOF,Num-j), threadID);
				theDm_tot_MDOFResponseList[j-1]->setCurrentValue(gsl_vector_get(Dm_tot_MDOF,Num-j), threadID);
				theAm_tot_MDOFResponseList[j-1]->setCurrentValue(gsl_vector_get(Am_tot_MDOF,Num-j), threadID);
				theFixedDuctility_MDOFResponseList[j-1]->setCurrentValue(gsl_vector_get(FixedDuc_MDOF,Num-j), threadID);
				theAm_tot_fix_MDOFResponseList[j-1]->setCurrentValue(gsl_vector_get(Am_tot_fix_MDOF,Num-j), threadID);
				theDm_fix_MDOFResponseList[j-1]->setCurrentValue(gsl_vector_get(Dm_fix_MDOF,Num-j), threadID);
			}
			theMaximumDuctilityResponse->setCurrentValue(ductility_max, threadID);
			theMaximumDuctilityStoryResponse->setCurrentValue(ductility_maxNumber, threadID);
			theMaximumDuctilityMDOFfyfixSDOFResponse->setCurrentValue(ductility_max2, threadID);
			theMaximumDuctilityStoryMDOFfyfixSDOFResponse->setCurrentValue(ductility_maxNumber2, threadID);
			theDuctilitySDOFfyfixSDOFResponse->setCurrentValue(ductility_max_eq2, threadID);
			theFixedMaximumDuctilityResponse->setCurrentValue(Fixductility_max, threadID);
			theFixedMaximumDuctilityStoryResponse->setCurrentValue(Fixductility_maxNumber, threadID);
			theFixedStrengthReductionFactorMDOFResponse->setCurrentValue(RfixMDOF, threadID);
			theSSIStrengthReductionFactorMDOFResponse->setCurrentValue(RssiMDOF, threadID);
			theFixedStrengthReductionFactorSDOFResponse->setCurrentValue(RfixSDOF, threadID);
			theSSIStrengthReductionFactorSDOFResponse->setCurrentValue(RssiSDOF, threadID);
			theSSIStrengthReductionFactorSDOFfyfixSDOFResponse->setCurrentValue(RssiSDOF2, threadID);
			theFixedStrengthReductionFactorResponse->setCurrentValue(R, threadID);
			theFixedElasticBaseShearRatioResponse->setCurrentValue(Vfixratio, threadID);
			theSSIElasticBaseShearRatioResponse->setCurrentValue(Vssiratio, threadID);
			theSSIModificationFactorAlpha_RResponse->setCurrentValue(Alpha_RSSI, threadID);
			theFixedModificationFactorAlpha_RResponse->setCurrentValue(Alpha_RFixed, threadID);
			theSSIModificationFactorAlpha_VResponse->setCurrentValue(Alpha_VSSI, threadID);
			theFixedModificationFactorAlpha_VResponse->setCurrentValue(Alpha_VFixed, threadID);

		}else{
			theElasticDisplacementResponse->setCurrentValue(gsl_vector_get(Dessi_MDOF,0), threadID);
			theElasticAccelarationResponse->setCurrentValue(gsl_vector_get(Aessi_MDOF,0), threadID);
			theTotalElasticDisplacementResponse->setCurrentValue(gsl_vector_get(De_tot_MDOF,0), threadID);
			theTotalElasticAccelarationResponse->setCurrentValue(gsl_vector_get(Ae_tot_MDOF,0), threadID);
			theInelasticDisplacementResponse->setCurrentValue(gsl_vector_get(Dm_MDOF,0), threadID);
			theInelasticAccelarationResponse->setCurrentValue(gsl_vector_get(Am_MDOF,0), threadID);
			theTotalInelasticDisplacementResponse->setCurrentValue(gsl_vector_get(Dm_tot_MDOF,0), threadID);
			theTotalInelasticAccelarationResponse->setCurrentValue(gsl_vector_get(Dm_tot_MDOF,0), threadID);
			theDuctilityResponse->setCurrentValue(gsl_vector_get(ductility_MDOF,0), threadID);
			theFixedDisplacementResponse->setCurrentValue(gsl_vector_get(Dm_fix_MDOF,0), threadID);
			theFixedTotalAccelarationResponse->setCurrentValue(gsl_vector_get(Am_tot_fix_MDOF,0), threadID);
			theFixedDuctilityResponse->setCurrentValue(gsl_vector_get(FixedDuc_MDOF,0), threadID);
			theFixedStrengthReductionFactorResponse->setCurrentValue(RfixMDOF, threadID);
			theSSIStrengthReductionFactorResponse->setCurrentValue(RssiMDOF, threadID);
		}
	}
	
	

	// Freeing allocated memory
	gsl_matrix_free(M);
	gsl_matrix_free(K);
	gsl_matrix_free(C);
	gsl_matrix_free(L);
	gsl_matrix_free(Mfix);
	gsl_matrix_free(Kfix);
	gsl_matrix_free(Cfix);
	gsl_matrix_free(Lfix);
	gsl_vector_free(kstr);
	gsl_matrix_free(M_eq);
	gsl_matrix_free(K_eq);
	gsl_matrix_free(C_eq);
	gsl_matrix_free(L_eq);
	gsl_matrix_free(Mfix_eq);
	gsl_matrix_free(Kfix_eq);
	gsl_matrix_free(Cfix_eq);
	gsl_matrix_free(Lfix_eq);
	gsl_vector_free(kstr_eq);
	//Linear Parameters
	gsl_vector_free(Defix_MDOF);
	gsl_vector_free(De_tot_fix_MDOF);
	gsl_vector_free(Dessi_MDOF);
	gsl_vector_free(De_tot_MDOF);
	gsl_vector_free(Aefix_MDOF);
	gsl_vector_free(Ae_tot_fix_MDOF);
	gsl_vector_free(Aessi_MDOF);
	gsl_vector_free(Ae_tot_MDOF);
	gsl_vector_free(Defix_eq);
	gsl_vector_free(De_tot_fix_eq);
	gsl_vector_free(Dessi_eq);
	gsl_vector_free(De_tot_eq);
	gsl_vector_free(Aefix_eq);
	gsl_vector_free(Ae_tot_fix_eq);
	gsl_vector_free(Aessi_eq);
	gsl_vector_free(Ae_tot_eq);
	//Nonlinear Parameters
	gsl_vector_free(Dm_MDOF);         
	gsl_vector_free(Dm_tot_MDOF);   
	gsl_vector_free(Dm_fix_MDOF);
	gsl_vector_free(Dm_tot_fix_MDOF);
	gsl_vector_free(Am_MDOF);           
	gsl_vector_free(Am_tot_MDOF);  
	gsl_vector_free(Am_fix_MDOF);
	gsl_vector_free(Am_tot_fix_MDOF);
	gsl_vector_free(ductility_MDOF);
	gsl_vector_free(Dm_MDOF2);         
	gsl_vector_free(Dm_tot_MDOF2);   
	gsl_vector_free(Am_MDOF2);           
	gsl_vector_free(Am_tot_MDOF2);  
	gsl_vector_free(ductility_MDOF2);
	gsl_vector_free(De_Intensity_MDOF);
	gsl_vector_free(Ae_Intensity_MDOF);
	gsl_vector_free(De_tot_Intensity_MDOF);
	gsl_vector_free(Ae_tot_Intensity_MDOF);
	gsl_vector_free(Dm_Intensity_MDOF);
	gsl_vector_free(Am_Intensity_MDOF);
	gsl_vector_free(Dm_tot_Intensity_MDOF);
	gsl_vector_free(Am_tot_Intensity_MDOF);
	gsl_vector_free(Fy_mIntensity_MDOF);
	gsl_vector_free(Fo_mIntensity_MDOF);
	gsl_vector_free(FixedDuc_MDOF);
	gsl_vector_free(Dm_eq);           
	gsl_vector_free(Dm_tot_eq);   
	gsl_vector_free(Dm_fix_eq);
	gsl_vector_free(Dm_tot_fix_eq);
	gsl_vector_free(Am_eq);           
	gsl_vector_free(Am_tot_eq);  
	gsl_vector_free(Am_fix_eq);
	gsl_vector_free(Am_tot_fix_eq);
	gsl_vector_free(ductility_eq);
	gsl_vector_free(FixedDuc_eq);
	gsl_vector_free(Dm_eq2);           
	gsl_vector_free(Dm_tot_eq2);   
	gsl_vector_free(Am_eq2);           
	gsl_vector_free(Am_tot_eq2);  
	gsl_vector_free(ductility_eq2);

	return 0;
}

// ---------------------------------------- Main Code End

// ---------------------------------------- Stiffness And Damping Matrix For MDOF
int RSoilStructureInteractionModel::stiffnessAndDampingMatrixMDOF(double Tn, double vs, double wn, double h_r0, double mbar, double xi_s, int Num, gsl_vector *kstr, gsl_matrix *Kfix, gsl_matrix *Mfix, gsl_matrix *Cfix, double *h_story, double *htotal, double Mtotal, double *r0, double *roh, double *landam, double *landah, int threadID)
{
	int gsl_status;
	//Stiffness Distribution Method

	/*double deltak;
	double a = theLastStoryStiffnessRatioIntercept->getCurrentValue(threadID);	//stiffness distribution pattern
	double b = theLastStoryStiffnessRatioSlope->getCurrentValue(threadID);
	double nexp = theStiffnessVariationExponent->getCurrentValue(threadID);
	deltak = a+b*Num;
	gsl_vector *kstr = gsl_vector_calloc(Num);	//[k1/k1;k2/k1;...;ki/k1;...]
	for (int i=1;i<=n;i++){ 
		gsl_vector_set(kstr,i-1,1-(1-deltak)*pow(((i-1)/(Num-1)),nexp));
	}
	*/
	gsl_matrix *tempk=gsl_matrix_calloc(Num,Num);
	gsl_vector *dist = gsl_vector_calloc(Num);
	int latdist;
	latdist = lateralforcedistribution(Tn,Num,dist,threadID);
	if (latdist){
		rCritical(threadID) << "Error in lateral force distribution function";
		return -1;
	}
	gsl_vector *tempkstr = gsl_vector_calloc(Num);
	gsl_status = gsl_vector_memcpy(tempkstr,dist);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
		return -1;
	}
	for (int i=1;i<=Num;i++){
		double ss=0.0;
		for (int j=i;j<=Num;j++){
			ss=ss+gsl_vector_get(tempkstr,j-1);
		}
		gsl_vector_set(kstr,Num-i,ss);
	}
	
	gsl_status = gsl_vector_scale(kstr,gsl_vector_get(kstr,Num-1));
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
		return -1;
	}
	//Stiffness distribution pattern
	gsl_matrix *Kbar = gsl_matrix_calloc(Num,Num);
	gsl_matrix_set(Kbar,0,0,gsl_vector_get(kstr,0));
	for (int i=2;i<=Num;i++){
		gsl_matrix_set(Kbar,i-1,i-2,-1*gsl_vector_get(kstr,i-2));
		gsl_matrix_set(Kbar,i-2,i-1,-1*gsl_vector_get(kstr,i-2));
		gsl_matrix_set(Kbar,i-1,i-1,gsl_vector_get(kstr,i-2)+gsl_vector_get(kstr,i-1));
	}

	//Initial Stiffness Matrix of Structure
	gsl_vector *Wn2 = gsl_vector_calloc (Num);
	gsl_matrix *Phi = gsl_matrix_calloc (Num,Num);
	gsl_matrix_set_all(Phi,0.0);
	gsl_vector *Wn2_i = gsl_vector_calloc (Num);
	gsl_matrix *Phi_i = gsl_matrix_calloc (Num,Num);
	double k1=1;
	gsl_status = gsl_matrix_memcpy(Kfix,Kbar);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_matrix_scale(Kfix,k1);
	gsl_eigen_symmv_workspace * ws= gsl_eigen_symmv_alloc (Num);
	gsl_status = gsl_matrix_memcpy(tempk,Kfix);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_eigen_symmv (tempk, Wn2_i,Phi_i,ws);		//mass matrix=m*I which m is mass of a story, so Wn2_i=wn^2*m
	gsl_eigen_symmv_sort (Wn2_i,Phi_i,GSL_EIGEN_SORT_ABS_ASC);
	k1=gsl_pow_2(wn)/gsl_vector_get(Wn2_i,0);
	gsl_matrix_scale(Kfix,k1);
	gsl_status = gsl_matrix_memcpy(tempk,Kfix);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_eigen_symmv (tempk, Wn2,Phi,ws);	//Kfix should be m^-1*kfix but m=I -->In Finding Phi, there is no difference between amounts of m --> "Effect of m.pdf"
	gsl_eigen_symmv_sort (Wn2,Phi,GSL_EIGEN_SORT_ABS_ASC);

	//Effective Height of the Structure + Radius of Foundation
	double Lh=0.0;
	double Lteta=0.0;
	for (int i=1;i<=Num;i++){
		Lh=Lh+gsl_matrix_get(Phi,i-1,0);
		Lteta=Lteta+gsl_matrix_get(Phi,i-1,0)*(Num-i+1)/Num;
	}
	*r0=1.0;
	double hbar=h_r0*(*r0);
	*htotal=hbar/(Lteta/Lh);
	*landah = hbar/(*htotal);
	*h_story=(*htotal)/Num;
	double m_story=(Mtotal)/Num;
	double mup=0.0;
	double mdown=0.0;
	for (int i=1;i<=Num;i++){
		mup=mup+m_story*gsl_matrix_get(Phi,i-1,0);
		mdown=mdown+m_story*pow(gsl_matrix_get(Phi,i-1,0),2);
	}
	double mstr=mup*mup/mdown;
	*landam=mstr/(Mtotal);
	*roh=(Mtotal)/(mbar*(*r0)*(*r0)*(*htotal));
	/*double hbar=Lteta/Lh;
	*htotal=Num*h_story;
	*r0=h_r0/hbar;	//radius of foundation
	*roh=1800;	//roh=1800 kg/m3 for soil
	Mtotal=mbar*(*roh)*(*r0)*(*r0)*(*htotal);		//Mass of total structure
	double m_story=(Mtotal)/Num;*/

	//Mass Matrix
	for (int i=1;i<=Num;i++){
		gsl_matrix_set(Mfix,i-1,i-1,m_story);
	}

	//Finding Final Kfix
	gsl_status = gsl_matrix_memcpy(Kfix,Kbar);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_matrix_memcpy(tempk,Kfix);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_blas_dtrsm(CblasLeft,CblasUpper,CblasNoTrans,CblasNonUnit,1.0,Mfix,tempk); //tempk=inv(m)*Kfix
	gsl_eigen_symmv (tempk, Wn2_i,Phi_i,ws);	
	gsl_eigen_symmv_sort (Wn2_i,Phi_i,GSL_EIGEN_SORT_ABS_ASC);
	k1=wn*wn/gsl_vector_get(Wn2_i,0);
	gsl_matrix_scale(Kfix,k1);
	gsl_vector_scale(kstr,k1);	//kstr=[kn,kn-1,...,k1] --> it's needed for nonlinear analysis

	//Eigen-Value -> Wn^2 , Phi
	gsl_status = gsl_matrix_memcpy(tempk,Kfix);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_blas_dtrsm(CblasLeft,CblasUpper,CblasNoTrans,CblasNonUnit,1.0,Mfix,tempk); //tempk=inv(m)*Kfix
	gsl_eigen_symmv (tempk, Wn2,Phi,ws);
	gsl_eigen_symmv_sort (Wn2,Phi,GSL_EIGEN_SORT_ABS_ASC);

	//SuperPosition of Modal Damping(Chopra 11.4.20)
	gsl_matrix *Mgeneral = gsl_matrix_calloc(Num,Num);
	gsl_matrix *tempm=gsl_matrix_calloc(Num,Num);
	gsl_blas_dgemm (CblasNoTrans,CblasNoTrans,1,Mfix,Phi,0,tempm);
	gsl_blas_dgemm (CblasTrans, CblasNoTrans,1, Phi, tempm,0, Mgeneral);
	gsl_matrix *tempc=gsl_matrix_calloc(Num,Num);
	for (int j=1;j<=Num;j++){
		gsl_matrix_set(tempc,j-1,j-1,xi_s*sqrt(gsl_vector_get(Wn2,j-1))*2*gsl_matrix_get(Mgeneral,j-1,j-1));	//Cn=xi*2*Mn*wn(11.4.16)
	}
	//c = (mΦM−1)C(M−1ΦTm)
	gsl_blas_dgemm(CblasNoTrans,CblasNoTrans,1,Mfix,Phi,0,tempm);
	gsl_blas_dtrsm(CblasRight,CblasUpper,CblasNoTrans,CblasNonUnit,1,Mgeneral,tempm);
	gsl_blas_dgemm(CblasNoTrans,CblasNoTrans,1,tempm,tempc,0,tempk);
	gsl_blas_dtrsm(CblasRight,CblasUpper,CblasNoTrans,CblasNonUnit,1,Mgeneral,tempk);
	gsl_blas_dgemm(CblasNoTrans,CblasTrans,1,tempk,Phi,0,tempm);
	gsl_blas_dgemm(CblasNoTrans,CblasNoTrans,1,tempm,Mfix,0,Cfix);
	
	/*gsl_vector *Period = gsl_vector_calloc(Num);
	for (int j=1;j<=Num;j++){
		gsl_vector_set(Period,j-1,2*M_PI/sqrt(gsl_vector_get(Wn2,j-1)));
	}*/

	/*RDomain *theDomain = qobject_cast<RDomain *>(this->parent());
	rDebug(threadID) << "Mfix = " << endl;
	theDomain->print(Mfix);
	rDebug(threadID) << "Kfix = " << endl;
	theDomain->print(Kfix);
	rDebug(threadID) << "Cfix = " << endl;
	theDomain->print(Cfix);
	rDebug(threadID) << "kstr= " << endl;
	theDomain->print(kstr);
	rDebug(threadID) <<"LandaM = "<< landam << endl;
	rDebug(threadID) <<"LandaH = "<< landah << endl;*/

	// Freeing allocated memory
	gsl_eigen_symmv_free(ws);
	gsl_vector_free(Wn2);
	gsl_vector_free(Wn2_i);
	gsl_matrix_free(Kbar);
	gsl_matrix_free(Phi);
	gsl_matrix_free(Phi_i);
	gsl_matrix_free(tempk);
	gsl_matrix_free(tempm);
	gsl_matrix_free(tempc);
	gsl_matrix_free(Mgeneral);
	gsl_vector_free(tempkstr);
	gsl_vector_free(dist);
	
	return 0;
}

// ---------------------------------------- End of stiffness and damping matrix for MDOF

// ------------------------------------------ Runmode Given Duc
int RSoilStructureInteractionModel::RunmodeGivenDuc(gsl_matrix *K,gsl_matrix *M, gsl_matrix *C, gsl_matrix *Kfix, gsl_matrix *Mfix, gsl_matrix *Cfix, gsl_matrix *L, gsl_matrix *Lfix, gsl_vector *kstr, double VeFix, double VeSSI, QVector<double> FIMh, QVector<double> FIMr, double n, double dt, double e, double alpha, double target_ductility, double Tn, double vs, double h_story, double dof, int Num, gsl_vector *fy,  gsl_vector *ductility_MDOF, double *ductility_max, int *ductility_maxNumber, double *R, gsl_vector *Dm_MDOF, gsl_vector *Dm_tot_MDOF, gsl_vector *Am_MDOF, gsl_vector *Am_tot_MDOF, int threadID)
{
	int gsl_status;
	int nla_status;
	gsl_vector *tempfy = gsl_vector_calloc(Num);
	gsl_vector *dist = gsl_vector_calloc(Num);
	double duc_max;
	int duc_maxNumber;
	int latdist;
	latdist = lateralforcedistribution(Tn,Num,dist, threadID);
	if (latdist){
		rCritical(threadID) <<"Error in lateral force distribution function";
		return -1;
	}

	//NonLinear Analysis of Soil Structure System : Fyssi
	double fbar_y=1.0;
	gsl_vector_set_all(ductility_MDOF,1.0);
	double multiplier=0.85;
	while (gsl_vector_max(ductility_MDOF)<=target_ductility){
		fbar_y=fbar_y*multiplier;
		for (int i=1;i<=Num;i++){
			gsl_vector_set(tempfy,Num-i,VeSSI*fbar_y*gsl_vector_get(dist,i-1));
		}
		double sp=0.0;
		for (int i=1;i<=Num;i++){
			sp=sp+gsl_vector_get(tempfy,i-1);
			gsl_vector_set(fy,i-1,sp);
		}
				
		nla_status = nonlinearAnalysisMDOF(h_story,e,fy,alpha,kstr,Kfix,K,C,M,L,FIMh,FIMr,n,dt,dof,Num,ductility_MDOF,&duc_max,&duc_maxNumber,Dm_MDOF,Dm_tot_MDOF, Am_MDOF, Am_tot_MDOF, threadID);
		if (nla_status) {  
			rCritical(threadID) << "Error in NonlinearAnalysis function";
			return -1;
		}
	}

	//first=min(fbar_y/(0.85^5),1); 
	double first=1.0;
	if ( fbar_y/gsl_pow_5(0.85) < 1){
		first = fbar_y/gsl_pow_5(0.85);
	}
	double last=fbar_y;
	int start;
	QVector<double> fbar;
	for (int i=0; i<=10 ; i++){
		fbar.insert(i, first+i*(last-first)/10);
	}
	QVector<double> mubar(fbar.count());
	mubar.fill(0.0);
	if (first==1){
		mubar[0]=1.0;
		start=1;
	}
	else{
		start=0;
	}
	for (int i=start; i <= fbar.count()-1 ; i++){
		fbar_y = fbar[i];
		for (int i=1;i<=Num;i++){
			gsl_vector_set(tempfy,Num-i,VeSSI*fbar_y*gsl_vector_get(dist,i-1));
		}
		double sp=0.0;
		for (int i=1;i<=Num;i++){
			sp=sp+gsl_vector_get(tempfy,i-1);
			gsl_vector_set(fy,i-1,sp);
		}	
				
		nla_status = nonlinearAnalysisMDOF(h_story,e,fy,alpha,kstr,Kfix,K,C,M,L,FIMh,FIMr,n,dt,dof,Num,ductility_MDOF,&mubar[i],&duc_maxNumber,Dm_MDOF,Dm_tot_MDOF, Am_MDOF, Am_tot_MDOF,threadID);
		if (nla_status) {  
			rCritical(threadID) << "Error in NonlinearAnalysis function";
			return -1;
		}	
	}
	fbar_y=0.0;
	gsl_vector_set_all(fy,0.0);
	double interp;
	int index=0;
	for (int i=0; i <= fbar.count()-2 ; i++ ){
		if ( (target_ductility - mubar[i])*(target_ductility-mubar[i+1]) < 0 ){
			interp = qExp((qLn(fbar[i+1])-qLn(fbar[i]))/(qLn(mubar[i+1])-qLn(mubar[i]))*(qLn(target_ductility)-qLn(mubar[i+1]))+qLn(fbar[i+1]));
			if (interp>fbar_y){
				fbar_y=interp;
				index=i;
			}
		}
	}
	for (int i=1;i<=Num;i++){
		gsl_vector_set(tempfy,Num-i,VeSSI*fbar_y*gsl_vector_get(dist,i-1));
	}
	double sp=0.0;
	for (int i=1;i<=Num;i++){
		sp=sp+gsl_vector_get(tempfy,i-1);
		gsl_vector_set(fy,i-1,sp);
	}	
	nla_status = nonlinearAnalysisMDOF(h_story,e,fy,alpha,kstr,Kfix,K,C,M,L,FIMh,FIMr,n,dt,dof,Num,ductility_MDOF,&duc_max,&duc_maxNumber, Dm_MDOF, Dm_tot_MDOF, Am_MDOF, Am_tot_MDOF,threadID);
	if (nla_status) {  
		rCritical(threadID) << "Error in NonlinearAnalysis function";
		return -1;
	}
	double f1=fbar[index];
	double f2=fbar[index+1];
	double mu1=mubar[index];
	double mu2=mubar[index+1];
	int trial=0;
	while ( (qAbs((duc_max)-target_ductility)>=0.01) && (trial<20) ){
		trial=trial+1;
		fbar_y=qExp((qLn(f2)-qLn(f1))/(qLn(mu2)-qLn(mu1))*(qLn(target_ductility)-qLn(mu2))+qLn(f2));
		for (int i=1;i<=Num;i++){
			gsl_vector_set(tempfy,Num-i,VeSSI*fbar_y*gsl_vector_get(dist,i-1));
		}
		double sp=0.0;
		for (int i=1;i<=Num;i++){
			sp=sp+gsl_vector_get(tempfy,i-1);
			gsl_vector_set(fy,i-1,sp);
		}	

		nla_status = nonlinearAnalysisMDOF(h_story,e,fy,alpha,kstr,Kfix,K,C,M,L,FIMh,FIMr,n,dt,dof,Num,ductility_MDOF,&duc_max,&duc_maxNumber,Dm_MDOF,Dm_tot_MDOF,Am_MDOF, Am_tot_MDOF,threadID);
		if (nla_status) {  
			rCritical(threadID) << "Error in NonlinearAnalysis function";
			return -1;
		}
		mu1=mu2;
		f1=f2;
		mu2=duc_max;
		f2=fbar_y;
	}
	if (trial==20){
		first=fbar[index];
		last=fbar[index+1];
		for (int i=0; i<=fbar.count()-1 ; i++){
			fbar[i] = first + i*(last-first)/10;
		}
		mubar.fill(0.0);
		if (first==1){
			mubar[0]=1.0;
			start=1;
		}
		else{
			start=0;
		}
		for (int i=start; i<=fbar.count()-1 ; i++){
			fbar_y = fbar[i];
			for (int i=1;i<=Num;i++){
				gsl_vector_set(tempfy,Num-i,VeSSI*fbar_y*gsl_vector_get(dist,i-1));
			}
			double sp=0.0;
			for (int i=1;i<=Num;i++){
				sp=sp+gsl_vector_get(tempfy,i-1);
				gsl_vector_set(fy,i-1,sp);
			}
			nla_status = nonlinearAnalysisMDOF(h_story,e,fy,alpha,kstr,Kfix,K,C,M,L,FIMh,FIMr,n,dt,dof,Num,ductility_MDOF, &mubar[i],&duc_maxNumber,Dm_MDOF,Dm_tot_MDOF,Am_MDOF, Am_tot_MDOF,threadID);
			if (nla_status) {  
				rCritical(threadID) << "Error in NonlinearAnalysis function";
				return -1;
			}
		}
		fbar_y = 0.0;
		for (int i=0 ; i<=fbar.count()-2 ; i++){
			if ((target_ductility-mubar[i])*(target_ductility-mubar[i+1])<0){
				interp=qExp((qLn(fbar[i+1])-qLn(fbar[i]))/(qLn(mubar[i+1])-qLn(mubar[i]))*(qLn(target_ductility)-qLn(mubar[i+1]))+qLn(fbar[i+1]));
				if (interp>fbar_y){
					fbar_y=interp;
				}
			}
		}
		if (fbar_y==0.0){
			fbar_y=qExp( ((qLn(fbar[10])-qLn(fbar[0]))/(qLn(mubar[10])-qLn(mubar[0])))*(qLn(target_ductility)-qLn(mubar[10]))+qLn(fbar[10]) );
		}
		for (int i=1;i<=Num;i++){
			gsl_vector_set(tempfy,Num-i,VeSSI*fbar_y*gsl_vector_get(dist,i-1));
		}
		double sp=0.0;
		for (int i=1;i<=Num;i++){
			sp=sp+gsl_vector_get(tempfy,i-1);
			gsl_vector_set(fy,i-1,sp);
		}	
		nla_status = nonlinearAnalysisMDOF(h_story,e,fy,alpha,kstr,Kfix,K,C,M,L,FIMh,FIMr,n,dt,dof,Num,ductility_MDOF,&duc_max,&duc_maxNumber,Dm_MDOF,Dm_tot_MDOF,Am_MDOF, Am_tot_MDOF,threadID);
		if (nla_status) {  
			rCritical(threadID) << "Error in NonlinearAnalysis function";
			return -1;
		}
	} // end if (trial == 20)
	/*RDomain *theDomain = qobject_cast<RDomain *>(this->parent());
	rDebug(threadID) << "fy = " << endl;
	theDomain->print(fy);
	rDebug(threadID) << "tempfy = " << endl;
	theDomain->print(tempfy);
	rDebug(threadID) << "dist = " << endl;
	theDomain->print(dist);
	rDebug(threadID) << "Vel = " <<v<<endl;
	rDebug(threadID) << "fbar_y = " <<fbar_y<<endl;
	rDebug(threadID) << "R = " <<1/fbar_y<<endl;*/
	
	*R=1/fbar_y;
	*ductility_max = duc_max;
	*ductility_maxNumber = duc_maxNumber;

	//if (ductility < 1){
		//ductility=1;
	//}

	//Freeing allocated memory
	gsl_vector_free(tempfy);
	gsl_vector_free(dist);
	return 0;
}
// ---------------------------------------- End of Runmode Given Duc

// ---------------------------------------- Nonlinear Analysis SDOF Function
int RSoilStructureInteractionModel::nonlinearAnalysisSDOF(double he, double fy, double alpha, double ks, gsl_matrix *K, gsl_matrix *C, gsl_matrix *M, gsl_matrix *L, QVector<double> FIMh, QVector<double> FIMr, double n, double dt, double dof , double *ductility, double *Dm_tot, double *Am, double *Am_tot, int threadID)
{
	gsl_vector *Force_i = gsl_vector_calloc(dof);
	gsl_vector *Force_j = gsl_vector_calloc(dof);
	gsl_vector *u_i = gsl_vector_calloc(dof);
	gsl_vector *du_i = gsl_vector_calloc(dof);
	gsl_vector *u_j = gsl_vector_calloc(dof);
	gsl_vector *du_j = gsl_vector_calloc(dof);
	gsl_vector *d2u_j = gsl_vector_calloc(dof);
	gsl_vector *d2u_i = gsl_vector_calloc(dof);
	gsl_vector *disp = gsl_vector_calloc(n);
	gsl_vector *acc = gsl_vector_calloc(n);
	gsl_vector *time = gsl_vector_calloc(n);
	gsl_vector *tempf = gsl_vector_calloc(dof);
	//setting initial conditions
	bool flag;
	int gsl_status;
	int nm_status;
	double fs_i=0.0;
	double fs_j=0.0;
	double Dm=0.0;
	*Dm_tot=0.0;
	*Am=0.0;
	*Am_tot=0.0;

	gsl_matrix_set(K,0,0,ks);	//	K(1,1)=ks;
	double Dy = fy/ks ;
	double fyt=fy;		// Yield force in tension
	double fyc=-fy;		// Yield force in compression
	double key=0;
	bool CriticalFlag = true;
	double dt2=dt;
	double fs_mid=0;
	double t_mid=0;
	double dt_mid=0;
	double FIMh_mid=0;
	double FIMr_mid=0;


	gsl_vector *Force_mid = gsl_vector_calloc(dof);
	gsl_vector *u_mid = gsl_vector_calloc(dof);
	gsl_vector *du_mid = gsl_vector_calloc(dof);
	gsl_vector *d2u_mid = gsl_vector_calloc(dof);
	gsl_vector *Force_ii = gsl_vector_calloc(dof);
	gsl_vector *Force_jj = gsl_vector_calloc(dof);
	gsl_vector *u_ii = gsl_vector_calloc(dof);
	gsl_vector *du_ii = gsl_vector_calloc(dof);
	gsl_vector *d2u_ii = gsl_vector_calloc(dof);
	gsl_vector *d2u_jj = gsl_vector_calloc(dof);
	gsl_vector *Force_div = gsl_vector_calloc(dof);

	// step by step nonlinear analysis
	for(int i=0; i<=n-2 ;i++){	//for i=1:n-1
		gsl_vector_set(time,i,i*dt);
		CriticalFlag=true;
		double t_i=i*dt;
		double t_j=t_i+dt;
		double FIMh_i=FIMh[i];
		double FIMr_i=FIMr[i];
		double FIMh_j=FIMh[i+1];
		double FIMr_j=FIMr[i+1];
		//Force_i=-M*L*[FIMh(i);FIMr(i)];
		gsl_vector *tempv=gsl_vector_alloc(2);
		gsl_vector_set(tempv,0,FIMh[i]);
		gsl_vector_set(tempv,1,FIMr[i]);
		gsl_status = gsl_blas_dgemv(CblasNoTrans, -1.0, L, tempv,0.0,tempf);
		if (gsl_status) {  
			rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
			return -1;
		}
		gsl_status = gsl_blas_dgemv(CblasNoTrans, 1, M, tempf, 0.0, Force_i);
		if (gsl_status) {  
			rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
			return -1;
		}

		//Force_j=-M*L*[FIMh(i+1,1);FIMr(i+1,1)];
		gsl_vector_set(tempv,0,FIMh[i+1]);
		gsl_vector_set(tempv,1,FIMr[i+1]);
		gsl_status = gsl_blas_dgemv(CblasNoTrans, -1.0, L, tempv,0.0,tempf);
		if (gsl_status) {  
			rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
			return -1;
		}
		gsl_status = gsl_blas_dgemv(CblasNoTrans, 1, M, tempf, 0.0, Force_j);
		if (gsl_status) {  
			rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
			return -1;
		}
		gsl_vector_free(tempv);

		while (CriticalFlag==true){
			CriticalFlag=false;     //Determine two occurrence of critical points in one step

			//[u_j,du_j,d2u_j,fs_j]=NewmarkBeta(K,t_j-t_i,C,M,Force_i,Force_j,fs_i,u_i,du_i,dof);
			dt2=t_j-t_i;
			nm_status = newmarkBetaSDOF(K, dt2, C, M, Force_i, Force_j, fs_i, u_i, du_i, dof, u_j, du_j,d2u_i,d2u_j, &fs_j,threadID);
			if (nm_status) {  
				rCritical(threadID) << "Error in NewmarkBeta function";
				return -1;
			}

			//checking if the system becomes nonlinear or not
			if (((fs_j>fyt) && (fs_i<=fyt)) || ((fs_j<fyc) && (fs_i>=fyc))){

				//[t_mid,Force_mid,u_mid,du_mid,d2u_mid,fs_mid,flag]=BinarySearch(fs_i,fs_j,t_i,t_j,Force_i,Force_j,u_i,du_i,fyt,fyc,K,C,M,dof);
				// ---------------------------------------- Binary Search
				//function [t_mid,Force_mid,u_mid,du_mid,fs_mid,flag]=BinarySearch(fs_i,fs_j,t_i,t_j,Force_i,Force_j,u_i,du_i,fyt,fyc,K,C,M,dof)
				// Output Vars
				t_mid = 0.0;
				dt_mid = 0.0;
				fs_mid = 0.0;
				gsl_vector_set_zero(Force_mid);
				gsl_vector_set_zero(u_mid);
				gsl_vector_set_zero(du_mid);
				gsl_vector_set_zero(d2u_mid);

				// Binary Search Internal Variables
				double fs_ii=fs_i;
				double fs_jj=fs_j;
				double t_ii=t_i;
				double t_jj=t_j;
				double FIMh_ii=FIMh_i;
				double FIMr_ii=FIMr_i;
				double FIMh_jj=FIMh_j;
				double FIMr_jj=FIMr_j;
				gsl_status = gsl_vector_memcpy(Force_ii,Force_i);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}
				gsl_status = gsl_vector_memcpy(Force_jj,Force_j);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}
				gsl_status = gsl_vector_memcpy(u_ii,u_i);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}
				gsl_status = gsl_vector_memcpy(du_ii,du_i);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}
				gsl_status = gsl_vector_memcpy(d2u_ii,d2u_i);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}

				gsl_status = gsl_vector_memcpy(d2u_jj,d2u_j);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}
				flag=false;

				if ( (qAbs((fs_ii-fs_jj)/fs_ii)<0.0001) && (flag==false) ){   //dar in halat noghteye mid hamun ii darnazar gerefte mishavad

					t_mid=t_ii;
					FIMh_mid=FIMh_ii;
					FIMr_mid=FIMr_ii;
					gsl_status = gsl_vector_memcpy(Force_mid,Force_ii);
					if (gsl_status) {  
						rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
						return -1;
					}
					gsl_status = gsl_vector_memcpy(u_mid,u_ii);
					if (gsl_status) {  
						rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
						return -1;
					}
					gsl_status = gsl_vector_memcpy(du_mid,du_ii);
					if (gsl_status) {  
						rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
						return -1;
					}
					gsl_status = gsl_vector_memcpy(d2u_mid,d2u_ii);
					if (gsl_status) {  
						rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
						return -1;
					}

					fs_mid=fs_ii;
				}	//if ( (qAbs((fs_ii-fs_jj)/fs_ii)<0.0001) && (flag==false) )

				while ((qAbs((fs_ii-fs_jj)/fs_ii)>=0.0001) && (flag==false)) {  //dar in halat noghteye mid vasate ii va jj dar nazar gerefte mishavad

					t_mid = (t_ii+t_jj)/2;
					dt_mid = t_mid-t_ii;
					FIMh_mid=(FIMh_ii+FIMh_jj)/2;
					FIMr_mid=(FIMr_ii+FIMr_jj)/2;
					// Force_mid=(Force_ii+Force_jj)/2;
					gsl_status = gsl_vector_memcpy(Force_mid,Force_ii);
					if (gsl_status) {  
						rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
						return -1;
					}
					gsl_status = gsl_vector_add(Force_mid,Force_jj);
					if (gsl_status) {  
						rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
						return -1;
					}
					gsl_status = gsl_vector_scale(Force_mid,0.5);
					if (gsl_status) {  
						rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
						return -1;
					}
					if (dt_mid!=0){   //dar in halat ii ba mid be onvane 2 noghte dar nazar gerefte mishavad vali tahlilie anjam mishavad
						// [u_mid,du_mid,d2u_mid,fs_mid]=NewmarkBeta(K,dt_mid,C,M,Force_ii,Force_mid,fs_ii,u_ii,du_ii,dof);
						nm_status = newmarkBetaSDOF(K, dt_mid, C, M, Force_ii, Force_mid, fs_ii, u_ii, du_ii, dof, u_mid, du_mid,d2u_ii, d2u_mid, &fs_mid,threadID);
						if (nm_status) {  
							rCritical(threadID) << "Error in NewmarkBeta function";
							return -1;
						} 
						if (gsl_vector_get(du_ii,0)>=0){
							if (fs_mid>=fyt){
								fs_jj=fs_mid;
								t_jj=t_mid;
								FIMh_jj=FIMh_mid;
								FIMr_jj=FIMr_mid;
								gsl_status = gsl_vector_memcpy(Force_jj,Force_mid);
								if (gsl_status) {  
									rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
									return -1;
								}
							}
							else { // if (fs_mid<fyt)
								fs_ii=fs_mid;
								t_ii=t_mid;
								FIMh_ii=FIMh_mid;
								FIMr_ii=FIMr_mid;
								gsl_status = gsl_vector_memcpy(Force_ii,Force_mid);
								if (gsl_status) {  
									rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
									return -1;
								}
								gsl_status = gsl_vector_memcpy(u_ii,u_mid);
								if (gsl_status) {  
									rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
									return -1;
								}
								gsl_status = gsl_vector_memcpy(du_ii,du_mid);
								if (gsl_status) {  
									rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
									return -1;
								}
								gsl_status = gsl_vector_memcpy(d2u_ii,d2u_mid);
								if (gsl_status) {  
									rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
									return -1;
								}
							}
						}
						else { //if (gsl_vector_get(du_ii,0)<0)
							if (fs_mid<=fyc){
								fs_jj=fs_mid;
								t_jj=t_mid;
								FIMh_jj=FIMh_mid;
								FIMr_jj=FIMr_mid;
								gsl_status = gsl_vector_memcpy(Force_jj,Force_mid);
								if (gsl_status) {  
									rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
									return -1;
								}
							}
							else { //if (fs_mid>fyc){
								fs_ii=fs_mid;
								t_ii=t_mid;
								FIMh_ii=FIMh_mid;
								FIMr_ii=FIMr_mid;
								gsl_status = gsl_vector_memcpy(Force_ii,Force_mid);
								if (gsl_status) {  
									rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
									return -1;
								}
								gsl_status = gsl_vector_memcpy(u_ii,u_mid);
								if (gsl_status) {  
									rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
									return -1;
								}
								gsl_status = gsl_vector_memcpy(du_ii,du_mid);
								if (gsl_status) {  
									rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
									return -1;
								}
								gsl_status = gsl_vector_memcpy(d2u_ii,d2u_mid);
								if (gsl_status) {  
									rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
									return -1;
								}
							}
						}

					}  //end for if (dt_mid!=0)
					if ((qAbs(t_jj-t_ii)<=1.0/gsl_pow_int(10,14)) || (dt_mid==0)){ //in this condition the system does not become nonlinear
						flag=true;
						t_mid=t_j;
						FIMh_mid=FIMh_j;
						FIMr_mid=FIMr_j;
					}
					else {
						t_mid=t_ii;
						FIMh_mid=FIMh_ii;
						FIMr_mid=FIMr_ii;
						gsl_status = gsl_vector_memcpy(Force_mid,Force_ii);
						if (gsl_status) {  
							rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
							return -1;
						}
						gsl_status = gsl_vector_memcpy(u_mid,u_ii);
						if (gsl_status) {  
							rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
							return -1;
						}
						gsl_status = gsl_vector_memcpy(du_mid,du_ii);
						if (gsl_status) {  
							rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
							return -1;
						}
						gsl_status = gsl_vector_memcpy(d2u_mid,d2u_ii);
						if (gsl_status) {  
							rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
							return -1;
						}


						fs_mid=fs_ii;
					}
				}  ///end for while ((qAbs((fs_ii-fs_jj)/fs_ii)>=0.0001) && (flag==false))

				// ---------------------------------------- Binary Search End

				if (qAbs(gsl_vector_get(u_mid,0))>Dm){
					Dm=qAbs(gsl_vector_get(u_mid,0));
				}
				if (dof==4){
					double u_mid1 = gsl_vector_get(u_mid,0);
					double u_mid2 = gsl_vector_get(u_mid,1);
					double u_mid3 = gsl_vector_get(u_mid,2);
					if (qAbs(u_mid1+u_mid2+he*u_mid3)>*Dm_tot){
						*Dm_tot=qAbs(u_mid1+u_mid2+he*u_mid3);
					}
				}
				if (qAbs(gsl_vector_get(d2u_mid,0))>*Am){
					*Am=qAbs(gsl_vector_get(d2u_mid,0));
				}
				if (dof==1){
					double d2u_mid1 = gsl_vector_get(d2u_mid,0);
					double d2u_gh = FIMh_mid;
					double d2u_gr = FIMr_mid;
					if (qAbs(d2u_mid1+d2u_gh+he*d2u_gr)>*Am_tot){
						*Am_tot=qAbs(d2u_mid1+d2u_gh+he*d2u_gr);
					}
				}

				if (dof==4){
					double d2u_mid1 = gsl_vector_get(d2u_mid,0);
					double d2u_mid2 = gsl_vector_get(d2u_mid,1);
					double d2u_mid3 = gsl_vector_get(d2u_mid,2);
					double d2u_gh = FIMh_mid;
					double d2u_gr = FIMr_mid;
					if (qAbs(d2u_mid1+d2u_mid2+he*d2u_mid3+d2u_gh+he*d2u_gr)>*Am_tot){
						*Am_tot=qAbs(d2u_mid1+d2u_mid2+he*d2u_mid3+d2u_gh+he*d2u_gr);
					}
				}
				if (flag==false){
					if ((fs_j>fyt) && (fs_i<=fyt)){      //point 1
						fs_mid=fyt;
					}
					else if ((fs_j<fyc) && (fs_i>=fyc)){  //point 3
						fs_mid=fyc;
					}
				}
				//set initial condition for remaining calculation of the step
				t_i=t_mid;
				fs_i=fs_mid;
				FIMh_i=FIMh_mid;
				FIMr_i=FIMr_mid;

				gsl_status = gsl_vector_memcpy(u_i,u_mid);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}

				gsl_status = gsl_vector_memcpy(du_i,du_mid);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}

				gsl_status = gsl_vector_memcpy(Force_i,Force_mid);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}
			} // end if (((fs_j>fyt) && (fs_i<=fyt)) || ((fs_j<fyc) && (fs_i>=fyc))) 

			//checking if the system becomes linear or not
			else if (((key==1) && (gsl_vector_get(du_j,0)<0)) || ((key==-1) && (gsl_vector_get(du_j,0)>0))){
				flag=false;     //Determine if the system becomes linear in the step or not
				double divide=50.0;
				double div=0.0;
				gsl_status = gsl_vector_memcpy(Force_ii,Force_i);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}
				while ((flag==false) && (div<divide)){
					div=div+1;
					double FIMh_div=FIMh_i+(FIMh_j-FIMh_i)*div/divide;
					double FIMr_div=FIMr_i+(FIMr_j-FIMr_i)*div/divide;
					FIMh_mid=FIMh_div;
					FIMr_mid=FIMr_div;
					//double t_div = t_i + (t_j-t_i)*div/divide;

					//Force_div=Force_i+(Force_j-Force_i)*div/divide;
					gsl_status = gsl_vector_memcpy(Force_div,Force_j);
					if (gsl_status) {  
						rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
						return -1;
					}
					gsl_status = gsl_vector_sub(Force_div,Force_i);
					if (gsl_status) {  
						rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
						return -1;
					}
					gsl_status = gsl_vector_scale(Force_div,div/divide);
					if (gsl_status) {  
						rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
						return -1;
					}
					gsl_status = gsl_vector_add(Force_div,Force_i);
					if (gsl_status) {  
						rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
						return -1;
					}

					//[u_mid,du_mid,d2u_mid,d2u_i,fs_mid]=NewmarkBeta(K,(t_j-t_i)/divide,C,M,Force_ii,Force_div,fs_i,u_i,du_i,dof);
					dt2=(t_j-t_i)/divide;
					nm_status = newmarkBetaSDOF(K, dt2, C, M, Force_ii, Force_div, fs_i, u_i, du_i, dof, u_mid, du_mid,d2u_i,d2u_mid, &fs_mid, threadID);
					if (nm_status) {  
						rCritical(threadID) << "Error in NewmarkBeta function";
						return -1;
					}

					if ((key==1) && (gsl_vector_get(du_mid,0)<0)){          //point 2
						CriticalFlag=true;
						fyt=fs_i;
						fyc=fyt-2*fy;
						flag=true;
						fs_mid=fs_i;
						FIMh_mid=FIMh_i;
						FIMr_mid=FIMr_i;
						gsl_status = gsl_vector_memcpy(u_mid,u_i);
						if (gsl_status) {  
							rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
							return -1;
						}
						gsl_status = gsl_vector_memcpy(du_mid,du_i);
						if (gsl_status) {  
							rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
							return -1;
						}
						gsl_status = gsl_vector_memcpy(d2u_mid,d2u_i);
						if (gsl_status) {  
							rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
							return -1;
						}
						gsl_status = gsl_vector_memcpy(Force_div,Force_ii);
						if (gsl_status) {  
							rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
							return -1;
						}
					}
					else if ((key==-1) && (gsl_vector_get(du_mid,0)>0)){     //point 4
						CriticalFlag=true;
						fyc=fs_i;
						fyt=fyc+2*fy;
						flag=true;
						fs_mid=fs_i;
						FIMh_mid=FIMh_i;
						FIMr_mid=FIMr_i;
						gsl_status = gsl_vector_memcpy(u_mid,u_i);
						if (gsl_status) {  
							rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
							return -1;
						}
						gsl_status = gsl_vector_memcpy(du_mid,du_i);
						if (gsl_status) {  
							rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
							return -1;
						}
						gsl_status = gsl_vector_memcpy(d2u_mid,d2u_i);
						if (gsl_status) {  
							rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
							return -1;
						}
						gsl_status = gsl_vector_memcpy(Force_div,Force_ii);
						if (gsl_status) {  
							rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
							return -1;
						}
					}
					fs_i=fs_mid;
					FIMh_i=FIMh_mid;
					FIMr_i=FIMr_mid;
					gsl_status = gsl_vector_memcpy(u_i,u_mid);
					if (gsl_status) {  
						rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
						return -1;
					}
					gsl_status = gsl_vector_memcpy(du_i,du_mid);
					if (gsl_status) {  
						rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
						return -1;
					}
					gsl_status = gsl_vector_memcpy(Force_ii,Force_div);
					if (gsl_status) {  
						rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
						return -1;
					}

					if (qAbs(gsl_vector_get(u_mid,0))>Dm){
						Dm=qAbs(gsl_vector_get(u_mid,0));
					}
					if (qAbs(gsl_vector_get(d2u_mid,0))>*Am){
						*Am=qAbs(gsl_vector_get(d2u_mid,0));
					}
					//total
					if (dof==1){
						double d2u_mid1 = gsl_vector_get(d2u_mid,0);
						double d2u_gh = FIMh_mid;
						double d2u_gr = FIMr_mid;
						if (qAbs(d2u_mid1+d2u_gh+he*d2u_gr)>*Am_tot){
							*Am_tot=qAbs(d2u_mid1+d2u_gh+he*d2u_gr);
						}
					}
					if (dof==4){
						double u_mid1 = gsl_vector_get(u_mid,0);
						double u_mid2 = gsl_vector_get(u_mid,1);
						double u_mid3 = gsl_vector_get(u_mid,2);
						if (qAbs(u_mid1+u_mid2+he*u_mid3)>*Dm_tot){
							*Dm_tot=qAbs(u_mid1+u_mid2+he*u_mid3);
						}
						double d2u_mid1 = gsl_vector_get(d2u_mid,0);
						double d2u_mid2 = gsl_vector_get(d2u_mid,1);
						double d2u_mid3 = gsl_vector_get(d2u_mid,2);
						double d2u_gh = FIMh_mid;
						double d2u_gr = FIMr_mid;
						if (qAbs(d2u_mid1+d2u_mid2+he*d2u_mid3+d2u_gh+he*d2u_gr)>*Am_tot){
							*Am_tot=qAbs(d2u_mid1+d2u_mid2+he*d2u_mid3+d2u_gh+he*d2u_gr);
						}
					}
				}
				gsl_status = gsl_vector_memcpy(Force_i,Force_div);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}

				if (flag==true){
					t_i=t_i+(t_j-t_i)*(div-1)/divide;
				}
				if ((div==divide) && (flag==false)){
					t_i=t_j;
				}
				fs_i=fs_mid;
				FIMh_i=FIMh_mid;
				FIMr_i=FIMr_mid;
				gsl_status = gsl_vector_memcpy(u_i,u_mid);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}
				gsl_status = gsl_vector_memcpy(du_i,du_mid);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
					return -1;
				}

			} // end else if (((key==1) && (gsl_vector_get(du_j,0)<0)) || ((key==-1) && (gsl_vector_get(du_j,0)>0)))

			if (((fs_i<fyt) && (gsl_vector_get(du_i,0)>=0)) || (flag==true)){
				key=0;
				gsl_matrix_set(K,0,0,ks);
			}
			else if ((fs_i>=fyt) && (gsl_vector_get(du_i,0)>0)){
				key=1;
				gsl_matrix_set(K,0,0,alpha*ks);
				fs_i=fyt;
			}
			else if (((fs_i>fyc) && (gsl_vector_get(du_i,0)<=0)) || (flag==true)){
				key=0;
				gsl_matrix_set(K,0,0,ks);
			}
			else if ((fs_i<=fyc) && (gsl_vector_get(du_i,0)<0)){
				key=-1;
				gsl_matrix_set(K,0,0,alpha*ks);
				fs_i=fyc;
			}
		} // end while (CriticalFlag)

		//the system remains linear in the step or one of the critical points
		//occur in the first part of the step
		if (t_i!=t_j){
			//[u_j,du_j,d2u_j,d2u_i,fs_j]=NewmarkBeta(K,t_j-t_i,C,M,Force_i,Force_j,fs_i,u_i,du_i,dof);
			dt2=(t_j-t_i);
			nm_status = newmarkBetaSDOF(K, dt2, C, M, Force_i, Force_j, fs_i, u_i, du_i, dof, u_j, du_j,d2u_i,d2u_j, &fs_j,threadID);
			if (nm_status) {  
				rCritical(threadID) << "Error in NewmarkBeta function";
				return -1;
			}

			if (qAbs(gsl_vector_get(u_j,0))>Dm){
				Dm=qAbs(gsl_vector_get(u_j,0));
			}
			if (qAbs(gsl_vector_get(d2u_j,0))>*Am){
				*Am=qAbs(gsl_vector_get(d2u_j,0));
			}
			//total
			if (dof==1){
				double d2u_j1 = gsl_vector_get(d2u_j,0);
				double d2u_gh = FIMh[i+1];
				double d2u_gr = FIMr[i+1];
				if (qAbs(d2u_j1+d2u_gh+he*d2u_gr)>*Am_tot){
					*Am_tot=qAbs(d2u_j1+d2u_gh+he*d2u_gr);
				}
			}
			if (dof==4){
				double u_j1 = gsl_vector_get(u_j,0);
				double u_j2 = gsl_vector_get(u_j,1);
				double u_j3 = gsl_vector_get(u_j,2);

				if (qAbs(u_j1+u_j2+he*u_j3)>*Dm_tot){
					*Dm_tot=qAbs(u_j1+u_j2+he*u_j3);
				}
				double d2u_j1 = gsl_vector_get(d2u_j,0);
				double d2u_j2 = gsl_vector_get(d2u_j,1);
				double d2u_j3 = gsl_vector_get(d2u_j,2);
				double d2u_gh = FIMh[i+1];
				double d2u_gr = FIMr[i+1];				
				if (qAbs(d2u_j1+d2u_j2+he*d2u_j3+d2u_gh+he*d2u_gr)>*Am_tot){
					*Am_tot=qAbs(d2u_j1+d2u_j2+he*d2u_j3+d2u_gh+he*d2u_gr);
				}
			}

			fs_i=fs_j;
			gsl_status = gsl_vector_memcpy(u_i,u_j);
			if (gsl_status) {  
				rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
				return -1;
			}
			gsl_status = gsl_vector_memcpy(du_i,du_j);
			if (gsl_status) {  
				rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
				return -1;
			}
		}
		//gsl_vector_set(disp,i+1,gsl_vector_get(u_i,0));
		//gsl_vector_set(acc,i+1,gsl_vector_get(d2u_i,0));
	} // end for i

	*ductility = Dm/Dy;
	/*FILE *outacc;
	FILE *outdisp;
	FILE *outtime;
	outacc = fopen ("AccelerationSDOFNONLinear.txt","wt");
	outdisp = fopen ("DisplacementSDOFNONLinear.txt","wt");
	outtime= fopen ("TimeSDOFNONLinear.txt","wt");
	for (int i=1;i<=n;i++){
		fprintf(outacc,"%g\n",gsl_vector_get(acc,i-1));
		fprintf(outdisp,"%g\n",gsl_vector_get(disp,i-1));
		fprintf(outtime,"%g\n",gsl_vector_get(time,i-1));
	}*/
	// Freeing allocated memory ( Nonlinear Analysis )
	gsl_vector_free(Force_i);
	gsl_vector_free(Force_j);
	gsl_vector_free(Force_mid);
	gsl_vector_free(Force_div);
	gsl_vector_free(Force_ii);
	gsl_vector_free(Force_jj);
	gsl_vector_free(u_i);
	gsl_vector_free(du_i);
	gsl_vector_free(d2u_i);
	gsl_vector_free(u_j);
	gsl_vector_free(du_j);
	gsl_vector_free(d2u_j);
	gsl_vector_free(u_mid);
	gsl_vector_free(du_mid);
	gsl_vector_free(d2u_mid);
	gsl_vector_free(u_ii);
	gsl_vector_free(du_ii);
	gsl_vector_free(d2u_ii);
	gsl_vector_free(d2u_jj);
	gsl_vector_free(tempf);

	return 0;

}
// ---------------------------------------- Nonlinear Analysis SDOF End

// ---------------------------------------- Nonlinear Analysis MDOF Function
int RSoilStructureInteractionModel::nonlinearAnalysisMDOF(double h_story,double e,gsl_vector *fy, double alpha, gsl_vector* kstr, gsl_matrix *Kfix, gsl_matrix *K, gsl_matrix *C, gsl_matrix *M, gsl_matrix *L, QVector<double> FIMh, QVector<double> FIMr, double n, double dt, double dof , int Num, gsl_vector *ductility, double *ductility_max, int *ductility_maxNumber, gsl_vector *Dm, gsl_vector *Dm_tot, gsl_vector *Am, gsl_vector *Am_tot, int threadID)
{
	gsl_matrix *K_cp = gsl_matrix_calloc(dof,dof);
	gsl_vector *Force_i = gsl_vector_calloc(dof);
	gsl_vector *Force_j = gsl_vector_calloc(dof);
	gsl_vector *u_i = gsl_vector_calloc(dof);
	gsl_vector *du_i = gsl_vector_calloc(dof);
	gsl_vector *u_j = gsl_vector_calloc(dof);
	gsl_vector *du_j = gsl_vector_calloc(dof);
	gsl_vector *d2u_j = gsl_vector_calloc(dof);
	gsl_vector *d2u_i = gsl_vector_calloc(dof);
	gsl_vector *u_ii = gsl_vector_calloc(dof);
	gsl_vector *du_ii = gsl_vector_calloc(dof);
	gsl_vector *d2u_ii = gsl_vector_calloc(dof);
	gsl_vector *fs_ii = gsl_vector_calloc(Num);
	gsl_vector *tempfsii = gsl_vector_calloc(Num);
	gsl_vector *Force_ii = gsl_vector_calloc(dof);
	gsl_vector *Force_mid = gsl_vector_calloc(dof);
	gsl_vector *fs_mid =  gsl_vector_calloc(Num);
	gsl_vector *tempfsmid = gsl_vector_calloc(Num);
	gsl_vector *u_mid = gsl_vector_calloc(dof);
	gsl_vector *du_mid = gsl_vector_calloc(dof);
	gsl_vector *d2u_mid = gsl_vector_calloc(dof);
	gsl_vector *ddm_i = gsl_vector_calloc(Num);
	gsl_vector *ddm_ii = gsl_vector_calloc(Num);
	gsl_vector *ddm_mid = gsl_vector_calloc(Num);
	gsl_vector *ddm_j = gsl_vector_calloc(Num);
	gsl_vector *duc_i = gsl_vector_calloc(Num);
	gsl_vector *duc_ii = gsl_vector_calloc(Num);
	gsl_vector *duc_j = gsl_vector_calloc(Num);
	gsl_vector *duc_mid = gsl_vector_calloc(Num);
	//gsl_matrix *disp = gsl_matrix_calloc(n,Num);
	//gsl_matrix *acc = gsl_matrix_calloc(n,Num);
	//gsl_vector *time = gsl_vector_calloc(n);
	gsl_vector_set_all(Dm,0.0);
	gsl_vector_set_all(Dm_tot,0.0);
	gsl_vector_set_all(Am,0.0);
	gsl_vector_set_all(Am_tot,0.0);
	gsl_vector_set_all(ductility,0.0);
	*ductility_max=0.0;
	*ductility_maxNumber=0;

	//setting initial conditions
	int gsl_status;
	int nm_status;
	int bs_status;
	bool flag;
	bool flag2;
	gsl_vector *fs_i = gsl_vector_calloc(Num);
	gsl_vector *fs_j = gsl_vector_calloc(Num);
	gsl_vector *tempfsi = gsl_vector_calloc(Num);
	gsl_vector *tempfsj = gsl_vector_calloc(Num);
	gsl_vector *tempf = gsl_vector_calloc(Num);
	gsl_vector *tempv = gsl_vector_calloc(2);
	gsl_vector *tempF = gsl_vector_calloc(dof);
	gsl_vector *kstr_copy = gsl_vector_calloc(Num);
	gsl_status = gsl_vector_memcpy(kstr_copy,kstr);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL30:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_matrix_memcpy(K_cp,K);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL31:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_vector *Dy = gsl_vector_calloc(Num);		//Dy=fy/k
	for (int i=1;i<=Num;i++){
		gsl_vector_set(Dy,i-1,gsl_vector_get(fy,i-1)/gsl_vector_get(kstr,i-1));
	}
	gsl_status = gsl_vector_memcpy(tempf,fy);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL25:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_vector *fyt = gsl_vector_calloc(Num);
	gsl_status = gsl_vector_memcpy(fyt,tempf);		// Yield force in tension: fyt=fy
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL25:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_vector *fyc = gsl_vector_calloc(Num);		
	gsl_status = gsl_vector_scale(tempf,-1);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL26:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_vector_memcpy(fyc,tempf);	// Yield force in compression: fyc=-fyt
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL27:" << gsl_strerror(gsl_status);
		return -1;
	}
	
	gsl_vector *key = gsl_vector_calloc(Num);
	double dt2=dt;
	int cut=100;
	//gsl_matrix *force = gsl_matrix_calloc(n,dof);
	//gsl_vector *V = gsl_vector_calloc(n);
	// step by step nonlinear analysis
	for(int i=0; i<=n-2 ;i++){	//for i=1:n-1
		flag=true;
		flag2=true;
		gsl_vector_set_all(Force_mid,0.0);
		gsl_vector_set_all(fs_mid,0.0);
		gsl_vector_set_all(tempfsmid,0.0);
		gsl_vector_set_all(u_mid,0.0);
		gsl_vector_set_all(du_mid,0.0);
		gsl_vector_set_all(d2u_mid,0.0);
		gsl_vector_set_all(ddm_mid,0.0);
		gsl_vector_set_all(duc_mid,0.0);
		double FIMh_mid=0.0;
		double FIMr_mid=0.0;
		double t_i=i*dt;
		double t_j=t_i+dt;
		double FIMh_i=FIMh[i];
		double FIMr_i=FIMr[i];
		double FIMh_j=FIMh[i+1];
		double FIMr_j=FIMr[i+1];
		//Force_i=-M*L*[FIMh(i);FIMr(i)];
		gsl_vector_set(tempv,0,FIMh[i]);
		gsl_vector_set(tempv,1,FIMr[i]);
		gsl_status = gsl_blas_dgemv(CblasNoTrans, -1.0, L, tempv,0.0,tempF);
		if (gsl_status) {  
			rCritical(threadID) << "Error in GSL1:" << gsl_strerror(gsl_status);
			return -1;
		}
		gsl_status = gsl_blas_dgemv(CblasNoTrans, 1, M, tempF, 0.0, Force_i);
		if (gsl_status) {  
			rCritical(threadID) << "Error in GSL2:" << gsl_strerror(gsl_status);
			return -1;
		}

		//Force_j=-M*L*[FIMh(i+1,1);FIMr(i+1,1)];
		gsl_vector_set(tempv,0,FIMh[i+1]);
		gsl_vector_set(tempv,1,FIMr[i+1]);
		gsl_status = gsl_blas_dgemv(CblasNoTrans, -1.0, L, tempv,0.0,tempF);
		if (gsl_status) {  
			rCritical(threadID) << "Error in GSL3:" << gsl_strerror(gsl_status);
			return -1;
		}
		gsl_status = gsl_blas_dgemv(CblasNoTrans, 1, M, tempF, 0.0, Force_j);
		if (gsl_status) {  
			rCritical(threadID) << "Error in GSL4:" << gsl_strerror(gsl_status);
			return -1;
		}
		
		// ------------------------------------------------ Cutting
		//[u_j,du_j,d2u_j,fs_j]=NewmarkBetaNonlinear(kstr, K,t_j-t_i,C,M,Force_i,Force_j,fs_i,u_i,du_i,dof);
		dt2=t_j-t_i;
		nm_status = newmarkBetaMDOFNonlinear(kstr, K_cp, dt2, C, M, fyc, fyt, Force_i, Force_j, fs_i, tempfsi, u_i, du_i, Num, dof, u_j, du_j,d2u_i,d2u_j, fs_j, tempfsj,threadID);
		if (nm_status) {  
			rCritical(threadID) << "Error in NewmarkBetaMDOFNonlinear function";
			return -1;
		}
		for (int j=1;j<=Num;j++){
			if (j==Num){
				gsl_vector_set(ddm_i,Num-1,qAbs(gsl_vector_get(u_i,Num-1)));
			}
			else{
				gsl_vector_set(ddm_i,j-1,qAbs(gsl_vector_get(u_i,j-1)-gsl_vector_get(u_i,j)));
			}
			gsl_vector_set(duc_i,j-1,gsl_vector_get(ddm_i,j-1)/gsl_vector_get(Dy,j-1));
		}
		for (int j=1;j<=Num;j++){
			if (qAbs(gsl_vector_get(u_i,j-1))>gsl_vector_get(Dm,j-1)){
				gsl_vector_set(Dm,j-1,qAbs(gsl_vector_get(u_i,j-1)));
			}
			if (qAbs(gsl_vector_get(d2u_i,j-1))>(gsl_vector_get(Am,j-1))){
				gsl_vector_set(Am,j-1,qAbs(gsl_vector_get(d2u_i,j-1)));
			}
			if ((gsl_vector_get(duc_i,j-1))>gsl_vector_get(ductility,j-1)){
				gsl_vector_set(ductility,j-1,gsl_vector_get(duc_i,j-1));
			}
		}	//End of for j
/*		//Base shear
		double v=0.0;
		for (int j=1;j<=Num;j++){
			v=v+gsl_vector_get(fs_i,j-1);
		}
		if ((qAbs(v))>(qAbs(*Vm))){
			*Vm = v;
		}*/
		//total
		if (dof==Num){	//Fixed base
			for (int j=1;j<=Num;j++){
				if (qAbs(gsl_vector_get(d2u_i,j-1)+FIMh_i+((Num-j+1)*h_story+e)*FIMr_i)>gsl_vector_get(Am_tot,j-1)){
					gsl_vector_set(Am_tot,j-1,qAbs(gsl_vector_get(d2u_i,j-1)+FIMh_i+((Num-j+1)*h_story+e)*FIMr_i));
				}
			}
		} else{	//Flexible base
			for (int j=1;j<=Num;j++){
				double u_j1 = gsl_vector_get(u_i,j-1); //u structure
				double u_j2 = gsl_vector_get(u_i,dof-3); //uf
				double u_j3 = gsl_vector_get(u_i,dof-2); //tetaf
				if (qAbs(u_j1+u_j2+((Num-j+1)*h_story+e)*u_j3)>gsl_vector_get(Dm_tot,j-1)){
					gsl_vector_set(Dm_tot,j-1,qAbs(u_j1+u_j2+((Num-j+1)*h_story+e)*u_j3));
				}
				double d2u_j1 = gsl_vector_get(d2u_i,j-1); //d2u structure
				double d2u_j2 = gsl_vector_get(d2u_i,dof-3); //d2uf
				double d2u_j3 = gsl_vector_get(d2u_i,dof-2); //d2tetaf				
				if (qAbs(d2u_j1+d2u_j2+((Num-j+1)*h_story+e)*d2u_j3+FIMh_i+((Num-j+1)*h_story+e)*FIMr_i)>gsl_vector_get(Am_tot,j-1)){
					gsl_vector_set(Am_tot,j-1,qAbs(d2u_j1+d2u_j2+((Num-j+1)*h_story+e)*d2u_j3+FIMh_i+((Num-j+1)*h_story+e)*FIMr_i));
				}

			}	//end for j
		}	//End of if dof==Num
		for (int j=1;j<=Num;j++){
			if (((gsl_vector_get(tempfsj,j-1)>=gsl_vector_get(fyt,j-1)) && (gsl_vector_get(tempfsi,j-1)<gsl_vector_get(fyt,j-1))) || ((gsl_vector_get(tempfsj,j-1)<=gsl_vector_get(fyc,j-1)) && (gsl_vector_get(tempfsi,j-1)>gsl_vector_get(fyc,j-1)))){
   				flag=false;		//checking if the system becomes nonlinear or not
			}
			if (((gsl_vector_get(key,j-1)==1) && (gsl_vector_get(du_j,j-1)<0)) || ((gsl_vector_get(key,j-1)==-1) && (gsl_vector_get(du_j,j-1)>0))){
				flag2=false;	//checking if the system becomes linear or not
			}
		}
		gsl_status = gsl_vector_memcpy(Force_ii,Force_i);
		if (gsl_status) {  
			rCritical(threadID) << "Error in GSL5:" << gsl_strerror(gsl_status);
			return -1;
		}
		gsl_status = gsl_vector_memcpy(u_ii,u_i);
		if (gsl_status) {  
			rCritical(threadID) << "Error in GSL6:" << gsl_strerror(gsl_status);
			return -1;
		}
		gsl_status = gsl_vector_memcpy(du_ii,du_i);
		if (gsl_status) {  
			rCritical(threadID) << "Error in GSL7:" << gsl_strerror(gsl_status);
			return -1;
		}
		gsl_status = gsl_vector_memcpy(d2u_ii,d2u_i);
		if (gsl_status) {  
			rCritical(threadID) << "Error in GSL8:" << gsl_strerror(gsl_status);
			return -1;
		}
		gsl_status = gsl_vector_memcpy(fs_ii,fs_i);
		if (gsl_status) {  
			rCritical(threadID) << "Error in GSL9:" << gsl_strerror(gsl_status);
			return -1;
		}
		gsl_status = gsl_vector_memcpy(tempfsii,tempfsi);
		if (gsl_status) {  
			rCritical(threadID) << "Error in GSL9:" << gsl_strerror(gsl_status);
			return -1;
		}

		if (flag==false || flag2==false){
			dt2=dt2/cut;
			for (int ii=1;ii<=cut;ii++){
				//Line equation for Force_mid	
				FIMh_mid=FIMh_i+((FIMh_j-FIMh_i)/(t_j-t_i))*(ii*dt2);
				FIMr_mid=FIMr_i+((FIMr_j-FIMr_i)/(t_j-t_i))*(ii*dt2);
				gsl_vector_set(tempv,0,FIMh_mid);
				gsl_vector_set(tempv,1,FIMr_mid);
				gsl_status = gsl_blas_dgemv(CblasNoTrans, -1.0, L, tempv,0.0,tempF);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL1:" << gsl_strerror(gsl_status);
					return -1;
				}
				gsl_status = gsl_blas_dgemv(CblasNoTrans, 1, M, tempF, 0.0, Force_mid);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL2:" << gsl_strerror(gsl_status);
					return -1;
				}
				//[u_j,du_j,d2u_j,fs_j]=NewmarkBetaNonlinear(K,t_j-t_i,C,M,Force_i,Force_j,fs_i,u_i,du_i,dof);
				nm_status = newmarkBetaMDOFNonlinear(kstr, K_cp,dt2,C,M, fyc,fyt,Force_ii,Force_mid,fs_ii,tempfsii,u_ii,du_ii,Num,dof,u_mid,du_mid,d2u_ii,d2u_mid,fs_mid,tempfsmid,threadID);
				if (nm_status) {
					rCritical(threadID) << "Error in NewmarkBetaMDOFNonlinear function";
					return -1;
				}
				// Checking events
				for (int j=1;j<=Num;j++){
					if (((gsl_vector_get(tempfsmid,j-1)>=gsl_vector_get(fyt,j-1)) && gsl_vector_get(du_mid,j-1)>=0)||((gsl_vector_get(tempfsii,j-1)>=gsl_vector_get(fyt,j-1)) && (gsl_vector_get(du_ii,j-1)>=0) && (gsl_vector_get(du_mid,j-1)>=0))){
						double kk=gsl_vector_get(kstr,j-1);
						gsl_vector_set(kstr_copy,j-1,kk*alpha);
						gsl_vector_set(key,j-1,1);
					}else if (((gsl_vector_get(tempfsmid,j-1)<=gsl_vector_get(fyc,j-1)) && gsl_vector_get(du_mid,j-1)<=0)||((gsl_vector_get(tempfsii,j-1)<=gsl_vector_get(fyc,j-1)) && (gsl_vector_get(du_ii,j-1)<=0) && (gsl_vector_get(du_mid,j-1)<=0))){
						double kk=gsl_vector_get(kstr,j-1);
						gsl_vector_set(kstr_copy,j-1,kk*alpha);
						gsl_vector_set(key,j-1,-1);
					}else if (((gsl_vector_get(key,j-1)==1) && (gsl_vector_get(du_mid,j-1)<0))){
						gsl_vector_set(kstr_copy,j-1,gsl_vector_get(kstr,j-1));
						gsl_vector_set(key,j-1,0);
					}else if ((gsl_vector_get(key,j-1)==-1) && (gsl_vector_get(du_mid,j-1)>0)){
						gsl_vector_set(kstr_copy,j-1,gsl_vector_get(kstr,j-1));
						gsl_vector_set(key,j-1,0);
					}else if ((gsl_vector_get(tempfsmid,j-1)<gsl_vector_get(fyt,j-1) && gsl_vector_get(du_mid,j-1)>=0)){
						gsl_vector_set(kstr_copy,j-1,gsl_vector_get(kstr,j-1));
						gsl_vector_set(key,j-1,0);
					}else if ((gsl_vector_get(tempfsmid,j-1)>gsl_vector_get(fyc,j-1) && gsl_vector_get(du_mid,j-1)<=0)){
						gsl_vector_set(kstr_copy,j-1,gsl_vector_get(kstr,j-1));
						gsl_vector_set(key,j-1,0);
					}
				}
				for (int j=1;j<=Num;j++){
					if (j==Num){
						gsl_vector_set(ddm_mid,Num-1,qAbs(gsl_vector_get(u_mid,Num-1)));
					} else
					{
						gsl_vector_set(ddm_mid,j-1,qAbs(gsl_vector_get(u_mid,j-1)-gsl_vector_get(u_mid,j)));
					}
					gsl_vector_set(duc_mid,j-1,gsl_vector_get(ddm_mid,j-1)/gsl_vector_get(Dy,j-1));
				}
				//reset stiffness matrix
				gsl_status = gsl_matrix_memcpy(K_cp,K);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL10:" << gsl_strerror(gsl_status);
					return -1;
				}
				gsl_matrix_set(K_cp,0,0,gsl_vector_get(kstr_copy,0));
				for (int j=2;j<=Num;j++){
					gsl_matrix_set(K_cp,j-1,j-2,-1*gsl_vector_get(kstr_copy,j-2));
					gsl_matrix_set(K_cp,j-2,j-1,-1*gsl_vector_get(kstr_copy,j-2));
					gsl_matrix_set(K_cp,j-1,j-1,gsl_vector_get(kstr_copy,j-2)+gsl_vector_get(kstr_copy,j-1));
				}

				for (int j=1;j<=Num;j++){
					if (gsl_vector_get(kstr_copy,j-1)==0) {
						for (int jj=1;jj<=Num;jj++){
							gsl_matrix_set(K_cp,j-1,jj-1,0);
						}
					}

				}
				//Initial conditions for rest of calculation
				gsl_status = gsl_vector_memcpy(u_ii,u_mid);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL10:" << gsl_strerror(gsl_status);
					return -1;
				}
				gsl_status = gsl_vector_memcpy(du_ii,du_mid);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL11:" << gsl_strerror(gsl_status);
					return -1;
				}
				gsl_status = gsl_vector_memcpy(d2u_ii,d2u_mid);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL12:" << gsl_strerror(gsl_status);
					return -1;
				}
				gsl_status = gsl_vector_memcpy(fs_ii,fs_mid);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL13:" << gsl_strerror(gsl_status);
					return -1;
				}
				gsl_status = gsl_vector_memcpy(tempfsii,tempfsmid);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL13:" << gsl_strerror(gsl_status);
					return -1;
				}
				gsl_status = gsl_vector_memcpy(Force_ii,Force_mid);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL14:" << gsl_strerror(gsl_status);
					return -1;
				}
				gsl_status = gsl_vector_memcpy(ddm_ii,ddm_mid);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL14:" << gsl_strerror(gsl_status);
					return -1;
				}
				gsl_status = gsl_vector_memcpy(duc_ii,duc_mid);
				if (gsl_status) {  
					rCritical(threadID) << "Error in GSL14:" << gsl_strerror(gsl_status);
					return -1;
				}
				for (int j=1;j<=Num;j++){
					if (qAbs(gsl_vector_get(u_ii,j-1))>gsl_vector_get(Dm,j-1)){
						gsl_vector_set(Dm,j-1,qAbs(gsl_vector_get(u_ii,j-1)));
					}
					if (qAbs(gsl_vector_get(d2u_ii,j-1))>(gsl_vector_get(Am,j-1))){
						gsl_vector_set(Am,j-1,qAbs(gsl_vector_get(d2u_ii,j-1)));
					}
					if ((gsl_vector_get(duc_ii,j-1))>gsl_vector_get(ductility,j-1)){
						gsl_vector_set(ductility,j-1,gsl_vector_get(duc_ii,j-1));
					}
				}	//End of for j
/*				double v=0.0;
				for (int j=1;j<=Num;j++){
					v=v+gsl_vector_get(fs_ii,j-1);
				}
				if ((qAbs(v))>(qAbs(*Vm))){
					*Vm = v;
				}*/
				//total
				if (dof==Num){	//Fixed base
					for (int j=1;j<=Num;j++){
						if (qAbs(gsl_vector_get(d2u_ii,j-1)+FIMh_mid+((Num-j+1)*h_story+e)*FIMr_mid)>gsl_vector_get(Am_tot,j-1)){
							gsl_vector_set(Am_tot,j-1,qAbs(gsl_vector_get(d2u_ii,j-1)+FIMh_mid+((Num-j+1)*h_story+e)*FIMr_mid));
						}
					}
				} else{	//Flexible base
					for (int j=1;j<=Num;j++){
						double u_j1 = gsl_vector_get(u_ii,j-1); //u structure
						double u_j2 = gsl_vector_get(u_ii,dof-3); //uf
						double u_j3 = gsl_vector_get(u_ii,dof-2); //tetaf
						if (qAbs(u_j1+u_j2+((Num-j+1)*h_story+e)*u_j3)>gsl_vector_get(Dm_tot,j-1)){
							gsl_vector_set(Dm_tot,j-1,qAbs(u_j1+u_j2+((Num-j+1)*h_story+e)*u_j3));
						}
						double d2u_j1 = gsl_vector_get(d2u_ii,j-1); //d2u structure
						double d2u_j2 = gsl_vector_get(d2u_ii,dof-3); //d2uf
						double d2u_j3 = gsl_vector_get(d2u_ii,dof-2); //d2tetaf				
						if (qAbs(d2u_j1+d2u_j2+((Num-j+1)*h_story+e)*d2u_j3+FIMh_mid+((Num-j+1)*h_story+e)*FIMr_mid)>gsl_vector_get(Am_tot,j-1)){
							gsl_vector_set(Am_tot,j-1,qAbs(d2u_j1+d2u_j2+((Num-j+1)*h_story+e)*d2u_j3+FIMh_mid+((Num-j+1)*h_story+e)*FIMr_mid));
						}
					}	//end for j

				}	//End of if dof==Num
			}	//end for ii
			//initila condition for rest of calculations
			gsl_status = gsl_vector_memcpy(u_j,u_ii);
			if (gsl_status) {  
				rCritical(threadID) << "Error in GSL15:" << gsl_strerror(gsl_status);
				return -1;
			}
			gsl_status = gsl_vector_memcpy(du_j,du_ii);
			if (gsl_status) {  
				rCritical(threadID) << "Error in GSL16:" << gsl_strerror(gsl_status);
				return -1;
			}
			gsl_status = gsl_vector_memcpy(d2u_j,d2u_ii);
			if (gsl_status) {  
				rCritical(threadID) << "Error in GSL17:" << gsl_strerror(gsl_status);
				return -1;
			}
			gsl_status = gsl_vector_memcpy(fs_j,fs_ii);
			if (gsl_status) {  
				rCritical(threadID) << "Error in GSL18:" << gsl_strerror(gsl_status);
				return -1;
			}
			gsl_status = gsl_vector_memcpy(tempfsj,tempfsii);
			if (gsl_status) {  
				rCritical(threadID) << "Error in GSL18:" << gsl_strerror(gsl_status);
				return -1;
			}
		}	//end if flag==false || flag2==false

		for (int j=1;j<=Num;j++){
			gsl_vector_set(ddm_j,j-1,qAbs(gsl_vector_get(u_j,j-1)-gsl_vector_get(u_j,j)));
			if (j==Num){
				gsl_vector_set(ddm_j,Num-1,qAbs(gsl_vector_get(u_j,Num-1)));
			}
			gsl_vector_set(duc_j,j-1,gsl_vector_get(ddm_j,j-1)/gsl_vector_get(Dy,j-1));
		}

		for (int j=1;j<=Num;j++){
			if (qAbs(gsl_vector_get(u_j,j-1))>gsl_vector_get(Dm,j-1)){
				gsl_vector_set(Dm,j-1,qAbs(gsl_vector_get(u_j,j-1)));
			}
			if (qAbs(gsl_vector_get(d2u_j,j-1))>(gsl_vector_get(Am,j-1))){
				gsl_vector_set(Am,j-1,qAbs(gsl_vector_get(d2u_j,j-1)));
			}
			if ((gsl_vector_get(duc_j,j-1))>gsl_vector_get(ductility,j-1)){
				gsl_vector_set(ductility,j-1,gsl_vector_get(duc_j,j-1));
			}
		}	//End of for j
/*		v=0.0;
		for (int j=1;j<=Num;j++){
			v=v+gsl_vector_get(fs_j,j-1);
		}
		if ((qAbs(v))>(qAbs(*Vm))){
			*Vm = v;
		}*/
		//total
		if (dof==Num){	//Fixed base
			for (int j=1;j<=Num;j++){
				if (qAbs(gsl_vector_get(d2u_j,j-1)+FIMh_j+((Num-j+1)*h_story+e)*FIMr_j)>gsl_vector_get(Am_tot,j-1)){
					gsl_vector_set(Am_tot,j-1,qAbs(gsl_vector_get(d2u_j,j-1)+FIMh_j+((Num-j+1)*h_story+e)*FIMr_j));
				}
			}
		} else{	//Flexible base
			for (int j=1;j<=Num;j++){
				double u_j1 = gsl_vector_get(u_j,j-1); //u structure
				double u_j2 = gsl_vector_get(u_j,dof-3); //uf
				double u_j3 = gsl_vector_get(u_j,dof-2); //tetaf
				if (qAbs(u_j1+u_j2+((Num-j+1)*h_story+e)*u_j3)>gsl_vector_get(Dm_tot,j-1)){
					gsl_vector_set(Dm_tot,j-1,qAbs(u_j1+u_j2+((Num-j+1)*h_story+e)*u_j3));
				}
				double d2u_j1 = gsl_vector_get(d2u_j,j-1); //d2u structure
				double d2u_j2 = gsl_vector_get(d2u_j,dof-3); //d2uf
				double d2u_j3 = gsl_vector_get(d2u_j,dof-2); //d2tetaf				
				if (qAbs(d2u_j1+d2u_j2+((Num-j+1)*h_story+e)*d2u_j3+FIMh_j+((Num-j+1)*h_story+e)*FIMr_j)>gsl_vector_get(Am_tot,j-1)){
					gsl_vector_set(Am_tot,j-1,qAbs(d2u_j1+d2u_j2+((Num-j+1)*h_story+e)*d2u_j3+FIMh_j+((Num-j+1)*h_story+e)*FIMr_j));
				}
			}	//end for j
		}	//End of if dof==Num
		
		gsl_status = gsl_vector_memcpy(u_i,u_j);
		if (gsl_status) {  
			rCritical(threadID) << "Error in GSL19:" << gsl_strerror(gsl_status);
			return -1;
		}
		gsl_status = gsl_vector_memcpy(du_i,du_j);
		if (gsl_status) {  
			rCritical(threadID) << "Error in GSL20:" << gsl_strerror(gsl_status);
			return -1;
		}
		gsl_status = gsl_vector_memcpy(d2u_i,d2u_j);
		if (gsl_status) {  
			rCritical(threadID) << "Error in GSL21:" << gsl_strerror(gsl_status);
			return -1;
		}
		gsl_status = gsl_vector_memcpy(fs_i,fs_j);
		if (gsl_status) {  
			rCritical(threadID) << "Error in GSL22:" << gsl_strerror(gsl_status);
			return -1;
		}
		gsl_status = gsl_vector_memcpy(tempfsi,tempfsj);
		if (gsl_status) {  
			rCritical(threadID) << "Error in GSL22:" << gsl_strerror(gsl_status);
			return -1;
		}
		gsl_status = gsl_vector_memcpy(ddm_i,ddm_j);
		if (gsl_status) {  
			rCritical(threadID) << "Error in GSL22:" << gsl_strerror(gsl_status);
			return -1;
		}
		gsl_status = gsl_vector_memcpy(duc_i,duc_j);
		if (gsl_status) {  
			rCritical(threadID) << "Error in GSL22:" << gsl_strerror(gsl_status);
			return -1;
		}
		for (int j=1;j<=Num;j++){
			if (((gsl_vector_get(tempfsi,j-1)<gsl_vector_get(fyt,j-1)) && (gsl_vector_get(du_i,j-1)>=0))  ||((gsl_vector_get(tempfsi,j-1)>=gsl_vector_get(fyt,j-1)) && (gsl_vector_get(du_i,j-1)<0))){
				gsl_vector_set(key,j-1,0);
				gsl_vector_set(kstr_copy,j-1,gsl_vector_get(kstr,j-1));
			}
			else if ((gsl_vector_get(tempfsi,j-1)>=gsl_vector_get(fyt,j-1)) && (gsl_vector_get(du_i,j-1)>=0)){
				gsl_vector_set(key,j-1,1);
				double kk=gsl_vector_get(kstr,j-1);
				gsl_vector_set(kstr_copy,j-1,kk*alpha);
			}
			else if (((gsl_vector_get(tempfsi,j-1)>gsl_vector_get(fyc,j-1)) && (gsl_vector_get(du_i,j-1)<=0)) || (gsl_vector_get(tempfsi,j-1)<=gsl_vector_get(fyc,j-1)) && (gsl_vector_get(du_i,j-1)>0)){
				gsl_vector_set(key,j-1,0);
				gsl_vector_set(kstr_copy,j-1,gsl_vector_get(kstr,j-1));

			}
			else if ((gsl_vector_get(tempfsi,j-1)<=gsl_vector_get(fyc,j-1)) && (gsl_vector_get(du_i,j-1)<=0)){
				gsl_vector_set(key,j-1,-1);
				double kk=gsl_vector_get(kstr,j-1);
				gsl_vector_set(kstr_copy,j-1,kk*alpha);
			}
		}
		//reset stiffness matrix
		gsl_status = gsl_matrix_memcpy(K_cp,K);
		if (gsl_status) {  
			rCritical(threadID) << "Error in GSL10:" << gsl_strerror(gsl_status);
			return -1;
		}
		gsl_matrix_set(K_cp,0,0,gsl_vector_get(kstr_copy,0));
		for (int j=2;j<=Num;j++){
			gsl_matrix_set(K_cp,j-1,j-2,-1*gsl_vector_get(kstr_copy,j-2));
			gsl_matrix_set(K_cp,j-2,j-1,-1*gsl_vector_get(kstr_copy,j-2));
			gsl_matrix_set(K_cp,j-1,j-1,gsl_vector_get(kstr_copy,j-2)+gsl_vector_get(kstr_copy,j-1));
		}

		for (int j=1;j<=Num;j++){
			if (gsl_vector_get(kstr_copy,j-1)==0) {
				for (int jj=1;jj<=Num;jj++){
					gsl_matrix_set(K_cp,j-1,jj-1,0);
				}
			}

		}
	// ----------------------------------------------- End of Cutting
		/*gsl_vector_set(time,i+1,(i+1)*dt);
		for (int j=1;j<=Num;j++){
			gsl_matrix_set(disp,i+1,j-1,gsl_vector_get(u_j,j-1));
		}
		gsl_matrix_set(force,i+1,0,gsl_vector_get(fs_j,0));
		for (int j=2;j<=dof;j++){
			gsl_matrix_set(force,i+1,j-1,gsl_vector_get(fs_j,j-1)-gsl_vector_get(fs_j,j-2));
		}
		gsl_vector_set(V,i+1,gsl_vector_get(fs_j,Num-1));*/
	} // end for i

	*ductility_max=gsl_vector_max(ductility);
	*ductility_maxNumber=Num-gsl_vector_max_index(ductility);
	//rDebug(threadID) <<"Ducmax"<<gsl_vector_max(ductility)<<endl;
	//rDebug(threadID) <<"DucmaxNum"<<Num-gsl_vector_max_index(ductility)<<endl;

	//Print time-history of responses
/*
	if (Num==1){	//SDOF
		FILE *outdisp1;
		//FILE *outdisp2;
		FILE *outtime;
		//FILE *outforce1;
		//FILE *outforce2;
		FILE *outshear;
	//	outacc = fopen ("AccelerationMDOFNonLinear.txt","wt");
		outdisp1 = fopen ("SDisplacementMDOFNonLinearN1.txt","wt");
		outtime = fopen ("STimeMDOFNonLinear.txt","wt");
		//outforce1 = fopen ("SForceMDOFNonLinearN1.txt","wt");
		outshear = fopen ("SBaseShearMDOFNonLinear.txt","wt");
		for (int i=1;i<=n;i++){
		//	fprintf(outacc,"%g\n",gsl_matrix_get(acc,i-1,0));
			fprintf(outdisp1,"%g\n",gsl_matrix_get(disp,i-1,0));
			fprintf(outtime,"%g\n",gsl_vector_get(time,i-1));
			//fprintf(outforce1,"%g\n",gsl_matrix_get(force,i-1,0));
			fprintf(outshear,"%g\n",gsl_vector_get(V,i-1));
		}
		fclose(outtime);
		//fclose(outforce2);
		//fclose(outforce1);
		fclose(outshear);
		fclose(outdisp1);
		//fclose(outdisp2);
	}else{	//MDOF,N=2
		FILE *outdisp1;
		FILE *outdisp2;
		FILE *outtime;
		FILE *outforce1;
		FILE *outforce2;
		FILE *outshear;
	//	outacc = fopen ("AccelerationMDOFNonLinear.txt","wt");
		outdisp1 = fopen ("MDisplacementMDOFNonLinearN1.txt","wt");
		outdisp2 = fopen ("MDisplacementMDOFNonLinearN2.txt","wt");
		outtime = fopen ("MTimeMDOFNonLinear.txt","wt");
		outforce2 = fopen ("MForceMDOFNonLinearN2.txt","wt");
		outforce1 = fopen ("MForceMDOFNonLinearN1.txt","wt");
		outshear = fopen ("MBaseShearMDOFNonLinear.txt","wt");
		for (int i=1;i<=n;i++){
		//	fprintf(outacc,"%g\n",gsl_matrix_get(acc,i-1,0));
			fprintf(outdisp2,"%g\n",gsl_matrix_get(disp,i-1,0));
			fprintf(outdisp1,"%g\n",gsl_matrix_get(disp,i-1,1));
			fprintf(outtime,"%g\n",gsl_vector_get(time,i-1));
			fprintf(outforce2,"%g\n",gsl_matrix_get(force,i-1,0));
			fprintf(outforce1,"%g\n",gsl_matrix_get(force,i-1,1));
			fprintf(outshear,"%g\n",gsl_vector_get(V,i-1));
		}
		fclose(outtime);
		fclose(outforce2);
		fclose(outforce1);
		fclose(outshear);
		fclose(outdisp1);
		fclose(outdisp2);
	}*/


	// Freeing allocated memory ( Nonlinear Analysis )
	gsl_vector_free(Force_i);
	gsl_vector_free(Force_j);
	gsl_vector_free(Force_mid);
	gsl_vector_free(u_i);
	gsl_vector_free(du_i);
	gsl_vector_free(d2u_i);
	gsl_vector_free(u_j);
	gsl_vector_free(du_j);
	gsl_vector_free(d2u_j);
	gsl_vector_free(u_mid);
	gsl_vector_free(du_mid);
	gsl_vector_free(d2u_mid);
	gsl_vector_free(tempfsi);
	gsl_vector_free(tempfsj);
	gsl_vector_free(tempfsmid);
	gsl_vector_free(tempf);
	gsl_vector_free(tempF);
	gsl_vector_free(tempv);
	//gsl_matrix_free(acc);
	//gsl_matrix_free(disp);
	//gsl_vector_free(time);
	//gsl_matrix_free(force);
	//gsl_vector_free(V);
	gsl_matrix_free(K_cp);
	gsl_vector_free(u_ii);
	gsl_vector_free(du_ii);
	gsl_vector_free(d2u_ii);
	gsl_vector_free(fs_ii);
	gsl_vector_free(tempfsii);
	gsl_vector_free(Force_ii);
	gsl_vector_free(fs_mid);
	gsl_vector_free(fyc);
	gsl_vector_free(fyt);
	gsl_vector_free(key);
	gsl_vector_free(Dy);
	gsl_vector_free(fs_i);
	gsl_vector_free(fs_j);
	gsl_vector_free(kstr_copy);
	gsl_vector_free(ddm_i);
	gsl_vector_free(ddm_j);
	gsl_vector_free(ddm_ii);
	gsl_vector_free(ddm_mid);
	gsl_vector_free(duc_i);
	gsl_vector_free(duc_ii);
	gsl_vector_free(duc_j);
	gsl_vector_free(duc_mid);



	return 0;

}
// ---------------------------------------- Nonlinear Analysis MDOF End

// ---------------------------------------- linear Analysis SDOF Function
//[De,De_tot,Ae,Ae_tot]=LinearANAlysisSDOF(K,C,M,L,FIMh,FIMr,n,dt,dof);
int RSoilStructureInteractionModel::linearAnalysisSDOF(double he, gsl_matrix *K, gsl_matrix *C, gsl_matrix *M, gsl_matrix *L, QVector<double> FIMh, QVector<double> FIMr, double n, double dt, double dof , double *De, double *De_tot, double *Ae, double *Ae_tot, int threadID)
{
	gsl_vector *Force_i = gsl_vector_calloc(dof);
	gsl_vector *Force_j = gsl_vector_calloc(dof);
	gsl_vector *u_i = gsl_vector_calloc(dof);
	gsl_vector *du_i = gsl_vector_calloc(dof);
	gsl_vector *u_j = gsl_vector_calloc(dof);
	gsl_vector *du_j = gsl_vector_calloc(dof);
	gsl_vector *d2u_j = gsl_vector_calloc(dof);
	gsl_vector *d2u_i = gsl_vector_calloc(dof);
	gsl_vector *tempf = gsl_vector_calloc(dof);
	gsl_vector *tempv=gsl_vector_calloc(2);	
	//gsl_vector *disp = gsl_vector_calloc(n);
	//gsl_vector *acc = gsl_vector_calloc(n);
	//gsl_vector *time = gsl_vector_calloc(n);
	int gsl_status;
	int nm_status=0;		// Newmark function status
	double fs_j=0.0;
	double fs_i=0.0;
	*De = 0.0;			// Maximum Elastic Displacement 
	*De_tot = 0.0;	// Total Maximum Elastic Displacement
	*Ae = 0.0;			// Maximum Elastic Accelaration 
	*Ae_tot=0.0;	// Total Maximum Elastic Accelaration 

	for (int i=0; i <= n-2; i++){
		//gsl_vector_set(time,i,i*dt);
		//Force_i=-M*L*[FIMh(i);FIMr(i)];
		gsl_vector_set(tempv,0,FIMh[i]);
		gsl_vector_set(tempv,1,FIMr[i]);
		gsl_status = gsl_blas_dgemv(CblasNoTrans, -1.0, L, tempv,0.0,tempf);
		if (gsl_status) {  
			rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
			return -1;
		}
		gsl_status = gsl_blas_dgemv(CblasNoTrans, 1, M, tempf, 0.0, Force_i);
		if (gsl_status) {  
			rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
			return -1;
		}
		//Force_j=-M*L*[FIMh(i+1);FIMr(i+1)];
		gsl_vector_set(tempv,0,FIMh[i+1]);
		gsl_vector_set(tempv,1,FIMr[i+1]);
		gsl_status = gsl_blas_dgemv(CblasNoTrans, -1.0, L, tempv,0.0,tempf);
		if (gsl_status) {  
			rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
			return -1;
		}
		gsl_status = gsl_blas_dgemv(CblasNoTrans, 1, M, tempf, 0.0, Force_j);
		if (gsl_status) {  
			rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
			return -1;
		}

		//[u_j,du_j,d2u_i,d2u_j,fs_j]=NewmarkBeta(K,dt,C,M,Force_i,Force_j,fs_i,u_i,du_i,dof);
		nm_status = newmarkBetaSDOF(K, dt, C, M, Force_i, Force_j, fs_i, u_i, du_i, dof, u_j, du_j,d2u_i,d2u_j, &fs_j,threadID);
		if (nm_status) {  
			rCritical(threadID) << "Error in NewmarkBeta function";
			return -1;
		}
		//gsl_vector_set(disp,i+1,gsl_vector_get(u_j,0));
		//gsl_vector_set(acc,i+1,gsl_vector_get(d2u_j,0));
		// Calculationg Maximum Elastic Displacement:
		if (qAbs(gsl_vector_get(u_j,0))>*De){
			*De=qAbs(gsl_vector_get(u_j,0));
		} 
		// Calculationg Maximum Elastic Accelaration:
		if (qAbs(gsl_vector_get(d2u_j,0))>*Ae){
			*Ae = qAbs(gsl_vector_get(d2u_j,0));
		}

		//Calculationg Maximum Total Elastic Displacement & Accelaration:

		if (dof==1){
			*De_tot = *De;
			double d2u_j1=gsl_vector_get(d2u_j,0);
			double d2u_gh=FIMh[i+1];
			double d2u_gr=FIMr[i+1];
			if (qAbs(d2u_j1+d2u_gh+d2u_gr*he)> *Ae_tot){
				*Ae_tot = qAbs(d2u_j1+d2u_gh+d2u_gr*he);
			}
		}else{
			double u_j1=gsl_vector_get(u_j,0);
			double u_j2=gsl_vector_get(u_j,1);
			double u_j3=gsl_vector_get(u_j,2);
			if (qAbs(u_j1+u_j2+he*u_j3) > *De_tot){
				*De_tot=qAbs(u_j1+u_j2+he*u_j3);
			}
			double d2u_j1=gsl_vector_get(d2u_j,0);
			double d2u_j2=gsl_vector_get(d2u_j,1);
			double d2u_j3=gsl_vector_get(d2u_j,2);
			double d2u_gh=FIMh[i+1];
			double d2u_gr=FIMr[i+1];
			if (qAbs(d2u_j1+d2u_j2+he*d2u_j3+d2u_gh+d2u_gr*he) > *Ae_tot){
				*Ae_tot = qAbs(d2u_j1+d2u_j2+he*d2u_j3+d2u_gh+d2u_gr*he);
			}
		}

		fs_i=fs_j;
		gsl_status = gsl_vector_memcpy(u_i,u_j);
		if (gsl_status) {  
			rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
			return -1;
		}
		gsl_status = gsl_vector_memcpy(du_i,du_j);
		if (gsl_status) {  
			rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
			return -1;
		}
	}	// end for i
	/*RDomain *theDomain = qobject_cast<RDomain *>(this->parent());
	rDebug(threadID) << "disp = " << endl;
	theDomain->print(disp);
	rDebug(threadID) << "acc = " << endl;
	theDomain->print(acc);
	rDebug(threadID) << "time = " << endl;
	theDomain->print(time);*/
/*	FILE *outacc;
	FILE *outdisp;
	FILE *outtime;
	outacc = fopen ("AccelerationSDOFLinear.txt","wt");
	outdisp = fopen ("DisplacementSDOFLinear.txt","wt");
	outtime= fopen ("TimeSDOFLinear.txt","wt");
	for (int i=1;i<=n;i++){
		fprintf(outacc,"%g\n",gsl_vector_get(acc,i-1));
		fprintf(outdisp,"%g\n",gsl_vector_get(disp,i-1));
		fprintf(outtime,"%g\n",gsl_vector_get(time,i-1));
	}*/
	// Freeing allocated memory ( Linear Analysis )
	gsl_vector_free(tempv);
	gsl_vector_free(Force_i);
	gsl_vector_free(Force_j);
	gsl_vector_free(u_i);
	gsl_vector_free(u_j);
	gsl_vector_free(du_i);
	gsl_vector_free(du_j);
	gsl_vector_free(d2u_j);
	gsl_vector_free(d2u_i);
	gsl_vector_free(tempf);

	return 0;
}
// --------------------------------- Linear Analysis SDOF Function End

// ---------------------------------------- linear Analysis MDOF Function
//[De,De_tot,Ae,Ae_tot]=LinearANAlysisMDOF(h_story,kstr,K,C,M,L,FIMh,FIMr,n,dt,dof);
int RSoilStructureInteractionModel::linearAnalysisMDOF(double h_story, double e, gsl_vector *kstr,gsl_matrix *K, gsl_matrix *C, gsl_matrix *M, gsl_matrix *L, QVector<double> FIMh, QVector<double> FIMr, double n, double dt, double dof , int Num, gsl_vector *De, gsl_vector *De_tot, gsl_vector *Ae, gsl_vector *Ae_tot, double *Ve, int threadID)
{
	gsl_vector *Force_i = gsl_vector_calloc(dof);
	gsl_vector *Force_j = gsl_vector_calloc(dof);
	gsl_vector *fs_i = gsl_vector_calloc(Num);
	gsl_vector *fs_j = gsl_vector_calloc(Num);
	gsl_vector *u_i = gsl_vector_calloc(dof);
	gsl_vector *du_i = gsl_vector_calloc(dof);
	gsl_vector *u_j = gsl_vector_calloc(dof);
	gsl_vector *du_j = gsl_vector_calloc(dof);
	gsl_vector *d2u_j = gsl_vector_calloc(dof);
	gsl_vector *d2u_i = gsl_vector_calloc(dof);
	gsl_vector *tempf = gsl_vector_calloc(dof);
	gsl_vector *tempv=gsl_vector_calloc(2);	
	//gsl_matrix *disp = gsl_matrix_calloc(n,Num);
	//gsl_matrix *acc = gsl_matrix_calloc(n,Num);
	//gsl_vector *time = gsl_vector_calloc(n);
	//gsl_matrix *force = gsl_matrix_calloc(n,dof);
	//gsl_vector *V = gsl_vector_calloc(n);
	*Ve = 0.0;
	int gsl_status;
	int nm_status=0;		// Newmark function status
	gsl_vector_set_all(De,0);
	gsl_vector_set_all(Ae,0);
	gsl_vector_set_all(De_tot,0);
	gsl_vector_set_all(Ae_tot,0);
	for (int i=0; i <= n-2; i++){
		//Force_i=-M*L*[FIMh(i);FIMr(i)];
		gsl_vector_set(tempv,0,FIMh[i]);
		gsl_vector_set(tempv,1,FIMr[i]);
		gsl_status = gsl_blas_dgemv(CblasNoTrans, -1.0, L, tempv,0.0,tempf);
		if (gsl_status) {  
			rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
			return -1;
		}
		gsl_status = gsl_blas_dgemv(CblasNoTrans, 1, M, tempf, 0.0, Force_i);
		if (gsl_status) {  
			rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
			return -1;
		}
		//Force_j=-M*L*[FIMh(i+1);FIMr(i+1)];
		gsl_vector_set(tempv,0,FIMh[i+1]);
		gsl_vector_set(tempv,1,FIMr[i+1]);
		gsl_status = gsl_blas_dgemv(CblasNoTrans, -1.0, L, tempv,0.0,tempf);
		if (gsl_status) {  
			rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
			return -1;
		}
		gsl_status = gsl_blas_dgemv(CblasNoTrans, 1, M, tempf, 0.0, Force_j);
		if (gsl_status) {  
			rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
			return -1;
		}

		//[u_j,du_j,d2u_i,d2u_j,fs_j]=NewmarkBetaMDOFLinear(K,dt,C,M,Force_i,Force_j,fs_i,u_i,du_i,dof);
		nm_status = newmarkBetaMDOFLinear(K, dt, C, M, Force_i, Force_j, fs_i, u_i, du_i, Num, dof, u_j, du_j,d2u_i,d2u_j, fs_j,threadID);
		if (nm_status) {  
			rCritical(threadID) << "Error in NewmarkBetaMDOFLinear function";
			return -1;
		}
		double v=0;
		for (int j=1;j<=Num;j++){
			v=v+gsl_vector_get(fs_j,j-1);
		}
		if ((qAbs(v))>(qAbs(*Ve))){
			*Ve = qAbs(v);
		}

		/*gsl_vector_set(time,i+1,(i+1)*dt);
		for (int j=1;j<=Num;j++){
			gsl_matrix_set(disp,i+1,j-1,gsl_vector_get(u_j,j-1));
		}
		gsl_matrix_set(force,i+1,0,gsl_vector_get(fs_j,0));
		for (int j=2;j<=dof;j++){
			gsl_matrix_set(force,i+1,j-1,gsl_vector_get(fs_j,j-1)-gsl_vector_get(fs_j,j-2));
		}
		gsl_vector_set(V,i+1,gsl_vector_get(fs_j,Num-1));
		//gsl_matrix_set_row(acc,i+1,d2u_j);*/
		// Calculating Maximum Elastic Displacement:
		for (int j=1;j<=Num;j++){
			if (qAbs(gsl_vector_get(u_j,j-1))>(gsl_vector_get(De,j-1))){
				gsl_vector_set(De,j-1,qAbs(gsl_vector_get(u_j,j-1)));
			} 
			// Calculationg Maximum Elastic Accelaration:
			if (qAbs(gsl_vector_get(d2u_j,j-1))>(gsl_vector_get(Ae,j-1))){
				gsl_vector_set(Ae,j-1,qAbs(gsl_vector_get(d2u_j,j-1)));
			}
		}

		//Calculationg Maximum Total Elastic Displacement & Accelaration:
		double d2u_gh=FIMh[i+1];
		double d2u_gr=FIMr[i+1];
		if (dof==Num){	//Fixed Base
			gsl_status = gsl_vector_memcpy(De_tot,De);
			if (gsl_status) {  
				rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
				return -1;
			}
			for (int j=1;j<=Num;j++){
				if (qAbs(gsl_vector_get(d2u_j,j-1)+d2u_gh+d2u_gr*((Num-j+1)*h_story+e))> gsl_vector_get(Ae_tot,j-1)){		//j*h_story=height of story with respect to the ground;
					gsl_vector_set(Ae_tot,j-1,qAbs(gsl_vector_get(d2u_j,j-1)+d2u_gh+d2u_gr*((Num-j+1)*h_story+e)));
				}
			}
		}else{		 //Flexible Base
			for (int j=1;j<=Num;j++){
				double u_j2=gsl_vector_get(u_j,dof-3);
				double u_j3=gsl_vector_get(u_j,dof-2);
				if (qAbs(gsl_vector_get(u_j,j-1)+u_j2+((Num-j+1)*h_story+e)*u_j3) > gsl_vector_get(De_tot,j-1)){
					gsl_vector_set(De_tot,j-1,qAbs(gsl_vector_get(u_j,j-1)+u_j2+((Num-j+1)*h_story+e)*u_j3));
				}
				
				double d2u_j2=gsl_vector_get(d2u_j,dof-3);
				double d2u_j3=gsl_vector_get(d2u_j,dof-2);
				if (qAbs(gsl_vector_get(d2u_j,j-1)+d2u_j2+((Num-j+1)*h_story+e)*d2u_j3+d2u_gh+d2u_gr*((Num-j+1)*h_story+e)) > gsl_vector_get(Ae_tot,j-1)){
					gsl_vector_set(Ae_tot,j-1,qAbs(gsl_vector_get(d2u_j,j-1)+d2u_j2+((Num-j+1)*h_story+e)*d2u_j3+d2u_gh+d2u_gr*((Num-j+1)*h_story+e)));
				}
				
			} //end for j
		} //end if flexible/fixed
		gsl_status = gsl_vector_memcpy(fs_i,fs_j);
		if (gsl_status) {  
			rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
			return -1;
		}
		gsl_status = gsl_vector_memcpy(u_i,u_j);
		if (gsl_status) {  
			rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
			return -1;
		}
		gsl_status = gsl_vector_memcpy(du_i,du_j);
		if (gsl_status) {  
			rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
			return -1;
		}

	}	// end for i

/*	//Print time-history of responses
	
	if (Num==1){	//SDOF
		FILE *outdisp1;
		//FILE *outdisp2;
		FILE *outtime;
		//FILE *outforce1;
		//FILE *outforce2;
		FILE *outshear;
	//	outacc = fopen ("AccelerationMDOFLinear.txt","wt");
		outdisp1 = fopen ("SDisplacementMDOFLinearN1.txt","wt");
		outtime = fopen ("STimeMDOFLinear.txt","wt");
	//	outforce1 = fopen ("SForceMDOFLinearN1.txt","wt");
		outshear = fopen ("SBaseShearMDOFLinear.txt","wt");
		for (int i=1;i<=n;i++){
		//	fprintf(outacc,"%g\n",gsl_matrix_get(acc,i-1,0));
			fprintf(outdisp1,"%g\n",gsl_matrix_get(disp,i-1,0));
			fprintf(outtime,"%g\n",gsl_vector_get(time,i-1));
			//fprintf(outforce1,"%g\n",gsl_matrix_get(force,i-1,0));
			fprintf(outshear,"%g\n",gsl_vector_get(V,i-1));
		}
		fclose(outtime);
		//fclose(outforce2);
		//fclose(outforce1);
		fclose(outshear);
		fclose(outdisp1);
		//fclose(outdisp2);
	}else{	//MDOF,N=2
		FILE *outdisp1;
		FILE *outdisp2;
		FILE *outtime;
		FILE *outforce1;
		FILE *outforce2;
		FILE *outshear;
	//	outacc = fopen ("AccelerationMDOFLinear.txt","wt");
		outdisp1 = fopen ("MDisplacementMDOFLinearN1.txt","wt");
		outdisp2 = fopen ("MDisplacementMDOFLinearN2.txt","wt");
		outtime = fopen ("MTimeMDOFLinear.txt","wt");
		outforce2 = fopen ("MForceMDOFLinearN2.txt","wt");
		outforce1 = fopen ("MForceMDOFLinearN1.txt","wt");
		outshear = fopen ("MBaseShearMDOFLinear.txt","wt");
		for (int i=1;i<=n;i++){
		//	fprintf(outacc,"%g\n",gsl_matrix_get(acc,i-1,0));
			fprintf(outdisp2,"%g\n",gsl_matrix_get(disp,i-1,0));
			fprintf(outdisp1,"%g\n",gsl_matrix_get(disp,i-1,1));
			fprintf(outtime,"%g\n",gsl_vector_get(time,i-1));
			fprintf(outforce2,"%g\n",gsl_matrix_get(force,i-1,0));
			fprintf(outforce1,"%g\n",gsl_matrix_get(force,i-1,1));
			fprintf(outshear,"%g\n",gsl_vector_get(V,i-1));
		}
		fclose(outtime);
		fclose(outforce2);
		fclose(outforce1);
		fclose(outshear);
		fclose(outdisp1);
		fclose(outdisp2);
	}*/
	
	// Freeing allocated memory ( Linear Analysis )
	gsl_vector_free(tempv);
	gsl_vector_free(Force_i);
	gsl_vector_free(Force_j);
	gsl_vector_free(fs_i);
	gsl_vector_free(fs_j);
	gsl_vector_free(u_i);
	gsl_vector_free(u_j);
	gsl_vector_free(du_i);
	gsl_vector_free(du_j);
	gsl_vector_free(d2u_j);
	gsl_vector_free(d2u_i);
	gsl_vector_free(tempf);
	//gsl_matrix_free(disp);
	//gsl_matrix_free(acc);
	//gsl_vector_free(time);
	//gsl_matrix_free(force);
	//gsl_vector_free(V);


	return 0;
}
// --------------------------------- Linear Analysis MDOF Function End

// ---------------------------------------- NewmarkBeta for SDOF
//[u_j,du_j, d2u_i, d2u_j, fs_j]=NewmarkBetaSDOF(K,dt,C,M,Force_i,Force_j,fs_i,u_i,du_i,dof);
int RSoilStructureInteractionModel::newmarkBetaSDOF(gsl_matrix *K, double dt, gsl_matrix *C, gsl_matrix *M, gsl_vector *Force_i, gsl_vector *Force_j, double fs_i, gsl_vector *u_i, gsl_vector *du_i, int dof, gsl_vector *u_j, gsl_vector *du_j,gsl_vector *d2u_i,gsl_vector *d2u_j, double *fs_j , int threadID)
{	
	int gsl_status;
	double gamma=0.5;
	double beta=0.25;
	double g_acc=1.0;

	//k_bar=K+gamma/(beta*dt)*C+M/(beta*dt^2);
	gsl_matrix *k_bar = gsl_matrix_calloc(dof,dof);
	gsl_matrix *tempm = gsl_matrix_calloc(dof,dof);
	gsl_vector *tempv = gsl_vector_calloc(dof);
	gsl_vector *tempa = gsl_vector_calloc(dof);
	gsl_status = gsl_matrix_memcpy(k_bar,K);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
		return -1;
	}

	gsl_status = gsl_matrix_memcpy(tempm,C);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_matrix_scale(tempm,gamma/(beta*dt));
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_matrix_add(k_bar,tempm);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_matrix_memcpy(tempm,M);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_matrix_scale(tempm,1.0/(beta*dt*dt));
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_matrix_add(k_bar,tempm);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
		return -1;
	}
	//aa=1/(beta*dt)*M+(gamma/beta)*C;
	gsl_matrix *aa = gsl_matrix_calloc(dof,dof);

	gsl_status = gsl_matrix_memcpy(aa,M);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_matrix_scale(aa,1.0/(beta*dt));
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_matrix_memcpy(tempm,C);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_matrix_scale(tempm,gamma/beta);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_matrix_add(aa,tempm);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
		return -1;
	}
	//bb=1/(2*beta)*M+dt*(gamma/(2*beta)-1)*C;
	gsl_matrix *bb = gsl_matrix_calloc(dof,dof);

	gsl_status = gsl_matrix_memcpy(bb,M);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_matrix_scale(bb,1.0/(2*beta));
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_matrix_memcpy(tempm,C);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_matrix_scale(tempm,dt*(gamma/(2*beta)-1.0));
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_matrix_add(bb,tempm);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
		return -1;
	}
	//(dof=1): d2u_i=inv(M)*(g_acc*Force_i-fs_i-C*du_i);
	//(dof=4): d2u_i=inv(M)*(g_acc*Force_i-[fs_i;K(2:4,2:4)*u_i(2:4,1)]-C*du_i);
	gsl_vector *rhsv = gsl_vector_calloc(dof);
	gsl_vector *tempv2 = gsl_vector_calloc(dof);

	gsl_status = gsl_vector_memcpy(rhsv,Force_i);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_vector_scale(rhsv,g_acc);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_blas_dgemv(CblasNoTrans, 1.0, C, du_i,0.0,tempv);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_vector_sub(rhsv,tempv);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
		return -1;
	}
	double tempd;
	if (dof==1){
		tempd = gsl_vector_get(rhsv,0);
		tempd = tempd - fs_i;
		gsl_vector_set(rhsv,0,tempd);
	}
	if (dof==4){
		gsl_status = gsl_vector_memcpy(tempv,u_i);
		if (gsl_status) {  
			rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
			return -1;
		}
		gsl_vector_set(tempv,0,0.0);	
		gsl_status = gsl_blas_dgemv(CblasNoTrans, 1.0, K, tempv,0.0,tempv2);
		if (gsl_status) {  
			rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
			return -1;
		}
		gsl_vector_set(tempv2,0,fs_i);
		gsl_status = gsl_vector_sub(rhsv,tempv2);
		if (gsl_status) {  
			rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
			return -1;
		}
	}

	// Solving M * d2u_i = rhsv 
	int s;
	gsl_permutation *p = gsl_permutation_calloc(dof);
	gsl_status = gsl_matrix_memcpy(tempm,M);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_linalg_LU_decomp (tempm, p, &s);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_linalg_LU_solve (tempm, p, rhsv, d2u_i);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
		return -1;
	}
	//delta_p=g_acc*(Force_j-Force_i)+aa*du_i+bb*d2u_i;
	gsl_vector *delta_p = gsl_vector_calloc(dof);
	gsl_vector *delta_u = gsl_vector_calloc(dof);

	gsl_status = gsl_vector_memcpy(delta_p,Force_j);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_vector_sub(delta_p,Force_i);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_vector_scale(delta_p,g_acc);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_blas_dgemv(CblasNoTrans, 1.0, aa, du_i,0.0,tempv);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_vector_add(delta_p,tempv);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_blas_dgemv(CblasNoTrans, 1.0, bb, d2u_i,0.0,tempv);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_vector_add(delta_p,tempv);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
		return -1;
	}
	//delta_u=inv(k_bar)*delta_p;
	gsl_permutation_init(p);

	gsl_status = gsl_matrix_memcpy(tempm,k_bar);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_linalg_LU_decomp (tempm, p, &s);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_linalg_LU_solve (tempm, p, delta_p, delta_u);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
		return -1;
	}
	//delta_du=gamma/(beta*dt)*delta_u - (gamma/beta)*du_i + dt*(1-gamma/(2*beta))*d2u_i;
	gsl_vector *delta_du = gsl_vector_calloc(dof);

	gsl_status = gsl_vector_memcpy(delta_du,delta_u);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_vector_scale(delta_du,gamma/(beta*dt));
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_vector_memcpy(tempv,du_i);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_vector_scale(tempv,gamma/beta);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_vector_sub(delta_du,tempv);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_vector_memcpy(tempv,d2u_i);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_vector_scale(tempv,dt*(1.0-gamma/(2*beta)));
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_vector_add(delta_du,tempv);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
		return -1;
	}
	//delta_d2u=1/(beta*dt*dt)*delta_u - (1/beta*dt)*du_i +(1/(2*beta))*d2u_i;
	gsl_vector *delta_d2u = gsl_vector_calloc(dof);

	gsl_status = gsl_vector_memcpy(delta_d2u,delta_u);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_vector_scale(delta_d2u,1.0/(beta*dt*dt));
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_vector_memcpy(tempa,du_i);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_vector_scale(tempa,1.0/(beta*dt));
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_vector_sub(delta_d2u,tempa);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_vector_memcpy(tempa,d2u_i);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_vector_scale(tempa,1.0/(2*beta));
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_vector_sub(delta_d2u,tempa);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
		return -1;
	}
	//u_j=u_i+delta_u;
	gsl_status = gsl_vector_memcpy(u_j,u_i);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_vector_add(u_j,delta_u);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
		return -1;
	}
	//fs_j=fs_i+K(1,1)*delta_u(1,1);
	double k11 = gsl_matrix_get(K,0,0);
	double delta_u1 = gsl_vector_get(delta_u,0);
	*fs_j=fs_i+k11*delta_u1;
	//du_j=du_i+delta_du;
	gsl_status = gsl_vector_memcpy(du_j,du_i);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_vector_add(du_j,delta_du);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
		return -1;
	}
	//d2u_j=d2u_i+delta_d2u;
	gsl_status = gsl_vector_memcpy(d2u_j,d2u_i);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_vector_add(d2u_j,delta_d2u);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_permutation_free (p);
	gsl_matrix_free(k_bar);
	gsl_matrix_free(tempm);
	gsl_matrix_free(aa);
	gsl_matrix_free(bb);
	gsl_vector_free(tempv);
	gsl_vector_free(tempa);
	gsl_vector_free(tempv2);
	gsl_vector_free(rhsv);
	gsl_vector_free(delta_p);
	gsl_vector_free(delta_u);
	gsl_vector_free(delta_du);
	gsl_vector_free(delta_d2u);
	return 0;
}
//------------------------------------------ Newmark Beta SDOF End

// ---------------------------------------- NewmarkBeta for MDOF Nonlinear
//[u_j,du_j, d2u_i, d2u_j, fs_j]=NewmarkBetaMDOFNonlinear(kstr, K,dt,C,M, fyc, fyt,Force_i,Force_j,fs_i,u_i,du_i,dof);
int RSoilStructureInteractionModel::newmarkBetaMDOFNonlinear(gsl_vector *kstr, gsl_matrix *K, double dt, gsl_matrix *C, gsl_matrix *M, gsl_vector *fyc, gsl_vector *fyt, gsl_vector *Force_i, gsl_vector *Force_j, gsl_vector *fs_i, gsl_vector *tempfsi, gsl_vector *u_i, gsl_vector *du_i, int Num, int dof, gsl_vector *u_j, gsl_vector *du_j,gsl_vector *d2u_i,gsl_vector *d2u_j, gsl_vector *fs_j, gsl_vector *tempfsj, int threadID)
{	
	int gsl_status;
	double gamma=0.5;
	double beta=0.25;
	double g_acc=1.0;

	//k_bar=K+gamma/(beta*dt)*C+M/(beta*dt^2);
	gsl_matrix *k_bar = gsl_matrix_calloc(dof,dof);
	gsl_matrix *tempm = gsl_matrix_calloc(dof,dof);
	gsl_vector *tempv = gsl_vector_calloc(dof);
	gsl_vector *tempv2 = gsl_vector_calloc(dof);
	gsl_vector *tempa = gsl_vector_calloc(dof);
	gsl_status = gsl_matrix_memcpy(k_bar,K);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL1:" << gsl_strerror(gsl_status);
		return -1;
	}

	gsl_status = gsl_matrix_memcpy(tempm,C);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL2:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_matrix_scale(tempm,gamma/(beta*dt));
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL3:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_matrix_add(k_bar,tempm);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL4:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_matrix_memcpy(tempm,M);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL5:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_matrix_scale(tempm,1.0/(beta*dt*dt));
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL6:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_matrix_add(k_bar,tempm);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL7:" << gsl_strerror(gsl_status);
		return -1;
	}

	//aa=1/(beta*dt)*M+(gamma/beta)*C;
	gsl_matrix *aa = gsl_matrix_calloc(dof,dof);

	gsl_status = gsl_matrix_memcpy(aa,M);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL8:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_matrix_scale(aa,1.0/(beta*dt));
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL9:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_matrix_memcpy(tempm,C);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL10:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_matrix_scale(tempm,gamma/beta);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL11:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_matrix_add(aa,tempm);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL12:" << gsl_strerror(gsl_status);
		return -1;
	}

	//bb=1/(2*beta)*M+dt*(gamma/(2*beta)-1)*C;
	gsl_matrix *bb = gsl_matrix_calloc(dof,dof);

	gsl_status = gsl_matrix_memcpy(bb,M);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL13:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_matrix_scale(bb,1.0/(2*beta));
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL14:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_matrix_memcpy(tempm,C);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL15:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_matrix_scale(tempm,dt*((gamma/(2*beta))-1.0));
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL16:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_matrix_add(bb,tempm);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL17:" << gsl_strerror(gsl_status);
		return -1;
	}

	//(dof=Num): d2u_i=inv(M)*(g_acc*Force_i-fs_i-C*du_i);
	//(dof=Num+3): d2u_i=inv(M)*(g_acc*Force_i-[fs_i;K(Num+1:dof,Num+1:dof)*u_i(Num+1:dof,1)]-C*du_i);
	gsl_vector *rhsv = gsl_vector_calloc(dof);

	gsl_status = gsl_vector_memcpy(rhsv,Force_i);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL18:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_vector_scale(rhsv,g_acc);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL19:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_blas_dgemv(CblasNoTrans, 1.0, C, du_i,0.0,tempv);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL20:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_vector_sub(rhsv,tempv);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL21:" << gsl_strerror(gsl_status);
		return -1;
	}
	if (dof==Num){
		gsl_status = gsl_vector_sub(rhsv,fs_i);
		if (gsl_status) {  
			rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
			return -1;
		}
	}
	else{	//dof=Num+3
		gsl_status = gsl_vector_memcpy(tempv,u_i);
		if (gsl_status) {  
			rCritical(threadID) << "Error in GSL22:" << gsl_strerror(gsl_status);
			return -1;
		}
		for (int j=1;j<=Num;j++){
			gsl_vector_set(tempv,j-1,0.0);
		}
		gsl_status = gsl_blas_dgemv(CblasNoTrans, 1.0, K, tempv,0.0,tempv2);
		if (gsl_status) {  
			rCritical(threadID) << "Error in GSL23:" << gsl_strerror(gsl_status);
			return -1;
		}
		for (int j=1;j<=Num;j++){
			gsl_vector_set(tempv2,j-1,gsl_vector_get(fs_i,j-1));
		}
		gsl_status = gsl_vector_sub(rhsv,tempv2);
		if (gsl_status) {  
			rCritical(threadID) << "Error in GSL24:" << gsl_strerror(gsl_status);
			return -1;
		}
	}

	// Solving M * d2u_i = rhsv 
	int s;
	gsl_permutation *p = gsl_permutation_calloc(dof);
	gsl_status = gsl_matrix_memcpy(tempm,M);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL25:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_linalg_LU_decomp (tempm, p, &s);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL26:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_linalg_LU_solve (tempm, p, rhsv, d2u_i);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL27:" << gsl_strerror(gsl_status);
		return -1;
	}
	//delta_p=g_acc*(Force_j-Force_i)+aa*du_i+bb*d2u_i;
	gsl_vector *delta_p = gsl_vector_calloc(dof);
	gsl_vector *delta_u = gsl_vector_calloc(dof);

	gsl_status = gsl_vector_memcpy(delta_p,Force_j);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL28:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_vector_sub(delta_p,Force_i);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL29:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_vector_scale(delta_p,g_acc);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL30:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_blas_dgemv(CblasNoTrans, 1.0, aa, du_i,0.0,tempv);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL31:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_vector_add(delta_p,tempv);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL32:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_blas_dgemv(CblasNoTrans, 1.0, bb, d2u_i,0.0,tempv);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL33:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_vector_add(delta_p,tempv);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL34:" << gsl_strerror(gsl_status);
		return -1;
	}
	//delta_u=inv(k_bar)*delta_p;
	gsl_permutation_init(p);
	gsl_status = gsl_matrix_memcpy(tempm,k_bar);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL35:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_linalg_LU_decomp (tempm, p, &s);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL36:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_linalg_LU_solve (tempm, p, delta_p, delta_u);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL37:" << gsl_strerror(gsl_status);
		return -1;
	}
	//delta_du=gamma/(beta*dt)*delta_u - (gamma/beta)*du_i + dt*(1-gamma/(2*beta))*d2u_i;
	gsl_vector *delta_du = gsl_vector_calloc(dof);

	gsl_status = gsl_vector_memcpy(delta_du,delta_u);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL38:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_vector_scale(delta_du,gamma/(beta*dt));
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL39:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_vector_memcpy(tempv,du_i);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL40:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_vector_scale(tempv,gamma/beta);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL41:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_vector_sub(delta_du,tempv);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL42:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_vector_memcpy(tempv,d2u_i);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL43:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_vector_scale(tempv,dt*(1.0-gamma/(2*beta)));
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL44:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_vector_add(delta_du,tempv);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL45:" << gsl_strerror(gsl_status);
		return -1;
	}
	//delta_d2u=1/(beta*dt*dt)*delta_u - (1/beta*dt)*du_i -(1/(2*beta))*d2u_i;
	gsl_vector *delta_d2u = gsl_vector_calloc(dof);
	gsl_status = gsl_vector_memcpy(delta_d2u,delta_u);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL46:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_vector_scale(delta_d2u,1.0/(beta*dt*dt));
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL47:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_vector_memcpy(tempa,du_i);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL48:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_vector_scale(tempa,1.0/(beta*dt));
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL49:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_vector_sub(delta_d2u,tempa);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL50:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_vector_memcpy(tempa,d2u_i);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL51:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_vector_scale(tempa,1.0/(2*beta));
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL52:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_vector_sub(delta_d2u,tempa);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL53:" << gsl_strerror(gsl_status);
		return -1;
	}
	//u_j=u_i+delta_u;
	gsl_status = gsl_vector_memcpy(u_j,u_i);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL54:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_vector_add(u_j,delta_u);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL55:" << gsl_strerror(gsl_status);
		return -1;
	}
	
	//du_j=du_i+delta_du;
	gsl_status = gsl_vector_memcpy(du_j,du_i);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL59:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_vector_add(du_j,delta_du);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL60:" << gsl_strerror(gsl_status);
		return -1;
	}
	//d2u_j=d2u_i+delta_d2u;
	gsl_status = gsl_vector_memcpy(d2u_j,d2u_i);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL61:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_vector_add(d2u_j,delta_d2u);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL62:" << gsl_strerror(gsl_status);
		return -1;
	}
	//fs_j=fs_i+K*delta_u || fy
	gsl_vector *tempuj = gsl_vector_calloc(Num);
	gsl_vector_set(tempuj,Num-1,gsl_vector_get(delta_u,Num-1));
	for (int j=1;j<=(Num-1);j++){
		gsl_vector_set(tempuj,j-1,gsl_vector_get(delta_u,j-1)-gsl_vector_get(delta_u,j));
	}
	for (int j=1;j<=Num;j++){
		gsl_vector_set(tempfsj,j-1,gsl_vector_get(tempfsi,j-1)+(gsl_vector_get(tempuj,j-1)*gsl_vector_get(kstr,j-1)));
		if (gsl_vector_get(tempfsj,j-1)>=gsl_vector_get(fyt,j-1)){
			gsl_vector_set(tempfsj,j-1,gsl_vector_get(fyt,j-1));
		}else if (gsl_vector_get(tempfsj,j-1)<=gsl_vector_get(fyc,j-1)){
			gsl_vector_set(tempfsj,j-1,gsl_vector_get(fyc,j-1));
		}
	}
	gsl_vector_set(fs_j,0,gsl_vector_get(tempfsj,0));
 	for (int j=2;j<=Num;j++){
		gsl_vector_set(fs_j,j-1,gsl_vector_get(tempfsj,j-1)-gsl_vector_get(tempfsj,j-2));
	}

	//Freeing allocated memory
	gsl_permutation_free (p);
	gsl_matrix_free(k_bar);
	gsl_matrix_free(tempm);
	gsl_matrix_free(aa);
	gsl_matrix_free(bb);
	gsl_vector_free(tempv);
	gsl_vector_free(tempa);
	gsl_vector_free(tempv2);
	gsl_vector_free(tempuj);
	gsl_vector_free(rhsv);
	gsl_vector_free(delta_p);
	gsl_vector_free(delta_u);
	gsl_vector_free(delta_du);
	gsl_vector_free(delta_d2u);

	return 0;
}

//------------------------------------------ Newmark Beta MDOF Nonlinear End

// ---------------------------------------- NewmarkBeta for MDOF Linear
//[u_j,du_j, d2u_i, d2u_j, fs_j]=NewmarkBetaMDOFLinear(K,dt,C,M,Force_i,Force_j,fs_i,u_i,du_i,dof);
int RSoilStructureInteractionModel::newmarkBetaMDOFLinear(gsl_matrix *K, double dt, gsl_matrix *C, gsl_matrix *M, gsl_vector *Force_i, gsl_vector *Force_j, gsl_vector *fs_i, gsl_vector *u_i, gsl_vector *du_i, int Num, int dof, gsl_vector *u_j, gsl_vector *du_j,gsl_vector *d2u_i,gsl_vector *d2u_j, gsl_vector *fs_j , int threadID)
{	
	int gsl_status;
	double gamma=0.5;
	double beta=0.25;
	double g_acc=1.0;

	//k_bar=K+gamma/(beta*dt)*C+M/(beta*dt^2);
	gsl_matrix *k_bar = gsl_matrix_calloc(dof,dof);
	gsl_matrix *tempm = gsl_matrix_calloc(dof,dof);
	gsl_vector *tempv = gsl_vector_calloc(dof);
	gsl_vector *tempv2 = gsl_vector_calloc(dof);
	gsl_vector *tempa = gsl_vector_calloc(dof);
	gsl_status = gsl_matrix_memcpy(k_bar,K);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL1:" << gsl_strerror(gsl_status);
		return -1;
	}

	gsl_status = gsl_matrix_memcpy(tempm,C);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL2:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_matrix_scale(tempm,gamma/(beta*dt));
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL3:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_matrix_add(k_bar,tempm);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL4:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_matrix_memcpy(tempm,M);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL5:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_matrix_scale(tempm,1.0/(beta*dt*dt));
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL6:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_matrix_add(k_bar,tempm);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL7:" << gsl_strerror(gsl_status);
		return -1;
	}

	//aa=1/(beta*dt)*M+(gamma/beta)*C;
	gsl_matrix *aa = gsl_matrix_calloc(dof,dof);

	gsl_status = gsl_matrix_memcpy(aa,M);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL8:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_matrix_scale(aa,1.0/(beta*dt));
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL9:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_matrix_memcpy(tempm,C);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL10:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_matrix_scale(tempm,gamma/beta);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL11:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_matrix_add(aa,tempm);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL12:" << gsl_strerror(gsl_status);
		return -1;
	}

	//bb=1/(2*beta)*M+dt*(gamma/(2*beta)-1)*C;
	gsl_matrix *bb = gsl_matrix_calloc(dof,dof);

	gsl_status = gsl_matrix_memcpy(bb,M);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL13:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_matrix_scale(bb,1.0/(2*beta));
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL14:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_matrix_memcpy(tempm,C);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL15:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_matrix_scale(tempm,dt*((gamma/(2*beta))-1.0));
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL16:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_matrix_add(bb,tempm);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL17:" << gsl_strerror(gsl_status);
		return -1;
	}

	//(dof=Num): d2u_i=inv(M)*(g_acc*Force_i-fs_i-C*du_i);
	//(dof=Num+3): d2u_i=inv(M)*(g_acc*Force_i-[fs_i;K(Num+1:dof,Num+1:dof)*u_i(Num+1:dof,1)]-C*du_i);
	gsl_vector *rhsv = gsl_vector_calloc(dof);

	gsl_status = gsl_vector_memcpy(rhsv,Force_i);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL18:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_vector_scale(rhsv,g_acc);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL19:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_blas_dgemv(CblasNoTrans, 1.0, C, du_i,0.0,tempv);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL20:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_vector_sub(rhsv,tempv);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL21:" << gsl_strerror(gsl_status);
		return -1;
	}
	if (dof==Num){
		gsl_status = gsl_vector_sub(rhsv,fs_i);
		if (gsl_status) {  
			rCritical(threadID) << "Error in GSL:" << gsl_strerror(gsl_status);
			return -1;
		}
	}
	else{		//dof=Num+3
		gsl_status = gsl_vector_memcpy(tempv,u_i);
		if (gsl_status) {  
			rCritical(threadID) << "Error in GSL22:" << gsl_strerror(gsl_status);
			return -1;
		}
		for (int j=1;j<=Num;j++){
			gsl_vector_set(tempv,j-1,0.0);
		}
		gsl_status = gsl_blas_dgemv(CblasNoTrans, 1.0, K, tempv,0.0,tempv2);
		if (gsl_status) {  
			rCritical(threadID) << "Error in GSL23:" << gsl_strerror(gsl_status);
			return -1;
		}
		for (int j=1;j<=Num;j++){
			gsl_vector_set(tempv2,j-1,gsl_vector_get(fs_i,j-1));
		}
		gsl_status = gsl_vector_sub(rhsv,tempv2);
		if (gsl_status) {  
			rCritical(threadID) << "Error in GSL24:" << gsl_strerror(gsl_status);
			return -1;
		}
	}

	// Solving M * d2u_i = rhsv 
	int s;
	gsl_permutation *p = gsl_permutation_calloc(dof);
	gsl_status = gsl_matrix_memcpy(tempm,M);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL25:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_linalg_LU_decomp (tempm, p, &s);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL26:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_linalg_LU_solve (tempm, p, rhsv, d2u_i);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL27:" << gsl_strerror(gsl_status);
		return -1;
	}
	//delta_p=g_acc*(Force_j-Force_i)+aa*du_i+bb*d2u_i;
	gsl_vector *delta_p = gsl_vector_calloc(dof);
	gsl_vector *delta_u = gsl_vector_calloc(dof);

	gsl_status = gsl_vector_memcpy(delta_p,Force_j);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL28:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_vector_sub(delta_p,Force_i);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL29:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_vector_scale(delta_p,g_acc);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL30:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_blas_dgemv(CblasNoTrans, 1.0, aa, du_i,0.0,tempv);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL31:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_vector_add(delta_p,tempv);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL32:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_blas_dgemv(CblasNoTrans, 1.0, bb, d2u_i,0.0,tempv);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL33:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_vector_add(delta_p,tempv);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL34:" << gsl_strerror(gsl_status);
		return -1;
	}
	//delta_u=inv(k_bar)*delta_p;
	gsl_permutation_init(p);
	gsl_status = gsl_matrix_memcpy(tempm,k_bar);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL35:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_linalg_LU_decomp (tempm, p, &s);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL36:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_linalg_LU_solve (tempm, p, delta_p, delta_u);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL37:" << gsl_strerror(gsl_status);
		return -1;
	}
	//delta_du=gamma/(beta*dt)*delta_u - (gamma/beta)*du_i + dt*(1-gamma/(2*beta))*d2u_i;
	gsl_vector *delta_du = gsl_vector_calloc(dof);

	gsl_status = gsl_vector_memcpy(delta_du,delta_u);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL38:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_vector_scale(delta_du,gamma/(beta*dt));
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL39:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_vector_memcpy(tempv,du_i);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL40:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_vector_scale(tempv,gamma/beta);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL41:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_vector_sub(delta_du,tempv);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL42:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_vector_memcpy(tempv,d2u_i);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL43:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_vector_scale(tempv,dt*(1.0-gamma/(2*beta)));
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL44:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_vector_add(delta_du,tempv);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL45:" << gsl_strerror(gsl_status);
		return -1;
	}
	//delta_d2u=1/(beta*dt*dt)*delta_u - (1/beta*dt)*du_i -(1/(2*beta))*d2u_i;
	gsl_vector *delta_d2u = gsl_vector_calloc(dof);

	gsl_status = gsl_vector_memcpy(delta_d2u,delta_u);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL46:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_vector_scale(delta_d2u,1.0/(beta*dt*dt));
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL47:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_vector_memcpy(tempa,du_i);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL48:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_vector_scale(tempa,1.0/(beta*dt));
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL49:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_vector_sub(delta_d2u,tempa);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL50:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_vector_memcpy(tempa,d2u_i);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL51:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_vector_scale(tempa,1.0/(2*beta));
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL52:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_vector_sub(delta_d2u,tempa);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL53:" << gsl_strerror(gsl_status);
		return -1;
	}
	//u_j=u_i+delta_u;
	gsl_status = gsl_vector_memcpy(u_j,u_i);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL54:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_vector_add(u_j,delta_u);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL55:" << gsl_strerror(gsl_status);
		return -1;
	}
	
	//du_j=du_i+delta_du;
	gsl_status = gsl_vector_memcpy(du_j,du_i);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL59:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_vector_add(du_j,delta_du);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL60:" << gsl_strerror(gsl_status);
		return -1;
	}
	//d2u_j=d2u_i+delta_d2u;
	gsl_status = gsl_vector_memcpy(d2u_j,d2u_i);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL61:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_vector_add(d2u_j,delta_d2u);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL62:" << gsl_strerror(gsl_status);
		return -1;
	}
	//fs_j=fs_i+K(1:Num;1:Num)*delta_u(1:Num)
	gsl_matrix *tempk = gsl_matrix_calloc(Num,Num);
	gsl_vector *tempu = gsl_vector_calloc(Num);
	gsl_vector *tempv3 = gsl_vector_calloc(Num);
	for (int j=1;j<=Num;j++){
		gsl_vector_set(tempu,j-1,gsl_vector_get(delta_u,j-1));
		for (int i=1;i<=Num;i++){
			gsl_matrix_set(tempk,j-1,i-1,gsl_matrix_get(K,j-1,i-1));
		}
	}
	gsl_status = gsl_blas_dgemv(CblasNoTrans, 1.0, tempk,tempu,0.0,tempv3);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL56:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_vector_memcpy(fs_j,fs_i);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL57:" << gsl_strerror(gsl_status);
		return -1;
	}
	gsl_status = gsl_vector_add(fs_j,tempv3);
	if (gsl_status) {  
		rCritical(threadID) << "Error in GSL58:" << gsl_strerror(gsl_status);
		return -1;
	}

	gsl_permutation_free (p);
	gsl_matrix_free(k_bar);
	gsl_matrix_free(tempm);
	gsl_matrix_free(aa);
	gsl_matrix_free(bb);
	gsl_vector_free(tempv);
	gsl_vector_free(tempa);
	gsl_vector_free(tempu);
	gsl_matrix_free(tempk);
	gsl_vector_free(tempv2);
	gsl_vector_free(tempv3);
	gsl_vector_free(rhsv);
	gsl_vector_free(delta_p);
	gsl_vector_free(delta_u);
	gsl_vector_free(delta_du);
	gsl_vector_free(delta_d2u);
	return 0;
}

// ------------------------------------------ Newmark Beta MDOF Linear End

// ------------------------------------------ Lateral force distribution
//[dist]=lateralforcedistribution(Tn,Num);
int RSoilStructureInteractionModel::lateralforcedistribution(double Tn, int Num, gsl_vector *dist, int threadID)
{
	int gsl_status;
	gsl_vector *H = gsl_vector_calloc(Num);
	//ASCE7
	if (theShearDistributionMethod == RSoilStructureInteractionModel::ASCE7){
		double kapa;
		if (Tn<=0.5){
			kapa=1;
		}else if(Tn>=2.5){
			kapa=2;
		}else{
			kapa=0.5*Tn+0.75;
		}
		double Sigma=0.0;
		for (int i=1;i<=Num;i++){
			gsl_vector_set(H,i-1,i);	//H is height of the story with respect to the base --> Not dependent to value of h_story so it assumes 1
			//Attention: weight of stories are equal
			Sigma = Sigma + pow(gsl_vector_get(H,i-1),kapa);
		}
		for (int i=1;i<=Num;i++){
		gsl_vector_set(dist,i-1,pow(gsl_vector_get(H,i-1),kapa)/Sigma);
		}
	}
	//NBCC
	if (theShearDistributionMethod == RSoilStructureInteractionModel::NBCC){
		double Ft;
		if (Tn<=0.7){
			Ft=0.0;
		}else if(Tn>=3.6){
			Ft=0.25;
		}else{
			Ft=0.07*Tn;
		}
		double Sigma=0.0;
		for (int i=1;i<=Num;i++){
			gsl_vector_set(H,i-1,i);	//H is height of the story with respect to the base --> Not dependent to value of h_story so it assumes 1
			//Attention: weight of stories are equal
			Sigma = Sigma + gsl_vector_get(H,i-1);
		}
		for (int i=1;i<=Num;i++){
		gsl_vector_set(dist,i-1,(1-Ft)*gsl_vector_get(H,i-1)/Sigma);
		}
		double a=gsl_vector_get(dist,Num-1);
		gsl_vector_set(dist,Num-1,a+Ft);
	}
	//Mexico
	if (theShearDistributionMethod == RSoilStructureInteractionModel::MexicanCode){	
		double Tc=4.2;	//for soil type D
		if (getDisplayOutput()){
			rDebug(threadID) << "Zone type is considered as D" << endl;
		}
		if (Tn<=Tc){
			double Sigma=0.0;
			for (int i=1;i<=Num;i++){
				gsl_vector_set(H,i-1,i);	//H is height of the story with respect to the base --> Not dependent to value of h_story so it assumes 1
				//Attention: weight of stories are equal
				Sigma = Sigma + gsl_vector_get(H,i-1);
			}
			for (int i=1;i<=Num;i++){
				gsl_vector_set(dist,i-1,gsl_vector_get(H,i-1)/Sigma);
			}
		}else{
			if (getDisplayOutput()){
			rDebug(threadID) << "Q (seimic behavior factor) is considered as 1 to be conservative. Then Q' is 1 too." << endl;
			}
			int r=2;
			double v1=1-r*(1-pow(Tc/Tn,r));
			double v2=1.5*r*(1-pow(Tc/Tn,r));	
			double Sigma1=0.0;
			double Sigma2=0.0;
			for (int i=1;i<=Num;i++){
				gsl_vector_set(H,i-1,i);	//H is height of the story with respect to the base --> Not dependent to value of h_story so it assumes 1
				//Attention: weight of stories are equal
				Sigma1 = Sigma1 + gsl_vector_get(H,i-1);
				Sigma2 = Sigma2 + pow(gsl_vector_get(H,i-1),2);
			}
			for (int i=1;i<=Num;i++){
				gsl_vector_set(dist,i-1,(v1*gsl_vector_get(H,i-1)/Sigma1)+(v2*pow(gsl_vector_get(H,i-1),2)/Sigma2));
			}
		}	
	}
	//Eurocode
	if (theShearDistributionMethod == RSoilStructureInteractionModel::Eurocode){
		double Sigma=0.0;
		for (int i=1;i<=Num;i++){
			gsl_vector_set(H,i-1,i);	//H is height of the story with respect to the base --> Not dependent to value of h_story so it assumes 1
			//Attention: weight of stories are equal
			Sigma = Sigma + gsl_vector_get(H,i-1);
		}
		for (int i=1;i<=Num;i++){
		gsl_vector_set(dist,i-1,gsl_vector_get(H,i-1)/Sigma);
		}
	}

	//freeing allocated memory
	gsl_vector_free(H);
	return 0;
}
// ------------------------------------------ Lateral force distribution force End

bool RSoilStructureInteractionModel::canBeRunParallelly() {
	return true;
}