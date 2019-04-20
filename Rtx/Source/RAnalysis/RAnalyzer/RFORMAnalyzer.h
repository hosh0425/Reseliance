// Last modified by Mojtaba on June 29, 2012

#ifndef RFORMAnalyzer_H
#define RFORMAnalyzer_H

#include "RAnalyzer.h"

#include <gsl/gsl_vector.h>

class RFunction;
class RNonlinSingleConstrSolver;

class RFORMAnalyzer : public RAnalyzer
{
	Q_OBJECT

	// Sets the limit-state function
	Q_PROPERTY(QObject *LimitStateFunction READ getLimitStateFunction WRITE setLimitStateFunction)

	// Sets the "solver" for nonlinear single-constraint optimization problem in FORM
	Q_PROPERTY(QObject *NonlinearSingleConstraintSolver READ getNonlinearSingleConstraintSolver WRITE setNonlinearSingleConstraintSolver)

	// Indicates whether or not to compute rv sensitivities, i.e., eta and delta vectors
	Q_PROPERTY(bool ComputeRandomVariableSensitivities READ getComputeRandomVariableSensitivities WRITE setComputeRandomVariableSensitivities)

	// Indicates whether or not to compute dv sensitivities
	Q_PROPERTY(bool ComputeDecisionVariableSensitivities READ getComputeDecisionVariableSensitivities WRITE setComputeDecisionVariableSensitivities)

	// Indicates whether or not to compute the sensitivity wrt. the standard deviation of model responses
	Q_PROPERTY(bool ComputeModelResponseStandardDeviationSensitivities READ getComputeModelResponseStandardDeviationSensitivities WRITE setComputeModelResponseStandardDeviationSensitivities)

	// Indicates whether or not to print the sensitivities in the output pane (in any case, they are stored in the read-only meta-properties of rvs, dvs, and responses)
	Q_PROPERTY(bool PrintSensitivities READ getPrintSensitivities WRITE setPrintSensitivities)

	// Indicates whether or not to correct the failure probability with the first principal curvature (similar to SORM)
	Q_PROPERTY(bool CorrectProbabilityWithFirstPrincipalCurvature READ getCorrectProbabilityWithFirstPrincipalCurvature WRITE setCorrectProbabilityWithFirstPrincipalCurvature)

public:
	RFORMAnalyzer(QObject *parent, QString name);
	~RFORMAnalyzer();

	// getter/setter methods for meta-properties
	QObject *getLimitStateFunction() const;
	void setLimitStateFunction(QObject *value);
	QObject *getNonlinearSingleConstraintSolver() const;
	void setNonlinearSingleConstraintSolver(QObject *value);
	bool getComputeRandomVariableSensitivities() const;
	void setComputeRandomVariableSensitivities(bool value);
	bool getComputeDecisionVariableSensitivities() const;
	void setComputeDecisionVariableSensitivities(bool value);
	bool getComputeModelResponseStandardDeviationSensitivities() const;
	void setComputeModelResponseStandardDeviationSensitivities(bool value);
	bool getPrintSensitivities() const;
	void setPrintSensitivities(bool value);
	bool getCorrectProbabilityWithFirstPrincipalCurvature() const;
	void setCorrectProbabilityWithFirstPrincipalCurvature(bool value);
	bool canBeRunParallelly();

	
	// Returns the derivative of the reliability index wrt. mean of rvs
	gsl_vector *getdBetadMeanVector() const;

	// Returns the derivative of the reliability index wrt. stdv of rvs
	gsl_vector *getdBetadStdvVector() const;

	// Returns the derivative of the first principal curvature wrt. mean of rvs (Development of this derivative is in the experimental phase)
	gsl_vector *getdKappadMeanVector() const;

	// Returns the derivative of the first principal curvature wrt. stdv of rvs (Development of this derivative is in the experimental phase)
	gsl_vector *getdKappadStdvVector() const;

	// Runs the analysis
	int analyze();

	// An over-loaded method to run the analysis with the option of not parsing (tokenizing) the limit-state function, assuming that it has already been parsed
	// This is only called from within the load coincidence analysis in which the function need not be re-parsed for every FORM analysis for diffrenet hazards
	int analyze(bool populateListsInFunction);

private:
	// Data members for meta-properties
	RPointer<RFunction> theLimitStateFunction;
	RPointer<RNonlinSingleConstrSolver> theNonlinearSingleConstraintSolver;
	bool theComputeRandomVariableSensitivities;
	bool theComputeDecisionVariableSensitivities;
	bool theComputeModelResponseStandardDeviationSensitivities;
	bool thePrintSensitivities;
	bool theCorrectProbabilityWithFirstPrincipalCurvature;

	// Data members for sensitivity vectors
	gsl_vector *betaSensitivityWrtMeanVector;
	gsl_vector *betaSensitivityWrtStdvVector;
	gsl_vector *kappaSensitivityWrtMeanVector;
	gsl_vector *kappaSensitivityWrtStdvVector;
	
};

#endif // RFORMAnalyzer_H
