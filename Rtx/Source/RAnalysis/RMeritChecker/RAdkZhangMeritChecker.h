// Last modified by Mojtaba on June 29, 2012

#ifndef RAdkZhangMeritChecker_H
#define RAdkZhangMeritChecker_H

#include "RMeritChecker.h"

#include <gsl/gsl_vector.h>

class RAdkZhangMeritChecker : public RMeritChecker
{
	Q_OBJECT

	// Sets the parameter to compute the penalty factor c = |u|^2 / |grad(g)\^2 * Multiplier + Adder;
	Q_PROPERTY(double Multiplier READ getMultiplier WRITE setMultiplier)

	// Sets the parameter to compute the penalty factor c = |u|^2 / |grad(g)\^2 * Multiplier + Adder;
	Q_PROPERTY(double Adder READ getAdder WRITE setAdder)

	// Sets the factor in the merit function check: delta(merit) <= Factor * stepSize * [c*grad(g)+u].stepDirection
	Q_PROPERTY(double Factor READ getFactor WRITE setFactor)

public:
	RAdkZhangMeritChecker(QObject *parent, QString name);
	~RAdkZhangMeritChecker();

	// getter/setter methods for meta-properties
	double getMultiplier() const;
	void setMultiplier(double value);
	double getAdder() const;
	void setAdder(double value);
	double getFactor() const;
	void setFactor(double value);

	// Checks the merit function to see if the current step size in FORM is acceptable
	int check(gsl_vector *u, double functionValue, gsl_vector *functionGradient, double stepSize, const gsl_vector *stepDirection, double newFunctionValue);

	// Computes and returns the value of the merit function
	double getMeritFunctionValue(gsl_vector *u, double functionValue, gsl_vector *functionGradient);

	// Updates the merit function penalty factor c
	int updateMeritParameters(gsl_vector *u, double functionValue, gsl_vector *functionGradient);

	bool canBeRunParallelly();

private:
	// Data members for meta-properties
	double theMultiplier;
	double theAdder;
	double theFactor;

	// Data member for the penalty factor c
	double c;
	
};

#endif // RAdkZhangMeritChecker_H
