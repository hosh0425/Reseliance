
#include "RExampleAnalyzer.h"
#include "RTransformer.h"
#include "RFunction.h"
#include "RRandomVariable.h"
#include <gsl/gsl_vector.h>

RExampleAnalyzer::RExampleAnalyzer(QObject *parent, QString name)
	: RObject(parent, name)
{
	// Initializing data members
	theDoubleInput = 1.0;
	theIntegerInput = 1;
	theBooleanInput = false;
	//$$ theTransformer = 0;
}

RExampleAnalyzer::~RExampleAnalyzer()
{

}

double RExampleAnalyzer::getDoubleInput() const	
{	
	return theDoubleInput;
}	

void RExampleAnalyzer::setDoubleInput(double value)	
{	
	theDoubleInput = value;
}	

int RExampleAnalyzer::getIntegerInput() const	
{	
	return theIntegerInput;
}	

void RExampleAnalyzer::setIntegerInput(int value)	
{	
	theIntegerInput = value;
}	

QString RExampleAnalyzer::getStringInput() const	
{	
	return theStringInput;
}	

void RExampleAnalyzer::setStringInput(QString value)	
{	
	theStringInput = value;
}	

bool RExampleAnalyzer::getBooleanInput() const	
{	
	return theBooleanInput;
}	

void RExampleAnalyzer::setBooleanInput(bool value)	
{	
	theBooleanInput = value;
}	

QObject * RExampleAnalyzer::getTransformer() const	
{	
	return theTransformer.data();
}	

void RExampleAnalyzer::setTransformer(QObject *value)	
{	
	theTransformer = qobject_cast<RTransformer *>(value);
}

int RExampleAnalyzer::analyze(RFunction *theFunction)	
{	
	// This method is where the analysis algorithm is implemented


	// Checking if all pointer properties are given by the user
	int status = this->checkProperties();

	// Error handling
	if (status < 0) {

		// Printing an error message to the output pane
		qCritical() << "Error in" << this->objectName() << ":"
					<< "Could not find all the passed objects";
		
		// Returning the error code
		return -1;
	}

	// Parsing the function and pouplating its parameter lists
	status = theFunction->populateLists();
	if (status < 0) {
		qCritical() << "Error in" << this->objectName() << ":"
					<< "Could not parse the function"
					<< theFunction->objectName();
		return -1;
	}

	// Getting the list of random variables that affect the function
	QList<RRandomVariable *> *rvList = theFunction->getRandomVariableList();

	// Getting the number of random variables in rvList
	int numRVs = rvList->count();
	if (numRVs == 0) {
		qCritical() << "Error in" << this->objectName() << ":"
					<< "The function" << theFunction->objectName()
					<< "is not a function of any random variables";
		return -1;
	}

	// Allocating vector u with the size numRVs
	// u represents the random variable vector in the st. normal space
	gsl_vector *u = gsl_vector_calloc(numRVs);

	// Allocating vector x with the size numRVs
	// x represents the random variable vector in the original space
	gsl_vector *x = gsl_vector_calloc(numRVs);
		
	// Setting the random variable list in theTransformer
	// This also resets theTransfomer for a new analysis
	status = theTransformer.data()->setRandomVariableList(rvList);
	if (status < 0) {
		qCritical() << "Error in" << this->objectName() << ":"
					<< "Could not set the random variable list"
					<< "in the transformer";
		return -1;
	}

	// Transforming vector u to vector x
	status = theTransformer.data()->transformPointToOriginalSpace(u, x,0);
	if (status < 0) {
		qCritical() << "Error in" << this->objectName() << ":"
					<< "Could not transform the point to original space";
		return -1;
	}

	// Updating random variables parameters using vector x
	for (int i = 0; i < numRVs; i++) {
		double xi = gsl_vector_get(x, i);
		rvList->value(i)->setCurrentValue(xi);
	}

	// Updating the isAnalyzed flag of RResponses
	// due to the new realization of random variables
	theFunction->updateAnalysisStatus(0);

	// Evaluating the function
	status = theFunction->evaluateFunction(true, RObject::RandomVariable,0);
	if (status < 0) {
		qCritical() << "Error in" << this->objectName() << ":"
					<< "Could not evaluate the function" 
					<< theFunction->objectName();
		return -1;
	}

	// Getting the function value
	double fn = theFunction->getFunctionValue();

	// Evaluating the gradient of the function
	status = theFunction->evaluateGradient(RObject::RandomVariable);

	// Getting the gradient
	// Attention: "functionGradient" should already be allocated in "theFunction", here just the memory address is obtained.
	// Attention: the flag to evaluate the gradient with the function in the RFunction::evaluateFunction should be passed as true
	gsl_vector *gradient = theFunction->getGradient();
	if (status < 0) {
		qCritical() << "Error in" << this->objectName() << ":"
					<< "Could not evaluate the gradient of the function" 
					<< theFunction->objectName();
		return -1;
	}

	double result;
	if (theBooleanInput) {
		result = theDoubleInput * fn;
	} else {
		result = theIntegerInput * fn;
	}

	// Printing the result to the outpaut pane
	qDebug() << theStringInput << ":" << result;

	// Freeing vectors x and u
	gsl_vector_free(x);
	gsl_vector_free(u);

	// Returning 0, meaning that the analysis is carried out successfully
	return 0;
}	
