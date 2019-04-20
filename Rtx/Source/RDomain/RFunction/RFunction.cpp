#include "RFunction.h"

#include <QCoreApplication>
#include <QEventLoop>

#include "RParameter.h"
#include "RResponse.h"
#include "RRandomVariable.h"
#include "RDecisionVariable.h"
#include "RConstant.h"
#include "RDomain.h"
#include "RModel.h"

#include "RParallelProcessor.h"
#include "RParallelComputationManager.h"

#include <qreadwritelock.h>
#include <QMutexLocker>

RFunction::RFunction(QObject *parent, QString name)
	: RObject(parent, name),  RParallelComputerInterface()
{

	theDomain = qobject_cast<RDomain *>(parent);

	/* initiating  ($MTh$) */
	
	populateListLock = new QMutex();
	finiteDifferenceParameterIndexLock = new QMutex();

	finiteDifferenceParallelComputer = new RParallelProcessor(this);

	int arraySizeForParallelizedObject = getArraySizeForParallelizedObject();

	theParser = 0;
	theValue = new double[arraySizeForParallelizedObject];

	for (int thID = 0 ; thID < arraySizeForParallelizedObject; thID++){
		theValue[thID] = 0;
	}

	theGradient = 0;
	theGradientAnalysisType = RFunction::FiniteDifference;

	setProperty("PerturbationFactor", 1000);
	setProperty("EfficientPerturbation", true);

	theDisplayGradient = false;
}

RFunction::~RFunction()
{
	reset();
	delete [] theValue;

	delete populateListLock;
	delete finiteDifferenceParameterIndexLock;
	delete finiteDifferenceParallelComputer;
}

QString RFunction::getExpression() const
{
	return theExpression;
}

void RFunction::setExpression(QString value)
{
	theExpression = value;

	QRegExp regExp = QRegExp(QString("[\t\n\v\r%1%2]").arg(QChar(QChar::ParagraphSeparator)).arg(QChar(QChar::LineSeparator)));
	theExpression.remove(regExp);

	// When changing the expression, the parser is deleted so that the function be parsed again
	reset();
}

RFunction::RGradientAnalysisType RFunction::getGradientAnalysisType() const
{
	return theGradientAnalysisType;
}

void RFunction::setGradientAnalysisType(RFunction::RGradientAnalysisType value)
{
	theGradientAnalysisType = value;
	if (theGradientAnalysisType == RFunction::FiniteDifference) {
		QString perturbationFactorStr = property("PerturbationFactor").toString();
		bool ok;
		double initialPerturbationFactor = perturbationFactorStr.toDouble(&ok);
		if (!ok) {
			initialPerturbationFactor = 1000;
		}
		setProperty("PerturbationFactor", initialPerturbationFactor);

		QVariant efficientPerturbationVariant = property("EfficientPerturbation");
		bool initialEfficientPerturbation = true;
		if (efficientPerturbationVariant.isValid()) {
			initialEfficientPerturbation = efficientPerturbationVariant.toBool();
		}
		setProperty("EfficientPerturbation", initialEfficientPerturbation);
	} else {
		setProperty("PerturbationFactor", QVariant());
		setProperty("EfficientPerturbation", QVariant());
	}
}

//bool RFunction::getDisplayGradient() const
//{
//	return theDisplayGradient;
//}
//
//void RFunction::setDisplayGradient(bool value)
//{
//	theDisplayGradient = value;
//}

