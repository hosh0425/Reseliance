#include "RRegionalLossModel.h"

#include "RParameter.h"
#include "RResponse.h"
#include "RLocation.h"
#include "RMainWindow.h"
#include "RMapWidget.h"

#include <gsl/gsl_randist.h>
#include <gsl/gsl_cdf.h>
#include <gsl/gsl_math.h>

const double sphereRadius = 4.0 * M_PI * gsl_pow_2(EarthRadius * 1000.0);

RRegionalLossModel::RRegionalLossModel(QObject *parent, QString name)
	: RModel(parent, name)
{
	/* //$$ 
	theCentroidLocation = 0;
	theSa1 = 0;
	theSa2 = 0;
	theTime = 0;
	theAnnualInterestRate = 0;
	*/
	theColor = Qt::blue;
	theTotalArea = 0.0;
	
	theDomain->createObject(objectName() + "Response", "RGenericResponse");
	theLoss = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theLoss->setModel(this);
}

RRegionalLossModel::~RRegionalLossModel()
{

}

QString RRegionalLossModel::getBoundaryLocationList()	
{	
	return parameterListToString(theBoundaryLocationList);
}	
	
void RRegionalLossModel::setBoundaryLocationList(QString value)	
{	
	theBoundaryLocationList = stringToParameterList(value);
	
	if (theBoundaryLocationList.count() < 3) {
		theTotalArea = 0.0;
		rCritical() << "Warning: The number of boundary locations in the model" << objectName() << "is less than 3.";
	}
	
	// Checking if all the passed parameters are either RLocation or RLocationResponse
	parameterListToLocationList(theBoundaryLocationList, true);
}	

QObject * RRegionalLossModel::getCentroidLocation() const	
{	
	return theCentroidLocation.data();
}	
	
void RRegionalLossModel::setCentroidLocation(QObject *value)	
{	
	theCentroidLocation = qobject_cast<RParameter *>(value);

	// Checking the parameter for being a location or location response object
	parameterToLocation(theCentroidLocation.data());
}	

QObject * RRegionalLossModel::getSa1() const	
{	
	return theSa1.data();
}	
	
void RRegionalLossModel::setSa1(QObject *value)	
{	
	theSa1 = qobject_cast<RParameter *>(value);
}	
	
QObject * RRegionalLossModel::getSa2() const	
{	
	return theSa2.data();
}	
	
void RRegionalLossModel::setSa2(QObject *value)	
{	
	theSa2 = qobject_cast<RParameter *>(value);
}	

QString RRegionalLossModel::getUnitAreaCostList()	
{	
	return parameterListToString(theUnitAreaCostList);
}	
	
void RRegionalLossModel::setUnitAreaCostList(QString value)	
{	
	theUnitAreaCostList = stringToParameterList(value);
}	
	
QString RRegionalLossModel::getAreaRatioList()	
{	
	return parameterListToString(theAreaRatioList);
}	
	
void RRegionalLossModel::setAreaRatioList(QString value)	
{	
	theAreaRatioList = stringToParameterList(value);
}	

QObject * RRegionalLossModel::getTime() const	
{	
	return theTime.data();
}	
	
void RRegionalLossModel::setTime(QObject *value)	
{	
	theTime = qobject_cast<RParameter *>(value);
}	

QObject * RRegionalLossModel::getAnnualInterestRate() const	
{	
	return theAnnualInterestRate.data();
}	
	
void RRegionalLossModel::setAnnualInterestRate(QObject *value)	
{	
	theAnnualInterestRate = qobject_cast<RParameter *>(value);
}	
	
QString RRegionalLossModel::getTheta1List()	
{	
	return parameterListToString(theTheta1List);
}	
	
void RRegionalLossModel::setTheta1List(QString value)	
{	
	theTheta1List = stringToParameterList(value);
}	
	
QString RRegionalLossModel::getTheta2List()	
{	
	return parameterListToString(theTheta2List);
}	
	
void RRegionalLossModel::setTheta2List(QString value)	
{	
	theTheta2List = stringToParameterList(value);
}	
	
QString RRegionalLossModel::getTheta3List()	
{	
	return parameterListToString(theTheta3List);
}	
	
void RRegionalLossModel::setTheta3List(QString value)	
{	
	theTheta3List = stringToParameterList(value);
}	
	
QString RRegionalLossModel::getTheta4List()	
{	
	return parameterListToString(theTheta4List);
}	
	
void RRegionalLossModel::setTheta4List(QString value)	
{	
	theTheta4List = stringToParameterList(value);
}	
		
QString RRegionalLossModel::getTheta5List()	
{	
	return parameterListToString(theTheta5List);
}	
	
void RRegionalLossModel::setTheta5List(QString value)	
{	
	theTheta5List = stringToParameterList(value);
}	
	
