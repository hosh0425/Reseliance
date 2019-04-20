#ifndef RSoilStructureInteractionModel_H
#define RSoilStructureInteractionModel_H

#include "RModel.h"


class RParameter;
class RResponse;

class RSoilStructureInteractionModel : public RModel
{
	Q_OBJECT

		// Define a QProperty for each input (parameter) data member. The type of the QProperty is "QObject *"
		Q_PROPERTY(QObject *FixedBasePeriod READ getFixedBasePeriod WRITE setFixedBasePeriod)

		// ******************************************** Runmode 5
		/*Q_PROPERTY(RSoftOrStiff SoftOrStiff READ getSoftOrStiff WRITE setSoftOrStiff)
		Q_ENUMS(RSoftOrStiff);
		Q_PROPERTY(QObject *CoefficientTeta1 READ getCoefficientTeta1 WRITE setCoefficientTeta1)
		Q_PROPERTY(QObject *CoefficientTeta2 READ getCoefficientTeta2 WRITE setCoefficientTeta2)
		Q_PROPERTY(QObject *CoefficientTeta3 READ getCoefficientTeta3 WRITE setCoefficientTeta3)
		Q_PROPERTY(QObject *CoefficientTeta4 READ getCoefficientTeta4 WRITE setCoefficientTeta4)*/
		//*********************************************

		Q_PROPERTY(RMDOFOrSDOF MDOFOrSDOF READ getMDOFOrSDOF WRITE setMDOFOrSDOF)
		Q_ENUMS(RMDOFOrSDOF);

		//Q_PROPERTY(RProgramComputesNumberOfStories ProgramComputesNumberOfStories READ getProgramComputesNumberOfStories WRITE setProgramComputesNumberOfStories)
		//Q_ENUMS(RProgramComputesNumberOfStories);

		Q_PROPERTY(int NumberOfStories READ getNumberOfStories WRITE setNumberOfStories)
		Q_PROPERTY(QObject *StructureDamping READ getStructureDamping WRITE setStructureDamping)
		Q_PROPERTY(QObject *SoilDamping READ getSoilDamping WRITE setSoilDamping)
		Q_PROPERTY(QObject *PoissonRatio  READ getPoissonRatio  WRITE setPoissonRatio )
		Q_PROPERTY(QObject *AspectRatio READ getAspectRatio WRITE setAspectRatio)
		Q_PROPERTY(QObject *EmbedmentRatio READ getEmbedmentRatio WRITE setEmbedmentRatio)
		Q_PROPERTY(QObject *StructureToSoilMassRatio READ getStructureToSoilMassRatio WRITE setStructureToSoilMassRatio)
		Q_PROPERTY(QObject *FoundationToStructureMassRatio READ getFoundationToStructureMassRatio WRITE setFoundationToStructureMassRatio)
		Q_PROPERTY(QObject *FoundationCentroidRatio READ getFoundationCentroidRatio WRITE setFoundationCentroidRatio)
		Q_PROPERTY(RSSISeverityMeasure SSISeverityMeasure READ getSSISeverityMeasure WRITE setSSISeverityMeasure)
		Q_ENUMS(RSSISeverityMeasure);

		//Q_PROPERTY(QObject *LastStoryStiffnessRatioIntercept READ getLastStoryStiffnessRatioIntercept WRITE setLastStoryStiffnessRatioIntercept)
		//Q_PROPERTY(QObject *LastStoryStiffnessRatioSlope READ getLastStoryStiffnessRatioSlope WRITE setLastStoryStiffnessRatioSlope)
		//Q_PROPERTY(QObject *StiffnessVariationExponent READ getStiffnessVariationExponent WRITE setStiffnessVariationExponent)
		Q_PROPERTY(QObject *DimensionlessFrequencyOrShearWaveVelocity READ getDimensionlessFrequencyOrShearWaveVelocity WRITE setDimensionlessFrequencyOrShearWaveVelocity)
		Q_PROPERTY(RSoilType SoilType READ getSoilType WRITE setSoilType)
		Q_ENUMS(RSoilType);
		// Indicates whether degredation factor is applied or not
		Q_PROPERTY(bool ApplyShearWaveVelocityDegradationFactor READ getApplyShearWaveVelocityDegradationFactor WRITE setApplyShearWaveVelocityDegradationFactor)
		Q_PROPERTY(double Radius READ getRadius WRITE setRadius)
		Q_PROPERTY(RShearDistributionMethod ShearDistributionMethod READ getShearDistributionMethod WRITE setShearDistributionMethod)
		Q_ENUMS(RShearDistributionMethod);