int RFunction::populateLists()
{
	try {
		if (theParser) {
			reset();
		}

		// Instatiating "theParser" object. We use MuParser package in order to parse and evaluate an expression.
		
		int arraySizeForParallelizedObject = getArraySizeForParallelizedObject();

		theParser = new mu::Parser *[arraySizeForParallelizedObject];

		for (int thID = 0 ; thID < arraySizeForParallelizedObject ; thID++)
			theParser[thID] = new mu::Parser;

		std::string stdStrFunction = theExpression.toStdString();
		
		// Submitting the function expression to "theParser"
		for (int i = 0 ; i < arraySizeForParallelizedObject; i++)
			theParser[i]->SetExpr(stdStrFunction);

		//// Creating a list of ALL parameters in the domain
		//QList<RParameter *> parameterList = theDomain->findChildren<RParameter *>();
		//const int no = parameterList.count();

		//// Submitting the names and pointers to current value of ALL parameters in the domain by iterating on them
		//for (int i = 0; i < no; i++) {
		//	QString paramterStr = parameterList[i]->objectName();
		//	std::string paramterStdStr = paramterStr.toStdString();
		//	theParser->DefineVar(paramterStdStr, parameterList[i]->getCurrentValuePointer());
		//}

		// Getting a list of the name of parameters that are explicitly used in the function from "theParser". The list is in an array format, which is specific to MuParser package. The list is called "variables".
		mu::varmap_type variables = theParser[0]->GetUsedVar();

		// In the following lines, the names of parameters in the list "variables" is iterated over.
		// These parameters are the parameters (rvs, dvs, constants, and responses) that are explicitly used in this function.
		// The name of each parameter is passed to the domain in order to receive a pointer to the parameter (using findChild() method of the domain).
		// By the end of the loop, theExplicitParameterList, which is the list of explicitly used parameters in the function, is populated.
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
				rCritical() << "Error in function" << objectName() << ": Parameter" << paramterStr << "was not found in the domain.";
				// Delete "theParser" and clear the lists using "reset()" mtehod.
				reset();
				return -1;
			}

			// Submitting the name and a pointer to current value of the parameterObject
			for (int thID = 0; thID < arraySizeForParallelizedObject; thID++)
				theParser[thID]->DefineVar(paramterStdStr, parameterObject->getCurrentValuePointer(thID));

			if (!theExplicitParameterList.contains(parameterObject)) {
				theExplicitParameterList << parameterObject;
			}

			if (tempIndex % 10 == 0) {
				QCoreApplication::processEvents();
			}
			tempIndex++;
		}

		// This parameterSearchList will be looped over to include the the parameters that are implicitly used in the function in the lists of function.
		// This includes both parameter QProperties of random variables and decision variables and the parameters upstream.
		// The type of the parameter is determined by down-casting a pointer of the types RRandomVariable, RDecisionVariable, RConstant and RResponse to the parameter, respectively.
		// If the down-casting to -for example- RRandomVariable does not return a NULL pointer, then the type of this parameter is a random variable. Thus, the pointer is added to the list of random variables.
		// Likewise, if the type of the parameter is determined to be RDecisionVarible, RConstant, or RResponse, again the pointer is added to the corresponding lists, which are theDecisionVariableList, theConstantList, and theResponseList.
		QList<RParameter *> parameterSearchList = theExplicitParameterList;

		for (int i = 0; i < parameterSearchList.count(); i++) {
			if (i % 100 == 0) {
				QCoreApplication::processEvents();
			}

			RParameter *parameterObject = parameterSearchList[i];

			// adding a lock for the parameter if it has not been added before
			if (! theDependencyCalculationLockHash.contains(parameterObject))
				theDependencyCalculationLockHash.insert(parameterObject, new QReadWriteLock());

			// Down-casting a pointer with the type RRandomVariable to the parameter
			RRandomVariable *randomVariableObject = qobject_cast<RRandomVariable *>(parameterObject);
			// If the pointer is not NULL, then the type of the parameter is random variable. Thus, this parameter should be added to the list of random variables.
			if (randomVariableObject) {
				// If "theRandomVariableList" does not already contain this parameter ... 
				if (!theRandomVariableList.contains(randomVariableObject)) {
					// Adding the parameter to the "theRandomVariableList"
					theRandomVariableList.append(randomVariableObject);
					
					// Adding the parameter QProperties of the random variable to the parameterSearchList
					QList<RParameter *> tempParameterList = randomVariableObject->getParameterList();
					for (int j = 0; j < tempParameterList.count(); j++) {
						if (!parameterSearchList.contains(tempParameterList[j])) {
							parameterSearchList << tempParameterList[j];
						}
					}
				}
				continue;
			}

			// Down-casting a pointer with the type RDecisionVariable to the parameter
			RDecisionVariable *decisionVariableObject = qobject_cast<RDecisionVariable *>(parameterObject);
			// If the pointer is not NULL, then the type of the parameter is decision variable. Thus, this parameter should be added to the list of decision variables.
			if (decisionVariableObject) {
				// If "theDecisionVariableList" does not already contain this parameter ... 
				if (!theDecisionVariableList.contains(decisionVariableObject)) {
					// Adding the parameter to the "theDecisionVariableList"
					theDecisionVariableList.append(decisionVariableObject);
					
					// Adding the parameter QProperties of the decision variable to the parameterSearchList
					QList<RParameter *> tempParameterList = decisionVariableObject->getParameterList();
					for (int j = 0; j < tempParameterList.count(); j++) {
						if (!parameterSearchList.contains(tempParameterList[j])) {
							parameterSearchList << tempParameterList[j];
						}
					}
				}
				continue;
			}

			// Down-casting a pointer with the type RConstant to the parameter
			RConstant *constantObject = qobject_cast<RConstant *>(parameterObject);
			// If the pointer is not NULL, then the type of the parameter is constant. Thus, this parameter should be added to the list of constants.
			if (constantObject) {
				// If "theConstantList" does not already contain this parameter ... 
				if (!theConstantList.contains(constantObject)) {
					// Adding the parameter to the "theConstantList"
					theConstantList.append(constantObject);
				}
				continue;
			}

			// Down-casting a pointer with the type RResponse to the parameter
			RResponse *responseObject = qobject_cast<RResponse *>(parameterObject);
			// If the pointer is not NULL, then the type of the parameter is response. Thus, this parameter should be added to the list of responses.
			if (responseObject) {
				// If "theResponseList" does not already contain this parameter ...
				if (!theResponseList.contains(responseObject)) {
					// Adding the parameter to the "theResponseList"
					theResponseList.append(responseObject);
					
					responseObject->clearGradientMap();

					// Getting a pointer to the RModel, for which this response is an output.
					RModel *modelObject = qobject_cast<RModel *>(responseObject->getModel());

					// If "modelObject" is a NULL pointer, issue an error.
					if (!modelObject) {				
						rCritical() << "Error: Response" << responseObject->objectName() << "is not associated with any model.";
						return -1;
					}

					// Checking if we have already dealt with this model (modelObject).
					// That is, if the model is already in the "theModelList" of this function, no further action is needed and the loop continues to the next parameter
					if (!theModelList.contains(modelObject)) {
						// Adding the model to the theModelList
						theModelList.append(modelObject);
						
						int status = modelObject->checkProperties();
						if (status < 0) {
							rCritical() << "Error: The model" << modelObject->objectName() << "is missing one or more object properties.";
							return -1;
						}
						
						modelObject->reset();

						modelObject->clearDDMMaps();

						// Adding all input parameters of this RModel, "modelObject", the the parameterSearchList
						bool ok = true;
						QList<RParameter *> tempParameterList = modelObject->getAllParametersList(&ok);
						if (!ok) {
							rCritical() << "Error: Could not get the parameter list from the model" << modelObject->objectName() << ".";
							return -1;
						}
						for (int j = 0; j < tempParameterList.count(); j++) {
							if (!parameterSearchList.contains(tempParameterList[j])) {
								parameterSearchList << tempParameterList[j];
							}
						}
					}
				}
				continue;
			}
		}

		// Sorting "theModelList", "theRandomVariableList", "theDecisionVariableList", "theConstantList", and "theResponseList" according to alphabetical order of their object names.
		theDomain->sort(&theModelList);
		theDomain->sort(&theRandomVariableList);
		theDomain->sort(&theDecisionVariableList);
		theDomain->sort(&theConstantList);
		theDomain->sort(&theResponseList);

		// Setting the backup model list
		backupModelList = theModelList;

		

		// Setting the isAnalyzed flag of all responses to false
		for (int thID = 0 ; thID< arraySizeForParallelizedObject ; thID++)
			updateAnalysisStatus(thID);

		return 0;
	
	// Cathcing the exception thrown by "theParser", if there has been any (MuParser throws an exception for error handling). also, the error message is gotten from "theParser" and printed in the output pane.
	} catch (mu::Parser::exception_type &error) {
		std::string erroStdStr = error.GetMsg();
		rCritical() << "Error in function " << objectName() << ":" << qPrintable(QString().fromStdString(erroStdStr));
		return -2;
	}
}

