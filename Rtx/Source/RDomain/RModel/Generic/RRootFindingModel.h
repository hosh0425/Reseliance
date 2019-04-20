#ifndef RRootFindingModel_H
#define RRootFindingModel_H

#include "RModel.h"

#include "muParser.h"

class RParameter;
class RResponse;

class QReadWriteLock;

class RRootFindingModel : public RModel
{
	Q_OBJECT

	Q_PROPERTY(QString Expression READ getExpression WRITE setExpression)
	Q_PROPERTY(RAlgorithm Algorithm READ getAlgorithm WRITE setAlgorithm)
	Q_PROPERTY(double MaximumIterations READ getMaximumIterations WRITE setMaximumIterations)
	Q_PROPERTY(double Tolerance READ getTolerance WRITE setTolerance)
	Q_PROPERTY(double InitialRoot READ getInitialRoot WRITE setInitialRoot)
	Q_PROPERTY(double LowerBound READ getLowerBound WRITE setLowerBound)
	Q_PROPERTY(double UpperBound READ getUpperBound WRITE setUpperBound)

	Q_ENUMS(RAlgorithm)

public:
	// Define the constructor and destructor
	RRootFindingModel(QObject *parent, QString name);
	~RRootFindingModel();

	enum RAlgorithm{Newton, Secant, Steffenson, Bisection, FalsePosition, BrentDekker};

	QString getExpression() const;
	void setExpression(QString value);
	RAlgorithm getAlgorithm() const;
	void setAlgorithm(RAlgorithm value);
	double getMaximumIterations() const;
	void setMaximumIterations(double value);
	double getTolerance() const;
	void setTolerance(double value);
	double getInitialRoot() const;
	void setInitialRoot(double value);
	double getUpperBound() const;
	void setUpperBound(double value);
	double getLowerBound() const;
	void setLowerBound(double value);

	QList<RParameter *> getAllParametersList(bool *ok);

	// returns a pointer to the parser related to the given threadID ($MTh$)
	mu::Parser *getParser(int threadID);
	
	// evaluates the model in the given threadID ($MTh$)
	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID);
	
	int parseExpression();

	// returns true as this model is parallelized
	bool canBeRunParallelly();

private:
	void reset();

	// Declare input data members
	QString theExpression;
	RAlgorithm theAlgorithm;
	double theTolerance;
	double theInitialRoot;
	double theUpperBound;
	double theLowerBound;

	// IMPORTANT : DO NOT USE THIS PROPERTY OUT OF THE SAFE AREA OF ITS LOCK. YOU CAN USE ITS GETTER
	double theMaximumIterations;
	
	// Declare responses
	RResponse *theResponse;

	// an array of parsers, each for evaluating the expression in one thread ($MTh$)
	mu::Parser ** theParser;

	// a read write lock for editable property in GUI while analysis
	QReadWriteLock * theMaximumIterationsLock;
};

#endif // RRootFindingModel_H
