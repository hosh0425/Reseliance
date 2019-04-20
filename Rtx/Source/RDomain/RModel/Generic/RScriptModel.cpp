#include "RScriptModel.h"

#include "RParameter.h"
#include "RResponse.h"
#include "RMainWindow.h"

#include <QScriptEngineDebugger>
#include <QAction>

#include <qmutex.h>
#include <QMutexLocker>

QScriptValue print(QScriptContext *context, QScriptEngine *engine);
RScriptModel *thisScriptModel;

RScriptModel::RScriptModel(QObject *parent, QString name)
	: RModel(parent, name)
{
	theDDM = false;

	// creating as much engines as the number of threads

	int arraySizeForParallelizedObject = getArraySizeForParallelizedObject();
	theEngine = new QScriptEngine[arraySizeForParallelizedObject];

	for (int thID = 0 ; thID < arraySizeForParallelizedObject; thID++){
		theEngine[thID].setProperty("threadID", QVariant(thID));
		theEngine[thID].globalObject().setProperty("print", theEngine[thID].newFunction(print, 1));
	}
	thisScriptModel = this;

	scriptTextExtarctorLock = new QMutex();
}

RScriptModel::~RScriptModel()
{
	delete [] theEngine;
	delete scriptTextExtarctorLock;
}

QString RScriptModel::getInputParameterList()
{
	return parameterListToString(theInputParameterList);
}

void RScriptModel::setInputParameterList(QString value)
{
	theInputParameterList = stringToParameterList(value);
}

QString RScriptModel::getScriptFile()
{
	return theScriptFile;
}

void RScriptModel::setScriptFile(QString value)
{
	theScriptFile = RDomain::stringToFilePath(value);
}

bool RScriptModel::getDDM()
{
	return theDDM;
}

void RScriptModel::setDDM(bool value)
{
	theDDM = value;
}

int RScriptModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
{	
	/*
	// Clearing null pointers
	nullRemoverLock->lock();
	clearNullPointers(&theInputParameterList);
	nullRemoverLock->unlock();
	*/

	if (getDisplayOutput()) {
		rDebug(threadID) << "Starting to run script in model" << objectName() << "...";
	}

	for (int i = 0; i < theInputParameterList.count(); i++) {
		theEngine[threadID].globalObject().setProperty(theInputParameterList[i].data()->objectName(), theInputParameterList[i].data()->getCurrentValue(threadID));
	}

	// For DDM
	theEngine[threadID].globalObject().setProperty("evaluateDDM", evaluateDDM);

	// start of safe area scope
	{
		QMutexLocker locker(scriptTextExtarctorLock);

		if (theScriptText.isEmpty()) {
			QFile scriptFile(theScriptFile);
			if (!scriptFile.open(QIODevice::ReadOnly)) {
				// Error
				rCritical(threadID) << "Error: Could not open the script file" << theScriptFile << "given to model" << objectName() << ".";
				scriptTextExtarctorLock->unlock();
				return -1;
			}

			QTextStream stream(&scriptFile);
			theScriptText = stream.readAll();
			if (theScriptText.isEmpty()) {
				// Error
				rCritical(threadID) << "Error: The script file" << theScriptFile << "given to model" << objectName() << " is empty.";
				scriptTextExtarctorLock->unlock();
				return -1;
			}
			scriptFile.close();
		}
	}
	// end of safe area scope

	QScriptValue scriptResult = theEngine[threadID].evaluate(theScriptText, theScriptFile);
	if (theEngine[threadID].hasUncaughtException()) {
		int line = theEngine[threadID].uncaughtExceptionLineNumber();
		rCritical(threadID) << "Error: Uncaught exception at line" << line << "in the script file given to model" << objectName() << ":" << scriptResult.toString() << ".";
		return -1;
	}

	QList<RResponse *> outputResponseList = this->findChildren<RResponse *>();
	for (int i = 0; i < outputResponseList.count(); i++) {
		QScriptValue scriptValue = theEngine[threadID].evaluate(outputResponseList[i]->objectName());
		if (theEngine[threadID].hasUncaughtException()) {
			int line = theEngine[threadID].uncaughtExceptionLineNumber();
			rCritical(threadID) << "Error: Uncaught exception at line" << line << "in the script file given to model" << objectName() << ":" << scriptValue.toString() << ".";
			return -1;
		}
		double response = scriptValue.toNumber();
		outputResponseList[i]->setCurrentValue(response, threadID);
	}

	// Reading the DDM results from the script
	// Notation: The variable value that is demanded form the script has the following name: d_Response_d_Parameter
	int result = 0;
	if (evaluateDDM) {
		if (theDDM) {
			for (int i = 0; i < outputResponseList.count(); i++) {
				QMap<RParameter *, double> theGradientMap;
				for (int j = 0; j < theInputParameterList.count(); j++) {
					QString DDMVariableName = QString("d_%1_d_%2").arg(outputResponseList[i]->objectName()).arg(theInputParameterList[j].data()->objectName());
					QScriptValue scriptValue = theEngine[threadID].evaluate(DDMVariableName);
					if (theEngine[threadID].hasUncaughtException()) {
						int line = theEngine[threadID].uncaughtExceptionLineNumber();
						rCritical(threadID) << "Error: Uncaught exception at line" << line << "in the script file given to model" << objectName() << ":" << scriptValue.toString() << ".";
						rCritical(threadID) << "Error: Could not read the DDM value" << DDMVariableName << "in the model" << objectName() << ".";
						
						// return -10 means that the DDM evluation has failed
						return -10;
					}
					double responseDDM = scriptValue.toNumber();
					theGradientMap.insertMulti(theInputParameterList[j].data(), responseDDM);
				}
				theDDMMap.insertMulti(outputResponseList[i], theGradientMap);
			}
			result = 1;
		} else {
			result = 0;
		}
	}

	if (getDisplayOutput()) {
		rDebug(threadID) << "Running script in model" << objectName() << "done.";
		rDebug(threadID) << " ";
	}

	return result;
}

QStringList RScriptModel::getActionList()
{
	QStringList actionList;
	actionList << "&Debug";
	return actionList;
}

void RScriptModel::reset()
{
	theScriptText.clear();
}

void RScriptModel::debug()
{
	QScriptEngineDebugger *debugger = 0;
	debugger = this->findChild<QScriptEngineDebugger *>();
	if (debugger) {
		debugger->detach();
		//debugger->standardWindow()->close();
		delete debugger;
		debugger = 0;
	}

	debugger = new QScriptEngineDebugger(this);

	QWidget *debuggerWidget = debugger->standardWindow();
	//debuggerWidget->setObjectName("QScriptEngineDebugger");
	//RMainWindow *mainWindow = qobject_cast<RMainWindow *>(parent()->parent());
	//mainWindow->addSubWindow(debuggerWidget, objectName() + " Debugger", false);
	debuggerWidget->setWindowModality(Qt::ApplicationModal);

	debugger->attachTo(&theEngine[0]);
	debugger->action(QScriptEngineDebugger::InterruptAction)->trigger();
	evaluateModel(theDDM, RObject::RandomVariable,0);
}

QScriptValue print(QScriptContext *context, QScriptEngine *engine)
{

	// reading threadID from the "threadID" property of the engine
	int threadID = engine->property("threadID").toInt();

	if (thisScriptModel->getDisplayOutput() == true) {
		QString result;
		for (int i = 0; i < context->argumentCount(); i++) {
			//if (i > 0) {
			//	result.append(" ");
			//}
			result.append(context->argument(i).toString());
		}
		RObject::rDebug(threadID) << qPrintable(result);
	}
	return engine->undefinedValue();
}

bool RScriptModel::canBeRunParallelly(){
	return true;
}