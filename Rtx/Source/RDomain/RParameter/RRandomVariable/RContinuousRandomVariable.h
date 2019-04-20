// Last modified by Hesam T on June 24, 2015

#ifndef RContinuousRandomVariable_H
#define RContinuousRandomVariable_H

#include "RRandomVariable.h"
#include "RParameter.h"

enum DistributionTypeEnum
{
	Normal, Exponential, BoundedExponential, Linear, Laplace, Rayleigh, Gamma, Uniform, Lognormal, Chi_square, Scaled_inverse_chi_square, t_distribution, Beta, Logistic, Weibull, Gumbel,

#ifndef OLD_DISTRIBUTION

	FDistribution, Pareto, type2Gumbel, Frechet, Arcsine, BirnbaumSaunders, IrwinHall, Wald,

#endif

	User_Defined

};

class RContinuousRandomVariable : public RRandomVariable
{
	Q_OBJECT

		// Sets the probability distribution type
		Q_PROPERTY(QString DistributionType READ getDistributionType WRITE setDistributionType USER false)

		// Sets the mean value as a parameter object or a double
		Q_PROPERTY(QString Mean READ getMean WRITE setMean USER false)

		// Sets the stdv as a parameter object or a double
		Q_PROPERTY(QString StandardDeviation READ getStandardDeviation WRITE setStandardDeviation USER false)

		// Sets the coefficient of variation (CoV) as a parameter object or a double
		Q_PROPERTY(QString CoefficientOfVariation READ getCoefficientOfVariation WRITE setCoefficientOfVariation USER false)

		// Sets the first distribution parameter as a parameter object or a double
		Q_PROPERTY(QString Parameter1 READ getParameter1 WRITE setParameter1 USER false)

		// Sets the second distribution parameter as a parameter object or a double
		Q_PROPERTY(QString Parameter2 READ getParameter2 WRITE setParameter2 USER false)

		// Sets the third distribution parameter as a parameter object or a double
		Q_PROPERTY(QString Parameter3 READ getParameter3 WRITE setParameter3 USER false)

		// Sets the fourth distribution parameter as a parameter object or a double
		Q_PROPERTY(QString Parameter4 READ getParameter4 WRITE setParameter4 USER false)

		// Indicates whether the source of uncertainty is Aleatory of Epistemic
		Q_PROPERTY(RUncertaintyType UncertaintyType READ getUncertaintyType WRITE setUncertaintyType USER false)

		// Read-only meta-property that shows the Alpha importance measure
		Q_PROPERTY(double AlphaImportanceMeasure READ getAlphaImportanceMeasure USER false)

		// Read-only meta-property that shows the Gamma importance measure
		Q_PROPERTY(double GammaImportanceMeasure READ getGammaImportanceMeasure USER false)

		// Read-only meta-property that shows the importance measure from FOSM realiability analysis
		Q_PROPERTY(double FOSMImportanceMeasure READ getFOSMImportanceMeasure USER false)

		// Read-only meta-property that shows the Delta sensitivity measure (derivative of the reliability index wrt. mean times stdv)
		Q_PROPERTY(double DeltaSensitivityMeasure READ getDeltaSensitivityMeasure USER false)

		// Read-only meta-property that shows the Eta sensitivity measure (derivative of the reliability index wrt. stdv times stdv)
		Q_PROPERTY(double EtaSensitivityMeasure READ getEtaSensitivityMeasure USER false)

		// Read-only meta-property that shows the Kappa sensitivity measure (derivative of the reliability index wrt. CoV times stdv)
		Q_PROPERTY(double KappaSensitivityMeasure READ getKappaSensitivityMeasure USER false)

		// Read-only meta-property that shows the derivative of the realiability index wrt. the mean
		Q_PROPERTY(double BetaSensitivityWrtMean READ getBetaSensitivityWrtMean USER false)

		// Read-only meta-property that shows the derivative of the realiability index wrt. the stdv
		Q_PROPERTY(double BetaSensitivityWrtStandardDeviation READ getBetaSensitivityWrtStandardDeviation USER false)

		// Enum for the type of uncertainty; can be Aleatory or Epistemic
		Q_ENUMS(RUncertaintyType)

