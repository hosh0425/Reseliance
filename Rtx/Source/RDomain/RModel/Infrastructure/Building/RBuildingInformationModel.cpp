#include "RBuildingInformationModel.h"

#include "RParameter.h"
#include "RResponse.h"
#include "RTime.h"

RBuildingInformationModel::RBuildingInformationModel(QObject *parent, QString name)
	: RModel(parent, name)
{
	//$$ theLocation = 0;
	//$$ theTimeOfConstruction = 0;

	theRegion = RRegion::Iran;
	theConstructionQuality = RConstructionQuality::Ordinary;

	theStoryHeight = 3.0;
	theLoadBearingSystem = RBuildingInformationModel::ShearWall;
	theMaterial = RBuildingInformationModel::ReinforcedConcrete;
	theOccupancyType = RBuildingInformationModel::SingleResidence;
	thePlanShape = RBuildingInformationModel::Rectangular;
	thePlanIrregularity = false;
	theVerticalIrregularity = false;
	theSoftStorey = false;
	theOpening = false;
	theShortColumn = false;
	thePounding = false;
	theRetrofitted = false;
	theBasement = false;
	theBalcony = false;

	theTotalArea = 0;
	theNumberOfStoreys = 0;
	thePrototype = RBuildingInformationModel::ConcreteShearWall;
	theHeight = 0;

	int arraySizeForParallelizedObject = getArraySizeForParallelizedObject();
	theCodeLevel = new RCodeLevel[arraySizeForParallelizedObject];
	theCodeLevel[0] = RBuildingInformationModel::HighCode;
}

RBuildingInformationModel::~RBuildingInformationModel()
{
	delete [] theCodeLevel;
}

RBuildingInformationModel::RRegion RBuildingInformationModel::getRegion() const
{
	return theRegion;
}

void RBuildingInformationModel::setRegion(RBuildingInformationModel::RRegion value)
{
	theRegion = value;
}
RBuildingInformationModel::RConstructionQuality RBuildingInformationModel::getConstructionQuality() const
{
	return theConstructionQuality;
}

void RBuildingInformationModel::setConstructionQuality(RBuildingInformationModel::RConstructionQuality value)
{
	theConstructionQuality = value;
}

QObject * RBuildingInformationModel::getLocation() const	
{	
	return theLocation.data();
}	
	
void RBuildingInformationModel::setLocation(QObject *value)	
{	
	theLocation = qobject_cast<RParameter *>(value);

	// Verification that the user has given an RLocationResponse or an RLocation
	parameterToLocation(theLocation.data()); 
}	

QObject * RBuildingInformationModel::getTimeOfConstruction() const	
{	
	return theTimeOfConstruction.data();
}	
	
void RBuildingInformationModel::setTimeOfConstruction(QObject *value)	
{	
	theTimeOfConstruction = qobject_cast<RParameter *>(value);

	computeCodeLevel(this->getRetrofitted(), 0);
}	
	
RBuildingInformationModel::RLoadBearingSystem RBuildingInformationModel::getLoadBearingSystem() const	
{	
	return theLoadBearingSystem;
}	
	
void RBuildingInformationModel::setLoadBearingSystem(RBuildingInformationModel::RLoadBearingSystem value)	
{	
	theLoadBearingSystem = value;

	computePrototype();
}	
	
RBuildingInformationModel::RMaterial RBuildingInformationModel::getMaterial() const	
{	
	return theMaterial;
}	
	
void RBuildingInformationModel::setMaterial(RBuildingInformationModel::RMaterial value)	
{	
	theMaterial = value;

	computePrototype();
}	

double RBuildingInformationModel::getStoryHeight() const	
{	
	return theStoryHeight;
}	

void RBuildingInformationModel::setStoryHeight(double value)	
{	
	theStoryHeight = value;
	computeHeightAndArea();
}	


	
QString RBuildingInformationModel::getNumberOfStoreys()	
{	
	QString result = doubleVectorToString(theNumberOfStoreysVector);
	return result;
}	
	
