// Last modified by Mojtaba on March 19, 2014

#include "RFORMAnalyzer.h"

#include <QMap>
#include <QCoreApplication>
#include <QApplication>
#include <QDateTime>

#include "RMainWindow.h"
#include "RDomain.h"
#include "RFunction.h"
#include "RNonlinSingleConstrSolver.h"
#include "RStepperNonlinSingleConstrSolver.h"
#include "RStepSizeSearcher.h"
#include "RStepDirectionSearcher.h"
#include "RConvergenceChecker.h"
#include "RTransformer.h"
#include "RContinuousDecisionVariable.h"
#include "RContinuousRandomVariable.h"
#include "RNatafTransformer.h"
#include "RModel.h"
#include "RResponse.h"
#include "RCorrelation.h"

#include <gsl/gsl_blas.h>
#include <gsl/gsl_cdf.h>
#include <gsl/gsl_randist.h>


RFORMAnalyzer::RFORMAnalyzer(QObject *parent, QString name)
	: RAnalyzer(parent, name)
{
	//$$ theLimitStateFunction = 0;
	//$$ theNonlinearSingleConstraintSolver = 0;
	theComputeRandomVariableSensitivities = true;
	theComputeDecisionVariableSensitivities = true;
	theComputeModelResponseStandardDeviationSensitivities = true;
	thePrintSensitivities = false;
	theCorrectProbabilityWithFirstPrincipalCurvature = false;

	betaSensitivityWrtMeanVector = 0;
	betaSensitivityWrtStdvVector = 0;
	kappaSensitivityWrtMeanVector = 0;
	kappaSensitivityWrtStdvVector = 0;
}

RFORMAnalyzer::~RFORMAnalyzer()
{
	if (betaSensitivityWrtMeanVector) {
		gsl_vector_free(betaSensitivityWrtMeanVector);
		betaSensitivityWrtMeanVector = 0;
	}
	if (betaSensitivityWrtStdvVector) {
		gsl_vector_free(betaSensitivityWrtStdvVector);
		betaSensitivityWrtStdvVector = 0;
	}
	if (kappaSensitivityWrtMeanVector) {
		gsl_vector_free(kappaSensitivityWrtMeanVector);
		kappaSensitivityWrtMeanVector = 0;
	}
	if (kappaSensitivityWrtStdvVector) {
		gsl_vector_free(kappaSensitivityWrtStdvVector);
		kappaSensitivityWrtStdvVector = 0;
	}
}

QObject * RFORMAnalyzer::getLimitStateFunction() const	
{	
	return theLimitStateFunction.data();
}	
	
void RFORMAnalyzer::setLimitStateFunction(QObject *value)	
{	
	theLimitStateFunction = qobject_cast<RFunction *>(value);
}	
	
QObject * RFORMAnalyzer::getNonlinearSingleConstraintSolver() const	
{	
	return theNonlinearSingleConstraintSolver.data();
}	
	
void RFORMAnalyzer::setNonlinearSingleConstraintSolver(QObject *value)	
{	
	theNonlinearSingleConstraintSolver = qobject_cast<RNonlinSingleConstrSolver *>(value);
}	
	
bool RFORMAnalyzer::getComputeRandomVariableSensitivities() const	
{	
	return theComputeRandomVariableSensitivities;
}	
	
void RFORMAnalyzer::setComputeRandomVariableSensitivities(bool value)	
{	
	theComputeRandomVariableSensitivities = value;
}	
	
bool RFORMAnalyzer::getComputeDecisionVariableSensitivities() const	
{	
	return theComputeDecisionVariableSensitivities;
}	
	
void RFORMAnalyzer::setComputeDecisionVariableSensitivities(bool value)	
{	
	theComputeDecisionVariableSensitivities = value;
}	
	
bool RFORMAnalyzer::getComputeModelResponseStandardDeviationSensitivities() const	
{	
	return theComputeModelResponseStandardDeviationSensitivities;
}	
	
void RFORMAnalyzer::setComputeModelResponseStandardDeviationSensitivities(bool value)	
{	
	theComputeModelResponseStandardDeviationSensitivities = value;
}	
	
bool RFORMAnalyzer::getPrintSensitivities() const	
{	
	return thePrintSensitivities;
}	
	
