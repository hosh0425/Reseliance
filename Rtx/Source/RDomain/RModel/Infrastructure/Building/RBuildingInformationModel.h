#ifndef RBuildingInformationModel_H
#define RBuildingInformationModel_H

#include "RModel.h"

class RParameter;
class RResponse;

class RBuildingInformationModel : public RModel
{
	Q_OBJECT
	Q_PROPERTY(RRegion Region READ getRegion WRITE setRegion)

	Q_PROPERTY(QObject *Location READ getLocation WRITE setLocation)
	Q_PROPERTY(QObject *TimeOfConstruction READ getTimeOfConstruction WRITE setTimeOfConstruction)
	Q_PROPERTY(RConstructionQuality ConstructionQuality READ getConstructionQuality WRITE setConstructionQuality)
	Q_PROPERTY(RLoadBearingSystem LoadBearingSystem READ getLoadBearingSystem WRITE setLoadBearingSystem)
	Q_PROPERTY(RMaterial Material READ getMaterial WRITE setMaterial)
	Q_PROPERTY(QString NumberOfStoreys READ getNumberOfStoreys WRITE setNumberOfStoreys)
	Q_PROPERTY(double StoryHeight READ getStoryHeight WRITE setStoryHeight)
	Q_PROPERTY(QString FootprintArea READ getFootprintArea WRITE setFootprintArea)
	Q_PROPERTY(ROccupancyType OccupancyType READ getOccupancyType WRITE setOccupancyType)
	Q_PROPERTY(RPlanShape PlanShape READ getPlanShape WRITE setPlanShape)
	Q_PROPERTY(bool PlanIrregularity READ getPlanIrregularity WRITE setPlanIrregularity)
	Q_PROPERTY(bool VerticalIrregularity READ getVerticalIrregularity WRITE setVerticalIrregularity)
	Q_PROPERTY(bool SoftStorey READ getSoftStorey WRITE setSoftStorey)
	Q_PROPERTY(bool Opening READ getOpening WRITE setOpening)
	Q_PROPERTY(bool ShortColumn READ getShortColumn WRITE setShortColumn)
	Q_PROPERTY(bool Pounding READ getPounding WRITE setPounding)
	Q_PROPERTY(bool Retrofitted READ getRetrofitted WRITE setRetrofitted)
	Q_PROPERTY(bool Basement READ getBasement WRITE setBasement)
	Q_PROPERTY(bool Balcony READ getBalcony WRITE setBalcony)
	
	Q_PROPERTY(RPrototype Prototype READ getPrototype)
	Q_PROPERTY(RCodeLevel CodeLevel READ getCodeLevel)
	Q_PROPERTY(double Height READ getHeight)
	Q_PROPERTY(double TotalArea READ getTotalArea)

	Q_ENUMS(RRegion)
	Q_ENUMS(RConstructionQuality)

	Q_ENUMS(RLoadBearingSystem)
	Q_ENUMS(RMaterial)
	Q_ENUMS(ROccupancyType)
	Q_ENUMS(RPlanShape)
	Q_ENUMS(RCodeLevel)
	Q_ENUMS(RPrototype)

public:
	RBuildingInformationModel(QObject *parent, QString name);
	~RBuildingInformationModel();

	enum RRegion { Iran, US };
	enum RConstructionQuality {Superior, Ordinary, Inferior};

	enum RLoadBearingSystem{ShearWall, MomentFrame, BracedFrame, LightFrame, MasonryWall};
	enum RMaterial{ReinforcedConcrete, PrecastConcrete, Steel, Wood, ReinforcedMasonry, UnreinforcedMasonry, MobileHome};
	enum ROccupancyType{SingleResidence, MultiResidence, InstitutionalResidence, Trade, Services, Medical, Recreation, Parking, School, University, Government, Industry, Agriculture};
	enum RPlanShape{Rectangular, LShape, TShape, EShape, CShape, ZShape};
	enum RCodeLevel{PreCode, LowCode, ModerateCode, HighCode};
	enum RPrototype{ConcreteShearWall, ConcreteMomentFrame, ConcreteFrameWithMasonryInfillWall, PrecastFrameWithShearWall, SteelFrameWithConcreteShearWall, SteelMomentFrame, SteelBracedFrame, SteelLightFrame, SteelFrameWithMasonryInfillWall, WoodLargeFrame, WoodLightFrame, ReinforcedMasonryBearingWall, UnreinforcedMasonryBearingWall, Mobile};