void RBuildingInformationModel::setNumberOfStoreys(QString value)	
{	
	theNumberOfStoreysVector = stringToDoubleVector(value);

	computeHeightAndArea();
}	
	
QString RBuildingInformationModel::getFootprintArea()	
{	
	QString result = doubleVectorToString(theFootprintAreaVector);
	return result;
}	
	
void RBuildingInformationModel::setFootprintArea(QString value)	
{	
	theFootprintAreaVector = stringToDoubleVector(value);

	computeHeightAndArea();
}	
	
RBuildingInformationModel::ROccupancyType RBuildingInformationModel::getOccupancyType() const	
{	
	return theOccupancyType;
}	
	
void RBuildingInformationModel::setOccupancyType(RBuildingInformationModel::ROccupancyType value)	
{	
	theOccupancyType = value;
}	
	
RBuildingInformationModel::RPlanShape RBuildingInformationModel::getPlanShape() const	
{	
	return thePlanShape;
}	
	
void RBuildingInformationModel::setPlanShape(RBuildingInformationModel::RPlanShape value)	
{	
	thePlanShape = value;
}	
	
bool RBuildingInformationModel::getPlanIrregularity() const	
{	
	return thePlanIrregularity;
}	
	
void RBuildingInformationModel::setPlanIrregularity(bool value)	
{	
	thePlanIrregularity = value;
}	
	
bool RBuildingInformationModel::getVerticalIrregularity() const	
{	
	return theVerticalIrregularity;
}	
	
void RBuildingInformationModel::setVerticalIrregularity(bool value)	
{	
	theVerticalIrregularity = value;
}	
	
bool RBuildingInformationModel::getSoftStorey() const	
{	
	return theSoftStorey;
}	
	
void RBuildingInformationModel::setSoftStorey(bool value)	
{	
	theSoftStorey = value;
}	
	
bool RBuildingInformationModel::getOpening() const	
{	
	return theOpening;
}	
	
void RBuildingInformationModel::setOpening(bool value)	
{	
	theOpening = value;
}	
	
bool RBuildingInformationModel::getShortColumn() const	
{	
	return theShortColumn;
}	
	
void RBuildingInformationModel::setShortColumn(bool value)	
{	
	theShortColumn = value;
}	
	
bool RBuildingInformationModel::getPounding() const	
{	
	return thePounding;
}	
	
void RBuildingInformationModel::setPounding(bool value)	
{	
	thePounding = value;
}	
	
bool RBuildingInformationModel::getRetrofitted() const	
{	
	return theRetrofitted;
}	
	
void RBuildingInformationModel::setRetrofitted(bool value)	
{	
	theRetrofitted = value;
	
	computeCodeLevel(this->getRetrofitted(), 0);
}	
	
bool RBuildingInformationModel::getBasement() const	
{	
	return theBasement;
}	
	
void RBuildingInformationModel::setBasement(bool value)	
{	
	theBasement = value;
}	
	
bool RBuildingInformationModel::getBalcony() const	
{	
	return theBalcony;
}	
	
void RBuildingInformationModel::setBalcony(bool value)	
{	
	theBalcony = value;
}	

QVector<double> RBuildingInformationModel::getNumberOfStoreysVector() const
{
	return theNumberOfStoreysVector;
}

QVector<double> RBuildingInformationModel::getFootprintAreaVector() const
{
	return theFootprintAreaVector;
}

double RBuildingInformationModel::getTotalNumberOfStoreys() const
{	
	return theNumberOfStoreys;
}

double RBuildingInformationModel::getTotalArea() const
{	
	return theTotalArea;
}

RBuildingInformationModel::RCodeLevel RBuildingInformationModel::getCodeLevel(int threadID)
{	
	return theCodeLevel[threadID];
}

