#include "RRootFindingModel.h"

#include "RParameter.h"
#include "RResponse.h"

#include <gsl/gsl_errno.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_roots.h>

#include <qmutex.h>
#include <qreadwritelock.h>
#include <QMutexLocker>


double function(double x, void *params);
double functionDiff(double x, void *params);
void functionFdF(double x, void *params, double *y, double *dy);
RRootFindingModel *thisSolverModel;

// a lock for preventing reexecution of the parse method in several threads
QMutex * parseLock;

RRootFindingModel::RRootFindingModel(QObject *parent, QString name)
	: RModel(parent, name)
{
	theParser = 0;

	theAlgorithm = RRootFindingModel::Newton;
	theMaximumIterations = 100;
	theTolerance = 0.001;
	theInitialRoot = 1;
	theUpperBound = 10;
	theLowerBound = 0;
	
	theDomain->createObject(objectName() + "Response", "RGenericResponse");
	theResponse = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theResponse->setModel(this);

	thisSolverModel = this;

	parseLock = new QMutex();
	theMaximumIterationsLock = new QReadWriteLock();
}

RRootFindingModel::~RRootFindingModel()
{
	
	// free the allocated memory for all parsers in the array of parsers
	reset();

	// delete the lock
	delete parseLock;
	delete theMaximumIterationsLock;
}

QString RRootFindingModel::getExpression() const	
{	
	return theExpression;
}	
	
void RRootFindingModel::setExpression(QString value)	
{	
	theExpression = value;
	reset();
}	
	
RRootFindingModel::RAlgorithm RRootFindingModel::getAlgorithm() const	
{	
	return theAlgorithm;
}	
	
void RRootFindingModel::setAlgorithm(RRootFindingModel::RAlgorithm value)	
{	
	theAlgorithm = value;
}	
	
double RRootFindingModel::getMaximumIterations() const	
{	
	double value;

	theMaximumIterationsLock->lockForRead();
	value = theMaximumIterations;
	theMaximumIterationsLock->unlock();

	return value;
}	
	
void RRootFindingModel::setMaximumIterations(double value)	
{	
	theMaximumIterationsLock->lockForWrite();
	theMaximumIterations = value;
	theMaximumIterationsLock->unlock();
}	
	
double RRootFindingModel::getTolerance() const	
{	
	return theTolerance;
}	
	
void RRootFindingModel::setTolerance(double value)	
{	
	theTolerance = value;
}	
	
double RRootFindingModel::getInitialRoot() const	
{	
	return theInitialRoot;
}	
	
void RRootFindingModel::setInitialRoot(double value)	
{	
	theInitialRoot = value;
}	
	
double RRootFindingModel::getUpperBound() const	
{	
	return theUpperBound;
}	
	
void RRootFindingModel::setUpperBound(double value)	
{	
	theUpperBound = value;
}	
	
double RRootFindingModel::getLowerBound() const	
{	
	return theLowerBound;
}	
	
void RRootFindingModel::setLowerBound(double value)	
{	
	theLowerBound = value;
}	

int RRootFindingModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
{	
	int result;
	int iterationIndex = 0;
	double x; 
	//gsl_set_error_handler_off();

	if ((theAlgorithm == RRootFindingModel::Newton) || (theAlgorithm == RRootFindingModel::Secant) || (theAlgorithm == RRootFindingModel::Steffenson)) {
		const gsl_root_fdfsolver_type *type;
		gsl_root_fdfsolver *solver;
		double x0;
		x = theInitialRoot; 
		gsl_function_fdf GSLFunction;
		GSLFunction.f = &function;
		GSLFunction.df = &functionDiff;
		GSLFunction.fdf = &functionFdF;

		// setting threadID as a parameter
		GSLFunction.params = &threadID;

		if (theAlgorithm == RRootFindingModel::Newton) {
			type = gsl_root_fdfsolver_newton;
		} else if (theAlgorithm == RRootFindingModel::Secant) {
			type = gsl_root_fdfsolver_secant;
		} else if (theAlgorithm == RRootFindingModel::Steffenson) {
			type = gsl_root_fdfsolver_steffenson;
		}

		solver = gsl_root_fdfsolver_alloc(type);
		gsl_root_fdfsolver_set(solver, &GSLFunction, x);
		
		do {
			iterationIndex++;
			result = gsl_root_fdfsolver_iterate(solver);
			x0 = x;
			x = gsl_root_fdfsolver_root(solver);
			result = gsl_root_test_delta(x, x0, 0, theTolerance);
			if (result == GSL_SUCCESS) {
				theResponse->setCurrentValue(x, threadID);
				return 0;
			}
		} while (/*result == GSL_CONTINUE && */iterationIndex < getMaximumIterations());
	}
	else if ((theAlgorithm == RRootFindingModel::Bisection) || (theAlgorithm == RRootFindingModel::FalsePosition) || (theAlgorithm == RRootFindingModel::BrentDekker)) {
		const gsl_root_fsolver_type *type;
		gsl_root_fsolver *solver;
		double xLow = theLowerBound;
		double xHigh = theUpperBound;
		gsl_function GSLFunction;

		GSLFunction.function = &function;

		if (theAlgorithm == RRootFindingModel::Bisection) {
			type = gsl_root_fsolver_bisection;
		} else if (theAlgorithm == RRootFindingModel::FalsePosition) {
			type = gsl_root_fsolver_brent;
		} else if (theAlgorithm == RRootFindingModel::BrentDekker) {
			type = gsl_root_fsolver_falsepos;
		}

		solver = gsl_root_fsolver_alloc(type);
		gsl_root_fsolver_set(solver, &GSLFunction, xLow, xHigh);

		do {
			iterationIndex++;
			result = gsl_root_fsolver_iterate(solver);
			x = gsl_root_fsolver_root(solver);
			xLow = gsl_root_fsolver_x_lower(solver);
			xHigh = gsl_root_fsolver_x_upper(solver);
			result = gsl_root_test_interval (xLow, xHigh, 0, theTolerance);
			if (result == GSL_SUCCESS) {
				theResponse->setCurrentValue(x, threadID);
				return 0;
			}
		} while (/*result == GSL_CONTINUE && */iterationIndex < getMaximumIterations());
	}

	rCritical(threadID) << "Error in model" << objectName() << ": The maximum iterations is reached without convergence.";
	return -1;
}

int RRootFindingModel::parseExpression()
{
	try {

		if (theParser) {
			reset();
		}

		int arraySizeForParallelizedObject = getArraySizeForParallelizedObject();

		// Instatiating "theParser" object. We use MuParser package in order to parse and evaluate an expression.
		theParser = new mu::Parser * [arraySizeForParallelizedObject];

		for (int thID = 0; thID < arraySizeForParallelizedObject; thID++){
			theParser[thID] = new mu::Parser;
		}

		std::string stdStrFunction = theExpression.toStdString();
		// Submitting the function expression to "theParser"
		for (int thID = 0 ; thID < arraySizeForParallelizedObject ; thID++)
			theParser[thID]->SetExpr(stdStrFunction);

		// Creating a list of ALL parameters in the domain
		QList<RParameter *> parameterList = theDomain->findChildren<RParameter *>();
		const int no = parameterList.count();

		// Submitting the names and pointers to current value of ALL parameters in the domain by iterating on them, in each parser
		for (int i = 0; i < no; i++) {
			QString paramterStr = parameterList[i]->objectName();
			std::string paramterStdStr = paramterStr.toStdString();

			for (int thID = 0; thID < arraySizeForParallelizedObject; thID++){
				theParser[thID]->DefineVar(paramterStdStr, parameterList[i]->getCurrentValuePointer(thID));
			}
		}

		// Getting a list of the name of parameters that are explicitly used in the function from "theParser". The list is in an array format, which is specific to MuParser package. The list is called "variables".
		mu::varmap_type variables = theParser[0]->GetUsedVar();

		// In the following lines, the names of parameters in the list "variables" is iterated over.
		// These parameters are the parameters (rvs, dvs, constants, and responses) that are explicitly used in this function.
		// The name of each parameter is passed to the domain in order to receive a pointer to the parameter (using findChild() method of the domain).
		// By the end of the loop, theExplicitParameterList, which is the list of explicitly used parameters in the function, is populated.
		QList<RParameter *> theExplicitParameterList;
		mu::varmap_type::const_iterator item = variables.begin();
		QString paramterStr;
		int tempIndex = 0;
		bool xFound = false;
		for (; item != variables.end(); ++item) {
			// Reading the name of the parameter from "variables" list
			std::string paramterStdStr = item->first;
			// Converting the name from std::string to QString
			paramterStr = QString().fromStdString(paramterStdStr);

			// IF PARAMETER NAME IS "x", SKIP
			if (paramterStr == QString("x")) {
				xFound = true;
				continue;
			}

			// Finding the pointer to the parameter with this name
			RParameter *parameterObject = theDomain->findChild<RParameter *>(paramterStr);			
			
			// If the parameter with this name does not exist in the domain, issue an error.
			if (!parameterObject) {				
				rCritical() << "Error in model" << objectName() << ": Parameter" << paramterStr << "was not found in the domain.";
				// Delete "theParser" and clear the lists using "reset()" mtehod.
				reset();
				return -1;
			}

			if (!theExplicitParameterList.contains(parameterObject)) {
				theExplicitParameterList << parameterObject;
			}
			tempIndex++;
		}

		if (!xFound) {
			rCritical() << "Error in model" << this->objectName() << ": The unknown in the expression of the equation should be named \"x\". No instances of \"x\" was found in the expression.";
			return -1;
		}

		theAllParametersList << theExplicitParameterList;

		return 0;
	
	// Cathcing the exception thrown by "theParser", if there has been any (MuParser throws an exception for error handling). also, the error message is gotten from "theParser" and printed in the output pane.
	} catch (mu::Parser::exception_type &error) {
		std::string erroStdStr = error.GetMsg();
		rCritical() << "Error in model " << objectName() << ":" << qPrintable(QString().fromStdString(erroStdStr));
		return -1;
	}
}