QString RRegionalLossModel::getEpsilonList()	
{	
	return parameterListToString(theEpsilonList);
}	
	
void RRegionalLossModel::setEpsilonList(QString value)	
{	
	theEpsilonList = stringToParameterList(value);
}	

QColor RRegionalLossModel::getColor() const	
{	
	return theColor;
}	
	
void RRegionalLossModel::setColor(QColor value)	
{	
	theColor = value;
}	

double RRegionalLossModel::getTotalArea()	
{	
	if (theBoundaryLocationList.count() < 3) {
		theTotalArea = 0.0;
	} else {
		calculateArea(0);
		calculateCentroid(0);
	}
	return theTotalArea;
}	

int RRegionalLossModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
{	
	if (theTheta1List.count() != theTheta2List.count() || theTheta2List.count() != theTheta3List.count() || theTheta3List.count() != theTheta4List.count() || theTheta4List.count() != theTheta5List.count() || theTheta5List.count() != theEpsilonList.count() || theEpsilonList.count() != theUnitAreaCostList.count() || theUnitAreaCostList.count() != theAreaRatioList.count()) {
		rCritical(threadID) << "Error: The number of input parameters in the Theta 1 List, Theta 2 List, Theta 3 List, Theta 4 List, Theta 5 List, Epsilon List, Unit Area Cost List, and Area Ratio List in the object" << objectName() << "are not the same.";
		return -1;
	}
	if (theTheta1List.isEmpty()) {
		rCritical(threadID) << "Error: The property Theta 1 List in the object" << objectName() << "is empty.";
		return -1;
	}
	if (theTheta2List.isEmpty()) {
		rCritical(threadID) << "Error: The property Theta 2 List in the object" << objectName() << "is empty.";
		return -1;
	}
	if (theTheta3List.isEmpty()) {
		rCritical(threadID) << "Error: The property Theta 3 List in the object" << objectName() << "is empty.";
		return -1;
	}
	if (theTheta4List.isEmpty()) {
		rCritical(threadID) << "Error: The property Theta 4 List in the object" << objectName() << "is empty.";
		return -1;
	}
	if (theTheta5List.isEmpty()) {
		rCritical(threadID) << "Error: The property Theta 4 List in the object" << objectName() << "is empty.";
		return -1;
	}
	if (theEpsilonList.isEmpty()) {
		rCritical(threadID) << "Error: The property Epsilon List in the object" << objectName() << "is empty.";
		return -1;
	}
	if (theUnitAreaCostList.isEmpty()) {
		rCritical(threadID) << "Error: The property Unit Area Cost List in the object" << objectName() << "is empty.";
		return -1;
	}
	if (theUnitAreaCostList.isEmpty()) {
		rCritical(threadID) << "Error: The property Unit Area Cost List in the object" << objectName() << "is empty.";
		return -1;
	}

	if (theBoundaryLocationList.count() < 3) {
		theTotalArea = 0.0;
		rCritical(threadID) << "Warning: The number of boundary locations in the model" << objectName() << "is less than 3.";
		return -1;
	}

	double Sa1 = theSa1.data()->getCurrentValue(threadID);
	double Sa0_3 = theSa2.data()->getCurrentValue(threadID);
	double currentTime = theTime.data()->getCurrentValue(threadID);
	double interestRate = theAnnualInterestRate.data()->getCurrentValue(threadID);
	
	double totalLoss = 0.0;
	for (int i = 0; i < theUnitAreaCostList.count(); i++) {
		double theta1 = theTheta1List[i].data()->getCurrentValue(threadID);
		double theta2 = theTheta2List[i].data()->getCurrentValue(threadID);
		double theta3 = theTheta3List[i].data()->getCurrentValue(threadID);
		double theta4 = theTheta4List[i].data()->getCurrentValue(threadID);
		double theta5 = theTheta5List[i].data()->getCurrentValue(threadID);
		double epsilon = theEpsilonList[i].data()->getCurrentValue(threadID);
		double C = theUnitAreaCostList[i].data()->getCurrentValue(threadID);
		double ARatio = theAreaRatioList[i].data()->getCurrentValue(threadID);

		/*//TEMP////////////////////////////////////////////////////////////////
		double logSa0_3 = log(Sa0_3);
		double logSa1 = log(Sa1);
		double phiInside = theta2 * log(Sa0_3) + theta3 * log(Sa1) + theta4;
		double phi = gsl_cdf_gaussian_P(theta2 * log(Sa0_3) + theta3 * log(Sa1) + theta4, 1.0);
		double allBuEpsilon = theta1 * C * gsl_cdf_gaussian_P(theta2 * log(Sa0_3) + theta3 * log(Sa1) + theta4, 1.0);
		double l_ARatio = theta1 * C * gsl_cdf_gaussian_P(theta2 * log(Sa0_3) + theta3 * log(Sa1) + theta4, 1.0) + epsilon;*/
		
		double l = theta1 * gsl_cdf_gaussian_P(theta2 * pow(Sa0_3, theta5) + theta3 * pow(Sa1, theta5) + theta4, 1.0) * exp(epsilon);
		double loss = l * C * ARatio;
		totalLoss += loss;
	}

	double futureDiscountFactor = 1.0;
	if (currentTime > theInitialTime) {
		//futureDiscountFactor = pow(1 + interestRate, -(currentTime - theInitialTime) / 365.0);
		futureDiscountFactor = exp(-interestRate * (currentTime - theInitialTime) / 365.0);
	}

	totalLoss *= theTotalArea * futureDiscountFactor;

	theLoss->setCurrentValue(totalLoss,threadID);

	if (evaluateDDM) {
		// IMPLEMENT
	}

	/*int rr,gg,bb,aa;
	theColor.getRgb(&rr,&gg,&bb,&aa);
	rDebug() << objectName() << totalLoss << theTotalArea << rr << gg << bb << theColor.name();*/

	return 0;
}