void RFORMAnalyzer::setPrintSensitivities(bool value)	
{	
	thePrintSensitivities = value;
}	
	
bool RFORMAnalyzer::getCorrectProbabilityWithFirstPrincipalCurvature() const	
{	
	return theCorrectProbabilityWithFirstPrincipalCurvature;
}	
	
void RFORMAnalyzer::setCorrectProbabilityWithFirstPrincipalCurvature(bool value)	
{	
	theCorrectProbabilityWithFirstPrincipalCurvature = value;
}	

int RFORMAnalyzer::analyze()
{
	int result = this->analyze(true);
	if (result < 0) {
		return result;
	}
	return 0;
}

int RFORMAnalyzer::analyze(bool populateListsInFunction)
{
	// Inform the user that the analysis has started
	rDebug() << "##### STARTING FORM ANALYSIS WITH FUNCTION" << theLimitStateFunction.data()->objectName() << "...";
	rDebug() << "";

	QDateTime startTime = QDateTime::currentDateTime();


	if (!theLimitStateFunction) {
		rCritical() << "Error in" << objectName() << ": Please choose a function and try again";
		return -1;
	}

	// Checking if all analysis tools are provided
	int result = checkProperties();
	if (result < 0) {
		rCritical() << "Error in" << objectName() << ": Could find one or more necessary tools";
		return -1;
	}

	if (qobject_cast<QApplication*>(QCoreApplication::instance()) != 0) {
		theMainWindow->raiseDock("Output");
	}

	if (theComputeModelResponseStandardDeviationSensitivities) {
		theComputeRandomVariableSensitivities = true;
	}

	
	result = theNonlinearSingleConstraintSolver.data()->findDesignPoint(theLimitStateFunction.data(), populateListsInFunction);
	if (result < 0) {
		return result;
	}
	
	double beta = theNonlinearSingleConstraintSolver.data()->getDistance();
	double pf = theNonlinearSingleConstraintSolver.data()->getFailureProbability();
	
	
	// If the user has chosen to correct the results using the first principal curvature
	double betaSORM;
	double pfSORM;
	double firstPrincipalCurvature;
	if (theCorrectProbabilityWithFirstPrincipalCurvature) {
		theNonlinearSingleConstraintSolver.data()->correctWithFirstPrincipalCurvature();
		pfSORM = theNonlinearSingleConstraintSolver.data()->getFailureProbability();
		firstPrincipalCurvature = theNonlinearSingleConstraintSolver.data()->getFirstPrincipalCurvature();

		// Compute the corresponding beta
		betaSORM = -gsl_cdf_gaussian_Pinv(pfSORM, 1.0);
	}


	// Compute the random variable sensitivities
	if (theComputeRandomVariableSensitivities) {
		QList<RRandomVariable *> randomVariableList = *theLimitStateFunction.data()->getRandomVariableList();
		int numRVs = randomVariableList.count();

		// Free the vectors if they are aleardy allocated
		if (betaSensitivityWrtMeanVector) {
			gsl_vector_free(betaSensitivityWrtMeanVector);
			betaSensitivityWrtMeanVector = 0;
		}
		if (betaSensitivityWrtStdvVector) {
			gsl_vector_free(betaSensitivityWrtStdvVector);
			betaSensitivityWrtStdvVector = 0;
		}
		if (kappaSensitivityWrtMeanVector) {
			gsl_vector_free(kappaSensitivityWrtMeanVector);
			kappaSensitivityWrtMeanVector = 0;
		}
		if (kappaSensitivityWrtStdvVector) {
			gsl_vector_free(kappaSensitivityWrtStdvVector);
			kappaSensitivityWrtStdvVector = 0;
		}

		betaSensitivityWrtMeanVector = gsl_vector_calloc(numRVs);
		betaSensitivityWrtStdvVector = gsl_vector_calloc(numRVs);

		if (theCorrectProbabilityWithFirstPrincipalCurvature) {
			kappaSensitivityWrtMeanVector = gsl_vector_calloc(numRVs);
			kappaSensitivityWrtStdvVector = gsl_vector_calloc(numRVs);
		}

		
		
		theNonlinearSingleConstraintSolver.data()->computeRandomVariableSensitivities(betaSensitivityWrtMeanVector, betaSensitivityWrtStdvVector, kappaSensitivityWrtMeanVector, kappaSensitivityWrtStdvVector);
		

		
		// Compute delta, eta, and kappa (kappa is the sensitivity wrt. the coefficient of variation)
		
		/*gsl_vector *deltaVector = gsl_vector_calloc(numRVs);
		gsl_vector *etaVector = gsl_vector_calloc(numRVs);
		gsl_vector *kappaVector = gsl_vector_calloc(numRVs);*/

		if (thePrintSensitivities) {
			rDebug() << " ";
			rDebug() << " ";
			rDebug() << "Random variable sensitivities:";
			rDebug() << " ";
			rDebug() << "RandomVariableObjectName\tAlphaImportanceMeasure\tGammaImportanceMeasure\tDeltaSensitivityMeasure\tEtaSensitivityMeasure\tKappaSensitivityMeasure\tBetaSensitivityWrtMean\tBetaSensitivityWrtStandardDeviation";
		}

		for (int i = 0; i < numRVs; i++) {
			double mean = randomVariableList[i]->getMeanValue(0);
			double stdv = randomVariableList[i]->getStandardDeviationValue(0);
			double dBetadMean = gsl_vector_get(betaSensitivityWrtMeanVector, i);
			double dBetadStdv = gsl_vector_get(betaSensitivityWrtStdvVector, i);
			double kappaElement = 0.0;
			if (mean != 0.0) {
				kappaElement = -dBetadMean * stdv / ((stdv / mean) * (stdv / mean)) + dBetadStdv * mean;	
			}

			RContinuousRandomVariable *cRV = qobject_cast<RContinuousRandomVariable *>(randomVariableList[i]);
			double deltaElement = dBetadMean * stdv;
			double etaElement = dBetadStdv * stdv;
			cRV->setDeltaSensitivityMeasure(deltaElement);
			cRV->setEtaSensitivityMeasure(etaElement);
			cRV->setKappaSensitivityMeasure(kappaElement);
			cRV->setBetaSensitivityWrtMean(dBetadMean);
			cRV->setBetaSensitivityWrtStandardDeviation(dBetadStdv);

			if (thePrintSensitivities) {
				double alphaElement = cRV->getAlphaImportanceMeasure();
				double gammaElement = cRV->getGammaImportanceMeasure();
				rDebug() << qPrintable(QString("%1\t%2\t%3\t%4\t%5\t%6\t%7\t%8").arg(cRV->objectName()).arg(alphaElement, 0, 'g', 14).arg(gammaElement, 0, 'g', 14).arg(deltaElement, 0, 'g', 14).arg(etaElement, 0, 'g', 14).arg(kappaElement, 0, 'g', 14).arg(dBetadMean, 0, 'g', 14).arg(dBetadStdv, 0, 'g', 14));
			}

			/*gsl_vector_set(deltaVector, i, dBetadMean * stdv);
			gsl_vector_set(etaVector, i, dBetadStdv * stdv);
			gsl_vector_set(kappaVector, i, kappaElement);*/
		}

		/*// Normalize delta, eta, and kappa vectors [NOT NEEDED]
		double deltaNorm = gsl_blas_dnrm2(deltaVector);
		double etaNorm = gsl_blas_dnrm2(etaVector);
		double kappaNorm = gsl_blas_dnrm2(kappaVector);
		gsl_vector_scale(deltaVector, 1.0 / deltaNorm);
		gsl_vector_scale(etaVector, 1.0 / etaNorm);
		gsl_vector_scale(kappaVector, 1.0 / kappaNorm);

		for (int i = 0; i < randomVariableList.count(); i++) {
			RContinuousRandomVariable *cRV = qobject_cast<RContinuousRandomVariable *>(randomVariableList[i]);
			cRV->setDeltaSensitivityMeasure(gsl_vector_get(deltaVector, i));
			cRV->setEtaSensitivityMeasure(gsl_vector_get(etaVector, i));
			cRV->setKappaSensitivityMeasure(gsl_vector_get(kappaVector, i));
			cRV->setBetaSensitivityWrtMean(gsl_vector_get(betaSensitivityWrtMeanVector, i));
			cRV->setBetaSensitivityWrtStandardDeviation(gsl_vector_get(betaSensitivityWrtStdvVector, i));
		}

		if (deltaVector) {
			gsl_vector_free(deltaVector);
			deltaVector = 0;
		}
		if (etaVector) {
			gsl_vector_free(etaVector);
			etaVector = 0;
		}
		if (kappaVector) {
			gsl_vector_free(kappaVector);
			kappaVector = 0;
		}*/
	}


	if (theComputeModelResponseStandardDeviationSensitivities) {
		
		/*////////////////////////////////////////////////////////
		// TEMP Implementagtion: To see whether there is more uncertainty in earthquakes or buildings in the sensitivity paper.
		// The first 49 random variables belong to earthquake hazard
		RDomain *domain = qobject_cast<RDomain *>(theLimitStateFunction->parent());
		QList<RRandomVariable *> tempRVList1 = domain->findChildren<RRandomVariable *>();
		QList<RRandomVariable *> tempRVList;
		for (int i = 0; i < 49; i++) {
			tempRVList << tempRVList1[i];
		}
		////////////////////////////////////////////////////////*/




		// Evalaute the gradient of all models with respect to their input parameters
		// This is done by calling RFunction::evaluateGradient() and using the option to evaluate the gradient by DDM
		// The models that have DDMs, will evaluate the gradient of the model response using DDMs, and the models that don't, will do finite difference within the model
		
		// Setting the gradient analysis type to DDM
		RFunction::RGradientAnalysisType gradientTypeBackup = theLimitStateFunction.data()->getGradientAnalysisType();
		theLimitStateFunction.data()->setGradientAnalysisType(RFunction::DDM);

		theLimitStateFunction.data()->updateAnalysisStatus(0);
		result = theLimitStateFunction.data()->evaluateFunction(true, RObject::RandomVariable,0);
		if (result < 0) {
			rCritical() << "Error: Could not evaluate the value of the function for model sensitivity analysis.";
			return -1;
		}

		//result = theLimitStateFunction->evaluateGradient(RObject::RandomVariable);
		//if (result < 0) {
		//	rCritical() << "Error: Could not evaluate the gradient of the function for model sensitivity analysis.";
		//	return -1;
		//}

		// Now, all the models should have the gradients of the model response ready

		// Iterating on all models
		QList<RModel *> modelList = theLimitStateFunction.data()->getModelList();


		/*//### for debugging with finite difference
		RContinuousRandomVariable *tempCrv = qobject_cast<RContinuousRandomVariable *>(randomVariableList[3]);
		rDebug() << "Perturbed RV:" << tempCrv->objectName();
		tempCrv->setStandardDeviationValue(tempCrv->getStandardDeviationValue() * 1.001);*/

		
		// Establish the correltion matrix. Do not instantiate R is if there are no correlation objects.
		QList<RRandomVariable *> randomVariableList = *theLimitStateFunction.data()->getRandomVariableList();
		int numRVs = randomVariableList.count();
		gsl_matrix *R = 0;
		QList<RCorrelation *> correlationList = theDomain->findChildren<RCorrelation *>();
		if (!correlationList.isEmpty()) {	
			R = gsl_matrix_calloc(numRVs, numRVs);
			RRandomVariable *rv1;
			RRandomVariable *rv2;
			int index1;
			int index2;
			double rho;
			for (int i = 0; i < correlationList.count(); i++) {

				// Get the two random variable objects
				rv1 = qobject_cast<RRandomVariable *>(correlationList[i]->getRandomVariable1());
				rv2 = qobject_cast<RRandomVariable *>(correlationList[i]->getRandomVariable2());


				// Do nothing if one or both of the random variables are not subject to this analysis
				if ((!rv1) || (!rv2)) {
					rCritical() << "Warning: Two random variables must be given to the correlation object" << correlationList[i]->objectName();
					continue;
				}


				// Do nothing if the random variable wasn't found in the random variable list
				index1 = randomVariableList.indexOf(rv1);
				index2 = randomVariableList.indexOf(rv2);
				if ((index1 < 0) || (index2 < 0)) {
					continue;
				}


				// Get the correlation value
				rho = correlationList[i]->getCoefficient();


				// Set the correlation coefficient into the matrix
				gsl_matrix_set(R, index1, index2, rho);
				gsl_matrix_set(R, index2, index1, rho);
			}
		}

		if (thePrintSensitivities && !modelList.isEmpty()) {
			rDebug() << " ";
			rDebug() << " ";
			rDebug() << "Response standard deviation sensitivities:";
			rDebug() << " ";
			rDebug() << "ResponseObjectName\tTotalStandardDeviation\tTotalEpistemicStandardDeviation\tModelStandardDeviation\tModelEpistemicStandardDeviation\tBetaSensitivityWrtTotalStandardDeviation\tBetaSensitivityWrtTotalEpistemicStandardDeviation\tBetaSensitivityWrtModelStandardDeviation\tBetaSensitivityWrtModelEpistemicStandardDeviation";
		}

		for (int i = 0; i < modelList.count(); i++) {
			RModel *model = modelList[i];

			// Getting a list of random variables that are input to this model
			QList<RRandomVariable *> modelRVList = model->getRandomVariableList();

			// Getting a list of output responses of the model
			QList<RResponse *> modelResponseList = model->findChildren<RResponse *>();

			// Iterating on all responses of this model
			for (int j = 0; j < modelResponseList.count(); j++) {
				RResponse *response = modelResponseList[j];

				// Getting the gradient of the response with respect to all random variables of the theLimitStateFunction
				QMap<RParameter *, double> gradientMap = response->getGradientMap();

				// Declaring the stdvs and their sensitivities
				double totalStdv = 0.0;
				double dBetadTotalStdv = 0.0;
				double totalEpistemicStdv = 0.0;
				double dBetadTotalEpistemicStdv = 0.0;
				double modelStdv = 0.0;
				double dBetadModelStdv = 0.0;
				double modelEpistemicStdv = 0.0;
				double dBetadModelEpistemicStdv = 0.0;




				/*// TEST ON JUNE 10, 2012
				QString s = response->objectName();
				bool printData = false;
				if ((s == QString("B306LossModelResponse")) || (s == QString("B592LossModelResponse")) || (s == QString("B474LossModelResponse")) || (s == QString("B590LossModelResponse")) || (s == QString("B605LossModelResponse")) || (s == QString("B520LossModelResponse")) || (s == QString("B171LossModelResponse")) || (s == QString("B103LossModelResponse")) || (s == QString("B327LossModelResponse")) || (s == QString("B591LossModelResponse"))) {
					printData = true;
					rDebug() << "";
					rDebug() << qPrintable(s) << response->getCurrentValue();
					rDebug() << "RV" << "dBetadSigmai" << "dModelEpistemicStdvdSigmai" << "dResponsedParameteri" << "sigmai";
				}*/




				
				// Iterating on the gradientMap
				QMapIterator<RParameter *, double> ite(gradientMap);
				while (ite.hasNext()) {
					ite.next();
					RParameter *parameteri = ite.key();
					double dResponsedParameteri = ite.value();
						

					/*//### for debugging with finite difference
					//rDebug() << "I: d" << response->objectName() << "d" << parameteri->objectName() << "=" << dResponsedParameteri;*/


					RContinuousRandomVariable *cRVi = qobject_cast<RContinuousRandomVariable *>(parameteri);
					if (!cRVi) {
						rCritical() << "Error in model response standard deviation sensitivity analysis:" << parameteri->objectName() << "is not a random continuous random variable.";
						return -1;
					}
					int indexi = randomVariableList.indexOf(cRVi);
					double sigmai = cRVi->getStandardDeviationValue(0);
					bool iIsInputToThisModel = modelRVList.contains(cRVi);

					// Assign the diagonal related component of
					double stdvIncrementii = dResponsedParameteri * dResponsedParameteri * sigmai * sigmai;

					// Declare the dStdvdSigmai related variables
					double dTotalStdvdSigmai = 0.0;
					double dTotalEpistemicStdvdSigmai = 0.0;
					double dModelStdvdSigmai = 0.0;
					double dModelEpistemicStdvdSigmai = 0.0;

					// Declare the derivative of the response standard deviation with respect to the standard deviation of random variables and assing the value corresponding to the diagonal element
					double dStdvdSigmaiIncrementii = dResponsedParameteri * dResponsedParameteri * sigmai;
					
					totalStdv += stdvIncrementii;
					dTotalStdvdSigmai += dStdvdSigmaiIncrementii;
					if (iIsInputToThisModel) {
						modelStdv += stdvIncrementii;
						dModelStdvdSigmai += dStdvdSigmaiIncrementii;
					}
					if (cRVi->getUncertaintyType() == RContinuousRandomVariable::Epistemic) {
						totalEpistemicStdv += stdvIncrementii;
						dTotalEpistemicStdvdSigmai += dStdvdSigmaiIncrementii;
						if (iIsInputToThisModel) {
							modelEpistemicStdv += stdvIncrementii;
							dModelEpistemicStdvdSigmai += dStdvdSigmaiIncrementii;
						}
					}


					// Iterating on the gradientMap
					//QMap<RParameter *, double>::const_iterator jte = gradientMap.find(parameteri);
					//++jte;
					//while (jte != gradientMap.constEnd()) {
					QMap<RParameter *, double> gradientMap1 = gradientMap;
					QMapIterator<RParameter *, double> jte(gradientMap1);
					while (jte.hasNext()) {
						jte.next();
						RParameter *parameterj = jte.key();
						if (parameterj == parameteri) {
							continue;     // Because we have already accounted for the diagonal elements
						}
						RContinuousRandomVariable *cRVj = qobject_cast<RContinuousRandomVariable *>(parameterj);
						if (!cRVj) {
							rCritical() << "Error in model response standard deviation sensitivity analysis:" << parameterj->objectName() << "is not a random continuous random variable.";
							//return -1;
						}
						int indexj = randomVariableList.indexOf(cRVj);
						bool jIsInputToThisModel = modelRVList.contains(cRVj);

						double rhoij = 0.0;
						if (R) {
							rhoij = gsl_matrix_get(R, indexi, indexj);
						}
						if (rhoij != 0.0) {
							double dResponsedParameterj = jte.value();
							double sigmaj = cRVj->getStandardDeviationValue(0);

							/*//### for debugging with finite difference
							//rDebug() << "J: d" << response->objectName() << "d" << parameterj->objectName() << "=" << dResponsedParameterj;*/

							// Assing the off-diagonal components
							double stdvIncrementij = dResponsedParameteri * dResponsedParameterj * rhoij * sigmai * sigmaj;
							
							// Aassing the value corresponding to the off-diagonal elements
							double dStdvdSigmaiIncrementij = dResponsedParameteri * dResponsedParameterj * sigmaj * rhoij;

							totalStdv += stdvIncrementij;
							dTotalStdvdSigmai += dStdvdSigmaiIncrementij;
							if (iIsInputToThisModel && jIsInputToThisModel) {
								modelStdv += stdvIncrementij;
								dModelStdvdSigmai += dStdvdSigmaiIncrementij;
							}
							if (cRVi->getUncertaintyType() == RContinuousRandomVariable::Epistemic && cRVj->getUncertaintyType() == RContinuousRandomVariable::Epistemic) {
								totalEpistemicStdv += stdvIncrementij;
								dTotalEpistemicStdvdSigmai += dStdvdSigmaiIncrementij;
								if (iIsInputToThisModel && jIsInputToThisModel) {
									modelEpistemicStdv += stdvIncrementij;
									dModelEpistemicStdvdSigmai += dStdvdSigmaiIncrementij;
								}
							}
						}
						//++jte;
					}
					
					// Increment the sensitivities
					double dBetadSigmai = gsl_vector_get(betaSensitivityWrtStdvVector, indexi);

					


					
					/*// TEST ON JUNE 10, 2012
					if (printData && iIsInputToThisModel) {
						rDebug() << qPrintable(parameteri->objectName()) << dBetadSigmai << dModelEpistemicStdvdSigmai << dResponsedParameteri << sigmai;
					}*/





					/*////////////////////////////////////////////////////////
					// TEMP Implementagtion: To see whether there is more uncertainty in earthquakes or buildings in the sensitivity paper.
					// The first 49 random variables belong to earthquake hazard
					if (!tempRVList.contains(cRVi)) {
						continue;
					}
					////////////////////////////////////////////////////////*/

					
					if (dTotalStdvdSigmai != 0.0) {
						dBetadTotalStdv += dBetadSigmai / dTotalStdvdSigmai;
					}
					if (dTotalEpistemicStdvdSigmai != 0.0) {
						dBetadTotalEpistemicStdv += dBetadSigmai / dTotalEpistemicStdvdSigmai;
					}
					if (dModelStdvdSigmai != 0.0) {
						dBetadModelStdv += dBetadSigmai / dModelStdvdSigmai;
					}
					if (dModelEpistemicStdvdSigmai != 0.0) {
						dBetadModelEpistemicStdv += dBetadSigmai / dModelEpistemicStdvdSigmai;
					}

					/*//### for debugging with finite difference
					rDebug() << response->objectName() << parameteri->objectName() << "dTotalStdvdSigmai" << dTotalStdvdSigmai;
					rDebug() << response->objectName() << parameteri->objectName() << "dTotalEpistemicStdvdSigmai" << dTotalEpistemicStdvdSigmai;
					rDebug() << response->objectName() << parameteri->objectName() << "dModelStdvdSigmai" << dModelStdvdSigmai;
					rDebug() << response->objectName() << parameteri->objectName() << "dModelEpistemicStdvdSigmai" << dModelEpistemicStdvdSigmai;*/
				}

				totalStdv = sqrt(totalStdv);
				totalEpistemicStdv = sqrt(totalEpistemicStdv);
				modelStdv = sqrt(modelStdv);
				modelEpistemicStdv = sqrt(modelEpistemicStdv);

				/*//### for debugging with finite difference
				rDebug() << response->objectName() << "totalStdv" << totalStdv;
				rDebug() << response->objectName() << "totalEpistemicStdv" << totalEpistemicStdv;
				rDebug() << response->objectName() << "modelStdv" << modelStdv;
				rDebug() << response->objectName() << "modelEpistemicStdv" << modelEpistemicStdv;*/
			
				if (totalStdv != 0.0) {
					dBetadTotalStdv *= totalStdv;
				}
				if (totalEpistemicStdv != 0.0) {
					dBetadTotalEpistemicStdv *= totalEpistemicStdv;
				}
				if (modelStdv != 0.0) {
					dBetadModelStdv *= modelStdv;
				}
				if (modelEpistemicStdv != 0.0) {
					dBetadModelEpistemicStdv *= modelEpistemicStdv;
				}

				// Assign the computed standard deviations and sensitivities
				response->setTotalStandardDeviation(totalStdv);
				response->setTotalEpistemicStandardDeviation(totalEpistemicStdv);
				response->setModelStandardDeviation(modelStdv);
				response->setModelEpistemicStandardDeviation(modelEpistemicStdv);

				response->setBetaSensitivityWrtTotalStandardDeviation(dBetadTotalStdv);
				response->setBetaSensitivityWrtTotalEpistemicStandardDeviation(dBetadTotalEpistemicStdv);
				response->setBetaSensitivityWrtModelStandardDeviation(dBetadModelStdv);
				response->setBetaSensitivityWrtModelEpistemicStandardDeviation(dBetadModelEpistemicStdv);


				if (thePrintSensitivities) {
					rDebug() << qPrintable(QString("%1\t%2\t%3\t%4\t%5\t%6\t%7\t%8\t%9").arg(response->objectName()).arg(totalStdv, 0, 'g', 14).arg(totalEpistemicStdv, 0, 'g', 14).arg(modelStdv, 0, 'g', 14).arg(modelEpistemicStdv, 0, 'g', 14).arg(dBetadTotalStdv, 0, 'g', 14).arg(dBetadTotalEpistemicStdv, 0, 'g', 14).arg(dBetadModelStdv, 0, 'g', 14).arg(dBetadModelEpistemicStdv, 0, 'g', 14));
				}
			}
		}
		if (R) {
			gsl_matrix_free(R);
			R = 0;
		}
			
		// Restoring the gradient analysis type
		theLimitStateFunction.data()->setGradientAnalysisType(gradientTypeBackup);

	}

	// ATTENTION: DECISION VARIABLE SENSITIVITY MUST BE DONE AS THE LAST TASK, BECAUSE WEHN EVALUATING THE GRADIENT WRT. DECISION VARIABLES USING DDM, THE GRADIENT MAPS FOR RANDOM VARIABLES ARE ERASED.
	// Compute the decision variables sensitivities: d(Beta)/d(Ci) (Ci is the incremental cost of the decision variable i)
	if (theComputeDecisionVariableSensitivities) {
		QList<RDecisionVariable *> decisionVariableList = theLimitStateFunction.data()->getDecisionVariableList();
		if (!decisionVariableList.isEmpty()) {
			if (theLimitStateFunction.data()->getGradientAnalysisType() == RFunction::DDM) {
				theLimitStateFunction.data()->clearDDMMaps();
				theLimitStateFunction.data()->updateAnalysisStatus(0);
				result = theLimitStateFunction.data()->evaluateFunction(true, RObject::DecisionVariable,0);
			}
			result = theLimitStateFunction.data()->evaluateGradient(RObject::DecisionVariable);
			if (result < 0) {
				rCritical() << "Error: Could not evaluate the sensitivities for decision variables.";
				return 0;
			}
			gsl_vector *dvGradientVector = theLimitStateFunction.data()->getGradient();
			//double dvGradientVectorNorm = gsl_blas_dnrm2(dvGradientVector);
			gsl_vector *gradientInStandardNormalSpace = theNonlinearSingleConstraintSolver.data()->getGradientInStandardNormalSpace();
			double gradientInStandardNormalSpaceNorm = gsl_blas_dnrm2(gradientInStandardNormalSpace);

			double betaPDF = gsl_ran_gaussian_pdf(beta, 1.0);

			if (thePrintSensitivities && !decisionVariableList.isEmpty()) {
				rDebug() << " ";
				rDebug() << " ";
				rDebug() << "Decision variable sensitivities:";
				rDebug() << " ";
				rDebug() << "DecisionVariableObjectName\tBetaSensitivityWrtCost\tPfSensitivityWrtCost";
			}

			for (int i = 0; i < decisionVariableList.count(); i++) {
				RContinuousDecisionVariable *cDV = qobject_cast<RContinuousDecisionVariable *>(decisionVariableList[i]);
				if (cDV) {
					double dC_dDV = cDV->getIncrementalCostValue(0);
					double dBeta_dC = 1 / gradientInStandardNormalSpaceNorm * gsl_vector_get(dvGradientVector, i) / dC_dDV;
					double dPf_dC = -dBeta_dC * betaPDF;
					cDV->setBetaSensitivityWrtCost(dBeta_dC);
					cDV->setPfSensitivityWrtCost(dPf_dC);

					if (thePrintSensitivities) {
						rDebug() << qPrintable(QString("%1\t%2\t%3").arg(cDV->objectName()).arg(dBeta_dC, 0, 'g', 14).arg(dPf_dC, 0, 'g', 14));
					}
				}
			}
		}
	}


	rDebug() << "";
	rDebug() << "The reliability index:";
	rDebug() << "Beta =" << beta;
	rDebug() << "";
	rDebug() << "The probability of failure:";
	rDebug() << "pf =" << pf;
	rDebug() << "";
	rDebug() << "";

	
	if (theCorrectProbabilityWithFirstPrincipalCurvature) {
		rDebug() << "The last two trial points in the search for the design point in FORM are employed to compute the first principal curvature of the limit-state function.";
		rDebug() << "The first principal curvature:";
		rDebug() << "kappa =" << firstPrincipalCurvature;
		rDebug() << "";
		rDebug() << "Corrected probability of failure according to Breitung's improved fromula:";
		rDebug() << "pf =" << pfSORM;
		rDebug() << "";
		rDebug() << "Corresponding reliability index:";
		rDebug() << "Beta =" << betaSORM;
		rDebug() << "";
	}

	QDateTime finishTime = QDateTime::currentDateTime();
	double numDays = startTime.daysTo(finishTime);
	double numMilliSecs = startTime.time().msecsTo(finishTime.time());
	double numSecs = numDays * 86400.0 + numMilliSecs / 1000.0;

	rDebug() << "##### FORM ANALYSIS DONE IN" << numSecs << "SECONDS.";
	rDebug() << "";
	rDebug() << "";




	return 0;
}


gsl_vector * RFORMAnalyzer::getdBetadMeanVector() const
{
	return betaSensitivityWrtMeanVector;
}

gsl_vector * RFORMAnalyzer::getdBetadStdvVector() const
{
	return betaSensitivityWrtStdvVector;
}

gsl_vector * RFORMAnalyzer::getdKappadMeanVector() const
{
	return kappaSensitivityWrtMeanVector;
}

gsl_vector * RFORMAnalyzer::getdKappadStdvVector() const
{
	return kappaSensitivityWrtStdvVector;
}

bool RFORMAnalyzer::canBeRunParallelly() {
	return false;
}