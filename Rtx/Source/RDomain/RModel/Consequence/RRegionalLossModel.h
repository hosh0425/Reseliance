#ifndef RRegionalLossModel_H
#define RRegionalLossModel_H

#include <QColor>

#include "RModel.h"

class RParameter;
class RResponse;
class RLocation;

class RRegionalLossModel : public RModel
{
	Q_OBJECT

	Q_PROPERTY(QString BoundaryLocationList READ getBoundaryLocationList WRITE setBoundaryLocationList)
	Q_PROPERTY(QObject *CentroidLocation READ getCentroidLocation WRITE setCentroidLocation)
	Q_PROPERTY(QObject *Sa1 READ getSa1 WRITE setSa1)
	Q_PROPERTY(QObject *Sa2 READ getSa2 WRITE setSa2)
	Q_PROPERTY(QString UnitAreaCostList READ getUnitAreaCostList WRITE setUnitAreaCostList)
	Q_PROPERTY(QString AreaRatioList READ getAreaRatioList WRITE setAreaRatioList)
	Q_PROPERTY(QObject *Time READ getTime WRITE setTime)
	Q_PROPERTY(QObject *AnnualInterestRate READ getAnnualInterestRate WRITE setAnnualInterestRate)
	Q_PROPERTY(QString Theta1List READ getTheta1List WRITE setTheta1List)
	Q_PROPERTY(QString Theta2List READ getTheta2List WRITE setTheta2List)
	Q_PROPERTY(QString Theta3List READ getTheta3List WRITE setTheta3List)
	Q_PROPERTY(QString Theta4List READ getTheta4List WRITE setTheta4List)
	Q_PROPERTY(QString Theta5List READ getTheta5List WRITE setTheta5List)
	Q_PROPERTY(QString EpsilonList READ getEpsilonList WRITE setEpsilonList)
	Q_PROPERTY(QColor Color READ getColor WRITE setColor)
	Q_PROPERTY(double TotalArea READ getTotalArea)

public:
	RRegionalLossModel(QObject *parent, QString name);
	~RRegionalLossModel();

	QString getBoundaryLocationList();
	void setBoundaryLocationList(QString value);
	QObject *getCentroidLocation() const;
	void setCentroidLocation(QObject *value);
	QObject *getSa1() const;
	void setSa1(QObject *value);
	QObject *getSa2() const;
	void setSa2(QObject *value);
	QString getUnitAreaCostList();
	void setUnitAreaCostList(QString value);
	QString getAreaRatioList();
	void setAreaRatioList(QString value);
	QObject *getTime() const;
	void setTime(QObject *value);
	QObject *getAnnualInterestRate() const;
	void setAnnualInterestRate(QObject *value);
	QString getTheta1List();
	void setTheta1List(QString value);
	QString getTheta2List();
	void setTheta2List(QString value);
	QString getTheta3List();
	void setTheta3List(QString value);
	QString getTheta4List();
	void setTheta4List(QString value);
	QString getTheta5List();
	void setTheta5List(QString value);
	QString getEpsilonList();
	void setEpsilonList(QString value);
	QColor getColor() const;
	void setColor(QColor value);
	double getTotalArea();

	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID);

	// A method to determine if a location falls within this sector, in the given threadID
	bool hasInside(RLocation *passedLocation, int threadID);

	QStringList getActionList();
	void reset();

	// returns true as this model is parallelized
	bool canBeRunParallelly();

public slots:
	void displayonMap();

private slots:
	void removefromMap();
	void displayLocationsInside();

private:

	void calculateArea(int threadID);
	void calculateCentroid(int threadID);

	QList<RPointer<RParameter> > theBoundaryLocationList;
	RPointer<RParameter> theCentroidLocation;
	RPointer<RParameter> theSa1;
	RPointer<RParameter> theSa2;
	QList<RPointer<RParameter> > theUnitAreaCostList;
	QList<RPointer<RParameter> > theAreaRatioList;
	RPointer<RParameter> theTime;
	RPointer<RParameter> theAnnualInterestRate;
	QList<RPointer<RParameter> > theTheta1List;
	QList<RPointer<RParameter> > theTheta2List;
	QList<RPointer<RParameter> > theTheta3List;
	QList<RPointer<RParameter> > theTheta4List;
	QList<RPointer<RParameter> > theTheta5List;
	QList<RPointer<RParameter> > theEpsilonList;
	QColor theColor;
	double theTotalArea;

	double theInitialTime;

	RResponse *theLoss;

};

#endif // RRegionalLossModel_H