	RRegion getRegion() const;
	void setRegion(RRegion value);
	RConstructionQuality getConstructionQuality() const;
	void setConstructionQuality(RConstructionQuality value);

	QObject *getLocation() const;
	void setLocation(QObject *value);
	QObject *getTimeOfConstruction() const;
	void setTimeOfConstruction(QObject *value);
	RLoadBearingSystem getLoadBearingSystem() const;
	void setLoadBearingSystem(RLoadBearingSystem value);
	RMaterial getMaterial() const;
	void setMaterial(RMaterial value);
	QString getNumberOfStoreys();
	void setNumberOfStoreys(QString value);
	double getStoryHeight() const;
    void setStoryHeight(double value);
	QString getFootprintArea();
	void setFootprintArea(QString value);
	ROccupancyType getOccupancyType() const;
	void setOccupancyType(ROccupancyType value);
	RPlanShape getPlanShape() const;
	void setPlanShape(RPlanShape value);
	bool getPlanIrregularity() const;
	void setPlanIrregularity(bool value);
	bool getVerticalIrregularity() const;
	void setVerticalIrregularity(bool value);
	bool getSoftStorey() const;
	void setSoftStorey(bool value);
	bool getOpening() const;
	void setOpening(bool value);
	bool getShortColumn() const;
	void setShortColumn(bool value);
	bool getPounding() const;
	void setPounding(bool value);
	bool getRetrofitted() const;
	void setRetrofitted(bool value);
	bool getBasement() const;
	void setBasement(bool value);
	bool getBalcony() const;
	void setBalcony(bool value);

	// Services
	QVector<double> getNumberOfStoreysVector() const;
	QVector<double> getFootprintAreaVector() const;
	double getTotalNumberOfStoreys() const;
	double getTotalArea() const;
	RCodeLevel getCodeLevel(int threadID = 0);
	void setCodeLevel(RCodeLevel value);
	RPrototype getPrototype() const;
	double getHeight() const;

	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID);
	void updateHistoryVariables(int threadID);
	void resetHistoryVariables(int threadID);

	// Copies the value at index 0 to the index of the given threadID for data members that are changed to array due to parallelization. Each subclass must also call the method of their superclass
	void initializeForParallelComputation(int threadID);

	// returns true as this model is parallelized
	bool canBeRunParallelly();

private:

	void computeHeightAndArea();
	void computeCodeLevel(bool retrofitted, int threadID);
	void computePrototype();

	RRegion theRegion;
	RConstructionQuality theConstructionQuality;

	RPointer<RParameter> theLocation;
	RPointer<RParameter> theTimeOfConstruction;
	RLoadBearingSystem theLoadBearingSystem;
	RMaterial theMaterial;
	QVector<double> theNumberOfStoreysVector;
	double theStoryHeight;
	QVector<double> theFootprintAreaVector;
	ROccupancyType theOccupancyType;
	RPlanShape thePlanShape;
	bool thePlanIrregularity;
	bool theVerticalIrregularity;
	bool theSoftStorey;
	bool theOpening;
	bool theShortColumn;
	bool thePounding;
	bool theRetrofitted;
	bool theBasement;
	bool theBalcony;

	double theNumberOfStoreys;
	double theTotalArea;
	double theHeight;
	RCodeLevel * theCodeLevel;
	RPrototype thePrototype;

};

#endif // RBuildingInformationModel_H