RBuildingInformationModel::RPrototype RBuildingInformationModel::getPrototype() const
{
	return thePrototype;
}

double RBuildingInformationModel::getHeight() const
{	
	return theHeight;
}

void RBuildingInformationModel::computeCodeLevel(bool retrofitted, int threadID)
{	
	RTime *theTime = qobject_cast<RTime *>(theTimeOfConstruction.data());
	if (!theTime) {
		rCritical(threadID) << "Error in model" << objectName() << ": A time object is not passed as the time of construction. Rt will use the default code level.";
		theCodeLevel[threadID] = RBuildingInformationModel::PreCode;
		return;
	}
	int yearOfConstrcution = theTime->getCurrentYear(threadID);
	double constructionQuality = this->getConstructionQuality();
	double region = this->getRegion();

	if (region == RBuildingInformationModel::US) {
		if (yearOfConstrcution <= 1940) {
			if (retrofitted) {
				theCodeLevel[threadID] = RBuildingInformationModel::ModerateCode;
			} else {
				theCodeLevel[threadID] = RBuildingInformationModel::PreCode;
			}
		}
		else if (yearOfConstrcution <= 1975) {
			if (retrofitted) {
				theCodeLevel[threadID] = RBuildingInformationModel::ModerateCode;
			} else {
				theCodeLevel[threadID] = RBuildingInformationModel::LowCode;
			}
		}
		else {
			if (retrofitted) {
				theCodeLevel[threadID] = RBuildingInformationModel::HighCode;
			} else {
				theCodeLevel[threadID] = RBuildingInformationModel::ModerateCode;
			}
		}
	}

	//Compute code level fo Iran based on Amin Ghasemi paper
	if (region == RBuildingInformationModel::Iran) {
		if (yearOfConstrcution <= 1971) {
			if (retrofitted) {
				theCodeLevel[threadID] = RBuildingInformationModel::ModerateCode;
			}
			else {
				theCodeLevel[threadID] = RBuildingInformationModel::PreCode;
			}
		}
		else if (yearOfConstrcution <= 2000) {
			if (constructionQuality == RBuildingInformationModel::Inferior) {
				if (retrofitted) {
					theCodeLevel[threadID] = RBuildingInformationModel::ModerateCode;
				}
				else {
					theCodeLevel[threadID] = RBuildingInformationModel::PreCode;
				}
			}
			else {
				if (retrofitted) {
					theCodeLevel[threadID] = RBuildingInformationModel::ModerateCode;
				}
				else {
					theCodeLevel[threadID] = RBuildingInformationModel::LowCode;
				}
			}

			
		}
		else {
			if (constructionQuality == RBuildingInformationModel::Inferior) {
				if (retrofitted) {
					theCodeLevel[threadID] = RBuildingInformationModel::ModerateCode;
				}
				else {
					theCodeLevel[threadID] = RBuildingInformationModel::LowCode;
				}
			}
			else {
				if (retrofitted) {
					theCodeLevel[threadID] = RBuildingInformationModel::HighCode;
				}
				else {
					theCodeLevel[threadID] = RBuildingInformationModel::ModerateCode;
				}
			}
		}
	}
}

