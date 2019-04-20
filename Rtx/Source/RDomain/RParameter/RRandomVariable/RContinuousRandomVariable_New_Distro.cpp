#include "RContinuousRandomVariable.h"

#include "RMainWindow.h"
#include "RPlotWidget.h"

#include <QInputDialog>

//#include <qwt_plot.h>
//#include <qwt_plot_curve.h>
//#include <qwt_legend.h>

#include <gsl/gsl_randist.h>
#include <gsl/gsl_cdf.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_sf_gamma.h>

#include <math.h>
#include "RDomain.h"

#include <qmutex.h>

RContinuousRandomVariable::RContinuousRandomVariable(QObject *parent, QString name)
	: RRandomVariable(parent, name)
{
	
	/* //$$
	theMeanParameter = 0;
	theStandardDeviationParameter = 0;
	theCoefficientOfVariationParameter = 0;
	theParameter1Parameter = 0;
	theParameter2Parameter = 0;
	theParameter3Parameter = 0;
	theParameter4Parameter = 0;
	*/

	theUncertaintyType = RContinuousRandomVariable::Aleatory;

	// allocating memory (size = the number of threads) and initiating for the array fields ($MTh$)
	int arraySizeForParallelizedObject = getArraySizeForParallelizedObject();

	theMeanDouble = new double[arraySizeForParallelizedObject];
	theStandardDeviationDouble = new double[arraySizeForParallelizedObject];
	theCoefficientOfVariationDouble = new double[arraySizeForParallelizedObject];
	theParameter1Double = new double[arraySizeForParallelizedObject];
	theParameter2Double = new double[arraySizeForParallelizedObject];
	theParameter3Double = new double[arraySizeForParallelizedObject];
	theParameter4Double = new double[arraySizeForParallelizedObject];
	theIncrementalCostOfMeanDouble = new double[arraySizeForParallelizedObject];
	theIncrementalCostOfStandardDeviationDouble = new double[arraySizeForParallelizedObject];

	// set the values at index=0 to 0.0

	theMeanDouble[0] = 0.0;
	theStandardDeviationDouble[0] = 0.0;
	theCoefficientOfVariationDouble[0] = 0.0;
	theParameter1Double[0] = 0.0;
	theParameter2Double[0] = 0.0;
	theParameter3Double[0] = 0.0;
	theParameter4Double[0] = 0.0;

	theIncrementalCostOfMeanDouble[0] = 1;
	theIncrementalCostOfStandardDeviationDouble[0] = 1;

	theAlphaImportanceMeasure = 0.0;
	theGammaImportanceMeasure = 0.0;
	theFOSMImportanceMeasure = 0.0;
	theDeltaSensitivityMeasure = 0.0;
	theEtaSensitivityMeasure = 0.0;
	theKappaSensitivityMeasure = 0.0;
	theBetaSensitivityWrtMean = 0.0;
	theBetaSensitivityWrtStandardDeviation = 0.0;

	theDistributionType = "Normal (mean, stdv)";

	if (theDistributionType == QString("User Defined")) {
		QString XPointsStr = property("XPoints").toString();
		QString PDFPointsStr = property("PDFPoints").toString();
		setProperty("XPoints", XPointsStr);
		setProperty("PDFPoints", PDFPointsStr);
	}
	else {
		setProperty("XPoints", QVariant());
		setProperty("PDFPoints", QVariant());
	}
}

RContinuousRandomVariable::~RContinuousRandomVariable()
{
	// free the memory allocated for arrays
	delete[] theMeanDouble;
	delete[] theStandardDeviationDouble;
	delete[] theCoefficientOfVariationDouble;
	delete[] theParameter1Double;
	delete[] theParameter2Double;
	delete[] theParameter3Double;
	delete[] theParameter4Double;
	delete[] theIncrementalCostOfMeanDouble;
	delete[] theIncrementalCostOfStandardDeviationDouble;
}

QString RContinuousRandomVariable::getDistributionType() const
{
	return theDistributionType;
}

QString RContinuousRandomVariable::getMean()
{
	return getStringParameter(&theMeanDouble, theMeanParameter);
}

QString RContinuousRandomVariable::getStandardDeviation()
{
	return getStringParameter(&theStandardDeviationDouble, theStandardDeviationParameter);
}

QString RContinuousRandomVariable::getCoefficientOfVariation()
{
	return getStringParameter(&theCoefficientOfVariationDouble, theCoefficientOfVariationParameter);
}

QString RContinuousRandomVariable::getParameter1()
{
	return getStringParameter(&theParameter1Double, theParameter1Parameter);
}

QString RContinuousRandomVariable::getParameter2()
{
	return getStringParameter(&theParameter2Double, theParameter2Parameter);
}

QString RContinuousRandomVariable::getParameter3()
{
	return getStringParameter(&theParameter3Double, theParameter3Parameter);
}

QString RContinuousRandomVariable::getParameter4()
{
	return getStringParameter(&theParameter4Double, theParameter4Parameter);
}

RContinuousRandomVariable::RUncertaintyType RContinuousRandomVariable::getUncertaintyType()
{
	return theUncertaintyType;
}

void RContinuousRandomVariable::setDistributionType(QString value)
{
	theDistributionType = value;

	if (theDistributionType == QString("User Defined")) {
		QString XPointsStr = property("XPoints").toString(); 
		QString PDFPointsStr = property("PDFPoints").toString();
		setProperty("XPoints", XPointsStr);
		setProperty("PDFPoints", PDFPointsStr);
	} else {
		setProperty("XPoints", QVariant());
		setProperty("PDFPoints", QVariant());
	}

	updateParameters();
}

void RContinuousRandomVariable::setMean(QString value)
{
	setStringParameter(value, &theMeanDouble, &theMeanParameter);
	setCurrentValue(getMeanValue());
	updateParameters();
}

void RContinuousRandomVariable::setStandardDeviation(QString value)
{

	setStringParameter(value, &theStandardDeviationDouble, &theStandardDeviationParameter);
	updateParameters();
}

void RContinuousRandomVariable::setCoefficientOfVariation(QString value)
{
	setStringParameter(value, &theCoefficientOfVariationDouble, &theCoefficientOfVariationParameter);
	updateMeanStdvByCOV();
	updateParameters();
}

void RContinuousRandomVariable::setParameter1(QString value)
{
	setStringParameter(value, &theParameter1Double, &theParameter1Parameter);
	updateMeanStdv();
	setCurrentValue(getMeanValue());
}

void RContinuousRandomVariable::setParameter2(QString value)
{
	setStringParameter(value, &theParameter2Double, &theParameter2Parameter);
	updateMeanStdv();
	setCurrentValue(getMeanValue());
}

void RContinuousRandomVariable::setParameter3(QString value)
{
	setStringParameter(value, &theParameter3Double, &theParameter3Parameter);
	updateMeanStdv();
	setCurrentValue(getMeanValue());
}

void RContinuousRandomVariable::setParameter4(QString value)
{
	setStringParameter(value, &theParameter4Double, &theParameter4Parameter);
	updateMeanStdv();
	setCurrentValue(getMeanValue());
}

void RContinuousRandomVariable::setUncertaintyType(RContinuousRandomVariable::RUncertaintyType value)
{
	theUncertaintyType = value;
}

QString RContinuousRandomVariable::getIncrementalCostOfMean()
{
	return getStringParameter(&theIncrementalCostOfMeanDouble, theIncrementalCostOfMeanParameter);
}

QString RContinuousRandomVariable::getIncrementalCostOfStandardDeviation()
{
	return getStringParameter(&theIncrementalCostOfStandardDeviationDouble, theIncrementalCostOfStandardDeviationParameter);
}

double RContinuousRandomVariable::getIncrementalCostOfMeanValue()
{
	return getDoubleParameter(&theIncrementalCostOfMeanDouble, theIncrementalCostOfMeanParameter);
}

double RContinuousRandomVariable::getIncrementalCostOfStandardDeviationValue()
{
	return getDoubleParameter(&theIncrementalCostOfStandardDeviationDouble, theIncrementalCostOfStandardDeviationParameter);
}

void RContinuousRandomVariable::setIncrementalCostOfMean(QString value)
{
	setStringParameter(value, &theIncrementalCostOfMeanDouble, &theIncrementalCostOfMeanParameter);
	setCurrentValue(getIncrementalCostOfMeanValue());
	updateParameters();
}

void RContinuousRandomVariable::setIncrementalCostOfMeanValue(double value)
{
	if (theIncrementalCostOfMeanParameter == 0) {
		theIncrementalCostOfMeanDouble = value;
	}
	else {
		theIncrementalCostOfMeanParameter->setCurrentValue(value);
	}
	updateParameters();
}

void RContinuousRandomVariable::setIncrementalCostOfStandardDeviation(QString value)
{
	setStringParameter(value, &theIncrementalCostOfStandardDeviationDouble, &theIncrementalCostOfStandardDeviationParameter);
	setCurrentValue(getIncrementalCostOfStandardDeviationValue());
	updateParameters();
}

void RContinuousRandomVariable::setIncrementalCostOfStandardDeviationValue(double value)
{
	if (theIncrementalCostOfStandardDeviationParameter == 0) {
		theIncrementalCostOfStandardDeviationDouble = value;
	}
	else {
		theIncrementalCostOfStandardDeviationParameter->setCurrentValue(value);
	}
	updateParameters();
}

double RContinuousRandomVariable::getMeanValue()
{
	return getDoubleParameter(&theMeanDouble, theMeanParameter);
}

double RContinuousRandomVariable::getCoefficientOfVariationValue()
{
	return getDoubleParameter(&theCoefficientOfVariationDouble, theCoefficientOfVariationParameter);
}

double RContinuousRandomVariable::getStandardDeviationValue()
{
	return getDoubleParameter(&theStandardDeviationDouble, theStandardDeviationParameter);
}

double RContinuousRandomVariable::getParameter1Value()
{
	return getDoubleParameter(&theParameter1Double, theParameter1Parameter);
}