		Q_PROPERTY(RRunMode RunMode READ getRunMode WRITE setRunMode)
		Q_ENUMS(RRunMode);
		Q_PROPERTY(double SsASCE READ getSsASCE WRITE setSsASCE)
		Q_PROPERTY(double S1ASCE READ getS1ASCE WRITE setS1ASCE)

		Q_PROPERTY(QObject *StrengthReductionFactorOrTargetDuctility READ getStrengthReductionFactorOrTargetDuctility WRITE setStrengthReductionFactorOrTargetDuctility)
		//Q_PROPERTY(QObject *TargetDuctility READ getTargetDuctility WRITE setTargetDuctility)
		Q_PROPERTY(double OverStrength READ getOverStrength WRITE setOverStrength)
		Q_PROPERTY(double OverStrengthNEHRP READ getOverStrengthNEHRP WRITE setOverStrengthNEHRP)
		Q_PROPERTY(QString Records READ getRecords WRITE setRecords)
		Q_PROPERTY(QString RecordTimeSteps READ getRecordTimeSteps WRITE setRecordTimeSteps)
		Q_PROPERTY(QObject *RecordSelectionUncertainty READ getRecordSelectionUncertainty WRITE setRecordSelectionUncertainty)
		Q_PROPERTY(RRecordScalingMethod RecordScalingMethod READ getRecordScalingMethod WRITE setRecordScalingMethod)
		Q_ENUMS(RRecordScalingMethod);

		Q_PROPERTY(QObject *IntensityMeasure READ getIntensityMeasure WRITE setIntensityMeasure)
	
		Q_PROPERTY(RRecordScaling RecordScaling READ getRecordScaling WRITE setRecordScaling)
		Q_ENUMS(RRecordScaling);
	    
		Q_PROPERTY(double ScaleFactorLowerBound READ getScaleFactorLowerBound WRITE setScaleFactorLowerBound)
	    Q_PROPERTY(double ScaleFactorUpperBound READ getScaleFactorUpperBound WRITE setScaleFactorUpperBound)
 

public:
	// Define the constructor and destructor
	RSoilStructureInteractionModel(QObject *parent, QString name);
	~RSoilStructureInteractionModel();

	// Define the mandatory read/write methods for all QProperties
	QObject *getFixedBasePeriod() const;
	void setFixedBasePeriod(QObject *value);
	
	//******************************Runmode 5
	/*enum RSoftOrStiff{Soft, Stiff};
	RSoftOrStiff getSoftOrStiff() const;
	void setSoftOrStiff(RSoftOrStiff value);
	QObject *getCoefficientTeta1() const;
	void setCoefficientTeta1(QObject *value);
	QObject *getCoefficientTeta2() const;
	void setCoefficientTeta2(QObject *value);
	QObject *getCoefficientTeta3() const;
	void setCoefficientTeta3(QObject *value);
	QObject *getCoefficientTeta4() const;
	void setCoefficientTeta4(QObject *value);*/
	//******************************
	
	enum RMDOFOrSDOF{MDOF, SDOF};
	RMDOFOrSDOF getMDOFOrSDOF() const;
	void setMDOFOrSDOF(RMDOFOrSDOF value);

	//enum RProgramComputesNumberOfStories{Yes, No};
	//RProgramComputesNumberOfStories getProgramComputesNumberOfStories() const;
	//void setProgramComputesNumberOfStories(RProgramComputesNumberOfStories value);

