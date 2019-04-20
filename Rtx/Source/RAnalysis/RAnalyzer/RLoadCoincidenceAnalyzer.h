// Last modified by Mojtaba on June 29, 2012

#ifndef RLoadCoincidenceAnalyzer_H
#define RLoadCoincidenceAnalyzer_H

#include "RAnalyzer.h"

#include <gsl/gsl_vector.h>

class RFunction;
class RRandomNumberGenerator;
class RTransformer;
class RAccumulator;
class RFORMAnalyzer;
class RSamplingAnalyzer;
class RModel;
class ROccurrenceModel;

class RLoadCoincidenceAnalyzer : public RAnalyzer
{
	Q_OBJECT

	// Sets the limit-state function
	Q_PROPERTY(QObject *LimitStateFunction READ getLimitStateFunction WRITE setLimitStateFunction)

	// Indicates the analysis type to obtain the failure probability due to each hazard and hazard combination; current options are FORM, Sampling, and ImportanceSampling
	Q_PROPERTY(RAnalysisType AnalysisType READ getAnalysisType WRITE setAnalysisType)

	// Sets the RFORMAnalyzer object, which is used to run FORM analysis
	Q_PROPERTY(QObject *FORMAnalyzer READ getFORMAnalyzer WRITE setFORMAnalyzer)

	// Sets the RSamplingAnalyzer object, which is used to run sampling analysis
	Q_PROPERTY(QObject *SamplingAnalyzer READ getSamplingAnalyzer WRITE setSamplingAnalyzer)

	// Sets a list of the occurrence models for hazards that are considered in this analysis
	Q_PROPERTY(QString OccurrenceModelList READ getOccurrenceModelList WRITE setOccurrenceModelList)

	// Sets the maximum level of coincidence for hazards, e.g., 1 means that only individual hazards are analyzed and no coincidence in considered, and 2 means that individual hazards as well as the coincidence of 2 hazards are analyzed but not the coincidence of 3 hazards and above
	Q_PROPERTY(int MaxNumberOfCoincidentHazards READ getMaxNumberOfCoincidentHazards WRITE setMaxNumberOfCoincidentHazards)

	// Sets the time period under consideration in the unit of "Days"; default is 18262 days (=50 years)
	Q_PROPERTY(double Timespan READ getTimespan WRITE setTimespan)

	// Indicates whether or not to compute rv sensitivities, i.e., eta and delta vectors
	Q_PROPERTY(bool ComputeRandomVariableSensitivities READ getComputeRandomVariableSensitivities WRITE setComputeRandomVariableSensitivities)

	// Indicates whether or not to compute dv sensitivities
	Q_PROPERTY(bool ComputeDecisionVariableSensitivities READ getComputeDecisionVariableSensitivities WRITE setComputeDecisionVariableSensitivities)

	// Indicates whether or not to compute the sensitivity wrt. the standard deviation of model responses
	Q_PROPERTY(bool ComputeModelResponseStandardDeviationSensitivities READ getComputeModelResponseStandardDeviationSensitivities WRITE setComputeModelResponseStandardDeviationSensitivities)

	// Indicates whether or not to print the sensitivities in the output pane (in any case, they are stored in the read-only meta-properties of rvs, dvs, and responses)
	Q_PROPERTY(bool PrintSensitivities READ getPrintSensitivities WRITE setPrintSensitivities)

	// Indicates whether or not to correct the failure probability from FORM with the first principal curvature (similar to SORM)
	Q_PROPERTY(bool CorrectProbabilityWithFirstPrincipalCurvature READ getCorrectProbabilityWithFirstPrincipalCurvature WRITE setCorrectProbabilityWithFirstPrincipalCurvature)

	// Sets the text file that is read to get the starting realization of rvs for FORM analyses due to all hazards and hazard combinations; every row in the file contains as many realizations as the number of hazards and hazard combinations
	Q_PROPERTY(QString StartPointFile READ getStartPointFile WRITE setStartPointFile)

	// Sets the text file in which the design points from FORM analyses due to all hazards and hazard combinations are stored; every row in the file contains as many design point realizations as the number of hazards and hazard combinations
	Q_PROPERTY(QString DesignPointFile READ getDesignPointFile WRITE setDesignPointFile)
	
	// Enum for the type of the analysis; can be FORM, Sampling, or ImportanceSampling
	Q_ENUMS(RAnalysisType)

public:
	RLoadCoincidenceAnalyzer(QObject *parent, QString name);
	~RLoadCoincidenceAnalyzer();

	// Declaring the RAnalysisType enum values
	enum RAnalysisType{FORM, Sampling, ImportanceSampling};

	// getter/setter methods for meta-properties
	QObject *getLimitStateFunction() const;
	void setLimitStateFunction(QObject *value);
	RAnalysisType getAnalysisType();
	void setAnalysisType(RAnalysisType value);
	QObject *getFORMAnalyzer() const;
	void setFORMAnalyzer(QObject *value);
	QObject *getSamplingAnalyzer() const;
	void setSamplingAnalyzer(QObject *value);
	QString getOccurrenceModelList();
	void setOccurrenceModelList(QString value);
	int getMaxNumberOfCoincidentHazards() const;
	void setMaxNumberOfCoincidentHazards(int value);
	double getTimespan() const;
	void setTimespan(double value);
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
	QString getStartPointFile();
	void setStartPointFile(QString value);
	QString getDesignPointFile();
	void setDesignPointFile(QString value);

	int analyze();

	bool canBeRunParallelly();
private slots:
	// Slot that is trigerred when a sub-window is added to the main window; when called, the title of the plot sub-windows is modified to reflect the name of the hazard that is being analyzed
	void onSubWindowAdded(QWidget *subWindow);

private:
	// Data members for meta-properties
	RPointer<RFunction> theLimitStateFunction;
	RAnalysisType theAnalysisType;
	RPointer<RFORMAnalyzer> theFORMAnalyzer;
	RPointer<RSamplingAnalyzer> theSamplingAnalyzer;
	QList<RPointer<RModel> > theOccurrenceModelList;
	int theMaxNumberOfCoincidentHazards;
	double theTimespan;
	bool theComputeRandomVariableSensitivities;
	bool theComputeDecisionVariableSensitivities;
	bool theComputeModelResponseStandardDeviationSensitivities;
	bool thePrintSensitivities;
	bool theCorrectProbabilityWithFirstPrincipalCurvature;
	QString theStartPointFile;
	QString theDesignPointFile;

	// Auxiliary data member that stores the name of the hazard that is being analyzed
	QString theCurrentHazardNamesStr;
	
};

#endif // RLoadCoincidenceAnalyzer_H