int RFunction::evaluateFunction(bool evaluateGradientWithFunction, RObject::RParameterType parameterType, int threadID)
{

	try {
		// if "theParser" is NULL, it mean that the function has not been parsed. If not, no action is needed.

		// start of safe area scope
		{
			QMutexLocker locker(populateListLock);

			if (!theParser) {
				// Parsing the function
				int result = populateLists();
				if (result < 0) {
					rCritical(threadID) << "Error: Could not parse the function" << objectName() << ".";
					populateListLock->unlock();
					return -1;
				}
			}
		}
		// end of safe area scope

		int result = 0;

		// Iterating on the models to evaluate the information for each model needed by upstream models
		for (int i = 0; i < theModelList.count(); i++) {
			result = theModelList[i]->setResponsesThatFlowUpstream(threadID);
			if (result < 0) {
				rCritical(threadID) << "Error: Could not evalute Information in the model " << theModelList[i]->objectName() << ".";
				return -1;
			}
		}
		
		// A line to keep the GUI responsive
		if (threadID == 0)
			QCoreApplication::processEvents();

		// Iterating on the responses that are EXPLICITLY used in the function expression in order to start the propagation of "runanalysis()" in models.
		for (int i = 0; i < theExplicitParameterList.count(); i++) {
			// Getting the pointer to the response. The pointer is named "theResponseObject".
			RResponse *theResponseObject = qobject_cast<RResponse *>(theExplicitParameterList[i]);

			// If "theResponseObject" is a NULL pointer, issue an error.
			if (!theResponseObject) {
				continue;
			}

			// If "theResponseObject" is not analyzed (that is, if its "isAnalyzed" flag is false), then an analysis is needed to update its current value. If not, no action is needed and the loop continues to the next response.
			if (!(theResponseObject->isAnalyzed(threadID))) {
				
				// Getting a pointer to the RModel, for which this response is an output. The pointer is named "theModel".
				RModel *theModel = qobject_cast<RModel *>(theResponseObject->getModel());
				// If "modelObject" is a NULL pointer, issue an error.
				if (!theModel) {				
					rCritical(threadID) << "Error: The response" << theResponseObject << "is not associated with any model.";
					return -1;
				}
				
				// Run analysis to update the value of the response
				if (theGradientAnalysisType == RFunction::DDM) { 
					result = theModel->runAnalysis(evaluateGradientWithFunction, parameterType, threadID);
				} else {
					result = theModel->runAnalysis(false, parameterType, threadID);
				}

				if (result < 0) {
					rCritical(threadID) << "Error: Could not run the analysis in the model " << theModel->objectName() << ".";
					return result;
				}
			}
		}
		// result = 0 mean function is evaluated without error;
		// result = -1 means function is not evaluated due to errors other than non-convergence;
		// result < -1 means function is not evaluated due to non-convergence.

		// Now that all responses are updated, the expression is evaluated by "theParser".
		// It should be noted that since "theParser" has the pointer to "currentValue" of all parameters, there is no need to submit their new values.
		theValue[threadID] = theParser[threadID]->Eval();	

		////////////////
		//Is it needed to re-parse the fucntion at each step?
		//If so, the follwoing lines are needed.
		//reset();
		////////////////

		return 0;

	// Cathcing the exception thrown by "theParser", if there has been any (MuParser throws an exception for error handling). also, the error message is gotten from "theParser" and printed in the output pane.
	} catch (mu::Parser::exception_type &error) {
		std::string erroStdStr = error.GetMsg();
		rCritical(threadID) << "Error in function" << objectName() << ": " << qPrintable(QString().fromStdString(erroStdStr));
		return -1;
	}
}

