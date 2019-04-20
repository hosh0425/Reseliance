// Last modified by Mojtaba on March 19, 2014

#ifndef RAccumulator_H
#define RAccumulator_H

#include "RObject.h"
#include "RRandomVariable.h"
#include <gsl/gsl_vector.h>


#ifndef NO_QCP
#include "qcustomplot.h"
#endif

class RFunction;
class QReadWriteLock;

class RAccumulator : public RObject
{
	Q_OBJECT

	// Sets the maximum numbe of samples
	Q_PROPERTY(long MaximumIterations READ getMaximumIterations WRITE setMaximumIterations USER true)

	// Sets the refresh rate of the plots at every ... samples
	Q_PROPERTY(long PlottingInterval READ getPlottingInterval WRITE setPlottingInterval USER true)

	// Determines if the plots should be shown
	Q_PROPERTY(bool DisplayDiagrams READ getDisplayDiagrams WRITE setDisplayDiagrams USER true)

	// Controls the amount of output that is shown in the output pane; options are None, Minimum, Moderate, and Maximum
	Q_PROPERTY(ROutputDisplayLevelType OutputDisplayLevel READ getOutputDisplayLevel WRITE setOutputDisplayLevel USER true)


public:
	RAccumulator(QObject *parent, QString name);
	virtual ~RAccumulator();

	// getter/setter methods for meta-properties
	long getMaximumIterations();
	void setMaximumIterations(long value);
	long getPlottingInterval() const;
	void setPlottingInterval(long value);
	bool getDisplayDiagrams();
	void setDisplayDiagrams(bool value);

	// Sensitivity implementations
	void setSensitivityRandomVariableList(QList<RPointer<RRandomVariable> > randomVariableList);
	virtual void setSensitivityFunctionFlagList(QList<bool> flaglist) = 0;
	void setComputeRandomVariableSensitivities(bool value);


	// Accumulates the result of the sample which is prepared in the given threadID
	virtual int accumulate(QVector<double> functionValueVector, gsl_vector * standardNormalRandomNumbers, QVector<double> sensitivityResultMeanVector, QVector<double> sensitivityResultStdvVector, int threadID) = 0;

	// Sets the list of functions
	virtual void setFunctionList(QList<RFunction *> passedFunctionList) = 0;

	// Gets a flag that indicates if the sampling should continue	
	virtual bool getContinueFlag() = 0;

	// Resets the value of some data members before a new analysis
	virtual void reset() = 0;

	// This method is called to do the final stuff when analysis is ended 
	virtual void terminateAccumulation() = 0;

	// Returns true if the accumulator needs standard normal random numbers  in order to accumulate
	virtual bool doesNeedStandardNormalRandomNumbers() = 0;

	// overwriting qDebug and qCritical to use accumulator specific input streams
	static QDebug rDebug(int threadID = 0);
	static QDebug rCritical(int threadID = 0);

protected: 
	
	// Data members that are inherited by subclasses

	// Data members for sampling analysis
	QList<RFunction *> theFunctionList;
	long theIterationIndex;

	// setter and getter for private data member : theContiueFlag
	bool getTheContinueFlagValue();
	void setTheContinueFlagValue(bool newValue);

	// Data member for sensitivity anlysis
	bool theComputeRandomVariableSensitivities;
	int theSRVSize;
	QList<bool> theSensitivityFunctionFlagList;
	QList<RPointer<RRandomVariable> > theSensitivityRandomVariableList;

private:

	// Data members for meta-properties
	long theMaximumIterations;
	long thePlottingInterval;
	bool theDisplayDiagrams;

	// locks for meta properties' getter and setter
	QReadWriteLock * theMaximumIterationsLock;
	QReadWriteLock * thePlottingIntervalLock;
	QReadWriteLock * theDisplayDiagramsLock;

	bool theContinueFlag;
	QReadWriteLock * theContinueFlagLock;
};

#endif // RAccumulator_H
