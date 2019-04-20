#ifndef REMMEModel_H
#define REMMEModel_H

#include "RExternalSoftwareModel.h"

class REMMEModel : public RExternalSoftwareModel
{
	Q_OBJECT

	Q_PROPERTY(QString ParameterList READ getParameterList WRITE setParameterList)
	Q_PROPERTY(QString InputScenarioFile READ getInputScenarioFile WRITE setInputScenarioFile)
	Q_PROPERTY(QString OutputScenarioFile READ getOutputScenarioFile WRITE setOutputScenarioFile)

public:
	REMMEModel(QObject *parent, QString name);
	~REMMEModel();

	QString getParameterList();
	void setParameterList(QString value);

	QString getInputScenarioFile() const;
	void setInputScenarioFile(QString value);

	QString getOutputScenarioFile() const;
	void setOutputScenarioFile(QString value);

	// evaluates model in the given threadID ($MTh$)
	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID);
	void exitProcess(int threadID);

	// Returns the path which is set as working path in sequential analysis (here the absolute path of theInputScenarioFile)
	QString getTheMainWorkingPath();

	// returns true as this model is parallelized
	bool canBeRunParallelly();

private:
	QList<RPointer<RParameter> > theParameterList;
	QString theInputScenarioFile;
	QString theOutputScenarioFile;

};

#endif // REMMEModel_H
