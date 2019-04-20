// Last modified by Mojtaba on June 10, 2014

#include "RDomain.h"

#include <QMessageBox>
#include <QApplication>
#include <QMetaProperty>
#include <QDir>

#include "RMainWindow.h"

// PARAMETERS AND FUNCTION //
#include "RLocation.h"
#include "RFunction.h"
#include "RConstant.h"
#include "RResponse.h"
#include "RFileResponse.h"
#include "RCommandResponse.h"
#include "RGenericResponse.h"
#include "RLocationResponse.h"
#include "RDatabaseResponse.h"
#include "RDecisionVariable.h"
#include "RContinuousDecisionVariable.h"
#include "RRandomVariable.h"
#include "RContinuousRandomVariable.h"
#include "RCorrelation.h"
#include "RTime.h"


// MODEL
#include "RModel.h"


//OCCURRENCE MODELS
#include "RPoissonPulseProcessModel.h"


//EVENT MODELS
#include "ROperationEventType1Model.h"
#include "ROperationEventType2Model.h"

#include "RTimeEventType1Model.h"
#include "RTimeEventType2Model.h"



// HAZARD MODELS
#include "RCircularAreaSourceModel.h"
#include "RRectangularAreaSourceModel.h"
#include "RPolygonalAreaSourceModel.h"	
#include "RSingleLineSourceModel.h"
#include "RMultipointLineSourceModel.h"
#include "RMomentMagnitudeModel.h"
#include "RBoundedExponentialMagnitudeModel.h"
#include "RGenericAttenuationModel.h"

#include "RJoynerBoore1981IntensityModel.h"
#include "RAtkinson1997IntensityModel.h"
#include "RSimpleWindVelocityModel.h"
#include "RSimpleWindPressureModel.h"
#include "RSimpleSnowLoadModel.h"
#include "RQuadrilateralAreaSourceModel.h"
#include "RBooreAtkinson2008IntensityModel.h"	
#include "RAtkinsonBoore2003IntensityModel.h"
#include "RDuWang2012IntensityModel.h"
#include "RCampbellBozorgnia2008IntensityModel.h"
#include "RAbrahamsonSilva2008IntensityModel.h"
#include "RChiouYoungs2008IntensityModel.h"
#include "RBooreEtAl2013IntensityModel.h"
#include "RAbrahamsonEtAl2013IntensityModel.h"	
#include "RIdriss2013IntensityModel.h"
#include "RCampbellBozorgnia2013IntensityModel.h"
#include "RChiouYoungs2013IntensityModel.h"
#include "RRezaeianDerKiureghianArtificialGroundMotionModel.h"
#include "RChiouYoungs2013IntensityModel.h"





// INFRASTRUCTURE MODELS
#include "RStModel.h"
#include "RBuildingInformationModel.h"
#include "RBuildingResponseModel.h"
#include "RSimpleDeteriorationModel.h"
#include "RSoilStructureInteractionModel.h"
#include "RGypsumWallboardPartitionDamageModel.h"
#include "RSuspendedCeilingDamageModel.h"
#include "RTractionElevatorDamageModel.h"
#include "RConcreteSpecialMomentFrameInteriorJointDamageModel.h"
#include "RConcreteIntermediateMomentFrameInteriorJointDamageModel.h"
#include "RConcreteMomentFrameExteriorJointDamageModel.h"
#include "RCurtainWallDamageModel.h"
#include "RComponentLossModel.h"
#include "RInjurySocialLossModel.h"
#include "RPopulationDensityModel.h"

// INFRASTRUCTURE MODELS/ BUILDINGS MODELS
#include "RBuildingInspectionTimeModel.h"
#include "RBuildingInspectionCostModel.h"
#include "RBuildingRepairMobilizationTimeModel.h"
#include "RBuildingRepairMobilizationCostModel.h"
#include "RBuildingRepairTimeModel.h"
#include "RBuildingRestorationModel.h"
#include "RBuildingUtilityModel.h"
#include "RBuildingCasualtyModel.h"
#include "RBuildingSearchAndRescueTimeModel.h"
#include "RBuildingSearchAndRescueCostModel.h"
#include "RBuildingFatalityCostModel.h"
#include "RBuildingRelocationModel.h"

// INFRASTRUCTURE/COMMERCIAL MODELS
#include "RCommercialInformationModel.h"
#include "RCommercialLossModel.h"	

// INFRASTRUCTURE/POWER MODELS
#include "RPowerStationServiceabilityModel.h"
#include "RPowerStationDamageModel.h"
#include "RPowerStationRepairTimeModel.h"
#include "RPowerStationRepairCostModel.h"
#include "RPowerStationInformationModel.h"
#include "RPowerStationInspectionTimeModel.h"
#include "RPowerStationInspectionCostModel.h"

// INFRASTRUCTURE/WATER MODELS
#include "RWaterStationServiceabilityModel.h"
#include "RWaterStationDamageModel.h"
#include "RWaterStationRepairTimeModel.h"
#include "RWaterStationRepairCostModel.h"
#include "RWaterStationInformationModel.h"
#include "RWaterStationFunctionalityModel.h"
#include "RWaterStationInspectionTimeModel.h"
#include "RWaterStationInspectionCostModel.h"

// INFRASTRUCTURE/HEALTHCARE MODELS
#include "RHealthcareCapacityModel.h"
#include "RHealthcareInformationModel.h"
#include "RHealthcareRestorationModel.h"
#include "RHealthcareDemandModel.h"
#include "RHealthcareTreatmentCostModel.h"
#include "RHealthcareTreatmentTimeModel.h"

// CONSEQUENCE MODELS
#include "RSimpleDamageCurveModel.h"
#include "RSimpleBuildingRepairCostModel.h"
#include "RFemaNibsBridgeDamageModel.h"
#include "RRegionalLossModel.h"
#include "RFemaNibsBuildingRepairCostModel.h"
#include "RDiscountingModel.h"
#include "RBuildingDamageModel.h"
#include "RBuildingRepairCostModel.h"
#include "RMasonryBuildingDamageModel.h"
#include "RMasonryBuildingRetrofitCostModel.h" 

#include "RRelocationCostModel.h" 
#include "RLifeQualityReductionModel.h" 





// ENVIRONMENTAL MODELS
#include "RSimpleBuildingLifeCycleWaterUsageModel.h"
#include "RSimpleBuildingLifeCycleGWPModel.h"
#include "RSimpleBuildingLifeCycleEnergyUsageModel.h"
#include "RDetailedBuildingLifeCycleWaterUsageModel.h"
#include "RDetailedBuildingLifeCycleGWPModel.h"
#include "RDetailedBuildingLifeCycleEnergyUsageModel.h"







// EXTERNAL SOFTWARE
#include "RMatlabModel.h"
#include "ROpenSeesModel.h"
#include "RANSYSModel.h"
#include "RAbaqusModel.h"
#include "RUSFOSModel.h"
#include "REMMEModel.h"
#include "RSAP2000Model.h"





// GENERIC MODELS
#include "RAlgebraicExpressionModel.h"
#include "RRandomVariableWithRandomParametersModel.h"
#include "RRootFindingModel.h"
#include "RScriptModel.h"





/*// UNOFFICIAL MODELS
#include "RFemaNibsBuildingDamage1Model.h"
#include "RFemaNibsBuildingDamage2Model.h"
#include "RFemaNibsBuildingDamage3Model.h"
#include "RBridgeFailureModel.h"
#include "RDamageCurveModel.h"
#include "RBuildingCostModel.h"
#include "RTransportationAccessibilityModel.h"
#include "RTransportationConsequenceReaderModel.h"
#include "RLocalInterpolationModel.h"
#include "RAtkinsonSaIntensityGeneratorModel.h"	
#include "RFemaNibsBuildingDamage3GeneratorModel.h"
#include "RFemaNibsBuildingDamage4GeneratorModel.h"
#include "RFemaNibsBuildingDamage5Model.h"
#include "RFemaNibsBuildingDamage6VerificationModel.h"
#include "RFemaNibsBuildingDamage7VerificationModel.h"
#include "RSimplifiedBuildingInformationModel.h"
#include "RSimplifiedBuildingEarthquakeResponseModel.h"
#include "RSimplifiedBuildingEarthquakeDamageModel.h"
#include "RSimplifiedBuildingEarthquakeLossModel.h"
#include "RRegionModel.h"	
#include "RMatlabModel.h"
#include "RFemaNibsBuildingDataGeneratorModel.h"
#include "RBuildingModel.h"*/





// ANALYSIS TOOLS
#include "RStandardConvergenceChecker.h"
#include "RStepperNonlinSingleConstrSolver.h"
#include "RHLRFStepDirectionSearcher.h"
#include "RFixedStepSizeSearcher.h"
#include "RNoCorrelationTransformer.h"
#include "RArmijoStepSizeSearcher.h"
#include "RNatafTransformer.h"
#include "RRandomNumberGenerator.h"
#include "RIndependentNormalRandomNumberGenerator.h"
#include "RAccumulator.h"
#include "RFailureProbabilityAccumulator.h"
#include "RHistogramAccumulator.h"
#include "RAdkZhangMeritChecker.h"
#include "RScenarioSamplingAnalyzer.h"
#include "RFunctionEvaluationAnalyzer.h"
#include "RFORMAnalyzer.h"
#include "RSORMAnalyzer.h"
#include "RFOSMAnalyzer.h"
#include "RSamplingAnalyzer.h"
#include "RLoadCoincidenceAnalyzer.h"
#include "RDiscreteEventSamplingAnalyzer.h"




// St classes
#include "SNode.h"
#include "STrussElement.h"
#include "SFrameElement.h"
#include "SElement.h"
#include "SMaterial.h"
#include "SSection.h"
#include "SGenericSection.h"
#include "SRectangularSection.h"
#include "SNodalLoad.h"
#include "SFixedNode.h"
#include "SRigidLink.h"
#include "SQuad4Element.h"
#include "SMember.h"
#include "SWallMember.h"
#include "SLUSolver.h"
#include "STransformationMatrixAssembler.h"

#include "RParallelComputationManager.h"
#include "ROutputManager.h"

/* ----------------------------- Hash and switch case ------------------------------------------- */

QHash<QString,ClassName> RDomain::classNamesHash;
bool RDomain::classNamesAddedToHash = false;

/* -------------------------------------------------------------------------------------------------- */


RDomain::RDomain(QObject *parent, QString name)
: RObject(parent, name)
{
	theLastAddedObject = 0;
	isInputFileBeingOpened = false;
	theOutputByteArray = 0;
	if (qobject_cast<QApplication*>(QCoreApplication::instance()) == 0) {
		theOutputByteArray = new QByteArray();
	}

	// adding defined classes' name to hash
	if (!classNamesAddedToHash){
		addClassNamesToHash();
		classNamesAddedToHash = true;
	}

	theNumberOfOngoingAnalysis = 0;

	// telling parallel computation manager that a new RDomain is created
	RParallelComputationManager::getInstance()->domainIsReset();
}

RDomain::~RDomain()
{
}

