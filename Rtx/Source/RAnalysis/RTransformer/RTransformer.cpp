#include "RTransformer.h"

#include "RDomain.h"
#include "RRandomVariable.h"
#include "RContinuousRandomVariable.h"

#include <gsl/gsl_randist.h>
#include <gsl/gsl_cdf.h>

#include <math.h>
#include <QMetaProperty>

RTransformer::RTransformer(QObject *parent, QString name)
	: RObject(parent, name)
{
	theDomain = qobject_cast<RDomain *>(parent);
	randomVariablesCount = 0;
	randomVariableList = 0;
	//$$ theHelperTransformer = 0;
}

RTransformer::~RTransformer()
{
	if (theHelperTransformer)
		delete theHelperTransformer.data();
}

void RTransformer::setOutputDisplayLevel(ROutputDisplayLevelType value){

	// changing its own property from RObject
	RObject::setOutputDisplayLevel(value);

	// changing the helper transformer's property
	if (theHelperTransformer)
		theHelperTransformer.data()->setOutputDisplayLevel(value);
}

int RTransformer::setRandomVariableList(QList<RRandomVariable *> *passedRandomVariableList)
{
	// Assign the random variable list
	randomVariableList = passedRandomVariableList;
	randomVariablesCount = randomVariableList->count();

	int result = initializeAtStartOfAnalysis();
	if (result < 0) {
		rCritical() << "Error: Could not establish the correlation matrix.";
		return -1;
	}

	return 0;
}

int RTransformer::computeDzDMeanStdv(RContinuousRandomVariable *randomVariable, double x, double z, double *DzDMean, double *DzDStdv, int threadID)
{
	int status = 0;

	QString distributionType = randomVariable->getDistributionType();

	double dzdMean = 0.0;
	double dzdStdv = 0.0;
	if (distributionType == QString("Normal (mean, stdv)")) {
		double mean = randomVariable->getParameter1Value(threadID);
		double stdv = randomVariable->getParameter2Value(threadID);
		dzdMean = -1.0 / stdv;
		dzdStdv = -(x - mean) / (stdv * stdv);
	}
	else if (distributionType == QString("Uniform (a, b)")) {
		double a = randomVariable->getParameter1Value(threadID);
		double b = randomVariable->getParameter2Value(threadID);
		double pz = 0.39894228048 * exp(-z * z / 2.0);
		dzdMean = -1.0 / (pz * (b - a));
		dzdStdv = 1.732050807 * (a + b - 2.0 * x) * (-dzdMean / (b - a));
	}
	else if (distributionType == QString("Lognormal (zeta, sigma)")) {
		double mean = randomVariable->getMeanValue(threadID);
		double stdv = randomVariable->getStandardDeviationValue(threadID);
		double a = mean * mean + stdv * stdv;
		dzdMean = 0.5 * (-2.0 * mean * mean * log(a) + 4.0 * mean * mean * log(mean) - 3.0 * stdv * stdv * log(a) + 4.0 * stdv * stdv * log(mean) + 2.0 * stdv * stdv * log(x)) / (pow(log(a) - 2.0 * log(mean), 1.5) * mean * a);
		dzdStdv = 0.5 * stdv * (log(a) - 2.0 * log(x)) / (pow(log(a) - 2.0 * log(mean), 1.5) * a);
	}
	else {
		double rvCDF = randomVariable->getCDFValue(x,threadID);
		double factor = 1.0 / gsl_ran_gaussian_pdf(gsl_cdf_gaussian_Pinv(rvCDF, 1.0), 1.0);
		double dCDFdMean;
		double dCDFdStdv;
		status = randomVariable->computeCDFSensitivityToParameters(x, &dCDFdMean, &dCDFdStdv,threadID);
		dzdMean = dCDFdMean * factor;
		dzdStdv = dCDFdStdv * factor;
	}

	*DzDMean = dzdMean;
	*DzDStdv = dzdStdv;

	return 0;
}

double RTransformer::dphi_dui(const gsl_vector *u, int ui)
{
	double result = 1;
	for (int i = 0; i < u->size; i++) {
		if (i == ui) {
			result *= -1.0 * gsl_vector_get(u, i) * gsl_ran_ugaussian_pdf(gsl_vector_get(u, i));
		}
		else
		{
			result *= gsl_ran_ugaussian_pdf(gsl_vector_get(u, i));
		}
	}
	return result;
}

double RTransformer::phi(const gsl_vector *u)
{
	double result = 1;
	for (int i = 0; i < u->size; i++) {
		result *= gsl_ran_ugaussian_pdf(gsl_vector_get(u, i));
	}
	return result;
}

RPointer<RTransformer> RTransformer::createHelperTransformer(){

	if (theHelperTransformer)
		delete theHelperTransformer.data();

	theHelperTransformer = createNewTransformerOfTheSameType();

	// setting all properties to the properties of the object
	// Looping over QProperties
	for (int i = 0; i < this->metaObject()->propertyCount(); i++) {
		
		// Getting the meta property
		QMetaProperty metaProperty = this->metaObject()->property(i);
		QVariant::Type type = metaProperty.type();
		const char * propertyName = metaProperty.name();
		
		theHelperTransformer.data()->setProperty(propertyName, this->property(propertyName));
	}

	// setting the parent to null
	theHelperTransformer.data()->setParent(0);

	return theHelperTransformer;
}