void RRootFindingModel::reset()
{
	
	// free the allocated memory for all parsers in the array of parsers
	if (theParser) {

		int arraySizeForParallelizedObject = getArraySizeForParallelizedObject();

		for (int thID = 0; thID < arraySizeForParallelizedObject; thID++){
			if (theParser[thID])
				delete theParser[thID];
		}

		delete [] theParser;
		theParser = 0;
	}

	theAllParametersList.clear();
}

QList<RParameter *> RRootFindingModel::getAllParametersList(bool *ok)
{
	// start of safe area
	{
		QMutexLocker locker(parseLock);

		if (!theParser){
			int result = parseExpression();
			*ok = true;
			if (result < 0) {
				rCritical() << "Error: Could not parse the expression in model" << objectName() << ".";
				reset();
				*ok = false;
			}
		}
	}
	// end of safe area

	return theAllParametersList;
}

mu::Parser * RRootFindingModel::getParser(int threadID)
{
	return theParser[threadID];
}

double function(double x, void *params)
{

	int threadID = (* ((int *) params));

	//using namespace RRootFindingModel;
	try {
		// if "theParser" is NULL, it mean that the function has not been parsed. If not, no action is needed.
		mu::Parser *theParser = thisSolverModel->getParser(threadID);
		
		// start of safe area scope
		{
			QMutexLocker locker(parseLock);

			if (!theParser) {
				// Parsing the function
				int result = thisSolverModel->parseExpression();
				if (result < 0) {
					RObject::rCritical(threadID) << "Error: Could not parse the function" << thisSolverModel->objectName() << ".";
					parseLock->unlock();
					return -1;
				}
			}
		}
		// end of safe area scope

		theParser->DefineVar("x", &x);
		double value = theParser->Eval();
		theParser->RemoveVar("x");
		return value;

	// Cathcing the exception thrown by "theParser", if there has been any (MuParser throws an exception for error handling). also, the error message is gotten from "theParser" and printed in the output pane.
	} catch (mu::Parser::exception_type &error) {
		std::string erroStdStr = error.GetMsg();
		RObject::rCritical(threadID) << "Error in model" << thisSolverModel->objectName() << ": " << qPrintable(QString().fromStdString(erroStdStr));
	}
}

double functionDiff(double x, void *params)
{

	int threadID = (* ((int *) params));

	try {
		// if "theParser" is NULL, it mean that the function has not been parsed. If not, no action is needed.
		mu::Parser *theParser = thisSolverModel->getParser(threadID);

		parseLock->lock();
		if (!theParser) {
			// Parsing the function
			int result = thisSolverModel->parseExpression();
			if (result < 0) {
				RObject::rCritical(threadID) << "Error: Could not parse the function" << thisSolverModel->objectName() << ".";
				parseLock->unlock();
				return -1;
			}
		}
		parseLock->unlock();

		theParser->DefineVar("x", &x);
		mu::varmap_type valiableList = theParser->GetUsedVar();
		double diff = theParser->Diff(valiableList["x"], x);
		theParser->RemoveVar("x");
		return diff;

	// Cathcing the exception thrown by "theParser", if there has been any (MuParser throws an exception for error handling). also, the error message is gotten from "theParser" and printed in the output pane.
	} catch (mu::Parser::exception_type &error) {
		std::string erroStdStr = error.GetMsg();
		RObject::rCritical(threadID) << "Error in model" << thisSolverModel->objectName() << ": " << qPrintable(QString().fromStdString(erroStdStr));
	}
}

void functionFdF(double x, void *params, double *y, double *dy)
{
	*y = function(x, params);
	*dy = functionDiff(x, params);
}

bool RRootFindingModel::canBeRunParallelly(){
	return true;
}