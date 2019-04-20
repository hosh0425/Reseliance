// Last modified by Mojtaba on June 29, 2012

#ifndef RFOSMAnalyzer_H
#define RFOSMAnalyzer_H

#include "RAnalyzer.h"

class RFunction;

class RFOSMAnalyzer : public RAnalyzer
{
	Q_OBJECT

	// Sets the function subject to FOSM approximation or FOSM reliability analysis
	Q_PROPERTY(QObject *Function READ getFunction WRITE setFunction)

	// Indicates whether or not to print the correlation matrix to the output pane
	Q_PROPERTY(bool PrintCorrelationMatrix READ getPrintCorrelationMatrix WRITE setPrintCorrelationMatrix)

	// Indicates whether or not to print the covariance matrix to the output pane
	Q_PROPERTY(bool PrintCovarianceMatrix READ getPrintCovarianceMatrix WRITE setPrintCovarianceMatrix)

public:
	RFOSMAnalyzer(QObject *parent, QString name);
	~RFOSMAnalyzer();

	// getter/setter methods for meta-properties
	QObject *getFunction() const;
	void setFunction(QObject *value);
	bool getPrintCorrelationMatrix() const;
	void setPrintCorrelationMatrix(bool value);
	bool getPrintCovarianceMatrix() const;
	void setPrintCovarianceMatrix(bool value);

	// Runs the analysis
	int analyze();

	bool canBeRunParallelly();
private:
	// Data members for meta-properties
	RPointer<RFunction> theFunction;
	bool thePrintCorrelationMatrix;
	bool thePrintCovarianceMatrix;
	
};

#endif // RFOSMAnalyzer_H