double RContinuousRandomVariable::getParameter2Value()
{
	return getDoubleParameter(&theParameter2Double, theParameter2Parameter);
}

double RContinuousRandomVariable::getParameter3Value()
{
	return getDoubleParameter(&theParameter3Double, theParameter3Parameter);
}

double RContinuousRandomVariable::getParameter4Value()
{
	return getDoubleParameter(&theParameter4Double, theParameter4Parameter);
}

double RContinuousRandomVariable::getAlphaImportanceMeasure() const
{
	return theAlphaImportanceMeasure;
}

void RContinuousRandomVariable::setAlphaImportanceMeasure(double value)
{
	theAlphaImportanceMeasure = value;
}

double RContinuousRandomVariable::getGammaImportanceMeasure() const
{
	return theGammaImportanceMeasure;
}

void RContinuousRandomVariable::setGammaImportanceMeasure(double value)
{
	theGammaImportanceMeasure = value;
}

double RContinuousRandomVariable::getFOSMImportanceMeasure() const
{
	return theFOSMImportanceMeasure;
}

void RContinuousRandomVariable::setFOSMImportanceMeasure(double value)
{
	theFOSMImportanceMeasure = value;
}

double RContinuousRandomVariable::getDeltaSensitivityMeasure() const
{
	return theDeltaSensitivityMeasure;
}

void RContinuousRandomVariable::setDeltaSensitivityMeasure(double value)
{
	theDeltaSensitivityMeasure = value;
}

double RContinuousRandomVariable::getEtaSensitivityMeasure() const
{
	return theEtaSensitivityMeasure;
}

void RContinuousRandomVariable::setEtaSensitivityMeasure(double value)
{
	theEtaSensitivityMeasure = value;
}

double RContinuousRandomVariable::getKappaSensitivityMeasure() const
{
	return theKappaSensitivityMeasure;
}

void RContinuousRandomVariable::setKappaSensitivityMeasure(double value)
{
	theKappaSensitivityMeasure = value;
}

double RContinuousRandomVariable::getBetaSensitivityWrtMean() const
{
	return theBetaSensitivityWrtMean;
}

void RContinuousRandomVariable::setBetaSensitivityWrtMean(double value)
{
	theBetaSensitivityWrtMean = value;
}

double RContinuousRandomVariable::getBetaSensitivityWrtStandardDeviation() const
{
	return theBetaSensitivityWrtStandardDeviation;
}

void RContinuousRandomVariable::setBetaSensitivityWrtStandardDeviation(double value)
{
	theBetaSensitivityWrtStandardDeviation = value;
}


double RContinuousRandomVariable::getPDFValue(double x)
{	
	double result;

	if (theStandardDeviationDouble == 0.0) {
		qCritical() << "WARNING: The standard deviation of the random variable" << objectName() << "is Zero.";
	}

	if (theDistributionType == QString("Normal (mean, stdv)")) {
		double mean = getParameter1Value();
		double stdv = getParameter2Value();
		result = gsl_ran_gaussian_pdf(x - mean, stdv);
	}
	else if (theDistributionType == QString("Exponential (mu, xo)")) {
		double mu = getParameter1Value();
		double xo = getParameter2Value();
		result = gsl_ran_exponential_pdf(x - xo, mu);
	}
	else if (theDistributionType == QString("Bounded Exponential (mu, xo, min, max)")) {
		double min = getParameter3Value();
		double max = getParameter4Value();
		if (x < min || x > max) {
			result = 0.0;
		}
		else {
			double mu = getParameter1Value();
			double xo = getParameter2Value();
			double k = 1.0 / (1.0 - exp(-mu * (max - min)));
			result = k * mu * exp(-mu * (x - xo - min));
		}
	}
	else if (theDistributionType == QString("Linear (a, b)")) {
		double min = getParameter1Value();
		double max = getParameter2Value();
		if (x < min || x > max) {
			result = 0.0;
		}
		else {
			result = 2 * x / (max * max - min * min);
		}
	}
	else if (theDistributionType == QString("Laplace (a, xo)")) {
		double a = getParameter1Value();
		double xo = getParameter2Value();
		result = gsl_ran_laplace_pdf(x - xo, a);
	}
	else if (theDistributionType == QString("Rayleigh (sigma, xo)")) {
		double sigma = getParameter1Value();
		double xo = getParameter2Value();
		result = gsl_ran_rayleigh_pdf(x - xo, sigma);
	}
	else if (theDistributionType == QString("Gamma (a, b)")) {
		double a = getParameter1Value();
		double b = getParameter2Value();
		result = gsl_ran_gamma_pdf(x, a, b);
	}
	else if (theDistributionType == QString("Uniform (a, b)")) {
		double a = getParameter1Value();
		double b = getParameter2Value();
		result = gsl_ran_flat_pdf(x, a, b);
	}
	else if (theDistributionType == QString("Lognormal (zeta, sigma)")) {
		double zeta = getParameter1Value();
		double sigma = getParameter2Value();
		result = gsl_ran_lognormal_pdf(x, zeta, sigma);
	}
	else if (theDistributionType == QString("Chi-square (nu, xo)")) {
		double nu = getParameter1Value();
		double xo = getParameter2Value();
		result = gsl_ran_chisq_pdf(x - xo, nu);
	}
	else if (theDistributionType == QString("t-distribution (nu, xo)")) {
		double nu = getParameter1Value();
		double xo = getParameter2Value();
		result = gsl_ran_tdist_pdf(x - xo, nu);
	}
	else if (theDistributionType == QString("Beta (a, b, min, max)")) {
		double a = getParameter1Value();
		double b = getParameter2Value();
		double min = getParameter3Value();
		double max = getParameter4Value();
		result = gsl_ran_beta_pdf((x - min)/(max - min), a, b) / (max - min);
	}
	else if (theDistributionType == QString("Logistic (a, xo)")) {
		double a = getParameter1Value();
		double xo = getParameter2Value();
		result = gsl_ran_logistic_pdf(x - xo, a);
	}
	else if (theDistributionType == QString("Weibull (a, b)")) {
		double a = getParameter1Value();
		double b = getParameter2Value();
		result = gsl_ran_weibull_pdf(x, a, b);
	}
	else if (theDistributionType == QString("Gumbel (a, b)")) {
		double a = getParameter1Value();
		double b = getParameter2Value();
		result = gsl_ran_gumbel1_pdf(x, a, b);
	}
	else if (theDistributionType == QString("Cauchy (a, xo)")) {
		double a = getParameter1Value();
		double xo = getParameter2Value();
		result = gsl_ran_cauchy_pdf(x - xo, a);
	}
	else if (theDistributionType == QString("F-distribution (nu1, nu2)")) {
		double nu1 = getParameter1Value();
		double nu2 = getParameter2Value();
		result = gsl_ran_fdist_pdf(x, nu1, nu2);
	}
	else if (theDistributionType == QString("Pareto (a, b)")) {
		double a = getParameter1Value();
		double b = getParameter2Value();
		result = gsl_ran_pareto_pdf(x, a, b);
	}
	else if (theDistributionType == QString("Type-2 Gumbel (a, b)")) {
		double a = getParameter1Value();
		double b = getParameter2Value();
		result = gsl_ran_gumbel2_pdf(x, a, b);
	}
	else if (theDistributionType == QString("Frechet (a, s, m)")) {
		double a = getParameter1Value();
		double s = getParameter2Value();//Scale; > Wikipedia.
		double m = getParameter3Value();//Location of mininum; > Wikipedia.
		result = gsl_ran_gumbel2_pdf( (x - m) / s, a, 1) / s;
	}
	else if (theDistributionType == QString("Arcsine (a, b)")) {
		double a = getParameter1Value();//min value
		double b = getParameter2Value();//max value
		result = 1 / (b - a) * 1 / ( M_PI * sqrt( ((x - a) / (b - a)) * ((b - x) / (b - a)) ) );
	}
	else if (theDistributionType == QString("Birnbaum-Saunders (a, b, m)")) {
		double a = getParameter1Value();//shape parameter
		double b = getParameter2Value();//scale
		double m = getParameter3Value();//location of min
		double y = (x - m) / b;
		if (y <= 0) {
			result = 0;
		} else {
		result = 1 / b * ( (sqrt(y) + sqrt(1 / y)) / 2 / a / y ) *  gsl_ran_gaussian_pdf( ((sqrt(y) - sqrt(1 / y)) / a ) , 1);
		}
	}
	else if (theDistributionType == QString("Irwin-Hall (n, a, b)")) {
		unsigned int n = getParameter1Value();//The number of the refference uniform variables.
		double a = getParameter2Value();//The min of the refference uniform variables.
		double b = getParameter3Value();//The max of the refference uniform variables.
		double y = (x - n * a) / (b - a);
		double sum = 0;
		unsigned int k;
		for (k = 0; k <= floor (y); k++){
			sum  = sum + pow(-1.0, k * 1.0) * gsl_sf_choose(n, k) * pow( ( y - k), (n - 1) * 1.0 );
		}
		result = sum / gsl_sf_fact(n - 1) / (b - a);
	}
	else if (theDistributionType == QString("Wald (mu, lambda)")) {
		if (x <= 0) {
			result = 0;
		} else {
			double mu = getParameter1Value();
			double lambda = getParameter2Value();
			result = sqrt(lambda / 2 / M_PI / pow(x, 3)) * exp(-1 * lambda * pow(x - mu, 2) / 2 / pow(mu, 2) / x);
		}
	}
	else if (theDistributionType == QString("Levy (mu, c)")) {
		double mu = getParameter1Value();
		if (x <= mu) {
			result = 0;
		} else {
			double c = getParameter2Value();
			result = sqrt(c / 2 / M_PI) * exp(-1 * c / 2 / (x - mu)) / ((x - mu) * sqrt (x - mu));//pow(x - m, 3/2) does not work properly!
		}
	}
	else if (theDistributionType == QString("User Defined")) {
		if (userDefinedErrorCheck() < 0) {
			return 0.0;
		}
		result = -1.0;
		if (x < XVector[0]) {
			return 0.0;
		} else if (x > XVector.last()) {
			return 0.0;
		} else {
			for (int i = 1; i < XVector.count(); i++) {
				if (x <= XVector[i]) {
					double a = (PDFVector[i] - PDFVector[i-1]) / (XVector[i] - XVector[i-1]);
					result = (a * (x - XVector[i-1]) + PDFVector[i-1]);
					break;
				}
			}
			if (result < -1e-10) {
				qCritical() << "Error: The user defined distribution function in random variable" << objectName() << "seems to be invalid. The distribution is returned as 0.0.";
				return 0.0;
			}
		}
	}
	else {
		qCritical() << "Warning: Illegal probability distribution type has been passed to the random variable" << objectName() << ".";
	}

	return result;
}