	int getNumberOfStories() const;
	void setNumberOfStories(int value);
	QObject *getStructureDamping() const;
	void setStructureDamping(QObject *value);
	QObject *getSoilDamping() const;
	void setSoilDamping(QObject *value);
	QObject *getPoissonRatio () const;
	void setPoissonRatio (QObject *value);
	QObject *getAspectRatio() const;
	void setAspectRatio(QObject *value);
	QObject *getEmbedmentRatio() const;
	void setEmbedmentRatio(QObject *value);
	QObject *getStructureToSoilMassRatio() const;
	void setStructureToSoilMassRatio(QObject *value);
	QObject *getFoundationToStructureMassRatio() const;
	void setFoundationToStructureMassRatio(QObject *value);
	QObject *getFoundationCentroidRatio() const;
	void setFoundationCentroidRatio(QObject *value);
	//QObject *getLastStoryStiffnessRatioIntercept() const;
	//void setLastStoryStiffnessRatioIntercept(QObject *value);
	//QObject *getLastStoryStiffnessRatioSlope() const;
	//void setLastStoryStiffnessRatioSlope(QObject *value);
	//QObject *getStiffnessVariationExponent() const;
	//void setStiffnessVariationExponent(QObject *value);

	enum RSSISeverityMeasure{DimensionlessFrequency, ShearWaveVelocity};
	RSSISeverityMeasure getSSISeverityMeasure() const;
	void setSSISeverityMeasure(RSSISeverityMeasure value);
	enum RSoilType{SoilTypeC, SoilTypeD, SoilTypeE};
	RSoilType getSoilType() const;
	void setSoilType(RSoilType value);
	bool getApplyShearWaveVelocityDegradationFactor();
	void setApplyShearWaveVelocityDegradationFactor(bool value);
	QObject *getDimensionlessFrequencyOrShearWaveVelocity() const;
	void setDimensionlessFrequencyOrShearWaveVelocity(QObject *value);
	
	enum RShearDistributionMethod{ASCE7, NBCC, MexicanCode, Eurocode};
	RShearDistributionMethod getShearDistributionMethod() const;
	void setShearDistributionMethod(RShearDistributionMethod value);

	enum RRunMode{GivenRSameR, GivenDucSameR, GivenDucComputeModificationFactor, GivenDucSameRorFy, /*GivenDucSameAlpha,*/ GivenRFixSameFy, GivenDucFixSameFy, GivenDucSSISameFy, GivenRReducedFyATC, GivenRReducedFyASCE_05, GivenRReducedFyASCE_10, GivenRReducedFyNEHRP};
	RRunMode getRunMode() const;
	void setRunMode(RRunMode value);
	QObject *getStrengthReductionFactorOrTargetDuctility() const;
	void setStrengthReductionFactorOrTargetDuctility(QObject *value);
	//QObject *getTargetDuctility() const;
	//void setTargetDuctility(QObject *value);

	QString getRecords() const;
	void setRecords(QString value);
	QString getRecordTimeSteps();
	void setRecordTimeSteps(QString value);
	
	QObject *getRecordSelectionUncertainty() const;
	void setRecordSelectionUncertainty(QObject *value);

	enum RRecordScalingMethod{PGA, Sa, Spectrum};
	RRecordScalingMethod getRecordScalingMethod() const;
	void setRecordScalingMethod(RRecordScalingMethod value);
	QObject *getIntensityMeasure() const;
	void setIntensityMeasure(QObject *value);

	enum RRecordScaling{All, Adaptive, Unscaled};
	RRecordScaling getRecordScaling() const;
	void setRecordScaling(RRecordScaling value);

	double getRadius() const;
    void setRadius(double value);

	double getSsASCE() const;
    void setSsASCE(double value);

	double getS1ASCE() const;
    void setS1ASCE(double value);

	double getOverStrength() const;
    void setOverStrength(double value);

	double getOverStrengthNEHRP() const;
    void setOverStrengthNEHRP(double value);

	double getScaleFactorLowerBound() const;
	void setScaleFactorLowerBound(double value);
	
	double getScaleFactorUpperBound() const;
	void setScaleFactorUpperBound(double value);