double RFunction::getFunctionValue() const
{
	return theValue[0];
}

double RFunction::getFunctionValue(int threadID){

	return theValue[threadID];
}

void RFunction::reset()
{
	if (theParser) {

		int arraySizeForParallelizedObject = getArraySizeForParallelizedObject();

		for (int thID = 0 ; thID < arraySizeForParallelizedObject ; thID++)
			if (theParser[thID])
				delete theParser[thID];

		delete [] theParser;
		theParser = 0;
	}

	if (theGradient) {
		gsl_vector_free(theGradient);
		theGradient = 0;
	}

	theModelList.clear();
	backupModelList.clear();
	theRandomVariableList.clear();
	theDecisionVariableList.clear();
	theConstantList.clear();
	theResponseList.clear();
	theExplicitParameterList.clear();

	theDependencyHash.clear();

	// DDM
	clearDDMMaps();

	// all locks of theDependencyLockMap
	qDeleteAll(theDependencyCalculationLockHash);
	theDependencyCalculationLockHash.clear();
}

void RFunction::clearDDMMaps()
{
	theExplicitResponseDependenceMap.clear();
	for (int i = 0; i < backupModelList.count(); i++) {
		backupModelList[i]->clearDDMMaps();
	}

}

int RFunction::evaluateGradient(RObject::RParameterType parameterType)
{
	if (!theParser) {
		rCritical() << "Error in function" << objectName() << ": " << "The function has not been parsed due to errors.";
		return -1;
	}

	int result = 0;

	// If the gradient type is finite difefrence
	double perturbationFactor = 0.0;
	if (theGradientAnalysisType == RFunction::FiniteDifference) {
		// Getting the pertubation factor from its QProperty.
		// It should be noted that "perturbationFactor" is a dynamic QProperty and thus, it does not have any setter and getter and data member associated with.
		perturbationFactor = property("PerturbationFactor").toDouble();	

		// If "perturbationFactor" is not a positive number, issue an error.
		if (perturbationFactor <= 0) {
			rCritical() << "Error in function" << objectName() << ": Perturbation factor must be greater than Zero.";
			return -1;
		}
	}

	// Backing up the function value to return to them after gradient analysis
	double theValueBackup = theValue[0];

	// Backing up the response values to return to them after gradient analysis in case of finite difference
	if (theGradientAnalysisType == RFunction::FiniteDifference) {
		for (int i = 0; i < theResponseList.count(); i++) {
			theResponseList[i]->backup(0);
		}
	}

	QList<RParameter *> parameterList;
	QString parameterName;
	if (parameterType == RObject::RandomVariable) {
		parameterName = "random variables";
		for (int i = 0; i < theRandomVariableList.count(); i++) {
			parameterList << theRandomVariableList[i];
		}
	} else if (parameterType == RObject::DecisionVariable) {
		parameterName = "decision variables";
		for (int i = 0; i < theDecisionVariableList.count(); i++) {
			parameterList << theDecisionVariableList[i];
		}
	} else if (parameterType == RObject::Constant) {
		parameterName = "constants";
		for (int i = 0; i < theConstantList.count(); i++) {
			parameterList << theConstantList[i];
		}
	} else if (parameterType == RObject::Response) {
		parameterName = "responses";
		for (int i = 0; i < theResponseList.count(); i++) {
			parameterList << theResponseList[i];
		}
	}

	if (parameterList.isEmpty()) {
		rCritical() << "Error: The function" << objectName() << "is not a function of any" << qPrintable(parameterName) << ".";
		return -1;
	}

	int count = parameterList.count();

	if (!theGradient) {
		theGradient = gsl_vector_calloc(count);
	} else {
		if (count != theGradient->size) {
			gsl_vector_free(theGradient);
			theGradient = gsl_vector_calloc(count);
		}
	}
				
	// Setting all elements of "theGradient" vector to Zero
	gsl_vector_set_zero(theGradient);

	if (theGradientAnalysisType == RFunction::FiniteDifference) {	

		int generalParameterIndex = 0;
		void * finiteDifferenceInputArgs[] = {(void *)(&parameterList), (void *)(&generalParameterIndex), (void *)(&parameterType), (void *)(& perturbationFactor), (void *)(&theValueBackup)};
		int calculationStatus = finiteDifferenceParallelComputer->run(&RFunction::finiteDifferenceWrapper, this, finiteDifferenceInputArgs, parameterList.size());

		// if error has occured , return the error code
		if (calculationStatus < 0){
			return calculationStatus;
		}
	} else if (theGradientAnalysisType == RFunction::DDM) {
	
		//ATTENTION: THIS IMPLEMENTATION ASSUMED THAT WHEN THE FUNCTION IS SET ON DDM, BEFORE CALLING THE RFunction:evaluateGradient() METHOD, THE RFunction:evaluateFunction() IS CALLED, SO THAT THE DDMS IN MODELS ARE EVALUATED.

		QList<RParameter *> explicitParameterList;
		QList<RResponse *> explicitResponseList;

		// Iterate on theExplicitParameterList to populate explicitParameterList and explicitResponseList
		for (int i = 0; i < theExplicitParameterList.count(); i++) {
			QList<RParameter *> parameterList;
			if (parameterType == RObject::RandomVariable) {
				RRandomVariable *parameter = qobject_cast<RRandomVariable *>(theExplicitParameterList[i]);
				if (parameter) {
					explicitParameterList << theExplicitParameterList[i];
				}
			} else if (parameterType == RObject::DecisionVariable) {
				RDecisionVariable *parameter = qobject_cast<RDecisionVariable *>(theExplicitParameterList[i]);
				if (parameter) {
					explicitParameterList << theExplicitParameterList[i];
				}
			} else if (parameterType == RObject::Constant) {
				RConstant *parameter = qobject_cast<RConstant *>(theExplicitParameterList[i]);
				if (parameter) {
					explicitParameterList << theExplicitParameterList[i];
				}
			}
			RResponse *response = qobject_cast<RResponse *>(theExplicitParameterList[i]);
			if (response) {
				explicitResponseList << response;
			}
		}
			
		// Calculating the finite difference of function with respect to explicitParameterList
		QMap<RParameter *, double> explicitParameterGradientMap;
		for (int i = 0; i < explicitParameterList.count(); i++) {
			double finiteDifferenceValue;
			int status;
			finiteDifference(explicitParameterList[i], parameterType, 10000.0, theValueBackup, &finiteDifferenceValue, &status, 0);

			if (status < 0) {
				return -1;
			}

			explicitParameterGradientMap[explicitParameterList[i]] = finiteDifferenceValue;
		}

		// Calculating the finite difference of function with respect to explicitResponseList
		QMap<RResponse *, double> explicitResponseGradientMap;
		for (int i = 0; i < explicitResponseList.count(); i++) {
			double finiteDifferenceValue;
			int status;
			finiteDifference(explicitResponseList[i], RObject::Response, 10000.0, theValueBackup, &finiteDifferenceValue, &status, 0);

			if (status < 0) {
				return -1;
			}

			explicitResponseGradientMap[explicitResponseList[i]] = finiteDifferenceValue;
		}


		// Establish theExplicitResponseDependenceMap once and for all; that is, this will be run only at first evaluation of the gradient by DDM
		if (theExplicitResponseDependenceMap.isEmpty()) {
			for (int i = 0; i < explicitResponseList.count(); i++) {
				QMap<RParameter *, double> theGradientMap = explicitResponseList[i]->getGradientMap();
				QList<RParameter *> dependenceToThisResponseList = theGradientMap.uniqueKeys();////????
				
				// Find the index of this explicit response in theExplicitParameterList
				int index = theExplicitParameterList.indexOf(explicitResponseList[i]);

				// Iterate on the RParameters that affect this explicit response, namely dependenceToThisResponseList, and add the index of this response to the map of that parameter
				for (int j = 0; j < dependenceToThisResponseList.count(); j++) {
					theExplicitResponseDependenceMap.insertMulti(dependenceToThisResponseList[j], index);
				}
			}
			// Add the -1 to the map of direct input parameters to indicate they are explicitly used in the function
			for (int i = 0; i < explicitParameterList.count(); i++) {
				theExplicitResponseDependenceMap.insertMulti(explicitParameterList[i], -1);
			}
		}

		// Calculating theGradient
		for (int i = 0; i < count; i++) {
			double gradElement = 0.0;
			RParameter *parameter = parameterList[i];

			// Exporting the index list of input responses that depend on this parameter (RV/DV/C)
			QList<int> indexList = theExplicitResponseDependenceMap.values(parameter);

			for (int j = 0; j < indexList.count(); j++) {
				int index = indexList[j];
				if (index == -1) {
					// Means that the function explicitly depends on the parameter
					gradElement += explicitParameterGradientMap.value(parameter);
				} else {
					// Get the pointer to the response with the index "index" in theExplicitParameterList
					RResponse *response = qobject_cast<RResponse *>(theExplicitParameterList[index]);
					if (!response) {
						rCritical() << "Error: Could not evaluate the gradient of the function" << objectName() << ". The parameter" << theExplicitParameterList[index]->objectName() << "in not a response.";
						return -1;
					}

					// Export the gradient element of the response with respect to the parameter
					QMap<RParameter *, double> responseGradientMap = response->getGradientMap();
					double responseGradientdouble = responseGradientMap.value(parameter);
					
					// Export the finite difference of the function with respect to the response
					double fd = explicitResponseGradientMap.value(response);

					// Add up the product of these 2 doubles :  d(function) / d(parameter) = [d(function) / d(response)] * [d(response) / d(parameter)]
					gradElement += fd * responseGradientdouble;
				}
			}
			// Assign this element of theGradient
			gsl_vector_set(theGradient, i, gradElement);
		}
	}

	// Restoring the  value of the function
	theValue[0] = theValueBackup;
		
	// Printing the gradient vector to the output pane is user has set "theDisplayGradient" flag to true.
	if (theDisplayGradient == true) {
		rDebug() << "Gradient in Original Space:";
		theDomain->print(theGradient, 0);
		rDebug() << "";
	}
	
	return 0;

	//Test for differentiation using Mu::Parser library
	//double aa = theParser->Diff(variables["c3"],3);
	//double aa = theParser->Diff(currentItem->second,3);
}

