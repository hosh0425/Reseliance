// Last modified by Mojtaba on June 29, 2012

#ifndef RScenarioSamplingAnalyzer_H
#define RScenarioSamplingAnalyzer_H

#include "RAnalyzer.h"
#include "RParallelComputerInterface.h"
#include <gsl/gsl_vector.h>

#include "RPlotWidget.h"

class RFunction;
class RRandomNumberGenerator;
class RTransformer;
class RAccumulator;
class RModel;
class RTime;
class ROccurrenceModel;
class RRandomVariable;


class RRandomVariable;

class QMutex;
class QReadWriteLock;

class RParallelProcessor;


class RScenarioSamplingAnalyzer : public RAnalyzer, public RParallelComputerInterface
{
	Q_OBJECT

	// Sets the RRandomNumberGenerator object, which generates realizations for rvs
	Q_PROPERTY(QObject *RandomNumberGenerator READ getRandomNumberGenerator WRITE setRandomNumberGenerator USER false)

	// Sets the RTransformer object, which transforms points between original and standard normal spaces
	Q_PROPERTY(QObject *Transformer READ getTransformer WRITE setTransformer  USER false)

	// Sets the RAccumulator object, which accumulates the outcome of samples
	Q_PROPERTY(QObject *Accumulator READ getAccumulator WRITE setAccumulator  USER false)

	// Sets the list of functions subject to sampling analysis
	Q_PROPERTY(QString FunctionList READ getFunctionList WRITE setFunctionList  USER false)

	// Sets the flags that indicate how to accumulate the function values during a T-year scenario (sample): 0 means add function values together, e.g., for loss; 1 means get the maximum over T years, e.g., for drift, 2 means get the minimum over T years, e.g., for drift limit-states, 3 means keep the last function value, e.g., for permanent deformation
	Q_PROPERTY(QString FunctionAccumulationTypeList READ getFunctionAccumulationTypeList WRITE setFunctionAccumulationTypeList  USER false)

	// Sets the Threshold value, which turns a function into a limit-state function, i.e., the limit-state function will be equal to threshold minus the origina function; this is useful when doing a "Failure Probability" sampling on a response that should be acumulated over time, such as economic loss
	Q_PROPERTY(QString ThresholdList READ getThresholdList WRITE setThresholdList  USER false)

	// Sets a list of the occurrence models for hazards that are considered in this analysis
	Q_PROPERTY(QString OccurrenceModelList READ getOccurrenceModelList WRITE setOccurrenceModelList  USER false)

	// Determines if the plots should be shown
	Q_PROPERTY(bool DisplayDiagrams READ getDisplayDiagrams WRITE setDisplayDiagrams USER true)

	// Sets the time period under consideration in the unit of "Days"; default is 18262 days (=50 years)
	Q_PROPERTY(double Timespan READ getTimespan WRITE setTimespan  USER false)

	// Sets the Time object, which will contains the current time during T and is passed to time-varying models, such as the discounting model
	Q_PROPERTY(QObject *TimeParameter READ getTimeParameter WRITE setTimeParameter  USER false)

	// Sets if the sensitvity wrt to mean and stdv of random variables should be done or not
	Q_PROPERTY(bool ComputeRandomVariableSensitivities READ getComputeRandomVariableSensitivities WRITE setComputeRandomVariableSensitivities)
	
	// Sets if the correlations of sensitivity random variables should be neglected or not
	Q_PROPERTY(bool NeglectCorrelationInSensitivity READ getNeglectCorrelationInSensitivity WRITE setNeglectCorrelationInSensitivity)
	
	// Sets the list of random variables whose sensitvity is sought (can be empty when sensitivity analysis is not done)
	Q_PROPERTY(QString SensitivityRandomVariableList READ getSensitivityRandomVariableList WRITE setSensitivityRandomVariableList)

	// Sets the flags that indicate for which function the sensitvity analysis should be done (can be empty when sensitivity analysis is not done)
	Q_PROPERTY(QString SensitivityFunctionList READ getSensitivityFunctionList WRITE setSensitivityFunctionList)


public:
	RScenarioSamplingAnalyzer(QObject *parent, QString name);
	~RScenarioSamplingAnalyzer();

