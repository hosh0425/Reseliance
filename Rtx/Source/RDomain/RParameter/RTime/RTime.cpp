#include "RTime.h"

RTime::RTime(QObject *parent, QString name)
	: RParameter(parent, name)
{

	// defining array data fields
	int arraySizeForParallelizedObject = getArraySizeForParallelizedObject();
	theCurrentTime = new QDateTime[arraySizeForParallelizedObject];

	format = RTime::getTimeFormat();
	theCurrentTime[0] = QDateTime::currentDateTime();
	constTime = QDateTime(QDate(1900, 1, 1), QTime(0, 0, 0, 0));
	theReferenceTime = QDateTime(constTime);
	updateCurrentValue(0);
}

RTime::~RTime()
{
	delete [] theCurrentTime;
}

QString RTime::getCurrentTime() const	
{	
	return theCurrentTime[0].toString(format);
}	
	
void RTime::setCurrentTime(QString value)	
{	
	theCurrentTime[0] = QDateTime().fromString(value, format);
	updateCurrentValue(0);
}	
	
QDateTime RTime::getReferenceTime() const	
{	
	return theReferenceTime;
}	
	
void RTime::setReferenceTime(QDateTime value)	
{	
	theReferenceTime = value;
	updateCurrentValue(0);
}	

void RTime::setCurrentValue(double value)
{
	setCurrentValue(value, 0);
}

void RTime::setCurrentValue(double value, int threadID)
{
	RParameter::setCurrentValue(value, threadID);
	updateCurrentTime(threadID);
}

void RTime::updateCurrentTime(int threadID)
{
	//if (theUnit == RTime::Year) {
	//	theCurrentTime = theReferenceTime.addYears(theCurrentValue);
	//} else if (theUnit == RTime::Day) {
	//	theCurrentTime = theReferenceTime.addDays(theCurrentValue);
	//} else if (theUnit == RTime::Second) {		
	//	theCurrentTime = theReferenceTime.addSecs(theCurrentValue);
	//}
	theCurrentTime[threadID] = theReferenceTime.addDays(theCurrentValue[threadID]);
	theCurrentTime[threadID] = theCurrentTime[threadID].addMSecs((theCurrentValue[threadID] - int(theCurrentValue[threadID])) * 86400000.0);
}

void RTime::updateCurrentValue(int threadID)
{
	double temp = 0;
	//if (theUnit == RTime::Year) {
	//	temp = theCurrentTime.date().year() - theReferenceTime.date().year();
	//	temp += (theCurrentTime.date().day() - theReferenceTime.date().day()) / 365.0;
	//	temp += (theCurrentTime.time().day() - theReferenceTime.date().day()) / 365.0;
	//} else
	//if (theUnit == RTime::Day) {
	//	temp = theReferenceTime.daysTo(theCurrentTime);
	//	temp += theReferenceTime.time().msecsTo(theCurrentTime.time()) / 86400000.0;
	//} else if (theUnit == RTime::Second) {		
	//	temp = theReferenceTime.secsTo(theCurrentTime);
	//	temp += theReferenceTime.time().secsTo(theCurrentTime.time()) / 86400.0;
	//}

	
	temp = theReferenceTime.daysTo(theCurrentTime[threadID]);
	QString hh = theCurrentTime[threadID].toString(format);
	temp += theReferenceTime.time().msecsTo(theCurrentTime[threadID].time()) / 86400000.0;
	RParameter::setCurrentValue(temp, threadID);
}

int RTime::getCurrentYear(int threadID) const
{
	return theCurrentTime[threadID].date().year();
}

int RTime::getCurrentMonth(int threadID) const
{
	return theCurrentTime[threadID].date().month();
}

int RTime::getCurrentDay(int threadID) const
{
	return theCurrentTime[threadID].date().day();
}

int RTime::getCurrentDayOfWeek(int threadID) const
{
	return theCurrentTime[threadID].date().dayOfWeek();
}

int RTime::getCurrentHour(int threadID) const
{
	return theCurrentTime[threadID].time().hour();
}

int RTime::getCurrentMinute(int threadID) const
{
	return theCurrentTime[threadID].time().minute();
}

QString RTime::getTimeFormat()
{
	return QString("yyyy-MM-dd hh:mm:ss.zzz");
}

void RTime::initializeForParallelComputation(int threadID){

	// call the method in superclass
	RParameter::initializeForParallelComputation(threadID);

	setCurrentValue(theCurrentValue[0],threadID);
}

bool RTime::canBeRunParallelly(){
	return true;
}