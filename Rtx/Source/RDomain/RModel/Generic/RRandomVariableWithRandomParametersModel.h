#ifndef RRandomVariableWithRandomParametersModel_H
#define RRandomVariableWithRandomParametersModel_H

#include "RModel.h"

class RParameter;
class RResponse;

//class QMutex;

class RRandomVariableWithRandomParametersModel : public RModel
{
	Q_OBJECT

	Q_PROPERTY(QObject *StandardNormalRandomVariable READ getStandardNormalRandomVariable WRITE setStandardNormalRandomVariable)
	Q_PROPERTY(QString DistributionType READ getDistributionType WRITE setDistributionType)
	Q_PROPERTY(RRandomParameterType RandomParameterType READ getRandomParameterType WRITE setRandomParameterType)
	Q_PROPERTY(QString ParameterList READ getParameterList WRITE setParameterList)
	
	Q_ENUMS(RRandomParameterType)

public:
	RRandomVariableWithRandomParametersModel(QObject *parent, QString name);
	~RRandomVariableWithRandomParametersModel();

	enum RRandomParameterType{Parameter1ToParameter4, MeanStdvParameter3Parameter4};

	QObject *getStandardNormalRandomVariable() const;
	void setStandardNormalRandomVariable(QObject *value);
	QString getDistributionType() const;
	void setDistributionType(QString value);
	RRandomParameterType getRandomParameterType() const;
	void setRandomParameterType(RRandomParameterType value);
	QString getParameterList();
	void setParameterList(QString value);

	// ATTENTION: It is assumed that theStandardNormalRandomVariable is the "physical parameter" that should be sampled over time in a scenario sampling analysis.
	QList<RParameter *> getPhysicalParameterList();

	// evaluates the model in the given threadID ($MTh$)
	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID);

	void setDynamicProperty(QString propertyName, QVariant propertyValue);

	// returns true as this model is parallelized
	bool canBeRunParallelly();

private:

	RPointer<RParameter> theStandardNormalRandomVariable;
	QString theDistributionType;
	RRandomParameterType theRandomParameterType;
	QList<RPointer<RParameter> > theParameterList;
	
	// Declare responses
	RResponse *theResponse;
};

#endif // RRandomVariableWithRandomParametersModel_H
