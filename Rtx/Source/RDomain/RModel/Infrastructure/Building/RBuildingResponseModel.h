#ifndef RBuildingResponseModel_H
#define RBuildingResponseModel_H

#include "RModel.h"

class RParameter;
class RResponse;
class QMutex;

class RBuildingResponseModel : public RModel
{
	Q_OBJECT	
		
	Q_PROPERTY(QObject *BuildingInformationModel READ getBuildingInformationModel WRITE setBuildingInformationModel)
	Q_PROPERTY(QObject *PeriodInformation READ getPeriodInformation WRITE setPeriodInformation)
	Q_PROPERTY(QObject *Sa READ getSa WRITE setSa)

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
	Q_PROPERTY(QObject *Theta16 READ getTheta16 WRITE setTheta16)
	Q_PROPERTY(QObject *Theta17 READ getTheta17 WRITE setTheta17)
	Q_PROPERTY(QObject *Theta18 READ getTheta18 WRITE setTheta18)
	Q_PROPERTY(QObject *Theta19 READ getTheta19 WRITE setTheta19)
	Q_PROPERTY(QObject *Theta20 READ getTheta20 WRITE setTheta20)
	Q_PROPERTY(QObject *Theta21 READ getTheta21 WRITE setTheta21)

	Q_PROPERTY(QObject *Sigma1 READ getSigma1 WRITE setSigma1)
	Q_PROPERTY(QObject *Sigma2 READ getSigma2 WRITE setSigma2)
	Q_PROPERTY(QObject *Epsilon1 READ getEpsilon1 WRITE setEpsilon1)
	Q_PROPERTY(QObject *Epsilon2 READ getEpsilon2 WRITE setEpsilon2)

public:
	RBuildingResponseModel(QObject *parent, QString name);
	~RBuildingResponseModel();

	QObject *getBuildingInformationModel() const;
	void setBuildingInformationModel(QObject *value);
	QObject *getPeriodInformation() const;
	void setPeriodInformation(QObject *value);
	QObject *getSa() const;
	void setSa(QObject *value);

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
	QObject *getTheta16() const;
	void setTheta16(QObject *value);
	QObject *getTheta17() const;
	void setTheta17(QObject *value);
	QObject *getTheta18() const;
	void setTheta18(QObject *value);
	QObject *getTheta19() const;
	void setTheta19(QObject *value);
	QObject *getTheta20() const;
	void setTheta20(QObject *value);

	QObject *getTheta21() const;
	void setTheta21(QObject *value);
	QObject *getSigma1() const;
	void setSigma1(QObject *value);
	QObject *getSigma2() const;
	void setSigma2(QObject *value);
	QObject *getEpsilon1() const;
	void setEpsilon1(QObject *value);
	QObject *getEpsilon2() const;
	void setEpsilon2(QObject *value);

	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID);
	int setResponsesThatFlowUpstream(int threadID);

	void reset();

	// returns true as this model is parallelized
	bool canBeRunParallelly();

private:
	RPointer<RModel> theBuildingInformationModel;
	RPointer<RParameter> thePeriodInformation;
	RPointer<RParameter> theSa;
	
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
	RPointer<RParameter> theTheta16;
	RPointer<RParameter> theTheta17;
	RPointer<RParameter> theTheta18;
	RPointer<RParameter> theTheta19;
	RPointer<RParameter> theTheta20;

	RPointer<RParameter> theTheta21;
	RPointer<RParameter> theSigma1;
	RPointer<RParameter> theSigma2;
	RPointer<RParameter> theEpsilon1;
	RPointer<RParameter> theEpsilon2;

	RResponse *thePeakDrift;
	RResponse *thePeakAcc;

	bool upstreamParametersListHasBeenSet;
	QMutex* upstreamParametersListLock;

};

#endif // RBuildingResponseModel_H
