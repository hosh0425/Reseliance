#ifndef RScriptModel_H
#define RScriptModel_H

#include "RModel.h"

#include <QtScript>

class RParameter;
class RResponse;

class QMutex;

class RScriptModel : public RModel
{
	Q_OBJECT

	Q_PROPERTY(QString InputParameterList READ getInputParameterList WRITE setInputParameterList)
	Q_PROPERTY(QString ScriptFile READ getScriptFile WRITE setScriptFile)
	Q_PROPERTY(bool DDM READ getDDM WRITE setDDM)

public:
	RScriptModel(QObject *parent, QString name);
	~RScriptModel();

	// Define the mandatory read/write methods for all QProperties
	QString getInputParameterList();
	void setInputParameterList(QString value);
	QString getScriptFile();
	void setScriptFile(QString value);
	bool getDDM();
	void setDDM(bool value);

	// Define the method where the actual model is coded, in the given threadID ($MTh$)
	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID);

	QStringList getActionList();
	void reset();

	// returns true as this model is parallelized
	bool canBeRunParallelly();

private slots:
	void debug();

private:
	// Declare the list of input data members
	QList<RPointer<RParameter> > theInputParameterList;
	QString theScriptFile;
	bool theDDM;

	QScriptEngine * theEngine;
	QString theScriptText;

	// a lock for preventing extracting the script in more than one thread simultanously
	QMutex * scriptTextExtarctorLock;

};

#endif // RScriptModel_H
