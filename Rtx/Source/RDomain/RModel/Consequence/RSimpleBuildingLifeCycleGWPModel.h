#ifndef RSimpleBuildingLifeCycleGWPModel_H
#define RSimpleBuildingLifeCycleGWPModel_H

#include "RModel.h"

class RParameter;
class RResponse;

class RSimpleBuildingLifeCycleGWPModel : public RModel
{
	Q_OBJECT

	// Define a QProperty for each input (parameter) data member. The type of the QProperty is "QObject *"
	Q_PROPERTY(QObject *LifeCycleEnergy READ getLifeCycleEnergy WRITE setLifeCycleEnergy)
	Q_PROPERTY(QObject *OperationEnergy READ getOperationEnergy WRITE setOperationEnergy)
	Q_PROPERTY(QObject *GWPIntensityOperation READ getGWPIntensityOperation WRITE setGWPIntensityOperation)
	Q_PROPERTY(QObject *GWPIntensityNonOperation READ getGWPIntensityNonOperation WRITE setGWPIntensityNonOperation)



public:
	// Define the constructor and destructor
	RSimpleBuildingLifeCycleGWPModel(QObject *parent, QString &name);
	~RSimpleBuildingLifeCycleGWPModel();

	// Define the mandatory read/write methods for all QProperties
	QObject *getLifeCycleEnergy() const;
	void setLifeCycleEnergy(QObject *value);

	QObject *getOperationEnergy() const;
	void setOperationEnergy(QObject *value);

	QObject *getGWPIntensityOperation() const;
	void setGWPIntensityOperation(QObject *value);

	QObject *getGWPIntensityNonOperation() const;
	void setGWPIntensityNonOperation(QObject *value);




	// Define the method where the actual model is coded
	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID);

	// returns true as this model is parallelized
	bool canBeRunParallelly();

private:
	// Declare input data members
	RPointer<RParameter> theLifeCycleEnergy;
	RPointer<RParameter> theOperationEnergy;
	RPointer<RParameter> theGWPIntensityOperation;
	RPointer<RParameter> theGWPIntensityNonOperation;



	// Declare responses
	RResponse *theGWPCoarseResponse;

};

#endif // RSimpleBuildingLifeCycleGWPModel_H