void RBuildingInformationModel::computePrototype()
{
	bool ok = true; 
	if (theMaterial == RBuildingInformationModel::ReinforcedConcrete) {
		if (theLoadBearingSystem == RBuildingInformationModel::ShearWall) {
			thePrototype = RBuildingInformationModel::ConcreteShearWall;
		} else if (theLoadBearingSystem == RBuildingInformationModel::MomentFrame) {
			thePrototype = RBuildingInformationModel::ConcreteMomentFrame;
		} else if (theLoadBearingSystem == RBuildingInformationModel::MasonryWall) {
			thePrototype = RBuildingInformationModel::ConcreteFrameWithMasonryInfillWall;
		} else {
			ok = false;
		}
	} else if (theMaterial == RBuildingInformationModel::PrecastConcrete) {
		if (theLoadBearingSystem == RBuildingInformationModel::ShearWall) {
			thePrototype = RBuildingInformationModel::PrecastFrameWithShearWall;
		} else {
			ok = false;
		}
	} else if (theMaterial == RBuildingInformationModel::Steel) {
		if (theLoadBearingSystem == RBuildingInformationModel::ShearWall) {
			thePrototype = RBuildingInformationModel::SteelFrameWithConcreteShearWall;
		} else if (theLoadBearingSystem == RBuildingInformationModel::MomentFrame) {
			thePrototype = RBuildingInformationModel::SteelMomentFrame;
		} else if (theLoadBearingSystem == RBuildingInformationModel::BracedFrame) {
			thePrototype = RBuildingInformationModel::SteelBracedFrame;
		} else if (theLoadBearingSystem == RBuildingInformationModel::LightFrame) {
			thePrototype = RBuildingInformationModel::SteelLightFrame;
		} else if (theLoadBearingSystem == RBuildingInformationModel::MasonryWall) {
			thePrototype = RBuildingInformationModel::SteelFrameWithMasonryInfillWall;
		} else {
			ok = false;
		}
	} else if (theMaterial == RBuildingInformationModel::Wood) {
		if (theLoadBearingSystem == RBuildingInformationModel::MomentFrame) {
			thePrototype = RBuildingInformationModel::WoodLargeFrame;
		} else if (theLoadBearingSystem == RBuildingInformationModel::LightFrame) {
			thePrototype = RBuildingInformationModel::WoodLightFrame;
		} else {
			ok = false;
		}
	} else if (theMaterial == RBuildingInformationModel::ReinforcedMasonry) {
		if (theLoadBearingSystem == RBuildingInformationModel::MasonryWall) {
			thePrototype = RBuildingInformationModel::ReinforcedMasonryBearingWall;
		} else {
			ok = false;
		}
	} else if (theMaterial == RBuildingInformationModel::UnreinforcedMasonry) {
		if (theLoadBearingSystem == RBuildingInformationModel::MasonryWall) {
			thePrototype = RBuildingInformationModel::UnreinforcedMasonryBearingWall;
		} else {
			ok = false;
		}
	} else if (theMaterial == RBuildingInformationModel::MobileHome) {
		thePrototype = RBuildingInformationModel::Mobile;
	}

	if (ok == false) {
		//rCritical() << "Error in model" << objectName() << ": This combination of the material and the load bearing system is no allowed. Rt will use the default prototype.";
	}
}

void RBuildingInformationModel::computeHeightAndArea()
{	
	if (theNumberOfStoreysVector.count() != theFootprintAreaVector.count()) {
		//rCritical() << "Error in model" << objectName() << ": The number of items passed to the \"Number of Storeys\" and \"Footprint Area\" must be equal. The infromation on the total area and the height is not updated";
		return;
	}
	theNumberOfStoreys = 0;
	theTotalArea = 0;
	for (int i = 0; i < theNumberOfStoreysVector.count(); i++) {
		theNumberOfStoreys += theNumberOfStoreysVector[i];
		theTotalArea += theNumberOfStoreysVector[i] * theFootprintAreaVector[i];
	}
	theHeight = theNumberOfStoreys * theStoryHeight;
}

int RBuildingInformationModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
{	
	return 0;
}

void RBuildingInformationModel::updateHistoryVariables(int threadID)
{
	computeCodeLevel(true, threadID);
}

void RBuildingInformationModel::resetHistoryVariables(int threadID)
{
	computeCodeLevel(this->getRetrofitted(), threadID);
}

void RBuildingInformationModel::initializeForParallelComputation(int threadID){

	// initialize in superclass
	RModel::initializeForParallelComputation(threadID);
		
	theCodeLevel[threadID] = theCodeLevel[0];
}

bool RBuildingInformationModel::canBeRunParallelly(){
	return true;
}
