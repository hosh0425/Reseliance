// Last modified by Mojtaba on June 30, 2012

#ifndef RConstant_H
#define RConstant_H

#include "RParameter.h"

class RConstant : public RParameter
{
	Q_OBJECT

	// Current value of the constant
	Q_PROPERTY(double CurrentValue READ getCurrentValue WRITE setCurrentValue  USER false)

public:
	RConstant(QObject *parent, QString name);
	~RConstant();

	// RConstant may be used as a response that flows upstream (e.g., when an attenuation model needs the natural period from a building model)
	// In this situation, the list of parameters that are used to compute such a response must be given to the RConstant object via this method
	void setUpstreamParameterList(QList<RParameter *> parameterList);

	// Returns the list of parameters that are used to compute a response that flows upstream, as explained above
	QList<RParameter *> getUpstreamParameterList();

	// returns true as this parameter is parallelized
	bool canBeRunParallelly();

private:
	// Data member for the list of parameters as explained above
	QList<RParameter *> theUpstreamParameterList;
	
};

#endif // RConstant_H
