#include "RComponentLossModel.h"	

#include "RParameter.h"	
#include "RResponse.h"	

#include <gsl/gsl_vector.h>
#include <gsl/gsl_math.h>

#include <math.h>	

RComponentLossModel::RComponentLossModel(QObject *parent, QString name)
	: RModel(parent, name)
{
	// Initializing the data members
	//theReplacementCostPerUnit = 0;
	//theAreaOfOneQuantity = 0;
	theAreaDenepdent = RComponentLossModel::Yes;
	theBulkDiscount = RComponentLossModel::True;


	// Instantiating the response object
	theDomain->createObject(objectName() + "Response", "RGenericResponse");
	theRBuildingLoss = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	theRBuildingLoss->setModel(this);
}

RComponentLossModel::~RComponentLossModel()
{

}

double RComponentLossModel::getAreaOfOneQuantity() const
{
	return theAreaOfOneQuantity;
}

void RComponentLossModel::setAreaOfOneQuantity(double value)
{
	theAreaOfOneQuantity = value;
}

QString RComponentLossModel::getDamageRatioList()
{
	return parameterListToString(theDamageRatioList);
}

void RComponentLossModel::setDamageRatioList(QString value)
{
	theDamageRatioList = stringToParameterList(value);
}

QString RComponentLossModel::getAreas()
{
	QString result = doubleVectorToString(theAreasVector);
	return result;
}

void RComponentLossModel::setAreas(QString value)
{
	theAreasVector = stringToDoubleVector(value);

}


QObject * RComponentLossModel::getReplacementCostPerUnit() const
{
	return theReplacementCostPerUnit.data();
}

void RComponentLossModel::setReplacementCostPerUnit(QObject *value)
{
	theReplacementCostPerUnit = qobject_cast<RParameter *>(value);
}

QString RComponentLossModel::getThresholdDamageRatios()
{
	QString result = doubleVectorToString(theThresholdDamageRatiosVector);
	return result;
}

void RComponentLossModel::setThresholdDamageRatios(QString value)
{
	theThresholdDamageRatiosVector = stringToDoubleVector(value);

}

QString RComponentLossModel::getLowerQuantity()
{
	QString result = doubleVectorToString(theLowerQuantityVector);
	return result;
}

void RComponentLossModel::setLowerQuantity(QString value)
{
	theLowerQuantityVector = stringToDoubleVector(value);

}

QString RComponentLossModel::getUpperQuantity()
{
	QString result = doubleVectorToString(theUpperQuantityVector);
	return result;
}

void RComponentLossModel::setUpperQuantity(QString value)
{
	theUpperQuantityVector = stringToDoubleVector(value);

}

QString RComponentLossModel::getDiscountRates()
{
	QString result = doubleVectorToString(theDiscountRatesVector);
	return result;
}

void RComponentLossModel::setDiscountRates(QString value)
{
	theDiscountRatesVector = stringToDoubleVector(value);

}

RComponentLossModel::RAreaDenepdent RComponentLossModel::getAreaDenepdent() const
{
	return theAreaDenepdent;
}

void RComponentLossModel::setAreaDenepdent(RComponentLossModel::RAreaDenepdent value)
{
	theAreaDenepdent = value;
}

RComponentLossModel::RBulkDiscount RComponentLossModel::getBulkDiscount() const
{
	return theBulkDiscount;
}

void RComponentLossModel::setBulkDiscount(RComponentLossModel::RBulkDiscount value)
{
	theBulkDiscount = value;
}

QVector<double> RComponentLossModel::getAreasVector() const
{
	return theAreasVector;
}
QVector<double> RComponentLossModel::getThresholdDamageRatiosVector() const
{
	return theThresholdDamageRatiosVector;
}
QVector<double> RComponentLossModel::getLowerQuantityVector() const
{
	return theLowerQuantityVector;
}

QVector<double> RComponentLossModel::getUpperQuantityVector() const
{
	return theUpperQuantityVector;
}
QVector<double> RComponentLossModel::getDiscountRatesVector() const
{
	return theDiscountRatesVector;
}

QObject * RComponentLossModel::getRepairStatus() const
{
	return theRepairStatus.data();
}

void RComponentLossModel::setRepairStatus(QObject *value)
{
	theRepairStatus = qobject_cast<RParameter *>(value);
}

int RComponentLossModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int ThreadID)
{
	double PI = M_PI;
	double MaxCost = theReplacementCostPerUnit.data()->getCurrentValue();

	//Saving threshold DR vector in a vector named dsVector
	int dsCount = theThresholdDamageRatiosVector.count();
	gsl_vector *dsVector = gsl_vector_calloc(dsCount);
	for (int i = 0; i < dsCount; i++) {
		gsl_vector_set(dsVector, i, theThresholdDamageRatiosVector[i]);
	}

	//Saving discount rates in a vector named discVector
	int discCount = theDiscountRatesVector.count();
	gsl_vector *discVector = gsl_vector_calloc(discCount);
	for (int i = 0; i < discCount; i++) {
		gsl_vector_set(discVector, i, theDiscountRatesVector[i]);
	}
	int lowerCount = theLowerQuantityVector.count();
	if (discCount != (lowerCount)) {
		qCritical() << "Error: Number Of Two Lists Of Lower And Discount Are Not The Same";
		return -1;
	}
	//Saving lower quantities in a vector named lowerVector
	gsl_vector *lowerVector = gsl_vector_calloc(lowerCount);
	for (int i = 0; i < lowerCount; i++) {
		gsl_vector_set(lowerVector, i, theLowerQuantityVector[i]);
	}

	int upperCount = theUpperQuantityVector.count();
	if (discCount != (upperCount)) {
		qCritical() << "Error: Number Of Two Lists Of Upper And Discount Are Not The Same";
		return -1;
	}
	//Saving upper quantities in a vector named upperVector
	gsl_vector *upperVector = gsl_vector_calloc(upperCount);
	for (int i = 0; i < upperCount; i++) {
		gsl_vector_set(upperVector, i, theLowerQuantityVector[i]);
	}

	//Saving Damage ratios in a vector named drVector
	int drCount = theDamageRatioList.count();
	gsl_vector *drVector = gsl_vector_calloc(drCount);
	for (int i = 0; i < drCount; i++) {
		gsl_vector_set(drVector, i, theDamageRatioList[i].data()->getCurrentValue());
	}

	double Loss = 0.0;


	double numberOfDS = dsCount / 2;

	//IF AREA As INPUT IS IMPORTANT, 
	if (theAreaDenepdent == 1 && theBulkDiscount == 1) {

		if (drCount != theAreasVector.count()) {
			qCritical() << "Error: Number Of Two Lists Of Area And Damage Ratio Are Not The Same";
			return -1;
		}
		gsl_vector *areaVector = gsl_vector_calloc(drCount);
		for (int i = 0; i < drCount; i++) {
			gsl_vector_set(areaVector, i, theAreasVector[i]);
		}

		gsl_vector *drArea = gsl_vector_calloc(numberOfDS);
		double totalArea = 0.0;
		double count;

		for (int j = numberOfDS - 1; j > -1; j--) {
			for (int i = 0; i < drCount; i++) {
				totalArea = 0.0;
				double dr = gsl_vector_get(drVector, i);
				//computing number of components experienced DS j and save it in variable count
				RiFunction(gsl_vector_get(dsVector, 2 * j), gsl_vector_get(dsVector, 2 * j + 1), dr, &count);
				//ceilingDR3Count = ceilingDR3Count+count;
				//Adding area of the components experienced DS j to the total area of components which experienced DS j
				gsl_vector_set(drArea, j, (gsl_vector_get(drArea, j) + count * gsl_vector_get(areaVector, i)));
				totalArea = totalArea + gsl_vector_get(areaVector, i);
			}
			//Ni=Sigma(Ri(DR))-Sigma(Nj,j>i)
			for (int k = j + 1; k < numberOfDS; k++) {
				gsl_vector_set(drArea, j, (gsl_vector_get(drArea, j) - gsl_vector_get(drArea, k)));
			}
		}
		//counting number of elements with minor damage
		double dr0Area = totalArea;
		for (int k = 0; k < numberOfDS; k++) {
			dr0Area = dr0Area - gsl_vector_get(drArea, k);
		}


		//calculating reducing coefficients
		gsl_vector *rCRVector = gsl_vector_calloc(discCount + 1);
		double rcr = 0.0;
		for (int i = 0; i < discCount; i++) {
			//calculating reducing cost ratio for each MOR--here MOR i
			RCRFunction(gsl_vector_get(lowerVector, i), gsl_vector_get(upperVector, i), 1, (1 - gsl_vector_get(discVector, i)), (gsl_vector_get(drArea, i) / theAreaOfOneQuantity), &rcr);
			gsl_vector_set(rCRVector, i + 1, rcr);
		}
		double rCR0 = 0.0;
		//Calculating rcr for components with minor damage
		RCRFunction(gsl_vector_get(lowerVector, 0), gsl_vector_get(upperVector, 0), 1, (1 - gsl_vector_get(discVector, 0)), (dr0Area / (theAreaOfOneQuantity)), &rCR0);
		gsl_vector_set(rCRVector, 0, rCR0);

		//calculating loss

		//For more explanation, refer to thesis
		gsl_vector *R = gsl_vector_calloc(numberOfDS + 1 + 1);
		gsl_vector_set(R, 0, 1);
		gsl_vector_set(R, numberOfDS + 1, 0);
		double ci;
		for (int i = 0; i < drCount; i++) {
			double dr = gsl_vector_get(drVector, i);
			for (int j = 0; j < numberOfDS; j++) { //Determining component i is in which MOR
				RiFunction(gsl_vector_get(dsVector, 2 * j), gsl_vector_get(dsVector, 2 * j + 1), dr, &count);
				gsl_vector_set(R, j + 1, count);
			}
			ci = gsl_vector_get(drVector, i) * MaxCost * gsl_vector_get(areaVector, i);
			for (int k = 0; k < numberOfDS + 1; k++) {
				Loss = Loss + gsl_vector_get(rCRVector, k) * (gsl_vector_get(R, k) - gsl_vector_get(R, k + 1)) * ci; //It is my method to compute loss with considering rcr
			}
		}
		gsl_vector_free(dsVector);
		gsl_vector_free(discVector);
		gsl_vector_free(lowerVector);
		gsl_vector_free(upperVector);
		gsl_vector_free(drVector);
		gsl_vector_free(areaVector);
		gsl_vector_free(drArea);
		gsl_vector_free(rCRVector);
		gsl_vector_free(R);

		//IF AREA OF THE MEMBERS ARE NOT IMPORTANT IN CALCULATION OF LOSS, AND IF COST OF REPAIR IS BulkDiscount WITH INCREASE IN THE QUANTITY OF EMELENTS NEED REPAIR
	}
	else if (theAreaDenepdent == 0 && theBulkDiscount == 1) {

		gsl_vector *drICount = gsl_vector_calloc(numberOfDS);

		double count;

		for (int j = numberOfDS - 1; j > -1; j--) {
			for (int i = 0; i < drCount; i++) {
				double dr = gsl_vector_get(drVector, i);
				//Determining if component i is in DS j or not
				RiFunction(gsl_vector_get(dsVector, 2 * j), gsl_vector_get(dsVector, 2 * j + 1), dr, &count);
				gsl_vector_set(drICount, j, (gsl_vector_get(drICount, j) + count));

			}
			for (int k = j + 1; k < numberOfDS; k++) { //like above--in first if
				gsl_vector_set(drICount, j, (gsl_vector_get(drICount, j) - gsl_vector_get(drICount, k)));
			}
		}
		double dr0Count = drCount;
		for (int k = 0; k < numberOfDS; k++) {
			dr0Count = dr0Count - gsl_vector_get(drICount, k);
		}


		//calculating reducing coefficients
		gsl_vector *rCRVector = gsl_vector_calloc(discCount + 1);
		double rcr = 0.0;
		for (int i = 0; i < discCount; i++) {
			RCRFunction(gsl_vector_get(lowerVector, i), gsl_vector_get(upperVector, i), 1, (1 - gsl_vector_get(discVector, i)), gsl_vector_get(drICount, i), &rcr);
			gsl_vector_set(rCRVector, i + 1, rcr);
		}
		double rCR0 = 0.0;
		RCRFunction(gsl_vector_get(lowerVector, 0), gsl_vector_get(upperVector, 0), 1, (1 - gsl_vector_get(discVector, 0)), dr0Count, &rCR0);
		gsl_vector_set(rCRVector, 0, rCR0);

		//calculating loss
		gsl_vector *R = gsl_vector_calloc(numberOfDS + 1 + 1);
		gsl_vector_set(R, 0, 1);
		gsl_vector_set(R, numberOfDS + 1, 0);
		double ci;
		for (int i = 0; i < drCount; i++) {
			double dr = gsl_vector_get(drVector, i);
			for (int j = 0; j < numberOfDS; j++) {
				RiFunction(gsl_vector_get(dsVector, 2 * j), gsl_vector_get(dsVector, 2 * j + 1), dr, &count);
				gsl_vector_set(R, j + 1, count);
			}
			ci = gsl_vector_get(drVector, i) * MaxCost;
			for (int k = 0; k < numberOfDS + 1; k++) {
				Loss = Loss + gsl_vector_get(rCRVector, k) * (gsl_vector_get(R, k) - gsl_vector_get(R, k + 1)) * ci;
			}
		}
		gsl_vector_free(drICount);
		gsl_vector_free(dsVector);
		gsl_vector_free(discVector);
		gsl_vector_free(lowerVector);
		gsl_vector_free(upperVector);
		gsl_vector_free(drVector);
		gsl_vector_free(rCRVector);
		gsl_vector_free(R);
	}

	else if (theAreaDenepdent == 0 && theBulkDiscount == 0) {

		double ci = 0.0;
		for (int i = 0; i < drCount; i++) {
			ci = gsl_vector_get(drVector, i)*MaxCost;
			Loss = Loss + ci;
		}
		gsl_vector_free(dsVector);
		gsl_vector_free(discVector);
		gsl_vector_free(lowerVector);
		gsl_vector_free(upperVector);
		gsl_vector_free(drVector);
	}

	else if (theAreaDenepdent == 1 && theBulkDiscount == 0) {
		//WE HAVEN'T HAD SUCH AN PERFORMANCE GROUP UNTIL NOW
		double ci = 0.0;

		if (drCount != theAreasVector.count()) {
			qCritical() << "Error: Number Of Two Lists Of Damage Ratio and Area Are Not The Same";
			return -1;
		}
		gsl_vector *areaVector = gsl_vector_calloc(drCount);
		for (int i = 0; i < drCount; i++) {
			gsl_vector_set(areaVector, i, theAreasVector[i]);
		}


		for (int i = 0; i < drCount; i++) {
			ci = gsl_vector_get(drVector, i) * MaxCost * gsl_vector_get(areaVector, i);
			Loss = Loss + ci;
		}
		gsl_vector_free(dsVector);
		gsl_vector_free(discVector);
		gsl_vector_free(lowerVector);
		gsl_vector_free(upperVector);
		gsl_vector_free(drVector);
		gsl_vector_free(areaVector);
	}

	double repairStatus = theRepairStatus.data()->getCurrentValue();
	//If the repairStatus is zero, i.e., the repair opeation has not been done yet, the repair cost is zero
	Loss = Loss * repairStatus;
	theRBuildingLoss->setCurrentValue(Loss);
	return 0;
}
int RComponentLossModel::RiFunction(double lq, double uq, double dr, double *count)
{
	double PI = M_PI;
	double a = 0.5; double d = 0.5;
	double b = PI / (uq - lq);
	double c = (3 * PI) / 2 - lq * b;

	if (dr <= lq) {
		*count = 0;
	}
	else if ((dr > lq) && (dr < uq)) {
		*count = a * sin(b*dr + c) + d;
	}
	else {
		*count = 1;
	}
	return -1;
}

int RComponentLossModel::RCRFunction(double lq, double uq, double rl, double ru, double n, double *coeff)
{
	double PI = M_PI;
	double b = PI / (uq - lq);     //y=a*sin(bx+c)+d
	double c = (PI / 2) - lq*b;
	double d = (1 + (ru / rl)) / 2;
	double a = 1 - d;

	if (n <= lq) {
		*coeff = 1;
	}
	else if ((n > lq) && (n < uq)) {
		*coeff = a * sin(b*n + c) + d;
	}
	else {
		*coeff = (ru / rl);
	}
	return -1;
}

bool RComponentLossModel::canBeRunParallelly() {
	return false;
}