int RDomain::createObject(QString name, QString passedClassName, bool isAutoGenerated)
{
	int namingResult = 0;
	if (!isInputFileBeingOpened) {
		namingResult = nameIsValid(name);
	}

	// getting the value corresponding to string passed as className without its space
	QString className = passedClassName.remove(' ');
	ClassName classEnum = classNamesHash.value(className,NoValue_CN);

	// if it does not exist in the trie, searching for the className with lowerCase characters
	if (classEnum == NoValue_CN){
		className = className.toLower();
		classEnum = classNamesHash.value(className,NoValue_CN);
	}

	if (namingResult == 0) {

		// CREATING THE OBJECT
		switch(classEnum){

			// PARAMETERS AND FUNCTION
			case RLocation_CN : {
				theLastAddedObject = new RLocation(this, name);
				break;
			}
			case RConstant_CN : {
				theLastAddedObject = new RConstant(this, name);
				break;
			}
			case RTime_CN : {
				theLastAddedObject = new RTime(this, name);
				break;
			}
			case RFileResponse_CN : {
				theLastAddedObject = new RFileResponse(this, name);
				break;
			}
			case RCommandResponse_CN : {
				theLastAddedObject = new RCommandResponse(this, name);
				break;
			}
			case RGenericResponse_CN : {
				theLastAddedObject = new RGenericResponse(this, name);
				break;
			}
			case RDatabaseResponse_CN : {
				theLastAddedObject = new RDatabaseResponse(this, name);
				break;
			}
			case RLocationResponse_CN : {
				theLastAddedObject = new RLocationResponse(this, name);
				break;
			}
			case RContinuousDecisionVariable_CN : {
				theLastAddedObject = new RContinuousDecisionVariable(this, name);
				break;
			}
			case RContinuousRandomVariable_CN : {
				theLastAddedObject = new RContinuousRandomVariable(this, name);
				break;
			}
			case RCorrelation_CN : {
				theLastAddedObject = new RCorrelation(this, name);
				break;
			}
			case RFunction_CN : {
				theLastAddedObject = new RFunction(this, name);
				break;
			}

			 // EVENT MODELS
			case ROperationEventType1Model_CN: {
				theLastAddedObject = new ROperationEventType1Model(this, name);
				break;
			}
			case ROperationEventType2Model_CN: {
				theLastAddedObject = new ROperationEventType2Model(this, name);
				break;
			}
			case RTimeEventType1Model_CN: {
				theLastAddedObject = new RTimeEventType1Model(this, name);
				break;
			}
			case RTimeEventType2Model_CN: {
				theLastAddedObject = new RTimeEventType2Model(this, name);
				break;
			}

			// OCCURRENCE MODELS
			case RPoissonPulseProcessModel_CN : {
				theLastAddedObject = new RPoissonPulseProcessModel(this, name);
				break;
			}


			// HAZARD MODELS
			case RSingleLineSourceModel_CN : {
				theLastAddedObject = new RSingleLineSourceModel(this, name);
				break;
			}
			case RMultipointLineSourceModel_CN : {
				theLastAddedObject = new RMultipointLineSourceModel(this, name);
				break;
			}
			case RCircularAreaSourceModel_CN : {
				theLastAddedObject = new RCircularAreaSourceModel(this, name);
				break;
			}
			case RRectangularAreaSourceModel_CN : {
				theLastAddedObject = new RRectangularAreaSourceModel(this, name);
				break;
			}
			case RQuadrilateralAreaSourceModel_CN : {
				theLastAddedObject = new RQuadrilateralAreaSourceModel(this, name);
				break;
			}
			case RPolygonalAreaSourceModel_CN : {
				theLastAddedObject = new RPolygonalAreaSourceModel(this, name);
				break;
			}
			case RMomentMagnitudeModel_CN : {
				theLastAddedObject = new RMomentMagnitudeModel(this, name);
				break;
			}
			case RBoundedExponentialMagnitudeModel_CN : {
				theLastAddedObject = new RBoundedExponentialMagnitudeModel(this, name);
				break;
			}
			case RGenericAttenuationModel_CN : {
				theLastAddedObject = new RGenericAttenuationModel(this, name);
				break;
			}
			case RJoynerBoore1981IntensityModel_CN : {
				theLastAddedObject = new RJoynerBoore1981IntensityModel(this, name);
				break;
			}
			case RAtkinson1997IntensityModel_CN : {
				theLastAddedObject = new RAtkinson1997IntensityModel(this, name);
				break;
			}
			case RBooreAtkinson2008IntensityModel_CN : {
				theLastAddedObject = new RBooreAtkinson2008IntensityModel(this, name);
				break;
			}
			case RAtkinsonBoore2003IntensityModel_CN : {
				theLastAddedObject = new RAtkinsonBoore2003IntensityModel(this, name);
				break;
			}
			case RDuWang2012IntensityModel_CN : {
				theLastAddedObject = new RDuWang2012IntensityModel(this, name);
				break;
			}
			case RCampbellBozorgnia2008IntensityModel_CN : {
				theLastAddedObject = new RCampbellBozorgnia2008IntensityModel(this, name);
				break;
			}
			case RAbrahamsonSilva2008IntensityModel_CN : {
				theLastAddedObject = new RAbrahamsonSilva2008IntensityModel(this, name);
				break;
			}
			case RChiouYoungs2008IntensityModel_CN : {
				theLastAddedObject = new RChiouYoungs2008IntensityModel(this, name);
				break;
			}
			case RBooreEtAl2013IntensityModel_CN : {
				theLastAddedObject = new RBooreEtAl2013IntensityModel(this, name);
				break;
			}
			case RAbrahamsonEtAl2013IntensityModel_CN : {
				theLastAddedObject = new RAbrahamsonEtAl2013IntensityModel(this, name);
				break;
			}
			case RIdriss2013IntensityModel_CN : {
				theLastAddedObject = new RIdriss2013IntensityModel(this, name);
				break;
			}
			case RCampbellBozorgnia2013IntensityModel_CN : {
				theLastAddedObject = new RCampbellBozorgnia2013IntensityModel(this, name);
				break;
			}
			case RChiouYoungs2013IntensityModel_CN : {
				theLastAddedObject = new RChiouYoungs2013IntensityModel(this, name);
				break;
			}
			case RRezaeianDerKiureghianArtificialGroundMotionModel_CN :{
				theLastAddedObject = new RRezaeianDerKiureghianArtificialGroundMotionModel(this, name);
				break;
			}
			case RSimpleWindVelocityModel_CN : {
				theLastAddedObject = new RSimpleWindVelocityModel(this, name);
				break;
			}
			case RSimpleWindPressureModel_CN : {
				theLastAddedObject = new RSimpleWindPressureModel(this, name);
				break;
			}
			case RSimpleSnowLoadModel_CN : {
				theLastAddedObject = new RSimpleSnowLoadModel(this, name);
				break;
			}


			// INFRASTRUCTURE MODELS
			case RStModel_CN : {
				theLastAddedObject = new RStModel(this, name);
				break;
			}		
			case RBuildingInformationModel_CN : {
				theLastAddedObject = new RBuildingInformationModel(this, name);
				break;
			}		
			case RBuildingResponseModel_CN : {
				theLastAddedObject = new RBuildingResponseModel(this, name);
				break;
			}		
			case RSimpleDeteriorationModel_CN : {
				theLastAddedObject = new RSimpleDeteriorationModel(this, name);
				break;
			}
			case RSoilStructureInteractionModel_CN : {
				theLastAddedObject = new RSoilStructureInteractionModel(this, name);
				break;
			}

			 // INFRASTRUCTURE/BUILDING MODELS
			case RBuildingInspectionTimeModel_CN: {
				theLastAddedObject = new RBuildingInspectionTimeModel(this, name);
				break;
			}
			case RBuildingInspectionCostModel_CN: {
				theLastAddedObject = new RBuildingInspectionCostModel(this, name);
				break;
			}
			case RBuildingRepairMobilizationTimeModel_CN: {
				theLastAddedObject = new RBuildingRepairMobilizationTimeModel(this, name);
				break;
			}
			case RBuildingRepairMobilizationCostModel_CN: {
				theLastAddedObject = new RBuildingRepairMobilizationCostModel(this, name);
				break;
			}
			case RBuildingRepairTimeModel_CN: {
				theLastAddedObject = new RBuildingRepairTimeModel(this, name);
				break;
			}
			case RBuildingRestorationModel_CN: {
				theLastAddedObject = new RBuildingRestorationModel(this, name);
				break;
			}
			case RBuildingUtilityModel_CN: {
				theLastAddedObject = new RBuildingUtilityModel(this, name);
				break;
			}
			case RBuildingCasualtyModel_CN: {
				theLastAddedObject = new RBuildingCasualtyModel(this, name);
				break;
			}
			case RBuildingSearchAndRescueTimeModel_CN: {
				theLastAddedObject = new RBuildingSearchAndRescueTimeModel(this, name);
				break;
			}
			case RBuildingSearchAndRescueCostModel_CN: {
				theLastAddedObject = new RBuildingSearchAndRescueCostModel(this, name);
				break;
			}
			case RBuildingFatalityCostModel_CN: {
				theLastAddedObject = new RBuildingFatalityCostModel(this, name);
				break;
			}
			case RBuildingRelocationModel_CN: {
				theLastAddedObject = new RBuildingRelocationModel(this, name);
				break;
			}

			// INFRASTRUCTURE/COMMERCIAL MODELS
			case RCommercialInformationModel_CN: {
				theLastAddedObject = new RCommercialInformationModel(this, name);
				break;
			}
			case RCommercialLossModel_CN: {
				theLastAddedObject = new RCommercialLossModel(this, name);
				break;
			}

			// INFRASTRUCTURE/POWER MODELS
			case RPowerStationDamageModel_CN: {
				theLastAddedObject = new RPowerStationDamageModel(this, name);
				break;
			}
			case RPowerStationInformationModel_CN: {
				theLastAddedObject = new RPowerStationInformationModel(this, name);
				break;
			}
			case RPowerStationInspectionTimeModel_CN: {
				theLastAddedObject = new RPowerStationInspectionTimeModel(this, name);
				break;
			}
			case RPowerStationInspectionCostModel_CN: {
				theLastAddedObject = new RPowerStationInspectionCostModel(this, name);
				break;
			}
			case RPowerStationServiceabilityModel_CN: {
				theLastAddedObject = new RPowerStationServiceabilityModel(this, name);
				break;
			}
			case RPowerStationRepairTimeModel_CN: {
				theLastAddedObject = new RPowerStationRepairTimeModel(this, name);
				break;
			}
			case RPowerStationRepairCostModel_CN: {
				theLastAddedObject = new RPowerStationRepairCostModel(this, name);
				break;
			}

			// INFRASTRUCTURE/WATER MODELS
			case RWaterStationDamageModel_CN: {
				theLastAddedObject = new RWaterStationDamageModel(this, name);
				break;
			}
			case RWaterStationInformationModel_CN: {
				theLastAddedObject = new RWaterStationInformationModel(this, name);
				break;
			}
			case RWaterStationInspectionTimeModel_CN: {
				theLastAddedObject = new RWaterStationInspectionTimeModel(this, name);
				break;
			}
			case RWaterStationInspectionCostModel_CN: {
				theLastAddedObject = new RWaterStationInspectionCostModel(this, name);
				break;
			}
			case RWaterStationServiceabilityModel_CN: {
				theLastAddedObject = new RWaterStationServiceabilityModel(this, name);
				break;
			}
			case RWaterStationFunctionalityModel_CN: {
				theLastAddedObject = new RWaterStationFunctionalityModel(this, name);
				break;
			}
			case RWaterStationRepairTimeModel_CN: {
				theLastAddedObject = new RWaterStationRepairTimeModel(this, name);
				break;
			}
			case RWaterStationRepairCostModel_CN: {
				theLastAddedObject = new RWaterStationRepairCostModel(this, name);
				break;
			}

			// INFRASTRUCTURE/HEALTHCARE MODELS
			case RHealthcareCapacityModel_CN: {
				theLastAddedObject = new RHealthcareCapacityModel(this, name);
				break;
			}
			case RHealthcareInformationModel_CN: {
				theLastAddedObject = new RHealthcareInformationModel(this, name);
				break;
			}
			case RHealthcareRestorationModel_CN: {
				theLastAddedObject = new RHealthcareRestorationModel(this, name);
				break;
			}
			case RHealthcareDemandModel_CN: {
				theLastAddedObject = new RHealthcareDemandModel(this, name);
				break;
			}
			case RHealthcareTreatmentCostModel_CN: {
				theLastAddedObject = new RHealthcareTreatmentCostModel(this, name);
				break;
			}
			case RHealthcareTreatmentTimeModel_CN: {
				theLastAddedObject = new RHealthcareTreatmentTimeModel(this, name);
				break;
			}

			// CONSEQUENCE MODELS
			case RSimpleDamageCurveModel_CN : {
				theLastAddedObject = new RSimpleDamageCurveModel(this, name);
				break;
			}
			case RSimpleBuildingRepairCostModel_CN : {
				theLastAddedObject = new RSimpleBuildingRepairCostModel(this, name);
				break;
			}
			case RFemaNibsBridgeDamageModel_CN : {
				theLastAddedObject = new RFemaNibsBridgeDamageModel(this, name);
				break;
			}		
			case RFemaNibsBuildingRepairCostModel_CN : {
				theLastAddedObject = new RFemaNibsBuildingRepairCostModel(this, name);
				break;
			}
			case RRegionalLossModel_CN : {
				theLastAddedObject = new RRegionalLossModel(this, name);
				break;
			}
			case RDiscountingModel_CN : {
				theLastAddedObject = new RDiscountingModel(this, name);
				break;
			}		
			case RBuildingDamageModel_CN : {
				theLastAddedObject = new RBuildingDamageModel(this, name);
				break;
			}		
			case RBuildingRepairCostModel_CN : {
				theLastAddedObject = new RBuildingRepairCostModel(this, name);
				break;
			}
			case RMasonryBuildingDamageModel_CN : {
				theLastAddedObject = new RMasonryBuildingDamageModel(this, name);
				break;
			}
			case RMasonryBuildingRetrofitCostModel_CN : {
				theLastAddedObject = new RMasonryBuildingRetrofitCostModel(this, name);
				break;
			}
			case RSimpleBuildingLifeCycleEnergyUsageModel_CN : {
				theLastAddedObject = new RSimpleBuildingLifeCycleEnergyUsageModel(this, name);
				break;
			}
			case RDetailedBuildingLifeCycleEnergyUsageModel_CN : {
				theLastAddedObject = new RDetailedBuildingLifeCycleEnergyUsageModel(this, name);
				break;
			}
			case RSimpleBuildingLifeCycleWaterUsageModel_CN : {
				theLastAddedObject = new RSimpleBuildingLifeCycleWaterUsageModel(this, name);
				break;
			}
			case RDetailedBuildingLifeCycleWaterUsageModel_CN : {
				theLastAddedObject = new RDetailedBuildingLifeCycleWaterUsageModel(this, name);
				break;
			}
			case RSimpleBuildingLifeCycleGWPModel_CN : {
				theLastAddedObject = new RSimpleBuildingLifeCycleGWPModel(this, name);
				break;
			}
			case RDetailedBuildingLifeCycleGWPModel_CN : {
				theLastAddedObject = new RDetailedBuildingLifeCycleGWPModel(this, name);
				break;
			}
				// New
			case RGypsumWallboardPartitionDamageModel_CN: {
				theLastAddedObject = new RGypsumWallboardPartitionDamageModel(this, name);
				break;
			}
			case RSuspendedCeilingDamageModel_CN: {
				theLastAddedObject = new RSuspendedCeilingDamageModel(this, name);
				break;
			}
			case RTractionElevatorDamageModel_CN: {
				theLastAddedObject = new RTractionElevatorDamageModel(this, name);
				break;
			}
			case RConcreteSpecialMomentFrameInteriorJointDamageModel_CN: {
				theLastAddedObject = new RConcreteSpecialMomentFrameInteriorJointDamageModel(this, name);
				break;
			}
			case RConcreteIntermediateMomentFrameInteriorJointDamageModel_CN: {
				theLastAddedObject = new RConcreteIntermediateMomentFrameInteriorJointDamageModel(this, name);
				break;
			}
			case RConcreteMomentFrameExteriorJointDamageModel_CN: {
				theLastAddedObject = new RConcreteMomentFrameExteriorJointDamageModel(this, name);
				break;
			}
			case RCurtainWallDamageModel_CN: {
				theLastAddedObject = new RCurtainWallDamageModel(this, name);
				break;
			}
			case RComponentLossModel_CN: {
				theLastAddedObject = new RComponentLossModel(this, name);
				break;
			}
				// Social
			case RInjurySocialLossModel_CN: {
				theLastAddedObject = new RInjurySocialLossModel(this, name);
				break;
			}
			case RPopulationDensityModel_CN: {
				theLastAddedObject = new RPopulationDensityModel(this, name);
				break;
			}
			case RRelocationCostModel_CN: {
				theLastAddedObject = new RRelocationCostModel(this, name);
				break;
			}
			case RLifeQualityReductionModel_CN: {
				theLastAddedObject = new RLifeQualityReductionModel(this, name);
				break;
			}

			// EXTERNAL SOFTWARE
			case RMatlabModel_CN :{
				theLastAddedObject = new RMatlabModel(this, name);
				break;
			}
			case ROpenSeesModel_CN : {
				theLastAddedObject = new ROpenSeesModel(this, name);
				break;
			}
			case RANSYSModel_CN : {
				theLastAddedObject = new RANSYSModel(this, name);
				break;
			}
			case RAbaqusModel_CN : {
				theLastAddedObject = new RAbaqusModel(this, name);
				break;
			}
			case RUSFOSModel_CN : {
				theLastAddedObject = new RUSFOSModel(this, name);
				break;
			}
			case REMMEModel_CN : {
				theLastAddedObject = new REMMEModel(this, name);
				break;
			}
			case RSAP2000Model_CN : {
				theLastAddedObject = new RSAP2000Model(this, name);
				break;
			}


			// GENERIC MODELS
			case RAlgebraicExpressionModel_CN : {
				theLastAddedObject = new RAlgebraicExpressionModel(this, name);
				break;
			}
			case RRandomVariableWithRandomParametersModel_CN : {
				theLastAddedObject = new RRandomVariableWithRandomParametersModel(this, name);
				break;
			}
			case RRootFindingModel_CN : {
				theLastAddedObject = new RRootFindingModel(this, name);
				break;
			}
			case RScriptModel_CN : {
				theLastAddedObject = new RScriptModel(this, name);
				break;
			}


			// ANALYSIS TOOLS
			case RStepperNonlinSingleConstrSolver_CN : {
				theLastAddedObject = new RStepperNonlinSingleConstrSolver(this, name);
				break;
			}
			case RFixedStepSizeSearcher_CN : {
				theLastAddedObject = new RFixedStepSizeSearcher(this, name);
				break;
			}
			case RArmijoStepSizeSearcher_CN : {
				theLastAddedObject = new RArmijoStepSizeSearcher(this, name);
				break;
			}
			case RHLRFStepDirectionSearcher_CN : {
				theLastAddedObject = new RHLRFStepDirectionSearcher(this, name);
				break;
			}
			case RPolakHeStepDirectionSearcher_CN : {
				//theLastAddedObject = new RPolakHeStepDirectionSearcher(this, name);
				break;
			}
			case RSQPStepDirectionSearcher_CN : {
				//theLastAddedObject = new RSQPStepDirectionSearcher(this, name);
				break;
			}
			case RNoCorrelationTransformer_CN : {
				theLastAddedObject = new RNoCorrelationTransformer(this, name);
				break;
			}
			case RNatafTransformer_CN : {
				theLastAddedObject = new RNatafTransformer(this, name);
				break;
			}
			case RStandardConvergenceChecker_CN : {
				theLastAddedObject = new RStandardConvergenceChecker(this, name);
				break;
			}
			case ROptimalityConditionConvergenceChecker_CN : {
				//theLastAddedObject = new ROptimalityConditionConvergenceChecker(this, name);
				break;
			}
			case RIndependentNormalRandomNumberGenerator_CN : {
				theLastAddedObject = new RIndependentNormalRandomNumberGenerator(this, name);
				break;
			}
			case RFailureProbabilityAccumulator_CN : {
				theLastAddedObject = new RFailureProbabilityAccumulator(this, name);
				break;
			}
			case RHistogramAccumulator_CN : {
				theLastAddedObject = new RHistogramAccumulator(this, name);
				break;
			}
			case RAdkZhangMeritChecker_CN : {
				theLastAddedObject = new RAdkZhangMeritChecker(this, name);
				break;
			}
			case RScenarioSamplingAnalyzer_CN : {
				theLastAddedObject = new RScenarioSamplingAnalyzer(this, name);
				break;
			}
			case RFunctionEvaluationAnalyzer_CN : {
				theLastAddedObject = new RFunctionEvaluationAnalyzer(this, name);
				break;
			}
			case RFORMAnalyzer_CN : {
				theLastAddedObject = new RFORMAnalyzer(this, name);
				break;
			}
			case RSORMAnalyzer_CN: {
				theLastAddedObject = new RSORMAnalyzer(this, name);
				break;
			}
			case RFOSMAnalyzer_CN : {
				theLastAddedObject = new RFOSMAnalyzer(this, name);
				break;
			}
			case RSamplingAnalyzer_CN : {
				theLastAddedObject = new RSamplingAnalyzer(this, name);
				break;
			}
			case RLoadCoincidenceAnalyzer_CN : {
				theLastAddedObject = new RLoadCoincidenceAnalyzer(this, name);
				break;
			}
			case RDiscreteEventSamplingAnalyzer_CN: {
				theLastAddedObject = new RDiscreteEventSamplingAnalyzer(this, name);
				break;
			}

			// St classes
			case SLUSolver_CN : {
				theLastAddedObject = new SLUSolver(this, name);
				break;
			}
			case STransformationMatrixAssembler_CN : {
				theLastAddedObject = new STransformationMatrixAssembler(this, name);
				break;
			}
			case SNode_CN : {
				theLastAddedObject = new SNode(this, name);
				break;
			}
			case SFixedNode_CN : {
				theLastAddedObject = new SFixedNode(this, name);
				break;
			}
			case SRigidLink_CN : {
				theLastAddedObject = new SRigidLink(this, name);
				break;
			}
			case SNodalLoad_CN : {
				theLastAddedObject = new SNodalLoad(this, name);
				break;
			}
			case STrussElement_CN : {
				theLastAddedObject = new STrussElement(this, name);
				break;
			}
			case SFrameElement_CN : {
				theLastAddedObject = new SFrameElement(this, name);
				break;
			}
			case SQuad4Element_CN : {
				theLastAddedObject = new SQuad4Element(this, name);
				break;
			}
			case SWallMember_CN : {
				theLastAddedObject = new SWallMember(this, name);
				break;
			}
			case SGenericSection_CN : {
				theLastAddedObject = new SGenericSection(this, name);
				break;
			}
			case SRectangularSection_CN : {
				theLastAddedObject = new SRectangularSection(this, name);
				break;
			}
			case SMaterial_CN : {
				theLastAddedObject = new SMaterial(this, name);
				break;
			}


			default : {

				rDebug() << "class not entered " << className << "\n";

				return -1;
			}
		}

		if (theLastAddedObject) {
			RObject *rObject = qobject_cast<RObject *>(theLastAddedObject);
			rObject->setAutoGenerated(isAutoGenerated);
			RParallelComputationManager::getInstance()->newObjectCreated();
		} else {
			return -1;
		}

		return 0;
	} else if (namingResult == -1) {
		// The name already exists in the domain.
		QString newName = name + "_New";
		//QMessageBox::warning(0, tr("Warning"), QString("An object with the name \"%1\" already exists. Rt will use the name \"%2\" instead.").arg(name).arg(newName), QMessageBox::Ok | QMessageBox::Default);
		rCritical() << "Warning: An object with the name" << name << "already exists. Rt will use the name" << newName << "instead.";
		int result = createObject(newName, className);
		if (result < 0 ) {
			return -1;
		}
		return 1;
	} else if (namingResult < -1) {
		// The name contains illegal characters.
		return -1;
	}
}

