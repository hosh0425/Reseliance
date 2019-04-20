// Last modified by Mojtaba on June 29, 2012

#ifndef RAnalyzer_H
#define RAnalyzer_H

#include "RObject.h"
#include "RPointer.h" //salarifard

class RDomain;
class RMainWindow;

class RAnalyzer : public RObject
{
	Q_OBJECT

public:
	RAnalyzer(QObject *parent, QString name);
	virtual ~RAnalyzer();

	// IMPORTANT : THIS IS THE METHOD WHICH MUST BE CALLED IN GUI
	// Starts a new analysis. This is the method which must be called through GUI so parameters about there is ongoing analysis can be set
	int startAnalyzing();

	// Runs the analysis
	virtual int analyze() = 0;

	// Returns a list of "right-click" menu items
	QStringList getActionList();

protected:

	// Data members that are inherited by subclasses
	RDomain *theDomain;
	RMainWindow *theMainWindow;

private slots:
	// Slot that is called by the GUI to trigger an analysis from the "right-click" menu
	void runAnalysis();

private:
	
};

#endif // RAnalyzer_H
