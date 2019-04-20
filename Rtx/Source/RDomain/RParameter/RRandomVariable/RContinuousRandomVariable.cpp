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
	DistributionEnumType = Normal;

	if (theDistributionType == QString("User Defined")) {
		DistributionEnumType = User_Defined;
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
	return getStringParameter(&theMeanDouble[0], theMeanParameter.data());
}

QString RContinuousRandomVariable::getStandardDeviation()
{
	return getStringParameter(&theStandardDeviationDouble[0], theStandardDeviationParameter.data());
}

QString RContinuousRandomVariable::getCoefficientOfVariation()
{
	return getStringParameter(&theCoefficientOfVariationDouble[0], theCoefficientOfVariationParameter.data());
}

QString RContinuousRandomVariable::getParameter1()
{
	return getStringParameter(&theParameter1Double[0], theParameter1Parameter.data());
}

QString RContinuousRandomVariable::getParameter2()
{
	return getStringParameter(&theParameter2Double[0], theParameter2Parameter.data());
}

QString RContinuousRandomVariable::getParameter3()
{
	return getStringParameter(&theParameter3Double[0], theParameter3Parameter.data());
}

QString RContinuousRandomVariable::getParameter4()
{
	return getStringParameter(&theParameter4Double[0], theParameter4Parameter.data());
}

RContinuousRandomVariable::RUncertaintyType RContinuousRandomVariable::getUncertaintyType()
{
	return theUncertaintyType;
}

void RContinuousRandomVariable::setDistributionType(QString value)
{
	theDistributionType = value;

	if (theDistributionType == ("Normal (mean, stdv)")) {
		DistributionEnumType = Normal;
	}
	else if (theDistributionType == ("Exponential (mu, xo)")) {
		DistributionEnumType = Exponential;
	}
	else if (theDistributionType == ("Bounded Exponential (mu, xo, min, max)")) {
		DistributionEnumType = BoundedExponential;
	}
	else if (theDistributionType == ("Linear (a, b)")) {
		DistributionEnumType = Linear;
	}
	else if (theDistributionType == ("Laplace (a, xo)")) {
		DistributionEnumType = Laplace;
	}
	else if (theDistributionType == ("Rayleigh (sigma, xo)")) {
		DistributionEnumType = Rayleigh;
	}
	else if (theDistributionType == ("Gamma (a, b)")) {
		DistributionEnumType = Gamma;
	}
	else if (theDistributionType == ("Uniform (a, b)")) {
		DistributionEnumType = Uniform;
	}
	else if (theDistributionType == ("Lognormal (zeta, sigma)")) {
		DistributionEnumType = Lognormal;
	}
	else if (theDistributionType == ("Chi-square (nu, xo)")) {
		DistributionEnumType = Chi_square;
	}
	else if (theDistributionType == ("Scaled Inverse Chi-square (nu, xo, sigma)")) {
		DistributionEnumType = Scaled_inverse_chi_square;
	}
	else if (theDistributionType == ("t-distribution (nu, xo, sigma)")) {
		DistributionEnumType = t_distribution;
	}
	else if (theDistributionType == ("Beta (a, b, min, max)")) {
		DistributionEnumType = Beta;
	}
	else if (theDistributionType == ("Logistic (a, xo)")) {
		DistributionEnumType = Logistic;
	}
	else if (theDistributionType == ("Weibull (a, b)")) {
		DistributionEnumType = Weibull;
	}
	else if (theDistributionType == ("Gumbel (a, b)")) {
		DistributionEnumType = Gumbel;
	}


#ifndef OLD_DISTRIBUTION
	else if (theDistributionType == QString("F-distribution (nu1, nu2)")) {
		DistributionEnumType = FDistribution;
	}
	else if (theDistributionType == QString("Pareto (a, b)")) {
		DistributionEnumType = Pareto;
	}
	else if (theDistributionType == QString("Type-2 Gumbel (a, b)")) {
		DistributionEnumType = type2Gumbel;
	}
	else if (theDistributionType == QString("Frechet (a, s, m)")) {
		DistributionEnumType = Frechet;
	}
	else if (theDistributionType == QString("Arcsine (a, b)")) {
		DistributionEnumType = Arcsine;
	}
	else if (theDistributionType == QString("Birnbaum-Saunders (a, b, m)")) {
		DistributionEnumType = BirnbaumSaunders;
	}
	else if (theDistributionType == QString("Irwin-Hall (n, a, b)")) {
		DistributionEnumType = IrwinHall;
	}
	else if (theDistributionType == QString("Wald (mu, lambda)")) {
		DistributionEnumType = Wald;
	}
#endif // OLD_DISTRIBUTION

	else {
		setProperty("XPoints", QVariant());
		setProperty("PDFPoints", QVariant());
	}

	if (theDistributionType == QString("User Defined")) {
		DistributionEnumType = User_Defined;
		QString XPointsStr = property("XPoints").toString();
		QString PDFPointsStr = property("PDFPoints").toString();
		setProperty("XPoints", XPointsStr);
		setProperty("PDFPoints", PDFPointsStr);
	}
	else {
		setProperty("XPoints", QVariant());
		setProperty("PDFPoints", QVariant());
	}

	updateParameters(0);
}

void RContinuousRandomVariable::setMean(QString value)
{
	setStringParameter(value, &theMeanDouble[0], &theMeanParameter);
	setCurrentValue(getMeanValue(0));
	updateParameters(0);
}

void RContinuousRandomVariable::setStandardDeviation(QString value)
{
	setStringParameter(value, &theStandardDeviationDouble[0], &theStandardDeviationParameter);
	updateParameters(0);
}

void RContinuousRandomVariable::setCoefficientOfVariation(QString value)
{
	setStringParameter(value, &theCoefficientOfVariationDouble[0], &theCoefficientOfVariationParameter);
	updateMeanStdvByCOV(0);
	updateParameters(0);
}

void RContinuousRandomVariable::setParameter1(QString value)
{
	setStringParameter(value, &theParameter1Double[0], &theParameter1Parameter);
	updateMeanStdv(0);
	setCurrentValue(getMeanValue(0));
}

void RContinuousRandomVariable::setParameter2(QString value)
{
	setStringParameter(value, &theParameter2Double[0], &theParameter2Parameter);
	updateMeanStdv(0);
	setCurrentValue(getMeanValue(0));
}

void RContinuousRandomVariable::setParameter3(QString value)
{
	setStringParameter(value, &theParameter3Double[0], &theParameter3Parameter);
	updateMeanStdv(0);
	setCurrentValue(getMeanValue(0));
}

void RContinuousRandomVariable::setParameter4(QString value)
{
	setStringParameter(value, &theParameter4Double[0], &theParameter4Parameter);
	updateMeanStdv(0);
	setCurrentValue(getMeanValue(0));
}

void RContinuousRandomVariable::setUncertaintyType(RContinuousRandomVariable::RUncertaintyType value)
{
	theUncertaintyType = value;
}

QString RContinuousRandomVariable::getIncrementalCostOfMean()
{
	return getStringParameter(&theIncrementalCostOfMeanDouble[0], theIncrementalCostOfMeanParameter.data());
}

QString RContinuousRandomVariable::getIncrementalCostOfStandardDeviation()
{
	return getStringParameter(&theIncrementalCostOfStandardDeviationDouble[0], theIncrementalCostOfStandardDeviationParameter.data());
}

void RContinuousRandomVariable::setIncrementalCostOfMean(QString value)
{
	setStringParameter(value, &theIncrementalCostOfMeanDouble[0], &theIncrementalCostOfMeanParameter);
}

void RContinuousRandomVariable::setIncrementalCostOfStandardDeviation(QString value)
{
	setStringParameter(value, &theIncrementalCostOfStandardDeviationDouble[0], &theIncrementalCostOfStandardDeviationParameter);
}

double RContinuousRandomVariable::getMeanValue(int threadID)
{
	return getDoubleParameter(&theMeanDouble[threadID], theMeanParameter.data(), threadID);
}

void RContinuousRandomVariable::setMeanValue(double value, int threadID)
{
	setDoubleParameter(value, &theMeanDouble[threadID], theMeanParameter.data(), threadID);
	updateParameters(threadID);
}

double RContinuousRandomVariable::getStandardDeviationValue(int threadID)
{
	return getDoubleParameter(&theStandardDeviationDouble[threadID], theStandardDeviationParameter.data(), threadID);
}

void RContinuousRandomVariable::setStandardDeviationValue(double value, int threadID)
{
	setDoubleParameter(value, &theStandardDeviationDouble[threadID], theStandardDeviationParameter.data(), threadID);
	updateParameters(threadID);
}

double RContinuousRandomVariable::getCoefficientOfVariationValue(int threadID)
{
	return getDoubleParameter(&theCoefficientOfVariationDouble[threadID], theCoefficientOfVariationParameter.data(), threadID);
}

void RContinuousRandomVariable::setCoefficientOfVariationValue(double value, int threadID)
{
	setDoubleParameter(value, &theCoefficientOfVariationDouble[threadID], theCoefficientOfVariationParameter.data(), threadID);
	updateMeanStdvByCOV(threadID);
	updateParameters(threadID);
}

double RContinuousRandomVariable::getParameter1Value(int threadID)
{
	return getDoubleParameter(&theParameter1Double[threadID], theParameter1Parameter.data(), threadID);
}

