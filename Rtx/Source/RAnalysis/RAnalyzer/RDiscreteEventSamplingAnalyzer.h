//Developed by Hossein Nasr

#ifndef RDiscreteEventSamplingAnalyzer_H
#define RDiscreteEventSamplingAnalyzer_H

#include "RAnalyzer.h"
#include "RParallelComputerInterface.h"
#include <gsl/gsl_vector.h>
#include "RPlotWidget.h"

class QwtPlot;
class QwtPlotCurve;

class RFunction;
class RRandomNumberGenerator;
class RTransformer;
class RAccumulator;
class RHistogramAccumulator;
class RModel;
class RTime;
class ROccurrenceModel;
class REventModel;
class RRandomVariable;
class RResponse;

class QMutex;
class QReadWriteLock;

class RParallelProcessor;

class RDiscreteEventSamplingAnalyzer : public RAnalyzer, public RParallelComputerInterface
{
	Q_OBJECT

		// Controls the amount of output that is shown in the output pane; options are None, Minimum, Moderate, and Maximum
		Q_PROPERTY(ROutputDisplayLevelType OutputDisplayLevel READ getOutputDisplayLevel WRITE setOutputDisplayLevel USER true)

		// Determines if the plots should be shown
		Q_PROPERTY(bool DisplayDiagrams READ getDisplayDiagrams WRITE setDisplayDiagrams)

		// Sets the cost estimation interval in number of days
		Q_PROPERTY(double FunctionEstimationInterval READ getFunctionEstimationInterval WRITE setFunctionEstimationInterval)

		// Sets the RRandomNumberGenerator object, which generates realizations for rvs
		Q_PROPERTY(QObject *RandomNumberGenerator READ getRandomNumberGenerator WRITE setRandomNumberGenerator)

		// Sets the RTransformer object, which transforms points between original and standard normal spaces
		Q_PROPERTY(QObject *Transformer READ getTransformer WRITE setTransformer)

		// Sets the RAccumulator object, which accumulates the outcome of samples
		Q_PROPERTY(QObject *Accumulator READ getAccumulator WRITE setAccumulator)

		// Sets the list of functions subject to sampling analysis
		Q_PROPERTY(QString FunctionList READ getFunctionList WRITE setFunctionList)

		// Sets the flags that indicate how to create output PDF and CDF files: 0 means no file; 1 means to create PDF and CDF files for incremental values of function  during recovery, 2 means to create PDF and CDF files for cumulative values of function during recovery,  3 means to create PDF and CDF files for both incremental values and cumulative values of function during recovery
		Q_PROPERTY(QString FunctionAccumulationTypeList READ getFunctionAccumulationTypeList WRITE setFunctionAccumulationTypeList)

		// Sets a list of the occurrence models for hazards that are considered in this analysis
		Q_PROPERTY(QString OccurrenceModelList READ getOccurrenceModelList WRITE setOccurrenceModelList)

		// Sets a list of the event sheduler models that are considered in this analysis
		Q_PROPERTY(QString EventModelList READ getEventModelList WRITE setEventModelList)

		//// Sets the time period under consideration in the unit of "Days"; default is 18262 days (=50 years)
		//Q_PROPERTY(double Timespan READ getTimespan WRITE setTimespan)

		// Sets the Time object, which will contains the current time during T and is passed to time-varying models, such as the discounting model
		Q_PROPERTY(QObject *TimeParameter READ getTimeParameter WRITE setTimeParameter)

public:
	RDiscreteEventSamplingAnalyzer(QObject *parent, QString name);
	~RDiscreteEventSamplingAnalyzer();

	bool getDisplayDiagrams();
	void setDisplayDiagrams(bool value);

	double getFunctionEstimationInterval() const;
	void setFunctionEstimationInterval(double value);

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

	//QString getThresholdList();
	//void setThresholdList(QString value);

	QString getOccurrenceModelList();
	void setOccurrenceModelList(QString value);

	QString getEventModelList();
	void setEventModelList(QString value);

	//double getTimespan() const;
	//void setTimespan(double value);

	QObject *getTimeParameter() const;
	void setTimeParameter(QObject *value);

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
	int doAnalysis(/*bool isFirstSimulation,*/ int * generalSampleCount, int threadID = 0);

	// the part which is called when analysis in one thread is finished
	int endAnalysis(gsl_vector * x, int errorCode);

	// a lock for creating safe area for increasing sample count in different threads
	QMutex * sampleCountLock;
	QMutex * theIterationIndexLock;
	QMutex * plottingIsEnabledLock;
	QMutex * theContinueFlagLock;
	QMutex * theMaxTimeSizeLock;
	QMutex * accumulatorLock;
	QMutex * modifyTimeSizeLock;
	QMutex * updateSamplePlotLock;
	QMutex * evaluateDependencyLock;


