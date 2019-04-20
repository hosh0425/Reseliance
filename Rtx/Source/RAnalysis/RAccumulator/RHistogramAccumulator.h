// Last modified by Mojtaba on March 16, 2014

#ifndef RHistogramAccumulator_H
#define RHistogramAccumulator_H

#include "RAccumulator.h"

class QFile;

class RRandomNumberGenerator;

class RHistogramAccumulator : public RAccumulator
{
	Q_OBJECT

	// Sets the number of bins in the histogram
	Q_PROPERTY(int NumberOfBins READ getNumberOfBins WRITE setNumberOfBins USER true)

	// Sets the name of the text file in which the sample outcomes are stored
	Q_PROPERTY(QString OutputFile READ getOutputFile WRITE setOutputFile USER false)

	// Asks if the sample outcomes be saved per sample or not
	Q_PROPERTY(bool SaveOutputPerSample READ getSaveOutputPerSample WRITE setSaveOutputPerSample)

	// Sets the name of the text file in which the PDF is stored
	Q_PROPERTY(QString PDFFile READ getPDFFile WRITE setPDFFile USER false)

	// Sets the name of the text file in which the CDF amd CoV are stored
	Q_PROPERTY(QString CDFFile READ getCDFFile WRITE setCDFFile USER false)


	// Asks if the mean sensitvity plots is drawn or not
	Q_PROPERTY(bool DisplayMeanSensitvityDiagrams READ getDisplayMeanSensitvityDiagrams WRITE setDisplayMeanSensitvityDiagrams)

	// Asks if the standard deviation sensitvity plots is drawn or not
	Q_PROPERTY(bool DisplayStandardDeviationSensitvityDiagrams READ getDisplayStandardDeviationSensitvityDiagrams WRITE setDisplayStandardDeviationSensitvityDiagrams)

	// Sets the name of the text file in which the CCDF Sensitivities and their CoV are stored
	Q_PROPERTY(QString CCDFSensitivityFile READ getCCDFSensitivityFile WRITE setCCDFSensitivityFile)

	// Sets the name of the text file in which the Joint PDF Sensitivities are stored
	Q_PROPERTY(QString JointPDFSensitivityFile READ getJointPDFSensitivityFile WRITE setJointPDFSensitivityFile)	
 
public:
	
	RHistogramAccumulator(QObject *parent, QString name);
	~RHistogramAccumulator();

	// getter/setter methods for meta-properties
	int getNumberOfBins() const;
	void setNumberOfBins(int value);
	QString getOutputFile() const;
	void setOutputFile(QString value);
	QString getPDFFile() const;
	void setPDFFile(QString value);
	QString getCDFFile() const;
	void setCDFFile(QString value);

	// sensitivity implementations - getters and setters for properties
	bool getDisplayMeanSensitvityDiagrams() const;
	void setDisplayMeanSensitvityDiagrams(bool value);
	bool getDisplayStandardDeviationSensitvityDiagrams() const;
	void setDisplayStandardDeviationSensitvityDiagrams(bool value);
	bool getSaveSensitivityResultsToFile() const;
	void setSaveSensitivityResultsToFile(bool value);
	QString getCCDFSensitivityFile() const;
	void setCCDFSensitivityFile(QString value);
	QString getJointPDFSensitivityFile() const;
	void setJointPDFSensitivityFile(QString value);
	bool getSaveOutputPerSample() const;
	void setSaveOutputPerSample(bool value);

	// Sensitivity implementations
	void setSensitivityFunctionFlagList(QList<bool> flaglist);


	// Accumulates the result of the sample which is prepared in the given threadID, needs to be inside a safe area
	int accumulate(QVector<double> functionValueVector, gsl_vector * standardNormalRandomNumbers, QVector<double> sensitivityResultMeanVector, QVector<double> sensitivityResultStdvVector, int threadID);

	// Sets the list of functions
	void setFunctionList(QList<RFunction *> passedFunctionList);

	// Gets a flag indicating if the sampling should continue	
	bool getContinueFlag();

	// Resets the value of some data members before a new analysis
	void reset();

	// This method is called to do the final stuff when analysis is ended 
	void terminateAccumulation();

	// Returns true if the accumulator needs standard normal random numbers  in order to accumulate
	bool doesNeedStandardNormalRandomNumbers();

	// returns true as this accumulator is parallelized
	bool canBeRunParallelly();

signals :

