#include "RRandomVariableWithRandomParametersModel.h"

#include "RParameter.h"
#include "RResponse.h"
#include "RContinuousRandomVariable.h"

#include <gsl/gsl_math.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_cdf.h>

RRandomVariableWithRandomParametersModel::RRandomVariableWithRandomParametersModel(QObject *parent, QString name)
	: RModel(parent, name)
{
	//$$ theStandardNormalRandomVariable = 0;
	theDistributionType = "Normal (mean, stdv)";
	theRandomParameterType = RRandomVariableWithRandomParametersModel::Parameter1ToParameter4;
	
	// Instantiating the output response theResponse
	theDomain->createObject(objectName() + "Response", "RGenericResponse");
	theResponse = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theResponse->setModel(this);
}

RRandomVariableWithRandomParametersModel::~RRandomVariableWithRandomParametersModel()
{
}

QObject * RRandomVariableWithRandomParametersModel::getStandardNormalRandomVariable() const
{
	return theStandardNormalRandomVariable.data();
}

void RRandomVariableWithRandomParametersModel::setStandardNormalRandomVariable(QObject *value)
{
	
	theStandardNormalRandomVariable = qobject_cast<RParameter *>(value);

	/*// Check that the user has given a standard random variable:
	RContinuousRandomVariable *theRV = qobject_cast<RContinuousRandomVariable *>(value);  
	if (!theRV) {
		rCritical() << "Warning: The parameter given to the" << objectName() << "as the StandardNormalRandomVariable must be a continuous standard normal random variable.";
		return;
	}

	QString type = theRV->getDistributionType();
	double mean = theRV->getParameter1Value();
	double stdv = theRV->getParameter2Value();
	if ((type != QString("Normal (mean, stdv)")) || (mean != 0.0) || (stdv != 1.0)) {
		rCritical() << "Warning: The random variable given to the" << objectName() << "as the StandardNormalRandomVariable must be a standard normal random variable.";
	}*/
}

QString RRandomVariableWithRandomParametersModel::getDistributionType() const	
{	
	return theDistributionType;
}	

void RRandomVariableWithRandomParametersModel::setDistributionType(QString value)	
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
}	

void RRandomVariableWithRandomParametersModel::setRandomParameterType(RRandomVariableWithRandomParametersModel::RRandomParameterType value)	
{	
	theRandomParameterType = value;
}	

RRandomVariableWithRandomParametersModel::RRandomParameterType RRandomVariableWithRandomParametersModel::getRandomParameterType() const	
{	
	return theRandomParameterType;
}	

QString RRandomVariableWithRandomParametersModel::getParameterList()
{
	return parameterListToString(theParameterList);
}

void RRandomVariableWithRandomParametersModel::setParameterList(QString value)
{
	theParameterList = stringToParameterList(value);
}

QList<RParameter *> RRandomVariableWithRandomParametersModel::getPhysicalParameterList()
{
	QList<RParameter *> paramList;
	paramList << theStandardNormalRandomVariable.data();
	return paramList;
}

int RRandomVariableWithRandomParametersModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
{	
	/*
	// Clearing null pointers
	nullRemoverLock->lock();
	clearNullPointers(&theParameterList);
	nullRemoverLock->unlock();
	*/

	// Calculate the probability of the model StandardNormalRandomVariable
	double realization = theStandardNormalRandomVariable.data()->getCurrentValue(threadID);
	double probability = gsl_cdf_gaussian_P(realization, 1.0);

	// As the random variable object is defined inside the method = there is a separate variable for each thread => GUI setters and getters can be called on that without any conflicts
	RContinuousRandomVariable randomVariable(0, QString("RV"));
	randomVariable.setDistributionType(theDistributionType);

	if (theDistributionType == QString("User Defined")) {
		randomVariable.setDynamicProperty("XPoints", this->property("XPoints"));
		randomVariable.setDynamicProperty("PDFPoints", this->property("PDFPoints"));
	} else if (theRandomParameterType == RRandomVariableWithRandomParametersModel::Parameter1ToParameter4) {
		if (theParameterList.count() >= 1) {
			double parameterValue = theParameterList[0].data()->getCurrentValue(threadID);
			randomVariable.setParameter1(QString("%1").arg(parameterValue));
		}
		if (theParameterList.count() >= 2) {
			double parameterValue = theParameterList[1].data()->getCurrentValue(threadID);
			randomVariable.setParameter2(QString("%1").arg(parameterValue));
		}
		if (theParameterList.count() >= 3) {
			double parameterValue = theParameterList[2].data()->getCurrentValue(threadID);
			randomVariable.setParameter3(QString("%1").arg(parameterValue));
		}
		if (theParameterList.count() >= 4) {
			double parameterValue = theParameterList[3].data()->getCurrentValue(threadID);
			randomVariable.setParameter4(QString("%1").arg(parameterValue));
		}
	} else if (theRandomParameterType == RRandomVariableWithRandomParametersModel::MeanStdvParameter3Parameter4) {
		if (theParameterList.count() >= 3) {
			double parameterValue = theParameterList[2].data()->getCurrentValue(threadID);
			randomVariable.setParameter3(QString("%1").arg(parameterValue));
		}
		if (theParameterList.count() >= 4) {
			double parameterValue = theParameterList[3].data()->getCurrentValue(threadID);
			randomVariable.setParameter4(QString("%1").arg(parameterValue));
		}
		if (theParameterList.count() >= 1) {
			double parameterValue = theParameterList[0].data()->getCurrentValue(threadID);
			randomVariable.setMean(QString("%1").arg(parameterValue));
		}
		if (theParameterList.count() >= 2) {
			double parameterValue = theParameterList[1].data()->getCurrentValue(threadID);
			randomVariable.setStandardDeviation(QString("%1").arg(parameterValue));
		}
	} 

	// the method is called on threadID = 0 of the object because only the values of this thread for the object is set (as the object is created inside the method)
	double response = randomVariable.getInverseCDFValue(probability, 0);

	// Updating the response current value
	theResponse->setCurrentValue(response, threadID);

	return 0;
}

void RRandomVariableWithRandomParametersModel::setDynamicProperty(QString propertyName, QVariant propertyValue)
{
	if (QString(propertyName) == QString("XPoints")) {
		QVector<double> XVector = stringToDoubleVector(propertyValue.toString());
		QString temp = doubleVectorToString(XVector);
		if (propertyValue != temp) {
			this->setProperty("XPoints", temp);
		}
	} else if (QString(propertyName) == QString("PDFPoints")) {
		QVector<double> PDFVector = stringToDoubleVector(propertyValue.toString());
		QString temp = doubleVectorToString(PDFVector);
		if (propertyValue != temp) {
			this->setProperty("PDFPoints", temp);
		}
	}
}

bool RRandomVariableWithRandomParametersModel::canBeRunParallelly(){
	return true;
}