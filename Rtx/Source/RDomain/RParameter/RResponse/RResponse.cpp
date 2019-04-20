#include "RResponse.h"

#include "RModel.h"
#include "RDomain.h"

RResponse::RResponse(QObject *parent, QString name)
	: RParameter(parent, name)
{

	/* ($MTh$) initializing */
	int arraySizeForParallelizedObject = getArraySizeForParallelizedObject();

	isAnalyzedFlag = new bool[arraySizeForParallelizedObject];
	backupValue = new double[arraySizeForParallelizedObject];

	for (int thID = 0 ; thID < arraySizeForParallelizedObject ; thID++)
		isAnalyzedFlag[thID] = false;

	//$$ theModel = 0;
	theTotalStandardDeviation = 0;
	theBetaSensitivityWrtTotalStandardDeviation = 0;
	theTotalEpistemicStandardDeviation = 0;
	theBetaSensitivityWrtTotalEpistemicStandardDeviation = 0;
	theModelStandardDeviation = 0;
	theBetaSensitivityWrtModelStandardDeviation = 0;
	theModelEpistemicStandardDeviation = 0;
	theBetaSensitivityWrtModelEpistemicStandardDeviation = 0;
}

RResponse::~RResponse()
{

	delete [] backupValue;
	delete [] isAnalyzedFlag;
}

void RResponse::setCurrentValue(double value)
{
	//RParameter *parameter = qobject_cast<RParameter *>(this);
	//parameter->theCurrentValue(value);
	RParameter::setCurrentValue(value);
	this->setAnalyzed(true,0);
}

void RResponse::setCurrentValue(double value, int threadID){
	
	RParameter::setCurrentValue(value, threadID);
	this->setAnalyzed(true, threadID);
}

bool RResponse::isAnalyzed(int threadID)
{
	return isAnalyzedFlag[threadID];
}

void RResponse::setAnalyzed(bool analyzed, int threadID)
{
	isAnalyzedFlag[threadID] = analyzed;
}

QObject * RResponse::getModel() const
{
	return theModel.data();
}

double RResponse::getTotalStandardDeviation() const	
{	
	return theTotalStandardDeviation;
}	
	
void RResponse::setTotalStandardDeviation(double value)	
{	
	theTotalStandardDeviation = value;
}	
	
double RResponse::getBetaSensitivityWrtTotalStandardDeviation() const	
{	
	return theBetaSensitivityWrtTotalStandardDeviation;
}	
	
void RResponse::setBetaSensitivityWrtTotalStandardDeviation(double value)	
{	
	theBetaSensitivityWrtTotalStandardDeviation = value;
}	
	
double RResponse::getTotalEpistemicStandardDeviation() const	
{	
	return theTotalEpistemicStandardDeviation;
}	
	
void RResponse::setTotalEpistemicStandardDeviation(double value)	
{	
	theTotalEpistemicStandardDeviation = value;
}	
	
double RResponse::getBetaSensitivityWrtTotalEpistemicStandardDeviation() const	
{	
	return theBetaSensitivityWrtTotalEpistemicStandardDeviation;
}	
	
void RResponse::setBetaSensitivityWrtTotalEpistemicStandardDeviation(double value)	
{	
	theBetaSensitivityWrtTotalEpistemicStandardDeviation = value;
}	
	
double RResponse::getModelStandardDeviation() const	
{	
	return theModelStandardDeviation;
}	
	
void RResponse::setModelStandardDeviation(double value)	
{	
	theModelStandardDeviation = value;
}	
	
double RResponse::getBetaSensitivityWrtModelStandardDeviation() const	
{	
	return theBetaSensitivityWrtModelStandardDeviation;
}	
	
void RResponse::setBetaSensitivityWrtModelStandardDeviation(double value)	
{	
	theBetaSensitivityWrtModelStandardDeviation = value;
}	
	
double RResponse::getModelEpistemicStandardDeviation() const	
{	
	return theModelEpistemicStandardDeviation;
}	
	
void RResponse::setModelEpistemicStandardDeviation(double value)	
{	
	theModelEpistemicStandardDeviation = value;
}	
	
double RResponse::getBetaSensitivityWrtModelEpistemicStandardDeviation() const	
{	
	return theBetaSensitivityWrtModelEpistemicStandardDeviation;
}	
	
void RResponse::setBetaSensitivityWrtModelEpistemicStandardDeviation(double value)	
{	
	theBetaSensitivityWrtModelEpistemicStandardDeviation = value;
}	

void RResponse::setModel(QObject *value)
{
	RModel *tempModel = qobject_cast<RModel *>(value);
	if (tempModel) {
		theModel = tempModel;
		setParent(theModel.data());
	} else {
		if (theModel) {
			setParent(theModel.data()->parent());
		}
		theModel.clear();
	}
}

QMap<RParameter *, double> RResponse::getGradientMap()
{
	return theGradientMap;
}

void RResponse::setGradientMap(QMap<RParameter *, double> passedGradeintMap)
{
	theGradientMap.clear();
	theGradientMap = passedGradeintMap;
}

void RResponse::clearGradientMap()
{
	theGradientMap.clear();
}

void RResponse::backup(int threadID)
{
	backupValue[threadID] = theCurrentValue[threadID];
}

void RResponse::restore(int threadID)
{
	theCurrentValue[threadID] = backupValue[threadID];
}

QVector<double> RResponse::getValueVector(int threadID)
{
	QVector<double> resultVector;
	resultVector << theCurrentValue[threadID];
	return resultVector;
}

QVector<double> RResponse::getBackupVector(int threadID)
{
	QVector<double> resultVector;
	resultVector << backupValue[threadID];
	return resultVector;
}

void RResponse::setValueVector(QVector<double> valueVector, int threadID)
{
	if (valueVector.isEmpty()) {
		rCritical(threadID) << "Error: The value vector passed to the response" << objectName() << " is empty.";
		return;
	}
	theCurrentValue[threadID] = valueVector[0];
}

int RResponse::updateValue(int threadID)
{
	return 0;
}