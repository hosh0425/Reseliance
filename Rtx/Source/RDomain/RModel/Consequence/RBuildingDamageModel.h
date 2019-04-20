#ifndef RBuildingDamageModel_H
#define RBuildingDamageModel_H

#include "RModel.h"

class RParameter;
class RResponse;

class RBuildingDamageModel : public RModel
{
	Q_OBJECT	
		
	Q_PROPERTY(QObject *BuildingInformationModel READ getBuildingInformationModel WRITE setBuildingInformationModel)
	Q_PROPERTY(QObject *PeakDriftRatio READ getPeakDriftRatio WRITE setPeakDriftRatio)
	Q_PROPERTY(QObject *PeakAcceleration READ getPeakAcceleration WRITE setPeakAcceleration)

	Q_PROPERTY(QObject *Theta1 READ getTheta1 WRITE setTheta1)
	Q_PROPERTY(QObject *Theta2 READ getTheta2 WRITE setTheta2)
	Q_PROPERTY(QObject *Theta3 READ getTheta3 WRITE setTheta3)
	Q_PROPERTY(QObject *Theta4 READ getTheta4 WRITE setTheta4)
	Q_PROPERTY(QObject *Theta5 READ getTheta5 WRITE setTheta5)
	Q_PROPERTY(QObject *Theta6 READ getTheta6 WRITE setTheta6)
	Q_PROPERTY(QObject *Theta7 READ getTheta7 WRITE setTheta7)
	Q_PROPERTY(QObject *Theta8 READ getTheta8 WRITE setTheta8)
	Q_PROPERTY(QObject *Theta9 READ getTheta9 WRITE setTheta9)
	Q_PROPERTY(QObject *Theta10 READ getTheta10 WRITE setTheta10)
	Q_PROPERTY(QObject *Theta11 READ getTheta11 WRITE setTheta11)
	Q_PROPERTY(QObject *Theta12 READ getTheta12 WRITE setTheta12)
	Q_PROPERTY(QObject *Theta13 READ getTheta13 WRITE setTheta13)
	Q_PROPERTY(QObject *Theta14 READ getTheta14 WRITE setTheta14)
	Q_PROPERTY(QObject *Theta15 READ getTheta15 WRITE setTheta15)

	Q_PROPERTY(QObject *Sigma1 READ getSigma1 WRITE setSigma1)
	Q_PROPERTY(QObject *Sigma2 READ getSigma2 WRITE setSigma2)
	Q_PROPERTY(QObject *Sigma3 READ getSigma3 WRITE setSigma3)
	Q_PROPERTY(QObject *Sigma4 READ getSigma4 WRITE setSigma4)
	Q_PROPERTY(QObject *Epsilon1 READ getEpsilon1 WRITE setEpsilon1)
	Q_PROPERTY(QObject *Epsilon2 READ getEpsilon2 WRITE setEpsilon2)
	Q_PROPERTY(QObject *Epsilon3 READ getEpsilon3 WRITE setEpsilon3)
	Q_PROPERTY(QObject *Epsilon4 READ getEpsilon4 WRITE setEpsilon4)

public:
	RBuildingDamageModel(QObject *parent, QString name);
	~RBuildingDamageModel();

	QObject *getBuildingInformationModel() const;
	void setBuildingInformationModel(QObject *value);
	QObject *getPeakDriftRatio() const;
	void setPeakDriftRatio(QObject *value);
	QObject *getPeakAcceleration() const;
	void setPeakAcceleration(QObject *value);

	QObject *getTheta1() const;
	void setTheta1(QObject *value);
	QObject *getTheta2() const;
	void setTheta2(QObject *value);
	QObject *getTheta3() const;
	void setTheta3(QObject *value);
	QObject *getTheta4() const;
	void setTheta4(QObject *value);
	QObject *getTheta5() const;
	void setTheta5(QObject *value);
	QObject *getTheta6() const;
	void setTheta6(QObject *value);
	QObject *getTheta7() const;
	void setTheta7(QObject *value);
	QObject *getTheta8() const;
	void setTheta8(QObject *value);
	QObject *getTheta9() const;
	void setTheta9(QObject *value);
	QObject *getTheta10() const;
	void setTheta10(QObject *value);
	QObject *getTheta11() const;
	void setTheta11(QObject *value);
	QObject *getTheta12() const;
	void setTheta12(QObject *value);
	QObject *getTheta13() const;
	void setTheta13(QObject *value);
	QObject *getTheta14() const;
	void setTheta14(QObject *value);
	QObject *getTheta15() const;
	void setTheta15(QObject *value);
	QObject *getSigma1() const;
	void setSigma1(QObject *value);
	QObject *getSigma2() const;
	void setSigma2(QObject *value);
	QObject *getSigma3() const;
	void setSigma3(QObject *value);
	QObject *getSigma4() const;
	void setSigma4(QObject *value);
	QObject *getEpsilon1() const;
	void setEpsilon1(QObject *value);
	QObject *getEpsilon2() const;
	void setEpsilon2(QObject *value);
	QObject *getEpsilon3() const;
	void setEpsilon3(QObject *value);
	QObject *getEpsilon4() const;
	void setEpsilon4(QObject *value);

	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID);
	void updateHistoryVariables(int threadID);
	void resetHistoryVariables(int threadID);

	// returns true as this model is parallelized
	bool canBeRunParallelly();

private:
	RPointer<RModel> theBuildingInformationModel;
	RPointer<RParameter> thePeakDriftRatio;
	RPointer<RParameter> thePeakAcceleration;

	RPointer<RParameter> theTheta1;
	RPointer<RParameter> theTheta2;
	RPointer<RParameter> theTheta3;
	RPointer<RParameter> theTheta4;
	RPointer<RParameter> theTheta5;
	RPointer<RParameter> theTheta6;
	RPointer<RParameter> theTheta7;
	RPointer<RParameter> theTheta8;
	RPointer<RParameter> theTheta9;
	RPointer<RParameter> theTheta10;
	RPointer<RParameter> theTheta11;
	RPointer<RParameter> theTheta12;
	RPointer<RParameter> theTheta13;
	RPointer<RParameter> theTheta14;
	RPointer<RParameter> theTheta15;
	RPointer<RParameter> theSigma1;
	RPointer<RParameter> theSigma2;
	RPointer<RParameter> theSigma3;
	RPointer<RParameter> theSigma4;
	RPointer<RParameter> theEpsilon1;
	RPointer<RParameter> theEpsilon2;
	RPointer<RParameter> theEpsilon3;
	RPointer<RParameter> theEpsilon4;

	RResponse *theStructuralDamageRatio;
	RResponse *theNonstructuralDriftDamageRatio;
	RResponse *theNonstructuralAccelerationDamageRatio;
	RResponse *theContentDamageRatio;

};

#endif // RBuildingDamageModel_H