void RFunction::finiteDifference(RParameter *parameter, RObject::RParameterType parameterType, double perturbationFactor, double originalFunctionValue, double *derivative, int *status, int threadID)
{
	// Taking a backup from the current value of the parameter
	double currentValueBackup = parameter->getCurrentValue(threadID);
	
	double h;
	if (parameterType == RObject::RandomVariable) {
		// Getting the standard deviation of the random variable
		RRandomVariable *randomVariable = qobject_cast<RRandomVariable *>(parameter);
		double standardDeviation = randomVariable->getStandardDeviationValue(threadID);
		if (standardDeviation == 0) {
			rCritical(threadID) << "WARNING: The standard deviation of the random avriable" << randomVariable->objectName() << " is 0. Rt is perturbing the random variable by (1.0/perturbation factor) in the finite difference analysis.";
			standardDeviation = 1.0;
		}

		// Perturbing the current value of the random variable 
		h = standardDeviation / perturbationFactor;				
	} else {
		if (currentValueBackup == 0.0) {
			h = 0.001;
			//rCritical() << "WARNING: Parameter is zero, selecting" << h << "as the perturbation in finite difference analysis.";
		} else {
			h = currentValueBackup / perturbationFactor;
		}
	}
	
	// Perturb the value of the parameter
	double purterbedValue = currentValueBackup + h;
	parameter->setCurrentValue(purterbedValue, threadID);

	int result = evaluateFunction(false, parameterType, threadID);
	if (result < 0) {
		rCritical(threadID) << "Error: Could not evaluate the function" << objectName() << " while calculating its partial derivative with respect to the parameter" << parameter->objectName() << ".";
	}

	// Calculating the partial derivative value
	double gradElement = (theValue[threadID] - originalFunctionValue) / h;

	// Restoring the current value of the random variable
	parameter->setCurrentValue(currentValueBackup, threadID);	

	*derivative = gradElement;
	*status = result;
}

