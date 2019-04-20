// Last modified by Mojtaba on March 19, 2014

#ifndef RDomain_H
#define RDomain_H

#include "RObject.h"

#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>

class RModel;
class RParameter;
class RRandomVariable;
class RDecisionVariable;
class RConstant;
class RResponse;

// ENUM for possible name of classes
enum ClassName{

	// no value for undefined strings
	NoValue_CN ,


	/* CLASSES */


	// PARAMETERS ANDD FUNCTION
	RLocation_CN , RConstant_CN , RTime_CN , RFileResponse_CN , RCommandResponse_CN , RGenericResponse_CN , RDatabaseResponse_CN , RLocationResponse_CN , RContinuousDecisionVariable_CN , RContinuousRandomVariable_CN , RCorrelation_CN , RFunction_CN ,

	// EVENT MODELS
	ROperationEventType1Model_CN,
	ROperationEventType2Model_CN,

	RTimeEventType1Model_CN,
	RTimeEventType2Model_CN,

	// OCCURENCE MODELS
	RPoissonPulseProcessModel_CN , 

	// HAZARD MODELS
	RSingleLineSourceModel_CN , RMultipointLineSourceModel_CN , RCircularAreaSourceModel_CN , RRectangularAreaSourceModel_CN , RQuadrilateralAreaSourceModel_CN , RPolygonalAreaSourceModel_CN , RMomentMagnitudeModel_CN , RBoundedExponentialMagnitudeModel_CN , RGenericAttenuationModel_CN , RJoynerBoore1981IntensityModel_CN , RAtkinson1997IntensityModel_CN , RBooreAtkinson2008IntensityModel_CN , RAtkinsonBoore2003IntensityModel_CN , RDuWang2012IntensityModel_CN , RCampbellBozorgnia2008IntensityModel_CN , RAbrahamsonSilva2008IntensityModel_CN , RChiouYoungs2008IntensityModel_CN, RBooreEtAl2013IntensityModel_CN, RAbrahamsonEtAl2013IntensityModel_CN, RIdriss2013IntensityModel_CN, RCampbellBozorgnia2013IntensityModel_CN, RChiouYoungs2013IntensityModel_CN , RRezaeianDerKiureghianArtificialGroundMotionModel_CN, RSimpleWindVelocityModel_CN , RSimpleWindPressureModel_CN , RSimpleSnowLoadModel_CN ,

	// INFRASTRUCTURE MODELS
	RStModel_CN , 
	RBuildingInformationModel_CN , RBuildingResponseModel_CN , RSimpleDeteriorationModel_CN , RSoilStructureInteractionModel_CN , 
	RBuildingInspectionTimeModel_CN, RBuildingInspectionCostModel_CN, RBuildingRepairMobilizationTimeModel_CN, RBuildingRepairMobilizationCostModel_CN, RBuildingRepairTimeModel_CN, RBuildingRestorationModel_CN, RBuildingUtilityModel_CN, RBuildingCasualtyModel_CN, RBuildingSearchAndRescueTimeModel_CN, RBuildingSearchAndRescueCostModel_CN, RBuildingFatalityCostModel_CN, RBuildingRelocationModel_CN,
	RCommercialInformationModel_CN, RCommercialLossModel_CN ,
	RPowerStationInformationModel_CN , RPowerStationDamageModel_CN , RPowerStationRepairTimeModel_CN , RPowerStationRepairCostModel_CN , RPowerStationServiceabilityModel_CN , RPowerStationInspectionTimeModel_CN , RPowerStationInspectionCostModel_CN,
	RWaterStationInformationModel_CN, RWaterStationDamageModel_CN, RWaterStationRepairTimeModel_CN, RWaterStationRepairCostModel_CN, RWaterStationServiceabilityModel_CN, RWaterStationFunctionalityModel_CN, RWaterStationInspectionTimeModel_CN, RWaterStationInspectionCostModel_CN,

	RHealthcareInformationModel_CN , RHealthcareCapacityModel_CN, RHealthcareDemandModel_CN, RHealthcareRestorationModel_CN, RHealthcareTreatmentCostModel_CN, RHealthcareTreatmentTimeModel_CN,

	// CONSEQUENCE MODELS
	RSimpleDamageCurveModel_CN , RSimpleBuildingRepairCostModel_CN , RFemaNibsBridgeDamageModel_CN , RFemaNibsBuildingRepairCostModel_CN , RRegionalLossModel_CN , RDiscountingModel_CN , RBuildingDamageModel_CN , RBuildingRepairCostModel_CN, RMasonryBuildingDamageModel_CN, RMasonryBuildingRetrofitCostModel_CN , RSimpleBuildingLifeCycleEnergyUsageModel_CN , RDetailedBuildingLifeCycleEnergyUsageModel_CN , RSimpleBuildingLifeCycleWaterUsageModel_CN , RDetailedBuildingLifeCycleWaterUsageModel_CN , RSimpleBuildingLifeCycleGWPModel_CN , RDetailedBuildingLifeCycleGWPModel_CN , RRelocationCostModel_CN,
	