	// Boolean variable that allows to run the inside accumulator
	bool doAccumulate;

	// Boolean variable that is true if the analysis is multi threaded
	bool multiThreaded;

	// a reference to parallelComputer class to handle most of the stuff about parallelism
	RParallelProcessor * analysisParallelComputer;

	// Data members for meta-properties
	bool theDisplayDiagrams;
	double theFunctionEstimationInterval;
	RPointer<RRandomNumberGenerator> theRandomNumberGenerator;
	RPointer<RTransformer> theTransformer;
	RPointer<RAccumulator> theAccumulator;
	RHistogramAccumulator *theHistogramAccumulator;
	QList<RPointer<RFunction> > theFunctionList;
	QVector<int> theFunctionAccumulationTypeList;
	QList<RPointer<RModel> > theOccurrenceModelList;
	QList<RPointer<RModel> > theEventModelList;
	RPointer<RTime> theTimeParameter;


	// List of random variables
	QList<RRandomVariable *> theAllRandomVariableList;
	QList<double> backupAllRandomVariablesValueList;
	int allRandomVariableCount;


	//// auxiliary transformer for physical random variables
	//RPointer<RTransformer> thePhysicalRandomVariablesTransformer;
	//QList<RRandomVariable *> physicalRandomVariableList;
	//int physicalRandomVariableCount;

	//// List of all parameters that all of the functions contain
	//QList<RParameter *> theAllParameterList;
	//QList<double> allParameterBackupList;

	// List of all response parameters that all functions contain
	QSet<RResponse *> allFunctionResponseSet;

	//Aux List of occurrence models- All and the ones which are not used
	QList<ROccurrenceModel *> hazardModelList;
	QList<ROccurrenceModel *> unusedOccurrenceModelList;
	QList<RModel *> unusedAssociatedModelList;
	QList<RModel *> unusedMagnitudeModelList;

	//List of normalizied rate of occurrence of all hazards
	QList<double> hazardNormalizedOccurenceRateList;
	QList<double> hazardOccurenceRateList;
	

	//Aux List of Event models- All and the ones which are not used
	QList<REventModel *> eventModelList;
	QList<REventModel *> unusedEventModelList;

	//double theTimeSpan;


	//A function that has all of the responses that affect theFunctionList
	RFunction *allInitialStateFunction;

	//// Data members for event map
	//QMap <double, QVariantList> theAllEventMap;
	//QMap <double, QVariantList> theNewEventMap;

	//a vector that stores fixed interval values for the maximum recovery time
	QVector<double> maxFixedIntervalVector;
	double initialTime;


	// Data members for parameter and model set
	//this list contains all of the models that all of fuctions contain
	//QList<RModel *> theInitialModelList;
	QList<RModel *> theAllModelList;

	// Backup of the list of models that affect all fuctions contain
	QList<RModel *> backupAllModelList;

	// Data member that relates each parameter with a list of models it affects
	QHash<RParameter *, QList<RModel *> > theDependencyHash;


	// Establishes a list of models that are affected by the passedParameter
	void evaluateDependencies(RParameter *passedParameter);

	// Updates the isAnalyzed flag of responses of models that are affected by the passedParameter
	void updateAnalysisStatus(RParameter *passedParameter, int threadID);

	// Overloaded method that updates the isAnalyzed flag of responses of models that are affected by the passedParameterList
	void updateAnalysisStatus(QList<RPointer<RParameter> > passedParameterList, int threadID);

	//// Auxiliary method to update the plots
	//void updatePlot(int functionIndex);

	// Auxiliary method to save the PDf in a text Files
	void savePDFtoTextFile(int functionIndex, bool incremental);

	// Auxiliary method to save the CDf in a text Files
	void saveCDFtoTextFile(int functionIndex, bool incremental);

	// Auxiliary method to compute the vector of CDFs and CoVs
	void computeCDFandCoV(QList<double> *CDFList, QList<double> *CoVList);

	// Auxiliary method to evaluate the histogram
	void evaluateHistogram(int functionIndex, int timeIndex, bool incremental, QVector<double> *xVector, QVector<double> *PDFVector);

	// Sets the list of functions
	void initializeAccumulator();

	// Modify the vector size according to the sample recovery time
	void modifyTimeSize(QVector<double> sampleFixedIntervalVector, QVector<QList<double> > *sampleResultFixedIntervalListVector, QVector<QList<double> > *sampleCumulativeResultFixedIntervalListVector);

