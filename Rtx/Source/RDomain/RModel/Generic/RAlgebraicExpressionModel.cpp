#include "RAlgebraicExpressionModel.h"

#include "RParameter.h"
#include "RResponse.h"
#include "RDomain.h"

#include <QMutex>
#include <QMutexLocker>

RAlgebraicExpressionModel::RAlgebraicExpressionModel(QObject *parent, QString name)
	: RModel(parent, name)
{
	theParser = 0;
	
	theDomain->createObject(objectName() + "Response", "RGenericResponse");
	theResponse = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theResponse->setModel(this);

	parseLock = new QMutex();
}

RAlgebraicExpressionModel::~RAlgebraicExpressionModel()
{

	// free the allocated memory for all parsers in the array of parsers
	reset();

	// delete the lock
	delete parseLock;
}

QString RAlgebraicExpressionModel::getExpression() const	
{	
	return theExpression;
}	

void RAlgebraicExpressionModel::setExpression(QString value)	
{	
	theExpression = value;
	reset();
}	

QList<RParameter *> RAlgebraicExpressionModel::getPhysicalParameterList()
{
	/*if (theAllParametersList.isEmpty()) {
		bool ok;
		getAllParametersList(&ok);
	}*/
	QList<RParameter *> tempList;
	for (int i = 0; i < thePhysicalParameters.count(); i++) {
		tempList << thePhysicalParameters[i].data();
	}
	return tempList;
}

QString RAlgebraicExpressionModel::getPhysicalParameters()
{
	return parameterListToString(thePhysicalParameters);
}

void RAlgebraicExpressionModel::setPhysicalParameters(QString value)
{
	thePhysicalParameters = stringToParameterList(value);
}

int RAlgebraicExpressionModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
{	
	try {

		// locking to allow only one thread to run this part
		// start of safe area scope
		{
			QMutexLocker locker(parseLock);

			// if "theParser" is NULL, it mean that the function has not been parsed. If not, no action is needed.
			if (!theParser) {
				// Parsing the function
				int result = parseExpression();
				if (result < 0) {
					rCritical(threadID) << "Error: Could not parse the function" << objectName() << ".";
					return -1;
				}
			}
		}
		// end of safe area scope

		double response = theParser[threadID]->Eval();
		theResponse->setCurrentValue(response,threadID);
		return 0;

	// Cathcing the exception thrown by "theParser", if there has been any (MuParser throws an exception for error handling). also, the error message is gotten from "theParser" and printed in the output pane.
	} catch (mu::Parser::exception_type &error) {
		std::string erroStdStr = error.GetMsg();
		rCritical(threadID) << "Error in model" << objectName() << ": " << qPrintable(QString().fromStdString(erroStdStr));
		return -1;
	}
}

int RAlgebraicExpressionModel::parseExpression()
{
	try {
		if (theParser) {
			reset();
		}

		int arraySizeForParallelizedObject = getArraySizeForParallelizedObject();

		// Instatiating "theParser" objects. We use MuParser package in order to parse and evaluate an expression.
		theParser = new mu::Parser * [arraySizeForParallelizedObject];
		for (int thID = 0; thID < arraySizeForParallelizedObject; thID++){
			theParser[thID] = new mu::Parser;
		}

		std::string stdStrFunction = theExpression.toStdString();
		
		for (int thID = 0; thID < arraySizeForParallelizedObject; thID++){
			// Submitting the function expression to "theParser"
			theParser[thID]->SetExpr(stdStrFunction);
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
		for (; item != variables.end(); ++item) {
			// Reading the name of the parameter from "variables" list
			std::string paramterStdStr = item->first;
			// Converting the name from std::string to QString
			paramterStr = QString().fromStdString(paramterStdStr);
			// Finding the pointer to the parameter with this name
			RParameter *parameterObject = theDomain->findChild<RParameter *>(paramterStr);			
			
			// If the parameter with this name does not exist in the domain, issue an error.
			if (!parameterObject) {				
				rCritical() << "Error in model" << objectName() << ": Parameter" << paramterStr << "was not found in the domain.";
				// Delete "theParser" and clear the lists using "reset()" mtehod.
				reset();
				return -1;
			}

			// Submitting the name and a pointer to current value of the parameterObject, in each parser
			for (int thID = 0; thID < arraySizeForParallelizedObject; thID++){
				theParser[thID]->DefineVar(paramterStdStr, parameterObject->getCurrentValuePointer(thID));
			}

			if (!theExplicitParameterList.contains(parameterObject)) {
				theExplicitParameterList << parameterObject;
			}
			tempIndex++;
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

void RAlgebraicExpressionModel::reset()
{
	
	// free the allocated memory for all parsers in the array of parsers
	if (theParser) {

		int arraySizeForParallelizedObject = getArraySizeForParallelizedObject();

		for (int thID = 0 ; thID < arraySizeForParallelizedObject ; thID++)
			if (theParser[thID])
				delete theParser[thID];

		delete [] theParser;

		theParser = 0;
	}

	theAllParametersList.clear();
}

QList<RParameter *> RAlgebraicExpressionModel::getAllParametersList(bool *ok)
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

bool RAlgebraicExpressionModel::canBeRunParallelly(){
	return true;
}