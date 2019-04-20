// Last modified by Mojtaba on March 19, 2014

#include "RFunctionEvaluationAnalyzer.h"

#include <QMap>
#include <QCoreApplication>
#include <QDateTime>
#include <QApplication>

#include "RMainWindow.h"
#include "RDomain.h"
#include "RFunction.h"
#include "RContinuousRandomVariable.h"
#include "RResponse.h"
#include "RModel.h"
#include "RFlowchartWidget.h"

#include <gsl/gsl_vector.h>

RFunctionEvaluationAnalyzer::RFunctionEvaluationAnalyzer(QObject *parent, QString name)
	: RAnalyzer(parent, name)
{
	//$$ theFunction = 0;
	theEvaluateGradient = false;
	theSetRandomVariablesToMean = false;
	thePrintRandomVariableList = false;
	thePlotModelFlowchart = true;
}

RFunctionEvaluationAnalyzer::~RFunctionEvaluationAnalyzer()
{

}

QObject * RFunctionEvaluationAnalyzer::getFunction() const	
{	
	return theFunction.data();
}	
	
void RFunctionEvaluationAnalyzer::setFunction(QObject *value)	
{	
	theFunction = qobject_cast<RFunction *>(value);
}	
	
bool RFunctionEvaluationAnalyzer::getEvaluateGradient() const	
{	
	return theEvaluateGradient;
}	
	
void RFunctionEvaluationAnalyzer::setEvaluateGradient(bool value)	
{	
	theEvaluateGradient = value;
}	
	
bool RFunctionEvaluationAnalyzer::getSetRandomVariablesToMean() const	
{	
	return theSetRandomVariablesToMean;
}	
	
void RFunctionEvaluationAnalyzer::setSetRandomVariablesToMean(bool value)	
{	
	theSetRandomVariablesToMean = value;
}	
	
bool RFunctionEvaluationAnalyzer::getPrintRandomVariableList() const	
{	
	return thePrintRandomVariableList;
}	
	
void RFunctionEvaluationAnalyzer::setPrintRandomVariableList(bool value)	
{	
	thePrintRandomVariableList = value;
}	
	
bool RFunctionEvaluationAnalyzer::getPlotModelFlowchart() const	
{	
	return thePlotModelFlowchart;
}	
	
void RFunctionEvaluationAnalyzer::setPlotModelFlowchart(bool value)	
{	
	thePlotModelFlowchart = value;
}	