double RContinuousRandomVariable::getCDFValue(double x)
{	
	double result;

	if (theStandardDeviationDouble == 0.0) {
		qCritical() << "WARNING: The standard deviation of the random variable" << objectName() << "is Zero.";
	}

	if (theDistributionType == QString("Normal (mean, stdv)")) {
		double mean = getParameter1Value();
		double stdv = getParameter2Value();
		result = gsl_cdf_gaussian_P(x-mean, stdv);
	}
	else if (theDistributionType == QString("Exponential (mu, xo)")) {
		double mu = getParameter1Value();
		double xo = getParameter2Value();
		result = gsl_cdf_exponential_P(x-xo,mu);
	}
	else if (theDistributionType == QString("Bounded Exponential (mu, xo, min, max)")) {
		double min = getParameter3Value();
		double max = getParameter4Value();
		if (x<min) {
			result = 0.0;
		}
		else if (x>max) {
			result = 1.0;
		}
		else {
			double mu = getParameter1Value();
			double xo = getParameter2Value();
			double k = 1.0 / (1.0 - exp(-mu*(max-min)) );
			result = k - k * exp(-mu*(x-xo-min));
		}
	}
	else if (theDistributionType == QString("Linear (a, b)")) {
		double min = getParameter1Value();
		double max = getParameter2Value();
		if (x<min) {
			result = 0.0;
		}
		else if (x>max) {
			result = 1.0;
		}
		else {
			result = (x*x - min*min) / (max*max - min*min);
		}
	}
	else if (theDistributionType == QString("Laplace (a, xo)")) {
		double a = getParameter1Value();
		double xo = getParameter2Value();
		result = gsl_cdf_laplace_P(x-xo,a);
	}
	else if (theDistributionType == QString("Rayleigh (sigma, xo)")) {
		double sigma = getParameter1Value();
		double xo = getParameter2Value();
		result = gsl_cdf_rayleigh_P(x-xo,sigma);
	}
	else if (theDistributionType == QString("Gamma (a, b)")) {
		double a = getParameter1Value();
		double b = getParameter2Value();
		result = gsl_cdf_gamma_P(x,a,b);
	}
	else if (theDistributionType == QString("Uniform (a, b)")) {
		double a = getParameter1Value();
		double b = getParameter2Value();
		result = gsl_cdf_flat_P(x,a,b);
	}
	else if (theDistributionType == QString("Lognormal (zeta, sigma)")) {
		double zeta = getParameter1Value();
		double sigma = getParameter2Value();
		result = gsl_cdf_lognormal_P(x,zeta,sigma);
	}
	else if (theDistributionType == QString("Chi-square (nu, xo)")) {
		double nu = getParameter1Value();
		double xo = getParameter2Value();
		result = gsl_cdf_chisq_P(x-xo,nu);
	}
	else if (theDistributionType == QString("t-distribution (nu, xo)")) {
		double nu = getParameter1Value();
		double xo = getParameter2Value();
		result = gsl_cdf_tdist_P(x-xo,nu);
	}
	else if (theDistributionType == QString("Beta (a, b, min, max)")) {
		double a = getParameter1Value();
		double b = getParameter2Value();
		double min = getParameter3Value();
		double max = getParameter4Value();
		result = gsl_cdf_beta_P((x-min)/(max-min),a,b);
	}
	else if (theDistributionType == QString("Logistic (a, xo)")) {
		double a = getParameter1Value();
		double xo = getParameter2Value();
		result = gsl_cdf_logistic_P(x-xo,a);
	}
	else if (theDistributionType == QString("Weibull (a, b)")) {
		double a = getParameter1Value();
		double b = getParameter2Value();
		result = gsl_cdf_weibull_P(x,a,b);
	}
	else if (theDistributionType == QString("Gumbel (a, b)")) {
		double a = getParameter1Value();
		double b = getParameter2Value();
		// Attention: Bug in GSL: Cannot use the GSL method.
		//result = gsl_cdf_gumbel1_P(x,a,b);
		result = exp(-b * exp(-a * x));
	}
	else if (theDistributionType == QString("Cauchy (a, xo)")) {
		double a = getParameter1Value();
		double xo = getParameter2Value();
		result = gsl_cdf_cauchy_P(x - xo, a);
	}
	else if (theDistributionType == QString("F-distribution (nu1, nu2)")) {
		double nu1 = getParameter1Value();
		double nu2 = getParameter2Value();
		result = gsl_cdf_fdist_P(x, nu1, nu2);
	}
	else if (theDistributionType == QString("Pareto (a, b)")) {
		double a = getParameter1Value();
		double b = getParameter2Value();
		result = gsl_cdf_pareto_P(x, a, b);
	}
	else if (theDistributionType == QString("Type-2 Gumbel (a, b)")) {
		double a = getParameter1Value();
		double b = getParameter2Value();
		//*The gsl gumble2 function has a bug, which calculates the CDF in negative x's similar to positive ones!
		if (x > 0) {
			result = gsl_cdf_gumbel2_P(x, a, b);
		} else {
			result = 0;
		}
	}
	else if (theDistributionType == QString("Frechet (a, s, m)")) {
		double a = getParameter1Value();
		double s = getParameter2Value();//Scale; > Wikipedia.
		double m = getParameter3Value();//Location of mininum; > Wikipedia.
		//*The gsl gumble2 function has a bug, which calculates the CDF in negative x's similar to positive ones!
		if ((x - m) / s >= 0){
			result = gsl_cdf_gumbel2_P( (x - m) / s, a, 1);
		} else {
			result = 0;
		}
	}
	else if (theDistributionType == QString("Arcsine (a, b)")) {
		double a = getParameter1Value();
		double b = getParameter2Value();
		result = asin( sqrt( (x - a) / (b - a) ) ) * 2 / M_PI;
	}
	else if (theDistributionType == QString("Birnbaum-Saunders (a, b, m)")) {
		double a = getParameter1Value();
		double b = getParameter2Value();
		double m = getParameter3Value();
		double y =(x - m) / b;
		result = gsl_cdf_gaussian_P(((sqrt(y) - sqrt(1 / y)) / a), 1);
	}
	else if (theDistributionType == QString("Irwin-Hall (n, a, b)")) {
		unsigned int n = getParameter1Value();//The number of the refference uniform variables.
		double a = getParameter2Value();//The min of the refference uniform variables.
		double b = getParameter3Value();//The max of the refference uniform variables.
		double y = (x - n * a) / (b - a);
		if (y >= n * b){
			result = 1;
		} else {
			double sum = 0;
			unsigned int k;
			for (k = 0; k <= floor (y); k++){
				sum  = sum + pow(-1.0, k * 1.0) * gsl_sf_choose(n, k) * pow( ( y - k), n * 1.0 );
			}
			result = sum / gsl_sf_fact(n);
		}
	}
	else if (theDistributionType == QString("Wald (mu, lambda)")) {
		if (x <= 0) {
			result = 0;
		} else {
			double mu = getParameter1Value();
			double lambda = getParameter2Value();
			result = gsl_cdf_gaussian_P((sqrt(lambda / x) * (x / mu - 1)), 1.0) + exp(2 * lambda / mu) * gsl_cdf_gaussian_P(-1 * (sqrt(lambda / x) * (x / mu + 1)) , 1.0);
		}
	}
	else if (theDistributionType == QString("Levy (mu, c)")) {
		double mu = getParameter1Value();
		if (x <= mu) {
			result = 0;
		} else {
			double c = getParameter2Value();
			double y = sqrt(c / 2 / (x - mu));
			result = gsl_sf_erfc(y);
		}
	}
	else if (theDistributionType == QString("User Defined")) {
		if (userDefinedErrorCheck() < 0) {
			return 0.0;
		}
		result = -1.0;
		if (x < XVector[0]) {
			return 0.0;
		} else if (x > XVector.last()) {
			return 1.0;
		} else {
			double sum = 0.0;
			for (int i = 1; i < XVector.count(); i++) {
				if (x <= XVector[i]) {
					result = (sum + 0.5 * (getPDFValue(x) + PDFVector[i-1]) * (x - XVector[i-1]));
					break;
				}
				sum += 0.5 * (PDFVector[i] + PDFVector[i-1]) * (XVector[i] - XVector[i-1]);
			}			
			if (result < -1e-10) {
				qCritical() << "Error: The user defined distribution function in random variable" << objectName() << "seems to be invalid. The distribution is returned as 0.0.";
				return 0.0;
			}
		}
	}
	else {
		qCritical() << "Warning: Illegal probability distribution type has been passed to the random variable" << objectName() << ".";
	}

	return result;
}

