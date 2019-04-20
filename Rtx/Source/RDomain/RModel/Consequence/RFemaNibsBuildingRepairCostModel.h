#ifndef RFemaNibsBuildingRepairCostModel_H
#define RFemaNibsBuildingRepairCostModel_H

#include "RModel.h"

class RParameter;
class RResponse;

class RFemaNibsBuildingRepairCostModel : public RModel
{
	Q_OBJECT

	// Define a QProperty for each input (parameter) data member. The type of the QProperty is "QObject *"
	Q_PROPERTY(RBuildingType BuildingType READ getBuildingType WRITE setBuildingType)
	Q_PROPERTY(ROccupancyClass OccupancyClass READ getOccupancyClass WRITE setOccupancyClass)
	Q_PROPERTY(RCodeDesignLevel CodeDesignLevel READ getCodeDesignLevel WRITE setCodeDesignLevel)
	Q_PROPERTY(QObject *Sa1 READ getSa1 WRITE setSa1)
	Q_PROPERTY(QObject *Sa2 READ getSa2 WRITE setSa2)
	Q_PROPERTY(QObject *Magnitude READ getMagnitude WRITE setMagnitude)
	Q_PROPERTY(double NumberOfStoreys READ getNumberOfStoreys WRITE setNumberOfStoreys)
	Q_PROPERTY(double FootprintArea READ getFootprintArea WRITE setFootprintArea)
	Q_PROPERTY(QObject *RepairStatus READ getRepairStatus WRITE setRepairStatus)

	Q_ENUMS(RBuildingType)
	Q_ENUMS(ROccupancyClass)
	Q_ENUMS(RCodeDesignLevel)

public:
	// Define the constructor and destructor
	RFemaNibsBuildingRepairCostModel(QObject *parent, QString name);
	~RFemaNibsBuildingRepairCostModel();

	enum RBuildingType{W1, W2, S1L, S1M, S1H, S2L, S2M, S2H, S3, S4L, S4M, S4H, S5L, S5M, S5H, C1L, C1M, C1H, C2L, C2M, C2H, C3L, C3M, C3H, PC1, PC2L, PC2M, PC2H, RM1L, RM1M, RM2L, RM2M, RM2H, URML, URMM, MH};
	enum ROccupancyClass{RES1, RES2, RES3A, RES3B, RES3C, RES3D, RES3E, RES3F, RES4, RES5, RES6, COM1, COM2, COM3, COM4, COM5, COM6, COM7, COM8, COM9, COM10, IND1, IND2, IND3, IND4, IND5, IND6, AGR1, REL1, GOV1, GOV2, EDU1, EDU2};
	enum RCodeDesignLevel{High, Moderate, Low, Pre};

	// Define the mandatory read/write methods for all QProperties
	QObject *getMagnitude() const;
	void setMagnitude(QObject *value);
	QObject *getSa1() const;
	void setSa1(QObject *value);
	QObject *getSa2() const;
	void setSa2(QObject *value);
	RBuildingType getBuildingType() const;
	void setBuildingType(RBuildingType value);
	ROccupancyClass getOccupancyClass() const;
	void setOccupancyClass(ROccupancyClass value);
	RCodeDesignLevel getCodeDesignLevel() const;
	void setCodeDesignLevel(RCodeDesignLevel value);
	double getFootprintArea() const;
	void setFootprintArea(double value);
	double getNumberOfStoreys() const;
	void setNumberOfStoreys(double value);
	QObject *getRepairStatus() const;
	void setRepairStatus(QObject *value);

	// Define the method where the actual model is coded
	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID);

	// returns true as this model is parallelized
	bool canBeRunParallelly();

private:
	// Declare input data members
	RPointer<RParameter> theSa1;
	RPointer<RParameter> theSa2;
	RPointer<RParameter> theMagnitude;
	RBuildingType theBuildingType;
	ROccupancyClass theOccupancyClass;
	RCodeDesignLevel theCodeDesignLevel;
	double theFootprintArea;
	double theNumberOfStoreys;
	RPointer<RParameter> theRepairStatus;

	
	// Declare responses
	RResponse *theDamageFactor;

};

#endif // RFemaNibsBuildingRepairCostModel_H