int RFunctionEvaluationAnalyzer::analyze()
{
	// Inform the user that the analysis has started
	rDebug() << "##### STARTING TO EVALUATE FUNCTION" << theFunction.data()->objectName() << "...";
	rDebug() << "";

	QDateTime startTime = QDateTime::currentDateTime();


	if (!theFunction) {
		rCritical() << "Error in" << objectName() << ": Please choose a function and try again";
		return -1;
	}

	if (qobject_cast<QApplication*>(QCoreApplication::instance()) != 0) {
		theMainWindow->raiseDock("Output");
	}


	// Setting the current value of all the random variables to their mean if the user has asked to
	if (theSetRandomVariablesToMean) {
		QList<RContinuousRandomVariable *> randomVariableList = theDomain->findChildren<RContinuousRandomVariable *>();
		for (int i = 0; i < randomVariableList.count(); i++) {
			randomVariableList[i]->setCurrentValue(randomVariableList[i]->getMeanValue(0));
		}
	}
	
	// Parse the passed function
	int result = theFunction.data()->populateLists();
	if (result < 0) {
		rCritical() << "Error in" << objectName() << "Could not parse the function" << theFunction.data()->objectName();
		return -1;
	}

	

	// Printing the list of random variables to the output pane if the user has asked to
	if (thePrintRandomVariableList) {
		rDebug() << "The vector of random variables that affect this function:";
		QList<RRandomVariable *> randomVariableList = (*theFunction.data()->getRandomVariableList());
		for (int i = 0; i < randomVariableList.count(); i++) {
			rDebug() << qPrintable(QString("| %1|").arg(randomVariableList[i]->objectName(), -40));
		}
		rDebug() << "";
	}


	theFunction.data()->updateAnalysisStatus(0);
	
	if ((theEvaluateGradient) && (theFunction.data()->getGradientAnalysisType() == RFunction::DDM)) {
		result = theFunction.data()->evaluateFunction(true, RObject::RandomVariable,0);
	} else {
		result = theFunction.data()->evaluateFunction(false, RObject::RandomVariable,0);
	}	
	if (result < 0) {
		return -1;
	}

	gsl_vector *gradientVector = 0;
	if (theEvaluateGradient) {
		result = theFunction.data()->evaluateGradient(RObject::RandomVariable);	
		if (result < 0) {
			return -2;
		}
		gradientVector = theFunction.data()->getGradient();
	}

	double functionValue = theFunction.data()->getFunctionValue();
	
	rDebug() << "The value of the function:";
	rDebug() << qPrintable(theFunction.data()->objectName()) << "=" << functionValue;
	rDebug() << "";

	if ((theEvaluateGradient) && (gradientVector)) {
		rDebug() << "The gradient of the function:";
		theDomain->print(gradientVector, 0);
		rDebug() << "";
	}

	QDateTime finishTime = QDateTime::currentDateTime();
	double numDays = startTime.daysTo(finishTime);
	double numMilliSecs = startTime.time().msecsTo(finishTime.time());
	double numSecs = numDays * 86400.0 + numMilliSecs / 1000.0;

	rDebug() << "##### FUNCTION EVALUATION DONE IN" << numSecs << "SECONDS.";
	rDebug() << "";
	rDebug() << "";




	
	
	// Begin to draw the flowchart
	if (thePlotModelFlowchart && qobject_cast<QApplication*>(QCoreApplication::instance()) != 0) {
		QMap<RModel *, RModel *> modelMap;
		QList<QList<RModel *> > modelListList;
		QList<RParameter *> parameterList = theFunction.data()->getExplicitParameterList();

		// Find the first level after function
		QList<RModel *> level0ModelList;
		for (int i = 0; i < parameterList.count(); i++) {
			RResponse *response = qobject_cast<RResponse *>(parameterList[i]);
			if (!response) {
				continue;
			}

			RModel *model = qobject_cast<RModel *>(response->getModel());
			if (!level0ModelList.contains(model)) {
				level0ModelList << model;
			}
		}

		modelListList << level0ModelList;

		// Establish the modelListList and modelMap
		for (int i = 0; i < modelListList.count(); i++) {
			QList<RModel *> leveliModelList = modelListList[i];
			QList<RModel *> nextLevelModelList;

			for (int j = 0; j < leveliModelList.count(); j++) {
				RModel *model = leveliModelList[j];

				QList<RResponse *> responseList = model->getResponseList();
				for (int k = 0; k < responseList.count(); k++) {
					RModel *responseModel = qobject_cast<RModel *>(responseList[k]->getModel());
					if (!nextLevelModelList.contains(responseModel)) {
						nextLevelModelList << responseModel;
					}
					modelMap.insertMulti(responseModel, model);
				}
			}

			if (!nextLevelModelList.isEmpty()) {
				modelListList << nextLevelModelList;
			}
		}

		// Reverse the modelListList
		int count = modelListList.count();
		for (int i = 0; i < count / 2.0; i++) {
			modelListList.swap(i, count - i - 1);
		}

		// Clear the duplicates from modelListList
		QList<RModel *> functionModelList = theFunction.data()->getModelList();
		for (int i = 0; i < modelListList.count(); i++) {
			QList<RModel *> leveliModelList = modelListList[i];

			for (int j = 0; j < leveliModelList.count(); j++) {
				RModel *model = leveliModelList[j];
				if (functionModelList.contains(model)) {
					functionModelList.removeOne(model);
				} else {
					leveliModelList.removeAt(j);
					j--;
				}
			}

			modelListList[i] = leveliModelList;
		}

		// Clear the duplicates from modelMap and store the final map in finalModelMap
		QMap<RModel *, QList<RModel *> > finalModelMap;
		QMapIterator<RModel *, RModel *> iterator(modelMap);
		while (iterator.hasNext()) {
			iterator.next();

			RModel *model = iterator.key();
			QList<RModel *> modelList = modelMap.values(model);
			QSet<RModel *> modelSet = modelList.toSet();
			modelList = modelSet.toList();
			finalModelMap[model] = modelList;
		}

		// Process the position of models in modelListList
		for (int i = 0; i < modelListList.count() - 1; i++) {
			QList<RModel *> leveliModelList = modelListList[i];
			QList<RModel *> nextLevelModelList = modelListList[i + 1];

			for (int j = 0; j < leveliModelList.count(); j++) {
				RModel *model = leveliModelList[j];
				QList<RModel *> dependentModelList = finalModelMap[model];

				for (int k = 0; k < dependentModelList.count(); k++) {
					RModel *dependentModel = dependentModelList[k];
					int lastIndex = nextLevelModelList.count() - 1;
					if (nextLevelModelList.contains(dependentModel)) {
						nextLevelModelList.move(nextLevelModelList.indexOf(dependentModel), lastIndex);
					}
				}
			}
			modelListList[i + 1] = nextLevelModelList;
		}

		// Convert the modelListList to objectListList and adding the function at the end
		QList<QObjectList> objectListList;
		for (int i = 0; i < modelListList.count(); i++) {
			QList<RModel *> leveliModelList = modelListList[i];
			QObjectList leveliObjectList;

			for (int j = 0; j < leveliModelList.count(); j++) {
				leveliObjectList << leveliModelList[j];
			}

			objectListList << leveliObjectList;
		}
		QObjectList functionObjectList;
		functionObjectList << theFunction.data();
		objectListList << functionObjectList;

		// Convert the finalModelMap to objectMap and adding the function at the end
		QMap<QPointer<QObject>, QObjectList> objectMap;
		QMapIterator<RModel *, QList<RModel *> > objectIterator(finalModelMap);
		while (objectIterator.hasNext()) {
			objectIterator.next();

			RModel *model = objectIterator.key();
			QList<RModel *> modelList = finalModelMap[model];

			QObjectList objectList;
			for (int i = 0; i < modelList.count(); i++) {
				objectList << modelList[i];
			}
			objectMap[model] = objectList;
		}
		for (int i = 0; i < level0ModelList.count(); i++) {
			objectMap[level0ModelList[i]] = functionObjectList;
		}
		
		// Instantiate the widget
		QPointer<RFlowchartWidget> widget = new RFlowchartWidget(theMainWindow);
		theMainWindow->addSubWindow(widget.data(), QString("%1 Model Flowchart").arg(theFunction.data()->objectName()), false);
		widget.data()->setObjectList(objectListList, objectMap);
	}



	return 0;
}

bool RFunctionEvaluationAnalyzer::canBeRunParallelly() {	
	return false;
}