double RContinuousRandomVariable::getInverseCDFValue(double p)
{
	if (p < 0.0 || p > 1.0) {
		qCritical() << "Warning: Illegal probability value input has been passed to the random variable" << objectName() << ".";

		/*// Is this okay?		
		if (p < 0.0) {
			p = 0.0;
		} else if (p > 1.0) {
			p = 1.0;
		}*/
	}

	double result;

	if (theStandardDeviationDouble == 0.0) {
		qCritical() << "WARNING: The standard deviation of the random variable" << objectName() << "is Zero.";
	}

	if (theDistributionType == QString("Normal (mean, stdv)")) {
		double mean = getParameter1Value();
		double stdv = getParameter2Value();
		result = gsl_cdf_gaussian_Pinv(p,stdv)+mean;
	}
	else if (theDistributionType == QString("Exponential (mu, xo)")) {
		double mu = getParameter1Value();
		double xo = getParameter2Value();
		result = gsl_cdf_exponential_Pinv(p,mu)+xo;
	}
	else if (theDistributionType == QString("Bounded Exponential (mu, xo, min, max)")) {
		double mu = getParameter1Value();
		double xo = getParameter2Value();
		double min = getParameter3Value();
		double max = getParameter4Value();
		double k = 1.0 / (1.0 - exp(-mu*(max-min)) );
		result = (mu*min-log((k-p)/k))/mu + xo;
	}
	else if (theDistributionType == QString("Linear (a, b)")) {
		double min = getParameter1Value();
		double max = getParameter2Value();
		result = sqrt(p * (max*max - min*min) + min*min);
	}
	else if (theDistributionType == QString("Laplace (a, xo)")) {
		double a = getParameter1Value();
		double xo = getParameter2Value();
		result = gsl_cdf_laplace_Pinv(p,a)+xo;
	}
	else if (theDistributionType == QString("Rayleigh (sigma, xo)")) {
		double sigma = getParameter1Value();
		double xo = getParameter2Value();
		result = gsl_cdf_rayleigh_Pinv(p,sigma)+xo;
	}
	else if (theDistributionType == QString("Gamma (a, b)")) {
		double a = getParameter1Value();
		double b = getParameter2Value();
		result = gsl_cdf_gamma_Pinv(p,a,b);
	}
	else if (theDistributionType == QString("Uniform (a, b)")) {
		double a = getParameter1Value();
		double b = getParameter2Value();
		result = gsl_cdf_flat_Pinv(p,a,b);
	}
	else if (theDistributionType == QString("Lognormal (zeta, sigma)")) {
		double zeta = getParameter1Value();
		double sigma = getParameter2Value();
		result = gsl_cdf_lognormal_Pinv(p,zeta,sigma);
	}
	else if (theDistributionType == QString("Chi-square (nu, xo)")) {
		double nu = getParameter1Value();
		double xo = getParameter2Value();
		result = gsl_cdf_chisq_Pinv(p,nu)+xo;
	}
	else if (theDistributionType == QString("t-distribution (nu, xo)")) {
		double nu = getParameter1Value();
		double xo = getParameter2Value();
		result = gsl_cdf_tdist_Pinv(p,nu)+xo;
	}
	else if (theDistributionType == QString("Beta (a, b, min, max)")) {
		double a = getParameter1Value();
		double b = getParameter2Value();
		double min = getParameter3Value();
		double max = getParameter4Value();
		result = gsl_cdf_beta_Pinv(p,a,b) * (max-min) + min;
	}
	else if (theDistributionType == QString("Logistic (a, xo)")) {
		double a = getParameter1Value();
		double xo = getParameter2Value();
		result = gsl_cdf_logistic_Pinv(p,a)+xo;
	}
	else if (theDistributionType == QString("Weibull (a, b)")) {
		double a = getParameter1Value();
		double b = getParameter2Value();
		result = gsl_cdf_weibull_Pinv(p,a,b);
	}
	else if (theDistributionType == QString("Gumbel (a, b)")) {
		double a = getParameter1Value();
		double b = getParameter2Value();
		result = gsl_cdf_gumbel1_Pinv(p,a,b);
	}
	else if (theDistributionType == QString("Cauchy (a, xo)")) {
		double a = getParameter1Value();
		double xo = getParameter2Value();
		result = gsl_cdf_cauchy_Pinv(p,a) + xo;
	}
	else if (theDistributionType == QString("F-distribution (nu1, nu2)")) {
		double nu1 = getParameter1Value();
		double nu2 = getParameter2Value();
		result = gsl_cdf_fdist_Pinv(p,nu1,nu2);
	}
	else if (theDistributionType == QString("Pareto (a, b)")) {
		double a = getParameter1Value();
		double b = getParameter2Value();
		result = gsl_cdf_pareto_Pinv(p, a, b);
	}
	else if (theDistributionType == QString("Type-2 Gumbel (a, b)")) {
		double a = getParameter1Value();
		double b = getParameter2Value();
		result = gsl_cdf_gumbel2_Pinv(p, a, b);
	}
	else if (theDistributionType == QString("Frechet (a, s, m)")) {
		double a = getParameter1Value();
		double s = getParameter2Value();
		double m = getParameter3Value();
		result = gsl_cdf_gumbel2_Pinv(p, a, 1) * s + m;
	}
	else if (theDistributionType == QString("Arcsine (a, b)")) {
		double a = getParameter1Value();
		double b = getParameter2Value();
		result = (b - a) * pow( sin( M_PI / 2 * p) , 2) + a;
	}
	else if (theDistributionType == QString("Birnbaum-Saunders (a, b, m)")) {
		double a = getParameter1Value();
		double b = getParameter2Value();
		double m = getParameter3Value();
		double HP = 2 + pow((a * gsl_cdf_gaussian_Pinv(p,1)) , 2);
		if (p <= .5) {
			result = b * (HP - sqrt(pow(HP, 2) - 4) ) / 2 + m;
		}else{
			result = b * (HP + sqrt(pow(HP, 2) - 4) ) / 2 + m;
		}
	}
	else if (theDistributionType == QString("Irwin-Hall (n, a, b)")) {
		qCritical() << "Warinin: The inverse-CDF for the Irwin-Hall distribution is not available.";
	}
	else if (theDistributionType == QString("Wald (mu, lambda)")) {
		qCritical() << "Warinin: The inverse-CDF for the Wald distribution is not available.";
	}
	else if (theDistributionType == QString("Levy (mu, c)")) {
		double mu = getParameter1Value();
		double c = getParameter2Value();
		double y = gsl_cdf_gaussian_Pinv(p / 2 , 1);
		result = c / pow(y, 2) + mu;
	}
	else if (theDistributionType == QString("User Defined")) {
		if (userDefinedErrorCheck() < 0) {
			return 0.0;
		}
		if (p == 0.0) {
			result = XVector.first();
		} else if (p == 1.0) {
			result = XVector.last();
		} else {
			for (int i = 1; i < XVector.count(); i++) {
				if (getCDFValue(XVector[i]) > p) {
					double a = (getPDFValue(XVector[i]) - getPDFValue(XVector[i-1])) / (XVector[i] - XVector[i-1]);
					if (a == 0.0 && getPDFValue(XVector[i-1]) == 0.0) {
						qCritical() << "Error: The user defined distribution function in random variable" << objectName() << "has a region of constant zero PDF. The distribution is returned as 0.0.";
						return 0.0;
					}
					else if (a == 0.0) {
						result = (p - getCDFValue(XVector[i-1]) + getPDFValue(XVector[i-1]) * XVector[i-1]) / getPDFValue(XVector[i-1]);
					}
					else {
						double AA = 0.5 * a;
						double BB = getPDFValue(XVector[i-1]);
						double CC = getCDFValue(XVector[i-1]) - p;
						double deltaX = (-BB + sqrt(BB * BB - 4.0 * AA * CC)) / (2.0 * AA);
						result = deltaX + (XVector[i-1]);
					}
					break;
				}
			}
		}
	}
	else {
		qCritical() << "Warning: Illegal probability distribution type has been passed to the random variable" << objectName() << ".";
	}

	return result;
}

