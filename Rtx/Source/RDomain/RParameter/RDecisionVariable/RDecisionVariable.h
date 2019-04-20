// Last modified by Mojtaba on June 30, 2012

#ifndef RDecisionVariable_H
#define RDecisionVariable_H

#include "RParameter.h"

class RDecisionVariable : public RParameter
{
	Q_OBJECT

	// Current realization of the decision variable
	Q_PROPERTY(double CurrentValue READ getCurrentValue WRITE setCurrentValue)

public:
	RDecisionVariable(QObject *parent, QString name);
	virtual ~RDecisionVariable();

	// Returns a list of parameters that are input to the decision variable as lower bound, upper bound, etc.
	virtual QList<RParameter *> getParameterList() = 0;

private:
	
};

#endif // RDecisionVariable_H
