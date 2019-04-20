// Last modified by Mojtaba on June 29, 2012

#ifndef RFunctionEvaluationAnalyzer_H
#define RFunctionEvaluationAnalyzer_H

#include "RAnalyzer.h"

class RFunction;

class RFunctionEvaluationAnalyzer : public RAnalyzer
{
	Q_OBJECT

	// Sets the function subject to evaluation
	Q_PROPERTY(QObject *Function READ getFunction WRITE setFunction)

	// Indicates whether or not to evaluate the function gradient as well
	Q_PROPERTY(bool EvaluateGradient READ getEvaluateGradient WRITE setEvaluateGradient)

	// Indicates whether or not to set the current value of all random variables to their mean value before the function evaluation
	Q_PROPERTY(bool SetRandomVariablesToMean READ getSetRandomVariablesToMean WRITE setSetRandomVariablesToMean)

	// Indicates whether or not to print a list of random variables that affect the passed function to the output pane
	Q_PROPERTY(bool PrintRandomVariableList READ getPrintRandomVariableList WRITE setPrintRandomVariableList)

	// Indicates whether or not to generate a flowchart of models
	Q_PROPERTY(bool PlotModelFlowchart READ getPlotModelFlowchart WRITE setPlotModelFlowchart)

public:
	RFunctionEvaluationAnalyzer(QObject *parent, QString name);
	~RFunctionEvaluationAnalyzer();

	// getter/setter methods for meta-properties
	QObject *getFunction() const;
	void setFunction(QObject *value);
	bool getEvaluateGradient() const;
	void setEvaluateGradient(bool value);
	bool getSetRandomVariablesToMean() const;
	void setSetRandomVariablesToMean(bool value);
	bool getPrintRandomVariableList() const;
	void setPrintRandomVariableList(bool value);
	bool getPlotModelFlowchart() const;
	void setPlotModelFlowchart(bool value);
	bool canBeRunParallelly();

	// Runs the analysis
	int analyze();

private:
	// Data members for meta-properties
	RPointer<RFunction> theFunction;
	bool theEvaluateGradient;
	bool theSetRandomVariablesToMean;
	bool thePrintRandomVariableList;
	bool thePlotModelFlowchart;
	
};

#endif // RFunctionEvaluationAnalyzer_H
