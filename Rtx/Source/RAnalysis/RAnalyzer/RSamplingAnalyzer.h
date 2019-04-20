// Last modified by Mojtaba on June 29, 2012

#ifndef RSamplingAnalyzer_H
#define RSamplingAnalyzer_H

#include "RAnalyzer.h"
#include "RParallelComputerInterface.h"
#include <gsl/gsl_vector.h>

class RFunction;
class RRandomNumberGenerator;
class RTransformer;
class RAccumulator;
class RRandomVariable;

class QMutex;
class RParallelProcessor;


class RSamplingAnalyzer : public RAnalyzer, public RParallelComputerInterface
{
	Q_OBJECT

	// Sets the RRandomNumberGenerator object, which generates realizations for rvs
	Q_PROPERTY(QObject *RandomNumberGenerator READ getRandomNumberGenerator WRITE setRandomNumberGenerator)

	// Sets the RTransformer object, which transforms points between original and standard normal spaces
	Q_PROPERTY(QObject *Transformer READ getTransformer WRITE setTransformer)

	// Sets the RAccumulator object, which accumulates the outcome of samples
	Q_PROPERTY(QObject *Accumulator READ getAccumulator WRITE setAccumulator)

	// Sets the list of functions subject to sampling analysis
	Q_PROPERTY(QString FunctionList READ getFunctionList WRITE setFunctionList)


	// Sets if the sensitvity wrt to mean and stdv of random variables should be done or not
	Q_PROPERTY(bool ComputeRandomVariableSensitivities READ getComputeRandomVariableSensitivities WRITE setComputeRandomVariableSensitivities)
	
	// Sets if the correlations of sensitivity random variables should be neglected or not
	//Q_PROPERTY(bool NeglectCorrelationInSensitivity READ getNeglectCorrelationInSensitivity WRITE setNeglectCorrelationInSensitivity)
	
	// Sets the list of random variables whose sensitvity is sought (can be empty when sensitivity analysis is not done)
	Q_PROPERTY(QString SensitivityRandomVariableList READ getSensitivityRandomVariableList WRITE setSensitivityRandomVariableList)
	
	// Sets the flags that indicate for which function the sensitvity analysis should be done (can be empty when sensitivity analysis is not done)
	Q_PROPERTY(QString SensitivityFunctionList READ getSensitivityFunctionList WRITE setSensitivityFunctionList)


public:
	RSamplingAnalyzer(QObject *parent, QString name);
	~RSamplingAnalyzer();

	// getter/setter methods for meta-properties
	QObject *getRandomNumberGenerator() const;
	void setRandomNumberGenerator(QObject *value);
	QObject *getTransformer() const;
	void setTransformer(QObject *value);
	QObject *getAccumulator() const;
	void setAccumulator(QObject *value);
	QString getFunctionList();
	void setFunctionList(QString value);

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

	// An over-loaded method to run the analysis with the option of not parsing (tokenizing) the functions, assuming that they have already been parsed
	// This is only called from within the load coincidence analysis in which the function need not be re-parsed for every sampling analysis for diffrenet hazards
	int analyze(bool populateListsInFunction);

	// Initializes some of the contributed objects for parallel computation
	void initializeForParallelComputation(int threadID);

	// Resets some of the contributed objects after parallel computation
	void resetAfterParallelComputation();

	// Returns true if all the important contributed objects can be run parallelly
	bool canBeRunParallelly();

private:

	// a wrapper around doAnalysis method to give to the parallel computer
	int static doAnalysisWrapper(void * theObject, void * inputsArgs[], int threadID);

	// the part of the analysis that is conducted in each thread
	int doAnalysis(int SRVSize, int threadID = 0);

	// frees all the given vectors and returns the error code (this piece of code is written to prevent code repitition)
	int endAnalysis(gsl_vector* x, gsl_vector* dfdMean_1_f, gsl_vector* dfdStdv_1_f, int errorCode);

	// Data members for meta-properties
	RPointer<RRandomNumberGenerator> theRandomNumberGenerator;
	RPointer<RTransformer> theTransformer;
	RPointer<RAccumulator> theAccumulator;
	QList<RPointer<RFunction> > theFunctionList;

	// the mereged list of random variables used in all of the passed functions
	QList<RRandomVariable *> theRandomVariableList;

	// a field for keeping the number of random variables
	int theNumberOfRandomVariables;

	// a reference to parallelComputer class to handle most of the stuff about parallelism
	RParallelProcessor * analysisParallelComputer;

	// Data members that gets the propertirs for sensitivity analysis
	QList<RPointer<RRandomVariable>> theSensitivityRandomVariableList;
	bool theComputeRandomVariableSensitivities;
	bool theNeglectCorrelationInSensitivity;
	QList<RPointer<RFunction>> theSensitivityFunctionList;

};

#endif // RSamplingAnalyzer_H
