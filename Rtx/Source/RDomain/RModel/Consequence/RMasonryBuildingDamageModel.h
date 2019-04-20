#ifndef RMasonryBuildingDamageModel_H
#define RMasonryBuildingDamageModel_H

#include "RModel.h"

class RParameter;
class RResponse;

class RMasonryBuildingDamageModel : public RModel
{
	Q_OBJECT

	Q_PROPERTY(bool WithTieColumn READ getWithTieColumn WRITE setWithTieColumn)
	Q_PROPERTY(bool RigidDiaphragm READ getRigidDiaphragm WRITE setRigidDiaphragm)
	Q_PROPERTY(QObject *BuildingInformationModel READ getBuildingInformationModel WRITE setBuildingInformationModel)
	Q_PROPERTY(QObject *DemandPGA READ getDemandPGA WRITE setDemandPGA)
	Q_PROPERTY(QObject *DesignBaseShearCoefficient READ getDesignBaseShearCoefficient WRITE setDesignBaseShearCoefficient)
	Q_PROPERTY(QObject *Theta1 READ getTheta1 WRITE setTheta1)
	Q_PROPERTY(QObject *Theta2 READ getTheta2 WRITE setTheta2)
	Q_PROPERTY(QObject *Theta3 READ getTheta3 WRITE setTheta3)
	Q_PROPERTY(QObject *Theta4 READ getTheta4 WRITE setTheta4)
	Q_PROPERTY(QObject *Theta5 READ getTheta5 WRITE setTheta5)
	Q_PROPERTY(QObject *Theta6 READ getTheta6 WRITE setTheta6)
	Q_PROPERTY(QObject *Epsilon READ getEpsilon WRITE setEpsilon)
	Q_PROPERTY(QObject *Sigma READ getSigma WRITE setSigma)

public:
	RMasonryBuildingDamageModel(QObject *parent, QString name);
	~RMasonryBuildingDamageModel();

	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID = 0);

	// returns true as this model is parallelized
	bool canBeRunParallelly();

	bool getWithTieColumn() const;
	bool getRigidDiaphragm() const;
	QObject *getBuildingInformationModel() const;
	QObject *getDemandPGA() const;
	QObject *getDesignBaseShearCoefficient() const;
	QObject *getTheta1() const;
	QObject *getTheta2() const;
	QObject *getTheta3() const;
	QObject *getTheta4() const;
	QObject *getTheta5() const;
	QObject *getTheta6() const;
	QObject *getEpsilon() const;
	QObject *getSigma() const;

	void setWithTieColumn(bool value);
	void setRigidDiaphragm(bool value);
	void setBuildingInformationModel(QObject *value);
	void setDemandPGA(QObject *value);
	void setDesignBaseShearCoefficient(QObject *value);
	void setTheta1(QObject *value);
	void setTheta2(QObject *value);
	void setTheta3(QObject *value);
	void setTheta4(QObject *value);
	void setTheta5(QObject *value);
	void setTheta6(QObject *value);
	void setEpsilon(QObject *value);
	void setSigma(QObject *value);

private:
	bool theWithTieColumn;
	bool theRigidDiaphragm;
	RPointer<RModel> theBuildingInformationModel;
	RPointer<RParameter> theDemandPGA;
	RPointer<RParameter> theDesignBaseShearCoefficient;
	RPointer<RParameter> theTheta1;
	RPointer<RParameter> theTheta2;
	RPointer<RParameter> theTheta3;
	RPointer<RParameter> theTheta4;
	RPointer<RParameter> theTheta5;
	RPointer<RParameter> theTheta6;
	RPointer<RParameter> theEpsilon;
	RPointer<RParameter> theSigma;

	RResponse *theMasonryDamageResponse;
};
#endif //RMasonryBuildingDamageModel_H