	// signal for updating plots of all functions, checkMinMax is a boolean argument to denote if min[i] != max[i] should be checked before updating plot
	void signalUpdatePlot(bool checkMinMax);

	// a signal for initiating all sensitivity plots
	void signalInitiateSensitivityPlots();


private slots :

	// Auxiliary method to update the plots, checkMinMax is a boolean argument to denote if min[i] != max[i] should be checked before updating plot
	void updatePlot(bool checkMinMax);

	// Auxiliary method for initiating sensitivity plots
	void initiateSensitivityPlotsSlot();

private:
	// Auxiliary method to evaluate the histogram
	void evaluateHistogram(int functionIndex, QVector<double> *xVector, QVector<double> *PDFVector);

	// Auxiliary method to save the CDf and CoV in a text file
	void saveCDFtoTextFile();
	
	// Auxiliary method to save the PDf in a text file
	void savePDFtoTextFile();
	
	// Auxiliary method to compute the vector of CDFs and CoVs
	void computeCDFandCoV(QVector<double> *CDFVector, QVector<double> *CoVVector);

	// Auxiliary method to evaluate Pf sensitivities for each threshold
	void evaluateSensitivity(QVector<double> SortedSensitivityVector, QVector<double> *SensitvityMu, QVector<double> *SensitvityCoV);

	// Auxiliary method to save the Sensitivity results in text files
	// Also, Carry out a numerical integration on the sensitivities across all function values to compute dE/dMean and dE/dStdv 
	void saveSensitvitytoTextFile(QList<RPointer<RRandomVariable> > sensitivityRandomVariableList);

	// Auxiliary method to compute trapezoidal integration, used to compute dE/dMean and dE/dStdv
	double trapezoidalIntegration(QVector<double> x, QVector<double> y);

	// Data members for meta-properties
	int theNumberOfBins;
	int cntrSensFunc;
	QString theOutputFile;
	QString thePDFFile;
	QString theCDFFile;

	// lock for safe reading and writing theNumberOfBins
	QReadWriteLock * theNumberOfBinsLock;

	// Data members for computing the mean and standard deviation
	QVector<double> sumVector;
	QVector<double> squaredSumVector;

	// Data members for plotting

	// a list of sorted vectors (each new value is inserted in its appropriate place)
	QList<QVector<double> > theFunctionValueVectorList;
	QVector<double> maxVector;
	QVector<double> minVector;

#ifndef NO_QCP
	QList<QPointer<QCustomPlot> > thePlotList;
	QList<QPointer<QCustomPlot> > theSensitivityPlotList;
	QList<QCPGraph *> thePDFCurveList;
	QList<QCPGraph *> theCDFCurveList;
	QList<QCPGraph *> theCoVCurveList;
	QList<QCPGraph *> theMeanMuCurveList;
	QList<QCPGraph *> theMeanCoVCurveList;
	QList<QCPGraph *> theStdvMuCurveList;
	QList<QCPGraph *> theStdvCoVCurveList;
#endif



	// Data member for the text file that stores the sample outcomes
	QFile *outputQFile;


	QString tempString;
	// Data member for sensitivity analysis

	// List of List of vectors: The vector contains df/dMean*1/f for a random variable and for a specific function; the middle list is for a random variable; the outside list is for a function
	QList<QVector<double> > theSensitivityMeanValueVectorList;
	QList<QVector<double> > theSensitivityStdvValueVectorList;

	bool theDisplayMeanSensitvityDiagrams;
	bool theDisplayStandardDeviationSensitvityDiagrams;

	bool theSaveOutputPerSample;
	// Vectors for storing the summation of the sensitivities of each function wrt each rv and its squared value; The sensitivity of the 1st function wrt to all rvs is first stored in the vector, followed by the sensitivity of the 2nd function wrt to all rvs, and so on
	QVector<double> sumMeanVector;
	QVector<double> squaredSumMeanVector;
	QVector<double> sumMeanBetaVector;
	QVector<double> sumStdvVector;
	QVector<double> squaredSumStdvVector;
	QVector<double> sumStdvBetaVector;


	// Stores the sensitivity of first moment wrt Mean and Stdv
	QList<QVector<double> > sensitivityFirstMomentList;

	QString theCCDFSensitivityFile;
	QString theJointPDFSensitivityFile;

	bool theSensitivityPlotsHaveBeenInitiated;
};

#endif // RHistogramAccumulator_H