		// Sets the Incremental cost of the mean value as a parameter object or a double
		Q_PROPERTY(QString IncrementalCostOfMean READ getIncrementalCostOfMean WRITE setIncrementalCostOfMean)

		// Sets the Incremental cost of the stdv value as a parameter object or a double
		Q_PROPERTY(QString IncrementalCostOfStandardDeviation READ getIncrementalCostOfStandardDeviation WRITE setIncrementalCostOfStandardDeviation)

public:
	RContinuousRandomVariable(QObject *parent, QString name);
	~RContinuousRandomVariable();

	// Declaring the RUncertaintyType enum values
	enum RUncertaintyType { Aleatory, Epistemic };

	DistributionTypeEnum DistributionEnumType;


	// getter/setter methods for meta-properties ,  (DO NOT USE THIS METHOD IN PARALLEL COMPUTATION, use the ones with threadID instead)
	QString getDistributionType() const;
	QString getMean();
	QString getStandardDeviation();
	QString getCoefficientOfVariation();
	QString getParameter1();
	QString getParameter2();
	QString getParameter3();
	QString getParameter4();
	RUncertaintyType getUncertaintyType();
	void setDistributionType(QString value);
	void setMean(QString value);
	void setStandardDeviation(QString value);
	void setCoefficientOfVariation(QString value);
	void setParameter1(QString value);
	void setParameter2(QString value);
	void setParameter3(QString value);
	void setParameter4(QString value);
	void setUncertaintyType(RUncertaintyType value);
	double getAlphaImportanceMeasure() const;
	void setAlphaImportanceMeasure(double value);
	double getGammaImportanceMeasure() const;
	void setGammaImportanceMeasure(double value);
	double getFOSMImportanceMeasure() const;
	void setFOSMImportanceMeasure(double value);
	double getDeltaSensitivityMeasure() const;
	void setDeltaSensitivityMeasure(double value);
	double getEtaSensitivityMeasure() const;
	void setEtaSensitivityMeasure(double value);
	double getKappaSensitivityMeasure() const;
	void setKappaSensitivityMeasure(double value);
	double getBetaSensitivityWrtMean() const;
	void setBetaSensitivityWrtMean(double value);
	double getBetaSensitivityWrtStandardDeviation() const;
	void setBetaSensitivityWrtStandardDeviation(double value);
	QString getIncrementalCostOfMean();
	QString getIncrementalCostOfStandardDeviation();
	void setIncrementalCostOfMean(QString value);
	void setIncrementalCostOfStandardDeviation(QString value);

	// getters and setters for using in normal code!
	double getMeanValue(int threadID);
	void setMeanValue(double value, int threadID);
	double getStandardDeviationValue(int threadID);
	void setStandardDeviationValue(double value, int threadID);
	double getCoefficientOfVariationValue(int threadID);
	void setCoefficientOfVariationValue(double value, int threadID);
	double getParameter1Value(int threadID);
	void setParameter1Value(double value, int threadID);
	double getParameter2Value(int threadID);
	void setParameter2Value(double value, int threadID);
	double getParameter3Value(int threadID);
	void setParameter3Value(double value, int threadID);
	double getParameter4Value(int threadID);
	void setParameter4Value(double value, int threadID);
	double getIncrementalCostOfMeanValue(int threadID);
	double getIncrementalCostOfStandardDeviationValue(int threadID);
	void setIncrementalCostOfMeanValue(double value, int threadID);
	void setIncrementalCostOfStandardDeviationValue(double value, int threadID);

	// Returns the PDF at a given realization of the random variable in the given threadID ($MTh$)
	double getPDFValue(double rvValue, int threadID);

	// Returns the CDF at a given realization of the random variable in the given threadID ($MTh$)
	double getCDFValue(double rvValue, int threadID);

	// Returns the inverse CDF at a given probability value in the given threadID ($MTh$)
	double getInverseCDFValue(double probValue, int threadID);

	// Returns a list of parameters that are input to the random variable as mean, stdv, etc.
	QList<RParameter *> getParameterList();

	// Sets the dynamic properties that appear when the "User-Defined" distribution type is selected; these properties setup the user-defined PDF
	void setDynamicProperty(QString propertyName, QVariant propertyValue);

	// Assigns the user-defined (x,PDF) to the corresponding data members and normalizes the sum of the PDFs to 1.0
	void setUserDefinedPDF(QVector<double> passedXVector, QVector<double> passedPDFVector, bool viaGUI = false);