gsl_vector * RFunction::getGradient() const
{
	return theGradient;
}

QList<RRandomVariable *> * RFunction::getRandomVariableList()
{	
	theDomain->sort(&theRandomVariableList);
	return &theRandomVariableList;
}

QList<RConstant *> RFunction::getConstantList()
{	
	theDomain->sort(&theConstantList);
	return theConstantList;
}

QList<RResponse *> RFunction::getResponseList()
{	
	theDomain->sort(&theResponseList);
	return theResponseList;
}

QList<RDecisionVariable *> RFunction::getDecisionVariableList()
{	
	theDomain->sort(&theDecisionVariableList);
	return theDecisionVariableList;
}

QList<RModel *> RFunction::getModelList()
{	
	return theModelList;
}

void RFunction::setResponseList(QList<RResponse *> newResponseList)
{
	theResponseList = newResponseList;
	
}

void RFunction::setModelList(QList<RModel *> newModelList)
{
	theModelList = newModelList;
}

void RFunction::setExplicitParameterList(QList<RParameter *> newParameterList)
{
	theExplicitParameterList = newParameterList;
}

QList<RParameter *> RFunction::getExplicitParameterList()
{	
	return theExplicitParameterList;
}

void RFunction::updateAnalysisStatus(RParameter *passedParameter, int threadID)
{
	QList<RModel *> dependentModelList;

	QReadWriteLock * lock = theDependencyCalculationLockHash[passedParameter];

	lock->lockForRead();

	if (!theDependencyHash.contains(passedParameter)) {
		lock->unlock();
		evaluateDependencies(passedParameter);
	}
	else{
		lock->unlock();
	}

	dependentModelList = theDependencyHash[passedParameter];

	for (int j = 0; j < dependentModelList.count(); j++) {
		if (theModelList.contains(dependentModelList[j])) {
			dependentModelList[j]->setResponseAnalyzed(false, threadID);
		}
	}
}