	// Define the method where the actual model is coded
	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID);

	// returns true as this model is parallelized
	bool canBeRunParallelly();

private:
	//Base shear distribution method
	int lateralforcedistribution(double Tn, int Num, gsl_vector *dist, int threadID);
	// Computes Kfix, Cfix, Mfix for MDOF
	int stiffnessAndDampingMatrixMDOF(double Tn, double vs, double wn, double h_r0, double mbar, double xi_s, int Num, gsl_vector *kstr, gsl_matrix *Kfix, gsl_matrix *Mfix, gsl_matrix *Cfix, double *h_story, double *htotal, double Mtotal, double *r0, double *roh, double *landam, double *landah, int threadID);
	
	//Running Runmode GivenDuc
	int RunmodeGivenDuc(gsl_matrix *K,gsl_matrix *M, gsl_matrix *C, gsl_matrix *Kfix, gsl_matrix *Mfix, gsl_matrix *Cfix, gsl_matrix *L, gsl_matrix *Lfix, gsl_vector *kstr, double VeFix, double VeSSI, QVector<double> FIMh, QVector<double> FIMr, double n, double dt, double e, double alpha, double target_ductility, double Tn, double vs, double h_story, double dof, int Num, gsl_vector *fy,  gsl_vector *ductility_MDOF, double *ductility_max, int *ductility_maxNumber, double *R, gsl_vector *Dm_MDOF, gsl_vector *Dm_tot_MDOF, gsl_vector *Am_MDOF, gsl_vector *Am_tot_MDOF, int threadID);

	// Computes u_j, du_j, d2u_i, d2u_j, fs_j, 
	int newmarkBetaSDOF(gsl_matrix *K, double dt, gsl_matrix *C, gsl_matrix *M, gsl_vector *Force_i, gsl_vector *Force_j, double fs_i, gsl_vector *u_i, gsl_vector *du_i, int dof, gsl_vector *u_j, gsl_vector *du_j,gsl_vector *d2u_i,gsl_vector *d2u_j, double *fs_j , int threadID);
	int newmarkBetaMDOFNonlinear(gsl_vector *kstr, gsl_matrix *K, double dt, gsl_matrix *C, gsl_matrix *M, gsl_vector *fyc, gsl_vector *fyt, gsl_vector *Force_i, gsl_vector *Force_j, gsl_vector *fs_i, gsl_vector *tempfsi, gsl_vector *u_i, gsl_vector *du_i, int Num, int dof, gsl_vector *u_j, gsl_vector *du_j,gsl_vector *d2u_i,gsl_vector *d2u_j, gsl_vector *fs_j, gsl_vector *tempfsj, int threadID);
	int newmarkBetaMDOFLinear(gsl_matrix *K, double dt, gsl_matrix *C, gsl_matrix *M, gsl_vector *Force_i, gsl_vector *Force_j, gsl_vector *fs_i, gsl_vector *u_i, gsl_vector *du_i, int Num, int dof, gsl_vector *u_j, gsl_vector *du_j,gsl_vector *d2u_i,gsl_vector *d2u_j, gsl_vector *fs_j , int threadID);

	// compute De, De_tot, Am, Am_tot,
	int linearAnalysisSDOF(double he, gsl_matrix *K, gsl_matrix *C, gsl_matrix *M, gsl_matrix *L, QVector<double> FIMh, QVector<double> FIMr, double n, double dt, double dof , double *De, double *De_tot, double *Ae, double *Ae_tot, int threadID);
	int linearAnalysisMDOF(double h_story,double e,gsl_vector *kstr, gsl_matrix *K, gsl_matrix *C, gsl_matrix *M, gsl_matrix *L, QVector<double> FIMh, QVector<double> FIMr, double n, double dt, double dof , int Num, gsl_vector *De, gsl_vector *De_tot, gsl_vector *Am, gsl_vector *Am_tot, double *Ve, int threadID);

	// Computes ductility, Dm, Dm_total
	int nonlinearAnalysisSDOF(double he, double fy, double alpha, double ks, gsl_matrix *K, gsl_matrix *C, gsl_matrix *M, gsl_matrix *L, QVector<double> FIMh, QVector<double> FIMr, double n, double dt, double dof , double *ductility, double *Dm_tot,  double *Am,  double *Am_tot, int threadID);
	int nonlinearAnalysisMDOF(double h_story,double e,gsl_vector *fy, double alpha, gsl_vector *kstr, gsl_matrix *Kfix, gsl_matrix *K, gsl_matrix *C, gsl_matrix *M, gsl_matrix *L, QVector<double> FIMh, QVector<double> FIMr, double n, double dt, double dof, int Num, gsl_vector *ductility, double *ductility_max, int *ductility_maxNumber, gsl_vector *Dm, gsl_vector *Dm_tot,  gsl_vector *Am, gsl_vector *Am_tot, int threadID);

	// Declare input data members
	RPointer<RParameter> theFixedBasePeriod;
	RMDOFOrSDOF theMDOFOrSDOF;
	//*********************************************************** Runmode 5
	/*RSoftOrStiff theSoftOrStiff;
	RPointer<RParameter> theCoefficientTeta1;
	RPointer<RParameter> theCoefficientTeta2;
	RPointer<RParameter> theCoefficientTeta3;
	RPointer<RParameter> theCoefficientTeta4;*/
	//***********************************************************
	//RProgramComputesNumberOfStories theProgramComputesNumberOfStories;
	int theNumberOfStories;
	RPointer<RParameter> theStructureDamping;
	RPointer<RParameter> theSoilDamping;
	RPointer<RParameter> thePoissonRatio ;
	RPointer<RParameter> theAspectRatio;
	RPointer<RParameter> theEmbedmentRatio;
	RPointer<RParameter> theStructureToSoilMassRatio;
	RPointer<RParameter> theFoundationToStructureMassRatio;
	RPointer<RParameter> theFoundationCentroidRatio;
	//RPointer<RParameter> theLastStoryStiffnessRatioIntercept;
	//RPointer<RParameter> theLastStoryStiffnessRatioSlope;
	//RPointer<RParameter> theStiffnessVariationExponent;
	RSSISeverityMeasure theSSISeverityMeasure;
	RSoilType theSoilType;
	bool theApplyShearWaveVelocityDegradationFactor;
	RPointer<RParameter> theDimensionlessFrequencyOrShearWaveVelocity;
	RShearDistributionMethod theShearDistributionMethod;
	RRunMode theRunMode;
	RPointer<RParameter> theStrengthReductionFactorOrTargetDuctility;
	//RPointer<RParameter> theTargetDuctility;
	RRecordScalingMethod theRecordScalingMethod;
	RPointer<RParameter> theIntensityMeasure;
	QStringList theRecords;
	QVector<double> theRecordTimeStepsVector;
	RPointer<RParameter> theRecordSelectionUncertainty;
	RRecordScaling theRecordScaling;
	double theRadius;
	double theSsASCE;
	double theS1ASCE;
	double theOverStrength;
	double theOverStrengthNEHRP;
	double theScaleFactorLowerBound;
	double theScaleFactorUpperBound;
	
	// Declare responses
	RResponse *theT_SSIResponse;
	RResponse *theVhatRatioResponse;
	RResponse *theElasticDisplacementResponse;
	RResponse *theElasticAccelarationResponse;
	RResponse *theTotalElasticDisplacementResponse;
	RResponse *theTotalElasticAccelarationResponse;
	RResponse *theInelasticDisplacementResponse;
	RResponse *theInelasticAccelarationResponse;
	RResponse *theTotalInelasticDisplacementResponse;
	RResponse *theTotalInelasticAccelarationResponse;
	RResponse *theDuctilityResponse;
	RResponse *theFixedStrengthReductionFactorResponse;
	RResponse *theSSIStrengthReductionFactorResponse;
	RResponse *theElasticDisplacement_IntensityResponse;
	RResponse *theElasticAccelaration_IntensityResponse;
	RResponse *theTotalElasticDisplacement_IntensityResponse;
	RResponse *theTotalElasticAccelaration_IntensityResponse;
	RResponse *theInelasticDisplacement_IntensityResponse;
	RResponse *theInelasticAccelaration_IntensityResponse;
	RResponse *theTotalInelasticDisplacement_IntensityResponse;
	RResponse *theTotalInelasticAccelaration_IntensityResponse;
	RResponse *theSystemDuctilityResponse;
	RResponse *theFixedDuctilityResponse;
	RResponse *theFixedTotalAccelarationResponse;
	RResponse *theFixedDisplacementResponse;
	RResponse *theFixedDesignDuctilityResponse;
	RResponse *theFixedDesignTotalAccelarationResponse;
	RResponse *theFixedDesignDisplacementResponse;
	RResponse *theSpringYieldForce_mIntensityResponse;
	RResponse *theElasticSpringForce_mIntensityResponse;
	
	// MDOF responses
	QList<RResponse *> theductility_MDOFResponseList;
	QList<RResponse *> theductility_MDOF2ResponseList;
	QList<RResponse *> theFixedDuctility_MDOFResponseList;
	QList<RResponse *> theDessi_MDOFResponseList;
	QList<RResponse *> theAessi_MDOFResponseList;
	QList<RResponse *> theDe_tot_MDOFResponseList;
	QList<RResponse *> theAe_tot_MDOFResponseList;
	QList<RResponse *> theDm_MDOFResponseList;
	QList<RResponse *> theAm_MDOFResponseList;
	QList<RResponse *> theDm_tot_MDOFResponseList;
	QList<RResponse *> theAm_tot_MDOFResponseList;
	QList<RResponse *> theAm_tot_fix_MDOFResponseList;
	QList<RResponse *> theDm_fix_MDOFResponseList;
	RResponse *theMaximumDuctilityMDOFfyfixSDOFResponse;
	RResponse *theMaximumDuctilityStoryMDOFfyfixSDOFResponse;
	RResponse *theDuctilitySDOFfyfixSDOFResponse;
	RResponse *theSSIStrengthReductionFactorSDOFfyfixSDOFResponse;
	RResponse *theMaximumDuctilityResponse;
	RResponse *theMaximumDuctilityStoryResponse;
	RResponse *theFixedMaximumDuctilityResponse;
	RResponse *theFixedMaximumDuctilityStoryResponse;
	RResponse *theFixedStrengthReductionFactorMDOFResponse;
	RResponse *theSSIStrengthReductionFactorMDOFResponse;
	RResponse *theFixedStrengthReductionFactorSDOFResponse;
	RResponse *theSSIStrengthReductionFactorSDOFResponse;
	RResponse *theSSIModificationFactorAlpha_RResponse;
	RResponse *theFixedModificationFactorAlpha_RResponse;
	RResponse *theSSIModificationFactorAlpha_VResponse;
	RResponse *theFixedModificationFactorAlpha_VResponse;
	RResponse *theFixedElasticBaseShearRatioResponse;
	RResponse *theSSIElasticBaseShearRatioResponse;

/*	
	RResponse *theElasticDisplacement_IntensityResponse;
	RResponse *theElasticAccelaration_IntensityResponse;
	RResponse *theTotalElasticDisplacement_IntensityResponse;
	RResponse *theTotalElasticAccelaration_IntensityResponse;
	RResponse *theInelasticDisplacement_IntensityResponse;
	RResponse *theInelasticAccelaration_IntensityResponse;
	RResponse *theTotalInelasticDisplacement_IntensityResponse;
	RResponse *theTotalInelasticAccelaration_IntensityResponse;
	RResponse *theFixedDesignDuctilityResponse;
	RResponse *theFixedDesignTotalAccelarationResponse;
	RResponse *theFixedDesignDisplacementResponse;
	RResponse *theSpringYieldForce_mIntensityResponse;
	RResponse *theElasticSpringForce_mIntensityResponse;
*/
	// Declare internal variables

	QVector<double> IntensityVector;
	QList<QVector<double> > recordAccVectorList;
	QVector<double> dtVector;

};

#endif // RSoilStructureInteractionModel_H
