#ifndef RMasonryBuildingRetrofitCostModel_H
#define RMasonryBuildingRetrofitCostModel_H

#include "RModel.h"

class RParameter;
class RResponse;

class RMasonryBuildingRetrofitCostModel : public RModel
{
	Q_OBJECT

	Q_PROPERTY(RRetrofitType RetrofitType READ getRetrofitType WRITE setRetrofitType)
	Q_PROPERTY(bool WithTieColumn READ getWithTieColumn WRITE setWithTieColumn)
	Q_PROPERTY(bool RigidDiaphragm READ getRigidDiaphragm WRITE setRigidDiaphragm)
	Q_PROPERTY(QObject *BuildingInformationModel READ getBuildingInformationModel WRITE setBuildingInformationModel)
	Q_PROPERTY(QObject *Alpha READ getAlpha WRITE setAlpha)
	Q_PROPERTY(QObject *Theta0 READ getTheta0 WRITE setTheta0)
	Q_PROPERTY(QObject *Theta1 READ getTheta1 WRITE setTheta1)
	Q_PROPERTY(QObject *Theta2 READ getTheta2 WRITE setTheta2)
	Q_PROPERTY(QObject *Epsilon READ getEpsilon WRITE setEpsilon)
	Q_PROPERTY(QObject *Sigma READ getSigma WRITE setSigma)
	Q_PROPERTY(QObject *CostEpsilon READ getCostEpsilon WRITE setCostEpsilon)
	Q_PROPERTY(double UnitAreaReplacementCost READ getUnitAreaReplacementCost WRITE setUnitAreaReplacementCost)
	Q_PROPERTY(QObject *RetrofitStatus READ getRetrofitStatus WRITE setRetrofitStatus)

	// Enum for the type of retrofit action
	Q_ENUMS(RRetrofitType)

public:
	RMasonryBuildingRetrofitCostModel(QObject *parent, QString name);
	~RMasonryBuildingRetrofitCostModel();

	// Declaring the type of retrofit action
	enum RRetrofitType{Shotcrete, SteelBelt, FRP};

	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID = 0);

	// returns true as this model is parallelized
	bool canBeRunParallelly();

	RRetrofitType getRetrofitType() const;
	bool getWithTieColumn() const;
	bool getRigidDiaphragm() const;
	QObject *getBuildingInformationModel() const;
	QObject *getAlpha() const;
	QObject *getTheta1() const;
	QObject *getTheta2() const;
	QObject *getTheta0() const;
	QObject *getEpsilon() const;
	QObject *getSigma() const;
	QObject *getCostEpsilon() const;
	double getUnitAreaReplacementCost() const;


	void setRetrofitType(RRetrofitType value);
	void setWithTieColumn(bool value);
	void setRigidDiaphragm(bool value);
	void setBuildingInformationModel(QObject *value);
	void setAlpha(QObject *value);
	void setTheta1(QObject *value);
	void setTheta2(QObject *value);
	void setTheta0(QObject *value);
	void setEpsilon(QObject *value);
	void setSigma(QObject *value);
	void setCostEpsilon(QObject *value);
	void setUnitAreaReplacementCost(double value);

	QObject *getRetrofitStatus() const;
	void setRetrofitStatus(QObject *value);

private:
	RRetrofitType theRetrofitType;
	bool theWithTieColumn;
	bool theRigidDiaphragm;
	RPointer<RModel> theBuildingInformationModel;
	RPointer<RParameter> theAlpha; // Denotes percenatage of increase in lateral strength of structure due tto retrofit
	RPointer<RParameter> theTheta1;
	RPointer<RParameter> theTheta2;
	RPointer<RParameter> theTheta0;
	RPointer<RParameter> theEpsilon;
	RPointer<RParameter> theSigma;
	RPointer<RParameter> theCostEpsilon;
	double theUnitAreaReplacementCost;

	RPointer<RParameter> theRetrofitStatus;

	RResponse *theRetrofitCost; // Ratio of retrofit cost to replacement cost * replacemnet cost * error(N(1,.1))
};
#endif //RMasonryBuildingRetrofitCostModel_H