void RRegionalLossModel::reset()
{
	calculateArea(0);
	calculateCentroid(0);
	theInitialTime = theTime.data()->getCurrentValue();
}

void RRegionalLossModel::calculateArea(int threadID)
{
	QList<RLocation *> locationList = parameterListToLocationList(theBoundaryLocationList);

	double angleSum = 0.0;
	int n = locationList.count();

	for (int i = 0; i < n; i++) {
		// j is the next point
		int j = i + 1;
		if (j == n) {
			j = 0;
		}
		
		// k is the previous point
		int k = i - 1;
		if (k == -1) {
			k = n - 1;
		}

		double bearing1 = locationList[i]->computeBearing(locationList[j],threadID);	
		double bearing2 = locationList[i]->computeBearing(locationList[k],threadID);
		double angle = bearing2 - bearing1;
		if (angle < 0) {
			angle += 2 * M_PI;
		}
		angleSum += angle;
	}

	theTotalArea = (angleSum - (n - 2) * M_PI) * gsl_pow_2(EarthRadius * 1000.0);

	if (theTotalArea > sphereRadius / 2.0) {
		theTotalArea = sphereRadius - theTotalArea;
	}

	//// Calculate the area using Google Maps for comparison (the map should be open)
	//RMainWindow *theMainWindow = qobject_cast<RMainWindow *>(theDomain->parent());
	//RMapWidget *theMapWidget = theMainWindow->findChild<RMapWidget *>();
	//double aa;
	//if (theMapWidget) {
	//	theTotalArea = theMapWidget->getArea(0);
	//}
}

void RRegionalLossModel::calculateCentroid(int threadID)
{
	// ATTENTION: THESE CALCULATIONS ARE FOR A PLANAR POLYGON, ALSO ITY DOESN'T WORK FOR A POLYGON CROSSING THE DATELINE

	RLocation *location = parameterToLocation(theCentroidLocation.data());	
	if (!location) {
		return;
	}

	QList<RLocation *> locationList = parameterListToLocationList(theBoundaryLocationList);
	if (locationList.count() < 3) {
		return;
	}

	locationList << locationList[0];
	double A = 0.0;
	double Cx = 0.0;
	double Cy = 0.0;

	for (int i = 0; i < locationList.count() - 1; i++) {
		double xi = locationList[i]->getLongitude(threadID);
		double xi_1 = locationList[i+1]->getLongitude(threadID);
		double yi = locationList[i]->getLatitude(threadID);
		double yi_1 = locationList[i+1]->getLatitude(threadID);

		Cx += (xi + xi_1) * (xi * yi_1 - xi_1 * yi);
		Cy += (yi + yi_1) * (xi * yi_1 - xi_1 * yi);

		A += xi * yi_1 - xi_1 * yi;
	}

	A *= 0.5;
	Cx *= 1.0 / 6.0 / A;
	Cy *= 1.0 / 6.0 / A;

	location->setLatitude(Cy,threadID);
	location->setLongitude(Cx,threadID);

	//A = qAbs(A);
}

bool RRegionalLossModel::hasInside(RLocation *passedLocation, int threadID)
{
	QList<RLocation *> locationList = parameterListToLocationList(theBoundaryLocationList);

	// This code adopted from the following URL: http://msdn.microsoft.com/en-us/library/cc451895.aspx
	int i;
	int j = locationList.count() - 1;
	bool isInside = false;
	double lat = passedLocation->getLatitude(threadID);
	double lng = passedLocation->getLongitude(threadID);
	for (i = 0; i < locationList.count(); i++) {
		if (locationList[i]->getLongitude(threadID) < lng && locationList[j]->getLongitude(threadID) >= lng ||  locationList[j]->getLongitude(threadID) < lng && locationList[i]->getLongitude(threadID) >= lng) {
			if (locationList[i]->getLatitude(threadID) + (lng - locationList[i]->getLongitude(threadID)) / (locationList[j]->getLongitude(threadID) - locationList[i]->getLongitude(threadID)) * (locationList[j]->getLatitude(threadID) - locationList[i]->getLatitude(threadID)) < lat) {
				isInside = !isInside; 
			}
		}
		j = i; 
	}
	return isInside; 
}