QStringList RDomain::getSubClasses(QString passedClassName)
{

	// checking if classes' name has been added to hash
	if (!classNamesAddedToHash){
		addClassNamesToHash();
		classNamesAddedToHash = true;
	}

	QStringList result;
	
	// getting Value corresponding to the string passed as the className
	QString className = passedClassName.remove(' ');  
	ClassName classEnum = classNamesHash.value(className,NoValue_CN);

	// BUILD THE OBJECTS PANE TREE

	switch (classEnum){

		// TOP LEVEL DOMAIN BRANCHES
		case RDomain_CN : {
			result << "Parameter" << "Function" << "Model";
			break;
		}

		// TOP LEVEL ANALYSIS BRANCHES
		case RAnalysis_CN : {
			result << "Analyzer" << "Nonlinear Single Constraint Solver" << "Step Size Searcher" << "Step Direction Searcher" << "Merit Checker" << "Transformer" << "Convergence Checker" << "Random Number Generator" << "Accumulator";
			break;
		}

		// PARAMETERS AND FUNCTION
		case RParameter_CN : {
			result << "Random Variable" << "Decision Variable" << "Constant" << "Response" << "Correlation" << "Location" << "Time";
			break;
		}
		case RParameterRandomVariable_CN : {
			result << "Continuous"/* << "Discrete"*/;
			break;
		}
		case RParameterDecisionVariable_CN : {
			result << "Continuous"/* << "Discrete"*/;
			break;
		}
		case RParameterResponse_CN : {
			result << "Generic" << "File" << "Database" << "Command" << "Location";
			break;
		}


		// TOP LEVEL MODEL BRANCHES
		case RModel_CN : {
			result << "Occurrence" << "Event" << "Hazard" << "Infrastructure" << "Consequence" << "External Software" << "Generic";// << "Unofficial";
			break;
		}

		// EVENT MODELS
		case RModelEvent_CN: {
			result << "Operation Type 1" << "Operation Type 2" << "Time Type 1" << "Time Type 2";
			break;
		}

		// OCCURRENCE MODELS
		case RModelOccurrence_CN : {
			result << "Poisson Pulse Process";
			break;
		}

		// HAZARD MODELS
		case RModelHazard_CN : {
			result << "Earthquake" << "Wind" << "Snow";
			break;
		}
		case RModelHazardEarthquake_CN : {
			result << "Magnitude" << "Location" << "Intensity";
			break;
		}
		case RModelHazardEarthquakeLocation_CN : {
			result << "Single Line Source" << "Multipoint Line Source" << "Rectangular Area Source" << "Quadrilateral Area Source" << "Circular Area Source" << "Polygonal Area Source";
			break;
		}
		case RModelHazardEarthquakeMagnitude_CN : {
			result << "Moment Magnitude" << "Bounded Exponential Magnitude";
			break;
		}
		case RModelHazardEarthquakeIntensity_CN : {
			result << "Generic Attenuation" << "Joyner Boore 1981 Intensity" << "Atkinson 1997 Intensity" << "Atkinson Boore 2003 Intensity" << "Boore Atkinson 2008 Intensity" << "Campbell Bozorgnia 2008 Intensity" << "Abrahamson Silva 2008 Intensity" << "Chiou Youngs 2008 Intensity" << "Du Wang 2012 Intensity" << "Boore et al 2013 Intensity" << "Abrahamson et al 2013 Intensity" << "Idriss 2013 Intensity" <<  "Campbell Bozorgnia 2013 Intensity" << "Chiou Youngs 2013 Intensity" << "Rezaeian DerKiureghian Artificial Ground Motion";
			break;
		}
		case RModelHazardWind_CN : {
			result << "Simple Wind Velocity" << "Simple Wind Pressure";
			break;
		}
		case RModelHazardSnow_CN : {
			result << "Simple Snow Load";
			break;
		}


		// INFRASTRUCTURE MODELS
		case RModelInfrastructure_CN : {
			result << "St" << "Building" << "Commercial" << "Power" << "Water" << "Healthcare";
			break;
		}

		 // INFRASTRUCTURE MODELS/ BUILDING
		case RModelInfrastructureBuilding_CN: {
			result << "Building Information" << "Building Response" << "Simple Deterioration" << "Soil Structure Interaction" << "Building Inspection Time" << "Building Inspection Cost" << "Building Search And Rescue Time" << "Building Search And Rescue Cost" << "Building Repair Mobilization Time" << "Building Repair Mobilization Cost" << "Building Restoration" << "Building Relocation" << "Building Repair Time" <<  "Building Utility" << "Building Casualty" << "Building Fatality Cost" ;
			break;
		}
		
		 // INFRASTRUCTURE MODELS/ BUILDING
		case RModelInfrastructureCommercial_CN: {
			result << "Commercial Information" << "Commercial Loss" ;
			break;
		}

		 // INFRASTRUCTURE/POWER MODELS
		case RModelInfrastructurePower_CN: {
			result << "Power Station Information" << "Power Station Inspection Time" << "Power Station Inspection Cost" << "Power Station Damage" << "Power Station Serviceability" << "Power Station Repair Time" << "Power Station Repair Cost";
			break;
		}

		  // INFRASTRUCTURE/WATER MODELS
		case RModelInfrastructureWater_CN: {
			result << "Water Station Information" << "Water Station Inspection Time" << "Water Station Inspection Cost" << "Water Station Damage" << "Water Station Functionality" << "Water Station Serviceability" << "Water Station Repair Time" << "Water Station Repair Cost";
			break;
		}

		// INFRASTRUCTURE/HEALTHCARE MODELS
		case RModelInfrastructureHealthcare_CN: {
			result << "Healthcare Information" << "Healthcare Capacity" << "Healthcare Demand" << "Healthcare Restoration" << "Healthcare Treatment Time" << "Healthcare Treatment Cost";
			break;
		}

		// CONSEQUENCE MODELS
		case RModelConsequence_CN : {
			result << "Economic" << "Environmental" << "Social";
			break;
		}
		case RModelConsequenceEconomic_CN : {
			result << "Simple Damage Curve" << "Simple Building Repair Cost" << "FEMA-NIBS Bridge Damage" << "FEMA-NIBS Building Repair Cost" << "Regional Loss" << "Discounting" << "Building Damage" << "Building Repair Cost" << "Masonry Building Damage" << "Masonry Building Retrofit Cost" << "Gypsum Wallboard Partition Damage" << "Suspended Ceiling Damage" << "Traction Elevator Damage" << "Concrete Special Moment Frame Interior Joint Damage" << "Concrete Intermediate Moment Frame Interior Joint Damage" << "Concrete Moment Frame Exterior Joint Damage" << "Curtain Wall Damage" << "Component Loss" << "Relocation Cost";
			break;
		}
		case RModelConsequenceEnvironmental_CN : {
			result << "Simple Building Life Cycle Energy Usage" << "Detailed Building Life Cycle Energy Usage" << "Simple Building Life Cycle Water Usage" << "Detailed Building Life Cycle Water Usage" << "Simple Building Life Cycle Global Warming Potential" << "Detailed Building Life Cycle Global Warming Potential";
			break;
		}
		case RModelConsequenceSocial_CN: {
			result << "Population Density" << "Injury Social Loss" << "Life Quality Reduction";
			break;
		}

		// EXTERNAL SOFTWARE
		case RModelExternalSoftware_CN : {
			result <<  "Matlab" << "OpenSees" << "ANSYS" << "Abaqus" << "SAP2000" << "USFOS" << "EMME";
			break;
		}

		// GENERIC MODELS
		case RModelGeneric_CN : {
			result << "Algebraic Expression" << "Random Variable With Random Parameters" << "Root Finding" << "Script";
			break;
		}

		// ANALYSIS TOOLS
		case RAnalyzer_CN : {
			result << "Function Evaluation" << "FOSM" << "FORM" << "SORM" << "Sampling" << "Load Coincidence" << "Scenario Sampling" << "Discrete Event Sampling";
			break;
		}
		case RConvergenceChecker_CN : {
			result << "Standard";// << "Optimality Condition";
			break;
		}
		case RNonlinearSingleConstraintSolver_CN : {
			result << "Stepper";
			break;
		}
		case RStepDirectionSearcher_CN : {
			result << "iHLRF";// << "Polak He" << "SQP";
			break;
		}
		case RStepSizeSearcher_CN : {
			result << "Fixed" << "Armijo";
			break;
		}
		case RTransformer_CN : {
			result << "No Correlation" << "Nataf";
			break;
		}
		case RRandomNumberGenerator_CN : {
			result << "Independent Normal";
			break;
		}
		case RAccumulator_CN : {
			result << "Failure Probability" << "Histogram";
			break;
		}
		case RMeritChecker_CN : {
			result << "Adk Zhang";
			break;
		}
	}

	return result;
}