void RContinuousRandomVariable::setParameter1Value(double value, int threadID)
{
	setDoubleParameter(value, &theParameter1Double[threadID], theParameter1Parameter.data(), threadID);
	updateParameters(threadID);
}

double RContinuousRandomVariable::getParameter2Value(int threadID)
{
	return getDoubleParameter(&theParameter2Double[threadID], theParameter2Parameter.data(), threadID);
}

void RContinuousRandomVariable::setParameter2Value(double value, int threadID)
{
	setDoubleParameter(value, &theParameter2Double[threadID], theParameter2Parameter.data(), threadID);
	updateParameters(threadID);
}

double RContinuousRandomVariable::getParameter3Value(int threadID)
{
	return getDoubleParameter(&theParameter3Double[threadID], theParameter3Parameter.data(), threadID);
}

void RContinuousRandomVariable::setParameter3Value(double value, int threadID)
{
	setDoubleParameter(value, &theParameter3Double[threadID], theParameter3Parameter.data(), threadID);
	updateParameters(threadID);
}

double RContinuousRandomVariable::getParameter4Value(int threadID)
{
	return getDoubleParameter(&theParameter4Double[threadID], theParameter4Parameter.data(), threadID);
}

void RContinuousRandomVariable::setParameter4Value(double value, int threadID)
{
	setDoubleParameter(value, &theParameter4Double[threadID], theParameter4Parameter.data(), threadID);
	updateParameters(threadID);
}

void RContinuousRandomVariable::setIncrementalCostOfMeanValue(double value, int threadID)
{
	setDoubleParameter(value, &theIncrementalCostOfMeanDouble[threadID], theIncrementalCostOfMeanParameter.data(), threadID);
}

void RContinuousRandomVariable::setIncrementalCostOfStandardDeviationValue(double value, int threadID)
{
	setDoubleParameter(value, &theIncrementalCostOfStandardDeviationDouble[threadID], theIncrementalCostOfStandardDeviationParameter.data(), threadID);
}


double RContinuousRandomVariable::getIncrementalCostOfMeanValue(int threadID)
{
	return getDoubleParameter(&theIncrementalCostOfMeanDouble[threadID], theIncrementalCostOfMeanParameter.data(), threadID);
}