void RFunction::evaluateDependencies(RParameter *passedParameter)
{

	QReadWriteLock * lock = theDependencyCalculationLockHash[passedParameter];
	lock->lockForWrite();

	// making sure that the dependencies of the parameter has net been evaluated before in other threads
	if (theDependencyHash.contains(passedParameter)){
		lock->unlock();
		return;
	}

	QList<RModel *> dependentModelList;
	
	// Estabishing a hash of RModels called "remainingModelList" and anserting all the models of theBackupModelList to it. The reason why backup lists are used here is explained where the backup lists are declated in the .h file.
	// The backup list is used, for example, to create QMaps for efficient finite difefrence analysis. The reason that the original lists are not used is that those lists may be modified in some of the analysis types, e.g., load coincidence. So, the backup lists should be used.
	QHash<RModel*, bool> remainingModelHash;
	int remainingModelsCount = backupModelList.size();
	for (int i = 0; i < backupModelList.count(); ++i){
		remainingModelHash.insert(backupModelList[i], true);
	}

	QHash<RModel *, bool>::Iterator nextRemainingModelIterator;

	// Searching for next model among the models in the "remainingModelList" by iterating on them
	for (nextRemainingModelIterator = remainingModelHash.begin(); nextRemainingModelIterator != remainingModelHash.end(); ++nextRemainingModelIterator) {
			
		// A line to keep the GUI responsive
		if (! RParallelComputationManager::getInstance()->isRunningInParallelMode()){
			QCoreApplication::processEvents();
		}

		// if model has been proven to be dependant before, skip checking it
		if (!nextRemainingModelIterator.value()){
			continue;
		}

		// Getting the pointer to this model. The pointer is named "modelObject".
		RModel *modelObject = nextRemainingModelIterator.key();

		// If the "modelObject" CONTAINS the "passedParameter" (that is, if the "passedParameter" is an input to "modelObject"), then this model and all of its outputs (reponses) along with the models downstream are affrected by the change in current value of this parameter.
		if (modelObject->contains(passedParameter)) {

			// setting the flag of the model in remainingModelHash to false, as it is proven to be dependant
			remainingModelHash[modelObject] = false;
			remainingModelsCount--;

			//// Setting the isAnalyzed flag of all the output responses of modelObject to false
			//modelObject->setResponseAnalyzed(false);
			dependentModelList.append(modelObject);

			// If the remainingModelList is empty, then the search is done:
			if (remainingModelsCount == 0) {
				break;
			}

			// Adding the output RResponses of the "modelObject" (which are its children) to the "searchList", so that they will be considered for flag updating of the models for which they are input.
			// Getting the list of the output RResponses on the "modelObject"
			QList<RResponse *> tempResponseList = modelObject->findChildren<RResponse *>();
			// Iterrating on the list
			for (int k = 0; k < tempResponseList.count(); k++) {
				// If the dependency map of the tempResponseList[k] is not establish, then estabish its dependency map first (recursive call)
				if (!theDependencyHash.contains(tempResponseList[k])) {
					evaluateDependencies(tempResponseList[k]);
				}					
					
				// Get the list of modelsthat are affected by tempResponseList[k]
				QList<RModel *> tempModelList = theDependencyHash[tempResponseList[k]];
				for (int l = 0; l < tempModelList.count(); l++) {
						
					// setting the flag of the model in remainingModelHash to false if it is true (it hasn't been added to theDependantModelList)
					if (remainingModelHash[tempModelList[l]]){
						remainingModelHash[tempModelList[l]] = false;
						remainingModelsCount--;

						// Appned the tempModelList to the dependentModelList of the parameter under consideration
						dependentModelList.append(tempModelList[l]);

						// If the remainingModelList is empty, then the search is done:
						if (remainingModelsCount == 0){
							break;
						}
					}
				}
			}
		}
	}

	theDependencyHash[passedParameter] = dependentModelList;

	lock->unlock();
}

void RFunction::updateAnalysisStatus(int threadID)
{
	// Setting the "isAnalyzed" flag of all the responses in the theResponseList of this function to false.
	for (int i = 0; i < theResponseList.count(); i++) {
		theResponseList[i]->setAnalyzed(false, threadID);
	}
}

void RFunction::updateAnalysisStatus(QList<RRandomVariable *> passedRandomVariableList, int threadID)
{
	QList<RParameter *> parameterObjectList;
	for (int i = 0; i < passedRandomVariableList.count(); i++) {
		parameterObjectList << passedRandomVariableList[i];
	}
	updateAnalysisStatus(parameterObjectList, threadID);
}

void RFunction::updateAnalysisStatus(QList<RParameter *> passedParameterList, int threadID)
{
	for (int i = 0; i < passedParameterList.count(); i++) {
		updateAnalysisStatus(passedParameterList[i], threadID);
	}
}