int RContinuousRandomVariable::updateParameters()
{
	// Initial declarations
	double parameter1 = 0.0;
	double parameter2 = 0.0;
	double parameter3 = 0.0;
	double parameter4 = 0.0;
	double mean = getMeanValue();
	double stdv = getStandardDeviationValue();
	double cov = qAbs(stdv / (mean));

	// Compute parameter values
	if (theDistributionType == QString("Normal (mean, stdv)")) {
		parameter1 = mean;
		parameter2 = stdv;
	}
	else if (theDistributionType == QString("Exponential (mu, xo)")) {
		parameter1 = stdv;
		parameter2 = mean-parameter1;
	}
	else if (theDistributionType == QString("Bounded Exponential (mu, xo, min, max)")) {
		//qCritical() << "Warning: Moments cannot be given for the" << theDistributionType << "distribution. Please give parameter values only.";
	}
	else if (theDistributionType == QString("Laplace (a, xo)")) {
		parameter2 = mean;
		parameter1 = stdv/sqrt(2.0);
	}
	else if (theDistributionType == QString("Linear (a, b)")) {
		//qCritical() << "Warning: Moments cannot be given for the" << theDistributionType << "distribution. Please give parameter values only.";
	}
	else if (theDistributionType == QString("Rayleigh (sigma, xo)")) {
		parameter1 = stdv/sqrt((4.0-M_PI)/2.0);
		parameter2 = mean - parameter1 * sqrt(M_PI/2.0);
	}
	else if (theDistributionType == QString("Gamma (a, b)")) {
		parameter1 = gsl_pow_2(mean/stdv);
		parameter2 = stdv*stdv / mean;
	}
	else if (theDistributionType == QString("Uniform (a, b)")) {
		parameter1 = mean-sqrt(3.0)*stdv;
		parameter2 = mean+sqrt(3.0)*stdv;
	}
	else if (theDistributionType == QString("Lognormal (zeta, sigma)")) {
		parameter1 = log(mean)-0.5*log(1+gsl_pow_2(stdv/mean));
		parameter2 = sqrt(log(gsl_pow_2(stdv / mean) + 1));
	}
	else if (theDistributionType == QString("Chi-square (nu, xo)")) {
		parameter1 = 0.5*stdv*stdv;
		parameter2 = mean - parameter1;
	}
	else if (theDistributionType == QString("t-distribution (nu, xo)")) {
		parameter2 = mean;
		parameter1 = 2*stdv*stdv/(stdv*stdv-1);
	}
	else if (theDistributionType == QString("Beta (a, b, min, max)")) {
		//qCritical() << "Warning: Moments cannot be given for the Beta distribution. Please give parameter values only.";
		parameter3 = getParameter3Value();
		parameter4 = getParameter4Value();
		double min = parameter3;
		double max = parameter4;
		double temp = (mean - min) * (max - mean) / (stdv * stdv) - 1.0;
		parameter1 = (mean - min) / (max - min) * temp;
		parameter2 = (max - mean) / (max - min) * temp;
	}
	else if (theDistributionType == QString("Logistic (a, xo)")) {
		parameter1 = stdv*sqrt(3.0)/M_PI;
		parameter2 = mean-parameter1;
	}
	else if (theDistributionType == QString("Weibull (a, b)")) {
		qCritical() << "Warning: Moments cannot be given for the Weibull distribution. Please give parameter values only.";
	}
	else if (theDistributionType == QString("Gumbel (a, b)")) {
		parameter1 = M_PI / (stdv * sqrt(6.0));
		parameter2 = exp(parameter1 * mean - M_EULER);
	}
	else if (theDistributionType == QString("Cauchy (a, xo)")) {
		qCritical() << "Warning: Standard deviation cannot be given for the Cauchy distribution. Please give mean or parameter values only.";
		parameter2 = mean;
	}
	else if (theDistributionType == QString("F-distribution (nu1, nu2)")) {
		parameter1 = (2 * pow(mean,2))/(pow(mean,2) - pow(mean,3) + 2* pow(stdv,2) - mean * pow(stdv,2));	
		parameter2 = 2 * mean / (mean - 1);
		if (parameter1 < 0 || parameter2 < 0){
			qCritical() << "Warning: The moments might be wrong. The F-distributions parameters cannot be negative.";
		}
	}
	else if (theDistributionType == QString("Pareto (a, b)")) {
		parameter1 = (stdv + sqrt(pow(mean,2) + pow(stdv,2))) / stdv;
		parameter2 = (pow(mean,2) + pow(stdv,2) - stdv * sqrt(pow(mean,2) + pow(stdv,2))) / mean;
	}
	else if (theDistributionType == QString("Type-2 Gumbel (a, b)")) {
		qCritical() << "Warning: Moments cannot be given for the Type-2 Gumbel distribution. Please give parameter values only.";
	}
	else if (theDistributionType == QString("Frechet (a, s, m)")) {
		qCritical() << "Warning: Moments cannot be given for the Frechet distribution. Please give parameter values only.";
	}
	else if (theDistributionType == QString("Arcsine (a, b)")) {
		parameter1 = mean - sqrt(2.0) * stdv;
		parameter2 = mean + sqrt(2.0) * stdv;
	}
	else if (theDistributionType == QString("Birnbaum-Saunders (a, b, m)")) {
		qCritical() << "Warning: Moments cannot be given for the Birnbaum-Saunders distribution. Please give parameter values only.";
	}
	else if (theDistributionType == QString("Irwin-Hall (n, a, b)")) {
		//Here, it is assumed that "n" remains constant. 
		parameter1 = getParameter1Value();
		parameter2 = (2 * mean / parameter1 - sqrt(12 / parameter1) * stdv) / 2;
		parameter3 = (2 * mean / parameter1 + sqrt(12 / parameter1) * stdv) / 2;
	}
	else if (theDistributionType == QString("Wald (mu, lambda)")) {
		parameter1 = mean;
		parameter2 = pow(mean, 3) / pow(stdv, 2);
	}
	else if (theDistributionType == QString("Levy (mu, c)")) {
		qCritical() << "Warning: Moments cannot be given for the Levy distribution. Please give parameter values only.";
	}
	else if (theDistributionType == QString("User Defined")) {

	}
	else {
		qCritical() << "Warning: Illegal probability distribution type has been passed to the random variable" << objectName() << ".";
	}

	// Update the parameter values; either the Double or the Parameter
	if (theCoefficientOfVariationParameter == 0) {
		theCoefficientOfVariationDouble = cov;
	}
	else {
		theCoefficientOfVariationParameter->setCurrentValue(cov);
	}

	if (theParameter1Parameter == 0) {
		theParameter1Double = parameter1;
	}
	else {
		theParameter1Parameter->setCurrentValue(parameter1);
	}

	if (theParameter2Parameter == 0) {
		theParameter2Double = parameter2;
	}
	else {
		theParameter2Parameter->setCurrentValue(parameter2);
	}

	if (theParameter3Parameter == 0) {
		theParameter3Double = parameter3;
	}
	else {
		theParameter3Parameter->setCurrentValue(parameter3);
	}

	if (theParameter4Parameter == 0) {
		theParameter4Double = parameter4;
	}
	else {
		theParameter4Parameter->setCurrentValue(parameter4);
	}

	return 0;
}

int RContinuousRandomVariable::updateMeanStdvByCOV()
{
	double mean = getMeanValue();
	double cov = getCoefficientOfVariationValue();
	double stdv = qAbs((mean) * cov);

	if (theStandardDeviationParameter == 0) {
		theStandardDeviationDouble = stdv;
	}
	else {
		theStandardDeviationParameter->setCurrentValue(stdv);
	}

	return 0;
}

