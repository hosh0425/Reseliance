#include "RExampleModel.h"
#include "RResponse.h"
#include "RParameter.h"

RExampleModel::RExampleModel(QObject *parent, QString name)
: RModel(parent, name)
{
	// Initilizing the data members
	//$$ theParameterInput = 0;

	// Instantiating the model response
	theDomain->createObject(objectName() + "Response", "RGenericResponse");
	theResponse = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theResponse->setModel(this);
}

RExampleModel::~RExampleModel()
{

}

QObject * RExampleModel::getParameterInput() const	
{	
	return theParameterInput.data();
}	

void RExampleModel::setParameterInput(QObject *value)	
{	
	theParameterInput = qobject_cast<RParameter *>(value);
}
QString RExampleModel::getParameterList()	
{	
	return parameterListToString(theParameterList);
}	

void RExampleModel::setParameterList(QString value)	
{	
	theParameterList = stringToParameterList(value);
}

int RExampleModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType)
{	
	// This method is where the model algorithm is implemented


	double result = 0;

	// Summing up the square of all parameters in theParameterList
	for (int i = 0; i < theParameterList.count(); i++) {
		double parameterValue = theParameterList[i].data()->getCurrentValue();
		result += parameterValue * parameterValue;
	}

	// Adding theParameterInput to the sum 
	result += theParameterInput.data()->getCurrentValue();

	// Updating the output response
	theResponse->setCurrentValue(result);


	// Evaluating DDM response sensitivities
	if (evaluateDDM) {
		// Declaring a QMap for the sensitivities with respect to theResponse
		QMap<RParameter *, double> theRGradientMap;

		// Differentiating theResponse with respect theParameterList members
		for (int i = 0; i < theParameterList.count(); i++) {
			double dR_dParameteri = 2 * theParameterList[i].data()->getCurrentValue();
			theRGradientMap.insertMulti(theParameterList[i].data(), dR_dParameteri);
		}

		// Differentiating  theResponse with respect to theParameterInput
		double dR_dParameterInput = 1.0;
		theRGradientMap.insertMulti(theParameterInput.data(), dR_dParameterInput);

		// Adding theRGradientMap to theDDMMap
		theDDMMap.insertMulti(theResponse, theRGradientMap);
	}

	// Returning 1, meaning that the analysis is carried out successfully
	return 1;
}