int RFunction::finiteDifferenceWrapper(void * theObject, void * inputArgs[], int threadID){
	
	// casting the object
	RFunction * thisFunction = (RFunction *)(theObject);
	
	// casting parameters
	QList<RParameter *> * parameterList = (QList<RParameter *> *) inputArgs[0];
	int * generalParameterIndex = (int *) inputArgs[1];
	RObject::RParameterType parameterType = *((RObject::RParameterType *) inputArgs[2]);
	double perturbationFactor = *((double *) inputArgs[3]);
	double originalFunctionValue = *((double *) inputArgs[4]);

	// calling the method
	return thisFunction->calculateFunctionGradientWithFD(parameterList, generalParameterIndex, parameterType, perturbationFactor, originalFunctionValue, threadID);
}

int RFunction::calculateFunctionGradientWithFD(QList<RParameter *> * parameterList, int * generalParameterIndex, RObject::RParameterType parameterType, double perturbationFactor, double originalFunctionValue, int threadID){

	// backing up the value of responses
	for (int i = 0; i < theResponseList.count(); i++) {
		theResponseList[i]->backup(threadID);
	}

	int parameterIndex;

	// starting iteration on different parameter indices
	while (finiteDifferenceParallelComputer->getContinueCondition()){

		// allocating next parameter index to work on in this thread
		finiteDifferenceParameterIndexLock->lock();
		parameterIndex = (* generalParameterIndex)++;
		finiteDifferenceParameterIndexLock->unlock();

		// if parameter index is out of range , it means function gradient has been calculated completely or is being completed in other threads
		if (parameterIndex >= parameterList->size())
			break;

		// Calling the method to update the isanalyzed flags of RResponses due to perturbation of this random variable
		// After the follwoing line is run, the "isAnalyzed" flag of all RResponses, which are affected by purturbing this random variable, is set to false.

		// IMPORTANT:
		bool doEfficientPerturbation = this->property("EfficientPerturbation").toBool();
		if (doEfficientPerturbation) {
			updateAnalysisStatus((*parameterList)[parameterIndex], threadID);
		} else {
			updateAnalysisStatus(threadID);
		}

		double gradElement;
		int status;
		finiteDifference((*parameterList)[parameterIndex], parameterType, perturbationFactor, originalFunctionValue, &gradElement, &status, threadID);

		if (status < 0) {
			return -1;
		}

		// Setting the element of "theGradient" vector to the calculated partial derivative
		gsl_vector_set(theGradient, parameterIndex, gradElement);

		// Restoring the response values to the original values backed up before gradient analysis
		for (int i = 0; i < theResponseList.count(); i++) {
			theResponseList[i]->restore(threadID);
		}

	}
	// end of iterating on different parameter indices


	return 0;
}

void RFunction::initializeForParallelComputation(int threadID){

	RObject::initializeForParallelComputation(threadID);

	QLinkedList<RObject *> contributedObjects = getTheListOfAllContributingObjects();

	QLinkedList<RObject *>::iterator obj_ptr_ptr;
	for (obj_ptr_ptr = contributedObjects.begin(); obj_ptr_ptr != contributedObjects.end(); ++obj_ptr_ptr) {
		(*obj_ptr_ptr)->initializeForParallelComputation(threadID);
	}
}

void RFunction::resetAfterParallelComputation(){

	RObject::resetAfterParallelComputation();

	QLinkedList<RObject *> contributedObjects = getTheListOfAllContributingObjects();

	QLinkedList<RObject *>::iterator obj_ptr_ptr;
	for (obj_ptr_ptr = contributedObjects.begin(); obj_ptr_ptr != contributedObjects.end(); ++obj_ptr_ptr) {
		(*obj_ptr_ptr)->resetAfterParallelComputation();
	}
}

bool RFunction::canBeRunParallelly(){
	
	QLinkedList<RObject *> contributedObjects = getTheListOfAllContributingObjects();

	QLinkedList<RObject *>::iterator obj_ptr_ptr;
	for (obj_ptr_ptr = contributedObjects.begin(); obj_ptr_ptr != contributedObjects.end(); ++obj_ptr_ptr) {
		if (!(*obj_ptr_ptr)->canBeRunParallelly()) {
			rDebug() << "The " << (*obj_ptr_ptr)->objectName() << " does not include the necessary implementations.";
			return false;
		}
	}

	return true;
}

QLinkedList<RObject *> RFunction::getTheListOfAllContributingObjects(){

	QLinkedList<RObject *> contributedObjectsList;

	for (int i = 0; i < theModelList.count(); ++i) {
		contributedObjectsList.append(theModelList[i]);
	}
	
	for (int i = 0; i < theRandomVariableList.count(); ++i) {
		contributedObjectsList.append(theRandomVariableList[i]);
	}

	for (int i = 0; i < theDecisionVariableList.count(); ++i) {
		contributedObjectsList.append(theDecisionVariableList[i]);
	}

	for (int i = 0; i < theConstantList.count(); ++i) {
		contributedObjectsList.append(theConstantList[i]);
	}

	for (int i = 0; i < theResponseList.count(); ++i) {
		contributedObjectsList.append(theResponseList[i]);
	}

	return contributedObjectsList;
}