int RContinuousRandomVariable::updateMeanStdv()
{
	// Initial declarations
	double mean = 0.0;
	double stdv = 0.0;
	double parameter1 = getParameter1Value();
	double parameter2 = getParameter2Value();
	double parameter3 = getParameter3Value();
	double parameter4 = getParameter4Value();

	if (theDistributionType == QString("Normal (mean, stdv)")) {
		mean = parameter1;
		stdv = parameter2;
	}
	else if (theDistributionType == QString("Exponential (mu, xo)")) {
		mean = parameter1+parameter2;
		stdv = parameter1;
	}
	else if (theDistributionType == QString("Bounded Exponential (mu, xo, min, max)")) {
		double mu = parameter1;
		double xo = parameter2;
		double min = parameter3;
		double max = parameter4;
		double k = 1.0 / (1.0 - exp(-mu*(max-min)) );
		mean = -k*(mu*max+1.0)/mu * exp(-mu*(max-min)) + (1+mu*min)/(mu)*k;
		stdv = sqrt( (-exp(-mu*(max-min))) * (mu*mu*max*max+2.0*mu*max+2.0-2.0*mean*mu*mu*max-2*mean*mu+mean*mean*mu*mu)*k/(mu*mu)  +  (mu*mu*min*min+2.0*mu*min+2.0-2.0*min*mean*mu*mu-2.0*mean*mu+mean*mean*mu*mu)*k/(mu*mu) );
		mean += xo;
	}
	else if (theDistributionType == QString("Linear (a, b)")) {
		double min = parameter1;
		double max = parameter2;
		mean = 2.0 / 3.0 * (min*min + min*max + max*max) / (min + max);
		stdv = sqrt((min*min + max*max) / 2.0 - mean*mean);
	}
	else if (theDistributionType == QString("Laplace (a, xo)")) {
		mean = parameter2;
		stdv = sqrt(2.0)*parameter1;
	}
	else if (theDistributionType == QString("Rayleigh (sigma, xo)")) {
		mean = parameter1 * sqrt(M_PI/2.0) + parameter2;
		stdv = parameter1*sqrt((4-M_PI)/2.0); 
	}
	else if (theDistributionType == QString("Gamma (a, b)")) {
		mean = parameter1*parameter2;
		stdv = sqrt(parameter1)*parameter2;
	}
	else if (theDistributionType == QString("Uniform (a, b)")) {
		mean = (parameter1+parameter2)/2.0;
		stdv = (parameter2-parameter1)/sqrt(12.0);
	}
	else if (theDistributionType == QString("Lognormal (zeta, sigma)")) {
		mean = exp(parameter1+0.5*parameter2*parameter2);
		stdv = sqrt((exp(parameter2*parameter2)-1))*exp(parameter1+0.5*parameter2*parameter2);
	}
	else if (theDistributionType == QString("Chi-square (nu, xo)")) {
		mean = parameter1+parameter2;
		stdv = sqrt(2.0*parameter1);
	}
	else if (theDistributionType == QString("t-distribution (nu, xo)")) {
		mean = parameter2;
		if (parameter1<=2.0) {
			qCritical() << "Warning: The standard deviation is not defined for the t-distribution for nu<=2.0. Stdv not set.";
		}
		else {
			stdv = sqrt(parameter1/(parameter1-2.0));
		}
	}
	else if (theDistributionType == QString("Beta (a, b, min, max)")) {
		double a = parameter1;
		double b = parameter2;
		double min = parameter3;
		double max = parameter4;
		mean = (a/(a+b))*(max-min)+min;
		stdv = ( 1/(a+b) * sqrt( a*b/(a+b+1) ) ) *(max-min);
	}
	else if (theDistributionType == QString("Logistic (a, xo)")) {
		mean = parameter1+parameter2;
		stdv = parameter1*M_PI/sqrt(3.0);
	}
	else if (theDistributionType == QString("Weibull (a, b)")) {
		mean = parameter1*gsl_sf_gamma(1.0+1.0/parameter2);
		stdv = parameter1* sqrt(gsl_sf_gamma(1+2.0/parameter2) - gsl_pow_2(gsl_sf_gamma(1+1.0/parameter2)));
	}
	else if (theDistributionType == QString("Gumbel (a, b)")) {
		mean = (log(parameter2) + M_EULER) / parameter1;
		stdv = M_PI / (parameter1 * sqrt(6.0));
	}
	else if (theDistributionType == QString("Cauchy (a, xo)")) {
		if (parameter1 <= 0){
			qCritical() << "Warning: The parameter1 must be grater than zero in the Cauchy distribution .";
		}
		mean = parameter2;
		stdv = std::numeric_limits<double>::quiet_NaN();
	}
	else if (theDistributionType == QString("F-distribution (nu1, nu2)")) {
		//For nu2>2 ; nu2>4.
		if (parameter2 <= 2){
			qCritical() << "Warning: The mean is undefined for nu2 less than or equal to 2 in the F-distribution.";
			mean = std::numeric_limits<double>::quiet_NaN();
		} else {
			mean = parameter2 / (parameter2 - 2);
		}
		if (parameter2 <= 4){
			qCritical() << "Warning: The standard deviation is undefined for nu2 less than or equal to 4 in the F-distribution.";
			stdv = std::numeric_limits<double>::quiet_NaN();
		} else {
			stdv = sqrt(2 * pow(parameter2 , 2) * (parameter1 + parameter2 - 2) / parameter1 / pow(parameter2 - 2 , 2) / (parameter2 - 4)) ;
		}
	}
	else if (theDistributionType == QString("Pareto (a, b)")) {
		if (parameter1 <= 0 || parameter2 <= 0){
			qCritical() << "Warning: The Pareto distribution parameters must be grater than zero.";
		}
		if (parameter1 <= 1) {
			mean = std::numeric_limits<double>::infinity();
		}else{
			mean = (parameter1 * parameter2) / (parameter1 - 1);
		}
		if (parameter1 > 0 && parameter1 <=2) {
			stdv = std::numeric_limits<double>::infinity();
		} else {
		stdv = sqrt( (pow(parameter2 , 2) * parameter1) / ( pow(parameter1 - 1 , 2) * (parameter1 - 2) ) );
		}
	}
	else if (theDistributionType == QString("Type-2 Gumbel (a, b)")) {
		if (parameter1 > 1){
			mean = gsl_sf_gamma(1 - 1 / parameter1);
		} else {
			mean = std::numeric_limits<double>::infinity();
		}
		if (parameter1 > 2){
			stdv = sqrt( gsl_sf_gamma(1 - 2 / parameter1) - pow( gsl_sf_gamma(1 - 1 / parameter1) , 2) );
		} else {
			stdv = std::numeric_limits<double>::infinity();
		}
	}
	else if (theDistributionType == QString("Frechet (a, s, m)")) {
		if (parameter1 > 1){
			mean = parameter3 + parameter2 * gsl_sf_gamma(1 - 1 / parameter1);
		} else {
			mean = std::numeric_limits<double>::infinity();
		}
		if (parameter1 > 2){
			stdv = parameter2 * sqrt( gsl_sf_gamma(1 - 2 / parameter1) - pow( gsl_sf_gamma(1 - 1 / parameter1) , 2) );
		} else {
			stdv = std::numeric_limits<double>::infinity();
		}
	}
	else if (theDistributionType == QString("Arcsine (a, b)")) {
		mean = (parameter1 + parameter2) / 2 ;
		stdv = (parameter2 - parameter1) / sqrt(8.0);
	}
	else if (theDistributionType == QString("Birnbaum-Saunders (a, b, m)")) {
		if (parameter1 <=0 || parameter2 <=0){
			qCritical() << "Warning: The parameters 1 and 2 in the Birnbaum-Saunders distribution must be grater than zero."; 
		}
		mean = parameter2 * (1 + pow(parameter1, 2) / 2) + parameter3;
		stdv = parameter2 * parameter1 * sqrt(4 + 5 * pow(parameter1, 2)) / 2;
	}
	else if (theDistributionType == QString("Irwin-Hall (n, a, b)")) {
		if (parameter1 <= 0){
			qCritical() << "Warning: The parameter1 in the Irwin-Hall distribution must be grater than zero.";
		} else {
			mean = (parameter3 + parameter2) * parameter1 / 2;
			stdv = (parameter3 - parameter2) * sqrt(parameter1 / 12);
		}
	}
	else if (theDistributionType == QString("Wald (mu, lambda)")) {
		if (parameter1 <= 0 || parameter2 <= 0){
			qCritical() << "Warning: The Wald distribution parameters must be grater than zero.";
		} else {
			mean = parameter1; 
			stdv = sqrt( pow(parameter1, 3) / parameter2);
		}
	}
	else if (theDistributionType == QString("Levy (mu, c)")) {
		if (parameter2 <= 0){
			qCritical() << "Warning: The Levy distribution parameter2 must be grater than zero.";
		} else {
			mean = std::numeric_limits<double>::infinity();
			stdv = std::numeric_limits<double>::infinity();
		}
	}
	else if (theDistributionType == QString("User Defined")) {
		//if (userDefinedErrorCheck() < 0) {
		//	return -1;
		//}

		for (int i = 1; i < XVector.count(); i++) {
			double a = (PDFVector[i] - PDFVector[i-1]) / (XVector[i] - XVector[i-1]);
			double b = PDFVector[i-1] - a * XVector[i-1];
			mean += a * (gsl_pow_3(XVector[i]) - gsl_pow_3(XVector[i-1])) / 3.0 +  0.5 * b * (gsl_pow_2(XVector[i]) - gsl_pow_2(XVector[i-1]));
		}

		for (int i = 1; i < XVector.count(); i++) {
			double a = (PDFVector[i] - PDFVector[i-1]) / (XVector[i] - XVector[i-1]);
			double b = PDFVector[i-1] - a * XVector[i-1];
			double x1 = XVector[i-1];
			double x2 = XVector[i];
			stdv += 0.25 * a * (gsl_pow_4(x2) - gsl_pow_4(x1)) + (b / 3.0 - 2.0 / 3.0 * mean * a) * (gsl_pow_3(x2) - gsl_pow_3(x1)) + (0.5 * a * gsl_pow_2(mean) - mean * b) * (gsl_pow_2(x2) - gsl_pow_2(x1)) + gsl_pow_2(mean) * b * (x2 - x1);
		}
		stdv =  sqrt(stdv);		
	}
	else {
		qCritical() << "Warning: Illegal probability distribution type has been passed to the random variable" << objectName() << ".";
	}

	double cov = qAbs(stdv / (mean));
	
	// Update the parameter values; either the Double or the Parameter
	if (theMeanParameter == 0) {
		theMeanDouble = mean;
	}
	else {
		theMeanParameter->setCurrentValue(mean);
	}

	if (theStandardDeviationParameter == 0) {
		theStandardDeviationDouble = stdv;
	}
	else {
		theStandardDeviationParameter->setCurrentValue(stdv);
	}

	if (theCoefficientOfVariationParameter == 0) {
		theCoefficientOfVariationDouble = cov;
	}
	else {
		theCoefficientOfVariationParameter->setCurrentValue(cov);
	}

	return 0;
}

void RContinuousRandomVariable::setMeanValue(double value)
{
	if (theMeanParameter == 0) {
		theMeanDouble = value;
	}
	else {
		theMeanParameter->setCurrentValue(value);
	}
	updateParameters();
}

void RContinuousRandomVariable::setStandardDeviationValue(double value)
{
	if (theStandardDeviationParameter == 0) {
		theStandardDeviationDouble = value;
	}
	else {
		theStandardDeviationParameter->setCurrentValue(value);
	}
	updateParameters();
}

void RContinuousRandomVariable::setParameter1Value(double value)
{
	if (theParameter1Parameter == 0) {
		theParameter1Double = value;
	}
	else {
		theParameter1Parameter->setCurrentValue(value);
	}
	updateParameters();
}

void RContinuousRandomVariable::setParameter2Value(double value)
{
	if (theParameter2Parameter == 0) {
		theParameter2Double = value;
	}
	else {
		theParameter2Parameter->setCurrentValue(value);
	}
	updateParameters();
}

void RContinuousRandomVariable::setParameter3Value(double value)
{
	if (theParameter3Parameter == 0) {
		theParameter3Double = value;
	}
	else {
		theParameter3Parameter->setCurrentValue(value);
	}
	updateParameters();
}

void RContinuousRandomVariable::setParameter4Value(double value)
{
	if (theParameter4Parameter == 0) {
		theParameter4Double = value;
	}
	else {
		theParameter4Parameter->setCurrentValue(value);
	}
	updateParameters();
}