	void terminateAccumulation();

	//Data members and methods for the inside accumulator
	long thePlottingInterval;
	bool plottingIsEnabled;
	void resetAccumulator();

	//The accumulaor that accumulates the time history values of functions through the recovery period
	int accumulate(QVector<QList<double> > functionIncrementalValueListVector, QVector<QList<double> > functionCumulativeValueListVector, int threadID);
	
	//Three dimensional lists that records the incremental and cumulative values for each sample[i] for each time interval[j] for each function [k]
	QVector<QVector<QList<double> > > theFunctionIncrementalValueListVectorVector;
	QVector<QVector<QList<double> > > theFunctionCumulativeValueListVectorVector;

	//data members that stores the mean and standard dev values of Incremental values for each function[i] for each time interval[j] 
	QList<QVector<double> > theFunctionIncrementalMeanValueVectorList;
	QList<QVector<double> > theFunctionIncrementalStdevValueVectorList;

	//data members that stores the mean values of Incremental values plus/minus one standard dev for each function[i] for each time interval[j] 
	QList<QVector<double> > theFunctionIncrementalPlusValueVectorList;
	QList<QVector<double> > theFunctionIncrementalMinusValueVectorList;

	//data members that stores the mean and standard dev values of cumulative values for for each function[i] for each time interval[j] 
	QList<QVector<double> > theFunctionCumulativeMeanValueVectorList;
	QList<QVector<double> > theFunctionCumulativeStdevValueVectorList;

	//data members that stores the mean values of cumulative values plus/minus one standard dev for each function[i] for each time interval[j] 
	QList<QVector<double> > theFunctionCumulativePlusValueVectorList;
	QList<QVector<double> > theFunctionCumulativeMinusValueVectorList;


	//General data members for accumulator
	int theIterationIndex;
	int theNumberOfBins;
	long theMaximumIterations;
	double theMaxTimeSize;
	bool theContinueFlag;

	// Auxiliary method to update the plots, this method is for the plots that show the three curves for mean and mean+sigma and mean-sigma values
	void updatePlot();

	// Auxiliary method to update the sample plots, this method is for the plots that show the function values at each estimation interval
	void updateSamplePlot(int sampleCount, QVector<double> sampleUniqueFinishTimeVector, QVector<QList<double> > sampleIncrementalResultListVector, QVector<QList<double> > sampleCumulativeResultListVector);

	// Data members for computing the mean and standard deviation in Incremental values for each time interval[i] for each function[j]
	QVector<QList<double> > sumIncrementalListVector;
	QVector<QList<double> > squaredSumIncrementalListVector;
	QVector<QList<double> > maxIncrementalListVector;
	QVector<QList<double> > minIncrementalListVector;

	// Data members for computing the mean and standard deviation in cumulative values
	QVector<QList<double> > sumCumulativeListVector;
	QVector<QList<double> > squaredSumCumulativeListVector;
	QVector<QList<double> > maxCumulativeListVector;
	QVector<QList<double> > minCumulativeListVector;


	//// data members for text files
	//QString theIncrementalPDFFiles;
	//QList<QString> theIncrementalPDFFileList;

	//QString theIncrementalCDFFiles;
	//QList<QString> theIncrementalCDFFileList;

	//QString theCumulativePDFFiles;
	//QList<QString> theCumulativePDFFileList;

	//QString theCumulativeCDFFiles;
	//QList<QString> theCumulativeCDFFileList;

	// Data members for plotting
	QList<QPointer<RPlotWidget> > theTimePlotList;
	QPointer<RPlotWidget> theTimePlot;

	QList<QPointer<RPlotWidget> > theCumulativeTimePlotList;
	QPointer<RPlotWidget> theCumulativeTimePlot;

#ifndef NO_QCP
	QList<QCPGraph *> theCurveList;
	QList<QCPGraph *> theCumulativeCurveList;

#endif // NO_QCP




#ifndef NO_QCP
	QList<QPointer<RPlotWidget> > theIncrementalPlotList;
	QList<QCPGraph *> theIncrementalMeanCurveList;
	QList<QCPGraph *> theIncrementalPlusCurveList;
	QList<QCPGraph *> theIncrementalMinusCurveList;

	QList<QPointer<RPlotWidget> > theCumulativePlotList;
	QList<QCPGraph *> theCumulativeMeanCurveList;
	QList<QCPGraph *> theCumulativePlusCurveList;
	QList<QCPGraph *> theCumulativeMinusCurveList;
#endif // NO_QCP


};

#endif // RDiscreteEventSamplingAnalyzer_H
#pragma once
