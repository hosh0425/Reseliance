#ifndef RSimpleBuildingLifeCycleWaterUsageModel_H
#define RSimpleBuildingLifeCycleWaterUsageModel_H

#include "RModel.h"

class RParameter;
class RResponse;

class RSimpleBuildingLifeCycleWaterUsageModel : public RModel
{
	Q_OBJECT

	// Define a QProperty for each input (parameter) data member. The type of the QProperty is "QObject *"
	Q_PROPERTY(QObject *PreUseWIntensity READ getPreUseWIntensity WRITE setPreUseWIntensity)
	Q_PROPERTY(QObject *NumberStoreys READ getNumberStoreys WRITE setNumberStoreys)
	Q_PROPERTY(QObject *FootprintArea READ getFootprintArea WRITE setFootprintArea)
	Q_PROPERTY(QObject *AnnualWIntensity READ getAnnualWIntensity WRITE setAnnualWIntensity)
	Q_PROPERTY(QObject *DesignLife READ getDesignLife WRITE setDesignLife)


public:
	// Define the constructor and destructor
	RSimpleBuildingLifeCycleWaterUsageModel(QObject *parent, QString &name);
	~RSimpleBuildingLifeCycleWaterUsageModel();

	// Define the mandatory read/write methods for all QProperties
	QObject *getPreUseWIntensity() const;
	void setPreUseWIntensity(QObject *value);

	QObject *getNumberStoreys() const;
	void setNumberStoreys(QObject *value);

	QObject *getFootprintArea() const;
	void setFootprintArea(QObject *value);

	QObject *getAnnualWIntensity() const;
	void setAnnualWIntensity(QObject *value);

	QObject *getDesignLife() const;
	void setDesignLife(QObject *value);



	// Define the method where the actual model is coded
	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID);

	// returns true as this model is parallelized
	bool canBeRunParallelly();

private:
	// Declare input data members
	RPointer<RParameter> thePreUseWIntensity;
	RPointer<RParameter> theNumberStoreys;
	RPointer<RParameter> theFootprintArea;
	RPointer<RParameter> theAnnualWIntensity;
	RPointer<RParameter> theDesignLife;


	// Declare responses
	RResponse *theWaterUsageCoarseResponse;

};

#endif // RSimpleBuildingLifeCycleWaterUsageModel_H
