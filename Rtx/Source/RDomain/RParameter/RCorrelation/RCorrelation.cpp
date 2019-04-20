#include "RCorrelation.h"

#include "RRandomVariable.h"

RCorrelation::RCorrelation(QObject *parent, QString name)
	: RParameter(parent, name)
{
/* //$$ 
	theRandomVariable1 = 0;
	theRandomVariable2 = 0;
*/

	theCoefficient = 0;
}

RCorrelation::~RCorrelation()
{

}

QObject * RCorrelation::getRandomVariable1() const	
{	
	return theRandomVariable1.data();
}	
	
void RCorrelation::setRandomVariable1(QObject *value)	
{	
	theRandomVariable1 = qobject_cast<RRandomVariable *>(value);
	
	if (theRandomVariable1 == theRandomVariable2) {
		rCritical() << "Warning: The first and second random variables in the correlation object" << objectName() << "are the same.";
	}
}	
	
QObject * RCorrelation::getRandomVariable2() const	
{	
	return theRandomVariable2.data();
}	
	
void RCorrelation::setRandomVariable2(QObject *value)	
{	
	theRandomVariable2 = qobject_cast<RRandomVariable *>(value);
	
	if (theRandomVariable1 == theRandomVariable2) {
		rCritical() << "Warning: The first and second random variables in the correlation object" << objectName() << "are the same.";
	}
}	

double RCorrelation::getCoefficient() const	
{	
	return theCoefficient;
}	

void RCorrelation::setCoefficient(double value)	
{	
	if ((value < -1.0) || (value > 1.0)) {
		rCritical() << "Error: The correlation coeffcient given to object" << objectName() << "must be between -1 and 1.";
		return;
	}

	theCoefficient = value;
}	

bool RCorrelation::canBeRunParallelly(){
	return true;
}