QObjectList RDomain::getObjectList(QString passedClassName)
{

	// getting Value corresponding to the string passed as the className
	QString className = passedClassName.remove(' ');
	ClassName classEnum = classNamesHash.value(className,NoValue_CN);

	QObjectList result;

	switch (classEnum){

		// PARAMETERS AND FUNCTION
		case RParameter_CN : {
			QList<RParameter *> objectList = this->findChildren<RParameter *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RParameterRandomVariable_CN : {
			QList<RRandomVariable *> objectList = this->findChildren<RRandomVariable *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RParameterDecisionVariable_CN : {
			QList<RDecisionVariable *> objectList = this->findChildren<RDecisionVariable *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RConstant_CN : {
			QList<RConstant *> objectList = this->findChildren<RConstant *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RParameterResponse_CN : {
			QList<RResponse *> objectList = this->findChildren<RResponse *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RCorrelation_CN : {
			QList<RCorrelation *> objectList = this->findChildren<RCorrelation *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RLocation_CN : {
			QList<RLocation *> objectList = this->findChildren<RLocation *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RTime_CN : {
			QList<RTime *> objectList = this->findChildren<RTime *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RFunction_CN : {
			QList<RFunction *> objectList = this->findChildren<RFunction *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RFileResponse_CN : {
			QList<RFileResponse *> objectList = this->findChildren<RFileResponse *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RCommandResponse_CN : {
			QList<RCommandResponse *> objectList = this->findChildren<RCommandResponse *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RGenericResponse_CN : {
			QList<RGenericResponse *> objectList = this->findChildren<RGenericResponse *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RDatabaseResponse_CN : {
			QList<RDatabaseResponse *> objectList = this->findChildren<RDatabaseResponse *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RLocationResponse_CN : {
			QList<RLocationResponse *> objectList = this->findChildren<RLocationResponse *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RContinuousRandomVariable_CN : {
			QList<RContinuousRandomVariable *> objectList = this->findChildren<RContinuousRandomVariable *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RContinuousDecisionVariable_CN : {
			QList<RContinuousDecisionVariable *> objectList = this->findChildren<RContinuousDecisionVariable *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}


		// MODEL
		case RModel_CN : {
			QList<RModel *> objectList = this->findChildren<RModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}

		// EVENT MODELS
		case ROperationEventType1Model_CN: {
			QList<ROperationEventType1Model *> objectList = this->findChildren<ROperationEventType1Model *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case ROperationEventType2Model_CN: {
			QList<ROperationEventType2Model *> objectList = this->findChildren<ROperationEventType2Model *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RTimeEventType1Model_CN: {
			QList<RTimeEventType1Model *> objectList = this->findChildren<RTimeEventType1Model *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RTimeEventType2Model_CN: {
			QList<RTimeEventType2Model *> objectList = this->findChildren<RTimeEventType2Model *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}


		// OCCURRENCE MODELS
		case RPoissonPulseProcessModel_CN : {
			QList<RPoissonPulseProcessModel *> objectList = this->findChildren<RPoissonPulseProcessModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}


		// HAZARD MODELS
		case RSingleLineSourceModel_CN : {
			QList<RSingleLineSourceModel *> objectList = this->findChildren<RSingleLineSourceModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RMultipointLineSourceModel_CN : {
			QList<RMultipointLineSourceModel *> objectList = this->findChildren<RMultipointLineSourceModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RCircularAreaSourceModel_CN : {
			QList<RCircularAreaSourceModel *> objectList = this->findChildren<RCircularAreaSourceModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RRectangularAreaSourceModel_CN : {
			QList<RRectangularAreaSourceModel *> objectList = this->findChildren<RRectangularAreaSourceModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RQuadrilateralAreaSourceModel_CN : {
			QList<RQuadrilateralAreaSourceModel *> objectList = this->findChildren<RQuadrilateralAreaSourceModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RPolygonalAreaSourceModel_CN : {
			QList<RPolygonalAreaSourceModel *> objectList = this->findChildren<RPolygonalAreaSourceModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RMomentMagnitudeModel_CN : {
			QList<RMomentMagnitudeModel *> objectList = this->findChildren<RMomentMagnitudeModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RBoundedExponentialMagnitudeModel_CN : {
			QList<RBoundedExponentialMagnitudeModel *> objectList = this->findChildren<RBoundedExponentialMagnitudeModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RGenericAttenuationModel_CN : {
			QList<RGenericAttenuationModel *> objectList = this->findChildren<RGenericAttenuationModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RJoynerBoore1981IntensityModel_CN : {
			QList<RJoynerBoore1981IntensityModel *> objectList = this->findChildren<RJoynerBoore1981IntensityModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RAtkinson1997IntensityModel_CN : {
			QList<RAtkinson1997IntensityModel *> objectList = this->findChildren<RAtkinson1997IntensityModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RBooreAtkinson2008IntensityModel_CN : {
			QList<RBooreAtkinson2008IntensityModel *> objectList = this->findChildren<RBooreAtkinson2008IntensityModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RAtkinsonBoore2003IntensityModel_CN : {
			QList<RAtkinsonBoore2003IntensityModel *> objectList = this->findChildren<RAtkinsonBoore2003IntensityModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RDuWang2012IntensityModel_CN : {
			QList<RDuWang2012IntensityModel *> objectList = this->findChildren<RDuWang2012IntensityModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RCampbellBozorgnia2008IntensityModel_CN : {
			QList<RCampbellBozorgnia2008IntensityModel *> objectList = this->findChildren<RCampbellBozorgnia2008IntensityModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RAbrahamsonSilva2008IntensityModel_CN : {
			QList<RAbrahamsonSilva2008IntensityModel *> objectList = this->findChildren<RAbrahamsonSilva2008IntensityModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RChiouYoungs2008IntensityModel_CN : {
			QList<RChiouYoungs2008IntensityModel *> objectList = this->findChildren<RChiouYoungs2008IntensityModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RBooreEtAl2013IntensityModel_CN : {
			QList<RBooreEtAl2013IntensityModel *> objectList = this->findChildren<RBooreEtAl2013IntensityModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RAbrahamsonEtAl2013IntensityModel_CN : {
			QList<RAbrahamsonEtAl2013IntensityModel *> objectList = this->findChildren<RAbrahamsonEtAl2013IntensityModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RIdriss2013IntensityModel_CN : {
			QList<RIdriss2013IntensityModel *> objectList = this->findChildren<RIdriss2013IntensityModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RCampbellBozorgnia2013IntensityModel_CN : {
			QList<RCampbellBozorgnia2013IntensityModel *> objectList = this->findChildren<RCampbellBozorgnia2013IntensityModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RChiouYoungs2013IntensityModel_CN : {
			QList<RChiouYoungs2013IntensityModel *> objectList = this->findChildren<RChiouYoungs2013IntensityModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RRezaeianDerKiureghianArtificialGroundMotionModel_CN: {
			QList<RRezaeianDerKiureghianArtificialGroundMotionModel *> objectList = this->findChildren<RRezaeianDerKiureghianArtificialGroundMotionModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}

		case RSimpleWindVelocityModel_CN : {
			QList<RSimpleWindVelocityModel *> objectList = this->findChildren<RSimpleWindVelocityModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RSimpleWindPressureModel_CN : {
			QList<RSimpleWindPressureModel *> objectList = this->findChildren<RSimpleWindPressureModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RSimpleSnowLoadModel_CN : {
			QList<RSimpleSnowLoadModel *> objectList = this->findChildren<RSimpleSnowLoadModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}


		// INFRASTRUCTURE MODELS
		case RStModel_CN : {
			QList<RStModel *> objectList = this->findChildren<RStModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RBuildingInformationModel_CN : {
			QList<RBuildingInformationModel *> objectList = this->findChildren<RBuildingInformationModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RBuildingResponseModel_CN : {
			QList<RBuildingResponseModel *> objectList = this->findChildren<RBuildingResponseModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RSimpleDeteriorationModel_CN : {
			QList<RSimpleDeteriorationModel *> objectList = this->findChildren<RSimpleDeteriorationModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RSoilStructureInteractionModel_CN : {
			QList<RSoilStructureInteractionModel *> objectList = this->findChildren<RSoilStructureInteractionModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}

		// INFRASTRUCTURE/BUILDINGS MODELS
		case RBuildingInspectionTimeModel_CN: {
			QList<RBuildingInspectionTimeModel *> objectList = this->findChildren<RBuildingInspectionTimeModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RBuildingInspectionCostModel_CN: {
			QList<RBuildingInspectionCostModel *> objectList = this->findChildren<RBuildingInspectionCostModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RBuildingRepairMobilizationTimeModel_CN: {
			QList<RBuildingRepairMobilizationTimeModel *> objectList = this->findChildren<RBuildingRepairMobilizationTimeModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RBuildingRepairMobilizationCostModel_CN: {
			QList<RBuildingRepairMobilizationCostModel *> objectList = this->findChildren<RBuildingRepairMobilizationCostModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RBuildingRepairTimeModel_CN: {
			QList<RBuildingRepairTimeModel *> objectList = this->findChildren<RBuildingRepairTimeModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RBuildingRestorationModel_CN: {
			QList<RBuildingRestorationModel *> objectList = this->findChildren<RBuildingRestorationModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RBuildingUtilityModel_CN: {
			QList<RBuildingUtilityModel *> objectList = this->findChildren<RBuildingUtilityModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RBuildingCasualtyModel_CN: {
			QList<RBuildingCasualtyModel *> objectList = this->findChildren<RBuildingCasualtyModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RBuildingSearchAndRescueTimeModel_CN: {
			QList<RBuildingSearchAndRescueTimeModel *> objectList = this->findChildren<RBuildingSearchAndRescueTimeModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RBuildingSearchAndRescueCostModel_CN: {
			QList<RBuildingSearchAndRescueCostModel *> objectList = this->findChildren<RBuildingSearchAndRescueCostModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RBuildingFatalityCostModel_CN: {
			QList<RBuildingFatalityCostModel *> objectList = this->findChildren<RBuildingFatalityCostModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RBuildingRelocationModel_CN: {
			QList<RBuildingRelocationModel *> objectList = this->findChildren<RBuildingRelocationModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}

		// INFRASTRUCTURE/COMMERCIAL MODELS
		case RCommercialInformationModel_CN: {
			QList<RCommercialInformationModel *> objectList = this->findChildren<RCommercialInformationModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RCommercialLossModel_CN: {
			QList<RCommercialLossModel *> objectList = this->findChildren<RCommercialLossModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}

		// INFRASTRUCTURE/POWER MODELS
		case RPowerStationDamageModel_CN: {
			QList<RPowerStationDamageModel *> objectList = this->findChildren<RPowerStationDamageModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RPowerStationInformationModel_CN: {
			QList<RPowerStationInformationModel *> objectList = this->findChildren<RPowerStationInformationModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RPowerStationInspectionTimeModel_CN: {
			QList<RPowerStationInspectionTimeModel *> objectList = this->findChildren<RPowerStationInspectionTimeModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RPowerStationInspectionCostModel_CN: {
			QList<RPowerStationInspectionCostModel *> objectList = this->findChildren<RPowerStationInspectionCostModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RPowerStationServiceabilityModel_CN: {
			QList<RPowerStationServiceabilityModel *> objectList = this->findChildren<RPowerStationServiceabilityModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RPowerStationRepairTimeModel_CN: {
			QList<RPowerStationRepairTimeModel *> objectList = this->findChildren<RPowerStationRepairTimeModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RPowerStationRepairCostModel_CN: {
			QList<RPowerStationRepairCostModel *> objectList = this->findChildren<RPowerStationRepairCostModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}

		// INFRASTRUCTURE/WATER MODELS
		case RWaterStationDamageModel_CN: {
			QList<RWaterStationDamageModel *> objectList = this->findChildren<RWaterStationDamageModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RWaterStationInformationModel_CN: {
			QList<RWaterStationInformationModel *> objectList = this->findChildren<RWaterStationInformationModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RWaterStationInspectionTimeModel_CN: {
			QList<RWaterStationInspectionTimeModel *> objectList = this->findChildren<RWaterStationInspectionTimeModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RWaterStationInspectionCostModel_CN: {
			QList<RWaterStationInspectionCostModel *> objectList = this->findChildren<RWaterStationInspectionCostModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RWaterStationServiceabilityModel_CN: {
			QList<RWaterStationServiceabilityModel *> objectList = this->findChildren<RWaterStationServiceabilityModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RWaterStationFunctionalityModel_CN: {
			QList<RWaterStationFunctionalityModel *> objectList = this->findChildren<RWaterStationFunctionalityModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RWaterStationRepairTimeModel_CN: {
			QList<RWaterStationRepairTimeModel *> objectList = this->findChildren<RWaterStationRepairTimeModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RWaterStationRepairCostModel_CN: {
			QList<RWaterStationRepairCostModel *> objectList = this->findChildren<RWaterStationRepairCostModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}

		// INFRASTRUCTURE/HEALTHCARE MODELS
		case RHealthcareCapacityModel_CN: {
			QList<RHealthcareCapacityModel *> objectList = this->findChildren<RHealthcareCapacityModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RHealthcareInformationModel_CN: {
			QList<RHealthcareInformationModel *> objectList = this->findChildren<RHealthcareInformationModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RHealthcareRestorationModel_CN: {
			QList<RHealthcareRestorationModel *> objectList = this->findChildren<RHealthcareRestorationModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RHealthcareDemandModel_CN: {
			QList<RHealthcareDemandModel *> objectList = this->findChildren<RHealthcareDemandModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RHealthcareTreatmentCostModel_CN: {
			QList<RHealthcareTreatmentCostModel *> objectList = this->findChildren<RHealthcareTreatmentCostModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RHealthcareTreatmentTimeModel_CN: {
			QList<RHealthcareTreatmentTimeModel *> objectList = this->findChildren<RHealthcareTreatmentTimeModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}


		// CONSEQUENCE MODELS
		case RSimpleDamageCurveModel_CN : {
			QList<RSimpleDamageCurveModel *> objectList = this->findChildren<RSimpleDamageCurveModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RSimpleBuildingRepairCostModel_CN : {
			QList<RSimpleBuildingRepairCostModel *> objectList = this->findChildren<RSimpleBuildingRepairCostModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RFemaNibsBridgeDamageModel_CN : {
			QList<RFemaNibsBridgeDamageModel *> objectList = this->findChildren<RFemaNibsBridgeDamageModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RFemaNibsBuildingRepairCostModel_CN : {
			QList<RFemaNibsBuildingRepairCostModel *> objectList = this->findChildren<RFemaNibsBuildingRepairCostModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RRegionalLossModel_CN : {
			QList<RRegionalLossModel *> objectList = this->findChildren<RRegionalLossModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RDiscountingModel_CN : {
			QList<RDiscountingModel *> objectList = this->findChildren<RDiscountingModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RBuildingDamageModel_CN : {
			QList<RBuildingDamageModel *> objectList = this->findChildren<RBuildingDamageModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RBuildingRepairCostModel_CN : {
			QList<RBuildingRepairCostModel *> objectList = this->findChildren<RBuildingRepairCostModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RMasonryBuildingDamageModel_CN : {
			QList<RMasonryBuildingDamageModel *> objectList = this->findChildren<RMasonryBuildingDamageModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RMasonryBuildingRetrofitCostModel_CN : {
			QList<RMasonryBuildingRetrofitCostModel *> objectList = this->findChildren<RMasonryBuildingRetrofitCostModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RSimpleBuildingLifeCycleEnergyUsageModel_CN : {
			QList<RSimpleBuildingLifeCycleEnergyUsageModel *> objectList = this->findChildren<RSimpleBuildingLifeCycleEnergyUsageModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RDetailedBuildingLifeCycleEnergyUsageModel_CN : {
			QList<RDetailedBuildingLifeCycleEnergyUsageModel *> objectList = this->findChildren<RDetailedBuildingLifeCycleEnergyUsageModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RSimpleBuildingLifeCycleWaterUsageModel_CN : {
			QList<RSimpleBuildingLifeCycleWaterUsageModel *> objectList = this->findChildren<RSimpleBuildingLifeCycleWaterUsageModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RDetailedBuildingLifeCycleWaterUsageModel_CN : {
			QList<RDetailedBuildingLifeCycleWaterUsageModel *> objectList = this->findChildren<RDetailedBuildingLifeCycleWaterUsageModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RSimpleBuildingLifeCycleGWPModel_CN : {
			QList<RSimpleBuildingLifeCycleGWPModel *> objectList = this->findChildren<RSimpleBuildingLifeCycleGWPModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RDetailedBuildingLifeCycleGWPModel_CN : {
			QList<RDetailedBuildingLifeCycleGWPModel *> objectList = this->findChildren<RDetailedBuildingLifeCycleGWPModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
			// New
		case RGypsumWallboardPartitionDamageModel_CN: {
			QList<RGypsumWallboardPartitionDamageModel *> objectList = this->findChildren<RGypsumWallboardPartitionDamageModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RSuspendedCeilingDamageModel_CN: {
			QList<RSuspendedCeilingDamageModel *> objectList = this->findChildren<RSuspendedCeilingDamageModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RTractionElevatorDamageModel_CN: {
			QList<RTractionElevatorDamageModel *> objectList = this->findChildren<RTractionElevatorDamageModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RConcreteSpecialMomentFrameInteriorJointDamageModel_CN: {
			QList<RConcreteSpecialMomentFrameInteriorJointDamageModel *> objectList = this->findChildren<RConcreteSpecialMomentFrameInteriorJointDamageModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RConcreteIntermediateMomentFrameInteriorJointDamageModel_CN: {
			QList<RConcreteIntermediateMomentFrameInteriorJointDamageModel *> objectList = this->findChildren<RConcreteIntermediateMomentFrameInteriorJointDamageModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RConcreteMomentFrameExteriorJointDamageModel_CN: {
			QList<RConcreteMomentFrameExteriorJointDamageModel *> objectList = this->findChildren<RConcreteMomentFrameExteriorJointDamageModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RCurtainWallDamageModel_CN: {
			QList<RCurtainWallDamageModel *> objectList = this->findChildren<RCurtainWallDamageModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RComponentLossModel_CN: {
			QList<RComponentLossModel *> objectList = this->findChildren<RComponentLossModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
			// Social
		case RInjurySocialLossModel_CN: {
			QList<RInjurySocialLossModel *> objectList = this->findChildren<RInjurySocialLossModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RPopulationDensityModel_CN: {
			QList<RPopulationDensityModel *> objectList = this->findChildren<RPopulationDensityModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RRelocationCostModel_CN: {
			QList<RRelocationCostModel *> objectList = this->findChildren<RRelocationCostModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RLifeQualityReductionModel_CN: {
			QList<RLifeQualityReductionModel *> objectList = this->findChildren<RLifeQualityReductionModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}

		// EXTERNAL SOFTWARE
		case RMatlabModel_CN : {
			QList<RMatlabModel *> objectList = this->findChildren<RMatlabModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case ROpenSeesModel_CN : {
			QList<ROpenSeesModel *> objectList = this->findChildren<ROpenSeesModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RANSYSModel_CN : {
			QList<RANSYSModel *> objectList = this->findChildren<RANSYSModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RAbaqusModel_CN : {
			QList<RAbaqusModel *> objectList = this->findChildren<RAbaqusModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RUSFOSModel_CN : {
			QList<RUSFOSModel *> objectList = this->findChildren<RUSFOSModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case REMMEModel_CN : {
			QList<REMMEModel *> objectList = this->findChildren<REMMEModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RSAP2000Model_CN : {
			QList<RSAP2000Model *> objectList = this->findChildren<RSAP2000Model *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}


		// GENERIC MODELS
		case RAlgebraicExpressionModel_CN : {
			QList<RAlgebraicExpressionModel *> objectList = this->findChildren<RAlgebraicExpressionModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RRandomVariableWithRandomParametersModel_CN : {
			QList<RRandomVariableWithRandomParametersModel *> objectList = this->findChildren<RRandomVariableWithRandomParametersModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RRootFindingModel_CN : {
			QList<RRootFindingModel *> objectList = this->findChildren<RRootFindingModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RScriptModel_CN : {
			QList<RScriptModel *> objectList = this->findChildren<RScriptModel *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}


		// ANALYSIS TOOLS
		case RConvergenceChecker_CN : {
			QList<RConvergenceChecker *> objectList = this->findChildren<RConvergenceChecker *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RNonlinearSingleConstraintSolver_CN : {
			QList<RNonlinSingleConstrSolver *> objectList = this->findChildren<RNonlinSingleConstrSolver *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RStepDirectionSearcher_CN : {
			QList<RStepDirectionSearcher *> objectList = this->findChildren<RStepDirectionSearcher *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RStepSizeSearcher_CN : {
			QList<RStepSizeSearcher *> objectList = this->findChildren<RStepSizeSearcher *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RTransformer_CN : {
			QList<RTransformer *> objectList = this->findChildren<RTransformer *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RRandomNumberGenerator_CN : {
			QList<RRandomNumberGenerator *> objectList = this->findChildren<RRandomNumberGenerator *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RAccumulator_CN : {
			QList<RAccumulator *> objectList = this->findChildren<RAccumulator *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RMeritChecker_CN : {
			QList<RMeritChecker *> objectList = this->findChildren<RMeritChecker *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RStepperNonlinSingleConstrSolver_CN : {
			QList<RStepperNonlinSingleConstrSolver *> objectList = this->findChildren<RStepperNonlinSingleConstrSolver *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RFixedStepSizeSearcher_CN : {
			QList<RFixedStepSizeSearcher *> objectList = this->findChildren<RFixedStepSizeSearcher *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RArmijoStepSizeSearcher_CN : {
			QList<RArmijoStepSizeSearcher *> objectList = this->findChildren<RArmijoStepSizeSearcher *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RHLRFStepDirectionSearcher_CN : {
			QList<RHLRFStepDirectionSearcher *> objectList = this->findChildren<RHLRFStepDirectionSearcher *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RNoCorrelationTransformer_CN : {
			QList<RNoCorrelationTransformer *> objectList = this->findChildren<RNoCorrelationTransformer *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RNatafTransformer_CN : {
			QList<RNatafTransformer *> objectList = this->findChildren<RNatafTransformer *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RStandardConvergenceChecker_CN : {
			QList<RStandardConvergenceChecker *> objectList = this->findChildren<RStandardConvergenceChecker *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RIndependentNormalRandomNumberGenerator_CN : {
			QList<RIndependentNormalRandomNumberGenerator *> objectList = this->findChildren<RIndependentNormalRandomNumberGenerator *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RFailureProbabilityAccumulator_CN : {
			QList<RFailureProbabilityAccumulator *> objectList = this->findChildren<RFailureProbabilityAccumulator *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RHistogramAccumulator_CN : {
			QList<RHistogramAccumulator *> objectList = this->findChildren<RHistogramAccumulator *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RAdkZhangMeritChecker_CN : {
			QList<RAdkZhangMeritChecker *> objectList = this->findChildren<RAdkZhangMeritChecker *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RScenarioSamplingAnalyzer_CN : {
			QList<RScenarioSamplingAnalyzer *> objectList = this->findChildren<RScenarioSamplingAnalyzer *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RFunctionEvaluationAnalyzer_CN : {
			QList<RFunctionEvaluationAnalyzer *> objectList = this->findChildren<RFunctionEvaluationAnalyzer *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RFORMAnalyzer_CN : {
			QList<RFORMAnalyzer *> objectList = this->findChildren<RFORMAnalyzer *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RSORMAnalyzer_CN: {
			QList<RSORMAnalyzer *> objectList = this->findChildren<RSORMAnalyzer *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RFOSMAnalyzer_CN : {
			QList<RFOSMAnalyzer *> objectList = this->findChildren<RFOSMAnalyzer *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RSamplingAnalyzer_CN : {
			QList<RSamplingAnalyzer *> objectList = this->findChildren<RSamplingAnalyzer *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}
		case RLoadCoincidenceAnalyzer_CN : {
			QList<RLoadCoincidenceAnalyzer *> objectList = this->findChildren<RLoadCoincidenceAnalyzer *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}	
		case RDiscreteEventSamplingAnalyzer_CN: {
			QList<RDiscreteEventSamplingAnalyzer *> objectList = this->findChildren<RDiscreteEventSamplingAnalyzer *>();
			for (int i = 0; i < objectList.count(); i++) {
				result << objectList[i];
			}
			break;
		}

	}

	sort(&result);
	return result;
}

QStringList RDomain::getDynamicPropertyList()
{
	QStringList result;
	result << "PerturbationFactor" << "EfficientPerturbation" << "XPoints" << "PDFPoints";
	return result;
}

QStringList RDomain::getPropertyStringList(QString propertyName)
{
	QStringList parameterPropertyList;
	parameterPropertyList << "Mean" << "StandardDeviation" << "CoefficientOfVariation" << "InitialValue" << "UpperBound" << "LowerBound" << "Parameter1" << "Parameter2" << "Parameter3" << "Parameter4" << "IncrementalCost";
	parameterPropertyList << "Magnitude";
	if (parameterPropertyList.contains(propertyName)) {
		return getStringList(getObjectList("Parameter"));

	} 
	else if (propertyName.contains("RandomVariable")) {
		return getStringList(getObjectList("Parameter Random Variable Continuous"));

	} 
	else if (propertyName == QString("Location")) {
		return getStringList(getObjectList("Parameter Location"));

	} 
	else if (propertyName == QString("DistributionType")) {
		return QStringList() << "Normal (mean, stdv)" << "Exponential (mu, xo)" << "Bounded Exponential (mu, xo, min, max)" << "Linear (a, b)" << "Laplace (a, xo)" << "Rayleigh (sigma, xo)" << "Gamma (a, b)" << "Uniform (a, b)" << "Lognormal (zeta, sigma)" << "Chi-square (nu, xo)" << "Scaled Inverse Chi-square (nu, xo, sigma)" << "t-distribution (nu, xo, sigma)" << "Beta (a, b, min, max)" << "Logistic (a, xo)" << "Weibull (a, b)" << "Gumbel (a, b)" << "Type-2 Gumbel (a, b)" << "Frechet (a, s, m)" << "F-distribution (nu1, nu2)" << "Pareto (a, b)" << "Arcsine (a, b)" << "Birnbaum-Saunders (a, b, m)" << "Irwin-Hall (n, a, b)" << "Wald (mu, lambda)" << "Arcsine (a, b)" << "User Defined";

	} 
	else if (propertyName == QString("Model")) {
		return getStringList(getObjectList("Model"));

	} 
	else if (propertyName == QString("Function") || propertyName == QString("LimitStateFunction")) {
		return getStringList(getObjectList("Function"));

	} 
	else if (propertyName == QString("TimeParameter")) {
		return getStringList(getObjectList("Parameter Time"));

	} 
	else if (propertyName == QString("RandomNumberGenerator")) {
		return getStringList(getObjectList("Random Number Generator"));

	} 
	else if (propertyName == QString("StepSizeSearcher")) {
		return getStringList(getObjectList("Step Size Searcher"));

	} 
	else if (propertyName == QString("StepDirectionSearcher")) {
		return getStringList(getObjectList("Step Direction Searcher"));

	} 
	else if (propertyName == QString("RandomNumberGenerator")) {
		return getStringList(getObjectList("Random Number Generator"));

	} 
	else if (propertyName == QString("Transformer")) {
		return getStringList(getObjectList("Transformer"));

	} 
	else if (propertyName == QString("ConvergenceChecker")) {
		return getStringList(getObjectList("Convergence Checker"));

	} 
	else if (propertyName == QString("MeritChecker")) {
		return getStringList(getObjectList("Merit Checker"));

	}  
	else if (propertyName == QString("NonlinearSingleConstraintSolver")) {
		return getStringList(getObjectList("Nonlinear Single Constraint Solver"));

	}   
	else if (propertyName == QString("Accumulator")) {
		return getStringList(getObjectList("Accumulator"));

	}  
	else if (propertyName == QString("Analyzer")) {
		return getStringList(getObjectList("Analyzer"));

	}
	else if (propertyName == QString("FORMAnalyzer")) {
		return getStringList(getObjectList("Analyzer FORM"));

	}
	else if (propertyName == QString("SamplingAnalyzer")) {
		return getStringList(getObjectList("Analyzer Sampling"));

	}
	else {
		return QStringList();
	}
}

QObject * RDomain::getLastAddedObject() const
{
	return theLastAddedObject;
}

QString RDomain::getProperty(const QObject *passedObject, QString propertyName) const
{
	QByteArray byteArrayName = propertyName.toLatin1();
	const char * charName = byteArrayName.data();
	int index = passedObject->metaObject()->indexOfProperty(charName);
	QVariant valueVariant;
	if (index >= 0) {
		QMetaProperty metaProperty = passedObject->metaObject()->property(index);
		QVariant::Type type = metaProperty.type();

		valueVariant = QVariant::fromValue(passedObject->property(charName));
		if (metaProperty.isEnumType()) {
			valueVariant = metaProperty.enumerator().valueToKey(valueVariant.toInt());
		}
		//const char * typeName = passedObject->metaObject()->property(i).typeName();

		if (type == QVariant::Invalid) {
			valueVariant = "";
		} else if (type == QMetaType::QObjectStar) {
			QObject *propertyObject = passedObject->property(charName).value<QObject *>();
			if (propertyObject) {
				valueVariant = propertyObject->objectName();
			} else {
				valueVariant = "";
			}
			//const char * propertyType = QVariant::fromValue(propertyObject).typeName();
		} else if (type == QVariant::Double) {
			if (!metaProperty.isWritable()) {
				bool ok;
				double tempDouble = valueVariant.toDouble(&ok);
				if (ok) {
					QString tempString = QString("%1").arg(tempDouble, 0, 'g', 8);
					valueVariant = tempString;
				}
			} else {
				bool ok;
				double tempDouble = valueVariant.toDouble(&ok);
				if (ok) {
					QString tempString = QString("%1").arg(tempDouble, 0, 'g', 13);
					valueVariant = tempString;
				}
			}
		}
	} else {
		valueVariant = passedObject->property(charName);

		if (valueVariant.type() == QVariant::Invalid) {
			return "No such property";
		}
	}

	QString result = valueVariant.toString();
	return result;
}

int RDomain::setProperty(QObject *passedObject, QString propertyName, QString propertyValue)
{
	QByteArray byteArrayName = propertyName.toLatin1();
	const char * charName = byteArrayName.data();
	int index = passedObject->metaObject()->indexOfProperty(charName);
	QVariant::Type type = passedObject->metaObject()->property(index).type();

	bool isStaticProperty = false;

	if (type == QMetaType::QObjectStar) {
		QObject *propertyObject = findChild<QObject *>(propertyValue);
		if (propertyObject) {
			isStaticProperty = passedObject->setProperty(charName, QVariant::fromValue(propertyObject));
		} else {
			if (!propertyValue.isEmpty()) {
				rCritical() << "Error in" << passedObject->objectName() << ": Could not find an object named" << propertyValue << "in the domain.";
				return 0;
			}
			QObject *nullPointer = 0;
			isStaticProperty = passedObject->setProperty(charName, qVariantFromValue(nullPointer));
		}
	} else {
		if (propertyName == QString("objectName")) {
			// Updating the object pane if the objectName has been changed.
			int result = renameObject(passedObject, propertyValue);
			if (result == 0) {
				// If the renaming was successful, then 1 is returned, so that the property widget sends a command to the tree widget to refresh the tree
				return 1;
			}
		} else {
			QMetaProperty metaProperty = passedObject->metaObject()->property(index);
			if (metaProperty.isEnumType()) {
				int enumValue = metaProperty.enumerator().keyToValue(propertyValue.toLatin1());
				if (enumValue < 0) {
					rCritical() << "Error: Could not set the property" << propertyName << "in the object" << passedObject->objectName() << ". The value" << propertyValue << "is invalid.";
				}
			}
			isStaticProperty = passedObject->setProperty(charName, QVariant::fromValue(propertyValue));
		}
	}
	if (isStaticProperty) {
		return 0;
	} else {
		// Returns -1 so that the main window knows that a property with this name is not a static property
		return -1;
	}
}

int RDomain::nameIsValid(QString name, bool checkForDuplicate)
{
	QRegExp regExp("[A-Za-z]+[A-Za-z0-9_]*");
	if (!regExp.exactMatch(name)) {
		QMessageBox::critical(0, tr("Error"), QString(tr("The object name \"%1\" is unacceptable. It should only contain alphanumeric characters and start with a letter.")).arg(name), QMessageBox::Ok | QMessageBox::Default);
		return -2;
	}

	if (checkForDuplicate) {
		// ATTENTION: THE FOLLOWING LINE MAKES THE PROCESS OF OPENING LARGE INPUT FILES VERY LONG
		QObject *theObject = this->findChild<QObject *>(name);
		if (theObject) {
			return -1;
		}
	}

	return 0;
}

void RDomain::setInputFileBeingOpened(bool value)
{
	isInputFileBeingOpened = value;
}

int RDomain::deleteObject(QString name)
{
	QObject *object = this->findChild<QObject *>(name);
	if (object) {
		disconnect(object, 0, 0, 0);
		((RObject *)object)->dereferenceSharedPointerToDeleteObject();
		delete object;
		return 0;
	} else {
		return -1;
	}
}

int RDomain::renameObject(QObject *passedObject, QString name)
{
	int result = nameIsValid(name);
	if (result == 0) {
		// The name is fine.
		QString oldName = passedObject->objectName();
		RObject *rObject = qobject_cast<RObject *>(passedObject);
		rObject->setObjectName(name);

		// Renaming the occurrences of this name in functions (?)
		//RParameter *parameter = qobject_cast<RParameter *>(passedObject);
		//if (parameter) {
		//	QList<RFunction *> functionList = this->findChildren<RFunction *>();
		//	for (int i = 0; i < functionList.count(); i++) {
		//		QString functionString = functionList[i]->getExpression();
		//		if (functionString.contains(oldName)) {
		//			QRegExp regExp("[A-Za-z0-9_]");
		//			functionString.replace(oldName, name);
		//		}
		//		functionList[i]->setFunction(functionString);
		//	}
		//}

		return 0;
	} else if (result == -1) {
		// The name already exists in the domain.
		QMessageBox::critical(0, tr("Error"), QString("Could not rename the object. An object with the name \"%1\" already exists.").arg(name), QMessageBox::Ok | QMessageBox::Default);
		return -1;
	} else if (result < -1) {
		// The name contains illegal characters.
		return -1;
	}
}

void RDomain::print(gsl_matrix *matrix, int threadID, QString outputFile)
{
	// printing the output of threads in different folders in the same directory as the given file
	if ((!outputFile.isNull()) && (threadID > 0)){
		QFileInfo fileInfo(outputFile);
		QString pureFileName = fileInfo.baseName();
		outputFile = outputFile.replace(pureFileName, pureFileName + "_thread" + QString::number(threadID));
	}

	int row = matrix->size1;
	int column = matrix->size2;

	QString startAndEndChar = "|";
	//QString startAndEndChar = QString(QChar(0x2502));  // Unicode number of the graphic sign
	
	QFile *file = 0;
	if (!outputFile.isNull()) {
		file = new QFile(outputFile);
		if (!file) {
			return;
		}
		if (file->exists()) {
			file->remove();
		}
		if (!file->open(QIODevice::WriteOnly | QIODevice::Text)) {
			rCritical(threadID) << "Warning: Could not create the file" << outputFile;
			return;
		}
		startAndEndChar.clear();
	}
		
	for (int i = 0; i < row; i++) {
		QString line = "";
		line += startAndEndChar;
		for (int j = 0; j < column; j++) {
			if (file) {
				line += QString("%1").arg(gsl_matrix_get(matrix, i, j), 0, 'g', 12);
			} else {
				// Is this enough precession?
				line += QString("%1").arg(gsl_matrix_get(matrix, i, j), 12, 'g', -1);
			}
			line += " ";
		}
		line += startAndEndChar;
		if (file) {
			line += "\n";
			QByteArray byteArray;
			byteArray.append(line);
			file->write(byteArray);
		} else {
			rDebug(threadID) << qPrintable(line);
		}
	}

	if (file) {
		file->flush();
		file->close();
		delete file;
		file = 0;
	}
}

void RDomain::print(gsl_vector *vector, int threadID, QString outputFile)
{
	gsl_matrix_view matrix = gsl_matrix_view_vector(vector, vector->size, 1);
	print(&matrix.matrix, threadID, outputFile);
}

int RDomain::readMatrixFromFile(gsl_matrix *matrix, QString inputFile)
{
	if (!matrix) {
		rCritical() << "Error: The passed matrix pointer is null";
		return -1;
	}
	int rowCount = matrix->size1;
	int colCount = matrix->size2;


	// Read the file here
	QFile file(inputFile);
	if (!file.open(QFile::ReadOnly)) {
		// Error
		rCritical() << "Error: Could not open the file" << inputFile;
		return -1;
	}

	// Read the file content into a QString
	QTextStream stream(&file);
	stream.seek(0);
	//QString fileContentString = in.readAll();

	// Tokenize the QString of file contents by "\n" and create a string list of input file lines
	QString verticalRegExpString = QString("[n\n\v\r%1%2]").arg(QChar(QChar::ParagraphSeparator)).arg(QChar(QChar::LineSeparator));
	QRegExp verticalRegExp = QRegExp(verticalRegExpString);
	//QStringList fileContentStringList = fileContentString.split(verticalRegExp, QString::SkipEmptyParts);

	QStringList fileContentStringList;
	QString line;
	QStringList lineList;
	stream.setAutoDetectUnicode(true);
	do {
		line = stream.readLine();
		if (!line.isEmpty()) {
			lineList = line.split(verticalRegExp, QString::SkipEmptyParts);
			fileContentStringList.append(lineList);
		}
		//QCoreApplication::processEvents();
	} while (!line.isNull());

	int fileRowCount = fileContentStringList.count();

	if (fileRowCount != rowCount) {
		rCritical() << "Error: The number of rows in the file" << inputFile << "does not match the number of rows of the passed matrix/vector";
		return -1;
	}

	QRegExp regExp = QRegExp("[ ,;\t]");
	int fileColCount = 0;

	for (int i = 0; i < fileContentStringList.count(); i++) {
		QStringList valueList = fileContentStringList[i].split(regExp, QString::SkipEmptyParts);

		if (i == 0) {
			fileColCount = valueList.count();
			if (fileColCount != colCount) {
				rCritical() << "Error: The number of columns in the file" << inputFile << "does not match the number of columns of the passed matrix/vector";
				return -1;
			}
		} else {
			if (fileColCount != valueList.count()) {
				rCritical() << "Error: The number of data at the line" << i + 1 << "is different than the line 1 in the file" << inputFile;
				return -1;
			}
		}

		for (int j = 0; j < valueList.count(); j++) {
			QString tempStr = valueList[j].trimmed();
			bool ok;
			double tempValue = tempStr.toDouble(&ok);
			if (!ok) {
				rCritical() << "Error: Could not read the value number" << j + 1 << "at the line" << i + 1 << "in the file" << inputFile;
				return -1;
			}
			
			gsl_matrix_set(matrix, i, j, tempValue);
		}
	}
	file.close();
	return 0;
}

int RDomain::readVectorFromFile(gsl_vector *vector, QString inputFile)
{
	if (!vector) {
		rCritical() << "Error: The passed vector pointer is null";
		return -1;
	}
	gsl_matrix *tempMatrix = gsl_matrix_calloc(vector->size, 1);
	int result = readMatrixFromFile(tempMatrix, inputFile);
	if (result < 0) {
		return -1;
	}
	gsl_matrix_get_col(vector,tempMatrix, 0); 
	gsl_matrix_free(tempMatrix);
	return 0;
}

QStringList RDomain::getStringList(QObjectList objectList)
{
	QStringList result;
	for (int i = 0; i < objectList.count(); i++) {
		result << objectList[i]->objectName();
	}
	return result;
}

int RDomain::sort(QObjectList *passedList)
{
	QObjectList searchList = *passedList;

	int beforeCount = searchList.count();
	QStringList nameList;
	QMap<QString, QObject *> map;
	for (int i = 0; i < searchList.count(); i++) {
		map[searchList[i]->objectName()] = searchList[i];
	}
	*passedList = map.values();
	int afterCount = map.count();
	if (afterCount < beforeCount) {
		// means a duplicated name has been removed
		return -1;
	} else {
		return 0;
	}

	/*bool duplicate = false;
	double count = passedList->count();
	bool swapped = true;
	while (swapped) {
	swapped = false;
	count--;
	for (int i = 0; i < count; i++) {
	QObject *first = passedList->value(i);
	QObject *second = passedList->value(i+1);
	QString name1 = first->objectName();
	QString name2 = second->objectName();
	if (name1 > name2) {
	passedList->replace(i, second);
	passedList->replace(i + 1, first);
	swapped = true;
	} else if (name1 == name2) {
	duplicate = true;
	}
	}
	}
	if (duplicate) {
	// means there is a duplicated name
	return -1;
	} else {
	return 0;
	}*/

	/*for (int i = 0; i < searchList.count(); i++) {
	nameList << searchList[i]->objectName();
	}
	qSort(nameList.begin(), nameList.end());
	passedList->clear();
	for (int i = 0; i < nameList.count(); i++) {
	for (int j = 0; j < searchList.count(); j++) {
	if (searchList[j]->objectName() == nameList[i]) {
	passedList->append(searchList[j]);
	searchList.removeAt(j);
	continue;
	}
	}
	}*/
}

int RDomain::sort(QList<RModel *> *passedList)
{
	QObjectList objectList;
	for (int i = 0; i < passedList->count(); i++) {
		QObject *qObject = passedList->value(i);
		objectList << qObject;
	}
	int result = sort(&objectList);
	passedList->clear();
	for (int i = 0; i < objectList.count(); i++) {
		RModel *object = qobject_cast<RModel *>(objectList[i]);
		passedList->append(object);
	}

	return result;
}

int RDomain::sort(QList<RParameter *> *passedList)
{
	QObjectList objectList;
	for (int i = 0; i < passedList->count(); i++) {
		QObject *qObject = passedList->value(i);
		objectList << qObject;
	}
	int result = sort(&objectList);
	passedList->clear();
	for (int i = 0; i < objectList.count(); i++) {
		RParameter *object = qobject_cast<RParameter *>(objectList[i]);
		passedList->append(object);
	}

	return result;
}

int RDomain::sort(QList<RRandomVariable *> *passedList, QString sortingMeasure)
{
	int result = 0;
	if (sortingMeasure == QString("Name")) {
		QObjectList objectList;
		for (int i = 0; i < passedList->count(); i++) {
			QObject *qObject = passedList->value(i);
			objectList << qObject;
		}
		int result = sort(&objectList);
		passedList->clear();
		for (int i = 0; i < objectList.count(); i++) {
			RRandomVariable *object = qobject_cast<RRandomVariable *>(objectList[i]);
			passedList->append(object);
		}
	} else {
		int count = passedList->count();
		bool swapped = true;
		while (swapped) {
			swapped = false;
			count--;
			for (int i = 0; i < count; i++) {
				RContinuousRandomVariable *firstVariable = qobject_cast<RContinuousRandomVariable *>(passedList->value(i));
				RContinuousRandomVariable *secondVariable = qobject_cast<RContinuousRandomVariable *>(passedList->value(i+1));
				double importance1;
				double importance2;
				if (sortingMeasure == QString("Alpha")) {
					importance1 = firstVariable->getAlphaImportanceMeasure();
					importance2 = secondVariable->getAlphaImportanceMeasure();
				} else if (sortingMeasure == QString("Gamma")) {
					importance1 = firstVariable->getGammaImportanceMeasure();
					importance2 = secondVariable->getGammaImportanceMeasure();
				}

				if (qAbs(importance1) < qAbs(importance2)) {
					passedList->replace(i, secondVariable);
					passedList->replace(i + 1, firstVariable);
					swapped = true;
				}
			}
		}
	}

	return result;
}

int RDomain::sort(QList<RDecisionVariable *> *passedList, QString sortingMeasure)
{
	int result = 0;
	if (sortingMeasure == QString("Name")) {
		QObjectList objectList;
		for (int i = 0; i < passedList->count(); i++) {
			QObject *qObject = passedList->value(i);
			objectList << qObject;
		}
		int result = sort(&objectList);
		passedList->clear();
		for (int i = 0; i < objectList.count(); i++) {
			RDecisionVariable *object = qobject_cast<RDecisionVariable *>(objectList[i]);
			passedList->append(object);
		}
	} 
	else if (sortingMeasure == QString("Sensitivity")) {
		int count = passedList->count();
		bool swapped = true;
		while (swapped) {
			swapped = false;
			count--;
			for (int i = 0; i < count; i++) {
				RContinuousDecisionVariable *firstVariable = qobject_cast<RContinuousDecisionVariable *>(passedList->value(i));
				RContinuousDecisionVariable *secondVariable = qobject_cast<RContinuousDecisionVariable *>(passedList->value(i+1));
				double importance1;
				double importance2;
				importance1 = firstVariable->getBetaSensitivityWrtCost();
				importance2 = secondVariable->getBetaSensitivityWrtCost();

				if (importance1 < importance2) {
					passedList->replace(i, secondVariable);
					passedList->replace(i + 1, firstVariable);
					swapped = true;
				}
			}
		}
	}

	return result;
}

int RDomain::sort(QList<RConstant *> *passedList)
{
	QObjectList objectList;
	for (int i = 0; i < passedList->count(); i++) {
		QObject *qObject = passedList->value(i);
		objectList << qObject;
	}
	int result = sort(&objectList);
	passedList->clear();
	for (int i = 0; i < objectList.count(); i++) {
		RConstant *object = qobject_cast<RConstant *>(objectList[i]);
		passedList->append(object);
	}

	return result;
}

int RDomain::sort(QList<RResponse *> *passedList)
{
	QObjectList objectList;
	for (int i = 0; i < passedList->count(); i++) {
		QObject *qObject = passedList->value(i);
		objectList << qObject;
	}
	int result = sort(&objectList);
	passedList->clear();
	for (int i = 0; i < objectList.count(); i++) {
		RResponse *object = qobject_cast<RResponse *>(objectList[i]);
		passedList->append(object);
	}

	return result;
}

QString RDomain::getFileName(QString filePath)
{
	return QFileInfo(filePath).completeBaseName();
}

QString RDomain::getAbsoluteFolderPath(QString filePath)
{
	return QFileInfo(filePath).absolutePath();
}

QString RDomain::stringToFilePath(QString string)
{
	string.remove('"');
	return QFileInfo(string).filePath();
}

QString RDomain::getCurrentFilePath() const
{
	return theCurrentFilePath;
}

void RDomain::setCurrentFilePath(QString value)
{
	theCurrentFilePath = value;

	QString workingPath = theCurrentFilePath;
	bool workingPathIsSet = QDir::setCurrent(workingPath);
}

int RDomain::openFile(QString fileName, QObjectList *objectsCreated, QString *errorMessage, bool StInputFile)
{
	QRegExp regExp("([[//]%#])");
	QChar propertyChar('|');
	QChar defineChar(':');

	// Get the list of dynamic QProperties that has been used in all classes from the domain
	QStringList dynamicPropertyList = RDomain::getDynamicPropertyList();

	int result = 0;
	
	// Get the current path of the program
	QString oldPath = this->getCurrentFilePath();

	// Open the file
	QFile file(fileName);
	if (!file.open(QFile::ReadOnly /*| QFile::Text*/)) {
		// Error
		*errorMessage = QString("Could not open the file %1.").arg(fileName);
		onFileOpenError(objectsCreated, oldPath, *errorMessage);
		return -1;
	}
	
	// Change the current path of the program
	if (!StInputFile) {
		this->setCurrentFilePath(RDomain::getAbsoluteFolderPath(fileName));
	}

	// Delete the current domain and re-instantiate it
	//resetDomain();	



	// Read the file content into a QString
	QTextStream stream(&file);
	stream.seek(0);

	// Tokenize the QString of file contents by "\n" and create a string list of input file lines
	QString verticalRegExpString = QString("[\n\v\r%1%2]").arg(QChar(QChar::ParagraphSeparator)).arg(QChar(QChar::LineSeparator));
	QRegExp verticalRegExp = QRegExp(verticalRegExpString);

	QStringList fileContentStringList;
	QString line;
	QStringList lineList;
	stream.setAutoDetectUnicode(true);
	do {
		line = stream.readLine();
		lineList = line.split(verticalRegExp/*, QString::SkipEmptyParts*/);
		fileContentStringList.append(lineList);
	} while (!line.isNull());

	QList<QStringList> propertyNamesList;
	QList<QStringList> propertyValuesList;

	// A list to store pointer to RParameters that are used in the St input file
	QObjectList stParameterList;

	// A string list to store a list of commands, such as "Analyze", "Output", or "Exit"
	QStringList commandStringList;

	// Inform theDomain that an input file is being opened
	this->setInputFileBeingOpened(true);

	// First stage: Instantiate objects
	int count = fileContentStringList.count();
	// Loop over the string list of all lines in the input file
	for (int i = 0; i < count; i++) {
		int progress = (int)i / 2.0 / count * (100.0 - 0.0);
		//rDebug() << progress;
		emit fileOpenProgressed(progress);

		// A line to keep the GUI responsive
		QCoreApplication::processEvents();

		// Reading the current line
		QString syntax = fileContentStringList[i].trimmed();
		fileContentStringList[i] = syntax;
		
		// Ignore empty lines
		if (syntax.isEmpty()) {
			continue;
		}

		// Ignore the comment lines (starting with //, %, and #)
		if ((syntax.startsWith("//")) || (syntax.startsWith('%')) || (syntax.startsWith('#')) || syntax.isEmpty()) {
			//fileContentStringList.removeAt(i);
			//i--;
			continue;
		}

		if (syntax.startsWith("Analyze") || syntax.startsWith("Output") || syntax.startsWith("Exit")) {
			commandStringList << syntax;
			continue;
		}

		else if (syntax.startsWith("ThreadCount")){

			// if there are created objects, skip setting!
			if (theLastAddedObject)
				continue;

			// spliting based on :
			QStringList tokens = QString(syntax).simplified().split(" ");

			if (tokens.size() != 2){
				* errorMessage = QString("Error in line %1: Valid Syntax for setting thread count is \"ThreadCount INTEGER_NUMBER\".").arg(i+1);
				onFileOpenError(objectsCreated, oldPath, *errorMessage);
			}
			
			bool ok = true;
			int threadCount = tokens[1].trimmed().toInt(&ok);

			if (!ok){
				* errorMessage = QString("Error in line %1: Valid Syntax for setting thread count is \"ThreadCount INTEGER_NUMBER.").arg(i+1);
				onFileOpenError(objectsCreated, oldPath, *errorMessage);
			}

			// else setting
			RParallelComputationManager::getInstance()->setMaxThreadCount(threadCount);
			continue;
		}

		// Split the current line from the locations of propertyChar, which is "|"
		QStringList parameterList = syntax.split(propertyChar, QString::SkipEmptyParts);
		
		// The first part after the split is the object's base class name
		QString objectClass = parameterList[0].trimmed();
		if (objectClass.isEmpty()) {
			// Error
			*errorMessage = QString("Error in line %1: The object defined at this line does not have the object type specified.").arg(i+1);
			onFileOpenError(objectsCreated, oldPath, *errorMessage);
			return -1;
		}
		
		QString objectName;

		QStringList propertyNameList;
		QStringList propertyValueList;

		bool hasObjectName = false;

		// Loop over splitted parts of this line
		for (int j = 1; j < parameterList.count(); j++) {
			QString parameter = parameterList[j].trimmed();	

			// Split this part form the location of the first occurrence of the character defineChar (':')
			int separatorIndex = parameter.indexOf(defineChar);
			if (separatorIndex < 0) {
				// Error
				*errorMessage = QString("Error in line %1: Could not read the properties of the object due to incorrect arguments.").arg(i+1);
				onFileOpenError(objectsCreated, oldPath, *errorMessage);
				return -1;
			}

			// Find the property name and its value
			propertyNameList += parameter.left(separatorIndex).trimmed().remove(' ');
			propertyValueList += parameter.right(parameter.length() - separatorIndex - 1).trimmed();

			if (propertyNameList[j-1] == QString("ObjectName")) {
				objectName = propertyValueList[j-1];
				result = this->nameIsValid(objectName, false);
				if (result < 0) {
					// Error	
					*errorMessage = "";
					onFileOpenError(objectsCreated, oldPath, *errorMessage);
					return -1;
				}	
				if (!objectName.isEmpty()) {
					hasObjectName = true;
				}
			}
		}
		propertyNamesList.append(propertyNameList);
		propertyValuesList.append(propertyValueList);
					
		if (!hasObjectName) {
			// Error
			*errorMessage = QString("Error in line %1: The object defined at this line does not have an object name specified.").arg(i+1);
			onFileOpenError(objectsCreated, oldPath, *errorMessage);
			return -1;
		}

		// Instantiate the object
		if ((!StInputFile) || ((StInputFile) && (objectClass != QString("RConstant")))) {
			result = this->createObject(objectName, objectClass, false);
		} else {
			// Parameters should not be opened in St input files
			if ((StInputFile) && (objectClass == QString("RConstant"))) {
				RParameter *parameter = this->findChild<RParameter *>(objectName);
				if (parameter) {
					stParameterList.append(parameter);
				}
				propertyNamesList.takeLast();
				propertyValuesList.takeLast();
			}
			continue;
		}

		if (result < 0) {
			// Error
			*errorMessage = QString("Error in line %1: Could not create the object defined at this line due to errors.").arg(i+1);
			onFileOpenError(objectsCreated, oldPath, *errorMessage);
			return -1;
		}
		objectsCreated->append(this->getLastAddedObject());
	}
	// Now, all lines in the input file are looped over

	// Inform theDomain that the operation of opening an input file is over
	this->setInputFileBeingOpened(false);

	// Checking for object name duplicates
	QObjectList allObjectList;
	if (!StInputFile) {
		allObjectList = this->findChildren<QObject *>();
	} else {
		// In case of opening an St file, do the name checking only for recently instantiated St objects.
		allObjectList = *objectsCreated;
	}
	QMap<QString, QObject *> map;
	for (int i = 0; i < allObjectList.count(); i++) {
		if (map.contains(allObjectList[i]->objectName())) {
			// Error
			*errorMessage =  QString("Error: Object with the name \"%1\" already exists.").arg(allObjectList[i]->objectName());
			onFileOpenError(objectsCreated, oldPath, *errorMessage);
			return -1;
		} else {
			map[allObjectList[i]->objectName()] = allObjectList[i];
		}
	}

	//#RMainWindow::noWarning = true;
	
	// Second stage: Assign QProperties
	count = objectsCreated->count();
	// Loop over the objectsCreated
	for (int i = 0; i < count; i++) {
		int progress = (int)((i / 2.0 / count + 0.5) * (100.0 - 0.0));
		//rDebug() << progress;
		emit fileOpenProgressed(progress);

		// A line to keep the GUI responsive
		QCoreApplication::processEvents();

		QObject *object = objectsCreated->value(i);
		if (!object) {
			// Error
			*errorMessage = QString("Error in line %1: Could not find the object \"%2\" in the domain.").arg(i+1).arg(object->objectName());
			onFileOpenError(objectsCreated, oldPath, *errorMessage);
			return -1;
		}

		QStringList propertyNameList = propertyNamesList[i];
		QStringList propertyValueList = propertyValuesList[i];

		// Loop over splitted parts of this line
		for (int j = 0; j < propertyNameList.count(); j++) {
			if (propertyNameList[j] == QString("ObjectName")) {
				continue;
			}

			QString propertyName = propertyNameList[j];

			// Set the property
			int result = this->setProperty(object, propertyNameList[j], propertyValueList[j]);
			bool isStaticProperty = !(result);
			
			// If the property that has just been set is not a static (hard-coded) property and is not listed in the list of dynamic properties, then issue and error
			if ((!isStaticProperty) && (!dynamicPropertyList.contains(propertyNameList[j]))) {				
				QByteArray byteArrayName = propertyNameList[j].toLatin1();
				const char *propertyNameChar = byteArrayName.data();
				object->setProperty(propertyNameChar, QVariant());

				// Error
				*errorMessage = QString("Error in line %1: The object \"%2\" does not have a property called \"%3\".").arg(i+1).arg(object->objectName()).arg(propertyNameList[j]);
				onFileOpenError(objectsCreated, oldPath, *errorMessage);
				return -1;
			}
		}
	}
	// Now, all object in the objectsCreated are looped over

	//#RMainWindow::noWarning = false;

	
	if (!StInputFile) {
		// Change the current directory path of the program to the path of the input file
		//QString currentTitle = RDomain::stringToFilePath(fileName);
		//updateApplicationTitle(RDomain::getFileName(currentTitle));
		this->setCurrentFilePath(RDomain::getAbsoluteFolderPath(fileName));
	} else {
		// In case of opening St input files, append stParameterList to objectList
		objectsCreated->append(stParameterList);
	}

	// Third stage: run commands
	QRegExp horizontalRegExp("[ \t]");
	for (int i = 0; i < commandStringList.count(); i++) {
		if (commandStringList[i].startsWith("Analyze")) {
			QStringList commandOptionsList = commandStringList[i].split(horizontalRegExp, QString::SkipEmptyParts);
			if (commandOptionsList.count() != 2) {
				rCritical() << "Error: The command" << commandStringList[i] << "is not recognized and is not run";
				continue;
			}
			RAnalyzer *analyzer = this->findChild<RAnalyzer *>(commandOptionsList[1]);
			if (!analyzer) {
				rCritical() << "Error: The command" << commandStringList[i] << "is not recognized and is not run";
				continue;
			}
			int status = analyzer->analyze();
			if (status < 0) {
				rCritical() << "Error in" << analyzer->objectName() << ": The analysis did not converge because of errors";
			}
		} else if (commandStringList[i].startsWith("Output")) {
			// Dumping the entire output pane in a text file
			QString outputFileName = commandStringList[i].remove(0, 6).trimmed();
			QFile *outputFile = new QFile(outputFileName);
			if (outputFile->exists()) {
				outputFile->remove();
			}
			if (!outputFile->open(QIODevice::WriteOnly | QIODevice::Text)) {
				rCritical() << "Warning: Could not create the file" << outputFileName << "to store the Rt output before exiting";
			}
			if (qobject_cast<QApplication*>(QCoreApplication::instance()) != 0) { // Means Rt is run in GUI mode
				RMainWindow *mainWindow = qobject_cast<RMainWindow *>(this->parent());			
				QByteArray consoleText =mainWindow->getConsoleText();
				outputFile->write(consoleText);
			} else { // Means Rt is run in console mode
				outputFile->write(*theOutputByteArray);
			}

			outputFile->flush();
			outputFile->close();
			delete outputFile;
		} else if (commandStringList[i].startsWith("Exit")) {
			//closeWithoutWarning = true;
			//this->close();

			exit(0);
		}
	}

	return 0;
}

void RDomain::onFileOpenError(QObjectList *objectList, QString oldFilePath, QString errorMessage)
{
	// Delete all created objects
	for (int i = 0; i < objectList->count(); i++) {
		delete objectList->value(i);
	}
	objectList->empty();

	// Show the error message
	if (this->parent() == 0) { // Means Rt is run in console mode
		rCritical() << errorMessage;
	}

	// Set the current file path of the program to the oldFilePath
	this->setCurrentFilePath(RDomain::getAbsoluteFolderPath(oldFilePath));
}

QByteArray * RDomain::getOutputByteArray()
{
	return theOutputByteArray;
}


void RDomain::addClassNamesToHash(){

	/* CLASSES */



	// PARAMETERS AND FUNCTION

	classNamesHash.insert("ParameterLocation" , RLocation_CN);
	classNamesHash.insert("rlocation" , RLocation_CN);

	classNamesHash.insert("ParameterConstant" , RConstant_CN);
	classNamesHash.insert("rconstant" , RConstant_CN);

	classNamesHash.insert("ParameterTime" , RTime_CN);
	classNamesHash.insert("rtime" , RTime_CN);

	classNamesHash.insert("ParameterResponseFile" , RFileResponse_CN);
	classNamesHash.insert("rfileresponse" , RFileResponse_CN);

	classNamesHash.insert("ParameterResponseCommand" , RCommandResponse_CN);
	classNamesHash.insert("rcommandresponse" , RCommandResponse_CN);

	classNamesHash.insert("ParameterResponseGeneric" , RGenericResponse_CN);
	classNamesHash.insert("rgenericresponse" , RGenericResponse_CN);

	classNamesHash.insert("ParameterResponseDatabase" , RDatabaseResponse_CN);
	classNamesHash.insert("rdatabaseresponse" , RDatabaseResponse_CN);

	classNamesHash.insert("ParameterResponseLocation" , RLocationResponse_CN);
	classNamesHash.insert("rlocationresponse" , RLocationResponse_CN);

	classNamesHash.insert("ParameterDecisionVariableContinuous" , RContinuousDecisionVariable_CN);
	classNamesHash.insert("rcontinuousdecisionvariable" , RContinuousDecisionVariable_CN);

	classNamesHash.insert("ParameterRandomVariableContinuous" , RContinuousRandomVariable_CN);
	classNamesHash.insert("rcontinuousrandomvariable" , RContinuousRandomVariable_CN);

	classNamesHash.insert("ParameterCorrelation" , RCorrelation_CN);
	classNamesHash.insert("rcorrelation" , RCorrelation_CN);

	classNamesHash.insert("Function" , RFunction_CN);
	classNamesHash.insert("rfunction" , RFunction_CN);


	// EVENT MODELS
	classNamesHash.insert("ModelEventOperationType1", ROperationEventType1Model_CN);
	classNamesHash.insert("roperationeventtype1model", ROperationEventType1Model_CN);

	classNamesHash.insert("ModelEventOperationType2", ROperationEventType2Model_CN);
	classNamesHash.insert("roperationeventtype2model", ROperationEventType2Model_CN);

	classNamesHash.insert("ModelEventTimeType1", RTimeEventType1Model_CN);
	classNamesHash.insert("rtimeeventtype1model", RTimeEventType1Model_CN);

	classNamesHash.insert("ModelEventTimeType2", RTimeEventType2Model_CN);
	classNamesHash.insert("rtimeeventtype2model", RTimeEventType2Model_CN);

	// OCUURENCE MODELS

	classNamesHash.insert("ModelOccurrencePoissonPulseProcess" , RPoissonPulseProcessModel_CN);
	classNamesHash.insert("rpoissonpulseprocessmodel" , RPoissonPulseProcessModel_CN);



	// HAZARD MODELS

	classNamesHash.insert("ModelHazardEarthquakeLocationSingleLineSource" , RSingleLineSourceModel_CN);
	classNamesHash.insert("rsinglelinesourcemodel" , RSingleLineSourceModel_CN);

	classNamesHash.insert("ModelHazardEarthquakeLocationMultipointLineSource" , RMultipointLineSourceModel_CN);
	classNamesHash.insert("rmultipointlinesourcemodel" , RMultipointLineSourceModel_CN);

	classNamesHash.insert("ModelHazardEarthquakeLocationCircularAreaSource" , RCircularAreaSourceModel_CN);
	classNamesHash.insert("rcircularareasourcemodel" , RCircularAreaSourceModel_CN);

	classNamesHash.insert("ModelHazardEarthquakeLocationRectangularAreaSource" , RRectangularAreaSourceModel_CN);
	classNamesHash.insert("rrectangularareasourcemodel" , RRectangularAreaSourceModel_CN);

	classNamesHash.insert("ModelHazardEarthquakeLocationQuadrilateralAreaSource" , RQuadrilateralAreaSourceModel_CN);
	classNamesHash.insert("rquadrilateralareasourcemodel" , RQuadrilateralAreaSourceModel_CN);

	classNamesHash.insert("ModelHazardEarthquakeLocationPolygonalAreaSource" , RPolygonalAreaSourceModel_CN);
	classNamesHash.insert("rpolygonalareasourcemodel" , RPolygonalAreaSourceModel_CN);

	classNamesHash.insert("ModelHazardEarthquakeMagnitudeMomentMagnitude" , RMomentMagnitudeModel_CN);
	classNamesHash.insert("rmomentmagnitudemodel" , RMomentMagnitudeModel_CN);

	classNamesHash.insert("ModelHazardEarthquakeMagnitudeBoundedExponentialMagnitude" , RBoundedExponentialMagnitudeModel_CN);
	classNamesHash.insert("rboundedexponentialmagnitudemodel" , RBoundedExponentialMagnitudeModel_CN);

	classNamesHash.insert("ModelHazardEarthquakeIntensityGenericAttenuation" , RGenericAttenuationModel_CN);
	classNamesHash.insert("rgenericattenuationmodel" , RGenericAttenuationModel_CN);

	classNamesHash.insert("ModelHazardEarthquakeIntensityJoynerBoore1981Intensity" , RJoynerBoore1981IntensityModel_CN);
	classNamesHash.insert("rjoynerboore1981intensitymodel" , RJoynerBoore1981IntensityModel_CN);

	classNamesHash.insert("ModelHazardEarthquakeIntensityAtkinson1997Intensity" , RAtkinson1997IntensityModel_CN);
	classNamesHash.insert("ratkinson1997intensitymodel" , RAtkinson1997IntensityModel_CN);

	classNamesHash.insert("ModelHazardEarthquakeIntensityBooreAtkinson2008Intensity" , RBooreAtkinson2008IntensityModel_CN);
	classNamesHash.insert("rbooreatkinson2008intensitymodel" , RBooreAtkinson2008IntensityModel_CN);

	classNamesHash.insert("ModelHazardEarthquakeIntensityAtkinsonBoore2003Intensity" , RAtkinsonBoore2003IntensityModel_CN);
	classNamesHash.insert("ratkinsonboore2003intensitymodel" , RAtkinsonBoore2003IntensityModel_CN);

	classNamesHash.insert("ModelHazardEarthquakeIntensityDuWang2012Intensity" , RDuWang2012IntensityModel_CN);
	classNamesHash.insert("rduwang2012intensitymodel" , RDuWang2012IntensityModel_CN);

	classNamesHash.insert("ModelHazardEarthquakeIntensityCampbellBozorgnia2008Intensity" , RCampbellBozorgnia2008IntensityModel_CN);
	classNamesHash.insert("rcampbellbozorgnia2008intensitymodel" , RCampbellBozorgnia2008IntensityModel_CN);

	classNamesHash.insert("ModelHazardEarthquakeIntensityAbrahamsonSilva2008Intensity" , RAbrahamsonSilva2008IntensityModel_CN);
	classNamesHash.insert("rabrahamsonsilva2008intensitymodel" , RAbrahamsonSilva2008IntensityModel_CN);

	classNamesHash.insert("ModelHazardEarthquakeIntensityChiouYoungs2008Intensity" , RChiouYoungs2008IntensityModel_CN);
	classNamesHash.insert("rchiouyoungs2008intensitymodel" , RChiouYoungs2008IntensityModel_CN);

	classNamesHash.insert("ModelHazardEarthquakeIntensityBooreetal2013Intensity" , RBooreEtAl2013IntensityModel_CN);
	classNamesHash.insert("rbooreetal2013intensitymodel" , RBooreEtAl2013IntensityModel_CN);

	classNamesHash.insert("ModelHazardEarthquakeIntensityAbrahamsonetal2013Intensity" , RAbrahamsonEtAl2013IntensityModel_CN);
	classNamesHash.insert("rabrahamsonetal2013intensitymodel" , RAbrahamsonEtAl2013IntensityModel_CN);

	classNamesHash.insert("ModelHazardEarthquakeIntensityIdriss2013Intensity" , RIdriss2013IntensityModel_CN);
	classNamesHash.insert("ridriss2013intensitymodel" , RIdriss2013IntensityModel_CN);

	classNamesHash.insert("ModelHazardEarthquakeIntensityCampbellBozorgnia2013Intensity" , RCampbellBozorgnia2013IntensityModel_CN);
	classNamesHash.insert("rcampbellbozorgnia2013intensitymodel" , RCampbellBozorgnia2013IntensityModel_CN);

	classNamesHash.insert("ModelHazardEarthquakeIntensityChiouYoungs2013Intensity" , RChiouYoungs2013IntensityModel_CN);
	classNamesHash.insert("rchiouyoungs2013intensitymodel" , RChiouYoungs2013IntensityModel_CN);

	classNamesHash.insert("ModelHazardEarthquakeIntensityRezaeianDerKiureghianArtificialGroundMotion", RRezaeianDerKiureghianArtificialGroundMotionModel_CN);
	classNamesHash.insert("rrezaeianderkiureghianartificialgroundmotionmodel", RRezaeianDerKiureghianArtificialGroundMotionModel_CN);

	classNamesHash.insert("ModelHazardWindSimpleWindVelocity" , RSimpleWindVelocityModel_CN);
	classNamesHash.insert("rsimplewindvelocitymodel" , RSimpleWindVelocityModel_CN);

	classNamesHash.insert("ModelHazardWindSimpleWindPressure" , RSimpleWindPressureModel_CN);
	classNamesHash.insert("rsimplewindpressuremodel" , RSimpleWindPressureModel_CN);

	classNamesHash.insert("ModelHazardSnowSimpleSnowLoad" , RSimpleSnowLoadModel_CN);
	classNamesHash.insert("rsimplesnowloadmodel" , RSimpleSnowLoadModel_CN);



	// INFRASTRUCTURE MODELS

	classNamesHash.insert("ModelInfrastructureSt" , RStModel_CN);
	classNamesHash.insert("rstmodel" , RStModel_CN);

	classNamesHash.insert("ModelInfrastructureBuildingBuildingInformation" , RBuildingInformationModel_CN);
	classNamesHash.insert("rbuildinginformationmodel" , RBuildingInformationModel_CN);

	classNamesHash.insert("ModelInfrastructureBuildingBuildingResponse" , RBuildingResponseModel_CN);
	classNamesHash.insert("rbuildingresponsemodel" , RBuildingResponseModel_CN);

	classNamesHash.insert("ModelInfrastructureBuildingSimpleDeterioration" , RSimpleDeteriorationModel_CN);
	classNamesHash.insert("rsimpledeteriorationmodel" , RSimpleDeteriorationModel_CN);

	classNamesHash.insert("ModelInfrastructureBuildingSoilStructureInteraction" , RSoilStructureInteractionModel_CN);
	classNamesHash.insert("rsoilstructureinteractionmodel" , RSoilStructureInteractionModel_CN);

	// INFRASTRUCTURE MODELS/ BUILDING MODELS
	classNamesHash.insert("ModelInfrastructureBuildingBuildingInspectionTime", RBuildingInspectionTimeModel_CN);
	classNamesHash.insert("rbuildinginspectiontimemodel", RBuildingInspectionTimeModel_CN);

	classNamesHash.insert("ModelInfrastructureBuildingBuildingInspectionCost", RBuildingInspectionCostModel_CN);
	classNamesHash.insert("rbuildinginspectioncostmodel", RBuildingInspectionCostModel_CN);

	classNamesHash.insert("ModelInfrastructureBuildingBuildingRepairMobilizationTime", RBuildingRepairMobilizationTimeModel_CN);
	classNamesHash.insert("rbuildingrepairmobilizationtimemodel", RBuildingRepairMobilizationTimeModel_CN);

	classNamesHash.insert("ModelInfrastructureBuildingBuildingRepairMobilizationCost", RBuildingRepairMobilizationCostModel_CN);
	classNamesHash.insert("rbuildingrepairmobilizationcostmodel", RBuildingRepairMobilizationCostModel_CN);

	classNamesHash.insert("ModelInfrastructureBuildingBuildingRepairTime", RBuildingRepairTimeModel_CN);
	classNamesHash.insert("rbuildingrepairtimemodel", RBuildingRepairTimeModel_CN);

	classNamesHash.insert("ModelInfrastructureBuildingBuildingRestoration", RBuildingRestorationModel_CN);
	classNamesHash.insert("rbuildingrestorationmodel", RBuildingRestorationModel_CN);

	classNamesHash.insert("ModelInfrastructureBuildingBuildingUtility", RBuildingUtilityModel_CN);
	classNamesHash.insert("rbuildingutilitymodel", RBuildingUtilityModel_CN);

	classNamesHash.insert("ModelInfrastructureBuildingBuildingCasualty", RBuildingCasualtyModel_CN);
	classNamesHash.insert("rbuildingcasualtymodel", RBuildingCasualtyModel_CN);

	classNamesHash.insert("ModelInfrastructureBuildingBuildingSearchAndRescueTime", RBuildingSearchAndRescueTimeModel_CN);
	classNamesHash.insert("rbuildingsearchandrescuetimemodel", RBuildingSearchAndRescueTimeModel_CN);

	classNamesHash.insert("ModelInfrastructureBuildingBuildingSearchAndRescueCost", RBuildingSearchAndRescueCostModel_CN);
	classNamesHash.insert("rbuildingsearchandrescuecostmodel", RBuildingSearchAndRescueCostModel_CN);

	classNamesHash.insert("ModelInfrastructureBuildingBuildingFatalityCost", RBuildingFatalityCostModel_CN);
	classNamesHash.insert("rbuildingfatalitycostmodel", RBuildingFatalityCostModel_CN);

	classNamesHash.insert("ModelInfrastructureBuildingBuildingRelocation", RBuildingRelocationModel_CN);
	classNamesHash.insert("rbuildingrelocationmodel", RBuildingRelocationModel_CN);

	// INFRASTRUCTURE/COMMERCIAL MODELS
	classNamesHash.insert("ModelInfrastructureCommercialCommercialInformation", RCommercialInformationModel_CN);
	classNamesHash.insert("rcommercialinformationmodel", RCommercialInformationModel_CN);

	classNamesHash.insert("ModelInfrastructureCommercialCommercialLoss", RCommercialLossModel_CN);
	classNamesHash.insert("rcommerciallossmodel", RCommercialLossModel_CN);

	// INFRASTRUCTURE/POWER MODELS
	classNamesHash.insert("ModelInfrastructurePowerPowerStationDamage", RPowerStationDamageModel_CN);
	classNamesHash.insert("rpowerstationdamagemodel", RPowerStationDamageModel_CN);

	classNamesHash.insert("ModelInfrastructurePowerPowerStationInformation", RPowerStationInformationModel_CN);
	classNamesHash.insert("rpowerstationinformationmodel", RPowerStationInformationModel_CN);

	classNamesHash.insert("ModelInfrastructurePowerPowerStationInspectionTime", RPowerStationInspectionTimeModel_CN);
	classNamesHash.insert("rpowerstationinspectiontimemodel", RPowerStationInspectionTimeModel_CN);

	classNamesHash.insert("ModelInfrastructurePowerPowerStationInspectionCost", RPowerStationInspectionCostModel_CN);
	classNamesHash.insert("rpowerstationinspectioncostmodel", RPowerStationInspectionCostModel_CN);

	classNamesHash.insert("ModelInfrastructurePowerPowerStationServiceability", RPowerStationServiceabilityModel_CN);
	classNamesHash.insert("rpowerstationserviceabilitymodel", RPowerStationServiceabilityModel_CN);

	classNamesHash.insert("ModelInfrastructurePowerPowerStationRepairTime", RPowerStationRepairTimeModel_CN);
	classNamesHash.insert("rpowerstationrepairtimemodel", RPowerStationRepairTimeModel_CN);

	classNamesHash.insert("ModelInfrastructurePowerPowerStationRepairCost", RPowerStationRepairCostModel_CN);
	classNamesHash.insert("rpowerstationrepaircostmodel", RPowerStationRepairCostModel_CN);

	// INFRASTRUCTURE/WATER MODELS
	classNamesHash.insert("ModelInfrastructureWaterWaterStationDamage", RWaterStationDamageModel_CN);
	classNamesHash.insert("rwaterstationdamagemodel", RWaterStationDamageModel_CN);

	classNamesHash.insert("ModelInfrastructureWaterWaterStationInformation", RWaterStationInformationModel_CN);
	classNamesHash.insert("rwaterstationinformationmodel", RWaterStationInformationModel_CN);

	classNamesHash.insert("ModelInfrastructureWaterWaterStationInspectionTime", RWaterStationInspectionTimeModel_CN);
	classNamesHash.insert("rwaterstationinspectiontimemodel", RWaterStationInspectionTimeModel_CN);

	classNamesHash.insert("ModelInfrastructureWaterWaterStationInspectionCost", RWaterStationInspectionCostModel_CN);
	classNamesHash.insert("rwaterstationinspectioncostmodel", RWaterStationInspectionCostModel_CN);

	classNamesHash.insert("ModelInfrastructureWaterWaterStationServiceability", RWaterStationServiceabilityModel_CN);
	classNamesHash.insert("rwaterstationserviceabilitymodel", RWaterStationServiceabilityModel_CN);

	classNamesHash.insert("ModelInfrastructureWaterWaterStationFunctionality", RWaterStationFunctionalityModel_CN);
	classNamesHash.insert("rwaterstationfunctionalitymodel", RWaterStationFunctionalityModel_CN);

	classNamesHash.insert("ModelInfrastructureWaterWaterStationRepairTime", RWaterStationRepairTimeModel_CN);
	classNamesHash.insert("rwaterstationrepairtimemodel", RWaterStationRepairTimeModel_CN);

	classNamesHash.insert("ModelInfrastructureWaterWaterStationRepairCost", RWaterStationRepairCostModel_CN);
	classNamesHash.insert("rwaterstationrepaircostmodel", RWaterStationRepairCostModel_CN);

	// INFRASTRUCTURE/HEALTHCARE MODELS
	classNamesHash.insert("ModelInfrastructureHealthcareHealthcareCapacity", RHealthcareCapacityModel_CN);
	classNamesHash.insert("rhealthcarecapacitymodel", RHealthcareCapacityModel_CN);

	classNamesHash.insert("ModelInfrastructureHealthcareHealthcareInformation", RHealthcareInformationModel_CN);
	classNamesHash.insert("rhealthcareinformationmodel", RHealthcareInformationModel_CN);

	classNamesHash.insert("ModelInfrastructureHealthcareHealthcareRestoration", RHealthcareRestorationModel_CN);
	classNamesHash.insert("rhealthcarerestorationmodel", RHealthcareRestorationModel_CN);

	classNamesHash.insert("ModelInfrastructureHealthcareHealthcareDemand", RHealthcareDemandModel_CN);
	classNamesHash.insert("rhealthcaredemandmodel", RHealthcareDemandModel_CN);

	classNamesHash.insert("ModelInfrastructureHealthcareHealthcareTreatmentCost", RHealthcareTreatmentCostModel_CN);
	classNamesHash.insert("rhealthcaretreatmentcostmodel", RHealthcareTreatmentCostModel_CN);

	classNamesHash.insert("ModelInfrastructureHealthcareHealthcareTreatmentTime", RHealthcareTreatmentTimeModel_CN);
	classNamesHash.insert("rhealthcaretreatmenttimemodel", RHealthcareTreatmentTimeModel_CN);

	// CONSEQUENCE MODELS

	classNamesHash.insert("ModelConsequenceEconomicSimpleDamageCurve" , RSimpleDamageCurveModel_CN);
	classNamesHash.insert("rsimpledamagecurvemodel" , RSimpleDamageCurveModel_CN);

	classNamesHash.insert("ModelConsequenceEconomicSimpleBuildingRepairCost" , RSimpleBuildingRepairCostModel_CN);
	classNamesHash.insert("rsimplebuildingrepaircostmodel" , RSimpleBuildingRepairCostModel_CN);

	classNamesHash.insert("ModelConsequenceEconomicFEMA-NIBSBridgeDamage" , RFemaNibsBridgeDamageModel_CN);
	classNamesHash.insert("rfemanibsbridgedamagemodel" , RFemaNibsBridgeDamageModel_CN);

	classNamesHash.insert("ModelConsequenceEconomicFEMA-NIBSBuildingRepairCost" , RFemaNibsBuildingRepairCostModel_CN);
	classNamesHash.insert("rfemanibsbuildingrepaircostmodel" , RFemaNibsBuildingRepairCostModel_CN);

	classNamesHash.insert("ModelConsequenceEconomicRegionalLoss" , RRegionalLossModel_CN);
	classNamesHash.insert("rregionallossmodel" , RRegionalLossModel_CN);

	classNamesHash.insert("ModelConsequenceEconomicDiscounting" , RDiscountingModel_CN);
	classNamesHash.insert("rdiscountingmodel" , RDiscountingModel_CN);

	classNamesHash.insert("ModelConsequenceEconomicBuildingDamage" , RBuildingDamageModel_CN);
	classNamesHash.insert("rbuildingdamagemodel" , RBuildingDamageModel_CN);

	classNamesHash.insert("ModelConsequenceEconomicBuildingRepairCost" , RBuildingRepairCostModel_CN);
	classNamesHash.insert("rbuildingrepaircostmodel" , RBuildingRepairCostModel_CN);

	classNamesHash.insert("ModelConsequenceEconomicMasonryBuildingDamage" , RMasonryBuildingDamageModel_CN);
	classNamesHash.insert("rmasonrybuildingdamagemodel" , RMasonryBuildingDamageModel_CN);

	classNamesHash.insert("ModelConsequenceEconomicMasonryBuildingRetrofitCost" , RMasonryBuildingRetrofitCostModel_CN);
	classNamesHash.insert("rmasonrybuildingretrofitcostmodel" , RMasonryBuildingRetrofitCostModel_CN);

	classNamesHash.insert("ModelConsequenceEnvironmentalSimpleBuildingLifeCycleEnergyUsage" , RSimpleBuildingLifeCycleEnergyUsageModel_CN);
	classNamesHash.insert("rsimplebuildinglifecycleenergyusagemodel" , RSimpleBuildingLifeCycleEnergyUsageModel_CN);

	classNamesHash.insert("ModelConsequenceEnvironmentalDetailedBuildingLifeCycleEnergyUsage" , RDetailedBuildingLifeCycleEnergyUsageModel_CN);
	classNamesHash.insert("rdetailedbuildinglifecycleenergyusagemodel" , RDetailedBuildingLifeCycleEnergyUsageModel_CN);

	classNamesHash.insert("ModelConsequenceEnvironmentalSimpleBuildingLifeCycleWaterUsage" , RSimpleBuildingLifeCycleWaterUsageModel_CN);
	classNamesHash.insert("rsimplebuildinglifecyclewaterusagemodel" , RSimpleBuildingLifeCycleWaterUsageModel_CN);

	classNamesHash.insert("ModelConsequenceEnvironmentalDetailedBuildingLifeCycleWaterUsage" , RDetailedBuildingLifeCycleWaterUsageModel_CN);
	classNamesHash.insert("rdetailedbuildinglifecyclewaterusagemodel" , RDetailedBuildingLifeCycleWaterUsageModel_CN);

	classNamesHash.insert("ModelConsequenceEnvironmentalSimpleBuildingLifeCycleGlobalWarmingPotential" , RSimpleBuildingLifeCycleGWPModel_CN);
	classNamesHash.insert("rsimplebuildinglifecyclegwpmodel" , RSimpleBuildingLifeCycleGWPModel_CN);

	classNamesHash.insert("ModelConsequenceEnvironmentalDetailedBuildingLifeCycleGlobalWarmingPotential" , RDetailedBuildingLifeCycleGWPModel_CN);
	classNamesHash.insert("rdetailedbuildinglifecyclegwpmodel" , RDetailedBuildingLifeCycleGWPModel_CN);

	classNamesHash.insert(QString("Model Consequence Economic Gypsum Wallboard Partition Damage").remove(' '), RGypsumWallboardPartitionDamageModel_CN);
	classNamesHash.insert(QString("RGypsumWallboardPartitionDamageModel").toLower(), RGypsumWallboardPartitionDamageModel_CN);

	classNamesHash.insert(QString("Model Consequence Economic Suspended Ceiling Damage").remove(' '), RSuspendedCeilingDamageModel_CN);
	classNamesHash.insert(QString("RSuspendedCeilingDamageModel").toLower(), RSuspendedCeilingDamageModel_CN);

	classNamesHash.insert(QString("Model Consequence Economic Traction Elevator Damage").remove(' '), RTractionElevatorDamageModel_CN);
	classNamesHash.insert(QString("RTractionElevatorDamageModel").toLower(), RTractionElevatorDamageModel_CN);

	classNamesHash.insert(QString("Model Consequence Economic Concrete Special Moment Frame Interior Joint Damage").remove(' '), RConcreteSpecialMomentFrameInteriorJointDamageModel_CN);
	classNamesHash.insert(QString("RConcreteSpecialMomentFrameInteriorJointDamageModel").toLower(), RConcreteSpecialMomentFrameInteriorJointDamageModel_CN);

	classNamesHash.insert(QString("Model Consequence Economic Concrete Intermediate Moment Frame Interior Joint Damage").remove(' '), RConcreteIntermediateMomentFrameInteriorJointDamageModel_CN);
	classNamesHash.insert(QString("RConcreteIntermediateMomentFrameInteriorJointDamageModel").toLower(), RConcreteIntermediateMomentFrameInteriorJointDamageModel_CN);

	classNamesHash.insert(QString("Model Consequence Economic Concrete Moment Frame Exterior Joint Damage").remove(' '), RConcreteMomentFrameExteriorJointDamageModel_CN);
	classNamesHash.insert(QString("RConcreteMomentFrameExteriorJointDamageModel").toLower(), RConcreteMomentFrameExteriorJointDamageModel_CN);

	classNamesHash.insert(QString("Model Consequence Economic Curtain Wall Damage").remove(' '), RCurtainWallDamageModel_CN);
	classNamesHash.insert(QString("RCurtainWallDamageModel").toLower(), RCurtainWallDamageModel_CN);

	classNamesHash.insert(QString("Model Consequence Economic Component Loss").remove(' '), RComponentLossModel_CN);
	classNamesHash.insert(QString("RComponentLossModel").toLower(), RComponentLossModel_CN);

	classNamesHash.insert("ModelConsequenceSocialInjurySocialLoss", RInjurySocialLossModel_CN);
	classNamesHash.insert("rinjurysociallossmodel", RInjurySocialLossModel_CN);

	classNamesHash.insert("ModelConsequenceSocialPopulationDensity", RPopulationDensityModel_CN);
	classNamesHash.insert("rpopulationdensitymodel", RPopulationDensityModel_CN);

	classNamesHash.insert("ModelConsequenceEconomicRelocationCost", RRelocationCostModel_CN);
	classNamesHash.insert("rrelocationcostmodel", RRelocationCostModel_CN);

	classNamesHash.insert("ModelConsequenceSocialLifeQualityReduction", RLifeQualityReductionModel_CN);
	classNamesHash.insert("rlifequalityreductionmodel", RLifeQualityReductionModel_CN);

	// EXTERNAL SOFTWARE
	
	classNamesHash.insert("ModelExternalSoftwareMatlab" , RMatlabModel_CN);
	classNamesHash.insert("rmatlabmodel" , RMatlabModel_CN);

	classNamesHash.insert("ModelExternalSoftwareOpenSees" , ROpenSeesModel_CN);
	classNamesHash.insert("ropenseesmodel" , ROpenSeesModel_CN);

	classNamesHash.insert("ModelExternalSoftwareANSYS" , RANSYSModel_CN);
	classNamesHash.insert("ransysmodel" , RANSYSModel_CN);

	classNamesHash.insert("ModelExternalSoftwareAbaqus" , RAbaqusModel_CN);
	classNamesHash.insert("rabaqusmodel" , RAbaqusModel_CN);

	classNamesHash.insert("ModelExternalSoftwareUSFOS" , RUSFOSModel_CN);
	classNamesHash.insert("rusfosmodel" , RUSFOSModel_CN);

	classNamesHash.insert("ModelExternalSoftwareEMME" , REMMEModel_CN);
	classNamesHash.insert("remmemodel" , REMMEModel_CN);

	classNamesHash.insert("ModelExternalSoftwareSAP2000" , RSAP2000Model_CN);
	classNamesHash.insert("rsap2000model" , RSAP2000Model_CN);



	// GENERIC MODELS

	classNamesHash.insert("ModelGenericAlgebraicExpression" , RAlgebraicExpressionModel_CN);
	classNamesHash.insert("ralgebraicexpressionmodel" , RAlgebraicExpressionModel_CN);

	classNamesHash.insert("ModelGenericRandomVariableWithRandomParameters" , RRandomVariableWithRandomParametersModel_CN);
	classNamesHash.insert("rrandomvariablewithrandomparametersmodel" , RRandomVariableWithRandomParametersModel_CN);

	classNamesHash.insert("ModelGenericRootFinding" , RRootFindingModel_CN);
	classNamesHash.insert("rrootfindingmodel" , RRootFindingModel_CN);

	classNamesHash.insert("ModelGenericScript" , RScriptModel_CN);
	classNamesHash.insert("rscriptmodel" , RScriptModel_CN);



	// ANALYSIS TOOLS

	classNamesHash.insert("NonlinearSingleConstraintSolverStepper" , RStepperNonlinSingleConstrSolver_CN);
	classNamesHash.insert("rsteppernonlinsingleconstrsolver" , RStepperNonlinSingleConstrSolver_CN);

	classNamesHash.insert("StepSizeSearcherFixed" , RFixedStepSizeSearcher_CN);
	classNamesHash.insert("rfixedstepsizesearcher" , RFixedStepSizeSearcher_CN);

	classNamesHash.insert("StepSizeSearcherArmijo" , RArmijoStepSizeSearcher_CN);
	classNamesHash.insert("rarmijostepsizesearcher" , RArmijoStepSizeSearcher_CN);

	classNamesHash.insert("StepDirectionSearcheriHLRF" , RHLRFStepDirectionSearcher_CN);
	classNamesHash.insert("rhlrfstepdirectionsearcher" , RHLRFStepDirectionSearcher_CN);

	classNamesHash.insert("StepDirectionSearcherPlakHe" , RPolakHeStepDirectionSearcher_CN);
	classNamesHash.insert("rpolakhestepdirectionsearcher" , RPolakHeStepDirectionSearcher_CN);

	classNamesHash.insert("StepDirectionSearcherSQP" , RSQPStepDirectionSearcher_CN);
	classNamesHash.insert("rsqpstepdirectionsearcher" , RSQPStepDirectionSearcher_CN);

	classNamesHash.insert("TransformerNoCorrelation" , RNoCorrelationTransformer_CN);
	classNamesHash.insert("rnocorrelationtransformer" , RNoCorrelationTransformer_CN);

	classNamesHash.insert("TransformerNataf" , RNatafTransformer_CN);
	classNamesHash.insert("rnataftransformer" , RNatafTransformer_CN);

	classNamesHash.insert("ConvergenceCheckerStandard" , RStandardConvergenceChecker_CN);
	classNamesHash.insert("rstandardconvergencechecker" , RStandardConvergenceChecker_CN);

	classNamesHash.insert("ConvergenceCheckerOptimalityCondition" , ROptimalityConditionConvergenceChecker_CN);
	classNamesHash.insert("roptimalityconditionconvergencechecker" , ROptimalityConditionConvergenceChecker_CN);

	classNamesHash.insert("RandomNumberGeneratorIndependentNormal" , RIndependentNormalRandomNumberGenerator_CN);
	classNamesHash.insert("rindependentnormalrandomnumbergenerator" , RIndependentNormalRandomNumberGenerator_CN);

	classNamesHash.insert("AccumulatorFailureProbability" , RFailureProbabilityAccumulator_CN);
	classNamesHash.insert("rfailureprobabilityaccumulator" , RFailureProbabilityAccumulator_CN);

	classNamesHash.insert("AccumulatorHistogram" , RHistogramAccumulator_CN);
	classNamesHash.insert("rhistogramaccumulator" , RHistogramAccumulator_CN);

	classNamesHash.insert("MeritCheckerAdkZhang" , RAdkZhangMeritChecker_CN);
	classNamesHash.insert("radkzhangmeritchecker" , RAdkZhangMeritChecker_CN);

	classNamesHash.insert("AnalyzerScenarioSampling" , RScenarioSamplingAnalyzer_CN);
	classNamesHash.insert("rscenariosamplinganalyzer" , RScenarioSamplingAnalyzer_CN);

	classNamesHash.insert("AnalyzerFunctionEvaluation" , RFunctionEvaluationAnalyzer_CN);
	classNamesHash.insert("rfunctionevaluationanalyzer" , RFunctionEvaluationAnalyzer_CN);

	classNamesHash.insert("AnalyzerFORM" , RFORMAnalyzer_CN);
	classNamesHash.insert("rformanalyzer" , RFORMAnalyzer_CN);

	classNamesHash.insert("AnalyzerSORM", RSORMAnalyzer_CN);
	classNamesHash.insert("rsormanalyzer", RSORMAnalyzer_CN);

	classNamesHash.insert("AnalyzerFOSM" , RFOSMAnalyzer_CN);
	classNamesHash.insert("rfosmanalyzer" , RFOSMAnalyzer_CN);

	classNamesHash.insert("AnalyzerSampling" , RSamplingAnalyzer_CN);
	classNamesHash.insert("rsamplinganalyzer" , RSamplingAnalyzer_CN);

	classNamesHash.insert("AnalyzerLoadCoincidence" , RLoadCoincidenceAnalyzer_CN);
	classNamesHash.insert("rloadcoincidenceanalyzer" , RLoadCoincidenceAnalyzer_CN);

	classNamesHash.insert("AnalyzerDiscreteEventSampling", RDiscreteEventSamplingAnalyzer_CN);
	classNamesHash.insert("rdiscreteeventsamplinganalyzer", RDiscreteEventSamplingAnalyzer_CN);


	// St classes

	classNamesHash.insert("SOESolverLUSolver" , SLUSolver_CN);
	classNamesHash.insert("slusolver" , SLUSolver_CN);

	classNamesHash.insert("AssemblerTransformationMatrices" , STransformationMatrixAssembler_CN);
	classNamesHash.insert("stransformationmatrixassembler" , STransformationMatrixAssembler_CN);

	classNamesHash.insert("Node" , SNode_CN);
	classNamesHash.insert("snode" , SNode_CN);

	classNamesHash.insert("RestraintFixedNode" , SFixedNode_CN);
	classNamesHash.insert("sfixednode" , SFixedNode_CN);

	classNamesHash.insert("ConnectionRigidLink" , SRigidLink_CN);
	classNamesHash.insert("srigidlink" , SRigidLink_CN);

	classNamesHash.insert("LoadNodalLoad" , SNodalLoad_CN);
	classNamesHash.insert("snodalload" , SNodalLoad_CN);

	classNamesHash.insert("ElementTruss" , STrussElement_CN);
	classNamesHash.insert("strusselement" , STrussElement_CN);

	classNamesHash.insert("ElementFrame" , SFrameElement_CN);
	classNamesHash.insert("sframeelement" , SFrameElement_CN);

	classNamesHash.insert("ElementQuad4" , SQuad4Element_CN);
	classNamesHash.insert("squad4element" , SQuad4Element_CN);

	classNamesHash.insert("MemberWall" , SWallMember_CN);
	classNamesHash.insert("swallmember" , SWallMember_CN);

	classNamesHash.insert("SectionGeneric" , SGenericSection_CN);
	classNamesHash.insert("sgenericsection" , SGenericSection_CN);

	classNamesHash.insert("SectionRectangular" , SRectangularSection_CN);
	classNamesHash.insert("srectangularsection" , SRectangularSection_CN);

	classNamesHash.insert("Material" , SMaterial_CN);
	classNamesHash.insert("smaterial" , SMaterial_CN);



	/* BASE CLASSES */



	// TOP LEVEL DOMAIN BRANCHES
	classNamesHash.insert("Domain" , RDomain_CN);

	// TOP LEVEL ANALYSIS BRANCHES
	classNamesHash.insert("Analysis" , RAnalysis_CN);

	// PARAMETERS AND FUNCTION
	classNamesHash.insert("Parameter" , RParameter_CN);
	classNamesHash.insert("ParameterRandomVariable" , RParameterRandomVariable_CN);
	classNamesHash.insert("ParameterDecisionVariable" , RParameterDecisionVariable_CN);
	classNamesHash.insert("ParameterResponse" , RParameterResponse_CN);

	// TOP LEVEL MODEL BRANCHES
	classNamesHash.insert("Model" , RModel_CN);

	// EVENT MODELS
	classNamesHash.insert("ModelEvent", RModelEvent_CN);

	// OCCURRENCE MODELS
	classNamesHash.insert("ModelOccurrence" , RModelOccurrence_CN);

	// HAZARD MODELS
	classNamesHash.insert("ModelHazard" , RModelHazard_CN);
	classNamesHash.insert("ModelHazardEarthquake" , RModelHazardEarthquake_CN);
	classNamesHash.insert("ModelHazardEarthquakeLocation" , RModelHazardEarthquakeLocation_CN);
	classNamesHash.insert("ModelHazardEarthquakeMagnitude" , RModelHazardEarthquakeMagnitude_CN);
	classNamesHash.insert("ModelHazardEarthquakeIntensity" , RModelHazardEarthquakeIntensity_CN);
	classNamesHash.insert("ModelHazardWind" , RModelHazardWind_CN);
	classNamesHash.insert("ModelHazardSnow" , RModelHazardSnow_CN);

	// INFRASTRUCTURE MODELS
	classNamesHash.insert("ModelInfrastructure" , RModelInfrastructure_CN);

	// INFRASTRUCTURE MODELS/ BUILDING MODELS
	classNamesHash.insert("ModelInfrastructureBuilding", RModelInfrastructureBuilding_CN);

	// INFRASTRUCTURE MODELS/ COMMERICIAL MODELS
	classNamesHash.insert("ModelInfrastructureCommercial", RModelInfrastructureCommercial_CN);

	// INFRASTRUCTURE MODELS/ HEALTHCARE MODELS
	classNamesHash.insert("ModelInfrastructureHealthcare", RModelInfrastructureHealthcare_CN);

	// INFRASTRUCTURE/POWER MODELS
	classNamesHash.insert("ModelInfrastructurePower", RModelInfrastructurePower_CN);

	// INFRASTRUCTURE/WATER MODELS
	classNamesHash.insert("ModelInfrastructureWater", RModelInfrastructureWater_CN);

	// CONSEQUENCE MODELS
	classNamesHash.insert("ModelConsequence" , RModelConsequence_CN);
	classNamesHash.insert("ModelConsequenceEconomic" , RModelConsequenceEconomic_CN);
	classNamesHash.insert("ModelConsequenceEnvironmental" , RModelConsequenceEnvironmental_CN);
	classNamesHash.insert("ModelConsequenceSocial", RModelConsequenceSocial_CN);
	
	// EXTERNAL SOFTWARE
	classNamesHash.insert("ModelExternalSoftware" , RModelExternalSoftware_CN);

	// GENERIC MODELS
	classNamesHash.insert("ModelGeneric" , RModelGeneric_CN);

	// ANALYSIS TOOLS
	classNamesHash.insert("Analyzer" , RAnalyzer_CN);
	classNamesHash.insert("ConvergenceChecker" , RConvergenceChecker_CN);
	classNamesHash.insert("NonlinearSingleConstraintSolver" , RNonlinearSingleConstraintSolver_CN);
	classNamesHash.insert("StepDirectionSearcher" , RStepDirectionSearcher_CN);
	classNamesHash.insert("StepSizeSearcher" , RStepSizeSearcher_CN);
	classNamesHash.insert("Transformer" , RTransformer_CN);
	classNamesHash.insert("RandomNumberGenerator" , RRandomNumberGenerator_CN);
	classNamesHash.insert("Accumulator" , RAccumulator_CN);
	classNamesHash.insert("MeritChecker" , RMeritChecker_CN);
}

void RDomain::newAnalysisStarted(){

	theNumberOfOngoingAnalysis++;

	// if it is only this analysis, signal!
	if (theNumberOfOngoingAnalysis == 1){
		emit newAnalysisStartedSignal();
	}
}

void RDomain::anAnalysisEnded(){

	theNumberOfOngoingAnalysis--;

	// if there is no more running analysis, signal!
	if (theNumberOfOngoingAnalysis == 0){
		emit allAnalysisEndedSignal();
	}
}

bool RDomain::thereIsOngoingAnalysis(){

	return (theNumberOfOngoingAnalysis > 0);
}

bool RDomain::canBeRunParallelly() {
	return false;
}