int RContinuousRandomVariable::computeCDFSensitivityToParameters(double rvValue, double *dCDFdMean, double *dCDFdStdv)
{
	double meanBackup = getMeanValue();
	double stdvBackup = getStandardDeviationValue();
	//double covBackup = getCoefficientOfVariationValue();
	//double parameter1Backup = getParameter1Value();
	//double parameter2Backup = getParameter2Value();
	//double parameter3Backup = getParameter3Value();
	//double parameter4Backup = getParameter4Value();
	double currentValueBackup = getCurrentValue();

	if (theDistributionType == QString("Normal (mean, stdv)") || theDistributionType == QString("Exponential (mu, xo)") || theDistributionType == QString("Laplace (a, xo)") || theDistributionType == QString("Rayleigh (sigma, xo)") || theDistributionType == QString("Gamma (a, b)") || theDistributionType == QString("Uniform (a, b)") || theDistributionType == QString("Lognormal (zeta, sigma)") || theDistributionType == QString("Chi-square (nu, xo)") || theDistributionType == QString("Beta (a, b, min, max)") || theDistributionType == QString("Logistic (a, xo)") || theDistributionType == QString("Gumbel (a, b)") || theDistributionType == QString("F-distribution (nu1, nu2)") || theDistributionType == QString("Pareto (a, b)") || theDistributionType == QString("Type-2 Gumbel (a, b)") || theDistributionType == QString("Frechet (a, s, m)") || theDistributionType == QString("Arcsine (a, b)") || theDistributionType == QString("Birnbaum-Saunders (a, b, m)") || theDistributionType == QString("Wald (mu, lambda)") || theDistributionType == QString("Levy (mu, c)") ) {
		double originalCDF = getCDFValue(rvValue);
		
		// Perturb the mean
		double mean = meanBackup;
		double h = stdvBackup / 200.0;
		mean += h;
		setMeanValue(mean);

		// Compute the sensitivity with finite difference
		double perturbedCDF = getCDFValue(rvValue);
		*dCDFdMean = (perturbedCDF - originalCDF) / h;
		
		// Restore the mean
		mean = meanBackup;
		setMeanValue(mean);
		setCurrentValue(currentValueBackup);



		// Perturb the stdv
		double stdv = stdvBackup;
		stdv += h;
		setStandardDeviationValue(stdv);
		
		// Compute the sensitivity with finite difference
		perturbedCDF = getCDFValue(rvValue);
		*dCDFdStdv = (perturbedCDF - originalCDF) / h;
		stdv = stdvBackup;
		
		// Restore the stdv
		setStandardDeviationValue(stdv);
		setCurrentValue(currentValueBackup);
	}
	else if (theDistributionType == QString("Bounded Exponential (mu, xo, min, max)") || theDistributionType == QString("Linear (a, b)") || theDistributionType == QString("Weibull (a, b)") || theDistributionType == QString("Cauchy (a, xo)") || theDistributionType == QString("Irwin-Hall (n, a, b)") || theDistributionType == QString("User Defined") ) {
		qCritical() << "Warning: The sensitivity of the CDF with respect to the mean and the standard deviation of the random variable" << objectName() << "cannot be computed. Setting the sensitivity to zero.";
		*dCDFdMean = 0.0;
		*dCDFdStdv = 0.0;
	}	
	else {
		qCritical() << "Warning: Illegal probability distribution type has been passed to the random variable" << objectName() << ".";
		*dCDFdMean = 0.0;
		*dCDFdStdv = 0.0;
	}

	return 0;
}
int RContinuousRandomVariable::computePDFSensitivityToParameters(double rvValue, double *dPDFdMean, double *dPDFdStdv)
{
	double meanBackup = getMeanValue();
	double stdvBackup = getStandardDeviationValue();
	double currentValueBackup = getCurrentValue();
	double Parameter1Backup = getParameter1Value();
	double Parameter2Backup = getParameter2Value();
	double Parameter3Backup = getParameter3Value();
	double Parameter4Backup = getParameter4Value();

	if (theDistributionType == QString("Normal (mean, stdv)") || theDistributionType == QString("Exponential (mu, xo)") || theDistributionType == QString("Laplace (a, xo)") || theDistributionType == QString("Rayleigh (sigma, xo)") || theDistributionType == QString("Gamma (a, b)") || theDistributionType == QString("Uniform (a, b)") || theDistributionType == QString("Lognormal (zeta, sigma)") || theDistributionType == QString("Chi-square (nu, xo)")  || theDistributionType == QString("Logistic (a, xo)") || theDistributionType == QString("Gumbel (a, b)") || theDistributionType == QString("F-distribution (nu1, nu2)") || theDistributionType == QString("Pareto (a, b)") || theDistributionType == QString("Type-2 Gumbel (a, b)") || theDistributionType == QString("Frechet (a, s, m)") || theDistributionType == QString("Arcsine (a, b)") || theDistributionType == QString("Birnbaum-Saunders (a, b, m)") || theDistributionType == QString("Wald (mu, lambda)") || theDistributionType == QString("Levy (mu, c)") ) {
		double originalPDF = getPDFValue(rvValue);
		
		// Perturb the mean
		double mean = meanBackup;
		double h = stdvBackup / 200.0;
		mean += h;
		setMeanValue(mean);

		// Compute the sensitivity with finite difference
		double perturbedPDF = getPDFValue(rvValue);
		*dPDFdMean = (perturbedPDF - originalPDF) / h;
		
		// Restore the mean
		mean = meanBackup;
		setMeanValue(mean);
		setCurrentValue(currentValueBackup);



		// Perturb the stdv
		double stdv = stdvBackup;
		stdv += h;
		setStandardDeviationValue(stdv);
		
		// Compute the sensitivity with finite difference
		perturbedPDF = getPDFValue(rvValue);
		*dPDFdStdv = (perturbedPDF - originalPDF) / h;
		stdv = stdvBackup;
		
		// Restore the stdv
		setStandardDeviationValue(stdv);
		setCurrentValue(currentValueBackup);
	}
	else if(theDistributionType == QString("Beta (a, b, min, max)")){
		double originalPDF = getPDFValue(rvValue);
		
		// Perturb the paramtere 3 and 4
		double mean = meanBackup;
		double h = stdvBackup / 200.0;
		double par1 = Parameter1Backup;
		double par2 = Parameter2Backup;
		double par3 = Parameter3Backup;
		double par4 = Parameter4Backup;
		mean += h;
		setMeanValue(mean);
		par3 += h;
		setParameter3Value(par3);
		par4 += h;
		setParameter4Value(par4);

		// Compute the sensitivity with finite difference
		double perturbedPDF = getPDFValue(rvValue);
		*dPDFdMean = (perturbedPDF - originalPDF) / h;
		
		// Restore the mean
		mean = meanBackup;
		par3 = Parameter3Backup;
		par4 = Parameter4Backup;
		setParameter3Value(par3);
		setParameter4Value(par4);
		setMeanValue(mean);
		setCurrentValue(currentValueBackup);



		// Perturb the stdv
		double stdv = stdvBackup;
		stdv += h;
		setStandardDeviationValue(stdv);
		
		// Compute the sensitivity with finite difference
		perturbedPDF = getPDFValue(rvValue);
		*dPDFdStdv = (perturbedPDF - originalPDF) / h;
		stdv = stdvBackup;
		
		// Restore the stdv
		setStandardDeviationValue(stdv);
		setCurrentValue(currentValueBackup);
	}
	else if (theDistributionType == QString("Bounded Exponential (mu, xo, min, max)") || theDistributionType == QString("Linear (a, b)") || theDistributionType == QString("Weibull (a, b)") || theDistributionType == QString("Cauchy (a, xo)") || theDistributionType == QString("Irwin-Hall (n, a, b)") || theDistributionType == QString("User Defined")) {
		qCritical() << "Warning: The sensitivity of the PDF with respect to the mean and the standard deviation of the random variable" << objectName() << "cannot be computed. Setting the sensitivity to zero.";
		*dPDFdMean = 0.0;
		*dPDFdStdv = 0.0;
	}	
	else {
		qCritical() << "Warning: Illegal probability distribution type has been passed to the random variable" << objectName() << ".";
		*dPDFdMean = 0.0;
		*dPDFdStdv = 0.0;
	}

	return 0;
}
int RContinuousRandomVariable::computeInverseCDFSensitivityToParameters(double probValue, double *dInverseCDFdMean, double *dInverseCDFdStdv)
{
	double meanBackup = getMeanValue();
	double stdvBackup = getStandardDeviationValue();
	double currentValueBackup = getCurrentValue();
	double Parameter1Backup = getParameter1Value();
	double Parameter2Backup = getParameter2Value();
	double Parameter3Backup = getParameter3Value();
	double Parameter4Backup = getParameter4Value();

	if (theDistributionType == QString("Normal (mean, stdv)") || theDistributionType == QString("Exponential (mu, xo)") || theDistributionType == QString("Laplace (a, xo)") || theDistributionType == QString("Rayleigh (sigma, xo)") || theDistributionType == QString("Gamma (a, b)") || theDistributionType == QString("Uniform (a, b)") || theDistributionType == QString("Lognormal (zeta, sigma)") || theDistributionType == QString("Chi-square (nu, xo)")  || theDistributionType == QString("Logistic (a, xo)") || theDistributionType == QString("Gumbel (a, b)") || theDistributionType == QString("F-distribution (nu1, nu2)") || theDistributionType == QString("Pareto (a, b)") || theDistributionType == QString("Type-2 Gumbel (a, b)") || theDistributionType == QString("Frechet (a, s, m)") || theDistributionType == QString("Arcsine (a, b)") || theDistributionType == QString("Birnbaum-Saunders (a, b, m)") ||  theDistributionType == QString("Levy (mu, c)") ){
		double originalInverseCDF = getInverseCDFValue(probValue);
		
		// Perturb the mean
		double mean = meanBackup;
		double h = stdvBackup / 200.0;
		mean += h;
		setMeanValue(mean);

		// Compute the sensitivity with finite difference
		double perturbedInverseCDF = getInverseCDFValue(probValue);
		*dInverseCDFdMean = (perturbedInverseCDF - originalInverseCDF) / h;
		
		// Restore the mean
		mean = meanBackup;
		setMeanValue(mean);
		setCurrentValue(currentValueBackup);



		// Perturb the stdv
		double stdv = stdvBackup;
		stdv += h;
		setStandardDeviationValue(stdv);
		
		// Compute the sensitivity with finite difference
		perturbedInverseCDF = getInverseCDFValue(probValue);
		*dInverseCDFdStdv = (perturbedInverseCDF - originalInverseCDF) / h;
		stdv = stdvBackup;
		
		// Restore the stdv
		setStandardDeviationValue(stdv);
		setCurrentValue(currentValueBackup);
	} else if (theDistributionType == QString("Beta (a, b, min, max)")) {
		double originalInverseCDF = getInverseCDFValue(probValue);
		
		// Perturb the paramtere 3 and 4
		double mean = meanBackup;
		double h = stdvBackup / 200.0;
		double par1 = Parameter1Backup;
		double par2 = Parameter2Backup;
		double par3 = Parameter3Backup;
		double par4 = Parameter4Backup;

		// For Beta distribution, if moments can not are set other parameters change. So, to keep stdv, a and b constant and change the mean as much as h, the following is done:
		mean += h;
		setMeanValue(mean);
		par3 += h;
		setParameter3Value(par3);
		par4 += h;
		setParameter4Value(par4);
		// Compute the sensitivity with finite difference
		double perturbedInverseCDF = getInverseCDFValue(probValue);
		*dInverseCDFdMean = (perturbedInverseCDF - originalInverseCDF) / h;
		
		// Restore the mean
		mean = meanBackup;
		setMeanValue(mean);
		par3 = Parameter3Backup;
		par4 = Parameter4Backup;
		setParameter3Value(par3);
		setParameter4Value(par4);
		setCurrentValue(currentValueBackup);



		// Perturb the stdv
		double stdv = stdvBackup;

		// For Beta distribution, if moments can not are set other parameters change. So, to keep mean, a and b constant and change the stdv as much as h, the following is done:
		double temp = h / (1.0/(par1 + par2) * sqrt(par1*par2 / (1 + par1 + par2)));
		stdv += h;
		setStandardDeviationValue(stdv);
		par3 -= temp / 2.0;
		setParameter3Value(par3);
		par4 += temp / 2.0;;
		setParameter4Value(par4);
		
		// Compute the sensitivity with finite difference
		perturbedInverseCDF = getInverseCDFValue(probValue);
		*dInverseCDFdStdv = (perturbedInverseCDF - originalInverseCDF) / h;

		// Restore the stdv
		stdv = stdvBackup;
		setStandardDeviationValue(stdv);
		par3 = Parameter3Backup;
		par4 = Parameter4Backup;
		setParameter3Value(par3);
		setParameter4Value(par4);
		setCurrentValue(currentValueBackup);
	}
	else if (theDistributionType == QString("Bounded Exponential (mu, xo, min, max)") || theDistributionType == QString("Linear (a, b)") || theDistributionType == QString("Weibull (a, b)") || theDistributionType == QString("Cauchy (a, xo)") || theDistributionType == QString("Irwin-Hall (n, a, b)") || theDistributionType == QString("Wald (mu, lambda)") || theDistributionType == QString("User Defined")) {
		qCritical() << "Warning: The sensitivity of the InverseCDF with respect to the mean and the standard deviation of the random variable" << objectName() << "cannot be computed. Setting the sensitivity to zero.";
		*dInverseCDFdMean = 0.0;
		*dInverseCDFdStdv = 0.0;
	}	
	else {
		qCritical() << "Warning: Illegal probability distribution type has been passed to the random variable" << objectName() << ".";
		*dInverseCDFdMean = 0.0;
		*dInverseCDFdStdv = 0.0;
	}

	return 0;
}