	RGypsumWallboardPartitionDamageModel_CN , RSuspendedCeilingDamageModel_CN , RTractionElevatorDamageModel_CN, RConcreteSpecialMomentFrameInteriorJointDamageModel_CN, RConcreteIntermediateMomentFrameInteriorJointDamageModel_CN , RConcreteMomentFrameExteriorJointDamageModel_CN , RCurtainWallDamageModel_CN, RComponentLossModel_CN,

	RInjurySocialLossModel_CN , RPopulationDensityModel_CN , RLifeQualityReductionModel_CN,

	// EXTERNAL SOFTWARE
	RMatlabModel_CN , ROpenSeesModel_CN , RANSYSModel_CN , RAbaqusModel_CN , RUSFOSModel_CN , REMMEModel_CN , RSAP2000Model_CN , 

	// GENERIC MODELS
	RAlgebraicExpressionModel_CN , RRandomVariableWithRandomParametersModel_CN , RRootFindingModel_CN , RScriptModel_CN , 

	// ANALYSIS TOOLS
	RStepperNonlinSingleConstrSolver_CN , RFixedStepSizeSearcher_CN , RArmijoStepSizeSearcher_CN , RHLRFStepDirectionSearcher_CN , RPolakHeStepDirectionSearcher_CN , RSQPStepDirectionSearcher_CN , RNoCorrelationTransformer_CN , RNatafTransformer_CN , RStandardConvergenceChecker_CN , ROptimalityConditionConvergenceChecker_CN , RIndependentNormalRandomNumberGenerator_CN , RFailureProbabilityAccumulator_CN , RHistogramAccumulator_CN , RAdkZhangMeritChecker_CN , RScenarioSamplingAnalyzer_CN , RFunctionEvaluationAnalyzer_CN , RFORMAnalyzer_CN , RSORMAnalyzer_CN , RFOSMAnalyzer_CN , RSamplingAnalyzer_CN , RLoadCoincidenceAnalyzer_CN , RDiscreteEventSamplingAnalyzer_CN ,

	// St classes
	SLUSolver_CN , STransformationMatrixAssembler_CN , SNode_CN , SFixedNode_CN , SRigidLink_CN , SNodalLoad_CN , STrussElement_CN , SFrameElement_CN , SQuad4Element_CN , SWallMember_CN , SGenericSection_CN , SRectangularSection_CN , SMaterial_CN , 



	/* BASE CLASSES */


	// TOP LEVEL DOMAIN BRANCHES
	RDomain_CN , 

	// TOP LEVEL ANALYSIS BRANCHES
	RAnalysis_CN ,

	// PARAMETERS AND FUNCTION
	RParameter_CN , RParameterRandomVariable_CN , RParameterDecisionVariable_CN , RParameterResponse_CN , 

	// TOP LEVEL MODEL BRANCHES
	RModel_CN ,

	// EVENT MODELS
	RModelEvent_CN ,
	
	// OCCURRENCE MODELS
	RModelOccurrence_CN ,

	// HAZARD MODELS
	RModelHazard_CN , RModelHazardEarthquake_CN , RModelHazardEarthquakeLocation_CN , RModelHazardEarthquakeMagnitude_CN , RModelHazardEarthquakeIntensity_CN , RModelHazardWind_CN , RModelHazardSnow_CN , 

	// INFRASTRUCTURE MODELS
	RModelInfrastructure_CN , 
	RModelInfrastructureBuilding_CN,
	RModelInfrastructureCommercial_CN,
	RModelInfrastructurePower_CN ,
	RModelInfrastructureWater_CN,
	RModelInfrastructureHealthcare_CN,

	// CONSEQUENCE MODELS
	RModelConsequence_CN , RModelConsequenceEconomic_CN , RModelConsequenceSocial_CN , RModelConsequenceEnvironmental_CN ,

	// EXTERNAL SOFTWARE
	RModelExternalSoftware_CN , 

	// GENERIC MODELS
	RModelGeneric_CN ,

	// ANALYSIS TOOLS
	RAnalyzer_CN , RConvergenceChecker_CN , RNonlinearSingleConstraintSolver_CN , RStepDirectionSearcher_CN , RStepSizeSearcher_CN , RTransformer_CN , RRandomNumberGenerator_CN , RAccumulator_CN , RMeritChecker_CN
};


class RDomain : public RObject
{
	Q_OBJECT

public:
	RDomain(QObject *parent, QString name);
	~RDomain();

	// Creates a new RObject given its object name, and class name
	int createObject(QString name, QString passedClassName, bool isAutoGenerated = true);

	// Deletes an RObject given its name
	int deleteObject(QString name);

	// Renames an RObject after checking if the name complies with the naming rules
	int renameObject(QObject *passedObject, QString name);

	// Returns a pointer to the last instantiated RObject 
	QObject *getLastAddedObject() const;

	// Checks if a given object name compies with the naming rules
	int nameIsValid(QString name, bool checkForDuplicate = true);

	// Sets a flag that indicates if Rt is currently in the process of opening an input file
	void setInputFileBeingOpened(bool value);