	// getter/setter methods for meta-properties
	QObject *getRandomNumberGenerator() const;
	void setRandomNumberGenerator(QObject *value);
	QObject *getTransformer() const;
	void setTransformer(QObject *value);
	QObject *getAccumulator() const;
	void setAccumulator(QObject *value);
	QString getFunctionList();
	void setFunctionList(QString value);
	QString getFunctionAccumulationTypeList();
	void setFunctionAccumulationTypeList(QString value);
	QString getThresholdList();
	void setThresholdList(QString value);
	QString getOccurrenceModelList();
	void setOccurrenceModelList(QString value);
	bool getDisplayDiagrams();
	void setDisplayDiagrams(bool value);
	double getTimespan() const;
	void setTimespan(double value);
	QObject *getTimeParameter() const;
	void setTimeParameter(QObject *value);

	QString getSensitivityRandomVariableList();
	void setSensitivityRandomVariableList(QString value);
	bool getComputeRandomVariableSensitivities();
	void setComputeRandomVariableSensitivities(bool vale);
	bool getNeglectCorrelationInSensitivity();
	void setNeglectCorrelationInSensitivity(bool vale);
	QString getSensitivityFunctionList();
	void setSensitivityFunctionList(QString value);


	// Runs the analysis
	int analyze();

	// Initializes some of the contributed objects for parallel computation
	void initializeForParallelComputation(int threadID);

	// Resets some of the contributed objects after parallel computation
	void resetAfterParallelComputation();

	// Returns true if all the important contributed objects can be run parallelly
	bool canBeRunParallelly();

private:

	// a wrapper around doAnalysis method to give to the parallel computer
	int static doAnalysisWrapper(void * theObject, void * inputsArgs[], int threadID);

	// the part of the analysis that is conducted in each thread. generalSampleCount is a reference to shared variable sample count which is going to be changed in all threads
	int doAnalysis(bool isFirstSimulation, int * generalSampleCount, int threadID = 0);

	// the part which is called when analysis in one thread is finished
	int endAnalysis(gsl_vector * x, gsl_vector * dfdMean_1_f, gsl_vector * dfdStdv_1_f, int errorCode);

	// Data members for meta-properties
	RPointer<RRandomNumberGenerator> theRandomNumberGenerator;
	RPointer<RTransformer> theTransformer;
	RPointer<RAccumulator> theAccumulator;
	QList<RPointer<RFunction> > theFunctionList;
	QVector<int> theFunctionAccumulationTypeList;
	QVector<double> theThresholdList;
	QList<RPointer<RModel> > theOccurrenceModelList;
	double theTimespan;
	RPointer<RTime> theTimeParameter;

	// Data members that gets the propertirs for sensitivity analysis
	QList<RPointer<RRandomVariable> > theSensitivityRandomVariableList;
	bool theComputeRandomVariableSensitivities;
	bool theNeglectCorrelationInSensitivity;
	QList<RPointer<RFunction> > theSensitivityFunctionList;
	
	// auxiliary transformer for physical random variables
	RPointer<RTransformer> thePhysicalRandomVariablesTransformer;

	/* Data members that are needed for analysis and are generated in the starting part of analyze method */
	
	// list of occurence models given as input
	QList<ROccurrenceModel *> hazardModelList;	

	// a merged "set" of models affecting the functions
	QSet<RModel *> theModelSet;

	// a merged list of all random variables used in all of the passed functions
	QList<RRandomVariable *> theRandomVariableList;

	// a merged list of the random variables which are mentioned as physical parameters, used in all of the passed functions
	QList<RRandomVariable *> physicalRandomVariableList;

	// the number of all random variables affecting all functions
	int randomVariablesCount;

	// the number of physical parameter random variables affecting all functions
	int physicalRandomVariablesCount;

	// list of the "unused" hazard occurrence models in the domain 
	QList<ROccurrenceModel *> unusedOccurrenceModelList;

	// lists of the models asssociated with unusedOccurrenceModelList
	QList<RModel *> unusedAssociatedModelList;
	QList<RModel *> unusedMagnitudeModelList;

	// the time span
	double T;

	// the time corresponding to the start of the time span
	double initialTime;

	// Plotting
	QPointer<RPlotWidget> theTimePlot;

#ifndef NO_QCP
	QList<QCPGraph *> theCurveList;
	QCPGraph *currentTimeCurve;
#endif // NO_QCP


	// Data members for meta-properties
	bool theDisplayDiagrams;

	// locks for meta properties' getter and setter
	QReadWriteLock * theMaximumIterationsLock;
	QReadWriteLock * thePlottingIntervalLock;
	QReadWriteLock * theDisplayDiagramsLock;

	double timePlotHeight;

	// a lock for creating safe area for increasing sample count in different threads
	QMutex * sampleCountLock;

	// a reference to parallelComputer class to handle most of the stuff about parallelism
	RParallelProcessor * analysisParallelComputer;
};

#endif // RScenarioSamplingAnalyzer_H
