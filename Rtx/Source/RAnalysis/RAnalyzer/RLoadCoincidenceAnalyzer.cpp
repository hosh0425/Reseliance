// Last modified by Mojtaba on March 19, 2014

#include "RLoadCoincidenceAnalyzer.h"

#include <QMessageBox>
#include <QMap>
#include <QFile>
#include <QApplication>

#include "RMainWindow.h"
#include "RDomain.h"
#include "RFunction.h"
#include "RModel.h"
#include "RRandomVariable.h"
#include "RTime.h"
#include "RTransformer.h"
#include "RAccumulator.h"
#include "RRandomNumberGenerator.h"
#include "RFailureProbabilityAccumulator.h"
#include "RIndependentNormalRandomNumberGenerator.h"
#include "RNonlinSingleConstrSolver.h"
#include "RStepperNonlinSingleConstrSolver.h"
#include "RStepSizeSearcher.h"
#include "RStepDirectionSearcher.h"
#include "RConvergenceChecker.h"
#include "ROccurrenceModel.h"
#include "RResponse.h"
#include "RPlotWidget.h"
#include "RPoissonPulseProcessModel.h"
#include "RContinuousDecisionVariable.h"
#include "RContinuousRandomVariable.h"
#include "RFORMAnalyzer.h"
#include "RSamplingAnalyzer.h"

//#include <qwt_plot_curve.h>

#include <gsl/gsl_vector.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_cdf.h>
#include <gsl/gsl_combination.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_sf_gamma.h>
#include <gsl/gsl_blas.h>


RLoadCoincidenceAnalyzer::RLoadCoincidenceAnalyzer(QObject *parent, QString name)
	: RAnalyzer(parent, name)
{
	//$$ theLimitStateFunction = 0;
    theAnalysisType = RLoadCoincidenceAnalyzer::FORM;
	//$$ theFORMAnalyzer = 0;
	//$$ theSamplingAnalyzer = 0;
	theMaxNumberOfCoincidentHazards = 1;
	theTimespan = 18262;
	theComputeRandomVariableSensitivities = true;
	theComputeDecisionVariableSensitivities = true;
	theComputeModelResponseStandardDeviationSensitivities = true;
	thePrintSensitivities = false;
	theCorrectProbabilityWithFirstPrincipalCurvature = false;
}

RLoadCoincidenceAnalyzer::~RLoadCoincidenceAnalyzer()
{

}

QObject * RLoadCoincidenceAnalyzer::getLimitStateFunction() const	
{	
	return theLimitStateFunction.data();
}	
	
void RLoadCoincidenceAnalyzer::setLimitStateFunction(QObject *value)	
{	
	theLimitStateFunction = qobject_cast<RFunction *>(value);
}	
	
RLoadCoincidenceAnalyzer::RAnalysisType RLoadCoincidenceAnalyzer::getAnalysisType()
{
	return theAnalysisType;
}

void RLoadCoincidenceAnalyzer::setAnalysisType(RLoadCoincidenceAnalyzer::RAnalysisType value)
{
	theAnalysisType = value;
}

QObject * RLoadCoincidenceAnalyzer::getFORMAnalyzer() const	
{	
	return theFORMAnalyzer.data();
}	
	
void RLoadCoincidenceAnalyzer::setFORMAnalyzer(QObject *value)	
{	
	theFORMAnalyzer = qobject_cast<RFORMAnalyzer *>(value);
}	
			
QObject * RLoadCoincidenceAnalyzer::getSamplingAnalyzer() const	
{	
	return theSamplingAnalyzer.data();
}	
	
void RLoadCoincidenceAnalyzer::setSamplingAnalyzer(QObject *value)	
{	
	theSamplingAnalyzer = qobject_cast<RSamplingAnalyzer *>(value);
}	

QString RLoadCoincidenceAnalyzer::getOccurrenceModelList()	
{	
	return modelListToString(theOccurrenceModelList);
}	
	
void RLoadCoincidenceAnalyzer::setOccurrenceModelList(QString value)	
{	
	theOccurrenceModelList = stringToModelList(value);
}	

int RLoadCoincidenceAnalyzer::getMaxNumberOfCoincidentHazards() const	
{	
	return theMaxNumberOfCoincidentHazards;
}	
	
void RLoadCoincidenceAnalyzer::setMaxNumberOfCoincidentHazards(int value)	
{	
	theMaxNumberOfCoincidentHazards = value;
}	

double RLoadCoincidenceAnalyzer::getTimespan() const	
{	
	return theTimespan;
}	
	
void RLoadCoincidenceAnalyzer::setTimespan(double value)	
{	
	theTimespan = value;
}	
	
bool RLoadCoincidenceAnalyzer::getComputeRandomVariableSensitivities() const	
{	
	return theComputeRandomVariableSensitivities;
}	
	
void RLoadCoincidenceAnalyzer::setComputeRandomVariableSensitivities(bool value)	
{	
	theComputeRandomVariableSensitivities = value;
}	
	
bool RLoadCoincidenceAnalyzer::getComputeDecisionVariableSensitivities() const	
{	
	return theComputeDecisionVariableSensitivities;
}	
	
void RLoadCoincidenceAnalyzer::setComputeDecisionVariableSensitivities(bool value)	
{	
	theComputeDecisionVariableSensitivities = value;
}	
	
bool RLoadCoincidenceAnalyzer::getComputeModelResponseStandardDeviationSensitivities() const	
{	
	return theComputeModelResponseStandardDeviationSensitivities;
}	
	
void RLoadCoincidenceAnalyzer::setComputeModelResponseStandardDeviationSensitivities(bool value)	
{	
	theComputeModelResponseStandardDeviationSensitivities = value;
}	
	
bool RLoadCoincidenceAnalyzer::getPrintSensitivities() const	
{	
	return thePrintSensitivities;
}	
	
void RLoadCoincidenceAnalyzer::setPrintSensitivities(bool value)	
{	
	thePrintSensitivities = value;
}	
	
bool RLoadCoincidenceAnalyzer::getCorrectProbabilityWithFirstPrincipalCurvature() const	
{	
	return theCorrectProbabilityWithFirstPrincipalCurvature;
}	
	
void RLoadCoincidenceAnalyzer::setCorrectProbabilityWithFirstPrincipalCurvature(bool value)	
{	
	theCorrectProbabilityWithFirstPrincipalCurvature = value;
}	

QString RLoadCoincidenceAnalyzer::getStartPointFile()	
{	
	return theStartPointFile;
}	
	
void RLoadCoincidenceAnalyzer::setStartPointFile(QString value)	
{	
	theStartPointFile = value;
}	

QString RLoadCoincidenceAnalyzer::getDesignPointFile()	
{	
	return theDesignPointFile;
}	
	
void RLoadCoincidenceAnalyzer::setDesignPointFile(QString value)	
{	
	theDesignPointFile = value;
}	