	// Returns a list of RObject pointer that are derived from the passed class name
	QObjectList getObjectList(QString passedClassName);

	// Returns a string-list of object names from the passed list of object pointers
	QStringList getStringList(QObjectList objectList);

	// Returns the value of a meta-property named "propertyName" for the passed object as a string
	QString getProperty(const QObject *passedObject, QString propertyName) const;

	// Sets the value of a meta-property named "propertyName" for the passed object to the passed value
	int setProperty(QObject *passedObject, QString propertyName, QString propertyValue);

	// Retunrs the current file path of Rt
	QString getCurrentFilePath() const;

	// Sets the current file path of Rt to the passed value
	void setCurrentFilePath(QString value);

	// Sorts the passed QObject list by their object names
	int sort(QObjectList *passedList);

	// Overload method that sorts the passed model list by their object names
	int sort(QList<RModel *> *passedList);

	// Overload method that sorts the passed parameter list by their object names
	int sort(QList<RParameter *> *passedList);

	// Overload method that sorts the passed rv list according to a sorting measure, which can be Name, Alpha, or Gamma
	int sort(QList<RRandomVariable *> *passedList, QString sortingMeasure = QString("Name"));

	// Overload method that sorts the passed dv list according to a sorting measure, which can be Name or Sensitivity
	int sort(QList<RDecisionVariable *> *passedList, QString sortingMeasure = QString("Name"));

	// Overload method that sorts the passed constant list by their object names
	int sort(QList<RConstant *> *passedList);

	// Overload method that sorts the passed response list by their object names
	int sort(QList<RResponse *> *passedList);

	// Returns a list of possible values for the passed property that the user can choose from
	QStringList getPropertyStringList(QString propertyName);

	// Prints the elements of the passed matrix to the output pane, or to the passed outputFile if any, in the given threadID's buffer (or main output in the case of threadID = 0)
	void print(gsl_matrix *matrix, int threadID, QString outputFile = QString());

	// Overloaded method that print the elements of the passed vector to the output pane, or to the passed outputFile if any, in the given threadID's buffer (or main output in the case of threadID = 0)
	void print(gsl_vector *vector, int threadID, QString outputFile = QString());

	// Reads the elements of the passed matrix from the inputFile
	int readMatrixFromFile(gsl_matrix *matrix, QString inputFile);
	
	// Overloaded method that reads the elements of the passed matrix from the inputFile
	int readVectorFromFile(gsl_vector *vector, QString inputFile);

	// Static method that returns the sub-branches of a base-class to be shown in the object pane tree
	static QStringList getSubClasses(QString baseClassType);

	// Static method that returns the name of all dynamic meta-properties in any class in Rtx, such as "PerturbationFactor" in RFunction
	static QStringList getDynamicPropertyList();

	// Static method that returns the absolute path to the folder that contains the passed file
	static QString getAbsoluteFolderPath(QString filePath);

	// Static method that returns the file name of the passed file
	static QString getFileName(QString filePath);

	// Static method that coverts the passed string to a file path, by removing the double-quotes from the string
	static QString stringToFilePath(QString string);

	// Opens an input file
	int openFile(QString fileName, QObjectList *objectsCreated, QString *errorMessage, bool StInputFile = false);

	// Returns a pointer to a QByteArray object that stores all the output pane messages in console mode
	QByteArray *getOutputByteArray();

	// is called when a new analysis starts
	void newAnalysisStarted();

	// is called when an ongoing analysis is ended
	void anAnalysisEnded();
	
	// Returns true if there is at least one ongoing analysis
	bool thereIsOngoingAnalysis();

	/* ----------------------------- Hash and switch case ------------------------------------------- */
	// A hash for getting the Enum value associated with the class name string
	static QHash<QString, ClassName> classNamesHash;

	static QHash<QString, ClassName> classNamesHashCore;

	/* ---------------------------------------------------------------------------------------------------- */

	bool canBeRunParallelly();

signals:

	// Signal emitted when a there is a progress in opening an input file
	void fileOpenProgressed(int percent);

	// Signal emitted when new analysis start
	void newAnalysisStartedSignal();

	// Signal emitted when analysis end
	void allAnalysisEndedSignal();

private:
	// Handles errors that occur when opening an Rt input file
	void onFileOpenError(QObjectList *objectList, QString oldFilePath, QString errorMessage);

	// pointer to the last instantiated rObject
	QObject *theLastAddedObject;

	// Data member that stores the current file path of Rt
	QString theCurrentFilePath;

	// Flag that indicates if Rt is currently in the process of opening an input file
	bool isInputFileBeingOpened;

	// A QByteArray object that stores all the output pane messages in console mode
	QByteArray *theOutputByteArray;

	/* ----------------------------- Hash and switch case ------------------------------------------- */

	// Adds defined classes' name to the hash
	static void addClassNamesToHash();

	// a boolean field to know if the name of classes has been added to the hash
	static bool classNamesAddedToHash;
	
	/* ---------------------------------------------------------------------------------------------------- */

	int theNumberOfOngoingAnalysis;
};

#endif // RDomain_H
