#ifndef RAlgebraicExpressionModel_H
#define RAlgebraicExpressionModel_H

#include "RModel.h"

#include "muParser.h"

class RParameter;
class RResponse;
class QMutex;

class RAlgebraicExpressionModel : public RModel
{
	Q_OBJECT

	Q_PROPERTY(QString Expression READ getExpression WRITE setExpression)
	Q_PROPERTY(QString PhysicalParameters READ getPhysicalParameters WRITE setPhysicalParameters)

public:
	// Define the constructor and destructor
	RAlgebraicExpressionModel(QObject *parent, QString name);
	~RAlgebraicExpressionModel();

	QString getExpression() const;
	void setExpression(QString value);

	QList<RParameter *> getAllParametersList(bool *ok);

	// Return a list of parameters that are "physical," i.e., they should be sampled for every new function evaluation over time in a scenario sampling analysis 
	QList<RParameter *> getPhysicalParameterList();
	QString getPhysicalParameters();
	void setPhysicalParameters(QString value);

	// evaluates the model in the given threadID ($MTh$)
	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID);

	// returns true as this model is parallelized
	bool canBeRunParallelly();

private:
	
	int parseExpression();
	void reset();

	// Declare input data members
	QString theExpression;
	QList<RPointer<RParameter> > thePhysicalParameters;
	
	// Declare responses
	RResponse *theResponse;

	// an array of parsers, each for evaluating the expression in one thread ($MTh$)
	mu::Parser ** theParser;

	// a lock for preventing reexecution of the parse method in several threads ($MTh$)
	QMutex * parseLock;
};

#endif // RAlgebraicExpressionModel_H
