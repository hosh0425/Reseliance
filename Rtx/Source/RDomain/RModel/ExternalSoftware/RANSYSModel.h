#ifndef RANSYSModel_H
#define RANSYSModel_H

#include "RExternalSoftwareModel.h"

class RANSYSModel : public RExternalSoftwareModel
{
	Q_OBJECT

	Q_PROPERTY(QString ParameterList READ getParameterList WRITE setParameterList)
	Q_PROPERTY(QString ExecutableFile READ getExecutableFile WRITE setExecutableFile)
	Q_PROPERTY(QString InputFile READ getInputFile WRITE setInputFile)

public:
	RANSYSModel(QObject *parent, QString name);
	~RANSYSModel();

	QString getParameterList();
	void setParameterList(QString value);

	QString getExecutableFile() const;
	void setExecutableFile(QString value);

	QString getInputFile() const;
	void setInputFile(QString value);


	// evaluates model in the given threadID ($MTh$)
	// The return value of evaluateModel:
	// 0 mean analysis is run without error;
	// -1 means analysis is not run due to errors other than non-convergence, e.g. the RResponse is not associated with any model, or the executable file of the model is not found;
	// In case of file interaction with external software: < -1 means function is not evaluated due to non-convergence (hopefully), because the output file is not found, or it does not have the desired number in the indicated coordinates
	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID);
	void exitProcess(int threadID);
	void reset();

	// Returns the path which is set as working path in sequential analysis (here the absolute path of theIputFile)
	QString getTheMainWorkingPath();

	// returns true as this model is parallelized
	bool canBeRunParallelly();

private:

	QList<RPointer<RParameter> > theParameterList;
	QString theExecutableFile;
	QString theInputFile;


};

#endif // RANSYSModel_H
