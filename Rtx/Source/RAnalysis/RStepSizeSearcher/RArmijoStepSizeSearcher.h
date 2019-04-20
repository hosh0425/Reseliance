// Last modified by Mojtaba on June 30, 2012

#ifndef RArmijoStepSizeSearcher_H
#define RArmijoStepSizeSearcher_H

#include "RStepSizeSearcher.h"
#include "RPointer.h" //salarifard

#include <gsl/gsl_vector.h>

class RMeritChecker;
class RTransformer;

class RArmijoStepSizeSearcher : public RStepSizeSearcher
{
	Q_OBJECT

	// Sets the RTransformer object, which transforms points and gradients between original and standard normal spaces
	Q_PROPERTY(QObject *Transformer READ getTransformer WRITE setTransformer)

	// Sets the RMeritChecker object, which checks the merit function to either accept or reject the step size
	Q_PROPERTY(QObject *MeritChecker READ getMeritChecker WRITE setMeritChecker)
	
	// Sets the maximum number of step size reductions before the step size is accepted
	Q_PROPERTY(int MaximumReductions READ getMaximumReductions WRITE setMaximumReductions)

	// Sets the reduction coefficient, i.e., the next step size will be equal to the previous step size
	Q_PROPERTY(double Base READ getBase WRITE setBase)

	// Sets the first trial value for the step size
	Q_PROPERTY(double InitialStepSize READ getInitialStepSize WRITE setInitialStepSize)

	// Sets the number of times that the InitialStepSize should be tried before restoring to a step size of 1.0 and starting to reduce the step size
	Q_PROPERTY(int InitialStepsCount READ getInitialStepsCount WRITE setInitialStepsCount)

	// Sets the radius of the hyper-sphere within which the trial steps are restricted to stay
	Q_PROPERTY(double SphereRadius READ getSphereRadius WRITE setSphereRadius)

	// Sets the distance from the hyper-sphere surface; if two consecutive trial points have been located within this distance, then the radius is increased by the amount SphereEvolution
	Q_PROPERTY(double SphereDistance READ getSphereDistance WRITE setSphereDistance)

	// Sets the amount of the increase in the sphere radius if two consecutive trial points have been located within SphereDistance from the hyper-sphere surface
	Q_PROPERTY(double SphereEvolution READ getSphereEvolution WRITE setSphereEvolution)

public:
	RArmijoStepSizeSearcher(QObject *parent, QString name);
	~RArmijoStepSizeSearcher();

	double getStepSize() const;
	int computeStepSize(int iterationIndex, gsl_vector *u, RFunction *function, double functionValue, gsl_vector *functionGradient, gsl_vector const *stepDirection, QList<RRandomVariable *> *randomVariableList);
	double getFunctionValue();

	//QProperties
	QObject *getTransformer() const;
	void setTransformer(QObject *value);
	QObject *getMeritChecker() const;
	void setMeritChecker(QObject *value);
	//QObject *getRootFinder() const;
	//void setRootFinder(QObject *value);
	int getMaximumReductions() const;
	void setMaximumReductions(int value);
	double getBase() const;
	void setBase(double value);
	double getInitialStepSize() const;
	void setInitialStepSize(double value);
	int getInitialStepsCount() const;
	void setInitialStepsCount(int value);
	double getSphereRadius() const;
	void setSphereRadius(double value);
	double getSphereDistance() const;
	void setSphereDistance(double value);
	double getSphereEvolution() const;
	void setSphereEvolution(double value);
	bool canBeRunParallelly();

private:
	double theStepSize;

	RPointer<RTransformer> theTransformer;
	RPointer<RMeritChecker> theMeritChecker;
	//RPointer<RRootFinder> theRootFinder;
	int theMaximumReductions;
	double theBase;
	double theInitialStepSize;
	int theInitialStepsCount;
	double theSphereRadius;
	double theSphereDistance;
	double theSphereEvolution;

	double theFunctionValue;
	bool isCloseToSphere;

	//DELETE
	//double oldDistance;

};

#endif // RArmijoStepSizeSearcher_H