QList<RParameter *> RContinuousRandomVariable::getParameterList()
{
	QList<RParameter *> theParameterList;
	if (theMeanParameter) {
		theParameterList << theMeanParameter;
	}
	if (theStandardDeviationParameter) {
		theParameterList << theStandardDeviationParameter;
	}
	if (theCoefficientOfVariationParameter) {
		theParameterList << theCoefficientOfVariationParameter;
	}
	if (theParameter1Parameter) {
		theParameterList << theParameter1Parameter;
	}
	if (theParameter2Parameter) {
		theParameterList << theParameter2Parameter;
	}
	if (theParameter3Parameter) {
		theParameterList << theParameter3Parameter;
	}	
	if (theParameter4Parameter) {
		theParameterList << theParameter4Parameter;
	}
	return theParameterList;
}

void RContinuousRandomVariable::plot(QString type)
{
	RMainWindow *mainWindow = qobject_cast<RMainWindow *>(parent()->parent());
	QwtPlot *thePlot = new RPlotWidget(mainWindow);
	mainWindow->addSubWindow(thePlot, objectName() + QString(" %1 Plot").arg(type));
	QString xAxisLabel;
	if (type == QString("Inverse CDF")) {
		xAxisLabel = "Probability";
	} else {
		xAxisLabel = "Random Variable";
	}
	thePlot->setAxisTitle(QwtPlot::xBottom, xAxisLabel);
	thePlot->setAxisTitle(QwtPlot::yLeft, type);
	thePlot->setCanvasBackground(QColor(Qt::white));
	//QwtLegend *theLegend = new QwtLegend(thePlot);
	//thePlot->insertLegend(theLegend, QwtPlot::TopLegend);
	thePlot->setMargin(10);

	QwtPlotCurve *theCurve = new QwtPlotCurve(type);
	theCurve->setPen(QColor(Qt::red));
	theCurve->setRenderHint(QwtPlotItem::RenderAntialiased, true);
	theCurve->attach(thePlot);

	QVector<double> xVector;
	QVector<double> yVector;

	double mean = getMeanValue();
	double standardDeviation = getStandardDeviationValue();

	//For ill-defined parameters:
	if (theDistributionType == QString("Cauchy (a, xo)")){
		//mean = getParameter2Value();
		standardDeviation = 2 * getParameter1Value(); // *!: This is an ad hoc modification! Because the stdv of the Cauchy dist. is undefined and cannot be used for plotting step.
	} else if (theDistributionType == QString("F-distribution (nu1, nu2)")) {
		if (getParameter2Value() <= 2) {// *!: This is an ad hoc modification! Because the moments of the F-distribution are undefined in this condition and cannot be used for plotting step.
			mean = 0;
			standardDeviation = 200/9; //This is the stdv given nu1=1 & nu2=5. 
		} else if (getParameter2Value() <= 4) {
			standardDeviation = 200/9;
		}
	} else if (theDistributionType == QString("Pareto (a, b)") || theDistributionType == QString("Type-2 Gumbel (a, b)") || theDistributionType == QString("Frechet (a, s, m)") ||  theDistributionType == QString("Levy (mu, c)") ) {
		if (gsl_isinf (mean) == 1){
			mean = getParameter2Value();
		}
		if (gsl_isinf (standardDeviation) == 1) {
			standardDeviation = 5;
		}
	} else if (theDistributionType == QString("Arcsine (a, b)")) {
		standardDeviation = (getParameter2Value() - getParameter1Value()) / 8;
	}

	int pointsCount = 1000;

	for (int i = 0; i <= pointsCount; i++) {	
		double x = (2.0 * i - pointsCount) / pointsCount * 3.0 * standardDeviation + mean;
		double y;
		if (type == QString("PDF")) {
			y = getPDFValue(x);
		} else if (type == QString("CDF")) {
			y = 0.0;
			double temp = getCDFValue(x);
			if (temp >= 0.0 && temp <= 1.0) {
				y = temp;
			}
		} else if (type == QString("Inverse CDF")) {
			if ((i == 0) || (i == pointsCount)) {
				continue;
			}
			x = 1.0 * i / pointsCount;
			y = getInverseCDFValue(x);
		}
		xVector << x;
		yVector << y;
	}

	theCurve->setData(xVector, yVector);

	thePlot->replot();
	//Returning to the original values:
	mean = getMeanValue();
	standardDeviation = getStandardDeviationValue();
}

void RContinuousRandomVariable::setDynamicProperty(QString propertyName, QVariant propertyValue)
{
	if (QString(propertyName) == QString("XPoints")) {
		XVector = stringToDoubleVector(propertyValue.toString());
		QString temp = doubleVectorToString(XVector);
		if (propertyValue != temp) {
			this->setProperty("XPoints", temp);
		}
	} else if (QString(propertyName) == QString("PDFPoints")) {
		PDFVector = stringToDoubleVector(propertyValue.toString());
		QString temp = doubleVectorToString(PDFVector);
		if (propertyValue != temp) {
			this->setProperty("PDFPoints", temp);
		}
	}
	if (!XVector.isEmpty() && !PDFVector.isEmpty() && XVector.count() == PDFVector.count()) {
		setUserDefinedPDF(XVector, PDFVector, true);
	}
}

void RContinuousRandomVariable::setUserDefinedPDF(QVector<double> passedXVector, QVector<double> passedPDFVector, bool viaGUI)
{
	XVector = passedXVector;
	PDFVector = passedPDFVector;
	updateMeanStdv();
	setCurrentValue(getMeanValue());
	double lastCDF = getCDFValue(XVector.last());
	if (qAbs(lastCDF - 1.0) > 1e-10) {
		for (int i = 0; i < PDFVector.count(); i++) {
			PDFVector[i] /= lastCDF;
		}
		updateMeanStdv();
		setCurrentValue(getMeanValue());
		
		if (viaGUI) {
			qCritical() << "Warning: The total area under the given PDF curve of the user-defined random variable" << objectName() << "is not equal to 1.0. Rt will normalize the given PDF.";
			this->setProperty("PDFPoints", doubleVectorToString(PDFVector));
		}
	}
}

int RContinuousRandomVariable::userDefinedErrorCheck()
{
	if (XVector.isEmpty() || PDFVector.isEmpty()) {
		qCritical() << "Error: The X points and/or the PDF points in the user-defined random variable" << objectName() << "is empty. The distribution is returned as 0.0.";
		return -1;
	}
	if (XVector.count() != PDFVector.count()) {
		qCritical() << "Error: The number of X points and PDF points in the user-defined random variable" << objectName() << "are not equal. The distribution is returned as 0.0.";
		return -1;
	}
	return 0;
}


QStringList RContinuousRandomVariable::getActionList()
{
	QStringList actionList;
	actionList << "Plot &PDF" << "Plot &CDF" << "Plot &Inverse CDF" << "Separator" << "&Get PDF" << "G&et CDF" << "Ge&t Inverse CDF";
	return actionList;
}

void RContinuousRandomVariable::plotPDF()
{
	plot("PDF");
}

void RContinuousRandomVariable::plotCDF()
{
	plot("CDF");
}

void RContinuousRandomVariable::plotInverseCDF()
{
	plot("Inverse CDF");
}

void RContinuousRandomVariable::get(QString type)
{
	RMainWindow *mainWindow = qobject_cast<RMainWindow *>(parent()->parent());

	bool ok;
	QString text = QInputDialog::getText(0, tr("Distribution"), tr("Enter value"), QLineEdit::Normal, "", &ok);
	double value;
	if (ok && !text.isEmpty()) {
		value = text.toDouble(&ok);
	}
	if (ok) {
		if (type == QString("PDF")) {
			qDebug() << "The PDF of the random variable" << objectName() << "at" << value << "is" << getPDFValue(value);
		} else if (type == QString("CDF")) {
			qDebug() << "The CDF of the random variable" << objectName() << "at" << value << "is" << getCDFValue(value);
		} else if (type == QString("Inverse CDF")) {
			qDebug() << "The inverse CDF of the random variable" << objectName() << "at" << value << "is" << getInverseCDFValue(value);
		}
	}
}

void RContinuousRandomVariable::getPDF()
{
	get("PDF");
}

void RContinuousRandomVariable::getCDF()
{
	get("CDF");
}

void RContinuousRandomVariable::getInverseCDF()
{
	get("Inverse CDF");
}