	// Computes the derivative of the CDF wrt. the mean and stdv by finite difference in the given threadID ($MTh$); used when the analytical derivative is not available
	int computeCDFSensitivityToParameters(double rvValue, double *dCDFdMean, double *dCDFdStdv, int threadID);

	// Computes the derivative of the PDF wrt. the mean and stdv by finite difference in the given threadID ($MTh$); used when the analytical derivative is not available
	int computePDFSensitivityToParameters(double rvValue, double *dPDFdMean, double *dPDFdStdv, int threadID);

	// Computes the derivative of the PDF wrt. the mean and stdv by finite difference in the given threadID ($MTh$); used when the analytical derivative is not available
	int computeInverseCDFSensitivityToParameters(double probValue, double *dInverseCDFdMean, double *dInverseCDFdStdv, int threadID);

	// Returns a list of "right-click" menu items
	QStringList getActionList();

	// Copies the value at index 0 to the index of the given threadID for data members that are changed to array due to parallelization. Each subclass must also call the method of their superclass
	void initializeForParallelComputation(int threadID);

	// returns true as this parameter is parallelized
	bool canBeRunParallelly();

	private slots:
	// Slot to plot the PDF
	void plotPDF();

	// Slot to plot the CDF
	void plotCDF();

	// Slot to plot the inverse CDF
	void plotInverseCDF();

	// Slot that is invoked when the user asks for PDF at a given realization by using the "right-lick" menu
	void getPDF();

	// Slot that is invoked when the user asks for CDF at a given realization by using the "right-lick" menu
	void getCDF();

	// Slot that is invoked when the user asks for inverse CDF at a given realization by using the "right-lick" menu
	void getInverseCDF();

private:

	// Auxiliary method that updates the distribution parameters when the mean, stdv, or CoV change, in the given threadID ($MTh$)
	int updateParameters(int threadID);

	// Auxiliary method that updates the mean and stdv values when the distribution parameters change, in the given threadID ($MTh$)
	int updateMeanStdv(int threadID);

	// Auxiliary method that updates the mean and stdv values when the CoV changes, in the given threadID ($MTh$)
	int updateMeanStdvByCOV(int threadID);

	// Auxiliary method to do the plotting for PDF, CDF, and inverse CDF
	void plot(QString type);

	// Auxiliary method to obtain the PDF, CDF, and inverse CDF when the user has asked to by using the "right-click" menu
	void get(QString type);

	// Issues an error when the passed user-defined (x,PDF) points are empty, or the number of x-values does not match that of PDF-values
	int userDefinedErrorCheck();

	// Data members for meta-properties
	QString theDistributionType;
	RPointer<RParameter> theMeanParameter;
	RPointer<RParameter> theStandardDeviationParameter;
	RPointer<RParameter> theCoefficientOfVariationParameter;
	RPointer<RParameter> theParameter1Parameter;
	RPointer<RParameter> theParameter2Parameter;
	RPointer<RParameter> theParameter3Parameter;
	RPointer<RParameter> theParameter4Parameter;
	RPointer<RParameter> theIncrementalCostOfMeanParameter;
	RPointer<RParameter> theIncrementalCostOfStandardDeviationParameter;

	// array of double to keep the values in each threadID ($MTh$)
	double *theMeanDouble;
	double *theStandardDeviationDouble;
	double *theCoefficientOfVariationDouble;
	double *theParameter1Double;
	double *theParameter2Double;
	double *theParameter3Double;
	double *theParameter4Double;

	RUncertaintyType theUncertaintyType;
	double theAlphaImportanceMeasure;
	double theGammaImportanceMeasure;
	double theFOSMImportanceMeasure;
	double theDeltaSensitivityMeasure;
	double theEtaSensitivityMeasure;
	double theKappaSensitivityMeasure;
	double theBetaSensitivityWrtMean;
	double theBetaSensitivityWrtStandardDeviation;

	double* theIncrementalCostOfMeanDouble;
	double* theIncrementalCostOfStandardDeviationDouble;

	// Data members for the dynamic properties for the user-defined distribution
	QVector<double> XVector;
	QVector<double> PDFVector;
};

#endif // RContinuousRandomVariable_H