double RContinuousRandomVariable::getIncrementalCostOfStandardDeviationValue(int threadID)
{
	return getDoubleParameter(&theIncrementalCostOfStandardDeviationDouble[threadID], theIncrementalCostOfStandardDeviationParameter.data(), threadID);
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


double RContinuousRandomVariable::getPDFValue(double x, int threadID)
{
	double result;

	double nu1, nu2, s, a, b, min, max, mean, stdv, xo, nu, sigma, zeta, mu, m, y, sum, k, n;
	unsigned int varnumber;
	if (theStandardDeviationDouble[threadID] == 0.0) {
		rCritical(threadID) << "WARNING: The standard deviation of the random variable" << objectName() << "is Zero.";
	}
	switch (DistributionEnumType)
	{
	case Normal:
		mean = getParameter1Value(threadID);
		stdv = getParameter2Value(threadID);
		result = gsl_ran_gaussian_pdf(x - mean, stdv);
		break;
	case Exponential:
		mu = getParameter1Value(threadID);
		xo = getParameter2Value(threadID);
		result = gsl_ran_exponential_pdf(x - xo, mu);
		break;
	case BoundedExponential:
		min = getParameter3Value(threadID);
		max = getParameter4Value(threadID);
		if (x < min || x > max) {
			result = 0.0;
		}
		else {
			mu = getParameter1Value(threadID);
			xo = getParameter2Value(threadID);
			double k = 1.0 / (1.0 - exp(-mu * (max - min)));
			result = k * mu * exp(-mu * (x - xo - min));
		}
		break;
	case Linear:
		min = getParameter1Value(threadID);
		max = getParameter2Value(threadID);
		if (x < min || x > max) {
			result = 0.0;
		}
		else {
			result = 2 * x / (max * max - min * min);
		}
		break;
	case Laplace:
		a = getParameter1Value(threadID);
		xo = getParameter2Value(threadID);
		result = gsl_ran_laplace_pdf(x - xo, a);
		break;
	case Rayleigh:
		sigma = getParameter1Value(threadID);
		xo = getParameter2Value(threadID);
		result = gsl_ran_rayleigh_pdf(x - xo, sigma);
		break;
	case Gamma:
		a = getParameter1Value(threadID);
		b = getParameter2Value(threadID);
		result = gsl_ran_gamma_pdf(x, a, b);
		break;
	case Uniform:
		a = getParameter1Value(threadID);
		b = getParameter2Value(threadID);
		result = gsl_ran_flat_pdf(x, a, b);
		break;
	case Lognormal:
		zeta = getParameter1Value(threadID);
		sigma = getParameter2Value(threadID);
		result = gsl_ran_lognormal_pdf(x, zeta, sigma);
		break;
	case Chi_square:
		nu = getParameter1Value(threadID);
		xo = getParameter2Value(threadID);
		result = gsl_ran_chisq_pdf(x - xo, nu);
		break;
	case Scaled_inverse_chi_square:
		nu = getParameter1Value(threadID);
		xo = getParameter2Value(threadID);
		sigma = getParameter3Value(threadID);
		result = gsl_ran_chisq_pdf(x - xo, nu) * (pow(nu*sigma, nu / 2))*((pow((x - xo), ((-nu / 2) - 1)))*exp(-(nu*sigma) / (2 * (x - xo)))) / ((pow((x - xo), ((nu / 2) - 1)))*exp(-(x - xo) / 2));
		break;
	case t_distribution:
		nu = getParameter1Value(threadID);
		xo = getParameter2Value(threadID);
		sigma = getParameter3Value(threadID);
		result = gsl_ran_tdist_pdf((x - xo) / sigma, nu);
		break;
	case Beta:
		a = getParameter1Value(threadID);
		b = getParameter2Value(threadID);
		min = getParameter3Value(threadID);
		max = getParameter4Value(threadID);
		result = gsl_ran_beta_pdf((x - min) / (max - min), a, b) / (max - min);
		break;
	case Logistic:
		a = getParameter1Value(threadID);
		xo = getParameter2Value(threadID);
		result = gsl_ran_logistic_pdf(x - xo, a);
		break;
	case Weibull:
		a = getParameter1Value(threadID);
		b = getParameter2Value(threadID);
		result = gsl_ran_weibull_pdf(x, a, b);
		break;
	case Gumbel:
		a = getParameter1Value(threadID);
		b = getParameter2Value(threadID);
		result = gsl_ran_gumbel1_pdf(x, a, b);
		break;

#ifndef OLD_DISTRIBUTION
	case FDistribution:
		nu1 = getParameter1Value(0);
		nu2 = getParameter2Value(0);
		result = gsl_ran_fdist_pdf(x, nu1, nu2);
		break;
	case Pareto:
		a = getParameter1Value(0);
		b = getParameter2Value(0);
		result = gsl_ran_pareto_pdf(x, a, b);
		break;
	case type2Gumbel:
		a = getParameter1Value(0);
		b = getParameter2Value(0);
		result = gsl_ran_gumbel2_pdf(x, a, b);
		break;
	case Frechet:
		a = getParameter1Value(0);
		s = getParameter2Value(0);//Scale; > Wikipedia.
		m = getParameter3Value(0);//Location of mininum; > Wikipedia.
		result = gsl_ran_gumbel2_pdf((x - m) / s, a, 1) / s;
		break;
	case Arcsine:
		a = getParameter1Value(0);//min value
		b = getParameter2Value(0);//max value
		result = 1 / (b - a) * 1 / (M_PI * sqrt(((x - a) / (b - a)) * ((b - x) / (b - a))));
		break;
	case BirnbaumSaunders:
		a = getParameter1Value(0);//shape parameter
		b = getParameter2Value(0);//scale
		m = getParameter3Value(0);//location of min
		y = (x - m) / b;
		if (y <= 0) {
			result = 0;
		}
		else {
			result = 1 / b * ((sqrt(y) + sqrt(1 / y)) / 2 / a / y) *  gsl_ran_gaussian_pdf(((sqrt(y) - sqrt(1 / y)) / a), 1);
		}
		break;
	case IrwinHall:
		varnumber = getParameter1Value(0);//The number of the reference uniform variables.
		a = getParameter2Value(0);//The min of the reference uniform variables.
		b = getParameter3Value(0);//The max of the reference uniform variables.
		y = (x - varnumber * a) / (b - a);
		sum = 0;
		unsigned int z;
		for (z = 0; z <= floor(y); z++) {
			// added 0.5 to prevent trunctation by double to int conversion
			sum = sum + pow(-1.0, z * 1.0) * gsl_sf_choose(varnumber + 0.5, z) * pow((y - z), (varnumber + 0.5 - 1) * 1.0);
		}
		result = sum / gsl_sf_fact(varnumber - 1) / (b - a);
		break;
	case Wald:
		if (x <= 0) {
			result = 0;
		}
		else {
			mu = getParameter1Value(0);
			double lambda = getParameter2Value(0);
			result = sqrt(lambda / 2 / M_PI / pow(x, 3)) * exp(-1 * lambda * pow(x - mu, 2) / 2 / pow(mu, 2) / x);
		}
		break;
#endif // OLD_DISTRIBUTION

	case User_Defined:
		if (userDefinedErrorCheck() < 0) {
			return 0.0;
		}
		result = -1.0;
		if (x < XVector[0]) {
			return 0.0;
		}
		else if (x > XVector.last()) {
			return 0.0;
		}
		else {
			for (int i = 1; i < XVector.count(); i++) {
				if (x <= XVector[i]) {
					a = (PDFVector[i] - PDFVector[i - 1]) / (XVector[i] - XVector[i - 1]);
					result = (a * (x - XVector[i - 1]) + PDFVector[i - 1]);
					break;
				}
			}
			if (result < -1e-10) {
				rCritical() << "Error: The user defined distribution function in random variable" << objectName() << "seems to be invalid. The distribution is returned as 0.0.";
				return 0.0;
			}
		}
		break;
	default:
		rCritical() << "Warning: Illegal probability distribution type has been passed to the random variable" << objectName() << ".";
		break;
	}

	return result;
}

double RContinuousRandomVariable::getCDFValue(double x, int threadID)
{
	double result;
	double nu1, nu2, a, b, min, max, mean, stdv, xo, nu, sigma, zeta, mu, sum, m, y, lambda, s, k, n;
	unsigned int varnumber;

	if (theStandardDeviationDouble[threadID] == 0.0) {
		rCritical(threadID) << "WARNING: The standard deviation of the random variable" << objectName() << "is Zero.";
	}
	switch (DistributionEnumType)
	{
	case Normal:
		mean = getParameter1Value(threadID);
		stdv = getParameter2Value(threadID);
		result = gsl_cdf_gaussian_P(x - mean, stdv);
		break;
	case Exponential:
		mu = getParameter1Value(threadID);
		xo = getParameter2Value(threadID);
		result = gsl_cdf_exponential_P(x - xo, mu);
		break;
	case BoundedExponential:
		min = getParameter3Value(threadID);
		max = getParameter4Value(threadID);
		if (x < min) {
			result = 0.0;
		}
		else if (x > max) {
			result = 1.0;
		}
		else {
			mu = getParameter1Value(threadID);
			xo = getParameter2Value(threadID);
			k = 1.0 / (1.0 - exp(-mu*(max - min)));
			result = k - k * exp(-mu*(x - xo - min));
		}
		break;
	case Linear:
		min = getParameter1Value(threadID);
		max = getParameter2Value(threadID);
		if (x < min) {
			result = 0.0;
		}
		else if (x > max) {
			result = 1.0;
		}
		else {
			result = (x*x - min*min) / (max*max - min*min);
		}
		break;
	case Laplace:
		a = getParameter1Value(threadID);
		xo = getParameter2Value(threadID);
		result = gsl_cdf_laplace_P(x - xo, a);
		break;
	case Rayleigh:
		sigma = getParameter1Value(threadID);
		xo = getParameter2Value(threadID);
		result = gsl_cdf_rayleigh_P(x - xo, sigma);
		break;
	case Gamma:
		a = getParameter1Value(threadID);
		b = getParameter2Value(threadID);
		result = gsl_cdf_gamma_P(x, a, b);
		break;
	case Uniform:
		a = getParameter1Value(threadID);
		b = getParameter2Value(threadID);
		result = gsl_cdf_flat_P(x, a, b);
		break;
	case Lognormal:
		zeta = getParameter1Value(threadID);
		sigma = getParameter2Value(threadID);
		result = gsl_cdf_lognormal_P(x, zeta, sigma);
		break;
	case Chi_square:
		nu = getParameter1Value(threadID);
		xo = getParameter2Value(threadID);
		result = gsl_cdf_chisq_P(x - xo, nu);
		break;
	case Scaled_inverse_chi_square:
		nu = getParameter1Value(threadID);
		xo = getParameter2Value(threadID);
		sigma = getParameter3Value(threadID);
		result = gsl_cdf_chisq_P(x - xo, nu) * gsl_sf_gamma_inc_Q(nu / 2, (nu*sigma) / (2 * (x - xo))) / gsl_sf_gamma_inc_P(nu / 2, (x - xo) / 2);
		break;
	case t_distribution:
		nu = getParameter1Value(threadID);
		xo = getParameter2Value(threadID);
		sigma = getParameter3Value(threadID);
		result = gsl_cdf_tdist_P((x - xo) / sigma, nu);
		break;
	case Beta:
		a = getParameter1Value(threadID);
		b = getParameter2Value(threadID);
		min = getParameter3Value(threadID);
		max = getParameter4Value(threadID);
		result = gsl_cdf_beta_P((x - min) / (max - min), a, b);
		break;
	case Logistic:
		a = getParameter1Value(threadID);
		xo = getParameter2Value(threadID);
		result = gsl_cdf_logistic_P(x - xo, a);
		break;
	case Weibull:
		a = getParameter1Value(threadID);
		b = getParameter2Value(threadID);
		result = gsl_cdf_weibull_P(x, a, b);
		break;
	case Gumbel:
		a = getParameter1Value(threadID);
		b = getParameter2Value(threadID);
		// Attention: Bug in GSL: Cannot use the GSL method.
		//result = gsl_cdf_gumbel1_P(x,a,b);
		result = exp(-b * exp(-a * x));
		break;

#ifndef OLD_DISTRIBUTION

	case FDistribution:
		nu1 = getParameter1Value(0);
		nu2 = getParameter2Value(0);
		result = gsl_cdf_fdist_P(x, nu1, nu2);
		break;
	case Pareto:
		a = getParameter1Value(0);
		b = getParameter2Value(0);
		result = gsl_cdf_pareto_P(x, a, b);
		break;
	case type2Gumbel:
		a = getParameter1Value(0);
		b = getParameter2Value(0);
		//*The gsl gumble2 function has a bug, which calculates the CDF in negative x's similar to positive ones!
		if (x > 0) {
			result = gsl_cdf_gumbel2_P(x, a, b);
		}
		else {
			result = 0;
		}
		break;
	case Frechet:
		a = getParameter1Value(0);
		s = getParameter2Value(0);//Scale; > Wikipedia.
		m = getParameter3Value(0);//Location of mininum; > Wikipedia.
								  //*The gsl gumble2 function has a bug, which calculates the CDF in negative x's similar to positive ones!
		if ((x - m) / s >= 0) {
			result = gsl_cdf_gumbel2_P((x - m) / s, a, 1);
		}
		else {
			result = 0;
		}
		break;
	case Arcsine:
		a = getParameter1Value(0);
		b = getParameter2Value(0);
		result = asin(sqrt((x - a) / (b - a))) * 2 / M_PI;
		break;
	case BirnbaumSaunders:
		a = getParameter1Value(0);
		b = getParameter2Value(0);
		m = getParameter3Value(0);
		y = (x - m) / b;
		result = gsl_cdf_gaussian_P(((sqrt(y) - sqrt(1 / y)) / a), 1);
		break;
	case IrwinHall:
		varnumber = getParameter1Value(0);//The number of the reference uniform variables.
		a = getParameter2Value(0);//The min of the reference uniform variables.
		b = getParameter3Value(0);//The max of the reference uniform variables.
		y = (x - varnumber * a) / (b - a);
		if (y >= varnumber * b) {
			result = 1;
		}
		else {
			sum = 0;
			unsigned int  ka;
			for (ka = 0; ka <= floor(y); ka++) {
				sum = sum + pow(-1.0, ka * 1.0) * gsl_sf_choose(varnumber + 0.5, ka + 0.5) * pow((y - ka), varnumber * 1.0);
			}
			// added 0.5 to prevent truncation by double to int conversion
			result = sum / gsl_sf_fact(varnumber + 0.5);
		}
		break;
	case Wald:
		if (x <= 0) {
			result = 0;
		}
		else {
			mu = getParameter1Value(0);
			lambda = getParameter2Value(0);
			result = gsl_cdf_gaussian_P((sqrt(lambda / x) * (x / mu - 1)), 1.0) + exp(2 * lambda / mu) * gsl_cdf_gaussian_P(-1 * (sqrt(lambda / x) * (x / mu + 1)), 1.0);
		}
		break;
#endif // OLD_DISTRIBUTION
	case User_Defined:
		if (userDefinedErrorCheck() < 0) {
			return 0.0;
		}
		result = -1.0;
		if (x < XVector[0]) {
			return 0.0;
		}
		else if (x > XVector.last()) {
			return 1.0;
		}
		else {
			sum = 0.0;
			for (int i = 1; i < XVector.count(); i++) {
				if (x <= XVector[i]) {
					result = (sum + 0.5 * (getPDFValue(x, threadID) + PDFVector[i - 1]) * (x - XVector[i - 1]));
					break;
				}
				sum += 0.5 * (PDFVector[i] + PDFVector[i - 1]) * (XVector[i] - XVector[i - 1]);
			}
			if (result < -1e-10) {
				rCritical() << "Error: The user defined distribution function in random variable" << objectName() << "seems to be invalid. The distribution is returned as 0.0.";
				return 0.0;
			}
		}
		break;
	default:	rCritical() << "Warning: Illegal probability distribution type has been passed to the random variable" << objectName() << ".";
		break;
	}

	return result;
}

double RContinuousRandomVariable::getInverseCDFValue(double p, int threadID)
{
	if (p < 0.0 || p > 1.0) {
		rCritical(threadID) << "Warning: Illegal probability value input has been passed to the random variable" << objectName() << ".";

		/*// Is this okay?
		if (p < 0.0) {
		p = 0.0;
		} else if (p > 1.0) {
		p = 1.0;
		}*/
	}

	double result;
	double nu1, nu2, a, b, k, min, max, mean, stdv, xo, nu, sigma, zeta, mu, sum, AA, BB, CC, deltaX, s, m, HP;

	if (theStandardDeviationDouble[threadID] == 0.0) {
		rCritical() << "WARNING: The standard deviation of the random variable" << objectName() << "is Zero.";
	}
	switch (DistributionEnumType)
	{
	case Normal:
		mean = getParameter1Value(threadID);
		stdv = getParameter2Value(threadID);
		result = gsl_cdf_gaussian_Pinv(p, stdv) + mean;
		break;
	case Exponential:
		mu = getParameter1Value(threadID);
		xo = getParameter2Value(threadID);
		result = gsl_cdf_exponential_Pinv(p, mu) + xo;
		break;
	case BoundedExponential:
		mu = getParameter1Value(threadID);
		xo = getParameter2Value(threadID);
		min = getParameter3Value(threadID);
		max = getParameter4Value(threadID);
		k = 1.0 / (1.0 - exp(-mu*(max - min)));
		result = (mu*min - log((k - p) / k)) / mu + xo;
		break;
	case Linear:
		min = getParameter1Value(threadID);
		max = getParameter2Value(threadID);
		result = sqrt(p * (max*max - min*min) + min*min);
		break;
	case Laplace:
		a = getParameter1Value(threadID);
		xo = getParameter2Value(threadID);
		result = gsl_cdf_laplace_Pinv(p, a) + xo;
		break;
	case Rayleigh:
		sigma = getParameter1Value(threadID);
		xo = getParameter2Value(threadID);
		result = gsl_cdf_rayleigh_Pinv(p, sigma) + xo;
		break;
	case Gamma:
		a = getParameter1Value(threadID);
		b = getParameter2Value(threadID);
		result = gsl_cdf_gamma_Pinv(p, a, b);
		break;
	case Uniform:
		a = getParameter1Value(threadID);
		b = getParameter2Value(threadID);
		result = gsl_cdf_flat_Pinv(p, a, b);
		break;
	case Lognormal:
		zeta = getParameter1Value(threadID);
		sigma = getParameter2Value(threadID);
		result = gsl_cdf_lognormal_Pinv(p, zeta, sigma);
		break;
	case Chi_square:
		nu = getParameter1Value(threadID);
		xo = getParameter2Value(threadID);
		result = gsl_cdf_chisq_Pinv(p, nu) + xo;
		break;
	case Scaled_inverse_chi_square:
		nu = getParameter1Value(threadID);
		xo = getParameter2Value(threadID);
		sigma = getParameter3Value(threadID);
		result = (nu*sigma) / gsl_cdf_chisq_Pinv(1 - p, nu) + xo;
		break;
	case t_distribution:
		nu = getParameter1Value(threadID);
		xo = getParameter2Value(threadID);
		sigma = getParameter3Value(threadID);
		result = gsl_cdf_tdist_Pinv(p, nu)*sigma + xo;
		break;
	case Beta:
		a = getParameter1Value(threadID);
		b = getParameter2Value(threadID);
		min = getParameter3Value(threadID);
		max = getParameter4Value(threadID);
		result = gsl_cdf_beta_Pinv(p, a, b) * (max - min) + min;
		break;
	case Logistic:
		a = getParameter1Value(threadID);
		xo = getParameter2Value(threadID);
		result = gsl_cdf_logistic_Pinv(p, a) + xo;
		break;
	case Weibull:
		a = getParameter1Value(threadID);
		b = getParameter2Value(threadID);
		result = gsl_cdf_weibull_Pinv(p, a, b);
		break;
	case Gumbel:
		a = getParameter1Value(threadID);
		b = getParameter2Value(threadID);
		result = gsl_cdf_gumbel1_Pinv(p, a, b);
		break;
#ifndef OLD_DISTRIBUTION

	case FDistribution:
		nu1 = getParameter1Value(0);
		nu2 = getParameter2Value(0);
		result = gsl_cdf_fdist_Pinv(p, nu1, nu2);
		break;
	case Pareto:
		a = getParameter1Value(0);
		b = getParameter2Value(0);
		result = gsl_cdf_pareto_Pinv(p, a, b);
		break;
	case type2Gumbel:
		a = getParameter1Value(0);
		b = getParameter2Value(0);
		result = gsl_cdf_gumbel2_Pinv(p, a, b);
		break;
	case Frechet:
		a = getParameter1Value(0);
		s = getParameter2Value(0);
		m = getParameter3Value(0);
		result = gsl_cdf_gumbel2_Pinv(p, a, 1) * s + m;
		break;
	case Arcsine:
		a = getParameter1Value(0);
		b = getParameter2Value(0);
		result = (b - a) * pow(sin(M_PI / 2 * p), 2) + a;
		break;
	case BirnbaumSaunders:
		a = getParameter1Value(0);
		b = getParameter2Value(0);
		m = getParameter3Value(0);
		HP = 2 + pow((a * gsl_cdf_gaussian_Pinv(p, 1)), 2);
		if (p <= .5) {
			result = b * (HP - sqrt(pow(HP, 2) - 4)) / 2 + m;
		}
		else {
			result = b * (HP + sqrt(pow(HP, 2) - 4)) / 2 + m;
		}
		break;
	case IrwinHall:
		qCritical() << "Warning: The inverse-CDF for the Irwin-Hall distribution is not available.";
		break;
	case Wald:
		qCritical() << "Warning: The inverse-CDF for the Wald distribution is not available.";
		break;
#endif // OLD_DISTRIBUTION
	case User_Defined:
		if (userDefinedErrorCheck() < 0) {
			return 0.0;
		}
		if (p == 0.0) {
			result = XVector.first();
		}
		else if (p == 1.0) {
			result = XVector.last();
		}
		else {
			for (int i = 1; i < XVector.count(); i++) {
				if (getCDFValue(XVector[i], threadID) > p) {
					a = (getPDFValue(XVector[i], threadID) - getPDFValue(XVector[i - 1], threadID)) / (XVector[i] - XVector[i - 1]);
					if (a == 0.0 && getPDFValue(XVector[i - 1], threadID) == 0.0) {
						rCritical() << "Error: The user defined distribution function in random variable" << objectName() << "has a region of constant zero PDF. The distribution is returned as 0.0.";
						return 0.0;
					}
					else if (a == 0.0) {
						result = (p - getCDFValue(XVector[i - 1], threadID) + getPDFValue(XVector[i - 1], threadID) * XVector[i - 1]) / getPDFValue(XVector[i - 1], threadID);
					}
					else {
						AA = 0.5 * a;
						BB = getPDFValue(XVector[i - 1], threadID);
						CC = getCDFValue(XVector[i - 1], threadID) - p;
						deltaX = (-BB + sqrt(BB * BB - 4.0 * AA * CC)) / (2.0 * AA);
						result = deltaX + (XVector[i - 1]);
					}
					break;
				}
			}
		}
		break;
	default:
		rCritical() << "Warning: Illegal probability distribution type has been passed to the random variable" << objectName() << ".";
		break;
	}


	return result;
}


int RContinuousRandomVariable::updateParameters(int threadID)
{
	// Initial declarations
	double parameter1 = 0.0;
	double parameter2 = 0.0;
	double parameter3 = 0.0;
	double parameter4 = 0.0;
	double mean = getMeanValue(threadID);
	double stdv = getStandardDeviationValue(threadID);
	double cov = qAbs(stdv / (mean));
	bool paramsCantBeUpdated = false;

	double min, max, temp;

	// Compute parameter values
	switch (DistributionEnumType)
	{
	case Normal:
		parameter1 = mean;
		parameter2 = stdv;
		break;
	case Exponential:
		parameter1 = stdv;
		parameter2 = mean - parameter1;
		break;
	case BoundedExponential:
		//rCritical() << "Warning: Moments cannot be given for the" << theDistributionType << "distribution. Please give parameter values only.";
		break;
	case Laplace:
		parameter2 = mean;
		parameter1 = stdv / sqrt(2.0);
		break;
	case Linear:
		//rCritical() << "Warning: Moments cannot be given for the" << theDistributionType << "distribution. Please give parameter values only.";
		break;
	case Rayleigh:
		parameter1 = stdv / sqrt((4.0 - M_PI) / 2.0);
		parameter2 = mean - parameter1 * sqrt(M_PI / 2.0);
		break;
	case Gamma:
		parameter1 = gsl_pow_2(mean / stdv);
		parameter2 = stdv*stdv / mean;
		break;
	case Uniform:
		parameter1 = mean - sqrt(3.0)*stdv;
		parameter2 = mean + sqrt(3.0)*stdv;
		break;
	case Lognormal:
		parameter1 = log(mean) - 0.5*log(1 + gsl_pow_2(stdv / mean));
		parameter2 = sqrt(log(gsl_pow_2(stdv / mean) + 1));
		break;
	case Chi_square:
		parameter1 = 0.5*stdv*stdv;
		parameter2 = mean - parameter1;
		break;
	case Scaled_inverse_chi_square:
		parameter2 = getParameter2Value(threadID);
		parameter1 = 2.0*gsl_pow_2(mean - parameter2) / gsl_pow_2(stdv) + 4;
		parameter3 = (mean - parameter2)*(parameter1 - 2) / parameter1;
		break;

	case t_distribution:
		parameter3 = getParameter3Value(threadID);
		parameter2 = mean;
		parameter1 = 2 * (stdv / parameter3)*(stdv / parameter3) / ((stdv / parameter3)*(stdv / parameter3) - 1);
		break;
	case Beta:
		//rCritical() << "Warning: Moments cannot be given for the Beta distribution. Please give parameter values only.";
		parameter3 = getParameter3Value(threadID);
		parameter4 = getParameter4Value(threadID);
		min = parameter3;
		max = parameter4;
		temp = (mean - min) * (max - mean) / (stdv * stdv) - 1.0;
		parameter1 = (mean - min) / (max - min) * temp;
		parameter2 = (max - mean) / (max - min) * temp;
		break;
	case Logistic:
		parameter1 = stdv*sqrt(3.0) / M_PI;
		parameter2 = mean - parameter1;
		break;
	case Weibull:
		rCritical() << "Warning: Moments cannot be given for the Weibull distribution. Please give parameter values only.";
		paramsCantBeUpdated = true;
		break;
	case Gumbel:
		parameter1 = M_PI / (stdv * sqrt(6.0));
		parameter2 = exp(parameter1 * mean - M_EULER);
		break;
#ifndef OLD_DISTRIBUTION

	case FDistribution:
		parameter1 = (2 * pow(mean, 2)) / (pow(mean, 2) - pow(mean, 3) + 2 * pow(stdv, 2) - mean * pow(stdv, 2));
		parameter2 = 2 * mean / (mean - 1);
		if (parameter1 < 0 || parameter2 < 0) {
			qCritical() << "Warning: The moments might be wrong. The F-distributions parameters cannot be negative.";
			paramsCantBeUpdated = true;
		}
		break;
	case Pareto:
		parameter1 = (stdv + sqrt(pow(mean, 2) + pow(stdv, 2))) / stdv;
		parameter2 = (pow(mean, 2) + pow(stdv, 2) - stdv * sqrt(pow(mean, 2) + pow(stdv, 2))) / mean;
		break;
	case type2Gumbel:
		qCritical() << "Warning: Moments cannot be given for the Type-2 Gumbel distribution. Please give parameter values only.";
		paramsCantBeUpdated = true;
		break;
	case Frechet:
		qCritical() << "Warning: Moments cannot be given for the Frechet distribution. Please give parameter values only.";
		paramsCantBeUpdated = true;
		break;
	case Arcsine:
		parameter1 = mean - sqrt(2.0) * stdv;
		parameter2 = mean + sqrt(2.0) * stdv;
		break;
	case BirnbaumSaunders:
		qCritical() << "Warning: Moments cannot be given for the Birnbaum-Saunders distribution. Please give parameter values only."; paramsCantBeUpdated = true;
		paramsCantBeUpdated = true;
		break;
	case IrwinHall:
		//Here, it is assumed that "n" remains constant. 
		parameter1 = getParameter1Value(0);
		parameter2 = (2 * mean / parameter1 - sqrt(12 / parameter1) * stdv) / 2;
		parameter3 = (2 * mean / parameter1 + sqrt(12 / parameter1) * stdv) / 2;
		break;
	case Wald:
		parameter1 = mean;
		parameter2 = pow(mean, 3) / pow(stdv, 2);
		break;
#endif // OLD_DISTRIBUTION

	case User_Defined:

		break;
	default:
		rCritical() << "Warning: Illegal probability distribution type has been passed to the random variable" << objectName() << ".";
		break;
	}

	// Update the parameter values; either the Double or the Parameter
	if (!theCoefficientOfVariationParameter) {
		theCoefficientOfVariationDouble[threadID] = cov;
	}
	else {
		theCoefficientOfVariationParameter.data()->setCurrentValue(cov, threadID);
	}

	if (!paramsCantBeUpdated) {
		if (!theParameter1Parameter) {
			theParameter1Double[threadID] = parameter1;
		}
		else {
			theParameter1Parameter.data()->setCurrentValue(parameter1, threadID);
		}

		if (!theParameter2Parameter) {
			theParameter2Double[threadID] = parameter2;
		}
		else {
			theParameter2Parameter.data()->setCurrentValue(parameter2, threadID);
		}

		if (!theParameter3Parameter) {
			theParameter3Double[threadID] = parameter3;
		}
		else {
			theParameter3Parameter.data()->setCurrentValue(parameter3, threadID);
		}

		if (!theParameter4Parameter) {
			theParameter4Double[threadID] = parameter4;
		}
		else {
			theParameter4Parameter.data()->setCurrentValue(parameter4, threadID);
		}
	}

	return 0;
}

int RContinuousRandomVariable::updateMeanStdvByCOV(int threadID)
{
	double mean = getMeanValue(threadID);
	double cov = getCoefficientOfVariationValue(threadID);
	double stdv = qAbs((mean)* cov);

	if (!theStandardDeviationParameter) {
		theStandardDeviationDouble[threadID] = stdv;
	}
	else {
		theStandardDeviationParameter.data()->setCurrentValue(stdv, threadID);
	}

	return 0;
}

int RContinuousRandomVariable::updateMeanStdv(int threadID)
{
	// Initial declarations
	double mean = 0.0;
	double stdv = 0.0;
	double parameter1 = getParameter1Value(threadID);
	double parameter2 = getParameter2Value(threadID);
	double parameter3 = getParameter3Value(threadID);
	double parameter4 = getParameter4Value(threadID);
	double a, b, min, max, mu, xo, k, x1, x2;
	switch (DistributionEnumType)
	{

	case Normal:
		mean = parameter1;
		stdv = parameter2;
		break;
	case Exponential:
		mean = parameter1 + parameter2;
		stdv = parameter1;
		break;
	case BoundedExponential:
		mu = parameter1;
		xo = parameter2;
		min = parameter3;
		max = parameter4;
		k = 1.0 / (1.0 - exp(-mu*(max - min)));
		mean = -k*(mu*max + 1.0) / mu * exp(-mu*(max - min)) + (1 + mu*min) / (mu)*k;
		stdv = sqrt((-exp(-mu*(max - min))) * (mu*mu*max*max + 2.0*mu*max + 2.0 - 2.0*mean*mu*mu*max - 2 * mean*mu + mean*mean*mu*mu)*k / (mu*mu) + (mu*mu*min*min + 2.0*mu*min + 2.0 - 2.0*min*mean*mu*mu - 2.0*mean*mu + mean*mean*mu*mu)*k / (mu*mu));
		mean += xo;
		break;
	case Linear:
		min = parameter1;
		max = parameter2;
		mean = 2.0 / 3.0 * (min*min + min*max + max*max) / (min + max);
		stdv = sqrt((min*min + max*max) / 2.0 - mean*mean);
		break;
	case Laplace:
		mean = parameter2;
		stdv = sqrt(2.0)*parameter1;
		break;
	case Rayleigh:
		mean = parameter1 * sqrt(M_PI / 2.0) + parameter2;
		stdv = parameter1*sqrt((4 - M_PI) / 2.0);
		break;
	case Gamma:
		mean = parameter1*parameter2;
		stdv = sqrt(parameter1)*parameter2;
		break;
	case Uniform:
		mean = (parameter1 + parameter2) / 2.0;
		stdv = (parameter2 - parameter1) / sqrt(12.0);
		break;
	case Lognormal:
		mean = exp(parameter1 + 0.5*parameter2*parameter2);
		stdv = sqrt((exp(parameter2*parameter2) - 1))*exp(parameter1 + 0.5*parameter2*parameter2);
		break;
	case Chi_square:
		mean = parameter1 + parameter2;
		stdv = sqrt(2.0*parameter1);
		break;
	case Scaled_inverse_chi_square:
		mean = (parameter1*parameter3) / (parameter1 - 2) + parameter2;
		if (parameter1 <= 4.0) {
			rCritical() << "Warning: The standard deviation is not defined for the Scaled inverse chi-square distribution for nu<=4.0. Stdv not set.";
		}
		else {
			stdv = sqrt(2.0*gsl_pow_2(parameter1*parameter3) / (gsl_pow_2(parameter1 - 2)*(parameter1 - 4)));
		}
		break;
	case t_distribution:
		mean = parameter2;
		if (parameter1 <= 2.0) {
			rCritical() << "Warning: The standard deviation is not defined for the t-distribution for nu<=2.0. Stdv not set.";
		}
		else {
			stdv = parameter3 * sqrt(parameter1 / (parameter1 - 2.0));
		}
		break;
	case Beta:
		a = parameter1;
		b = parameter2;
		min = parameter3;
		max = parameter4;
		mean = (a / (a + b))*(max - min) + min;
		stdv = (1 / (a + b) * sqrt(a*b / (a + b + 1))) *(max - min);
		break;
	case Logistic:
		mean = parameter1 + parameter2;
		stdv = parameter1*M_PI / sqrt(3.0);
		break;
	case Weibull:
		mean = parameter1*gsl_sf_gamma(1.0 + 1.0 / parameter2);
		stdv = parameter1* sqrt(gsl_sf_gamma(1 + 2.0 / parameter2) - gsl_pow_2(gsl_sf_gamma(1 + 1.0 / parameter2)));
		break;
	case Gumbel:
		mean = (log(parameter2) + M_EULER) / parameter1;
		stdv = M_PI / (parameter1 * sqrt(6.0));
		break;
#ifndef OLD_DISTRIBUTION
	case FDistribution:
		//For nu2>2 ; nu2>4.
		if (parameter2 <= 2) {
			qCritical() << "Warning: The mean is undefined for nu2 less than or equal to 2 in the F-distribution.";
			mean = std::numeric_limits<double>::quiet_NaN();
		}
		else {
			mean = parameter2 / (parameter2 - 2);
		}
		if (parameter2 <= 4) {
			qCritical() << "Warning: The standard deviation is undefined for nu2 less than or equal to 4 in the F-distribution.";
			stdv = std::numeric_limits<double>::quiet_NaN();
		}
		else {
			stdv = sqrt(2 * pow(parameter2, 2) * (parameter1 + parameter2 - 2) / parameter1 / pow(parameter2 - 2, 2) / (parameter2 - 4));
		}
		break;
	case Pareto:
		if (parameter1 <= 0 || parameter2 <= 0) {
			qCritical() << "Warning: The Pareto distribution parameters must be grater than zero.";
		}
		if (parameter1 <= 1) {
			mean = std::numeric_limits<double>::infinity();
		}
		else {
			mean = (parameter1 * parameter2) / (parameter1 - 1);
		}
		if (parameter1 > 0 && parameter1 <= 2) {
			stdv = std::numeric_limits<double>::infinity();
		}
		else {
			stdv = sqrt((pow(parameter2, 2) * parameter1) / (pow(parameter1 - 1, 2) * (parameter1 - 2)));
		}
		break;
	case type2Gumbel:
		if (parameter1 > 1) {
			mean = gsl_sf_gamma(1 - 1 / parameter1);
		}
		else {
			mean = std::numeric_limits<double>::infinity();
		}
		if (parameter1 > 2) {
			stdv = sqrt(gsl_sf_gamma(1 - 2 / parameter1) - pow(gsl_sf_gamma(1 - 1 / parameter1), 2));
		}
		else {
			stdv = std::numeric_limits<double>::infinity();
		}
		break;
	case Frechet:
		if (parameter1 > 1) {
			mean = parameter3 + parameter2 * gsl_sf_gamma(1 - 1 / parameter1);
		}
		else {
			mean = std::numeric_limits<double>::infinity();
		}
		if (parameter1 > 2) {
			stdv = parameter2 * sqrt(gsl_sf_gamma(1 - 2 / parameter1) - pow(gsl_sf_gamma(1 - 1 / parameter1), 2));
		}
		else {
			stdv = std::numeric_limits<double>::infinity();
		}
		break;
	case Arcsine:
		mean = (parameter1 + parameter2) / 2;
		stdv = (parameter2 - parameter1) / sqrt(8.0);
		break;
	case BirnbaumSaunders:
		if (parameter1 <= 0 || parameter2 <= 0) {
			qCritical() << "Warning: The parameters 1 and 2 in the Birnbaum-Saunders distribution must be grater than zero.";
		}
		mean = parameter2 * (1 + pow(parameter1, 2) / 2) + parameter3;
		stdv = parameter2 * parameter1 * sqrt(4 + 5 * pow(parameter1, 2)) / 2;
		break;
	case IrwinHall:
		if (parameter1 <= 0) {
			qCritical() << "Warning: The parameter1 in the Irwin-Hall distribution must be grater than zero.";
		}
		else {
			mean = (parameter3 + parameter2) * parameter1 / 2;
			stdv = (parameter3 - parameter2) * sqrt(parameter1 / 12);
		}
		break;
	case Wald:
		if (parameter1 <= 0 || parameter2 <= 0) {
			qCritical() << "Warning: The Wald distribution parameters must be grater than zero.";
		}
		else {
			mean = parameter1;
			stdv = sqrt(pow(parameter1, 3) / parameter2);
		}
		break;
#endif // OLD_DISTRIBUTION
	case User_Defined:
		//if (userDefinedErrorCheck() < 0) {
		//	return -1;
		//}

		for (int i = 1; i < XVector.count(); i++) {
			a = (PDFVector[i] - PDFVector[i - 1]) / (XVector[i] - XVector[i - 1]);
			b = PDFVector[i - 1] - a * XVector[i - 1];
			mean += a * (gsl_pow_3(XVector[i]) - gsl_pow_3(XVector[i - 1])) / 3.0 + 0.5 * b * (gsl_pow_2(XVector[i]) - gsl_pow_2(XVector[i - 1]));
		}

		for (int i = 1; i < XVector.count(); i++) {
			a = (PDFVector[i] - PDFVector[i - 1]) / (XVector[i] - XVector[i - 1]);
			b = PDFVector[i - 1] - a * XVector[i - 1];
			x1 = XVector[i - 1];
			x2 = XVector[i];
			stdv += 0.25 * a * (gsl_pow_4(x2) - gsl_pow_4(x1)) + (b / 3.0 - 2.0 / 3.0 * mean * a) * (gsl_pow_3(x2) - gsl_pow_3(x1)) + (0.5 * a * gsl_pow_2(mean) - mean * b) * (gsl_pow_2(x2) - gsl_pow_2(x1)) + gsl_pow_2(mean) * b * (x2 - x1);
		}
		stdv = sqrt(stdv);
		break;
	default:
		rCritical() << "Warning: Illegal probability distribution type has been passed to the random variable" << objectName() << ".";
		break;
	}

	double cov = qAbs(stdv / (mean));

	// Update the parameter values; either the Double or the Parameter
	if (!theMeanParameter) {
		theMeanDouble[threadID] = mean;
	}
	else {
		theMeanParameter.data()->setCurrentValue(mean, threadID);
	}

	if (!theStandardDeviationParameter) {
		theStandardDeviationDouble[threadID] = stdv;
	}
	else {
		theStandardDeviationParameter.data()->setCurrentValue(stdv, threadID);
	}

	if (!theCoefficientOfVariationParameter) {
		theCoefficientOfVariationDouble[threadID] = cov;
	}
	else {
		theCoefficientOfVariationParameter.data()->setCurrentValue(cov, threadID);
	}

	return 0;
}



int RContinuousRandomVariable::computeCDFSensitivityToParameters(double rvValue, double *dCDFdMean, double *dCDFdStdv, int threadID)
{
	double meanBackup = getMeanValue(threadID);
	double stdvBackup = getStandardDeviationValue(threadID);
	//double covBackup = getCoefficientOfVariationValue();
	//double parameter1Backup = getParameter1Value();
	//double parameter2Backup = getParameter2Value();
	//double parameter3Backup = getParameter3Value();
	//double parameter4Backup = getParameter4Value();
	double currentValueBackup = getCurrentValue(threadID);

	if (theDistributionType == QString("Normal (mean, stdv)") || theDistributionType == QString("Exponential (mu, xo)") || theDistributionType == QString("Laplace (a, xo)") || theDistributionType == QString("Rayleigh (sigma, xo)") || theDistributionType == QString("Gamma (a, b)") || theDistributionType == QString("Uniform (a, b)") || theDistributionType == QString("Lognormal (zeta, sigma)") || theDistributionType == QString("Chi-square (nu, xo)") || theDistributionType == QString("Beta (a, b, min, max)") || theDistributionType == QString("Logistic (a, xo)") || theDistributionType == QString("Gumbel (a, b)") || theDistributionType == QString("F-distribution (nu1, nu2)") || theDistributionType == QString("Pareto (a, b)") || theDistributionType == QString("Type-2 Gumbel (a, b)") || theDistributionType == QString("Frechet (a, s, m)") || theDistributionType == QString("Arcsine (a, b)") || theDistributionType == QString("Birnbaum-Saunders (a, b, m)") || theDistributionType == QString("Wald (mu, lambda)")) {
		double originalCDF = getCDFValue(rvValue, threadID);

		// Perturb the mean
		double mean = meanBackup;
		double h = stdvBackup / 200.0;
		mean += h;
		setMeanValue(mean, threadID);

		// Compute the sensitivity with finite difference
		double perturbedCDF = getCDFValue(rvValue, threadID);
		*dCDFdMean = (perturbedCDF - originalCDF) / h;

		// Restore the mean
		mean = meanBackup;
		setMeanValue(mean, threadID);
		setCurrentValue(currentValueBackup, threadID);



		// Perturb the stdv
		double stdv = stdvBackup;
		stdv += h;
		setStandardDeviationValue(stdv, threadID);

		// Compute the sensitivity with finite difference
		perturbedCDF = getCDFValue(rvValue, threadID);
		*dCDFdStdv = (perturbedCDF - originalCDF) / h;
		stdv = stdvBackup;

		// Restore the stdv
		setStandardDeviationValue(stdv, threadID);
		setCurrentValue(currentValueBackup, threadID);
	}
	else if (theDistributionType == QString("Bounded Exponential (mu, xo, min, max)") || theDistributionType == QString("Linear (a, b)") || theDistributionType == QString("Weibull (a, b)") || theDistributionType == QString("Irwin-Hall (n, a, b)") || theDistributionType == QString("User Defined")) {
		rCritical() << "Warning: The sensitivity of the CDF with respect to the mean and the standard deviation of the random variable" << objectName() << "cannot be computed. Setting the sensitivity to zero.";
		*dCDFdMean = 0.0;
		*dCDFdStdv = 0.0;
	}
	else {
		rCritical() << "Warning: Illegal probability distribution type has been passed to the random variable" << objectName() << ".";
		*dCDFdMean = 0.0;
		*dCDFdStdv = 0.0;
	}

	return 0;
}

int RContinuousRandomVariable::computePDFSensitivityToParameters(double rvValue, double *dPDFdMean, double *dPDFdStdv, int threadID)
{
	double meanBackup = getMeanValue(threadID);
	double stdvBackup = getStandardDeviationValue(threadID);
	double currentValueBackup = getCurrentValue(threadID);
	double Parameter1Backup = getParameter1Value(threadID);
	double Parameter2Backup = getParameter2Value(threadID);
	double Parameter3Backup = getParameter3Value(threadID);
	double Parameter4Backup = getParameter4Value(threadID);

	if (theDistributionType == QString("Normal (mean, stdv)") || theDistributionType == QString("Exponential (mu, xo)") || theDistributionType == QString("Laplace (a, xo)") || theDistributionType == QString("Rayleigh (sigma, xo)") || theDistributionType == QString("Gamma (a, b)") || theDistributionType == QString("Uniform (a, b)") || theDistributionType == QString("Lognormal (zeta, sigma)") || theDistributionType == QString("Chi-square (nu, xo)") || theDistributionType == QString("Logistic (a, xo)") || theDistributionType == QString("Gumbel (a, b)") || theDistributionType == QString("F-distribution (nu1, nu2)") || theDistributionType == QString("Pareto (a, b)") || theDistributionType == QString("Type-2 Gumbel (a, b)") || theDistributionType == QString("Frechet (a, s, m)") || theDistributionType == QString("Arcsine (a, b)") || theDistributionType == QString("Birnbaum-Saunders (a, b, m)") || theDistributionType == QString("Wald (mu, lambda)")) {
		double originalPDF = getPDFValue(rvValue, threadID);

		// Perturb the mean
		double mean = meanBackup;
		double h = stdvBackup / 200.0;
		mean += h;
		setMeanValue(mean, threadID);

		// Compute the sensitivity with finite difference
		double perturbedPDF = getPDFValue(rvValue, threadID);
		*dPDFdMean = (perturbedPDF - originalPDF) / h;

		// Restore the mean
		mean = meanBackup;
		setMeanValue(mean, threadID);
		setCurrentValue(currentValueBackup, threadID);



		// Perturb the stdv
		double stdv = stdvBackup;
		stdv += h;
		setStandardDeviationValue(stdv, threadID);

		// Compute the sensitivity with finite difference
		perturbedPDF = getPDFValue(rvValue, threadID);
		*dPDFdStdv = (perturbedPDF - originalPDF) / h;
		stdv = stdvBackup;

		// Restore the stdv
		setStandardDeviationValue(stdv, threadID);
		setCurrentValue(currentValueBackup, threadID);
	}
	else if (theDistributionType == QString("Beta (a, b, min, max)")) {
		double originalPDF = getPDFValue(rvValue, threadID);

		// Perturb the parameters 3 and 4
		double mean = meanBackup;
		double h = stdvBackup / 200.0;
		double par1 = Parameter1Backup;
		double par2 = Parameter2Backup;
		double par3 = Parameter3Backup;
		double par4 = Parameter4Backup;
		mean += h;
		setMeanValue(mean, threadID);
		par3 += h;
		setParameter3Value(par3, threadID);
		par4 += h;
		setParameter4Value(par4, threadID);

		// Compute the sensitivity with finite difference
		double perturbedPDF = getPDFValue(rvValue, threadID);
		*dPDFdMean = (perturbedPDF - originalPDF) / h;

		// Restore the mean
		mean = meanBackup;
		par3 = Parameter3Backup;
		par4 = Parameter4Backup;
		setParameter3Value(par3, threadID);
		setParameter4Value(par4, threadID);
		setMeanValue(mean, threadID);
		setCurrentValue(currentValueBackup, threadID);



		// Perturb the stdv
		double stdv = stdvBackup;
		stdv += h;
		setStandardDeviationValue(stdv, threadID);

		// Compute the sensitivity with finite difference
		perturbedPDF = getPDFValue(rvValue, threadID);
		*dPDFdStdv = (perturbedPDF - originalPDF) / h;
		stdv = stdvBackup;

		// Restore the stdv
		setStandardDeviationValue(stdv, threadID);
		setCurrentValue(currentValueBackup, threadID);
	}
	else if (theDistributionType == QString("Bounded Exponential (mu, xo, min, max)") || theDistributionType == QString("Linear (a, b)") || theDistributionType == QString("Weibull (a, b)") || theDistributionType == QString("Irwin-Hall (n, a, b)") || theDistributionType == QString("User Defined")) {
		rCritical(threadID) << "Warning: The sensitivity of the PDF with respect to the mean and the standard deviation of the random variable" << objectName() << "cannot be computed. Setting the sensitivity to zero.";
		*dPDFdMean = 0.0;
		*dPDFdStdv = 0.0;
	}
	else {
		rCritical(threadID) << "Warning: Illegal probability distribution type has been passed to the random variable" << objectName() << ".";
		*dPDFdMean = 0.0;
		*dPDFdStdv = 0.0;
	}

	return 0;
}

int RContinuousRandomVariable::computeInverseCDFSensitivityToParameters(double probValue, double *dInverseCDFdMean, double *dInverseCDFdStdv, int threadID)
{
	double meanBackup = getMeanValue(threadID);
	double stdvBackup = getStandardDeviationValue(threadID);
	double currentValueBackup = getCurrentValue(threadID);
	double Parameter1Backup = getParameter1Value(threadID);
	double Parameter2Backup = getParameter2Value(threadID);
	double Parameter3Backup = getParameter3Value(threadID);
	double Parameter4Backup = getParameter4Value(threadID);

	if (theDistributionType == QString("Normal (mean, stdv)") || theDistributionType == QString("Exponential (mu, xo)") || theDistributionType == QString("Laplace (a, xo)") || theDistributionType == QString("Rayleigh (sigma, xo)") || theDistributionType == QString("Gamma (a, b)") || theDistributionType == QString("Uniform (a, b)") || theDistributionType == QString("Lognormal (zeta, sigma)") || theDistributionType == QString("Chi-square (nu, xo)") || theDistributionType == QString("Logistic (a, xo)") || theDistributionType == QString("Gumbel (a, b)") || theDistributionType == QString("F-distribution (nu1, nu2)") || theDistributionType == QString("Pareto (a, b)") || theDistributionType == QString("Type-2 Gumbel (a, b)") || theDistributionType == QString("Frechet (a, s, m)") || theDistributionType == QString("Arcsine (a, b)") || theDistributionType == QString("Birnbaum-Saunders (a, b, m)")) {
		double originalInverseCDF = getInverseCDFValue(probValue, threadID);

		// Perturb the mean
		double mean = meanBackup;
		double h = stdvBackup / 200.0;
		mean += h;
		setMeanValue(mean, threadID);

		// Compute the sensitivity with finite difference
		double perturbedInverseCDF = getInverseCDFValue(probValue, threadID);
		*dInverseCDFdMean = (perturbedInverseCDF - originalInverseCDF) / h;

		// Restore the mean
		mean = meanBackup;
		setMeanValue(mean, threadID);
		setCurrentValue(currentValueBackup, threadID);



		// Perturb the stdv
		double stdv = stdvBackup;
		stdv += h;
		setStandardDeviationValue(stdv, threadID);

		// Compute the sensitivity with finite difference
		perturbedInverseCDF = getInverseCDFValue(probValue, threadID);
		*dInverseCDFdStdv = (perturbedInverseCDF - originalInverseCDF) / h;
		stdv = stdvBackup;

		// Restore the stdv
		setStandardDeviationValue(stdv, threadID);
		setCurrentValue(currentValueBackup, threadID);
	}
	else if (theDistributionType == QString("Beta (a, b, min, max)")) {
		double originalInverseCDF = getInverseCDFValue(probValue, threadID);

		// Perturb the parameters 3 and 4
		double mean = meanBackup;
		double h = stdvBackup / 200.0;
		double par1 = Parameter1Backup;
		double par2 = Parameter2Backup;
		double par3 = Parameter3Backup;
		double par4 = Parameter4Backup;

		// For Beta distribution, if moments can not are set other parameters change. So, to keep stdv, a and b constant and change the mean as much as h, the following is done:
		mean += h;
		setMeanValue(mean, threadID);
		par3 += h;
		setParameter3Value(par3, threadID);
		par4 += h;
		setParameter4Value(par4, threadID);
		// Compute the sensitivity with finite difference
		double perturbedInverseCDF = getInverseCDFValue(probValue, threadID);
		*dInverseCDFdMean = (perturbedInverseCDF - originalInverseCDF) / h;

		// Restore the mean
		mean = meanBackup;
		setMeanValue(mean, threadID);
		par3 = Parameter3Backup;
		par4 = Parameter4Backup;
		setParameter3Value(par3, threadID);
		setParameter4Value(par4, threadID);
		setCurrentValue(currentValueBackup, threadID);



		// Perturb the stdv
		double stdv = stdvBackup;

		// For Beta distribution, if moments can not are set other parameters change. So, to keep mean, a and b constant and change the stdv as much as h, the following is done:
		double temp = h / (1.0 / (par1 + par2) * sqrt(par1*par2 / (1 + par1 + par2)));
		stdv += h;
		setStandardDeviationValue(stdv, threadID);
		par3 -= temp / 2.0;
		setParameter3Value(par3, threadID);
		par4 += temp / 2.0;;
		setParameter4Value(par4, threadID);

		// Compute the sensitivity with finite difference
		perturbedInverseCDF = getInverseCDFValue(probValue, threadID);
		*dInverseCDFdStdv = (perturbedInverseCDF - originalInverseCDF) / h;

		// Restore the stdv
		stdv = stdvBackup;
		setStandardDeviationValue(stdv, threadID);
		par3 = Parameter3Backup;
		par4 = Parameter4Backup;
		setParameter3Value(par3, threadID);
		setParameter4Value(par4, threadID);
		setCurrentValue(currentValueBackup, threadID);
	}
	else if (theDistributionType == QString("Bounded Exponential (mu, xo, min, max)") || theDistributionType == QString("Linear (a, b)") || theDistributionType == QString("Weibull (a, b)") || theDistributionType == QString("Irwin-Hall (n, a, b)") || theDistributionType == QString("Wald (mu, lambda)") || theDistributionType == QString("User Defined")) {
		rCritical(threadID) << "Warning: The sensitivity of the InverseCDF with respect to the mean and the standard deviation of the random variable" << objectName() << "cannot be computed. Setting the sensitivity to zero.";
		*dInverseCDFdMean = 0.0;
		*dInverseCDFdStdv = 0.0;
	}
	else {
		rCritical(threadID) << "Warning: Illegal probability distribution type has been passed to the random variable" << objectName() << ".";
		*dInverseCDFdMean = 0.0;
		*dInverseCDFdStdv = 0.0;
	}

	return 0;
}


QList<RParameter *> RContinuousRandomVariable::getParameterList()
{
	QList<RParameter *> theParameterList;
	if (theMeanParameter) {
		theParameterList << theMeanParameter.data();
	}
	if (theStandardDeviationParameter) {
		theParameterList << theStandardDeviationParameter.data();
	}
	if (theCoefficientOfVariationParameter) {
		theParameterList << theCoefficientOfVariationParameter.data();
	}
	if (theParameter1Parameter) {
		theParameterList << theParameter1Parameter.data();
	}
	if (theParameter2Parameter) {
		theParameterList << theParameter2Parameter.data();
	}
	if (theParameter3Parameter) {
		theParameterList << theParameter3Parameter.data();
	}
	if (theParameter4Parameter) {
		theParameterList << theParameter4Parameter.data();
	}
	return theParameterList;
}

void RContinuousRandomVariable::plot(QString type)
{

#ifndef NO_QCP
	RMainWindow *mainWindow = qobject_cast<RMainWindow *>(parent()->parent());
	QCustomPlot *thePlot = new RPlotWidget(mainWindow);
	mainWindow->addSubWindow(thePlot, objectName() + QString(" %1 Plot").arg(type));
	QString xAxisLabel;
	if (type == QString("Inverse CDF")) {
		xAxisLabel = "Probability";
	}
	else {
		xAxisLabel = "Random Variable";
	}

	thePlot->setInteraction(QCP::iRangeDrag, true);
	thePlot->setInteraction(QCP::iRangeZoom);

	QList<QCPAxis*> axisrect;
	axisrect << thePlot->xAxis << thePlot->yAxis;
	thePlot->axisRect()->setRangeZoomAxes(axisrect);
	thePlot->xAxis->setLabel(xAxisLabel);
	thePlot->yAxis->setLabel(type);

	thePlot->addGraph(thePlot->xAxis, thePlot->yAxis);
	thePlot->graph(0)->setAntialiased(true);
	thePlot->graph(0)->setPen(QPen(Qt::red));
	thePlot->graph(0)->setName(type);
	thePlot->xAxis->setPadding(10);
	thePlot->yAxis2->setPadding(10);
	thePlot->axisRect()->setAutoMargins(QCP::msAll);
	thePlot->axisRect()->setMinimumMargins(QMargins(20, 20, 20, 20));
	thePlot->axisRect()->setMargins(QMargins(20, 20, 20, 20));

	//thePlot->yAxis->axisRect()->layout()->setAutoMargins(QCP::msAll);


	//thePlot->xAxis->setPadding(10);
	//thePlot->yAxis2->setPadding(10);

	thePlot->legend->setVisible(true);

	QVector<double> xVector;
	QVector<double> yVector;

	double mean = getMeanValue(0);
	double standardDeviation = getStandardDeviationValue(0);
	int pointsCount = 1000;

	for (int i = 0; i <= pointsCount; i++) {
		double x = (2.0 * i - pointsCount) / pointsCount * 3.0 * standardDeviation + mean;
		double y;
		if (type == ("PDF")) {
			y = getPDFValue(x, 0);
		}
		else if (type == ("CDF")) {
			y = 0.0;
			double temp = getCDFValue(x, 0);
			if (temp >= 0.0 && temp <= 1.0) {
				y = temp;
			}
		}
		else if (type == ("Inverse CDF")) {
			if ((i == 0) || (i == pointsCount)) {
				continue;
			}
			x = 1.0 * i / pointsCount;
			y = getInverseCDFValue(x, 0);
		}
		xVector << x;
		yVector << y;
	}

	thePlot->graph(0)->setData(xVector, yVector);

	//thePlot->xAxis->rescale(true);
	//thePlot->yAxis->rescale(true);
	thePlot->graph(0)->rescaleAxes();
	thePlot->xAxis->scaleRange(1.1);
	thePlot->yAxis->scaleRange(1.1);
	thePlot->replot();
#endif // NO_QCP

}


void RContinuousRandomVariable::setDynamicProperty(QString propertyName, QVariant propertyValue)
{
	if (QString(propertyName) == QString("XPoints")) {
		XVector = stringToDoubleVector(propertyValue.toString());
		QString temp = doubleVectorToString(XVector);
		if (propertyValue != temp) {
			this->setProperty("XPoints", temp);
		}
	}
	else if (QString(propertyName) == QString("PDFPoints")) {
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
	updateMeanStdv(0);
	setCurrentValue(getMeanValue(0));
	double lastCDF = getCDFValue(XVector.last(), 0);
	if (qAbs(lastCDF - 1.0) > 1e-10) {
		for (int i = 0; i < PDFVector.count(); i++) {
			PDFVector[i] /= lastCDF;
		}
		updateMeanStdv(0);
		setCurrentValue(getMeanValue(0));

		if (viaGUI) {
			rCritical() << "Warning: The total area under the given PDF curve of the user-defined random variable" << objectName() << "is not equal to 1.0. Rt will normalize the given PDF.";
			this->setProperty("PDFPoints", doubleVectorToString(PDFVector));
		}
	}
}

int RContinuousRandomVariable::userDefinedErrorCheck()
{
	if (XVector.isEmpty() || PDFVector.isEmpty()) {
		rCritical() << "Error: The X points and/or the PDF points in the user-defined random variable" << objectName() << "is empty. The distribution is returned as 0.0.";
		return -1;
	}
	if (XVector.count() != PDFVector.count()) {
		rCritical() << "Error: The number of X points and PDF points in the user-defined random variable" << objectName() << "are not equal. The distribution is returned as 0.0.";
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
			rDebug() << "The PDF of the random variable" << objectName() << "at" << value << "is" << getPDFValue(value, 0);
		}
		else if (type == QString("CDF")) {
			rDebug() << "The CDF of the random variable" << objectName() << "at" << value << "is" << getCDFValue(value, 0);
		}
		else if (type == QString("Inverse CDF")) {
			rDebug() << "The inverse CDF of the random variable" << objectName() << "at" << value << "is" << getInverseCDFValue(value, 0);
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

void RContinuousRandomVariable::initializeForParallelComputation(int threadID) {

	RRandomVariable::initializeForParallelComputation(threadID);

	theMeanDouble[threadID] = theMeanDouble[0];
	theStandardDeviationDouble[threadID] = theStandardDeviationDouble[0];
	theCoefficientOfVariationDouble[threadID] = theCoefficientOfVariationDouble[0];
	theParameter1Double[threadID] = theParameter1Double[0];
	theParameter2Double[threadID] = theParameter2Double[0];
	theParameter3Double[threadID] = theParameter3Double[0];
	theParameter4Double[threadID] = theParameter4Double[0];
	theIncrementalCostOfMeanDouble[threadID] = theIncrementalCostOfMeanDouble[0];
	theIncrementalCostOfStandardDeviationDouble[threadID] = theIncrementalCostOfStandardDeviationDouble[0];
}

bool RContinuousRandomVariable::canBeRunParallelly() {
	return true;
}