QStringList RRegionalLossModel::getActionList()
{
	QStringList actionList;
	actionList << "&Display on Map" << "Remove from Map" << "&Display &Locations Inside";
	return actionList;
}

void RRegionalLossModel::displayonMap()
{

	QList<RLocation *> locationList = parameterListToLocationList(theBoundaryLocationList);
	if (locationList.count() < 3) {
		rCritical() << "Warning: Please enter at least 3 location objects in the model" << objectName() << ".";
		return;
	}
	RMainWindow *theMainWindow = qobject_cast<RMainWindow *>(theDomain->parent());
	RMapWidget *theMapWidget = theMainWindow->findChild<RMapWidget *>();
	theMainWindow->setActiveSubWindow(theMapWidget);
	if (!theMapWidget) {
		locationList[0]->openMap();
		return;
	}
	theMapWidget->drawPolygon(this, parameterListToLocationList(theBoundaryLocationList), theColor, 0.6);

	calculateCentroid(0);

	RLocation *location = parameterToLocation(theCentroidLocation.data());
	if (location) {
		location->displayonMap();
	}


#ifndef WEB_PROTOTYPE
	QList<RLocation *> locationList = parameterListToLocationList(theBoundaryLocationList);
	if (locationList.count() < 3) {
		rCritical() << "Warning: Please enter at least 3 location objects in the model" << objectName() << ".";
		return;
	}
	RMainWindow *theMainWindow = qobject_cast<RMainWindow *>(theDomain->parent());
	RMapWidget *theMapWidget = theMainWindow->findChild<RMapWidget *>();
	theMainWindow->setActiveSubWindow(theMapWidget);
	if (!theMapWidget) {
		locationList[0]->openMap();
		return;
	}
	theMapWidget->drawPolygon(this, parameterListToLocationList(theBoundaryLocationList), theColor, 0.6);

	calculateCentroid(0);
	
	RLocation *location = parameterToLocation(theCentroidLocation.data());	
	if (location) {
		location->displayonMap();
	}
#endif // WEB_PROTOTYPE
}

void RRegionalLossModel::removefromMap()
{
	RMainWindow *theMainWindow = qobject_cast<RMainWindow *>(theDomain->parent());
	RMapWidget *theMapWidget = theMainWindow->findChild<RMapWidget *>();
	if (theMapWidget) {
		theMainWindow->setActiveSubWindow(theMapWidget);
		theMapWidget->removePolygon(this);
	
		RLocation *location = parameterToLocation(theCentroidLocation.data());	
		if (location) {
			location->removefromMap();
		}
	}
}

void RRegionalLossModel::displayLocationsInside()
{
	QList<RLocation *> locationList = theDomain->findChildren<RLocation *>();
	if (locationList.isEmpty()) {
		return;
	}

	QList<RLocation *> boundaryList = parameterListToLocationList(theBoundaryLocationList);
	if (boundaryList.isEmpty()) {
		return;
	}
	
	for (int i = 0; i < boundaryList.count(); i++) {
		locationList.removeOne(boundaryList[i]);
	}

	QList<RLocation *> insideList;
	for (int i = 0; i < locationList.count(); i++) {
		if (hasInside(locationList[i],0)) {
			insideList << locationList[i];
		}
		// A line to keep the GUI responsive
		if (i % 10 == 0) {
			QCoreApplication::processEvents();
		}
	}

	if (insideList.isEmpty()) {
		rDebug() << "No locations found inside the sector" << objectName() << ".";
	} else {
		rDebug() << "The locaions inside the sector" << objectName() << ":";
	}

	RMainWindow *theMainWindow = qobject_cast<RMainWindow *>(theDomain->parent());
	RMapWidget *theMapWidget = theMainWindow->findChild<RMapWidget *>();
	theMainWindow->setActiveSubWindow(theMapWidget);

	for (int i = 0; i < insideList.count(); i++) {
		rDebug() << qPrintable(insideList[i]->objectName());

		if (theMapWidget) {
			insideList[i]->displayonMap();
		}

		// A line to keep the GUI responsive
		if (i % 10 == 0) {
			QCoreApplication::processEvents();
		}
	}

	rDebug() << " ";
}

bool RRegionalLossModel::canBeRunParallelly(){
	return true;
}