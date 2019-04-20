// Last modified by Mojtaba on June 29, 2012

#ifndef RFailureProbabilityAccumulator_H
#define RFailureProbabilityAccumulator_H

#include "RAccumulator.h"

class RRandomNumberGenerator;

class RFailureProbabilityAccumulator : public RAccumulator
{
	Q_OBJECT

	// Sets the target CoV for the sampling analysis
	Q_PROPERTY(double TargetCoefficientOfVariation READ getTargetCoefficientOfVariation WRITE setTargetCoefficientOfVariation USER true)

	// Sets the RRandomNumberGenerator object, which is needed to get the realization of rvs in the standard normal space
	Q_PROPERTY(QObject *RandomNumberGenerator READ getRandomNumberGenerator WRITE setRandomNumberGenerator USER false)	

public:
	RFailureProbabilityAccumulator(QObject *parent, QString name);
	~RFailureProbabilityAccumulator();

	// getter/setter methods for meta-properties
	long getMaximumIterations();
	void setMaximumIterations(long value);
	double getTargetCoefficientOfVariation() const;
	void setTargetCoefficientOfVariation(double value);	
	QObject *getRandomNumberGenerator() const;
	void setRandomNumberGenerator(QObject *value);	

	// Accumulates the result of the sample which is prepared in the given threadID, needs to be inside a safe area
	int accumulate(QVector<double> functionValueVector, gsl_vector * standardNormalRandomNumbers, QVector<double> sensitivityResultMeanVector, QVector<double> sensitivityResultStdvVector, int threadID);

	// Sets the list of functions
	void setFunctionList(QList<RFunction *> passedFunctionList);

	// Gets a flag indicating if the sampling should continue	
	bool getContinueFlag();

	// Resets the value of some data members before a new analysis
	void reset();

	// Gets the value of te computed failure probability
	double getFailureProbability(RFunction *passedFunction);

	// Sensitivity implementations
	void setSensitivityFunctionFlagList(QList<bool> flaglist);

	// This method is called to do the final stuff when analysis is ended 
	void terminateAccumulation();

	// Returns true if the accumulator needs standard normal random numbers  in order to accumulate
	bool doesNeedStandardNormalRandomNumbers();

	// returns true as this accumulator is parallelized
	bool canBeRunParallelly();

signals :

	void signalUpdatePlot();


private slots :

	// Auxiliary method to update the plots
	void updatePlot();

private:
	// Auxiliary method  that resizes a QVector to "count" and sets all its values to Zero
	void zeroQVector(QVector<double> *passedVector, double count);

	// Data members for meta-properties
	RPointer<RRandomNumberGenerator> theRandomNumberGenerator;
	double theTargetCoefficientOfVariation;

	// lock for target coefficient of variation
	QReadWriteLock * theTargetCoefficientOfVariationLock;

	// Data members for updatiing the failure probability values
	QVector<double> qSum;
	QVector<double> qSquaredSum;
	QVector<double> qBar;
	QVector<double> qBarVariance;
	QVector<double> qBarCOV;	
	double governingCOV;
	bool failureHasOccured;

	// Data members for plotting

	QVector<double> theIndexVector;
	QList<QVector<double> > thePfCovVector;

#ifndef NO_QCP
	QList<QCPGraph *> theCurveList;
	QPointer<QCustomPlot> thePlot;
#endif // NO_QCP



	// Sensitivity implementations
	QVector<double> qMeanSensitivity;
	QVector<double> qStdvSensitivity;
	QVector<double> qBarMeanSensitivity;
	QVector<double> qBarStdvSensitivity;

	QVector<double> qSquareMeanSensitivity;
	QVector<double> qSquareStdvSensitivity;
	QVector<double> qCOVMeanSensitivity;
	QVector<double> qCOVStdvSensitivity;
	
};

#endif // RFailureProbabilityAccumulator_H
