#include "RAnalyzer.h"

#include "RDomain.h"
#include "RMainWindow.h"

RAnalyzer::RAnalyzer(QObject *parent, QString name)
	: RObject(parent, name)
{
	theDomain = qobject_cast<RDomain *>(parent);
	theMainWindow = qobject_cast<RMainWindow *>(parent->parent());
}

RAnalyzer::~RAnalyzer()
{

}

int RAnalyzer::startAnalyzing(){

	// informing the domain, a new analysis is starting
	theDomain->newAnalysisStarted();

	// analyzing
	int status = analyze();

	// informing the domain that the analysis ended
	theDomain->anAnalysisEnded();

	return status;
}

QStringList RAnalyzer::getActionList()
{
	QStringList actionList;
	actionList << "&Run Analysis";
	return actionList;
}

void RAnalyzer::runAnalysis()
{
	int status = startAnalyzing();
	if (status < 0) {
		rCritical() << "Error in" << objectName() << ": The analysis did not converge because of errors";
	}
}