int RLoadCoincidenceAnalyzer::analyze()
{
	// Inform the user that the analysis has started
	rDebug() << "##### STARTING LOAD COINCIDENCE ANALYSIS ...";
	rDebug() << "";

	QDateTime startTime = QDateTime::currentDateTime();

	if (qobject_cast<QApplication*>(QCoreApplication::instance()) != 0) {
		// Connecting the signal that is trigerred when a sub-window is added to the main window: Using this, the title of the plots generated by anaylsis tools can be modified
		bool ok = connect(theMainWindow, SIGNAL(subWindowAdded(QWidget *)), this, SLOT(onSubWindowAdded(QWidget *)));
	}


	if (!theLimitStateFunction) {
		rCritical() << "Error in" << objectName() << ": Please choose a function and try again";
		return -1;
	}

	RNonlinSingleConstrSolver *solver = 0;
	RRandomNumberGenerator *randomNumberGenerator = 0;
	RTransformer *transformer = 0;
	RAccumulator *accumulator = 0;

	int result = 0;

	if (theAnalysisType == RLoadCoincidenceAnalyzer::FORM || theAnalysisType == RLoadCoincidenceAnalyzer::ImportanceSampling) {
		if (!theFORMAnalyzer) {
			rCritical() << "Error in" << objectName() << ": Could find the FORM Analyzer";
			return -1;
		}
		theFORMAnalyzer.data()->setLimitStateFunction(theLimitStateFunction.data());
		solver = qobject_cast<RNonlinSingleConstrSolver *>(theFORMAnalyzer.data()->getNonlinearSingleConstraintSolver());

		result = solver->checkProperties();
		if (result < 0) {
			rCritical() << "Error: Could find one or more necessary tools in the object" << solver->objectName();
			return -1;
		}
	}

	if (theAnalysisType == RLoadCoincidenceAnalyzer::Sampling || theAnalysisType == RLoadCoincidenceAnalyzer::ImportanceSampling) {
		if (!theSamplingAnalyzer) {
			rCritical() << "Error in" << objectName() << ": Could find the Sampling Analyzer";
			return -1;
		}
		theSamplingAnalyzer.data()->setFunctionList(theLimitStateFunction.data()->objectName());
		
		randomNumberGenerator = qobject_cast<RRandomNumberGenerator *>(theSamplingAnalyzer.data()->getRandomNumberGenerator());
		transformer = qobject_cast<RTransformer *>(theSamplingAnalyzer.data()->getTransformer());
		accumulator = qobject_cast<RAccumulator *>(theSamplingAnalyzer.data()->getAccumulator());

		result = randomNumberGenerator->checkProperties();
		if (result < 0) {
			rCritical() << "Error: Could find one or more necessary tools in the object" << randomNumberGenerator->objectName();
			return -1;
		}

		result = transformer->checkProperties();
		if (result < 0) {
			rCritical() << "Error: Could find one or more necessary tools in the object" << transformer->objectName();
			return -1;
		}

		result = accumulator->checkProperties();
		if (result < 0) {
			rCritical() << "Error: Could find one or more necessary tools in the object" << accumulator->objectName();
			return -1;
		}
		// Temporary until the day that load coincidence can also work with a histogram accumulator
		RFailureProbabilityAccumulator *failureAccumulator = qobject_cast<RFailureProbabilityAccumulator *>(accumulator);
		if (!failureAccumulator) {
			rCritical() << "Error in" << objectName() << ": Currently, the load coincidence analysis only supports the Failure Probability Accumulator";
			return -1;
		}

		theComputeRandomVariableSensitivities = false;
		theComputeDecisionVariableSensitivities = false;
		theComputeModelResponseStandardDeviationSensitivities = false;
		thePrintSensitivities = false;
		theCorrectProbabilityWithFirstPrincipalCurvature = false;
	}


	// If the method of analysis is seleceted as importance sampling, because a FORM is carried out before sampling, the start point of the random generator should be set as CurrentValue
	if (theAnalysisType == RLoadCoincidenceAnalyzer::ImportanceSampling) {
		randomNumberGenerator->setStartPoint(RObject::CurrentValue);
	}


	// Create a list of occurrence models
	QList<ROccurrenceModel *> hazardModelList;
	for (int i = 0; i < theOccurrenceModelList.count(); i++) {
		ROccurrenceModel *model = qobject_cast<ROccurrenceModel *>(theOccurrenceModelList[i].data());
		if (model) {
			// ATTENTION: This analysis for now only support Poisson Pulse Processes as occurrence model.
			RPoissonPulseProcessModel *poissonModel = qobject_cast<RPoissonPulseProcessModel *>(model);
			if (!poissonModel) {
				rCritical() << "Error in" << objectName() << ": Currently, The load coincidence analysis only supports Poisson Pulse Processes as occurrence models";
				return -1;
			}

			hazardModelList << model;
		} else {
			// Issue an error if the model is not an occurrence model
			rCritical() << "Error in" << objectName() << ": The model %1 is not an occurrence model. Please correct and try again";
			return -1;
		}
	}

	// Issue an error if the nothing is entered in the hazardLineEdit
	if (hazardModelList.isEmpty()) {
		rCritical() << "Error in" << objectName() << ": Enter at least one occurrence model and try again";
		return -1;
	}

	// Issue an error if T <=0
	if (theTimespan <= 0) {
		rCritical() << "Error in" << objectName() << ": Enter a timespan greater than zero and try again";;
		return -1;
	}

	double T = theTimespan;


	// Check theMaxNumberOfCoincidentHazards integer
	if (theMaxNumberOfCoincidentHazards > hazardModelList.count()) {
		theMaxNumberOfCoincidentHazards = hazardModelList.count();
		rCritical() << "Warning: The maximum number of coincident hazards exceeds the number of hazards. Rt will consider all coincidences.";

	}
	if (theMaxNumberOfCoincidentHazards < 1) {
		theMaxNumberOfCoincidentHazards = 1;
		rCritical() << "Warning: The maximum number of coincident hazards is less than 1. Rt will use 1 instead.";

	}


	if (theComputeModelResponseStandardDeviationSensitivities) {
		theComputeRandomVariableSensitivities = true;
	}

	if (qobject_cast<QApplication*>(QCoreApplication::instance()) != 0) {
		theMainWindow->raiseDock("Output");
	}
	






	QList<RFunction *> functionList;
	functionList << theLimitStateFunction.data();
	if (accumulator) {
		accumulator->reset();
		// Set the functionList in the accumulator
		accumulator->setFunctionList(functionList);
	}

	if (randomNumberGenerator) {
		randomNumberGenerator->reset();
	}

	
	// Parse the function
	result = theLimitStateFunction.data()->populateLists();
	if (result < 0) {
		rCritical() << "Error: Could not parse the function" << theLimitStateFunction.data()->objectName() << ".";
		return -1;
	}
	
	
	QList<RRandomVariable *> theRandomVariableList = *(theLimitStateFunction.data()->getRandomVariableList());
	QList<RDecisionVariable *> theDecisionVariableList = theLimitStateFunction.data()->getDecisionVariableList();
	QList<RConstant *> theConstantList = theLimitStateFunction.data()->getConstantList();
	QList<RResponse *> theResponseList = theLimitStateFunction.data()->getResponseList();
	QList<RParameter *> theExplicitParameterList = theLimitStateFunction.data()->getExplicitParameterList();
	QList<RModel *> theModelList = theLimitStateFunction.data()->getModelList();
	
	
	// Soring the theRandomVariableList according to alphabetical order
	theDomain->sort(&theRandomVariableList);
	int count = theRandomVariableList.count();


	// Create a list of the "unused" hazard occurrence models in the domain (the ones that are in the domain, but are not given in the dialog box of the orchestrating algorithm)
	QList<ROccurrenceModel *> unusedOccurrenceModelList = theDomain->findChildren<ROccurrenceModel *>();
	for (int i = 0; i < hazardModelList.count(); i++) {
		unusedOccurrenceModelList.removeOne(hazardModelList[i]);
	}
	// Building a list of the models asssociated with unusedOccurrenceModelList and taking away those which do not affect the functions (This list will be used to forcibly turn off the hazards that affect the function, but are not given as a hazard to the dialog box of the orchestrating algorithm)	
	// Also building a response list associated with unusedAssociatedModelList and unusedMagnitudeModelList
	// Also, setting the value of output responses of unusedMagnitudeModelList to Zero
	// Also, setting isAnalyzedFlag of he output responses of both unusedMagnitudeModelList and unusedAssociatedModelList to true so that they are not run
	QList<RModel *> unusedAssociatedModelList;
	QList<RModel *> unusedMagnitudeModelList;
	QList<RResponse *> unusedResponseList;
	for (int i = 0; i < unusedOccurrenceModelList.count(); i++) {
		RModel *tempModel = qobject_cast<RModel *>(unusedOccurrenceModelList[i]->getMagnitudeModel());
		if ((!unusedMagnitudeModelList.contains(tempModel)) && (theModelList.contains(tempModel))) {
			unusedMagnitudeModelList << tempModel;
			QList<RResponse *> tempModelResponseList = tempModel->findChildren<RResponse *>();
			unusedResponseList << tempModelResponseList;
			for (int j = 0; j < tempModelResponseList.count(); j++) {
				tempModelResponseList[j]->setCurrentValue(0);
			}
			tempModel->setResponseAnalyzed(true,0);
		}
		QList<RModel *> tempModelList = unusedOccurrenceModelList[i]->getAssociatedModels();
		for (int j = 0; j < tempModelList.count(); j++) {
			if (!unusedAssociatedModelList.contains(tempModelList[j]) && theModelList.contains(tempModelList[j])) {
				unusedAssociatedModelList << tempModelList[j];

				QList<RResponse *> tempModelResponseList = tempModelList[j]->findChildren<RResponse *>();
				unusedResponseList << tempModelResponseList;
				tempModelList[j]->setResponseAnalyzed(true,0);
			}
		}
	}

	// Eliminating the unusedResponseList from theResponseList
	for (int i = 0; i < unusedResponseList.count(); i++) {
		bool ok = theResponseList.removeOne(unusedResponseList[i]);
	}

	// Eliminating the unusedResponseList from theExplicitParameterList
	for (int i = 0; i < unusedResponseList.count(); i++) {
		bool ok = theExplicitParameterList.removeOne(unusedResponseList[i]);
	}

	// Eliminating the unusedAssociatedModelList and unusedMagnitudeModelList from theModelList
	for (int i = 0; i < unusedAssociatedModelList.count(); i++) {
		bool ok = theModelList.removeOne(unusedAssociatedModelList[i]);
	}
	for (int i = 0; i < unusedMagnitudeModelList.count(); i++) {
		bool ok = theModelList.removeOne(unusedMagnitudeModelList[i]);
	}
	

	// Calculating all possible combinations at each level
	int hazardsCount = hazardModelList.count();
	int numCombinations = 0;
	QList<QList<QVector<int> > > combinationVectorLevelsList;
	gsl_combination *combination = 0;
	for (int i = 1; i <= theMaxNumberOfCoincidentHazards; i++) {
		combination = gsl_combination_calloc(hazardsCount, i);
		QList<QVector<int> > combinationVectorList;
		do {
			QVector<int> combinationVector;
			for (int j = 0; j < i; j++) {
				combinationVector << gsl_combination_get(combination, j);
			}
			combinationVectorList << combinationVector;
			numCombinations++;
		} while (gsl_combination_next(combination) == GSL_SUCCESS);
		combinationVectorLevelsList << combinationVectorList;
		gsl_combination_free(combination);
		combination = 0;
	}


	// Creating a vector of lambda and mud from all hazards
	QVector<double> lambdaVector;
	QVector<double> mudVector;
	for (int i = 0; i < hazardsCount; i++) {
		RPoissonPulseProcessModel *poissonModel = qobject_cast<RPoissonPulseProcessModel *>(hazardModelList[i]);
		lambdaVector << poissonModel->getNonzeroOccurrenceRateValue();
		mudVector << poissonModel->getMeanDuration();
	}

	
	// Initializing the decisionSensitivityVector
	int numDVs = theDecisionVariableList.count();
	QVector<double> decisionSensitivityVector;
	if (theComputeDecisionVariableSensitivities) {
		for (int i = 0; i < numDVs; i++) {
			decisionSensitivityVector << 0.0;
		}
	}


	int numRVs = theRandomVariableList.count();
	gsl_vector *dBetaJdMeanVector = 0;
	gsl_vector *dBetaJdStdvVector = 0;
	gsl_vector *dKappaJdMeanVector = 0;
	gsl_vector *dKappaJdStdvVector = 0;
	gsl_vector *alphaJVector = 0;
	gsl_vector *gammaJVector = 0;
	gsl_vector *dBetadMeanVector = 0;
	gsl_vector *dBetadStdvVector = 0;
	gsl_vector *dKappadMeanVector = 0;
	gsl_vector *dKappadStdvVector = 0;
	gsl_vector *alphaVector = 0;
	gsl_vector *gammaVector = 0;
	if (theComputeRandomVariableSensitivities) {
		dBetaJdMeanVector = gsl_vector_calloc(numRVs);
		dBetaJdStdvVector = gsl_vector_calloc(numRVs);
		alphaJVector = gsl_vector_calloc(numRVs);
		gammaJVector = gsl_vector_calloc(numRVs);
		dBetadMeanVector = gsl_vector_calloc(numRVs);
		dBetadStdvVector = gsl_vector_calloc(numRVs);
		alphaVector = gsl_vector_calloc(numRVs);
		gammaVector = gsl_vector_calloc(numRVs);

		if (theCorrectProbabilityWithFirstPrincipalCurvature) {
			dKappaJdMeanVector = gsl_vector_calloc(numRVs);
			dKappaJdStdvVector = gsl_vector_calloc(numRVs);
		}
	}


	// Getting a backup from the currentValue of all RVs if the start point of the search in FORM is the current value
	QVector<double> currentValueBackup;
	if (theAnalysisType == RLoadCoincidenceAnalyzer::FORM || theAnalysisType == RLoadCoincidenceAnalyzer::ImportanceSampling) {
		if (solver->getStartPoint() == RObject::CurrentValue) {
			for (int i = 0; i < numRVs; i++) {
				currentValueBackup << theRandomVariableList[i]->getCurrentValue();
			}
		}
	}

	

	// Instantiate a matrix to store the design point of the analyses for all hazards and hazard combinations
	gsl_matrix *designPointMatrix = 0;
	if (!theDesignPointFile.isEmpty()) {
		designPointMatrix = gsl_matrix_calloc(numRVs, numCombinations);
	}

	// Instantiate a matrix to read the start point of the analyses for all hazards and hazard combinations from the file specified by the user
	gsl_matrix *startPointMatrix = 0;
	if (!theStartPointFile.isEmpty()) {
		startPointMatrix = gsl_matrix_calloc(numRVs, numCombinations);
		result = theDomain->readMatrixFromFile(startPointMatrix, theStartPointFile);
		if (result < 0) {
			rCritical() << "Error: Could not read the values int the file" << theStartPointFile;
			gsl_matrix_free(startPointMatrix);
			startPointMatrix = 0;
		} else {
			solver->setStartPoint(RObject::CurrentValue);
		}
	}	


	// Vectors for computing model response stdv sensitivity
	gsl_vector *dBetadTotalStdvVector = 0;
	gsl_vector *dBetadTotalEpistemicStdvVector = 0;
	gsl_vector *dBetadModelStdvVector = 0;
	gsl_vector *dBetadModelEpistemicStdvVector = 0;
	int numResponse = theResponseList.count();
	if (theComputeModelResponseStandardDeviationSensitivities) {
		dBetadTotalStdvVector = gsl_vector_calloc(numResponse);
		dBetadTotalEpistemicStdvVector = gsl_vector_calloc(numResponse);
		dBetadModelStdvVector = gsl_vector_calloc(numResponse);
		dBetadModelEpistemicStdvVector = gsl_vector_calloc(numResponse);
	}



	// Iterating on the levels: Level 0 is each hazard alone; level 1 is the coincidence of two hazards; ...
	double probabilityTimesLambdaSum = 0.0;
	double OverallCorrectedFr = 1.0;
	int currentCombinationIndex = -1;
	for (int i = 0; i < combinationVectorLevelsList.count(); i++) {
		QList<QVector<int> > combinationVectorList = combinationVectorLevelsList[i];
		
		// Iterating on each scenario in the level, e.g., for level 2, scenarios are: a. hazards 1 and 2 are on; b. hazards 1 and 3 are on; ...
		double thisLevelProbTimesLambdaSum = 0.0;
		double thisLevelCorrectedFr = 1.0;
		for (int j = 0; j < combinationVectorList.count(); j++) {
			currentCombinationIndex++;

			// The combinationVector includes a vector of indices of hazards that should be on in the current scenario 
			QVector<int> combinationVector = combinationVectorList[j];

			// Informing the user which hazards are being run
			QString hazardNamesStr;
			if (combinationVector.count() == 1) {
				hazardNamesStr = "Hazard \"" + hazardModelList[combinationVector[0]]->objectName() + "\"";
				rDebug() << "Analyzing" << qPrintable(hazardNamesStr) << "...";
			} else {
				hazardNamesStr = "Hazards ";
				for (int k = 0; k < combinationVector.count(); k++) {
					hazardNamesStr += "\"" + hazardModelList[combinationVector[k]]->objectName() + "\"";
					if (k == combinationVector.count() - 1) {

					} else if (k == combinationVector.count() - 2) {
						hazardNamesStr += " and ";
					} else {
						hazardNamesStr += ", ";
					}
				}
				rDebug() << "Analyzing" << qPrintable(hazardNamesStr) << "...";
			}
			theCurrentHazardNamesStr = hazardNamesStr;

			

			//// If we're in levels 0 (only 1 hazard is on), and lambda*mud=1.0, then no analysis is needed for single hazards other than this hazard.
			//if (i == 0) {
			//	if (lambdaVector[combinationVector[0]] * mudVector[combinationVector[0]] == 1.0) {
			//		????
			//	}
			//}


			// Modifying the model list and response list of the function by eliminating the elements that belong to the "off" hazards
			QList<RModel *> thisAnalysisModelList = theModelList;
			QList<RResponse *> thisAnalysisResponseList = theResponseList;
			QList<RParameter *> thisAnalysisExplicitParameterList = theExplicitParameterList;
			for (int k = 0; k < hazardModelList.count(); k++) {
				if (!combinationVector.contains(k)) {
					// Setting the value of the responses of the magnitude model of "off" hazards to Zero
					RModel *magnitudeModel = qobject_cast<RModel *>(hazardModelList[k]->getMagnitudeModel());
					magnitudeModel->setResponseAnalyzed(true,0);
					bool ok = thisAnalysisModelList.removeOne(magnitudeModel);
					QList<RResponse *> magnitudeResponseList = magnitudeModel->findChildren<RResponse *>();
					for (int l = 0; l < magnitudeResponseList.count(); l++) {
						ok = thisAnalysisResponseList.removeOne(magnitudeResponseList[l]);
						ok = thisAnalysisExplicitParameterList.removeOne(magnitudeResponseList[l]);
						magnitudeResponseList[l]->setCurrentValue(0);
					}

					QList<RModel *> modelList = hazardModelList[k]->getAssociatedModels();
					for (int l = 0; l < modelList.count(); l++) {
						modelList[l]->setResponseAnalyzed(true,0);
						ok = thisAnalysisModelList.removeOne(modelList[l]);
						QList<RResponse *> responseList = modelList[l]->findChildren<RResponse *>();
						for (int m = 0; m < responseList.count(); m++) {
							ok = thisAnalysisResponseList.removeOne(responseList[m]);
							ok = thisAnalysisExplicitParameterList.removeOne(responseList[m]);
						}
					}
				}
			}

			// Assigning the modified lists to the functions (This prevents the function to run the "off" hazard models when running, e.g., FORM.)
			theLimitStateFunction.data()->setResponseList(thisAnalysisResponseList);
			theLimitStateFunction.data()->setModelList(thisAnalysisModelList);
			theLimitStateFunction.data()->setExplicitParameterList(thisAnalysisExplicitParameterList);


			// Resetting the models, clearing the Gradient Map in responses, and clearing DDM maps in models and the function. We have to do this, because we are not calling RFunction::populateLists() before the analysis for this hazard (because we do not want the lists to be established again). However, these clearing an resetting, which is normally done in RFunction::populateLists() is needed anyway.
			theLimitStateFunction.data()->clearDDMMaps();  // This will also clear the DDM maps for models
			for (int k = 0; k < thisAnalysisResponseList.count(); k++) {
				thisAnalysisResponseList[k]->clearGradientMap();
			}
			for (int k = 0; k < thisAnalysisModelList.count(); k++) {
				thisAnalysisModelList[k]->reset();
			}


			
			// Calculating the probability of failure pf = 1-Fy(r)
			double pf = 0;
			double beta = 0;
			int sensitivityAnalysisStatus;
			if (theAnalysisType == RLoadCoincidenceAnalyzer::FORM || theAnalysisType == RLoadCoincidenceAnalyzer::ImportanceSampling) {
				theFORMAnalyzer.data()->setComputeRandomVariableSensitivities(theComputeRandomVariableSensitivities || theComputeModelResponseStandardDeviationSensitivities);
				theFORMAnalyzer.data()->setComputeDecisionVariableSensitivities(theComputeDecisionVariableSensitivities);
				theFORMAnalyzer.data()->setComputeModelResponseStandardDeviationSensitivities(theComputeModelResponseStandardDeviationSensitivities);
				theFORMAnalyzer.data()->setCorrectProbabilityWithFirstPrincipalCurvature(theCorrectProbabilityWithFirstPrincipalCurvature);
				theFORMAnalyzer.data()->setPrintSensitivities(thePrintSensitivities);

				
				if (startPointMatrix) {
					// Assign the value of the start point that is given by the user in theStartPointFile
					for (int i = 0; i < numRVs; i++) {
						theRandomVariableList[i]->setCurrentValue(gsl_matrix_get(startPointMatrix, i, currentCombinationIndex));
					}
				} else {
					// Restor the currentValue of all RVs from the backup if the start point of the search in FORM is the current value
					if (solver->getStartPoint() == RObject::CurrentValue) {
						for (int i = 0; i < numRVs; i++) {
							theRandomVariableList[i]->setCurrentValue(currentValueBackup[i]);
						}
					}
				}


				result = theFORMAnalyzer.data()->analyze(false);
				sensitivityAnalysisStatus = result;
				if (result < 0) {
					rCritical() << "Warning: Could not perform the FORM analysis. Assuming a failure probability of 0";
					//return -1;
				
					pf = 0;

					if (theComputeRandomVariableSensitivities) {
						gsl_vector_set_zero(dBetaJdMeanVector);
						gsl_vector_set_zero(dBetaJdStdvVector);
						gsl_vector_set_zero(alphaJVector);
						gsl_vector_set_zero(gammaJVector);
						
						if (theCorrectProbabilityWithFirstPrincipalCurvature) {
							gsl_vector_set_zero(dKappaJdMeanVector);
							gsl_vector_set_zero(dKappaJdStdvVector);
						}
					}
				} else {
					pf = solver->getFailureProbability();
					beta = solver->getDistance();

					if (designPointMatrix) {
						// Storing thr design point for this combination of hazards in the designPointMatrix if the user has asked to
						gsl_matrix_set_col(designPointMatrix, currentCombinationIndex, solver->get_x());
					}

					if (theComputeRandomVariableSensitivities) {
						gsl_vector_memcpy(dBetaJdMeanVector, theFORMAnalyzer.data()->getdBetadMeanVector());
						gsl_vector_memcpy(dBetaJdStdvVector, theFORMAnalyzer.data()->getdBetadStdvVector());
						gsl_vector_memcpy(alphaJVector, solver->get_alpha());
						gsl_vector_memcpy(gammaJVector, solver->get_gamma());
						
						if (theCorrectProbabilityWithFirstPrincipalCurvature) {
							gsl_vector_memcpy(dKappaJdMeanVector, theFORMAnalyzer.data()->getdKappadMeanVector());
							gsl_vector_memcpy(dKappaJdStdvVector, theFORMAnalyzer.data()->getdKappadStdvVector());
						}
					}
				}
			}

			if (theAnalysisType == RLoadCoincidenceAnalyzer::Sampling || theAnalysisType == RLoadCoincidenceAnalyzer::ImportanceSampling) {			
				result = theSamplingAnalyzer.data()->analyze(false);
				if (result < 0) {
					rCritical() << "Error: Could not perform the sampling analysis.";
					return -1;
				}
				
				RFailureProbabilityAccumulator *failureAccumulator = qobject_cast<RFailureProbabilityAccumulator *>(accumulator);
				if (failureAccumulator) {
					pf = failureAccumulator->getFailureProbability(theLimitStateFunction.data());
					beta = -gsl_cdf_gaussian_Pinv(pf, 1.0);
				}
			}

			// Calculating the lambda of the coincidence:
			double coincidenceLambda = 0;
			double coincidencemud = 0;
			if (i > 0) {
				double lambdaProduct = 1.0;
				double mudProduct = 1.0;
				double mudInverseSum = 0;
				for (int k = 0; k < combinationVector.count(); k++) {
					int index = combinationVector[k];
					lambdaProduct *= lambdaVector[index];
					mudProduct *= mudVector[index];
					mudInverseSum += 1.0 / mudVector[index];
				}
				coincidenceLambda = lambdaProduct * mudProduct * mudInverseSum;
				rDebug() << "The coincidence non-zero occurrence rate is" << coincidenceLambda << ".";
				coincidencemud = mudProduct * ( mudProduct * mudInverseSum );
				rDebug() << "The coincidence mean duration is" << coincidencemud << "."; 			
			} else {
				coincidenceLambda = lambdaVector[combinationVector[0]];
			}

			// Calculating the life-time maximum value F(r) = exp(-lamda*T*(1-Fy(r))) where 1-Fy(r) = pf
			// and Corrected F(r) = (1- lambda*mud*(1-Fy(r)))*exp(-lamda*T*(1-Fy(r))) where 1-Fy(r) = pf
			double Fr = exp(-coincidenceLambda * T * pf); //???????????????
			double FrCorrected = (1- coincidenceLambda*coincidencemud*pf)*exp(-coincidenceLambda * T * pf); //???????????????
			rDebug() << "Lifetime probability of exceedance, according to Wen's approximate formula, due to" << qPrintable(hazardNamesStr) << "=" << 1.0 - Fr;		
			rDebug() << " ";
			rDebug() << "Lifetime  probability of exceedance due to" << qPrintable(hazardNamesStr) << "=" << 1.0 - FrCorrected;
			rDebug() << " ";
			rDebug() << " ";
			rDebug() << " ";


			
			// TEMPORARY CODE FOR DEBUGGING
			//////////////////###################3
			//if (i ==0 && j==0) {////////////////////###################3
			//	beta +=0.01;////////////////////###################3
			//}////////////////////###################3
			////////////////////###################3




			double betaPDF = gsl_ran_gaussian_pdf(beta, 1.0);
			if (pf == 1.0 || pf == 0.0) {
				betaPDF = 0;
			}

			double betaSensitivityFactor = coincidenceLambda * betaPDF;
			double kappaSensitivityFactor;
			if (theCorrectProbabilityWithFirstPrincipalCurvature) {
				// Obtain the first principal curvature
				double kappa = solver->getFirstPrincipalCurvature();



				// TEMPORARY CODE FOR DEBUGGING
				////////////////////###################3
				//pf = gsl_cdf_gaussian_P(-beta,1.0)/ sqrt(1.0 + betaPDF / gsl_cdf_gaussian_P(-beta,1.0) * kappa);////////////////////###################3
				////////////////////###################3



				double betaCCDF = gsl_cdf_gaussian_P(-beta, 1.0); // Warning: Non-convergence of FROM results in beta=0 in this code.

				betaSensitivityFactor *= (2 + 3 * kappa * betaPDF / betaCCDF - kappa * beta) / (2.0 * gsl_pow_3(sqrt(1.0 + kappa * betaPDF / betaCCDF)));

				kappaSensitivityFactor = betaSensitivityFactor / (2.0 * gsl_pow_3(sqrt(1.0 + kappa * betaPDF / betaCCDF)));
			}

			
			// Compute the decision variables sensitivities if the selected analysis type includes FORM: d(Beta)/d(Ci) (Ci is the incremental cost of the decision variable i)
			if (theComputeDecisionVariableSensitivities) {
				if (sensitivityAnalysisStatus >= 0) {   //  Check if the FORM has convereged; if not, it is assumed that the failure probability is very very small (near zero) an therefore, this analysis does not play a role in the overall decision sensitivity.
					if (!theDecisionVariableList.isEmpty()) {
						for (int k = 0; k < numDVs; k++) {
							RContinuousDecisionVariable *cDV = qobject_cast<RContinuousDecisionVariable *>(theDecisionVariableList[k]);
							if (cDV) {
								double dBeta_dC = cDV->getBetaSensitivityWrtCost();	

								
							
								// TEMPORARY CODE FOR DEBUGGING
								////////////////////###################3
								//dBeta_dC = 0.0;
								//if (i == 0 && j==0) {
								//	dBeta_dC = 1.0;
								//}
								////////////////////###################3



								decisionSensitivityVector[k] += dBeta_dC * betaSensitivityFactor;
							}
						}
					}
				}
			}




			// Compute the random variables sensitivities if the selected analysis type includes FORM
			if (theComputeRandomVariableSensitivities) {
				if (sensitivityAnalysisStatus >= 0) {   //  Check if the FORM has convereged; if not, it is assumed that the failure probability is very very small (near zero) an therefore, this analysis does not play a role in the overall sensitivity.
					gsl_vector_scale(alphaJVector, betaSensitivityFactor);
					gsl_vector_add(alphaVector, alphaJVector);

					gsl_vector_scale(gammaJVector, betaSensitivityFactor);
					gsl_vector_add(gammaVector, gammaJVector);

					gsl_vector_scale(dBetaJdMeanVector, betaSensitivityFactor);
					gsl_vector_add(dBetadMeanVector, dBetaJdMeanVector);

					gsl_vector_scale(dBetaJdStdvVector, betaSensitivityFactor);
					gsl_vector_add(dBetadStdvVector, dBetaJdStdvVector);
						
					if (theCorrectProbabilityWithFirstPrincipalCurvature) {
						gsl_vector_scale(dKappaJdMeanVector, kappaSensitivityFactor);
						gsl_vector_add(dBetadMeanVector, dKappaJdMeanVector);

						gsl_vector_scale(dKappaJdStdvVector, kappaSensitivityFactor);
						gsl_vector_add(dBetadStdvVector, dKappaJdStdvVector);
					}
				}
			}



			// Compute the model response stdv sitivities if the selected analysis type includes FORM
			if (theComputeModelResponseStandardDeviationSensitivities) {
				if (sensitivityAnalysisStatus >= 0) {   //  Check if the FORM has convereged; if not, it is assumed that the failure probability is very very small (near zero) an therefore, this analysis does not play a role in the overall sensitivity.
					int thisAnalysisNumResponse = thisAnalysisResponseList.count();
					for (int k = 0; k < thisAnalysisNumResponse; k++) {
						RResponse *response = thisAnalysisResponseList[k];
						double dBetadTotalStdv = response->getBetaSensitivityWrtTotalStandardDeviation() * betaSensitivityFactor;
						double dBetadTotalEpistemicStdv = response->getBetaSensitivityWrtTotalEpistemicStandardDeviation() * betaSensitivityFactor;
						double dBetadModelStdv = response->getBetaSensitivityWrtModelStandardDeviation() * betaSensitivityFactor;
						double dBetadModelEpistemicStdv = response->getBetaSensitivityWrtModelEpistemicStandardDeviation() * betaSensitivityFactor;

						int responseIndex = theResponseList.indexOf(response);

						gsl_vector_set(dBetadTotalStdvVector, responseIndex, gsl_vector_get(dBetadTotalStdvVector, responseIndex) + dBetadTotalStdv);
						gsl_vector_set(dBetadTotalEpistemicStdvVector, responseIndex, gsl_vector_get(dBetadTotalEpistemicStdvVector, responseIndex) + dBetadTotalEpistemicStdv);
						gsl_vector_set(dBetadModelStdvVector, responseIndex, gsl_vector_get(dBetadModelStdvVector, responseIndex) + dBetadModelStdv);
						gsl_vector_set(dBetadModelEpistemicStdvVector, responseIndex, gsl_vector_get(dBetadModelEpistemicStdvVector, responseIndex) + dBetadModelEpistemicStdv);
					}
				}
			}




			thisLevelProbTimesLambdaSum += coincidenceLambda * pf; 
			thisLevelCorrectedFr = thisLevelCorrectedFr * FrCorrected;
		}
		probabilityTimesLambdaSum += thisLevelProbTimesLambdaSum;
		OverallCorrectedFr = OverallCorrectedFr*thisLevelCorrectedFr;
	}

	double finalPf = 1.0 - exp(-probabilityTimesLambdaSum * T);  
	double finalBeta = -gsl_cdf_gaussian_Pinv(finalPf, 1.0);


	double finalPfCorrected = 1.0 - OverallCorrectedFr;

	
	double sensitivityCoeff = T * (1 - finalPf) / gsl_ran_gaussian_pdf(finalBeta, 1.0);
	
	
	// Continue computing the random variables sensitivities if the selected analysis type includes FORM
	if (theComputeRandomVariableSensitivities) {
		gsl_vector_scale(alphaVector, sensitivityCoeff);
		gsl_vector_scale(gammaVector, sensitivityCoeff);
		gsl_vector_scale(dBetadMeanVector, sensitivityCoeff);
		gsl_vector_scale(dBetadStdvVector, sensitivityCoeff);

		// Scaling the alphaVector and gammaVector vectors
		double alphaNorm = gsl_blas_dnrm2(alphaVector);
		gsl_vector_scale(alphaVector, 1.0 / alphaNorm);
		double gammaNorm = gsl_blas_dnrm2(gammaVector);
		gsl_vector_scale(gammaVector, 1.0 / gammaNorm);



		// Computing the hypothetical realization of the random varilabes at the hypothetical design point of the mult-hazard analysis: y* = beta*alpha
		gsl_vector *hypotheticalYStar = gsl_vector_calloc(numRVs);
		gsl_vector *hypotheticalXStar = gsl_vector_calloc(numRVs);
		gsl_vector_memcpy(hypotheticalYStar, alphaVector);
		gsl_vector_scale(hypotheticalYStar, finalBeta);
		RStepperNonlinSingleConstrSolver *stepperSolver = qobject_cast<RStepperNonlinSingleConstrSolver *>(solver);
		if (stepperSolver) {
			RTransformer *transformer = qobject_cast<RTransformer *>(stepperSolver->getTransformer());
			transformer->setRandomVariableList(&theRandomVariableList);
			result = transformer->transformPointToOriginalSpace(hypotheticalYStar, hypotheticalXStar,0);

			if (result < 0) {
				rCritical() << "Error: Could not transform from standard-normal space to original space when computing the hypothetical realization of random variables in the load coincidence analysis.";
			}
		} else {
			rCritical() << "Error: Could not compute the hypothetical realization of random variables in the load coincidence analysis. The algorithm is yet to be implemented for a solver other than the Stepper.";
		}




		if (thePrintSensitivities) {
			rDebug() << " ";
			rDebug() << " ";
			rDebug() << "Random variable sensitivities:";
			rDebug() << " ";
			rDebug() << "RandomVariableObjectName\tAlphaImportanceMeasure\tGammaImportanceMeasure\tDeltaSensitivityMeasure\tEtaSensitivityMeasure\tKappaSensitivityMeasure\tBetaSensitivityWrtMean\tBetaSensitivityWrtStandardDeviation";
		}

		for (int i = 0; i < numRVs; i++) {
			RContinuousRandomVariable *cRV = qobject_cast<RContinuousRandomVariable *>(theRandomVariableList[i]);
			if (cRV) {	
				// Compute sensitivity vectors
				double mean = cRV->getMeanValue(0);
				double stdv = cRV->getStandardDeviationValue(0);
				double dBetadMeani = gsl_vector_get(dBetadMeanVector, i);
				double dBetadStdvi = gsl_vector_get(dBetadStdvVector, i);
				double kappaElement = 0.0;
				if (mean != 0.0) {
					kappaElement = -dBetadMeani * stdv / ((stdv / mean) * (stdv / mean)) + dBetadStdvi * mean;	
				}
				double alphaElement = gsl_vector_get(alphaVector, i);
				double gammaElement = gsl_vector_get(gammaVector, i);
				double deltaElement = dBetadMeani * stdv;
				double etaElement = dBetadStdvi * stdv;

				// Assign the vectors to random variable properties
				cRV->setAlphaImportanceMeasure(alphaElement);
				cRV->setGammaImportanceMeasure(gammaElement);
				cRV->setDeltaSensitivityMeasure(deltaElement);
				cRV->setEtaSensitivityMeasure(etaElement);
				cRV->setKappaSensitivityMeasure(kappaElement);
				cRV->setBetaSensitivityWrtMean(dBetadMeani);
				cRV->setBetaSensitivityWrtStandardDeviation(dBetadStdvi);

				// Assigning the hypothetical x* as the current value of random variables
				cRV->setCurrentValue(gsl_vector_get(hypotheticalXStar, i));

				if (thePrintSensitivities) {
					rDebug() << qPrintable(QString("%1\t%2\t%3\t%4\t%5\t%6\t%7\t%8").arg(cRV->objectName()).arg(alphaElement, 0, 'g', 14).arg(gammaElement, 0, 'g', 14).arg(deltaElement, 0, 'g', 14).arg(etaElement, 0, 'g', 14).arg(kappaElement, 0, 'g', 14).arg(dBetadMeani, 0, 'g', 14).arg(dBetadStdvi, 0, 'g', 14));
				}
			}
		}
		if (dBetaJdMeanVector) {
			gsl_vector_free(dBetaJdMeanVector);
			dBetaJdMeanVector = 0;
		}
		if (dBetaJdStdvVector) {
			gsl_vector_free(dBetaJdStdvVector);
			dBetaJdStdvVector = 0;
		}
		if (dBetaJdMeanVector) {
			gsl_vector_free(dBetaJdMeanVector);
			dBetaJdMeanVector = 0;
		}
		if (dBetaJdStdvVector) {
			gsl_vector_free(dBetaJdStdvVector);
			dBetaJdStdvVector = 0;
		}
		if (theCorrectProbabilityWithFirstPrincipalCurvature) {
			if (dKappaJdMeanVector) {
				gsl_vector_free(dKappaJdMeanVector);
				dKappaJdMeanVector = 0;
			}
			if (dKappaJdStdvVector) {
				gsl_vector_free(dKappaJdStdvVector);
				dKappaJdStdvVector = 0;
			}
		}
		if (alphaJVector) {
			gsl_vector_free(alphaJVector);
			alphaJVector = 0;
		}
		if (gammaJVector) {
			gsl_vector_free(gammaJVector);
			gammaJVector = 0;
		}
		if (dBetadMeanVector) {
			gsl_vector_free(dBetadMeanVector);
			dBetadMeanVector = 0;
		}
		if (dBetadStdvVector) {
			gsl_vector_free(dBetadStdvVector);
			dBetadStdvVector = 0;
		}
		if (alphaVector) {
			gsl_vector_free(alphaVector);
			alphaVector = 0;
		}
		if (gammaVector) {
			gsl_vector_free(gammaVector);
			gammaVector = 0;
		}
		if (hypotheticalYStar) {
			gsl_vector_free(hypotheticalYStar);
			hypotheticalYStar = 0;
		}
		if (hypotheticalXStar) {
			gsl_vector_free(hypotheticalXStar);
			hypotheticalXStar = 0;
		}
	}


	// Compute the model response stdv sitivities if the selected analysis type includes FORM
	if (theComputeModelResponseStandardDeviationSensitivities) {
		gsl_vector_scale(dBetadTotalStdvVector, sensitivityCoeff);
		gsl_vector_scale(dBetadTotalEpistemicStdvVector, sensitivityCoeff);
		gsl_vector_scale(dBetadModelStdvVector, sensitivityCoeff);
		gsl_vector_scale(dBetadModelEpistemicStdvVector, sensitivityCoeff);
		
		if (thePrintSensitivities && !theResponseList.isEmpty()) {
			rDebug() << " ";
			rDebug() << " ";
			rDebug() << "Response standard deviation sensitivities:";
			rDebug() << " ";
			rDebug() << "ResponseObjectName\tBetaSensitivityWrtTotalStandardDeviation\tBetaSensitivityWrtTotalEpistemicStandardDeviation\tBetaSensitivityWrtModelStandardDeviation\tBetaSensitivityWrtModelEpistemicStandardDeviation";
		}

		for (int i = 0; i < numResponse; i++) {
			RResponse *response = theResponseList[i];

			double dBetadTotalStdv = gsl_vector_get(dBetadTotalStdvVector, i);
			double dBetadTotalEpistemicStdv = gsl_vector_get(dBetadTotalEpistemicStdvVector, i);
			double dBetadModelStdv = gsl_vector_get(dBetadModelStdvVector, i);
			double dBetadModelEpistemicStdv = gsl_vector_get(dBetadModelEpistemicStdvVector, i);

			response->setBetaSensitivityWrtTotalStandardDeviation(dBetadTotalStdv);
			response->setBetaSensitivityWrtTotalEpistemicStandardDeviation(dBetadTotalEpistemicStdv);
			response->setBetaSensitivityWrtModelStandardDeviation(dBetadModelStdv);
			response->setBetaSensitivityWrtModelEpistemicStandardDeviation(dBetadModelEpistemicStdv);

			if (thePrintSensitivities) {
				rDebug() << qPrintable(QString("%1\t%2\t%3\t%4\t%5").arg(response->objectName()).arg(dBetadTotalStdv, 0, 'g', 14).arg(dBetadTotalEpistemicStdv, 0, 'g', 14).arg(dBetadModelStdv, 0, 'g', 14).arg(dBetadModelEpistemicStdv, 0, 'g', 14));
			}
		}

		if (dBetadTotalStdvVector) {
			gsl_vector_free(dBetadTotalStdvVector);
			dBetadTotalStdvVector = 0;
		}
		if (dBetadTotalEpistemicStdvVector) {
			gsl_vector_free(dBetadTotalEpistemicStdvVector);
			dBetadTotalEpistemicStdvVector = 0;
		}
		if (dBetadModelStdvVector) {
			gsl_vector_free(dBetadModelStdvVector);
			dBetadModelStdvVector = 0;
		}
		if (dBetadModelEpistemicStdvVector) {
			gsl_vector_free(dBetadModelEpistemicStdvVector);
			dBetadModelEpistemicStdvVector = 0;
		}
	}

	// ATTENTION: DECISION VARIABLE SENSITIVITY MUST BE DONE AS THE LAST TASK, BECAUSE WEHN EVALUATING THE GRADIENT WRT. DECISION VARIABLES USING DDM, THE GRADIENT MAPS FOR RANDOM VARIABLES ARE ERASED.
	// Continue computing the decision variables sensitivities if the selected analysis type includes FORM: d(Beta)/d(Ci) (Ci is the incremental cost of the decision variable i)
	if (theComputeDecisionVariableSensitivities) {
		double finalBetaPDF = gsl_ran_gaussian_pdf(finalBeta, 1.0);

		if (thePrintSensitivities && !theDecisionVariableList.isEmpty()) {
			rDebug() << " ";
			rDebug() << " ";
			rDebug() << "Decision variable sensitivities:";
			rDebug() << " ";
			rDebug() << "DecisionVariableObjectName\tBetaSensitivityWrtCost\tPfSensitivityWrtCost";
		}

		for (int i = 0; i < numDVs; i++) {
			RContinuousDecisionVariable *cDV = qobject_cast<RContinuousDecisionVariable *>(theDecisionVariableList[i]);
			if (cDV) {	
				decisionSensitivityVector[i] *= sensitivityCoeff;
				double dBeta_dC = decisionSensitivityVector[i];
				double dPf_dC = -dBeta_dC * finalBetaPDF;
				cDV->setBetaSensitivityWrtCost(dBeta_dC);
				cDV->setPfSensitivityWrtCost(dPf_dC);

				if (thePrintSensitivities) {
					rDebug() << qPrintable(QString("%1\t%2\t%3").arg(cDV->objectName()).arg(dBeta_dC, 0, 'g', 14).arg(dPf_dC, 0, 'g', 14));
				}
			}
		}	
	}

	if (designPointMatrix) {
		// Store the design points due to all hazards and hazard combinations in theDesignPointFile if the user has asked to
		theDomain->print(designPointMatrix, 0, theDesignPointFile);
		
		gsl_matrix_free(designPointMatrix);
		designPointMatrix = 0;
	}
	
	if (startPointMatrix) {
		gsl_matrix_free(startPointMatrix);
		startPointMatrix = 0;
	}




	rDebug() << "";
	rDebug() << "Load combination analysis is complete. ";
	rDebug() << "";
	rDebug() << "The lifetime reliability index:";
	rDebug() << "Beta =" << qPrintable(QString("%1").arg(finalBeta, 0, 'g', 14));//finalBeta;
	rDebug() << "";
	rDebug() << "The lifetime probability of failure:";
	rDebug() << "pf =" << finalPf;
	rDebug() << "";
	rDebug() << "The lifetime Corrected probability of failure:";
	rDebug() << "pf =" << finalPfCorrected;
	rDebug() << "";

	QDateTime finishTime = QDateTime::currentDateTime();
	double numDays = startTime.daysTo(finishTime);
	double numMilliSecs = startTime.time().msecsTo(finishTime.time());
	double numSecs = numDays * 86400.0 + numMilliSecs / 1000.0;

	rDebug() << "##### LOAD COINCIDENCE ANALYSIS DONE IN" << numSecs << "SECONDS.";
	rDebug() << "";
	rDebug() << "";






	/*// TEMPORARY code to store pf and d(pf)/d(dv) in a text file named "pf.txt"
	QString fileName = QString("pf.txt");
	QFile *output = new QFile(fileName);
	if (output->exists()) {
		output->remove();
	}
	if (!output->open(QIODevice::WriteOnly | QIODevice::Text)) {
		rCritical() << "Warning: Could not create the file" << fileName;
	}

	QString tempString = QString("%1\n").arg(finalPf, 0, 'g', 14);
	for (int i = 0; i < numDVs; i++) {
		RContinuousDecisionVariable *cDV = qobject_cast<RContinuousDecisionVariable *>(theDecisionVariableList[i]);
		if (cDV) {	
			tempString += QString("%1\n").arg(cDV->getPfSensitivityWrtCost(), 0, 'g', 14);
		}
	}
	QByteArray byteArray;
	byteArray.append(tempString);
	output->write(byteArray);
	output->flush();
	output->close();
	delete output;
	// END of the TEMPORARY code*/






	// Resetting the function because its lists are manipulated
	theLimitStateFunction.data()->reset();
	

	if (qobject_cast<QApplication*>(QCoreApplication::instance()) != 0) {
		// Disconnecting the signal that is trigerred when a sub-window is added to the main window: Using this, the title of the plots generated by anaylsis tools can be modified
		bool ok = disconnect(theMainWindow, SIGNAL(subWindowAdded(QWidget *)), this, SLOT(onSubWindowAdded(QWidget *)));
	}

	return 0;
}

void RLoadCoincidenceAnalyzer::onSubWindowAdded(QWidget *subWindow)
{
	RPlotWidget *plot = qobject_cast<RPlotWidget *>(subWindow);
	if (plot) {
		QString title  = plot->windowTitle() + " for " + theCurrentHazardNamesStr;
		plot->setWindowTitle(title);
	}
}


bool RLoadCoincidenceAnalyzer::canBeRunParallelly() {
	return false;
}