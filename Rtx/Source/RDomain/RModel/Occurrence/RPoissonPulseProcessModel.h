#ifndef RPoissonPulseProcessModel_H
#define RPoissonPulseProcessModel_H

#include "RModel.h"
#include "ROccurrenceModel.h"

class RParameter;

class RPoissonPulseProcessModel : public ROccurrenceModel
{
	Q_OBJECT

	Q_PROPERTY(double MeanDuration READ getMeanDuration WRITE setMeanDuration)
	Q_PROPERTY(RTimeInstantGenerationScheme TimeInstantGenerationScheme READ getTimeInstantGenerationScheme WRITE setTimeInstantGenerationScheme)

	Q_ENUMS(RTimeInstantGenerationScheme);

public:
	RPoissonPulseProcessModel(QObject *parent, QString name);
	~RPoissonPulseProcessModel();

	enum RTimeInstantGenerationScheme{Exact, Approximate};

	
	double getMeanDuration() const;
	void setMeanDuration(double value);
	RTimeInstantGenerationScheme getTimeInstantGenerationScheme() const;
	void setTimeInstantGenerationScheme(RTimeInstantGenerationScheme value);
	

	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID);
	
	// Randomly defines durations that the model occures in them during the given time span, in the given threadID ($MTh$ - non-threadSafe)
	int establishTimeAxis(double T, gsl_rng *randomGenerator, int threadID);

	bool isRareHazard();

	// Returns true as the model is parallelized
	bool canBeRunParallelly();

private:

	
	double theMeanDuration;
	RTimeInstantGenerationScheme theTimeInstantGenerationScheme;
};

#endif // RPoissonPulseProcessModel_H
