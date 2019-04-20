#include "RPopulationDensityModel.h"	
	
#include "RParameter.h"	
#include "RResponse.h"	
#include "RTime.h"

#include <gsl/gsl_math.h>
	
#include <math.h>	
	
RPopulationDensityModel::RPopulationDensityModel(QObject *parent, QString name)	
	: RModel(parent, name)
{	
	// Initializing the data members
	//theTime = 0;
	//theMaximumPopulationDensity = 0;
	
	
	
	// Instantiating the response object
	theDomain->createObject(objectName() + "Response", "RGenericResponse");
	thePopulationDensity = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	thePopulationDensity->setModel(this);
}	
RPopulationDensityModel::~RPopulationDensityModel()	
{	

}	

QObject * RPopulationDensityModel::getTime() const	
{
	return theTime.data();
}	
		
void RPopulationDensityModel::setTime(QObject *value)	
{	
	theTime = qobject_cast<RParameter *>(value);
}

QObject * RPopulationDensityModel::getMaximumPopulationDensity() const	
{	
	return theMaximumPopulationDensity.data();
}	
		
void RPopulationDensityModel::setMaximumPopulationDensity(QObject *value)	
{	
	theMaximumPopulationDensity = qobject_cast<RParameter *>(value);
}

QString RPopulationDensityModel::getPopulationRatiosPerMonth()	
{	
	QString result = doubleVectorToString(thePopulationRatiosPerMonthVector);
	return result;
}	
	
void RPopulationDensityModel::setPopulationRatiosPerMonth(QString value)	
{	
	thePopulationRatiosPerMonthVector = stringToDoubleVector(value);

}	

QString RPopulationDensityModel::getPopulationRatiosPerWeekday()	
{	
	QString result = doubleVectorToString(thePopulationRatiosPerWeekdayVector);
	return result;
}	
	
void RPopulationDensityModel::setPopulationRatiosPerWeekday(QString value)	
{	
	thePopulationRatiosPerWeekdayVector = stringToDoubleVector(value);
}	
		
QString RPopulationDensityModel::getPopulationRatiosPerHour()	
{	
	QString result = doubleVectorToString(thePopulationRatiosPerHourVector);
	return result;
}	
	
void RPopulationDensityModel::setPopulationRatiosPerHour(QString value)	
{	
	thePopulationRatiosPerHourVector = stringToDoubleVector(value);
}	

QVector<double> RPopulationDensityModel::getPopulationRatiosPerHourVector() const
{
	return thePopulationRatiosPerHourVector;
}

QObject * RPopulationDensityModel::getTimeOffset() const
{
	return theTimeOffset.data();
}

void RPopulationDensityModel::setTimeOffset(QObject *value)
{
	theTimeOffset = qobject_cast<RParameter *>(value);
}
		
int RPopulationDensityModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int ThreadID)
	{	

	int hourRatioCount = thePopulationRatiosPerHourVector.count();
	if (hourRatioCount!=24) {
		qCritical() << "Error: Number Of inputs must be 24";
		return -1;
	}
	gsl_vector *hourRatioVector = gsl_vector_calloc(hourRatioCount);
	for (int i = 0; i < hourRatioCount; i++) {
	gsl_vector_set(hourRatioVector, i, thePopulationRatiosPerHourVector[i]);
	}

	int monthRatioCount = thePopulationRatiosPerMonthVector.count();
	if (monthRatioCount!=12) {
		qCritical() << "Error: Number Of inputs must be 12";
		return -1;
	}
	gsl_vector *monthRatioVector = gsl_vector_calloc(monthRatioCount);
	for (int i = 0; i < monthRatioCount; i++) {
	gsl_vector_set(monthRatioVector, i, thePopulationRatiosPerMonthVector[i]);
	}

	int weekDayRatioCount = thePopulationRatiosPerWeekdayVector.count();
	if (weekDayRatioCount!=7) {
		qCritical() << "Error: Number Of inputs must be 7";
		return -1;
	}
	gsl_vector *dayRatioVector = gsl_vector_calloc(weekDayRatioCount);
	for (int i = 0; i < weekDayRatioCount; i++) {
	gsl_vector_set(dayRatioVector, i, thePopulationRatiosPerWeekdayVector[i]);
	}

	//double T = theTime->getCurrentValue();
	double maxPopDens = theMaximumPopulationDensity.data()->getCurrentValue(ThreadID);

	//Assuming starting day is January 1, Saturday, 00:00 AM
	RTime *TimeParameter = qobject_cast<RTime *>(theTime.data());
	if (!TimeParameter) {
		qCritical() << "Error in model" << objectName() << ": A time object is not passed as the time.";
		return -1;
	}
	double currentTime = TimeParameter->getCurrentValue(ThreadID);
	double randomTime = theTimeOffset.data()->getCurrentValue(ThreadID);

	RTime T(0, QString("time"));
	T.setCurrentValue(currentTime + randomTime);


	double year = T.getCurrentYear(ThreadID); //THREAD_ID
	double month = T.getCurrentMonth(ThreadID); //THREAD_ID
	double dayOfWeek = T.getCurrentDayOfWeek(ThreadID); //THREAD_ID
	double dayOfMonth = T.getCurrentDay(ThreadID); //THREAD_ID
	double hour = T.getCurrentHour(ThreadID); //THREAD_ID
	double minute = T.getCurrentMinute(ThreadID); //THREAD_ID

		double count = 0.0;
		month = (month-1) + (dayOfMonth + hour/24 + minute/60/24)/30;
		hour = hour + minute/60;
		dayOfWeek = (dayOfWeek-1) + hour/24 + minute/60/24;

		double monthlyDecreasingCoefficient;
		if (month<=1.5&&month>=0.5){
			RiFunction(0.9,1.1,gsl_vector_get(monthRatioVector,0),gsl_vector_get(monthRatioVector,1),month,&monthlyDecreasingCoefficient);
		} else if(month<=2.5&&month>1.5){
			RiFunction(1.9,2.1,gsl_vector_get(monthRatioVector,1),gsl_vector_get(monthRatioVector,2),month,&monthlyDecreasingCoefficient);
		}else if(month<=3.5&&month>2.5){
			RiFunction(2.9,3.1,gsl_vector_get(monthRatioVector,2),gsl_vector_get(monthRatioVector,3),month,&monthlyDecreasingCoefficient);
		}else if(month<=4.5&&month>3.5){
			RiFunction(3.9,4.1,gsl_vector_get(monthRatioVector,3),gsl_vector_get(monthRatioVector,4),month,&monthlyDecreasingCoefficient);
		}else if(month<=5.5&&month>4.5){
			RiFunction(4.9,5.1,gsl_vector_get(monthRatioVector,4),gsl_vector_get(monthRatioVector,5),month,&monthlyDecreasingCoefficient);
		}else if(month<=6.5&&month>5.5){
			RiFunction(5.9,6.1,gsl_vector_get(monthRatioVector,5),gsl_vector_get(monthRatioVector,6),month,&monthlyDecreasingCoefficient);
		}else if(month<=7.5&&month>6.5){
			RiFunction(6.9,7.1,gsl_vector_get(monthRatioVector,6),gsl_vector_get(monthRatioVector,7),month,&monthlyDecreasingCoefficient);
		}else if(month<=8.5&&month>7.5){
			RiFunction(7.9,8.1,gsl_vector_get(monthRatioVector,7),gsl_vector_get(monthRatioVector,8),month,&monthlyDecreasingCoefficient);
		}else if(month<=9.5&&month>8.5){
			RiFunction(8.9,9.1,gsl_vector_get(monthRatioVector,8),gsl_vector_get(monthRatioVector,9),month,&monthlyDecreasingCoefficient);
		}else if(month<=10.5&&month>9.5){
			RiFunction(9.9,10.1,gsl_vector_get(monthRatioVector,9),gsl_vector_get(monthRatioVector,10),month,&monthlyDecreasingCoefficient);
		}else if(month<=11.5&&month>10.5){
			RiFunction(10.9,11.1,gsl_vector_get(monthRatioVector,10),gsl_vector_get(monthRatioVector,11),month,&monthlyDecreasingCoefficient);
		}else if(month<0.5){
			RiFunction(-0.1,0.1,gsl_vector_get(monthRatioVector,11),gsl_vector_get(monthRatioVector,0),month,&monthlyDecreasingCoefficient);
		}else if(month>11.5){
			RiFunction(11.9,12.1,gsl_vector_get(monthRatioVector,11),gsl_vector_get(monthRatioVector,0),month,&monthlyDecreasingCoefficient);
		}


		double dailyDecreasingCoefficient;
		if (dayOfWeek<=1.5&&dayOfWeek>=0.5){
			RiFunction(0.9,1.1,gsl_vector_get(dayRatioVector,2),gsl_vector_get(dayRatioVector,3),dayOfWeek,&dailyDecreasingCoefficient);
		} else if(dayOfWeek<=2.5&&dayOfWeek>1.5){
			RiFunction(1.9,2.1,gsl_vector_get(dayRatioVector,3),gsl_vector_get(dayRatioVector,4),dayOfWeek,&dailyDecreasingCoefficient);
		}else if(dayOfWeek<=3.5&&dayOfWeek>2.5){
			RiFunction(2.9,3.1,gsl_vector_get(dayRatioVector,4),gsl_vector_get(dayRatioVector,5),dayOfWeek,&dailyDecreasingCoefficient);
		}else if(dayOfWeek<=4.5&&dayOfWeek>3.5){
			RiFunction(3.9,4.1,gsl_vector_get(dayRatioVector,5),gsl_vector_get(dayRatioVector,6),dayOfWeek,&dailyDecreasingCoefficient);
		}else if(dayOfWeek<=5.5&&dayOfWeek>4.5){
			RiFunction(4.9,5.1,gsl_vector_get(dayRatioVector,6),gsl_vector_get(dayRatioVector,0),dayOfWeek,&dailyDecreasingCoefficient);
		}else if(dayOfWeek<=6.5&&dayOfWeek>5.5){
			RiFunction(5.9,6.1,gsl_vector_get(dayRatioVector,0),gsl_vector_get(dayRatioVector,1),dayOfWeek,&dailyDecreasingCoefficient);
		}else if(dayOfWeek>6.5){
			RiFunction(6.9,7.1,gsl_vector_get(dayRatioVector,1),gsl_vector_get(dayRatioVector,2),dayOfWeek,&dailyDecreasingCoefficient);
		}else if(dayOfWeek<0.5){
			RiFunction(-0.1,0.1,gsl_vector_get(dayRatioVector,1),gsl_vector_get(dayRatioVector,2),dayOfWeek,&dailyDecreasingCoefficient);
		}



		double hourlyDecreasingCoefficient;
		if (hour<=1.5&&hour>=0.5){
			RiFunction(0.9, 1.1, gsl_vector_get(hourRatioVector,0), gsl_vector_get(hourRatioVector,1), hour, &hourlyDecreasingCoefficient);
		} else if(hour<=2.5&&hour>1.5){
			RiFunction(1.9,2.1,gsl_vector_get(hourRatioVector,1),gsl_vector_get(hourRatioVector,2),hour,&hourlyDecreasingCoefficient);
		}else if(hour<=3.5&&hour>2.5){
			RiFunction(2.9,3.1,gsl_vector_get(hourRatioVector,2),gsl_vector_get(hourRatioVector,3),hour,&hourlyDecreasingCoefficient);
		}else if(hour<=4.5&&hour>3.5){
			RiFunction(3.9,4.1,gsl_vector_get(hourRatioVector,3),gsl_vector_get(hourRatioVector,4),hour,&hourlyDecreasingCoefficient);
		}else if(hour<=5.5&&hour>4.5){
			RiFunction(4.9,5.1,gsl_vector_get(hourRatioVector,4),gsl_vector_get(hourRatioVector,5),hour,&hourlyDecreasingCoefficient);
		}else if(hour<=6.5&&hour>5.5){
			RiFunction(5.9,6.1,gsl_vector_get(hourRatioVector,5),gsl_vector_get(hourRatioVector,6),hour,&hourlyDecreasingCoefficient);
		}else if(hour<=7.5&&hour>6.5){
			RiFunction(6.9,7.1,gsl_vector_get(hourRatioVector,6),gsl_vector_get(hourRatioVector,7),hour,&hourlyDecreasingCoefficient);
		}else if(hour<=8.5&&hour>7.5){
			RiFunction(7.9,8.1,gsl_vector_get(hourRatioVector,7),gsl_vector_get(hourRatioVector,8),hour,&hourlyDecreasingCoefficient);
		}else if(hour<=9.5&&hour>8.5){
			RiFunction(8.9,9.1,gsl_vector_get(hourRatioVector,8),gsl_vector_get(hourRatioVector,9),hour,&hourlyDecreasingCoefficient);
		}else if(hour<=10.5&&hour>9.5){
			RiFunction(9.9,10.1,gsl_vector_get(hourRatioVector,9),gsl_vector_get(hourRatioVector,10),hour,&hourlyDecreasingCoefficient);
		}else if(hour<=11.5&&hour>10.5){
			RiFunction(10.9,11.1,gsl_vector_get(hourRatioVector,10),gsl_vector_get(hourRatioVector,11),hour,&hourlyDecreasingCoefficient);
		}else if(hour<=12.5&&hour>11.5){
			RiFunction(11.9,12.1,gsl_vector_get(hourRatioVector,11),gsl_vector_get(hourRatioVector,12),hour,&hourlyDecreasingCoefficient);
		}else if(hour<=13.5&&hour>12.5){
			RiFunction(12.9,13.1,gsl_vector_get(hourRatioVector,12),gsl_vector_get(hourRatioVector,13),hour,&hourlyDecreasingCoefficient);
		}else if(hour<=14.5&&hour>13.5){
			RiFunction(13.9,14.1,gsl_vector_get(hourRatioVector,13),gsl_vector_get(hourRatioVector,14),hour,&hourlyDecreasingCoefficient);
		}else if(hour<=15.5&&hour>14.5){
			RiFunction(14.9,15.1,gsl_vector_get(hourRatioVector,14),gsl_vector_get(hourRatioVector,15),hour,&hourlyDecreasingCoefficient);
		}else if(hour<=16.5&&hour>15.5){
			RiFunction(15.9,16.1,gsl_vector_get(hourRatioVector,15),gsl_vector_get(hourRatioVector,16),hour,&hourlyDecreasingCoefficient);
		}else if(hour<=17.5&&hour>16.5){
			RiFunction(16.9,17.1,gsl_vector_get(hourRatioVector,16),gsl_vector_get(hourRatioVector,17),hour,&hourlyDecreasingCoefficient);
		}else if(hour<=18.5&&hour>17.5){
			RiFunction(17.9,18.1,gsl_vector_get(hourRatioVector,17),gsl_vector_get(hourRatioVector,18),hour,&hourlyDecreasingCoefficient);
		}else if(hour<=19.5&&hour>18.5){
			RiFunction(18.9,19.1,gsl_vector_get(hourRatioVector,18),gsl_vector_get(hourRatioVector,19),hour,&hourlyDecreasingCoefficient);
		}else if(hour<=20.5&&hour>19.5){
			RiFunction(19.9,20.1,gsl_vector_get(hourRatioVector,19),gsl_vector_get(hourRatioVector,20),hour,&hourlyDecreasingCoefficient);
		}else if(hour<=21.5&&hour>20.5){
			RiFunction(20.9,21.1,gsl_vector_get(hourRatioVector,20),gsl_vector_get(hourRatioVector,21),hour,&hourlyDecreasingCoefficient);
		}else if(hour<=22.5&&hour>21.5){
			RiFunction(21.9,22.1,gsl_vector_get(hourRatioVector,21),gsl_vector_get(hourRatioVector,22),hour,&hourlyDecreasingCoefficient);
		}else if(hour<=23.5&&hour>22.5){
			RiFunction(22.9,23.1,gsl_vector_get(hourRatioVector,22),gsl_vector_get(hourRatioVector,23),hour,&hourlyDecreasingCoefficient);
		}else if(hour>23.5){
			RiFunction(23.9,24.1,gsl_vector_get(hourRatioVector,23),gsl_vector_get(hourRatioVector,0),hour,&hourlyDecreasingCoefficient);
		}else if(hour<0.5){
			RiFunction(-0.1,0.1,gsl_vector_get(hourRatioVector,23),gsl_vector_get(hourRatioVector,0),hour,&hourlyDecreasingCoefficient);
		}


		double totalDecreasingCoefficient;
		totalDecreasingCoefficient = hourlyDecreasingCoefficient * dailyDecreasingCoefficient * monthlyDecreasingCoefficient;
		double PopDens = 0.0;
		PopDens = totalDecreasingCoefficient * maxPopDens;

		//Make memory free
		gsl_vector_free(hourRatioVector);
		gsl_vector_free(dayRatioVector);
		gsl_vector_free(monthRatioVector);

		thePopulationDensity->setCurrentValue(PopDens,ThreadID);



		return 0;
	}	


int RPopulationDensityModel::RiFunction(double lq, double uq, double rl, double ru, double dr, double *count)
	{
	double PI = M_PI;  //=a*sin(bx+c)+d
	double a = (ru-rl)/2; 
	double d = (ru+rl)/2;
	double b = PI/(uq-lq);
	double c = PI/2-uq*b;
	   
	if (dr <= lq) {
		*count = rl;
	}
	else if ((dr > lq)&&(dr < uq)) {
		*count = a * sin(b*dr+c) + d;
	}
	else {
		*count = ru;
	}
	return -1;
}


bool RPopulationDensityModel::canBeRunParallelly() {
	return true;
}

