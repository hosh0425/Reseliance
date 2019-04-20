#ifndef RSAP2000Model_H
#define RSAP2000Model_H

#include "RExternalSoftwareModel.h"

class RSAP2000Model : public RExternalSoftwareModel
{
	Q_OBJECT

	// Enum to indcate whether Standard or advanced solver should be used
	Q_ENUMS(RSolverType)

	Q_PROPERTY(QString ParameterList READ getParameterList WRITE setParameterList)
	Q_PROPERTY(QString ExecutableFile READ getExecutableFile WRITE setExecutableFile)
	Q_PROPERTY(QString InputFile READ getInputFile WRITE setInputFile)
	Q_PROPERTY(RSolverType SolverType READ getSolverType WRITE setSolverType)

public:
	RSAP2000Model(QObject *parent, QString name);
	~RSAP2000Model();

	// Declaring the ROutputDisplayLevelType enum values
	enum RSolverType{Standard, Advanced};

	QString getParameterList();
	void setParameterList(QString value);

	QString getInputFile() const;
	void setInputFile(QString value);

	QString getExecutableFile() const;
	void setExecutableFile(QString value);

	// setter/getter for the output display levels
	RSolverType getSolverType() const;
	void setSolverType(RSolverType value);

	// evaluates model in the given threadID ($MTh$)
	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID);
	void exitProcess(int threadID);

	// Returns the path which is set as working path in sequential analysis (here the absolute path of theIputFile)
	QString getTheMainWorkingPath();

	// returns true as this model is parallelized
	bool canBeRunParallelly();

private:
	QList<RPointer<RParameter> > theParameterList;
	QString theInputFile;
	QString theExecutableFile;
	QString theInputScenarioFile;
	QString theOutputScenarioFile;
	RSolverType theSolverType;

};

#endif // RSAP2000Model_H
