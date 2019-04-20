#include "RFemaNibsBuildingRepairCostModel.h"

#include "RParameter.h"
#include "RResponse.h"
#include "RContinuousRandomVariable.h"

#include <math.h>

//#include <QPlainTextEdit>

#include <gsl/gsl_math.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_cdf.h>

RFemaNibsBuildingRepairCostModel::RFemaNibsBuildingRepairCostModel(QObject *parent, QString name)
	: RModel(parent, name)
{
	// Initializing the pointer data members
	//$$ theSa1 = 0;
	//$$ theSa2 = 0;
	//$$ theMagnitude = 0;
	theBuildingType = RFemaNibsBuildingRepairCostModel::C2H;
	theOccupancyClass = RFemaNibsBuildingRepairCostModel::RES1;
	theCodeDesignLevel = RFemaNibsBuildingRepairCostModel::Moderate;
	theFootprintArea = 0;
	theNumberOfStoreys = 0;
	
	// Instantiating the output response
	theDomain->createObject(objectName() + "Response", "RGenericResponse");
	// Setting response pointer
	theDamageFactor = qobject_cast<RResponse *>(theDomain->getLastAddedObject());
	// Setting the "model" property of theResponse1 to be "this" object
	theDamageFactor->setModel(this);
}

RFemaNibsBuildingRepairCostModel::~RFemaNibsBuildingRepairCostModel()
{

}

QObject * RFemaNibsBuildingRepairCostModel::getSa1() const	
{	
	return theSa1.data();
}	
	
void RFemaNibsBuildingRepairCostModel::setSa1(QObject *value)	
{	
	theSa1 = qobject_cast<RParameter *>(value);
}	
	
QObject * RFemaNibsBuildingRepairCostModel::getSa2() const	
{	
	return theSa2.data();
}	
	
void RFemaNibsBuildingRepairCostModel::setSa2(QObject *value)	
{	
	theSa2 = qobject_cast<RParameter *>(value);
}
	
RFemaNibsBuildingRepairCostModel::RBuildingType RFemaNibsBuildingRepairCostModel::getBuildingType() const	
{	
	return theBuildingType;
}	
	
void RFemaNibsBuildingRepairCostModel::setBuildingType(RFemaNibsBuildingRepairCostModel::RBuildingType value)	
{	
	theBuildingType = value;
}	
	
RFemaNibsBuildingRepairCostModel::ROccupancyClass RFemaNibsBuildingRepairCostModel::getOccupancyClass() const	
{	
	return theOccupancyClass;
}	
	
void RFemaNibsBuildingRepairCostModel::setOccupancyClass(RFemaNibsBuildingRepairCostModel::ROccupancyClass value)	
{	
	theOccupancyClass = value;
}	
	
RFemaNibsBuildingRepairCostModel::RCodeDesignLevel RFemaNibsBuildingRepairCostModel::getCodeDesignLevel() const	
{	
	return theCodeDesignLevel;
}	
	
void RFemaNibsBuildingRepairCostModel::setCodeDesignLevel(RFemaNibsBuildingRepairCostModel::RCodeDesignLevel value)	
{	
	theCodeDesignLevel = value;
}
	
QObject * RFemaNibsBuildingRepairCostModel::getMagnitude() const	
{	
	return theMagnitude.data();
}	
	
void RFemaNibsBuildingRepairCostModel::setMagnitude(QObject *value)	
{	
	theMagnitude = qobject_cast<RParameter *>(value);
}	
	
double RFemaNibsBuildingRepairCostModel::getFootprintArea() const	
{	
	return theFootprintArea;
}	
	
void RFemaNibsBuildingRepairCostModel::setFootprintArea(double value)	
{	
	theFootprintArea = value;
}	
	
double RFemaNibsBuildingRepairCostModel::getNumberOfStoreys() const	
{	
	return theNumberOfStoreys;
}	
	
void RFemaNibsBuildingRepairCostModel::setNumberOfStoreys(double value)	
{	
	theNumberOfStoreys = value;
}	

QObject * RFemaNibsBuildingRepairCostModel::getRepairStatus() const
{
	return theRepairStatus.data();
}

void RFemaNibsBuildingRepairCostModel::setRepairStatus(QObject *value)
{
	theRepairStatus = qobject_cast<RParameter *>(value);
}

int RFemaNibsBuildingRepairCostModel::evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID)
{	
	double magnitude = theMagnitude.data()->getCurrentValue(threadID);
	double Sa1 = theSa1.data()->getCurrentValue(threadID);
	double Sa0_3 = theSa2.data()->getCurrentValue(threadID);

	double kappa;
	double Dy;
	double Ay;
	double Du;
	double Au;
	double betaE;
	double period;
	QVector<double> SDBarStrVector;
	QVector<double> SDStdvStrVector;
	QVector<double> SDBarNonstrDriftVector;
	QVector<double> SDStdvNonstrDriftVector;
	QVector<double> SABarNonstrAccVector;
	QVector<double> SAStdvNonstrAccVector;
	QVector<double> PGABarVector;
	QVector<double> PGAStdvVector;


	switch (theBuildingType){

	case RFemaNibsBuildingRepairCostModel::W1 :{
		betaE = 0.125;
		period = 0.35;
		if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::High) {
			// According to FEMA-NIBS document: 
			//		-- Before 1940: Pre
			//		-- Between 1940-1973: Moderate
			//		-- After 1973: High

			Dy = 0.48;
			Ay = 0.4;
			Du = 11.51;
			Au = 1.2;

			SDBarStrVector << 0.5 << 1.51 << 5.04 << 12.6;
			SDStdvStrVector << 0.8 << 0.81 << 0.85 << 0.97;

			SDBarNonstrDriftVector << 0.5 << 1.01 << 3.15 << 6.3;
			SDStdvNonstrDriftVector << 0.85 << 0.88 << 0.88 << 0.94;

			SABarNonstrAccVector << 0.3 << 0.6 << 1.2 << 2.4;
			SAStdvNonstrAccVector << 0.73 << 0.68 << 0.68 << 0.68;

			PGABarVector << 0.26 << 0.55 << 1.28 << 2.01;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 1;
			} else if (magnitude <= 7.5) {
				kappa = 0.8;
			} else {
				kappa = 0.5;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Moderate) {
			Dy = 0.36;
			Ay = 0.3;
			Du = 6.48;
			Au = 0.9;

			SDBarStrVector << 0.5 << 1.25 << 3.86 << 9.45;
			SDStdvStrVector << 0.84 << 0.86 << 0.89 << 1.04;

			SDBarNonstrDriftVector << 0.5 << 1.01 << 3.15 << 6.3 ;
			SDStdvNonstrDriftVector << 0.89 << 0.91 << 0.9 << 1.04 ;

			SABarNonstrAccVector << 0.25 << 0.5 << 1 << 2 ;
			SAStdvNonstrAccVector << 0.73 << 0.68 << 0.67 << 0.64 ;

            PGABarVector << 0.24 << 0.43 << 0.91 << 1.34;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.9;
			} else if (magnitude <= 7.5) {
				kappa = 0.6;
			} else {
				kappa = 0.3;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Low) {
			Dy = 0.24;
			Ay = 0.2;
			Du = 4.32;
			Au = 0.6;

			SDBarStrVector << 0.5 << 1.25 << 3.86 << 9.45;
			SDStdvStrVector << 0.93 << 0.98 << 1.02 << 0.99;

			SDBarNonstrDriftVector << 0.5 << 1.01 << 3.15 << 6.3 ;
			SDStdvNonstrDriftVector << 0.98 << 0.99 << 1.02 << 1.09 ;

			SABarNonstrAccVector << 0.2 << 0.4 << 0.8 << 1.6 ;
			SAStdvNonstrAccVector << 0.71 << 0.68 << 0.66 << 0.66 ;


			PGABarVector << 0.20 << 0.34 << 0.61 << 0.95;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.7;
			} else if (magnitude <= 7.5) {
				kappa = 0.4;
			} else {
				kappa = 0.2;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Pre) {
			Dy = 0.24;
			Ay = 0.2;
			Du = 4.32;
			Au = 0.6;

			SDBarStrVector << 0.4 << 1 << 3.09 << 7.56;
			SDStdvStrVector << 1.01 << 1.05 << 1.07 << 1.06;

			SDBarNonstrDriftVector << 0.5 << 1.01 << 3.15 << 6.3 ;
			SDStdvNonstrDriftVector << 1.07 << 1.11 << 1.11 << 1.14 ;

			SABarNonstrAccVector << 0.2 << 0.4 << 0.8 << 1.6 ;
			SAStdvNonstrAccVector << 0.72 << 0.7 << 0.67 << 0.67 ;

			PGABarVector << 0.18 << 0.29 << 0.51 << 0.77;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.5;
			} else if (magnitude <= 7.5) {
				kappa = 0.3;
			} else {
				kappa = 0.1;
			}
		}

		break;
	}

 	case RFemaNibsBuildingRepairCostModel::W2 : {
		betaE = 0.125;
		period = 0.4;
		if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::High) {
			Dy = 0.63;
			Ay = 0.4;
			Du = 12.53;
			Au = 1;

			SDBarStrVector << 0.86 << 2.59 << 8.64 << 21.6;
			SDStdvStrVector << 0.81 << 0.88 << 0.9 << 0.83;

            SDBarNonstrDriftVector << 0.86 << 1.73 << 5.4 << 10.8 ;
			SDStdvNonstrDriftVector << 0.87 << 0.89 << 0.96 << 0.94 ;

			SABarNonstrAccVector << 0.3 << 0.6 << 1.2 << 2.4 ;
			SAStdvNonstrAccVector << 0.7 << 0.67 << 0.67 << 0.68 ;

			PGABarVector << 0.26 << 0.56 << 1.15 << 2.08;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.9;
			} else if (magnitude <= 7.5) {
				kappa = 0.6;
			} else {
				kappa = 0.4;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Moderate) {
			Dy = 0.31;
			Ay = 0.2;
			Du = 4.7;
			Au = 0.5;

			SDBarStrVector << 0.86 << 2.14 << 6.62 << 16.2;
			SDStdvStrVector << 0.89 << 0.95 << 0.95 << 0.92;

			SDBarNonstrDriftVector << 0.86 << 1.73 << 5.4 << 10.8 ;
			SDStdvNonstrDriftVector << 0.94 << 0.99 << 1 << 0.9 ;

			SABarNonstrAccVector << 0.25 << 0.5 << 1 << 2 ;
			SAStdvNonstrAccVector << 0.68 << 0.67 << 0.68 << 0.68 ;

			PGABarVector << 0.20 << 0.35 << 0.64 << 1.13;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.8;
			} else if (magnitude <= 7.5) {
				kappa = 0.4;
			} else {
				kappa = 0.2;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Low) {
			Dy = 0.16;
			Ay = 0.1;
			Du = 2.35;
			Au = 0.25;

			SDBarStrVector << 0.86 << 2.14 << 6.62 << 16.2;
			SDStdvStrVector << 0.97 << 0.9 << 0.89 << 0.99;

			SDBarNonstrDriftVector << 0.86 << 1.73 << 5.4 << 10.8 ;
			SDStdvNonstrDriftVector << 1.01 << 0.97 << 0.93 << 1.03 ;

			SABarNonstrAccVector << 0.2 << 0.4 << 0.8 << 1.6 ;
			SAStdvNonstrAccVector << 0.67 << 0.67 << 0.7 << 0.7 ;

			PGABarVector << 0.14 << 0.23 << 0.48 << 0.75;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.6;
			} else if (magnitude <= 7.5) {
				kappa = 0.3;
			} else {
				kappa = 0.1;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Pre) {
			Dy = 0.16;
			Ay = 0.1;
			Du = 2.35;
			Au = 0.25;

			SDBarStrVector << 0.69 << 1.71 << 5.29 << 12.96;
			SDStdvStrVector << 1.04 << 0.97 << 0.9 << 0.99;

			SDBarNonstrDriftVector << 0.86 << 1.73 << 5.4 << 10.8 ;
			SDStdvNonstrDriftVector << 1.06 << 1 << 0.93 << 1.01 ;

			SABarNonstrAccVector << 0.2 << 0.4 << 0.8 << 1.6 ;
			SAStdvNonstrAccVector << 0.66 << 0.67 << 0.65 << 0.65 ;

			PGABarVector << 0.12 << 0.19 << 0.37 << 0.60;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.4;
			} else if (magnitude <= 7.5) {
				kappa = 0.2;
			} else {
				kappa = 0;
			}
		}

		break;
	}

 	case RFemaNibsBuildingRepairCostModel::S1L : {
		betaE = 0.06;
		period = 0.5;
		if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::High) {
			Dy = 0.61;
			Ay = 0.25;
			Du = 14.67;
			Au = 0.749;

			SDBarStrVector << 1.3 << 2.59 << 6.48 << 17.28;
			SDStdvStrVector << 0.8 << 0.76 << 0.69 << 0.72;

            SDBarNonstrDriftVector << 0.86 << 1.73 << 5.4 << 10.8 ;
			SDStdvNonstrDriftVector << 0.81 << 0.85 << 0.77 << 0.77 ;

			SABarNonstrAccVector << 0.3 << 0.6 << 1.2 << 2.4 ;
			SAStdvNonstrAccVector << 0.67 << 0.67 << 0.68 << 0.67 ;

			PGABarVector << 0.19 << 0.31 << 0.64 << 1.49;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.9;
			} else if (magnitude <= 7.5) {
				kappa = 0.6;
			} else {
				kappa = 0.4;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Moderate) {
			Dy = 0.31;
			Ay = 0.125;
			Du = 5.5;
			Au = 0.375;

			SDBarStrVector << 1.3 << 2.24 << 5.08 << 12.96;
			SDStdvStrVector << 0.8 << 0.75 << 0.74 << 0.88;

			SDBarNonstrDriftVector << 0.86 << 1.73 << 5.4 << 10.8 ;	
			SDStdvNonstrDriftVector << 0.84 << 0.83 << 0.79 << 0.87 ;

			SABarNonstrAccVector << 0.25 << 0.5 << 1 << 2 ;
			SAStdvNonstrAccVector << 0.67 << 0.66 << 0.67 << 0.67 ;

			PGABarVector << 0.15 << 0.22 << 0.42 << 0.80;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;


			if (magnitude <= 5.5) {
				kappa = 0.8;
			} else if (magnitude <= 7.5) {
				kappa = 0.4;
			} else {
				kappa = 0.2;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Low) {
			Dy = 0.15;
			Ay = 0.062;
			Du = 2.29;
			Au = 0.187;

			SDBarStrVector << 1.3 << 2.07 << 4.38 << 10.8;
			SDStdvStrVector << 0.77 << 0.78 << 0.78 << 0.96;

			SDBarNonstrDriftVector << 0.86 << 1.73 << 5.4 << 10.8 ;
			SDStdvNonstrDriftVector << 0.86 << 0.84 << 0.88 << 1 ;

			SABarNonstrAccVector << 0.2 << 0.4 << 0.8 << 1.6 ;
			SAStdvNonstrAccVector << 0.65 << 0.68 << 0.68 << 0.68 ;

			PGABarVector << 0.12 << 0.17 << 0.30 << 0.48;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.6;
			} else if (magnitude <= 7.5) {
				kappa = 0.3;
			} else {
				kappa = 0.1;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Pre) {
			Dy = 0.15;
			Ay = 0.062;
			Du = 2.75;
			Au = 0.187;

			SDBarStrVector << 1.04 << 1.65 << 3.5 << 8.64;
			SDStdvStrVector << 0.85 << 0.82 << 0.8 << 0.95;

			SDBarNonstrDriftVector << 0.86 << 1.73 << 5.4 << 10.8 ;
			SDStdvNonstrDriftVector << 0.9 << 0.87 << 0.91 << 1.02 ;

			SABarNonstrAccVector << 0.2 << 0.4 << 0.8 << 1.6 ;
			SAStdvNonstrAccVector << 0.66 << 0.68 << 0.68 << 0.68 ;

			PGABarVector << 0.09 << 0.13 << 0.22 << 0.38;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.4;
			} else if (magnitude <= 7.5) {
				kappa = 0.2;
			} else {
				kappa = 0;
			}
		}

		break;
	}

 	case RFemaNibsBuildingRepairCostModel::S1M: {
		betaE = 0.06;
		period = 1.08;
		if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::High) {
			Dy = 1.78;
			Ay = 0.156;
			Du = 28.4;
			Au = 0.468;

			SDBarStrVector << 2.16 << 4.32 << 10.8 << 28.8;
			SDStdvStrVector << 0.65 << 0.66 << 0.67 << 0.74;

			SABarNonstrAccVector << 0.3 << 0.6 << 1.2 << 2.4 ;
			SAStdvNonstrAccVector << 0.67 << 0.68 << 0.67 << 0.67 ;

			SDBarNonstrDriftVector << 2.16 << 4.32 << 13.5 << 27 ;	
			SDStdvNonstrDriftVector << 0.71 << 0.72 << 0.72 << 0.8 ;

			PGABarVector << 0.14 << 0.26 << 0.62 << 1.43;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.9;
			} else if (magnitude <= 7.5) {
				kappa = 0.6;
			} else {
				kappa = 0.4;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Moderate) {
			Dy = 0.89;
			Ay = 0.078;
			Du = 10.65;
			Au = 0.234;

			SDBarStrVector << 2.16 << 3.74 << 8.46 << 21.6;
			SDStdvStrVector << 0.65 << 0.68 << 0.69 << 0.87;

			SDBarNonstrDriftVector << 2.16 << 4.32 << 13.5 << 27 ;
			SDStdvNonstrDriftVector << 0.71 << 0.74 << 0.85 << 0.95 ;

			SABarNonstrAccVector << 0.25 << 0.5 << 1 << 2 ;
			SAStdvNonstrAccVector << 0.66 << 0.67 << 0.67 << 0.67 ;

			PGABarVector << 0.13 << 0.21 << 0.44 << 0.82;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.8;
			} else if (magnitude <= 7.5) {
				kappa = 0.4;
			} else {
				kappa = 0.2;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Low) {
			Dy = 0.44;
			Ay = 0.039;
			Du = 4.44;
			Au = 0.117;

			SDBarStrVector << 2.16 << 3.44 << 7.3 << 18;
			SDStdvStrVector << 0.68 << 0.78 << 0.85 << 0.98;

			SDBarNonstrDriftVector << 2.16 << 4.32 << 13.5 << 27 ;
			SDStdvNonstrDriftVector << 0.74 << 0.89 << 0.99 << 1.05 ;

			SABarNonstrAccVector << 0.2 << 0.4 << 0.8 << 1.6 ;
			SAStdvNonstrAccVector << 0.66 << 0.68 << 0.68 << 0.68 ;

            PGABarVector << 0.12 << 0.18 << 0.29 << 0.49;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.6;
			} else if (magnitude <= 7.5) {
				kappa = 0.3;
			} else {
				kappa = 0.1;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Pre) {
			Dy = 0.44;
			Ay = 0.039;
			Du = 5.33;
			Au = 0.117;

			SDBarStrVector << 1.73 << 2.76 << 5.84 << 14.4;
			SDStdvStrVector << 0.7 << 0.75 << 0.81 << 0.98;

			SDBarNonstrDriftVector << 2.16 << 4.32 << 13.5 << 27 ;
			SDStdvNonstrDriftVector << 0.8 << 0.92 << 0.99 << 1.06 ;

			SABarNonstrAccVector << 0.2 << 0.4 << 0.8 << 1.6 ;
			SAStdvNonstrAccVector << 0.66 << 0.68 << 0.68 << 0.68 ;

			PGABarVector << 0.09 << 0.14 << 0.23 << 0.39;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.4;
			} else if (magnitude <= 7.5) {
				kappa = 0.2;
			} else {
				kappa = 0;
			}
		}

		break;
	}

 	case RFemaNibsBuildingRepairCostModel::S1H: {
		betaE = 0.06;
		period = 2.21;
		if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::High) {
			Dy = 4.66;
			Ay = 0.098;
			Du = 55.88;
			Au = 0.293;

			SDBarStrVector << 3.37 << 6.74 << 16.85 << 44.93;
			SDStdvStrVector << 0.64 << 0.64 << 0.65 << 0.67;

			SDBarNonstrDriftVector << 4.49 << 8.99 << 28.08 << 56.16 ;	
			SDStdvNonstrDriftVector << 0.72 << 0.71 << 0.74 << 0.77 ;

			SABarNonstrAccVector << 0.3 << 0.6 << 1.2 << 2.4 ;
			SAStdvNonstrAccVector << 0.68 << 0.67 << 0.67 << 0.67 ;

			PGABarVector << 0.10 << 0.21 << 0.52 << 1.31;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.9;
			} else if (magnitude <= 7.5) {
				kappa = 0.6;
			} else {
				kappa = 0.4;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Moderate) {
			Dy = 2.33;
			Ay = 0.049;
			Du = 20.96;
			Au = 0.147;

			SDBarStrVector << 3.37 << 5.83 << 13.21 << 33.7;
			SDStdvStrVector << 0.64 << 0.64 << 0.71 << 0.83;

			SDBarNonstrDriftVector << 4.49 << 8.99 << 28.08 << 56.16 ;
			SDStdvNonstrDriftVector << 0.71 << 0.74 << 0.84 << 0.95 ;

			SABarNonstrAccVector << 0.25 << 0.5 << 1 << 2 ;
			SAStdvNonstrAccVector << 0.66 << 0.68 << 0.68 << 0.68 ;

			PGABarVector << 0.10 << 0.18 << 0.39 << 0.78;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.8;
			} else if (magnitude <= 7.5) {
				kappa = 0.4;
			} else {
				kappa = 0.2;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Low) {
			Dy = 1.16;
			Ay = 0.024;
			Du = 8.73;
			Au = 0.073;

			SDBarStrVector << 3.37 << 5.37 << 11.38 << 28.08;
			SDStdvStrVector << 0.66 << 0.7 << 0.76 << 0.92;

			SDBarNonstrDriftVector << 4.49 << 8.99 << 28.08 << 56.16 ;
			SDStdvNonstrDriftVector << 0.75 << 0.87 << 0.97 << 1.04 ;

			SABarNonstrAccVector << 0.2 << 0.4 << 0.8 << 1.6 ;
			SAStdvNonstrAccVector << 0.67 << 0.65 << 0.65 << 0.65 ;

			PGABarVector << 0.10 << 0.15 << 0.28 << 0.48;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.6;
			} else if (magnitude <= 7.5) {
				kappa = 0.3;
			} else {
				kappa = 0.1;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Pre) {
			Dy = 1.16;
			Ay = 0.024;
			Du = 10.48;
			Au = 0.073;

			SDBarStrVector << 2.7 << 4.3 << 9.11 << 22.46;
			SDStdvStrVector << 0.69 << 0.71 << 0.85 << 0.93;

			SDBarNonstrDriftVector << 4.49 << 8.99 << 28.08 << 56.16 ;
			SDStdvNonstrDriftVector << 0.79 << 0.89 << 1 << 1.07 ;

			SABarNonstrAccVector << 0.2 << 0.4 << 0.8 << 1.6 ;
			SAStdvNonstrAccVector << 0.68 << 0.68 << 0.68 << 0.68 ;

			PGABarVector << 0.08 << 0.12 << 0.22 << 0.38;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.4;
			} else if (magnitude <= 7.5) {
				kappa = 0.2;
			} else {
				kappa = 0;
			}
		}

		break;
	}

 	case RFemaNibsBuildingRepairCostModel::S2L : {
		betaE = 0.06;
		period = 0.4;
		if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::High) {
			Dy = 0.63;
			Ay = 0.4;
			Du = 10.02;
			Au = 0.8;

			SDBarStrVector << 1.08 << 2.16 << 6.48 << 17.28;
			SDStdvStrVector << 0.81 << 0.89 << 0.94 << 0.83;

			SDBarNonstrDriftVector << 0.86 << 1.73 << 5.4 << 10.8 ;	
			SDStdvNonstrDriftVector << 0.84 << 0.9 << 0.97 << 0.92 ;

			SABarNonstrAccVector << 0.3 << 0.6 << 1.2 << 2.4 ;
			SAStdvNonstrAccVector << 0.67 << 0.67 << 0.67 << 0.67 ;

			PGABarVector << 0.24 << 0.41 << 0.76 << 1.46;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.7;
			} else if (magnitude <= 7.5) {
				kappa = 0.5;
			} else {
				kappa = 0.3;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Moderate) {
			Dy = 0.31;
			Ay = 0.2;
			Du = 3.76;
			Au = 0.4;

			SDBarStrVector << 1.08 << 1.87 << 5.04 << 12.96;
			SDStdvStrVector << 0.93 << 0.92 << 0.93 << 0.93;
		
			SDBarNonstrDriftVector << 0.86 << 1.73 << 5.4 << 10.8 ;	
			SDStdvNonstrDriftVector << 0.93 << 0.99 << 0.96 << 0.92 ;

			SABarNonstrAccVector << 0.25 << 0.5 << 1 << 2 ;
			SAStdvNonstrAccVector << 0.66 << 0.66 << 0.68 << 0.68 ;

			PGABarVector << 0.20 << 0.26 << 0.46 << 0.84;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.6;
			} else if (magnitude <= 7.5) {
				kappa = 0.4;
			} else {
				kappa = 0.2;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Low) {
			Dy = 0.16;
			Ay = 0.1;
			Du = 1.57;
			Au = 0.2;

			SDBarStrVector << 1.08 << 1.73 << 4.32 << 10.8;
			SDStdvStrVector << 0.96 << 0.89 << 0.86 << 0.98;

			SDBarNonstrDriftVector << 0.86 << 1.73 << 5.4 << 10.8 ;
			SDStdvNonstrDriftVector << 1.01 << 0.94 << 0.94 << 1.03 ;

			SABarNonstrAccVector << 0.2 << 0.4 << 0.8 << 1.6 ;
			SAStdvNonstrAccVector << 0.65 << 0.68 << 0.68 << 0.68 ;

			PGABarVector << 0.13 << 0.17 << 0.30 << 0.50;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.5;
			} else if (magnitude <= 7.5) {
				kappa = 0.3;
			} else {
				kappa = 0.1;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Pre) {
			Dy = 0.16;
			Ay = 0.1;
			Du = 1.88;
			Au = 0.2;

			SDBarStrVector << 0.86 << 1.38 << 3.46 << 8.64;
			SDStdvStrVector << 1.01 << 0.96 << 0.88 << 0.98;

			SDBarNonstrDriftVector << 0.86 << 1.73 << 5.4 << 10.8 ;
			SDStdvNonstrDriftVector << 1.06 << 0.97 << 0.96 << 1.04 ;

			SABarNonstrAccVector << 0.2 << 0.4 << 0.8 << 1.6 ;
			SAStdvNonstrAccVector << 0.65 << 0.68 << 0.68 << 0.68 ;

			PGABarVector << 0.11 << 0.14 << 0.23 << 0.39;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.4;
			} else if (magnitude <= 7.5) {
				kappa = 0.2;
			} else {
				kappa = 0;
			}
		}

		break;
	}

 	case RFemaNibsBuildingRepairCostModel::S2M : {
		betaE = 0.06;
		period = 0.86;
		if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::High) {
			Dy = 2.43;
			Ay = 0.333;
			Du = 25.88;
			Au = 0.667;

			SDBarStrVector << 1.8 << 3.6 << 10.8 << 28.8;
			SDStdvStrVector << 0.67 << 0.67 << 0.68 << 0.79;

			SDBarNonstrDriftVector << 2.16 << 4.32 << 13.5 << 27 ;	
			SDStdvNonstrDriftVector << 0.71 << 0.74 << 0.74 << 0.84 ;

			SABarNonstrAccVector << 0.3 << 0.6 << 1.2 << 2.4 ;
			SAStdvNonstrAccVector << 0.69 << 0.66 << 0.66 << 0.66 ;
			
			PGABarVector << 0.14 << 0.27 << 0.73 << 1.62;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.7;
			} else if (magnitude <= 7.5) {
				kappa = 0.5;
			} else {
				kappa = 0.3;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Moderate) {
			Dy = 1.21;
			Ay = 0.167;
			Du = 9.7;
			Au = 0.333;

			SDBarStrVector << 1.8 << 3.12 << 8.4 << 21.6;
			SDStdvStrVector << 0.7 << 0.69 << 0.69 << 0.89;

			SDBarNonstrDriftVector << 2.16 << 4.32 << 13.5 << 27 ;
			SDStdvNonstrDriftVector << 0.74 << 0.74 << 0.85 << 0.96 ;

			SABarNonstrAccVector << 0.25 << 0.5 << 1 << 2 ;
			SAStdvNonstrAccVector << 0.66 << 0.65 << 0.65 << 0.65 ;

			PGABarVector << 0.14 << 0.22 << 0.53 << 0.97;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.6;
			} else if (magnitude <= 7.5) {
				kappa = 0.4;
			} else {
				kappa = 0.2;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Low) {
			Dy = 0.61;
			Ay = 0.083;
			Du = 4.04;
			Au = 0.167;

			SDBarStrVector << 1.8 << 2.88 << 7.2 << 18;
			SDStdvStrVector << 0.7 << 0.73 << 0.85 << 0.98;

			SDBarNonstrDriftVector << 2.16 << 4.32 << 13.5 << 27 ;
			SDStdvNonstrDriftVector << 0.77 << 0.87 << 0.99 << 1.05 ;

			SABarNonstrAccVector << 0.2 << 0.4 << 0.8 << 1.6 ;
			SAStdvNonstrAccVector << 0.65 << 0.68 << 0.68 << 0.68 ;

			PGABarVector << 0.12 << 0.18 << 0.35 << 0.58;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.5;
			} else if (magnitude <= 7.5) {
				kappa = 0.3;
			} else {
				kappa = 0.1;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Pre) {
			Dy = 0.61;
			Ay = 0.083;
			Du = 4.85;
			Au = 0.167;

			SDBarStrVector << 1.44 << 2.3 << 5.76 << 14.4;
			SDStdvStrVector << 0.73 << 0.75 << 0.8 << 0.98;

			SDBarNonstrDriftVector << 2.16 << 4.32 << 13.5 << 27 ;
			SDStdvNonstrDriftVector << 0.8 << 0.9 << 1.02 << 1.06 ;

			SABarNonstrAccVector << 0.2 << 0.4 << 0.8 << 1.6 ;
			SAStdvNonstrAccVector << 0.65 << 0.68 << 0.68 << 0.68 ;

			PGABarVector << 0.10 << 0.14 << 0.28 << 0.47;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.4;
			} else if (magnitude <= 7.5) {
				kappa = 0.2;
			} else {
				kappa = 0;
			}
		}

		break;
	}

 	case RFemaNibsBuildingRepairCostModel::S2H : {
		betaE = 0.06;
		period = 1.77;
		if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::High) {
			Dy = 7.75;
			Ay = 0.254;
			Du = 61.97;
			Au = 0.508;

			SDBarStrVector << 2.81 << 5.62 << 16.85 << 44.93;
			SDStdvStrVector << 0.63 << 0.63 << 0.64 << 0.71;

			SDBarNonstrDriftVector << 4.49 << 8.99 << 28.08 << 56.16 ;	
			SDStdvNonstrDriftVector << 0.71 << 0.71 << 0.72 << 0.78 ;

			SABarNonstrAccVector << 0.3 << 0.6 << 1.2 << 2.4 ;
			SAStdvNonstrAccVector << 0.68 << 0.66 << 0.65 << 0.65 ;

			PGABarVector << 0.11 << 0.22 << 0.65 << 1.60;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.7;
			} else if (magnitude <= 7.5) {
				kappa = 0.5;
			} else {
				kappa = 0.3;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Moderate) {
			Dy = 3.87;
			Ay = 0.127;
			Du = 23.24;
			Au = 0.254;

			SDBarStrVector << 2.81 << 4.87 << 13.1 << 33.7;
			SDStdvStrVector << 0.66 << 0.64 << 0.69 << 0.8;

			SDBarNonstrDriftVector << 4.49 << 8.99 << 28.08 << 56.16 ;
			SDStdvNonstrDriftVector << 0.72 << 0.73 << 0.8 << 0.94 ;

			SABarNonstrAccVector << 0.25 << 0.5 << 1 << 2 ;
			SAStdvNonstrAccVector << 0.65 << 0.65 << 0.65 << 0.65 ;

			PGABarVector << 0.11 << 0.19 << 0.49 << 1.02;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.6;
			} else if (magnitude <= 7.5) {
				kappa = 0.4;
			} else {
				kappa = 0.2;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Low) {
			Dy = 1.94;
			Ay = 0.063;
			Du = 9.68;
			Au = 0.127;

			SDBarStrVector << 2.81 << 4.49 << 11.23 << 28.08;
			SDStdvStrVector << 0.66 << 0.67 << 0.74 << 0.92;

			SDBarNonstrDriftVector << 4.49 << 8.99 << 28.08 << 56.16 ;
			SDStdvNonstrDriftVector << 0.74 << 0.86 << 0.97 << 1.05 ;

			SABarNonstrAccVector << 0.2 << 0.4 << 0.8 << 1.6 ;
			SAStdvNonstrAccVector << 0.65 << 0.68 << 0.68 << 0.68 ;

			PGABarVector << 0.11 << 0.17 << 0.36 << 0.63;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.5;
			} else if (magnitude <= 7.5) {
				kappa = 0.3;
			} else {
				kappa = 0.1;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Pre) {
			Dy = 1.94;
			Ay = 0.063;
			Du = 11.62;
			Au = 0.127;

			SDBarStrVector << 2.25 << 3.59 << 8.99 << 22.46;
			SDStdvStrVector << 0.7 << 0.7 << 0.84 << 0.91;

			SDBarNonstrDriftVector << 4.49 << 8.99 << 28.08 << 56.16 ;
			SDStdvNonstrDriftVector << 0.79 << 0.89 << 0.99 << 1.06 ;

			SABarNonstrAccVector << 0.2 << 0.4 << 0.8 << 1.6 ;
			SAStdvNonstrAccVector << 0.65 << 0.67 << 0.67 << 0.67 ;

			PGABarVector << 0.09 << 0.13 << 0.29 << 0.50;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.4;
			} else if (magnitude <= 7.5) {
				kappa = 0.2;
			} else {
				kappa = 0;
			}
		}

		break;
	}

 	case RFemaNibsBuildingRepairCostModel::S3 : {
		betaE = 0.06;
		period = 0.4;
		if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::High) {
			Dy = 0.63;
			Ay = 0.4;
			Du = 10.02;
			Au = 0.8;

			SDBarStrVector << 0.54 << 1.08 << 3.24 << 9.45;
			SDStdvStrVector << 0.81 << 0.82 << 0.91 << 0.9;

			SDBarNonstrDriftVector << 0.54 << 1.08 << 3.38 << 6.75 ;	
			SDStdvNonstrDriftVector << 0.86 << 0.88 << 0.98 << 0.98 ;

			SABarNonstrAccVector << 0.3 << 0.6 << 1.2 << 2.4 ;
			SAStdvNonstrAccVector << 0.68 << 0.67 << 0.67 << 0.67 ;

			PGABarVector << 0.15 << 0.26 << 0.54 << 1.00;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.7;
			} else if (magnitude <= 7.5) {
				kappa = 0.5;
			} else {
				kappa = 0.3;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Moderate) {
			Dy = 0.31;
			Ay = 0.2;
			Du = 3.76;
			Au = 0.4;

			SDBarStrVector << 0.54 << 0.94 << 2.52 << 7.09;
			SDStdvStrVector << 0.88 << 0.92 << 0.97 << 0.89;

			SDBarNonstrDriftVector << 0.54 << 1.08 << 3.38 << 6.75 ;	
			SDStdvNonstrDriftVector << 0.93 << 0.98 << 1.01 << 0.94 ;

			SABarNonstrAccVector << 0.25 << 0.5 << 1 << 2 ;
			SAStdvNonstrAccVector << 0.67 << 0.66 << 0.65 << 0.65 ;

			PGABarVector << 0.13 << 0.19 << 0.33 << 0.60;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.6;
			} else if (magnitude <= 7.5) {
				kappa = 0.4;
			} else {
				kappa = 0.2;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Low) {
			Dy = 0.16;
			Ay = 0.1;
			Du = 1.57;
			Au = 0.2;

			SDBarStrVector << 0.54 << 0.87 << 2.17 << 5.91;
			SDStdvStrVector << 0.98 << 0.99 << 1.01 << 0.9;

			SDBarNonstrDriftVector << 0.54 << 1.08 << 3.38 << 6.75 ;
			SDStdvNonstrDriftVector << 1.03 << 1.02 << 0.96 << 0.99 ;

			SABarNonstrAccVector << 0.2 << 0.4 << 0.8 << 1.6 ;
			SAStdvNonstrAccVector << 0.65 << 0.68 << 0.68 << 0.68 ;

			PGABarVector << 0.10 << 0.13 << 0.20 << 0.38;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.5;
			} else if (magnitude <= 7.5) {
				kappa = 0.3;
			} else {
				kappa = 0.1;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Pre) {
			Dy = 0.16;
			Ay = 0.1;
			Du = 1.88;
			Au = 0.2;

			SDBarStrVector << 0.43 << 0.69 << 1.73 << 4.73;
			SDStdvStrVector << 1.06 << 1.03 << 1.07 << 0.89;

	
			SDBarNonstrDriftVector << 0.54 << 1.08 << 3.38 << 6.75 ;
			SDStdvNonstrDriftVector << 1.11 << 1.05 << 0.96 << 1 ;

			SABarNonstrAccVector << 0.2 << 0.4 << 0.8 << 1.6 ;
			SAStdvNonstrAccVector << 0.65 << 0.68 << 0.68 << 0.68 ;



			PGABarVector << 0.08 << 0.10 << 0.16 << 0.30;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.4;
			} else if (magnitude <= 7.5) {
				kappa = 0.2;
			} else {
				kappa = 0;
			}
		}

		break;
	}

 	case RFemaNibsBuildingRepairCostModel::S4L : {
		betaE = 0.06;
		period = 0.35;
		if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::High) {
			Dy = 0.38;
			Ay = 0.32;
			Du = 6.91;
			Au = 0.72;

			SDBarStrVector << 0.86 << 1.73 << 5.18 << 15.12;
			SDStdvStrVector << 0.89 << 0.89 << 0.98 << 0.87;

			SDBarNonstrDriftVector << 0.86 << 1.73 << 5.4 << 10.8 ;	
			SDStdvNonstrDriftVector << 0.93 << 0.94 << 1.01 << 0.99 ;

			SABarNonstrAccVector << 0.3 << 0.6 << 1.2 << 2.4 ;
			SAStdvNonstrAccVector << 0.68 << 0.68 << 0.67 << 0.67 ;

			PGABarVector << 0.24 << 0.39 << 0.71 << 1.33;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.7;
			} else if (magnitude <= 7.5) {
				kappa = 0.5;
			} else {
				kappa = 0.3;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Moderate) {
			Dy = 0.19;
			Ay = 0.16;
			Du = 2.59;
			Au = 0.36;

			SDBarStrVector << 0.86 << 1.5 << 4.04 << 11.34;
			SDStdvStrVector << 0.96 << 1 << 1.03 << 0.92;

			SDBarNonstrDriftVector << 0.86 << 1.73 << 5.4 << 10.8 ;	
			SDStdvNonstrDriftVector << 1 << 1.06 << 0.99 << 0.96 ;

			SABarNonstrAccVector << 0.25 << 0.5 << 1 << 2 ;
			SAStdvNonstrAccVector << 0.66 << 0.66 << 0.66 << 0.66 ;

			PGABarVector << 0.19 << 0.26 << 0.41 << 0.78;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.6;
			} else if (magnitude <= 7.5) {
				kappa = 0.4;
			} else {
				kappa = 0.2;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Low) {
			Dy = 0.1;
			Ay = 0.08;
			Du = 1.08;
			Au = 0.18;

			SDBarStrVector << 0.86 << 1.38 << 3.47 << 9.45;
			SDStdvStrVector << 1.05 << 0.98 << 0.89 << 0.98;

			SDBarNonstrDriftVector << 0.86 << 1.73 << 5.4 << 10.8 ;
			SDStdvNonstrDriftVector << 1.09 << 0.99 << 0.96 << 1.03 ;

			SABarNonstrAccVector << 0.2 << 0.4 << 0.8 << 1.6 ;
			SAStdvNonstrAccVector << 0.65 << 0.68 << 0.68 << 0.68 ;

			PGABarVector << 0.13 << 0.16 << 0.26 << 0.46;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.5;
			} else if (magnitude <= 7.5) {
				kappa = 0.3;
			} else {
				kappa = 0.1;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Pre) {
			Dy = 0.1;
			Ay = 0.08;
			Du = 1.3;
			Au = 0.18;

			SDBarStrVector << 0.69 << 1.11 << 2.77 << 7.56;
			SDStdvStrVector << 1.11 << 1.03 << 0.99 << 0.98;

			SDBarNonstrDriftVector << 0.86 << 1.73 << 5.4 << 10.8 ;
			SDStdvNonstrDriftVector << 1.12 << 1 << 0.99 << 1.05 ;

			SABarNonstrAccVector << 0.2 << 0.4 << 0.8 << 1.6 ;
			SAStdvNonstrAccVector << 0.66 << 0.68 << 0.68 << 0.68 ;


			PGABarVector << 0.10 << 0.13 << 0.20 << 0.36;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.4;
			} else if (magnitude <= 7.5) {
				kappa = 0.2;
			} else {
				kappa = 0;
			}
		}

		break;
	}

 	case RFemaNibsBuildingRepairCostModel::S4M : {
		betaE = 0.06;
		period = 0.65;
		if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::High) {
			Dy = 1.09;
			Ay = 0.267;
			Du = 13.1;
			Au = 0.6;

			SDBarStrVector << 1.44 << 2.88 << 8.64 << 25.2;
			SDStdvStrVector << 0.77 << 0.72 << 0.7 << 0.89;

			SDBarNonstrDriftVector << 2.16 << 4.32 << 13.5 << 27 ;	
			SDStdvNonstrDriftVector << 0.8 << 0.76 << 0.76 << 0.93 ;

			SABarNonstrAccVector << 0.3 << 0.6 << 1.2 << 2.4 ;
			SAStdvNonstrAccVector << 0.67 << 0.65 << 0.66 << 0.66 ;
	
			PGABarVector << 0.16 << 0.28 << 0.73 << 1.56;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.7;
			} else if (magnitude <= 7.5) {
				kappa = 0.5;
			} else {
				kappa = 0.3;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Moderate) {
			Dy = 0.55;
			Ay = 0.133;
			Du = 4.91;
			Au = 0.3;

			SDBarStrVector << 1.44 << 2.5 << 6.73 << 18.9;
			SDStdvStrVector << 0.75 << 0.72 << 0.72 << 0.94;

			SDBarNonstrDriftVector << 2.16 << 4.32 << 13.5 << 27 ;
			SDStdvNonstrDriftVector << 0.77 << 0.8 << 0.95 << 1.04 ;

			SABarNonstrAccVector << 0.25 << 0.5 << 1 << 2 ;
			SAStdvNonstrAccVector << 0.65 << 0.65 << 0.65 << 0.65 ;

			PGABarVector << 0.14 << 0.22 << 0.51 << 0.92;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.6;
			} else if (magnitude <= 7.5) {
				kappa = 0.4;
			} else {
				kappa = 0.2;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Low) {
			Dy = 0.27;
			Ay = 0.067;
			Du = 2.05;
			Au = 0.15;

			SDBarStrVector << 1.44 << 2.31 << 5.78 << 15.75;
			SDStdvStrVector << 0.76 << 0.78 << 0.9 << 0.99;

			SDBarNonstrDriftVector << 2.16 << 4.32 << 13.5 << 27 ;
			SDStdvNonstrDriftVector << 0.83 << 0.95 << 1.04 << 1.07 ;

			SABarNonstrAccVector << 0.2 << 0.4 << 0.8 << 1.6 ;
			SAStdvNonstrAccVector << 0.65 << 0.68 << 0.68 << 0.68 ;

			PGABarVector << 0.12 << 0.17 << 0.31 << 0.54;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.5;
			} else if (magnitude <= 7.5) {
				kappa = 0.3;
			} else {
				kappa = 0.1;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Pre) {
			Dy = 0.27;
			Ay = 0.067;
			Du = 2.46;
			Au = 0.15;

			SDBarStrVector << 1.15 << 1.85 << 4.62 << 12.6;
			SDStdvStrVector << 0.81 << 0.8 << 0.94 << 1;

			SDBarNonstrDriftVector << 2.16 << 4.32 << 13.5 << 27 ;
			SDStdvNonstrDriftVector << 0.86 << 0.99 << 1.06 << 1.1 ;

			SABarNonstrAccVector << 0.2 << 0.4 << 0.8 << 1.6 ;
			SAStdvNonstrAccVector << 0.65 << 0.68 << 0.68 << 0.68 ;

			PGABarVector << 0.09 << 0.13 << 0.25 << 0.43;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.4;
			} else if (magnitude <= 7.5) {
				kappa = 0.2;
			} else {
				kappa = 0;
			}
		}

		break;
	}

 	case RFemaNibsBuildingRepairCostModel::S4H : {
		betaE = 0.06;
		period = 1.32;
		if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::High) {
			Dy = 3.49;
			Ay = 0.203;
			Du = 31.37;
			Au = 0.457;

			SDBarStrVector << 2.25 << 4.49 << 13.48 << 39.31;
			SDStdvStrVector << 0.64 << 0.66 << 0.69 << 0.77;

			SDBarNonstrDriftVector << 4.49 << 8.99 << 28.08 << 56.16 ;
			SDStdvNonstrDriftVector << 0.72 << 0.72 << 0.79 << 0.91 ;

			SABarNonstrAccVector << 0.3 << 0.6 << 1.2 << 2.4 ;
			SAStdvNonstrAccVector << 0.67 << 0.66 << 0.65 << 0.65 ;

			PGABarVector << 0.13 << 0.25 << 0.69 << 1.63;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.7;
			} else if (magnitude <= 7.5) {
				kappa = 0.5;
			} else {
				kappa = 0.3;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Moderate) {
			Dy = 1.74;
			Ay = 0.102;
			Du = 11.76;
			Au = 0.228;

			SDBarStrVector << 2.25 << 3.9 << 10.5 << 29.48;
			SDStdvStrVector << 0.66 << 0.67 << 0.7 << 0.9;

			SDBarNonstrDriftVector << 4.49 << 8.99 << 28.08 << 56.16 ;
			SDStdvNonstrDriftVector << 0.73 << 0.82 << 0.93 << 1.01 ;

			SABarNonstrAccVector << 0.25 << 0.5 << 1 << 2 ;
			SAStdvNonstrAccVector << 0.65 << 0.66 << 0.66 << 0.66 ;

			PGABarVector << 0.12 << 0.21 << 0.51 << 0.97;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.6;
			} else if (magnitude <= 7.5) {
				kappa = 0.4;
			} else {
				kappa = 0.2;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Low) {
			Dy = 0.87;
			Ay = 0.051;
			Du = 4.9;
			Au = 0.114;

			SDBarStrVector << 2.25 << 3.6 << 9.01 << 24.57;
			SDStdvStrVector << 0.7 << 0.75 << 0.9 << 0.98;

			SDBarNonstrDriftVector << 4.49 << 8.99 << 28.08 << 56.16 ;
			SDStdvNonstrDriftVector << 0.84 << 0.95 << 1.05 << 1.07 ;

			SABarNonstrAccVector << 0.2 << 0.4 << 0.8 << 1.6 ;
			SAStdvNonstrAccVector << 0.65 << 0.68 << 0.68 << 0.68 ;

			PGABarVector << 0.12 << 0.17 << 0.33 << 0.59;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;


			if (magnitude <= 5.5) {
				kappa = 0.5;
			} else if (magnitude <= 7.5) {
				kappa = 0.3;
			} else {
				kappa = 0.1;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Pre) {
			Dy = 0.87;
			Ay = 0.051;
			Du = 5.88;
			Au = 0.114;

			SDBarStrVector << 1.8 << 2.88 << 7.21 << 19.66;
			SDStdvStrVector << 0.73 << 0.75 << 0.9 << 0.97;

			SDBarNonstrDriftVector << 4.49 << 8.99 << 28.08 << 56.16 ;
			SDStdvNonstrDriftVector << 0.88 << 0.99 << 1.07 << 1.09 ;

			SABarNonstrAccVector << 0.2 << 0.4 << 0.8 << 1.6 ;
			SAStdvNonstrAccVector << 0.66 << 0.68 << 0.68 << 0.68 ;

			PGABarVector << 0.09 << 0.14 << 0.27 << 0.47;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.4;
			} else if (magnitude <= 7.5) {
				kappa = 0.2;
			} else {
				kappa = 0;
			}
		}

		break;
	}

 	case RFemaNibsBuildingRepairCostModel::S5L : {
		betaE = 0.06;
		period = 0.35;
		if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::High) {
			rCritical(threadID) << "Error in object" << objectName() << ": The building type" << theBuildingType << "does not include the code design level" << theCodeDesignLevel <<".";
			return -1;
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Moderate) {
			rCritical(threadID) << "Error in object" << objectName() << ": The building type" << theBuildingType << "does not include the code design level" << theCodeDesignLevel <<".";
			return -1;
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Low) {
			Dy = 0.12;
			Ay = 0.1;
			Du = 1.2;
			Au = 0.2;

			SDBarStrVector << 0.65 << 1.3 << 3.24 << 7.56;
			SDStdvStrVector << 1.11 << 1.04 << 0.99 << 0.95;

			SDBarNonstrDriftVector << 0.86 << 1.73 << 5.4 << 10.8 ;
			SDStdvNonstrDriftVector << 1.14 << 1.04 << 0.98 << 1.01 ;

			SABarNonstrAccVector << 0.2 << 0.4 << 0.8 << 1.6 ;
			SAStdvNonstrAccVector << 0.65 << 0.68 << 0.67 << 0.67 ;

			PGABarVector << 0.13 << 0.17 << 0.28 << 0.45;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.5;
			} else if (magnitude <= 7.5) {
				kappa = 0.3;
			} else {
				kappa = 0.1;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Pre) {
			Dy = 0.12;
			Ay = 0.1;
			Du = 1.2;
			Au = 0.2;

			SDBarStrVector << 0.52 << 1.04 << 2.59 << 6.05;
			SDStdvStrVector << 1.2 << 1.11 << 1.08 << 0.95;

			SDBarNonstrDriftVector << 0.86 << 1.73 << 5.4 << 10.8 ;
			SDStdvNonstrDriftVector << 1.18 << 1.06 << 0.98 << 1.03 ;

			SABarNonstrAccVector << 0.2 << 0.4 << 0.8 << 1.6 ;
			SAStdvNonstrAccVector << 0.65 << 0.68 << 0.68 << 0.68 ;

			PGABarVector << 0.11 << 0.14 << 0.22 << 0.37;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.4;
			} else if (magnitude <= 7.5) {
				kappa = 0.2;
			} else {
				kappa = 0;
			}
		}

		break;
	}

	case RFemaNibsBuildingRepairCostModel::S5M : {
		betaE = 0.06;
		period = 0.65;
		if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::High) {
			rCritical(threadID) << "Error in object" << objectName() << ": The building type" << theBuildingType << "does not include the code design level" << theCodeDesignLevel <<".";
			return -1;
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Moderate) {
			rCritical(threadID) << "Error in object" << objectName() << ": The building type" << theBuildingType << "does not include the code design level" << theCodeDesignLevel <<".";
			return -1;
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Low) {
			Dy = 0.34;
			Ay = 0.083;
			Du = 2.27;
			Au = 0.167;

			SDBarStrVector << 1.08 << 2.16 << 5.4 << 12.6;
			SDStdvStrVector << 0.77 << 0.79 << 0.87 << 0.98;

			SDBarNonstrDriftVector << 2.16 << 4.32 << 13.5 << 27 ;
			SDStdvNonstrDriftVector << 0.84 << 0.95 << 1.03 << 1.07 ;

			SABarNonstrAccVector << 0.2 << 0.4 << 0.8 << 1.6 ;
			SAStdvNonstrAccVector << 0.64 << 0.68 << 0.67 << 0.67 ;

			PGABarVector << 0.11 << 0.18 << 0.34 << 0.53;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.5;
			} else if (magnitude <= 7.5) {
				kappa = 0.3;
			} else {
				kappa = 0.1;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Pre) {
			Dy = 0.34;
			Ay = 0.083;
			Du = 2.27;
			Au = 0.167;

			SDBarStrVector << 0.86 << 1.73 << 4.32 << 10.08;
			SDStdvStrVector << 0.85 << 0.83 << 0.94 << 0.99;

			SDBarNonstrDriftVector << 2.16 << 4.32 << 13.5 << 27 ;
			SDStdvNonstrDriftVector << 0.86 << 0.99 << 1.05 << 1.09 ;

			SABarNonstrAccVector << 0.2 << 0.4 << 0.8 << 1.6 ;
			SAStdvNonstrAccVector << 0.65 << 0.68 << 0.68 << 0.68 ;

			PGABarVector << 0.09 << 0.14 << 0.28 << 0.43;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.4;
			} else if (magnitude <= 7.5) {
				kappa = 0.2;
			} else {
				kappa = 0;
			}
		}

		break;
	}

	case RFemaNibsBuildingRepairCostModel::S5H : {
		betaE = 0.06;
		period = 1.32;
		if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::High) {
			rCritical(threadID) << "Error in object" << objectName() << ": The building type" << theBuildingType << "does not include the code design level" << theCodeDesignLevel <<".";
			return -1;
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Moderate) {
			rCritical(threadID) << "Error in object" << objectName() << ": The building type" << theBuildingType << "does not include the code design level" << theCodeDesignLevel <<".";
			return -1;
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Low) {
			Dy = 1.09;
			Ay = 0.063;
			Du = 5.45;
			Au = 0.127;

			SDBarStrVector << 1.68 << 3.37 << 8.42 << 19.66;
			SDStdvStrVector << 0.7 << 0.73 << 0.89 << 0.97;

			SDBarNonstrDriftVector << 4.49 << 8.99 << 28.08 << 56.16 ;
			SDStdvNonstrDriftVector << 0.84 << 0.95 << 1.03 << 1.06 ;

			SABarNonstrAccVector << 0.2 << 0.4 << 0.8 << 1.6 ;
			SAStdvNonstrAccVector << 0.65 << 0.68 << 0.68 << 0.68 ;

			PGABarVector << 0.10 << 0.18 << 0.35 << 0.58;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.5;
			} else if (magnitude <= 7.5) {
				kappa = 0.3;
			} else {
				kappa = 0.1;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Pre) {
			Dy = 1.09;
			Ay = 0.063;
			Du = 5.45;
			Au = 0.127;

			SDBarStrVector << 1.35 << 2.7 << 6.74 << 15.72;
			SDStdvStrVector << 0.72 << 0.75 << 0.92 << 0.96;

			SDBarNonstrDriftVector << 4.49 << 8.99 << 28.08 << 56.16 ;
			SDStdvNonstrDriftVector << 0.88 << 0.91 << 1.05 << 1.09 ;

			SABarNonstrAccVector << 0.2 << 0.4 << 0.8 << 1.6 ;
			SAStdvNonstrAccVector << 0.65 << 0.68 << 0.68 << 0.68 ;

			PGABarVector << 0.08 << 0.14 << 0.29 << 0.46;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.4;
			} else if (magnitude <= 7.5) {
				kappa = 0.2;
			} else {
				kappa = 0;
			}
		}

		break;
	}

	case RFemaNibsBuildingRepairCostModel::C1L : {
		betaE = 0.07;
		period = 0.4;
		if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::High) {
			Dy = 0.39;
			Ay = 0.25;
			Du = 9.39;
			Au = 0.749;

			SDBarStrVector << 0.9 << 1.8 << 5.4 << 14.4;
			SDStdvStrVector << 0.81 << 0.84 << 0.86 << 0.81;

			SDBarNonstrDriftVector << 0.72 << 1.44 << 4.5 << 9 ;
			SDStdvNonstrDriftVector << 0.84 << 0.88 << 0.9 << 0.88 ;

			SABarNonstrAccVector << 0.3 << 0.6 << 1.2 << 2.4 ;
			SAStdvNonstrAccVector << 0.68 << 0.68 << 0.67 << 0.67 ;

			PGABarVector << 0.21 << 0.35 << 0.70 << 1.37;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.9;
			} else if (magnitude <= 7.5) {
				kappa = 0.6;
			} else {
				kappa = 0.4;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Moderate) {
			Dy = 0.2;
			Ay = 0.125;
			Du = 3.52;
			Au = 0.375;

			SDBarStrVector << 0.9 << 1.56 << 4.2 << 10.8;
			SDStdvStrVector << 0.89 << 0.9 << 0.9 << 0.89;

			SDBarNonstrDriftVector << 0.72 << 1.44 << 4.5 << 9 ;
			SDStdvNonstrDriftVector << 0.93 << 0.96 << 0.94 << 0.88 ;

			SABarNonstrAccVector << 0.25 << 0.5 << 1 << 2 ;
			SAStdvNonstrAccVector << 0.67 << 0.66 << 0.66 << 0.66 ;

			PGABarVector << 0.16 << 0.23 << 0.41 << 0.77;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.8;
			} else if (magnitude <= 7.5) {
				kappa = 0.4;
			} else {
				kappa = 0.2;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Low) {
			Dy = 0.1;
			Ay = 0.062;
			Du = 1.47;
			Au = 0.187;

			SDBarStrVector << 0.9 << 1.44 << 3.6 << 9;
			SDStdvStrVector << 0.95 << 0.91 << 0.85 << 0.97;

			SDBarNonstrDriftVector << 0.72 << 1.44 << 4.5 << 9 ;
			SDStdvNonstrDriftVector << 0.99 << 0.96 << 0.9 << 1.01 ;

			SABarNonstrAccVector << 0.2 << 0.4 << 0.8 << 1.6 ;
			SAStdvNonstrAccVector << 0.65 << 0.68 << 0.68 << 0.68 ;

			PGABarVector << 0.12 << 0.15 << 0.27 << 0.45;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.6;
			} else if (magnitude <= 7.5) {
				kappa = 0.3;
			} else {
				kappa = 0.1;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Pre) {
			Dy = 0.1;
			Ay = 0.062;
			Du = 1.76;
			Au = 0.187;

			SDBarStrVector << 0.72 << 1.15 << 2.88 << 7.2;
			SDStdvStrVector << 0.98 << 0.94 << 0.9 << 0.97;

			SDBarNonstrDriftVector << 0.72 << 1.44 << 4.5 << 9 ;
			SDStdvNonstrDriftVector << 1.02 << 0.98 << 0.93 << 1.03 ;

			SABarNonstrAccVector << 0.2 << 0.4 << 0.8 << 1.6 ;
			SAStdvNonstrAccVector << 0.66 << 0.68 << 0.68 << 0.68 ;

			PGABarVector << 0.10 << 0.12 << 0.21 << 0.36;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.4;
			} else if (magnitude <= 7.5) {
				kappa = 0.2;
			} else {
				kappa = 0;
			}
		}

		break;
	}

 	case RFemaNibsBuildingRepairCostModel::C1M : {
		betaE = 0.07;
		period = 0.75;
		if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::High) {
			Dy = 1.15;
			Ay = 0.208;
			Du = 18.44;
			Au = 0.624;

			SDBarStrVector << 1.5 << 3 << 9 << 24;
			SDStdvStrVector << 0.68 << 0.67 << 0.68 << 0.81;

			SDBarNonstrDriftVector << 1.8 << 3.6 << 11.25 << 22.5 ;
			SDStdvNonstrDriftVector << 0.72 << 0.73 << 0.74 << 0.84 ;

			SABarNonstrAccVector << 0.3 << 0.6 << 1.2 << 2.4 ;
			SAStdvNonstrAccVector << 0.68 << 0.68 << 0.66 << 0.66 ;

			PGABarVector << 0.15 << 0.27 << 0.73 << 1.61;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.9;
			} else if (magnitude <= 7.5) {
				kappa = 0.6;
			} else {
				kappa = 0.4;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Moderate) {
			Dy = 0.58;
			Ay = 0.104;
			Du = 6.91;
			Au = 0.312;

			SDBarStrVector << 1.5 << 2.6 << 7 << 18;
			SDStdvStrVector << 0.7 << 0.7 << 0.7 << 0.89;

			SDBarNonstrDriftVector << 1.8 << 3.6 << 11.25 << 22.5 ;
			SDStdvNonstrDriftVector << 0.77 << 0.76 << 0.87 << 0.98 ;

			SABarNonstrAccVector << 0.25 << 0.5 << 1 << 2 ;
			SAStdvNonstrAccVector << 0.66 << 0.65 << 0.63 << 0.63 ;

			PGABarVector << 0.13 << 0.21 << 0.49 << 0.89;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.8;
			} else if (magnitude <= 7.5) {
				kappa = 0.4;
			} else {
				kappa = 0.2;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Low) {
			Dy = 0.29;
			Ay = 0.052;
			Du = 2.88;
			Au = 0.156;

			SDBarStrVector << 1.5 << 2.4 << 6 << 15;
			SDStdvStrVector << 0.7 << 0.74 << 0.86 << 0.98;

			SDBarNonstrDriftVector << 1.8 << 3.6 << 11.25 << 22.5 ;
			SDStdvNonstrDriftVector << 0.79 << 0.88 << 0.99 << 1.06 ;

			SABarNonstrAccVector << 0.2 << 0.4 << 0.8 << 1.6 ;
			SAStdvNonstrAccVector << 0.65 << 0.68 << 0.68 << 0.68 ;

			PGABarVector << 0.12 << 0.17 << 0.32 << 0.54;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.6;
			} else if (magnitude <= 7.5) {
				kappa = 0.3;
			} else {
				kappa = 0.1;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Pre) {
			Dy = 0.29;
			Ay = 0.052;
			Du = 3.46;
			Au = 0.156;

			SDBarStrVector << 1.2 << 1.92 << 4.8 << 12;
			SDStdvStrVector << 0.73 << 0.77 << 0.83 << 0.98;

			SDBarNonstrDriftVector << 1.8 << 3.6 << 11.25 << 22.5 ;
			SDStdvNonstrDriftVector << 0.81 << 0.91 << 1.02 << 1.06 ;

			SABarNonstrAccVector << 0.2 << 0.4 << 0.8 << 1.6 ;
			SAStdvNonstrAccVector << 0.66 << 0.68 << 0.68 << 0.68 ;

			PGABarVector << 0.09 << 0.13 << 0.26 << 0.43;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.4;
			} else if (magnitude <= 7.5) {
				kappa = 0.2;
			} else {
				kappa = 0;
			}
		}

		break;
	}

 	case RFemaNibsBuildingRepairCostModel::C1H : {
		betaE = 0.07;
		period = 1.45;
		if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::High) {
			Dy = 2.01;
			Ay = 0.098;
			Du = 24.13;
			Au = 0.293;

			SDBarStrVector << 2.16 << 4.32 << 12.96 << 34.56;
			SDStdvStrVector << 0.66 << 0.64 << 0.67 << 0.78;

			SDBarNonstrDriftVector << 3.46 << 6.91 << 21.6 << 43.2 ;	
			SDStdvNonstrDriftVector << 0.71 << 0.71 << 0.78 << 0.88 ;

			SABarNonstrAccVector << 0.3 << 0.6 << 1.2 << 2.4 ;
			SAStdvNonstrAccVector << 0.66 << 0.66 << 0.66 << 0.66 ;

			PGABarVector << 0.11 << 0.22 << 0.62 << 1.35;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.9;
			} else if (magnitude <= 7.5) {
				kappa = 0.6;
			} else {
				kappa = 0.4;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Moderate) {
			Dy = 1.01;
			Ay = 0.049;
			Du = 9.05;
			Au = 0.147;

			SDBarStrVector << 2.16 << 3.74 << 10.08 << 25.92;
			SDStdvStrVector << 0.66 << 0.66 << 0.76 << 0.91;

			SDBarNonstrDriftVector << 3.46 << 6.91 << 21.6 << 43.2 ;
			SDStdvNonstrDriftVector << 0.74 << 0.8 << 0.94 << 1.03 ;

			SABarNonstrAccVector << 0.25 << 0.5 << 1 << 2 ;
			SAStdvNonstrAccVector << 0.65 << 0.67 << 0.67 << 0.67 ;

			PGABarVector << 0.11 << 0.18 << 0.41 << 0.74;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.8;
			} else if (magnitude <= 7.5) {
				kappa = 0.4;
			} else {
				kappa = 0.2;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Low) {
			Dy = 0.5;
			Ay = 0.024;
			Du = 3.77;
			Au = 0.073;

			SDBarStrVector << 2.16 << 3.46 << 8.64 << 21.6;
			SDStdvStrVector << 0.7 << 0.81 << 0.89 << 0.98;

			SDBarNonstrDriftVector << 3.46 << 6.91 << 21.6 << 43.2 ;
			SDStdvNonstrDriftVector << 0.87 << 0.96 << 1.02 << 1.06 ;

			SABarNonstrAccVector << 0.2 << 0.4 << 0.8 << 1.6 ;
			SAStdvNonstrAccVector << 0.67 << 0.67 << 0.67 << 0.67 ;

			PGABarVector << 0.10 << 0.15 << 0.27 << 0.44;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.6;
			} else if (magnitude <= 7.5) {
				kappa = 0.3;
			} else {
				kappa = 0.1;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Pre) {
			Dy = 0.5;
			Ay = 0.024;
			Du = 4.52;
			Au = 0.073;

			SDBarStrVector << 1.73 << 2.76 << 6.91 << 17.28;
			SDStdvStrVector << 0.71 << 0.8 << 0.94 << 1.01;

			SDBarNonstrDriftVector << 3.46 << 6.91 << 21.6 << 43.2 ;
			SDStdvNonstrDriftVector << 0.9 << 0.99 << 1.05 << 1.1 ;

			SABarNonstrAccVector << 0.2 << 0.4 << 0.8 << 1.6 ;
			SAStdvNonstrAccVector << 0.68 << 0.68 << 0.68 << 0.68 ;

			PGABarVector << 0.08 << 0.12 << 0.21 << 0.35;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.4;
			} else if (magnitude <= 7.5) {
				kappa = 0.2;
			} else {
				kappa = 0;
			}
		}

		break;
	}

 	case RFemaNibsBuildingRepairCostModel::C2L : {
		betaE = 0.07;
		period = 0.35;
		if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::High) {
			Dy = 0.48;
			Ay = 0.4;
			Du = 9.59;
			Au = 1;

			SDBarStrVector << 0.72 << 1.8 << 5.4 << 14.4;
			SDStdvStrVector << 0.81 << 0.84 << 0.93 << 0.92;

			SDBarNonstrDriftVector << 0.72 << 1.44 << 4.5 << 9 ;
			SDStdvNonstrDriftVector << 0.87 << 0.88 << 0.97 << 0.99 ;

			SABarNonstrAccVector << 0.3 << 0.6 << 1.2 << 2.4 ;
			SAStdvNonstrAccVector << 0.69 << 0.67 << 0.66 << 0.64 ;

			PGABarVector << 0.24 << 0.45 << 0.90 << 1.55;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.9;
			} else if (magnitude <= 7.5) {
				kappa = 0.6;
			} else {
				kappa = 0.4;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Moderate) {
			Dy = 0.24;
			Ay = 0.2;
			Du = 3.6;
			Au = 0.5;

			SDBarStrVector << 0.72 << 1.52 << 4.17 << 10.8;
			SDStdvStrVector << 0.91 << 0.97 << 1.03 << 0.87;

			SDBarNonstrDriftVector << 0.72 << 1.44 << 4.5 << 9 ;
			SDStdvNonstrDriftVector << 0.96 << 1 << 1.06 << 0.95 ;

			SABarNonstrAccVector << 0.25 << 0.5 << 1 << 2 ;
			SAStdvNonstrAccVector << 0.68 << 0.66 << 0.68 << 0.68 ;

			PGABarVector << 0.18 << 0.30 << 0.49 << 0.87;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.8;
			} else if (magnitude <= 7.5) {
				kappa = 0.4;
			} else {
				kappa = 0.2;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Low) {
			Dy = 0.12;
			Ay = 0.1;
			Du = 1.5;
			Au = 0.25;

			SDBarStrVector << 0.72 << 1.37 << 3.55 << 9;
			SDStdvStrVector << 1.04 << 1.02 << 0.99 << 0.95;

			SDBarNonstrDriftVector << 0.72 << 1.44 << 4.5 << 9 ;
			SDStdvNonstrDriftVector << 1.08 << 1.05 << 0.95 << 0.99 ;

			SABarNonstrAccVector << 0.2 << 0.4 << 0.8 << 1.6 ;
			SAStdvNonstrAccVector << 0.66 << 0.67 << 0.66 << 0.66 ;

			PGABarVector << 0.14 << 0.19 << 0.30 << 0.52;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.6;
			} else if (magnitude <= 7.5) {
				kappa = 0.3;
			} else {
				kappa = 0.1;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Pre) {
			Dy = 0.12;
			Ay = 0.1;
			Du = 1.8;
			Au = 0.25;

			SDBarStrVector << 0.58 << 1.1 << 2.84 << 7.2;
			SDStdvStrVector << 1.11 << 1.09 << 1.07 << 0.93;

			SDBarNonstrDriftVector << 0.72 << 1.44 << 4.5 << 9 ;
			SDStdvNonstrDriftVector << 1.14 << 1.08 << 0.97 << 1 ;

			SABarNonstrAccVector << 0.2 << 0.4 << 0.8 << 1.6 ;
			SAStdvNonstrAccVector << 0.65 << 0.67 << 0.67 << 0.67 ;

			PGABarVector << 0.11 << 0.15 << 0.24 << 0.42;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.4;
			} else if (magnitude <= 7.5) {
				kappa = 0.2;
			} else {
				kappa = 0;
			}
		}

		break;
	}

 	case RFemaNibsBuildingRepairCostModel::C2M : {
		betaE = 0.07;
		period = 0.56;
		if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::High) {
			Dy = 1.04;
			Ay = 0.333;
			Du = 13.84;
			Au = 0.833;

			SDBarStrVector << 1.2 << 3 << 9 << 24;
			SDStdvStrVector << 0.74 << 0.77 << 0.68 << 0.77;

			SDBarNonstrDriftVector << 1.8 << 3.6 << 11.25 << 22.5 ;
			SDStdvNonstrDriftVector << 0.84 << 0.82 << 0.74 << 0.81 ;

			SABarNonstrAccVector << 0.3 << 0.6 << 1.2 << 2.4 ;
			SAStdvNonstrAccVector << 0.7 << 0.65 << 0.65 << 0.65 ;

			PGABarVector << 0.17 << 0.36 << 0.87 << 1.95;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.9;
			} else if (magnitude <= 7.5) {
				kappa = 0.6;
			} else {
				kappa = 0.4;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Moderate) {
			Dy = 0.52;
			Ay = 0.167;
			Du = 5.19;
			Au = 0.417;

			SDBarStrVector << 1.2 << 2.53 << 6.95 << 18;
			SDStdvStrVector << 0.81 << 0.77 << 0.73 << 0.91;

			SDBarNonstrDriftVector << 1.8 << 3.6 << 11.25 << 22.5 ;
			SDStdvNonstrDriftVector << 0.84 << 0.81 << 0.83 << 0.98 ;

			SABarNonstrAccVector << 0.25 << 0.5 << 1 << 2 ;
			SAStdvNonstrAccVector << 0.67 << 0.64 << 0.67 << 0.67 ;

			PGABarVector << 0.15 << 0.26 << 0.55 << 1.02;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.8;
			} else if (magnitude <= 7.5) {
				kappa = 0.4;
			} else {
				kappa = 0.2;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Low) {
			Dy = 0.26;
			Ay = 0.083;
			Du = 2.16;
			Au = 0.208;

			SDBarStrVector << 1.2 << 2.29 << 5.92 << 15;
			SDStdvStrVector << 0.82 << 0.81 << 0.81 << 0.99;

			SDBarNonstrDriftVector << 1.8 << 3.6 << 11.25 << 22.5 ;
			SDStdvNonstrDriftVector << 0.84 << 0.87 << 1 << 1.06 ;

			SABarNonstrAccVector << 0.2 << 0.4 << 0.8 << 1.6 ;
			SAStdvNonstrAccVector << 0.64 << 0.66 << 0.65 << 0.65 ;

			PGABarVector << 0.12 << 0.19 << 0.38 << 0.63;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.6;
			} else if (magnitude <= 7.5) {
				kappa = 0.3;
			} else {
				kappa = 0.1;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Pre) {
			Dy = 0.26;
			Ay = 0.083;
			Du = 2.6;
			Au = 0.208;

			SDBarStrVector << 0.96 << 1.83 << 4.74 << 12;
			SDStdvStrVector << 0.86 << 0.83 << 0.8 << 0.98;

			SDBarNonstrDriftVector << 1.8 << 3.6 << 11.25 << 22.5 ;
			SDStdvNonstrDriftVector << 0.88 << 0.9 << 1.03 << 1.07 ;

			SABarNonstrAccVector << 0.2 << 0.4 << 0.8 << 1.6 ;
			SAStdvNonstrAccVector << 0.64 << 0.67 << 0.67 << 0.67 ;

			PGABarVector << 0.10 << 0.15 << 0.30 << 0.50;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.4;
			} else if (magnitude <= 7.5) {
				kappa = 0.2;
			} else {
				kappa = 0;
			}
		}

		break;
	}

 	case RFemaNibsBuildingRepairCostModel::C2H : {
		betaE = 0.07;
		period = 1.09;
		if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::High) {
			Dy = 2.94;
			Ay = 0.254;
			Du = 29.39;
			Au = 0.635;

			SDBarStrVector << 1.73 << 4.32 << 12.96 << 34.56;
			SDStdvStrVector << 0.68 << 0.65 << 0.66 << 0.75;

			SDBarNonstrDriftVector << 3.46 << 6.91 << 21.6 << 43.2 ;
			SDStdvNonstrDriftVector << 0.71 << 0.72 << 0.74 << 0.85 ;

			SABarNonstrAccVector << 0.3 << 0.6 << 1.2 << 2.4 ;
			SAStdvNonstrAccVector << 0.68 << 0.66 << 0.65 << 0.65 ;

			PGABarVector << 0.12 << 0.29 << 0.82 << 1.87;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.9;
			} else if (magnitude <= 7.5) {
				kappa = 0.6;
			} else {
				kappa = 0.4;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Moderate) {
			Dy = 1.47;
			Ay = 0.127;
			Du = 11.02;
			Au = 0.317;

			SDBarStrVector << 1.73 << 3.64 << 10 << 25.92;
			SDStdvStrVector << 0.66 << 0.68 << 0.7 << 0.87;

			SDBarNonstrDriftVector << 3.46 << 6.91 << 21.6 << 43.2 ;
			SDStdvNonstrDriftVector << 0.73 << 0.76 << 0.89 << 0.99 ;

			SABarNonstrAccVector << 0.25 << 0.5 << 1 << 2 ;
			SAStdvNonstrAccVector << 0.66 << 0.65 << 0.65 << 0.65 ;

			PGABarVector << 0.12 << 0.23 << 0.57 << 1.07;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.8;
			} else if (magnitude <= 7.5) {
				kappa = 0.4;
			} else {
				kappa = 0.2;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Low) {
			Dy = 0.74;
			Ay = 0.063;
			Du = 4.59;
			Au = 0.159;

			SDBarStrVector << 1.73 << 3.3 << 8.53 << 21.6;
			SDStdvStrVector << 0.68 << 0.73 << 0.84 << 0.95;

			SDBarNonstrDriftVector << 3.46 << 6.91 << 21.6 << 43.2 ;
			SDStdvNonstrDriftVector << 0.79 << 0.93 << 0.99 << 1.07 ;

			SABarNonstrAccVector << 0.2 << 0.4 << 0.8 << 1.6 ;
			SAStdvNonstrAccVector << 0.64 << 0.66 << 0.66 << 0.66 ;

			PGABarVector << 0.11 << 0.19 << 0.38 << 0.65;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.6;
			} else if (magnitude <= 7.5) {
				kappa = 0.3;
			} else {
				kappa = 0.1;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Pre) {
			Dy = 0.74;
			Ay = 0.063;
			Du = 5.51;
			Au = 0.159;

			SDBarStrVector << 1.38 << 2.64 << 6.82 << 17.28;
			SDStdvStrVector << 0.73 << 0.75 << 0.92 << 0.97;

			SDBarNonstrDriftVector << 3.46 << 6.91 << 21.6 << 43.2 ;
			SDStdvNonstrDriftVector << 0.83 << 0.97 << 1.05 << 1.07 ;

			SABarNonstrAccVector << 0.2 << 0.4 << 0.8 << 1.6 ;
			SAStdvNonstrAccVector << 0.65 << 0.67 << 0.67 << 0.67 ;

			PGABarVector << 0.09 << 0.15 << 0.31 << 0.52;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.4;
			} else if (magnitude <= 7.5) {
				kappa = 0.2;
			} else {
				kappa = 0;
			}
		}

		break;
	}

 	case RFemaNibsBuildingRepairCostModel::C3L : {
		betaE = 0.07;
		period = 0.35;
		if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::High) {
			rCritical(threadID) << "Error in object" << objectName() << ": The building type" << theBuildingType << "does not include the code design level" << theCodeDesignLevel <<".";
			return -1;
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Moderate) {
			rCritical(threadID) << "Error in object" << objectName() << ": The building type" << theBuildingType << "does not include the code design level" << theCodeDesignLevel <<".";
			return -1;
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Low) {
			Dy = 0.12;
			Ay = 0.1;
			Du = 1.35;
			Au = 0.225;

			SDBarStrVector << 0.54 << 1.08 << 2.7 << 6.3;
			SDStdvStrVector << 1.09 << 1.07 << 1.08 << 0.91;

			SDBarNonstrDriftVector << 0.72 << 1.44 << 4.5 << 9 ;
			SDStdvNonstrDriftVector << 1.13 << 1.08 << 0.95 << 1 ;

			SABarNonstrAccVector << 0.2 << 0.4 << 0.8 << 1.6 ;
			SAStdvNonstrAccVector << 0.65 << 0.67 << 0.66 << 0.66 ;

			PGABarVector << 0.12 << 0.17 << 0.26 << 0.44;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.5;
			} else if (magnitude <= 7.5) {
				kappa = 0.3;
			} else {
				kappa = 0.1;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Pre) {
			Dy = 0.12;
			Ay = 0.1;
			Du = 1.35;
			Au = 0.225;

			SDBarStrVector << 0.43 << 0.86 << 2.16 << 5.04;
			SDStdvStrVector << 1.19 << 1.15 << 1.15 << 0.92;

			SDBarNonstrDriftVector << 0.72 << 1.44 << 4.5 << 9 ;
			SDStdvNonstrDriftVector << 1.19 << 1.11 << 0.99 << 1.02 ;

			SABarNonstrAccVector << 0.2 << 0.4 << 0.8 << 1.6 ;
			SAStdvNonstrAccVector << 0.65 << 0.68 << 0.68 << 0.68 ;

			PGABarVector << 0.10 << 0.14 << 0.21 << 0.35;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.4;
			} else if (magnitude <= 7.5) {
				kappa = 0.2;
			} else {
				kappa = 0;
			}
		}

		break;
	}

	case RFemaNibsBuildingRepairCostModel::C3M : {
		betaE = 0.07;
		period = 0.56;
		if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::High) {
			rCritical(threadID) << "Error in object" << objectName() << ": The building type" << theBuildingType << "does not include the code design level" << theCodeDesignLevel <<".";
			return -1;
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Moderate) {
			rCritical(threadID) << "Error in object" << objectName() << ": The building type" << theBuildingType << "does not include the code design level" << theCodeDesignLevel <<".";
			return -1;
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Low) {
			Dy = 0.26;
			Ay = 0.083;
			Du = 1.95;
			Au = 0.188;

			SDBarStrVector << 0.9 << 1.8 << 4.5 << 10.5;
			SDStdvStrVector << 0.85 << 0.83 << 0.79 << 0.98;

			SDBarNonstrDriftVector << 1.8 << 3.6 << 11.25 << 22.5 ;
			SDStdvNonstrDriftVector << 0.88 << 0.92 << 1 << 1.06 ;

			SABarNonstrAccVector << 0.2 << 0.4 << 0.8 << 1.6 ;
			SAStdvNonstrAccVector << 0.64 << 0.67 << 0.66 << 0.66 ;

			PGABarVector << 0.11 << 0.17 << 0.32 << 0.51;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.5;
			} else if (magnitude <= 7.5) {
				kappa = 0.3;
			} else {
				kappa = 0.1;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Pre) {
			Dy = 0.26;
			Ay = 0.083;
			Du = 1.95;
			Au = 0.188;

			SDBarStrVector << 0.72 << 1.44 << 3.6 << 8.4;
			SDStdvStrVector << 0.9 << 0.86 << 0.9 << 0.96;

			SDBarNonstrDriftVector << 1.8 << 3.6 << 11.25 << 22.5 ;
			SDStdvNonstrDriftVector << 0.92 << 0.95 << 1.03 << 1.09 ;

			SABarNonstrAccVector << 0.2 << 0.4 << 0.8 << 1.6 ;
			SAStdvNonstrAccVector << 0.64 << 0.67 << 0.67 << 0.67 ;

			PGABarVector << 0.09 << 0.14 << 0.25 << 0.41;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.4;
			} else if (magnitude <= 7.5) {
				kappa = 0.2;
			} else {
				kappa = 0;
			}
		}

		break;
	}

	case RFemaNibsBuildingRepairCostModel::C3H : {
		betaE = 0.07;
		period = 1.09;
		if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::High) {
			rCritical(threadID) << "Error in object" << objectName() << ": The building type" << theBuildingType << "does not include the code design level" << theCodeDesignLevel <<".";
			return -1;
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Moderate) {
			rCritical(threadID) << "Error in object" << objectName() << ": The building type" << theBuildingType << "does not include the code design level" << theCodeDesignLevel <<".";
			return -1;
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Low) {
			Dy = 0.74;
			Ay = 0.063;
			Du = 4.13;
			Au = 0.143;

			SDBarStrVector << 1.3 << 2.59 << 6.48 << 15.12;
			SDStdvStrVector << 0.71 << 0.74 << 0.9 << 0.97;

			SDBarNonstrDriftVector << 3.46 << 6.91 << 21.6 << 43.2 ;
			SDStdvNonstrDriftVector << 0.83 << 0.96 << 1.02 << 1.06 ;

			SABarNonstrAccVector << 0.2 << 0.4 << 0.8 << 1.6 ;
			SAStdvNonstrAccVector << 0.64 << 0.67 << 0.67 << 0.67 ;

			PGABarVector << 0.09 << 0.16 << 0.33 << 0.53;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.5;
			} else if (magnitude <= 7.5) {
				kappa = 0.3;
			} else {
				kappa = 0.1;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Pre) {
			Dy = 0.74;
			Ay = 0.063;
			Du = 4.13;
			Au = 0.143;

			SDBarStrVector << 1.04 << 2.07 << 5.18 << 12.1;
			SDStdvStrVector << 0.73 << 0.75 << 0.9 << 0.95;

			SDBarNonstrDriftVector << 3.46 << 6.91 << 21.6 << 43.2 ;
			SDStdvNonstrDriftVector << 0.86 << 0.9 << 1.04 << 1.09 ;

			SABarNonstrAccVector << 0.2 << 0.4 << 0.8 << 1.6 ;
			SAStdvNonstrAccVector << 0.65 << 0.67 << 0.67 << 0.67 ;

			PGABarVector << 0.08 << 0.13 << 0.27 << 0.43;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.4;
			} else if (magnitude <= 7.5) {
				kappa = 0.2;
			} else {
				kappa = 0;
			}
		}

		break;
	}

	case RFemaNibsBuildingRepairCostModel::PC1 : {
		betaE = 0.07;
		period = 0.35;
		if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::High) {
			Dy = 0.72;
			Ay = 0.6;
			Du = 11.51;
			Au = 1.2;

			SDBarStrVector << 0.54 << 1.08 << 3.24 << 9.45;
			SDStdvStrVector << 0.76 << 0.86 << 0.88 << 0.99;

			SDBarNonstrDriftVector << 0.54 << 1.08 << 3.38 << 6.75 ;	
			SDStdvNonstrDriftVector << 0.82 << 0.91 << 0.95 << 1.03 ;

			SABarNonstrAccVector << 0.3 << 0.6 << 1.2 << 2.4 ;
			SAStdvNonstrAccVector << 0.74 << 0.67 << 0.67 << 0.64 ;

			PGABarVector << 0.20 << 0.35 << 0.72 << 1.25;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.7;
			} else if (magnitude <= 7.5) {
				kappa = 0.5;
			} else {
				kappa = 0.3;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Moderate) {
			Dy = 0.36;
			Ay = 0.3;
			Du = 4.32;
			Au = 0.6;

			SDBarStrVector << 0.54 << 0.94 << 2.52 << 7.09;
			SDStdvStrVector << 0.89 << 0.92 << 0.97 << 1.04;

			SDBarNonstrDriftVector << 0.54 << 1.08 << 3.38 << 6.75 ;
			SDStdvNonstrDriftVector << 0.94 << 0.99 << 1.05 << 1.08 ;

			SABarNonstrAccVector << 0.25 << 0.5 << 1 << 2 ;
			SAStdvNonstrAccVector << 0.68 << 0.67 << 0.66 << 0.66 ;

			PGABarVector << 0.18 << 0.24 << 0.44 << 0.71;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.6;
			} else if (magnitude <= 7.5) {
				kappa = 0.4;
			} else {
				kappa = 0.2;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Low) {
			Dy = 0.18;
			Ay = 0.15;
			Du = 1.8;
			Au = 0.3;

			SDBarStrVector << 0.54 << 0.87 << 2.17 << 5.91;
			SDStdvStrVector << 1 << 1.05 << 1.12 << 0.89;

			SDBarNonstrDriftVector << 0.54 << 1.08 << 3.38 << 6.75 ;
			SDStdvNonstrDriftVector << 1.05 << 1.1 << 1.1 << 0.93 ;

			SABarNonstrAccVector << 0.2 << 0.4 << 0.8 << 1.6 ;
			SAStdvNonstrAccVector << 0.66 << 0.66 << 0.66 << 0.66 ;

			PGABarVector << 0.13 << 0.17 << 0.25 << 0.45;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.5;
			} else if (magnitude <= 7.5) {
				kappa = 0.3;
			} else {
				kappa = 0.1;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Pre) {
			Dy = 0.18;
			Ay = 0.15;
			Du = 2.16;
			Au = 0.3;

			SDBarStrVector << 0.43 << 0.69 << 1.73 << 4.73;
			SDStdvStrVector << 1.14 << 1.14 << 1.17 << 0.98;

			SDBarNonstrDriftVector << 0.54 << 1.08 << 3.38 << 6.75 ;
			SDStdvNonstrDriftVector << 1.18 << 1.16 << 1.12 << 0.95 ;

			SABarNonstrAccVector << 0.2 << 0.4 << 0.8 << 1.6 ;
			SAStdvNonstrAccVector << 0.67 << 0.66 << 0.66 << 0.66 ;

			PGABarVector << 0.11 << 0.14 << 0.21 << 0.35;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.4;
			} else if (magnitude <= 7.5) {
				kappa = 0.2;
			} else {
				kappa = 0;
			}
		}

		break;
	}

 	case RFemaNibsBuildingRepairCostModel::PC2L : {
		betaE = 0.07;
		period = 0.35;
		if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::High) {
			Dy = 0.48;
			Ay = 0.4;
			Du = 7.67;
			Au = 0.8;

			SDBarStrVector << 0.72 << 1.44 << 4.32 << 12.6;
			SDStdvStrVector << 0.84 << 0.88 << 0.98 << 0.94;

			SDBarNonstrDriftVector << 0.72 << 1.44 << 4.5 << 9 ;	
			SDStdvNonstrDriftVector << 0.89 << 0.93 << 1.03 << 1.04 ;

			SABarNonstrAccVector << 0.3 << 0.6 << 1.2 << 2.4 ;
			SAStdvNonstrAccVector << 0.68 << 0.67 << 0.67 << 0.67 ;

			PGABarVector << 0.24 << 0.36 << 0.69 << 1.23;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.7;
			} else if (magnitude <= 7.5) {
				kappa = 0.5;
			} else {
				kappa = 0.3;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Moderate) {
			Dy = 0.24;
			Ay = 0.2;
			Du = 2.88;
			Au = 0.4;

			SDBarStrVector << 0.72 << 1.25 << 3.37 << 9.45;
			SDStdvStrVector << 0.96 << 1 << 1.03 << 0.88;

			SDBarNonstrDriftVector << 0.72 << 1.44 << 4.5 << 9 ;
			SDStdvNonstrDriftVector << 1 << 1.06 << 1.07 << 0.93 ;

			SABarNonstrAccVector << 0.25 << 0.5 << 1 << 2 ;
			SAStdvNonstrAccVector << 0.66 << 0.66 << 0.65 << 0.65 ;

			PGABarVector << 0.18 << 0.25 << 0.40 << 0.74;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.6;
			} else if (magnitude <= 7.5) {
				kappa = 0.4;
			} else {
				kappa = 0.2;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Low) {
			Dy = 0.12;
			Ay = 0.1;
			Du = 1.2;
			Au = 0.2;

			SDBarStrVector << 0.72 << 1.15 << 2.89 << 7.88;
			SDStdvStrVector << 1.08 << 1.03 << 0.98 << 0.96;

			SDBarNonstrDriftVector << 0.72 << 1.44 << 4.5 << 9 ;
			SDStdvNonstrDriftVector << 1.12 << 1.04 << 0.93 << 1.02 ;

			SABarNonstrAccVector << 0.2 << 0.4 << 0.8 << 1.6 ;
			SAStdvNonstrAccVector << 0.65 << 0.68 << 0.68 << 0.68 ;

			PGABarVector << 0.13 << 0.15 << 0.24 << 0.44;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.5;
			} else if (magnitude <= 7.5) {
				kappa = 0.3;
			} else {
				kappa = 0.1;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Pre) {
			Dy = 0.12;
			Ay = 0.1;
			Du = 1.44;
			Au = 0.2;

			SDBarStrVector << 0.58 << 0.92 << 2.31 << 6.3;
			SDStdvStrVector << 1.14 << 1.1 << 1.1 << 0.93;

			SDBarNonstrDriftVector << 0.72 << 1.44 << 4.5 << 9 ;	
			SDStdvNonstrDriftVector << 1.16 << 1.06 << 0.96 << 1.02 ;

			SABarNonstrAccVector << 0.2 << 0.4 << 0.8 << 1.6 ;
			SAStdvNonstrAccVector << 0.65 << 0.68 << 0.68 << 0.68 ;

			PGABarVector << 0.10 << 0.13 << 0.19 << 0.35;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.4;
			} else if (magnitude <= 7.5) {
				kappa = 0.2;
			} else {
				kappa = 0;
			}
		}

		break;
	}

 	case RFemaNibsBuildingRepairCostModel::PC2M : {
		betaE = 0.07;
		period = 0.56;
		if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::High) {
			Dy = 1.04;
			Ay = 0.333;
			Du = 11.07;
			Au = 0.667;

			SDBarStrVector << 1.2 << 2.4 << 7.2 << 21;
			SDStdvStrVector << 0.77 << 0.81 << 0.7 << 0.82;

			SDBarNonstrDriftVector << 1.8 << 3.6 << 11.25 << 22.5 ;
			SDStdvNonstrDriftVector << 0.87 << 0.83 << 0.77 << 0.89 ;

			SABarNonstrAccVector << 0.3 << 0.6 << 1.2 << 2.4 ;
			SAStdvNonstrAccVector << 0.68 << 0.65 << 0.66 << 0.66 ;

			PGABarVector << 0.17 << 0.29 << 0.67 << 1.51;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.7;
			} else if (magnitude <= 7.5) {
				kappa = 0.5;
			} else {
				kappa = 0.3;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Moderate) {
			Dy = 0.52;
			Ay = 0.167;
			Du = 4.15;
			Au = 0.333;

			SDBarStrVector << 1.2 << 2.08 << 5.61 << 15.75;
			SDStdvStrVector << 0.82 << 0.79 << 0.75 << 0.93;

			SDBarNonstrDriftVector << 1.8 << 3.6 << 11.25 << 22.5 ;
			SDStdvNonstrDriftVector << 0.85 << 0.83 << 0.92 << 1 ;

			SABarNonstrAccVector << 0.25 << 0.5 << 1 << 2 ;
			SAStdvNonstrAccVector << 0.65 << 0.65 << 0.65 << 0.65 ;

			PGABarVector << 0.15 << 0.21 << 0.45 << 0.86;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.6;
			} else if (magnitude <= 7.5) {
				kappa = 0.4;
			} else {
				kappa = 0.2;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Low) {
			Dy = 0.26;
			Ay = 0.083;
			Du = 1.73;
			Au = 0.167;

			SDBarStrVector << 1.2 << 1.92 << 4.81 << 13.12;
			SDStdvStrVector << 0.81 << 0.79 << 0.84 << 0.99;

			SDBarNonstrDriftVector << 1.8 << 3.6 << 11.25 << 22.5 ;
			SDStdvNonstrDriftVector << 0.86 << 0.93 << 1.02 << 1.07 ;

			SABarNonstrAccVector << 0.2 << 0.4 << 0.8 << 1.6 ;
			SAStdvNonstrAccVector << 0.64 << 0.68 << 0.68 << 0.68 ;

			PGABarVector << 0.11 << 0.16 << 0.31 << 0.52;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.5;
			} else if (magnitude <= 7.5) {
				kappa = 0.3;
			} else {
				kappa = 0.1;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Pre) {
			Dy = 0.26;
			Ay = 0.083;
			Du = 2.08;
			Au = 0.167;

			SDBarStrVector << 0.96 << 1.54 << 3.85 << 10.5;
			SDStdvStrVector << 0.87 << 0.83 << 0.91 << 1;

			SDBarNonstrDriftVector << 1.8 << 3.6 << 11.25 << 22.5 ;
			SDStdvNonstrDriftVector << 0.87 << 0.95 << 1.04 << 1.07 ;

			SABarNonstrAccVector << 0.2 << 0.4 << 0.8 << 1.6 ;
			SAStdvNonstrAccVector << 0.65 << 0.68 << 0.68 << 0.68 ;

			PGABarVector << 0.09 << 0.13 << 0.24 << 0.42;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.4;
			} else if (magnitude <= 7.5) {
				kappa = 0.2;
			} else {
				kappa = 0;
			}
		}

		break;
	}

 	case RFemaNibsBuildingRepairCostModel::PC2H : {
		betaE = 0.07;
		period = 1.09;
		if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::High) {
			Dy = 2.94;
			Ay = 0.254;
			Du = 23.52;
			Au = 0.508;

			SDBarStrVector << 1.73 << 3.46 << 10.37 << 30.24;
			SDStdvStrVector << 0.64 << 0.66 << 0.68 << 0.81;

			SDBarNonstrDriftVector << 3.46 << 6.91 << 21.6 << 43.2 ;
			SDStdvNonstrDriftVector << 0.73 << 0.73 << 0.77 << 0.89 ;

			SABarNonstrAccVector << 0.3 << 0.6 << 1.2 << 2.4 ;
			SAStdvNonstrAccVector << 0.67 << 0.65 << 0.65 << 0.65 ;

			PGABarVector << 0.12 << 0.23 << 0.63 << 1.49;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.7;
			} else if (magnitude <= 7.5) {
				kappa = 0.5;
			} else {
				kappa = 0.3;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Moderate) {
			Dy = 1.47;
			Ay = 0.127;
			Du = 8.82;
			Au = 0.254;

			SDBarStrVector << 1.73 << 3 << 8.08 << 22.68;
			SDStdvStrVector << 0.68 << 0.69 << 0.77 << 0.89;

			SDBarNonstrDriftVector << 3.46 << 6.91 << 21.6 << 43.2 ;
			SDStdvNonstrDriftVector << 0.74 << 0.79 << 0.93 << 1.02 ;

			SABarNonstrAccVector << 0.25 << 0.5 << 1 << 2 ;
			SAStdvNonstrAccVector << 0.64 << 0.65 << 0.65 << 0.65 ;

			PGABarVector << 0.12 << 0.19 << 0.46 << 0.90;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.6;
			} else if (magnitude <= 7.5) {
				kappa = 0.4;
			} else {
				kappa = 0.2;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Low) {
			Dy = 0.74;
			Ay = 0.063;
			Du = 3.67;
			Au = 0.127;

			SDBarStrVector << 1.73 << 2.77 << 6.93 << 18.9;
			SDStdvStrVector << 0.71 << 0.75 << 0.89 << 0.98;

			SDBarNonstrDriftVector << 3.46 << 6.91 << 21.6 << 43.2 ;
			SDStdvNonstrDriftVector << 0.83 << 0.94 << 1.04 << 1.07 ;

			SABarNonstrAccVector << 0.2 << 0.4 << 0.8 << 1.6 ;
			SAStdvNonstrAccVector << 0.64 << 0.67 << 0.67 << 0.67 ;

			PGABarVector << 0.11 << 0.16 << 0.31 << 0.55;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.5;
			} else if (magnitude <= 7.5) {
				kappa = 0.3;
			} else {
				kappa = 0.1;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Pre) {
			Dy = 0.74;
			Ay = 0.063;
			Du = 4.41;
			Au = 0.127;

			SDBarStrVector << 1.38 << 2.21 << 5.55 << 15.12;
			SDStdvStrVector << 0.74 << 0.75 << 0.91 << 0.96;

			SDBarNonstrDriftVector << 3.46 << 6.91 << 21.6 << 43.2 ;
			SDStdvNonstrDriftVector << 0.87 << 0.99 << 1.06 << 1.08 ;

			SABarNonstrAccVector << 0.2 << 0.4 << 0.8 << 1.6 ;
			SAStdvNonstrAccVector << 0.66 << 0.67 << 0.67 << 0.67 ;

			PGABarVector << 0.09 << 0.13 << 0.25 << 0.43;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.4;
			} else if (magnitude <= 7.5) {
				kappa = 0.2;
			} else {
				kappa = 0;
			}
		}

		break;
	}

 	case RFemaNibsBuildingRepairCostModel::RM1L : {
		betaE = 0.085;
		period = 0.35;
		if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::High) {
			Dy = 0.64;
			Ay = 0.533;
			Du = 10.23;
			Au = 1.066;

			SDBarStrVector << 0.72 << 1.44 << 4.32 << 12.6;
			SDStdvStrVector << 0.84 << 0.86 << 0.92 << 1.01;

			SDBarNonstrDriftVector << 0.72 << 1.44 << 4.5 << 9 ;	
			SDStdvNonstrDriftVector << 0.89 << 0.91 << 0.97 << 1.06 ;

			SABarNonstrAccVector << 0.3 << 0.6 << 1.2 << 2.4 ;
			SAStdvNonstrAccVector << 0.7 << 0.67 << 0.67 << 0.63 ;

			PGABarVector << 0.30 << 0.46 << 0.93 << 1.57;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.9;
			} else if (magnitude <= 7.5) {
				kappa = 0.6;
			} else {
				kappa = 0.4;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Moderate) {
			Dy = 0.32;
			Ay = 0.267;
			Du = 3.84;
			Au = 0.533;

			SDBarStrVector << 0.72 << 1.25 << 3.37 << 9.45;
			SDStdvStrVector << 0.96 << 0.99 << 1.05 << 0.94;

			SDBarNonstrDriftVector << 0.72 << 1.44 << 4.5 << 9 ;
			SDStdvNonstrDriftVector << 1 << 1.06 << 1.12 << 1.01 ;

			SABarNonstrAccVector << 0.25 << 0.5 << 1 << 2 ;
			SAStdvNonstrAccVector << 0.68 << 0.67 << 0.67 << 0.67 ;

			PGABarVector << 0.22 << 0.30 << 0.50 << 0.85;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.8;
			} else if (magnitude <= 7.5) {
				kappa = 0.4;
			} else {
				kappa = 0.2;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Low) {
			Dy = 0.16;
			Ay = 0.133;
			Du = 1.6;
			Au = 0.267;

			SDBarStrVector << 0.72 << 1.15 << 2.89 << 7.88;
			SDStdvStrVector << 1.11 << 1.1 << 1.1 << 0.92;

			SDBarNonstrDriftVector << 0.72 << 1.44 << 4.5 << 9 ;
			SDStdvNonstrDriftVector << 1.15 << 1.12 << 1.03 << 0.99 ;

			SABarNonstrAccVector << 0.2 << 0.4 << 0.8 << 1.6 ;
			SAStdvNonstrAccVector << 0.66 << 0.67 << 0.64 << 0.64 ;

			PGABarVector << 0.16 << 0.20 << 0.29 << 0.54;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.6;
			} else if (magnitude <= 7.5) {
				kappa = 0.3;
			} else {
				kappa = 0.1;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Pre) {
			Dy = 0.16;
			Ay = 0.133;
			Du = 1.92;
			Au = 0.267;

			SDBarStrVector << 0.58 << 0.92 << 2.31 << 6.3;
			SDStdvStrVector << 1.2 << 1.17 << 1.17 << 0.94;

			SDBarNonstrDriftVector << 0.72 << 1.44 << 4.5 << 9 ;
			SDStdvNonstrDriftVector << 1.22 << 1.14 << 1.03 << 0.99 ;

			SABarNonstrAccVector << 0.2 << 0.4 << 0.8 << 1.6 ;
			SAStdvNonstrAccVector << 0.66 << 0.67 << 0.66 << 0.66 ;

			PGABarVector << 0.13 << 0.16 << 0.24 << 0.43;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.4;
			} else if (magnitude <= 7.5) {
				kappa = 0.2;
			} else {
				kappa = 0;
			}
		}

		break;
	}

 	case RFemaNibsBuildingRepairCostModel::RM1M : {
		betaE = 0.085;
		period = 0.56;
		if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::High) {
			Dy = 1.38;
			Ay = 0.444;
			Du = 14.76;
			Au = 0.889;

			SDBarStrVector << 1.2 << 2.4 << 7.2 << 21;
			SDStdvStrVector << 0.71 << 0.81 << 0.76 << 0.75;
			
			SDBarNonstrDriftVector << 1.8 << 3.6 << 11.25 << 22.5 ;
			SDStdvNonstrDriftVector << 0.81 << 0.86 << 0.8 << 0.81 ;

			SABarNonstrAccVector << 0.3 << 0.6 << 1.2 << 2.4 ;
			SAStdvNonstrAccVector << 0.72 << 0.66 << 0.65 << 0.65 ;

			PGABarVector << 0.20 << 0.37 << 0.81 << 1.90;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.9;
			} else if (magnitude <= 7.5) {
				kappa = 0.6;
			} else {
				kappa = 0.4;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Moderate) {
			Dy = 0.69;
			Ay = 0.222;
			Du = 5.54;
			Au = 0.444;

			SDBarStrVector << 1.2 << 2.08 << 5.61 << 15.75;
			SDStdvStrVector << 0.81 << 0.82 << 0.8 << 0.89;

			SDBarNonstrDriftVector << 1.8 << 3.6 << 11.25 << 22.5 ;
			SDStdvNonstrDriftVector << 0.88 << 0.85 << 0.84 << 0.98 ;

			SABarNonstrAccVector << 0.25 << 0.5 << 1 << 2 ;
			SAStdvNonstrAccVector << 0.67 << 0.64 << 0.67 << 0.67 ;

			PGABarVector << 0.18 << 0.26 << 0.51 << 1.03;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.8;
			} else if (magnitude <= 7.5) {
				kappa = 0.4;
			} else {
				kappa = 0.2;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Low) {
			Dy = 0.35;
			Ay = 0.111;
			Du = 2.31;
			Au = 0.222;

			SDBarStrVector << 1.2 << 1.92 << 4.81 << 13.12;
			SDStdvStrVector << 0.87 << 0.84 << 0.79 << 0.96;

			SDBarNonstrDriftVector << 1.8 << 3.6 << 11.25 << 22.5 ;
			SDStdvNonstrDriftVector << 0.89 << 0.89 << 1 << 1.05 ;

			SABarNonstrAccVector << 0.2 << 0.4 << 0.8 << 1.6 ;
			SAStdvNonstrAccVector << 0.64 << 0.66 << 0.64 << 0.64 ;

			PGABarVector << 0.14 << 0.19 << 0.35 << 0.63;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.6;
			} else if (magnitude <= 7.5) {
				kappa = 0.3;
			} else {
				kappa = 0.1;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Pre) {
			Dy = 0.35;
			Ay = 0.111;
			Du = 2.77;
			Au = 0.222;

			SDBarStrVector << 0.96 << 1.54 << 3.85 << 10.5;
			SDStdvStrVector << 0.91 << 0.89 << 0.89 << 0.96;

			SDBarNonstrDriftVector << 1.8 << 3.6 << 11.25 << 22.5 ;
			SDStdvNonstrDriftVector << 0.93 << 0.92 << 1.02 << 1.07 ;

			SABarNonstrAccVector << 0.2 << 0.4 << 0.8 << 1.6 ;
			SAStdvNonstrAccVector << 0.64 << 0.66 << 0.65 << 0.65 ;

			PGABarVector << 0.11 << 0.15 << 0.28 << 0.50;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.4;
			} else if (magnitude <= 7.5) {
				kappa = 0.2;
			} else {
				kappa = 0;
			}
		}

		break;
	}

 	case RFemaNibsBuildingRepairCostModel::RM2L : {
		betaE = 0.085;
		period = 0.35;
		if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::High) {
			Dy = 0.64;
			Ay = 0.533;
			Du = 10.23;
			Au = 1.066;

			SDBarStrVector << 0.72 << 1.44 << 4.32 << 12.6;
			SDStdvStrVector << 0.8 << 0.81 << 0.91 << 0.98;

			SDBarNonstrDriftVector << 0.72 << 1.44 << 4.5 << 9 ;	
			SDStdvNonstrDriftVector << 0.85 << 0.87 << 0.95 << 1.03 ;

			SABarNonstrAccVector << 0.3 << 0.6 << 1.2 << 2.4 ;
			SAStdvNonstrAccVector << 0.7 << 0.66 << 0.67 << 0.64 ;

			PGABarVector << 0.26 << 0.42 << 0.87 << 1.49;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.9;
			} else if (magnitude <= 7.5) {
				kappa = 0.6;
			} else {
				kappa = 0.4;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Moderate) {
			Dy = 0.32;
			Ay = 0.267;
			Du = 3.84;
			Au = 0.533;

			SDBarStrVector << 0.72 << 1.25 << 3.37 << 9.45;
			SDStdvStrVector << 0.91 << 0.96 << 1.02 << 0.93;

			SDBarNonstrDriftVector << 0.72 << 1.44 << 4.5 << 9 ;
			SDStdvNonstrDriftVector << 0.96 << 1.02 << 1.1 << 0.99 ;

			SABarNonstrAccVector << 0.25 << 0.5 << 1 << 2 ;
			SAStdvNonstrAccVector << 0.68 << 0.66 << 0.67 << 0.67 ;

			PGABarVector << 0.20 << 0.28 << 0.47 << 0.81;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.8;
			} else if (magnitude <= 7.5) {
				kappa = 0.4;
			} else {
				kappa = 0.2;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Low) {
			Dy = 0.16;
			Ay = 0.133;
			Du = 1.6;
			Au = 0.267;

			SDBarStrVector << 0.72 << 1.15 << 2.89 << 7.88;
			SDStdvStrVector << 1.05 << 1.07 << 1.09 << 0.91;

			SDBarNonstrDriftVector << 0.72 << 1.44 << 4.5 << 9 ;
			SDStdvNonstrDriftVector << 1.09 << 1.08 << 1.01 << 0.99 ;

			SABarNonstrAccVector << 0.2 << 0.4 << 0.8 << 1.6 ;
			SAStdvNonstrAccVector << 0.66 << 0.67 << 0.64 << 0.64 ;

			PGABarVector << 0.14 << 0.18 << 0.28 << 0.51;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.6;
			} else if (magnitude <= 7.5) {
				kappa = 0.3;
			} else {
				kappa = 0.1;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Pre) {
			Dy = 0.16;
			Ay = 0.133;
			Du = 1.92;
			Au = 0.267;

			SDBarStrVector << 0.58 << 0.92 << 2.31 << 6.3;
			SDStdvStrVector << 1.14 << 1.1 << 1.15 << 0.92;

			SDBarNonstrDriftVector << 0.72 << 1.44 << 4.5 << 9 ;
			SDStdvNonstrDriftVector << 1.17 << 1.12 << 1.01 << 0.99 ;

			SABarNonstrAccVector << 0.2 << 0.4 << 0.8 << 1.6 ;
			SAStdvNonstrAccVector << 0.66 << 0.67 << 0.67 << 0.67 ;

			PGABarVector << 0.12 << 0.15 << 0.22 << 0.41;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.4;
			} else if (magnitude <= 7.5) {
				kappa = 0.2;
			} else {
				kappa = 0;
			}
		}

		break;
	}

 	case RFemaNibsBuildingRepairCostModel::RM2M : {
		betaE = 0.085;
		period = 0.56;
		if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::High) {
			Dy = 1.38;
			Ay = 0.444;
			Du = 14.76;
			Au = 0.889;

			SDBarStrVector << 1.2 << 2.4 << 7.2 << 21;
			SDStdvStrVector << 0.71 << 0.79 << 0.7 << 0.73;

			SDBarNonstrDriftVector << 1.8 << 3.6 << 11.25 << 22.5 ;
			SDStdvNonstrDriftVector << 0.82 << 0.84 << 0.76 << 0.8 ;

			SABarNonstrAccVector << 0.3 << 0.6 << 1.2 << 2.4 ;
			SAStdvNonstrAccVector << 0.72 << 0.65 << 0.65 << 0.65 ;

			PGABarVector << 0.17 << 0.33 << 0.75 << 1.83;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.9;
			} else if (magnitude <= 7.5) {
				kappa = 0.6;
			} else {
				kappa = 0.4;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Moderate) {
			Dy = 0.69;
			Ay = 0.222;
			Du = 5.54;
			Au = 0.444;

			SDBarStrVector << 1.2 << 2.08 << 5.61 << 15.75;
			SDStdvStrVector << 0.81 << 0.8 << 0.75 << 0.88;

			SDBarNonstrDriftVector << 1.8 << 3.6 << 11.25 << 22.5 ;
			SDStdvNonstrDriftVector << 0.88 << 0.83 << 0.81 << 0.98 ;

			SABarNonstrAccVector << 0.25 << 0.5 << 1 << 2 ;
			SAStdvNonstrAccVector << 0.67 << 0.64 << 0.67 << 0.67 ;

			PGABarVector << 0.16 << 0.23 << 0.48 << 0.99;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.8;
			} else if (magnitude <= 7.5) {
				kappa = 0.4;
			} else {
				kappa = 0.2;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Low) {
			Dy = 0.35;
			Ay = 0.111;
			Du = 2.31;
			Au = 0.222;

			SDBarStrVector << 1.2 << 1.92 << 4.81 << 13.12;
			SDStdvStrVector << 0.84 << 0.81 << 0.77 << 0.96;

			SDBarNonstrDriftVector << 1.8 << 3.6 << 11.25 << 22.5 ;
			SDStdvNonstrDriftVector << 0.85 << 0.86 << 0.99 << 1.06 ;

			SABarNonstrAccVector << 0.2 << 0.4 << 0.8 << 1.6 ;
			SAStdvNonstrAccVector << 0.64 << 0.66 << 0.65 << 0.65 ;

			PGABarVector << 0.12 << 0.17 << 0.34 << 0.60;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.6;
			} else if (magnitude <= 7.5) {
				kappa = 0.3;
			} else {
				kappa = 0.1;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Pre) {
			Dy = 0.35;
			Ay = 0.111;
			Du = 2.77;
			Au = 0.222;

			SDBarStrVector << 0.96 << 1.54 << 3.85 << 10.5;
			SDStdvStrVector << 0.89 << 0.87 << 0.87 << 0.96;

			SDBarNonstrDriftVector << 1.8 << 3.6 << 11.25 << 22.5 ;
			SDStdvNonstrDriftVector << 0.89 << 0.9 << 1.01 << 1.07 ;

			SABarNonstrAccVector << 0.2 << 0.4 << 0.8 << 1.6 ;
			SAStdvNonstrAccVector << 0.64 << 0.66 << 0.66 << 0.66 ;

			PGABarVector << 0.10 << 0.14 << 0.26 << 0.47;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.4;
			} else if (magnitude <= 7.5) {
				kappa = 0.2;
			} else {
				kappa = 0;
			}
		}

		break;
	}

 	case RFemaNibsBuildingRepairCostModel::RM2H : {
		betaE = 0.085;
		period = 1.09;
		if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::High) {
			Dy = 3.92;
			Ay = 0.338;
			Du = 31.35;
			Au = 0.677;

			SDBarStrVector << 1.73 << 3.46 << 10.37 << 30.24;
			SDStdvStrVector << 0.66 << 0.65 << 0.66 << 0.72;

			SDBarNonstrDriftVector << 3.46 << 6.91 << 21.6 << 43.2 ;	
			SDStdvNonstrDriftVector << 0.71 << 0.73 << 0.73 << 0.85 ;

			SABarNonstrAccVector << 0.3 << 0.6 << 1.2 << 2.4 ;
			SAStdvNonstrAccVector << 0.7 << 0.65 << 0.65 << 0.65 ;

			PGABarVector << 0.12 << 0.24 << 0.67 << 1.78;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.9;
			} else if (magnitude <= 7.5) {
				kappa = 0.6;
			} else {
				kappa = 0.4;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Moderate) {
			Dy = 1.96;
			Ay = 0.169;
			Du = 11.76;
			Au = 0.338;

			SDBarStrVector << 1.73 << 3 << 8.08 << 22.68;
			SDStdvStrVector << 0.67 << 0.69 << 0.7 << 0.86;

			SDBarNonstrDriftVector << 3.46 << 6.91 << 21.6 << 43.2 ;
			SDStdvNonstrDriftVector << 0.73 << 0.76 << 0.88 << 0.99 ;

			SABarNonstrAccVector << 0.25 << 0.5 << 1 << 2 ;
			SAStdvNonstrAccVector << 0.66 << 0.64 << 0.64 << 0.64 ;

			PGABarVector << 0.12 << 0.20 << 0.48 << 1.01;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.8;
			} else if (magnitude <= 7.5) {
				kappa = 0.4;
			} else {
				kappa = 0.2;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Low) {
			Dy = 0.98;
			Ay = 0.085;
			Du = 4.9;
			Au = 0.169;

			SDBarStrVector << 1.73 << 2.77 << 6.93 << 18.9;
			SDStdvStrVector << 0.69 << 0.72 << 0.87 << 0.96;

			SDBarNonstrDriftVector << 3.46 << 6.91 << 21.6 << 43.2 ;
			SDStdvNonstrDriftVector << 0.79 << 0.92 << 0.99 << 1.06 ;

			SABarNonstrAccVector << 0.2 << 0.4 << 0.8 << 1.6 ;
			SAStdvNonstrAccVector << 0.64 << 0.66 << 0.66 << 0.66 ;

			PGABarVector << 0.11 << 0.17 << 0.35 << 0.62;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.6;
			} else if (magnitude <= 7.5) {
				kappa = 0.3;
			} else {
				kappa = 0.1;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Pre) {
			Dy = 0.98;
			Ay = 0.085;
			Du = 5.88;
			Au = 0.169;

			SDBarStrVector << 1.38 << 2.21 << 5.55 << 15.12;
			SDStdvStrVector << 0.75 << 0.75 << 0.84 << 0.94;

			SDBarNonstrDriftVector << 3.46 << 6.91 << 21.6 << 43.2 ;
			SDStdvNonstrDriftVector << 0.82 << 0.96 << 1.04 << 1.07 ;

			SABarNonstrAccVector << 0.2 << 0.4 << 0.8 << 1.6 ;
			SAStdvNonstrAccVector << 0.65 << 0.67 << 0.67 << 0.67 ;

			PGABarVector << 0.09 << 0.13 << 0.27 << 0.50;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.4;
			} else if (magnitude <= 7.5) {
				kappa = 0.2;
			} else {
				kappa = 0;
			}
		}

		break;
	}

 	case RFemaNibsBuildingRepairCostModel::URML : {
		betaE = 0.1;
		period = 0.35;
		if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::High) {
			rCritical(threadID) << "Error in object" << objectName() << ": The building type" << theBuildingType << "does not include the code design level" << theCodeDesignLevel <<".";
			return -1;
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Moderate) {
			rCritical(threadID) << "Error in object" << objectName() << ": The building type" << theBuildingType << "does not include the code design level" << theCodeDesignLevel <<".";
			return -1;
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Low) {
			Dy = 0.24;
			Ay = 0.2;
			Du = 2.4;
			Au = 0.4;

			SDBarStrVector << 0.41 << 0.81 << 2.03 << 4.73;
			SDStdvStrVector << 0.99 << 1.05 << 1.1 << 1.08;

			SDBarNonstrDriftVector << 0.54 << 1.08 << 3.38 << 6.75 ;
			SDStdvNonstrDriftVector << 1.07 << 1.13 << 1.16 << 1.01 ;

			SABarNonstrAccVector << 0.2 << 0.4 << 0.8 << 1.6 ;
			SAStdvNonstrAccVector << 0.68 << 0.65 << 0.65 << 0.65 ;

			PGABarVector << 0.14 << 0.20 << 0.32 << 0.46;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.5;
			} else if (magnitude <= 7.5) {
				kappa = 0.3;
			} else {
				kappa = 0.1;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Pre) {
			Dy = 0.24;
			Ay = 0.2;
			Du = 2.4;
			Au = 0.4;

			SDBarStrVector << 0.32 << 0.65 << 1.62 << 3.78;
			SDStdvStrVector << 1.15 << 1.19 << 1.2 << 1.18;

			SDBarNonstrDriftVector << 0.54 << 1.08 << 3.38 << 6.75 ;
			SDStdvNonstrDriftVector << 1.21 << 1.23 << 1.23 << 1.03 ;

			SABarNonstrAccVector << 0.2 << 0.4 << 0.8 << 1.6 ;
			SAStdvNonstrAccVector << 0.69 << 0.65 << 0.65 << 0.65 ;

			PGABarVector << 0.13 << 0.17 << 0.26 << 0.37;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.4;
			} else if (magnitude <= 7.5) {
				kappa = 0.2;
			} else {
				kappa = 0;
			}
		}

		break;
	}

	case RFemaNibsBuildingRepairCostModel::URMM : {
		betaE = 0.1;
		period = 0.5;
		if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::High) {
			rCritical(threadID) << "Error in object" << objectName() << ": The building type" << theBuildingType << "does not include the code design level" << theCodeDesignLevel <<".";
			return -1;
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Moderate) {
			rCritical(threadID) << "Error in object" << objectName() << ": The building type" << theBuildingType << "does not include the code design level" << theCodeDesignLevel <<".";
			return -1;
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Low) {
			Dy = 0.27;
			Ay = 0.111;
			Du = 1.81;
			Au = 0.222;

			SDBarStrVector << 0.63 << 1.26 << 3.15 << 7.35;
			SDStdvStrVector << 0.91 << 0.92 << 0.87 << 0.91;

			SDBarNonstrDriftVector << 1.26 << 2.52 << 7.88 << 15.75 ;
			SDStdvNonstrDriftVector << 0.97 << 0.91 << 0.98 << 1.04 ;

			SABarNonstrAccVector << 0.2 << 0.4 << 0.8 << 1.6 ;
			SAStdvNonstrAccVector << 0.64 << 0.66 << 0.66 << 0.66 ;

			PGABarVector << 0.10 << 0.16 << 0.27<< 0.46;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.5;
			} else if (magnitude <= 7.5) {
				kappa = 0.3;
			} else {
				kappa = 0.1;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Pre) {
			Dy = 0.27;
			Ay = 0.111;
			Du = 1.81;
			Au = 0.222;

			SDBarStrVector << 0.5 << 1.01 << 2.52 << 5.88;
			SDStdvStrVector << 0.99 << 0.97 << 0.9 << 0.88;

			SDBarNonstrDriftVector << 1.26 << 2.52 << 7.88 << 15.75 ;
			SDStdvNonstrDriftVector << 0.99 << 0.95 << 0.99 << 1.06 ;

			SABarNonstrAccVector << 0.2 << 0.4 << 0.8 << 1.6 ;
			SAStdvNonstrAccVector << 0.64 << 0.66 << 0.66 << 0.66 ;

			PGABarVector << 0.09 << 0.13 << 0.21 << 0.38;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.4;
			} else if (magnitude <= 7.5) {
				kappa = 0.2;
			} else {
				kappa = 0;
			}
		}

		break;
	}

	case RFemaNibsBuildingRepairCostModel::MH : {
		betaE = 0.05;
		period = 0.35;
		if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::High) {
			Dy = 0.18;
			Ay = 0.15;
			Du = 2.16;
			Au = 0.3;

			SDBarStrVector << 0.48 << 0.96 << 2.88 << 8.4;
			SDStdvStrVector << 0.91 << 1 << 1.03 << 0.92;

			SDBarNonstrDriftVector << 0.48 << 0.96 << 3 << 6 ;	
			SDStdvNonstrDriftVector << 0.96 << 1.05 << 1.07 << 0.93 ;

			SABarNonstrAccVector << 0.3 << 0.6 << 1.2 << 2.4 ;
			SAStdvNonstrAccVector << 0.65 << 0.67 << 0.67 << 0.67 ;

			PGABarVector << 0.11 << 0.18 << 0.31 << 0.60;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.8;
			} else if (magnitude <= 7.5) {
				kappa = 0.4;
			} else {
				kappa = 0.2;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Moderate) {
			Dy = 0.18;
			Ay = 0.15;
			Du = 2.16;
			Au = 0.3;

			SDBarStrVector << 0.48 << 0.96 << 2.88 << 8.4;
			SDStdvStrVector << 0.91 << 1 << 1.03 << 0.92;

			SDBarNonstrDriftVector << 0.48 << 0.96 << 3 << 6 ;
			SDStdvNonstrDriftVector << 0.96 << 1.05 << 1.07 << 0.93 ;

			SABarNonstrAccVector << 0.25 << 0.5 << 1 << 2 ;
			SAStdvNonstrAccVector << 0.65 << 0.67 << 0.67 << 0.67 ;

			PGABarVector << 0.11 << 0.18 << 0.31 << 0.60;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.8;
			} else if (magnitude <= 7.5) {
				kappa = 0.4;
			} else {
				kappa = 0.2;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Low) {
			Dy = 0.18;
			Ay = 0.15;
			Du = 2.16;
			Au = 0.3;

			SDBarStrVector << 0.48 << 0.96 << 2.88 << 8.4;
			SDStdvStrVector << 0.91 << 1 << 1.03 << 0.92;

			SDBarNonstrDriftVector << 0.48 << 0.96 << 3 << 6 ;
			SDStdvNonstrDriftVector << 0.96 << 1.05 << 1.07 << 0.93 ;

			SABarNonstrAccVector << 0.2 << 0.4 << 0.8 << 1.6 ;
			SAStdvNonstrAccVector << 0.65 << 0.67 << 0.67 << 0.67 ;

			PGABarVector << 0.11 << 0.18 << 0.31 << 0.60;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.8;
			} else if (magnitude <= 7.5) {
				kappa = 0.4;
			} else {
				kappa = 0.2;
			}
		}
		else if (theCodeDesignLevel == RFemaNibsBuildingRepairCostModel::Pre) {
			Dy = 0.18;
			Ay = 0.15;
			Du = 2.16;
			Au = 0.3;

			SDBarStrVector << 0.38 << 0.77 << 2.3 << 6.72;
			SDStdvStrVector << 1.11 << 1.1 << 0.95 << 0.97;

			SDBarNonstrDriftVector << 0.48 << 0.96 << 3 << 6 ;
			SDStdvNonstrDriftVector << 1.15 << 1.09 << 0.93 << 0.99 ;

			SABarNonstrAccVector << 0.2 << 0.4 << 0.8 << 1.6 ;
			SAStdvNonstrAccVector << 0.67 << 0.65 << 0.65 << 0.65 ;

			PGABarVector << 0.08 << 0.11 << 0.18 << 0.34;
			PGAStdvVector << 0.64 << 0.64 << 0.64 << 0.64;

			if (magnitude <= 5.5) {
				kappa = 0.6;
			} else if (magnitude <= 7.5) {
				kappa = 0.3;
			} else {
				kappa = 0.1;
			}
		}

		break;
	}
	}

	// Enter the central damage factors
	QVector<double> CDFStrVector;
	QVector<double> CDFNonstrAccVector;
	QVector<double> CDFNonstrDriftVector;

	CDFStrVector << 0.0;
	CDFNonstrAccVector << 0.0;
	CDFNonstrDriftVector << 0.0;

	double cost;
	
	// Occupancy class information: The data below are from the chapter 15 of the FEMA-NIBS document

	switch (theOccupancyClass){
	
	case RFemaNibsBuildingRepairCostModel::RES1 : {
		CDFStrVector << 0.5 << 2.3 << 11.7 << 23.4;
		CDFNonstrAccVector << 0.5 << 2.7 << 8.0 << 26.6;
		CDFNonstrDriftVector << 1.0 << 5.5 << 25.0 << 50.0;
		cost = 90.85;	// Per square foot
		break;
	} 

	case RFemaNibsBuildingRepairCostModel::RES2 : {
		CDFStrVector << 0.4 << 2.4 << 7.3 << 24.4;
		CDFNonstrAccVector << 0.8 << 3.8 << 11.3 << 37.8;
		CDFNonstrDriftVector << 0.8 << 3.8 << 18.9 << 37.8;
		cost = 30.90;	// Per square foot
		break;
	} 

	case RFemaNibsBuildingRepairCostModel::RES3A : {
		CDFStrVector << 0.3 << 1.4 << 6.9 << 13.8 ;
		CDFNonstrAccVector << 0.8 << 4.3 << 13.1 << 43.7 ;
		CDFNonstrDriftVector << 0.9 << 4.3 << 21.3 << 42.5 ;
		cost = 100.77;
		break;
	}

	case RFemaNibsBuildingRepairCostModel::RES3B : {
		CDFStrVector << 0.3 << 1.4 << 6.9 << 13.8 ;
		CDFNonstrAccVector  << 0.8 << 4.3 << 13.1 << 43.7 ;
		CDFNonstrDriftVector   << 0.9 << 4.3 << 21.3 << 42.5 ;
		cost = 100.77;
		break;

	} 
	
	case RFemaNibsBuildingRepairCostModel::RES3C : {
		CDFStrVector << 0.3 << 1.4 << 6.9 << 13.8 ;
		CDFNonstrAccVector   << 0.8 << 4.3 << 13.1 << 43.7 ;
		CDFNonstrDriftVector  << 0.9 << 4.3 << 21.3 << 42.5 ;
		cost = 100.77;
		break;
	}

	case RFemaNibsBuildingRepairCostModel::RES3D : {
		CDFStrVector << 0.3 << 1.4 << 6.9 << 13.8 ;
		CDFNonstrAccVector  << 0.8 << 4.3 << 13.1 << 43.7 ;
		CDFNonstrDriftVector  << 0.9 << 4.3 << 21.3 << 42.5 ;
		cost = 100.77;
		break;
	}

	case RFemaNibsBuildingRepairCostModel::RES3E : {
		CDFStrVector << 0.3 << 1.4 << 6.9 << 13.8 ;
		CDFNonstrAccVector  << 0.8 << 4.3 << 13.1 << 43.7 ;
		CDFNonstrDriftVector << 0.9 << 4.3 << 21.3 << 42.5 ;
		cost = 100.77;
		break;
	} 

	case RFemaNibsBuildingRepairCostModel::RES3F: {
		CDFStrVector << 0.3 << 1.4 << 6.9 << 13.8 ;
		CDFNonstrAccVector << 0.8 << 4.3 << 13.1 << 43.7 ;
		CDFNonstrDriftVector << 0.9 << 4.3 << 21.3 << 42.5 ;
		cost = 100.77;
		break;
	} 
												  
	case RFemaNibsBuildingRepairCostModel::RES4 : {
		CDFStrVector << 0.2 << 1.4 << 6.8 << 13.6 ;	
		CDFNonstrAccVector << 0.9 << 4.3 << 13 << 43.2 ;	
		CDFNonstrDriftVector << 0.9 << 4.3 << 21.6 << 43.2 ;
		cost = 100.57;
		break;
	} 

	case RFemaNibsBuildingRepairCostModel::RES5: {
		CDFStrVector << 0.4 << 1.9 << 9.4 << 18.8 ;
		CDFNonstrAccVector << 0.8 << 4.1 << 12.4 << 41.2 ;
		CDFNonstrDriftVector << 0.8 << 4 << 20 << 40 ;
		cost = 110.54;
		break;
	} 

	case RFemaNibsBuildingRepairCostModel::RES6 : {
		CDFStrVector << 0.4 << 1.8 << 9.2 << 18.4 ;
		CDFNonstrAccVector << 0.8 << 4.1 << 12.2 << 40.8 ;
		CDFNonstrDriftVector << 0.8 << 4.1 << 20.4 << 40.8 ;
		cost = 104.62;
		break;
	} 

	case RFemaNibsBuildingRepairCostModel::COM1 : {
		CDFStrVector << 0.6 << 2.9 << 14.7 << 29.4 ;
		CDFNonstrAccVector << 0.8 << 4.4 << 12.9 << 43.1 ;
		CDFNonstrDriftVector << 0.6 << 2.7 << 13.8 << 27.5 ;
		cost = 77.17;
		break;
	} 

	case RFemaNibsBuildingRepairCostModel::COM2 : {
		CDFStrVector << 0.6 << 3.2 << 16.2 << 32.4 ;
		CDFNonstrAccVector << 0.8 << 4.2 << 12.4 << 41.1 ;
		CDFNonstrDriftVector << 0.6 << 2.6 << 13.2 << 26.5 ;
		cost = 59.25;
		break;
	} 

	case RFemaNibsBuildingRepairCostModel::COM3: {
		CDFStrVector << 0.3 << 1.6 << 8.1 << 16.2 ;
		CDFNonstrAccVector << 1 << 5 << 15 << 50 ;
		CDFNonstrDriftVector << 0.7 << 3.4 << 16.9 << 33.8 ;
		cost = 100.89;
		break;
	} 

	case RFemaNibsBuildingRepairCostModel::COM4: {
		CDFStrVector << 0.4 << 1.9 << 9.6 << 19.2 ;
		CDFNonstrAccVector << 0.9 << 4.8 << 14.4 << 47.9 ;
		CDFNonstrDriftVector << 0.7 << 3.3 << 16.4 << 32.9 ;
		cost = 133.31;
		break;
	} 

	case RFemaNibsBuildingRepairCostModel::COM5 : {
		CDFStrVector << 0.3 << 1.4 << 6.9 << 13.8 ;
		CDFNonstrAccVector << 1 << 5.2 << 15.5 << 51.7 ;
		CDFNonstrDriftVector << 0.7 << 3.4 << 17.2 << 34.5 ;
		cost = 88.21;
		break;
	} 

	case RFemaNibsBuildingRepairCostModel::COM6 : {
		CDFStrVector << 0.2 << 1.4 << 7 << 14 ;
		CDFNonstrAccVector << 1 << 5.1 << 15.4 << 51.3 ;
		CDFNonstrDriftVector << 0.8 << 3.5 << 17.4 << 34.7 ;
		cost = 149.29;
		break;
	}

	case RFemaNibsBuildingRepairCostModel::COM7 : {
		CDFStrVector << 0.3 << 1.4 << 7.2 << 14.4 ;
		CDFNonstrAccVector << 1 << 5.2 << 15.3 << 51.2 ;
		CDFNonstrDriftVector << 0.7 << 3.4 << 17.2 << 34.4 ;
		cost = 127.21;
		break;
	}

	case RFemaNibsBuildingRepairCostModel::COM8 : {
		CDFStrVector << 0.2 << 1 << 5 << 10 ;
		CDFNonstrAccVector << 1.1 << 5.4 << 16.3 << 54.4 ;
		CDFNonstrDriftVector << 0.7 << 3.6 << 17.8 << 35.6 ;
		cost = 113.23;
		break;
	}
												  
	case RFemaNibsBuildingRepairCostModel::COM9 : {
		CDFStrVector << 0.3 << 1.2 << 6.1 << 12.2 ;
		CDFNonstrAccVector << 1 << 5.3 << 15.8 << 52.7 ;
		CDFNonstrDriftVector << 0.7 << 3.5 << 17.6 << 35.1 ;
		cost = 105.98;
		break;
	}

	case RFemaNibsBuildingRepairCostModel::COM10 : {
		CDFStrVector << 1.3 << 6.1 << 30.4 << 60.9 ;
		CDFNonstrAccVector << 0.3 << 2.2 << 6.5 << 21.7 ;
		CDFNonstrDriftVector << 0.4 << 1.7 << 8.7 << 17.4 ;
		cost = 41.99;
		break;
	}

	case RFemaNibsBuildingRepairCostModel::IND1 : {
		CDFStrVector << 0.4 << 1.6 << 7.8 << 15.7 ;
		CDFNonstrAccVector << 1.4 << 7.2 << 21.8 << 72.5 ;
		CDFNonstrDriftVector << 0.2 << 1.2 << 5.9 << 11.8 ;
		cost = 76.22;
		break;
	}

	case RFemaNibsBuildingRepairCostModel::IND2 : {
		CDFStrVector << 0.4 << 1.6 << 7.8 << 15.7 ;
		CDFNonstrAccVector << 1.4 << 7.2 << 21.8 << 72.5 ;
		CDFNonstrDriftVector << 0.2 << 1.2 << 5.9 << 11.8 ;
		cost = 71.45;
		break;
	}

	case RFemaNibsBuildingRepairCostModel::IND3 : {
		CDFStrVector << 0.4 << 1.6 << 7.8 << 15.7 ;
		CDFNonstrAccVector << 1.4 << 7.2 << 21.8 << 72.5 ;
		CDFNonstrDriftVector << 0.2 << 1.2 << 5.9 << 11.8 ;
		cost = 90.65;
		break;
	}

	case RFemaNibsBuildingRepairCostModel::IND4 : {
		CDFStrVector << 0.4 << 1.6 << 7.8 << 15.7 ;
		CDFNonstrAccVector << 1.4 << 7.2 << 21.8 << 72.5 ;
		CDFNonstrDriftVector << 0.2 << 1.2 << 5.9 << 11.8 ;
		cost = 90.65;
		break;
	}

	case RFemaNibsBuildingRepairCostModel::IND5 : {
		CDFStrVector << 0.4 << 1.6 << 7.8 << 15.7 ;
		CDFNonstrAccVector << 1.4 << 7.2 << 21.8 << 72.5 ;
		CDFNonstrDriftVector << 0.2 << 1.2 << 5.9 << 11.8 ;
		cost = 90.65;
		break;
	}

	case RFemaNibsBuildingRepairCostModel::IND6 : {
		CDFStrVector << 0.4 << 1.6 << 7.8 << 15.7 ;
		CDFNonstrAccVector << 1.4 << 7.2 << 21.8 << 72.5 ;
		CDFNonstrDriftVector << 0.2 << 1.2 << 5.9 << 11.8 ;
		cost = 62.97;
		break;
	}

	case RFemaNibsBuildingRepairCostModel::AGR1 : {
		CDFStrVector << 0.8 << 4.6 << 23.1 << 46.2 ;
		CDFNonstrAccVector << 0.8 << 4.6 << 13.8 << 46.1 ;
		CDFNonstrDriftVector << 0 << 0.8 << 3.8 << 7.7 ;
		cost = 62.97;
		break;
	}

	case RFemaNibsBuildingRepairCostModel::REL1 : {
		CDFStrVector << 0.3 << 2 << 9.9 << 19.8 ;
		CDFNonstrAccVector << 0.9 << 4.7 << 14.3 << 47.6 ;
		CDFNonstrDriftVector << 0.8 << 3.3 << 16.3 << 32.6 ;
		cost = 114.08;
		break;
	}

	case RFemaNibsBuildingRepairCostModel::GOV1 : {
		CDFStrVector << 0.3 << 1.8 << 9 << 17.9 ;
		CDFNonstrAccVector << 1 << 4.9 << 14.8 << 49.3 ;
		CDFNonstrDriftVector << 0.7 << 3.3 << 16.4 << 32.8 ;
		cost = 117.69;
		break;
	}

	case RFemaNibsBuildingRepairCostModel::GOV2 : {
		CDFStrVector << 0.3 << 1.5 << 7.7 << 15.3 ;
		CDFNonstrAccVector << 1 << 5.1 << 15.1 << 50.5 ;
		CDFNonstrDriftVector << 0.7 << 3.4 << 17.1 << 34.2 ;
		cost = 109.70;
		break;
	}

	case RFemaNibsBuildingRepairCostModel::EDU1 : {
		CDFStrVector << 0.4 << 1.9 << 9.5 << 18.9 ;
		CDFNonstrAccVector << 0.7 << 3.2 << 9.7 << 32.4 ;
		CDFNonstrDriftVector << 0.9 << 4.9 << 24.3 << 48.7 ;
		cost = 98.87;
		break;
	}

	case RFemaNibsBuildingRepairCostModel::EDU2 : {
		CDFStrVector << 0.2 << 1.1 << 5.5 << 11 ;
		CDFNonstrAccVector << 0.6 << 2.9 << 8.7 << 29 ;
		CDFNonstrDriftVector << 1.2 << 6 << 30 << 60 ;
		cost = 109.38;
		break;
	}
	}

	// Threshold period of displacement preserevd region
	double Tvd = 10;// exp((magnitude - 5.0) / 2.0 * log(10.0));

	double SD;
	double SA;

	// Thresholds
	double cc = 386 / 4.0 / M_PI / M_PI; // = 9.8: Sd = 9.8 * Sa * T * T when Sa is in g and Sd in inch
	double SDav = cc * Sa1 * Sa1 / Sa0_3;
	double SDvd = cc * Sa1 * Tvd;

	// Determine SDp and SAp
	double SDp;
	double SAp;
	// State 1:
	SDp = Sa0_3 * Dy / Ay;
	// State 2:
	if (!(SDp <= SDav)) {
		SDp = sqrt(cc) * Sa1 * sqrt(Dy / Ay);
		// State 3:
		if (!((SDp > SDav) && (SDp <= SDvd))) {
			SDp = SDvd;
		}
	}
	if (SDp <= Dy) {
		SAp = Ay / Dy * SDp;
	} else if ((SDp > Dy) && (SDp <= Du)) {
		SAp = Ay + (Au - Ay) / (Du - Dy) * (SDp - Dy);
	} else {
		SAp =  Au;
	}

	// Calculate the total damping ratio
	//double betaEff = betaE + kappa * (Ay * Dy / 2.0 + (Au + Ay) * (Du - Dy) / 2.0) * 2.0 / (2 * M_PI * Du * Au);
	//double betaEff = betaE + kappa * 4.0 * (Ay * SDp - Dy * SAp)  / (2 * M_PI * SDp * SAp);
	double betaEff = betaE + kappa * 4.0 * SAp * (SDp - SAp/(Ay/Dy))  / (2 * M_PI * SDp * SAp);
	
	// Scale the spectral acceleration demand according to the new damping
	double Ra = 2.12 / (3.21 - 0.68 * log(betaEff * 100));
	double Rv = 1.65 / (2.31 - 0.41 * log(betaEff * 100));
	double Say1 = Sa1 / Rv;
	double Say0_3 = Sa0_3 / Ra;

	// Thresholds
	double SDyav = cc * Say1 * Say1 / Say0_3;
	double SDyvd = cc * Say1 * Tvd;

	// Starting to find the SD
	bool state123 = true;
	// State 1:
	SD = Say0_3 * Dy / Ay;
	SA = Say0_3; // or: SA = SD * Ay / Dy;

	// State 2:
	if (!((SD <= SDyav) && (SD <= Dy))) {
		SD = sqrt(cc) * Say1 * sqrt(Dy / Ay);
		SA = SD * Ay / Dy;

		// State 3:
		if (!((SD > SDyav) && (SD <= SDyvd) && (SD <= Dy))) {
			SD = SDyvd;
			SA = SD * Ay / Dy;

			// State 4:
			if (!(SD <= Dy)) {
				SD = Dy + (Du - Dy) / (Au - Ay) * (Say0_3 - Ay);
				SA = Say0_3; // or: SA = Ay + (Au - Ay) / (Du - Dy) * (SD - Dy);
				state123 = false;

				// State 5:
				if (!((SD <= SDyav) && (SD > Dy) && (SD <= Du))) {
					double AA = (Au - Ay) / (Du - Dy);
					double BB = Ay - AA * Dy;
					BB /= 2.0;
					double CC = -cc * Say1 * Say1;
					SD = (-BB + sqrt(BB * BB - AA * CC)) / AA;
					SA = Ay + (Au - Ay) / (Du - Dy) * (SD - Dy);

					// State 6:
					if (!((SD > SDyav) && (SD <= SDyvd) && (SD > Dy) && (SD <= Du))) {
						SD = SDyvd;
						SA = Ay + (Au - Ay) / (Du - Dy) * (SD - Dy);

						// State 7
						if (!((SD > Dy) && (SD <= Du))) {
							SD = cc * Say1 * Say1 / Au;
							SA = Au;

							// State 8
							if (!((SD > Du) && (SD > SDyav) && (SD <= SDyvd))) {
								SD = SDyvd;
								SA = Au;
								//rCritical(threadID) << "Warning: A too large deformation is calculated equal to" << SD << "inch in the model" << objectName() << ".";

							}
						}
					}
				}
			}
		}
	}

	// In case od state 1 or 2 or 3 (intersection in elastic capacity range), it should be ckecked whether the yield point falls between the elastic and inelastic demand curve:
	if (state123) {
		double SDe;
		double SAe;

		// State 1:
		SDe = Sa0_3 * Dy / Ay;
		SAe = Sa0_3; // or: SAe = SDe * Ay / Dy;

		// State 2:
		if (!((SDe <= SDav) && (SDe <= Dy))) {
			SDe = M_PI * Sa1 * sqrt(Dy / Ay);
			SAe = SDe * Ay / Dy;

			// State 3:
			if (!((SDe > SDav) && (SDe <= SDvd) && (SDe <= Dy))) {
				SDe = SDvd;
				SAe = SDe * Ay / Dy;

				// State 4,5,6,7:
				if (!(SDe <= Dy)) {
					SD = Dy;
					SA = Ay;
				} else {
					SD = SDe;
					SA = SAe;
				}
			} else {
				SD = SDe;
				SA = SAe;
			}
		} else {
			SD = SDe;
			SA = SAe;
		}
	}

	if (SA > Au) {
		SA = Au;
	}






	// Calculate the probability of each structural damage state
	QVector<double> probabilityVector;
	double prob = 1.0;
	int count = SDBarStrVector.count();
	for (int m = 0; m < count; m++) {
		double oldProb = prob;
		prob = gsl_cdf_gaussian_P(1.0 / SDStdvStrVector[m] * log(SD / SDBarStrVector[m]), 1);
		probabilityVector << oldProb - prob;
	}
	probabilityVector << prob - 0.0;

	// Caluclate the mean non-structural drift-sensitive  damage factor and standard deviation
	double MDFStr = 0;
	for (int i = 0; i < CDFStrVector.count(); i++) {
		MDFStr += CDFStrVector[i] * probabilityVector[i];
	}

	MDFStr /= 100.0;



	// Calculate the probability of each non-structural drift-sensitive damage state
	QVector<double> probabilityNDVector;
	double probND = 1.0;
	int countND = SDBarNonstrDriftVector.count();
	for (int m = 0; m < countND; m++) {
		double oldProb = probND;
		probND = gsl_cdf_gaussian_P(1.0 / SDStdvNonstrDriftVector[m] * log(SD / SDBarNonstrDriftVector[m]), 1);
		probabilityNDVector << oldProb - probND;
	}
	probabilityNDVector << probND - 0.0;

	// Caluclate the mean STRUCTURAL damage factor and standard deviation
	double MDFNonStrD = 0;
	for (int i = 0; i < CDFNonstrDriftVector.count(); i++) {
		MDFNonStrD += CDFNonstrDriftVector[i] * probabilityNDVector[i];
	}

	MDFNonStrD /= 100.0;



	// Calculate the probability of each non-structural acc-sensitive damage state
	QVector<double> probabilityNAVector;
	double probNA = 1.0;
	int countNA = SABarNonstrAccVector.count();
	for (int m = 0; m < countNA; m++) {
		double oldProb = probNA;
		probNA = gsl_cdf_gaussian_P(1.0 / SAStdvNonstrAccVector[m] * log(SA / SABarNonstrAccVector[m]), 1);
		probabilityNAVector << oldProb - probNA;
	}
	probabilityNAVector << probNA - 0.0;

	// Caluclate the mean non-structural acc-sensitive  damage factor and standard deviation
	double MDFNonStrA = 0;
	for (int i = 0; i < CDFNonstrAccVector.count(); i++) {
		MDFNonStrA += CDFNonstrAccVector[i] * probabilityNAVector[i];
	}

	MDFNonStrA /= 100.0;



	//// Calculate the probability of each content damage state: The probabilities are the same as those of acc-sensitive
	//QVector<double> CDFCVector;
	//CDFCVector << 0 << 1 << 5 << 25 << 50;

	//// Caluclate the mean content damage factor and standard deviation
	//double MDFC = 0;
	//for (int i = 0; i < CDFCVector.count(); i++) {
	//	MDFC += CDFCVector[i] * probabilityNAVector[i];
	//}

	//MDFC /= 100.0;


	double damageFactor = MDFNonStrA + MDFNonStrD + MDFStr /*+ MDFC*/;


	// Adjusting damage factor
	if (damageFactor < 0) {
		damageFactor = 0;
		rCritical(threadID) << "Warning: The damage factor evaluated in" << objectName() << "is less than 0.";
	}
	if (damageFactor > 1.0) {
		damageFactor = 1.0;
		rCritical(threadID) << "Warning: The damage factor evaluated in" << objectName() << "is greater than 1.";
	}

	// Convert cost from per sq ft to per m2 and from USD to CAD and from 2003 to 2010 by inflation
	cost *= 1 / 0.09290304 * 1.149168 * 1.56;

	//// ATTENTION: THIS LINE IS TO FILTER THE RESULT FOR A SPECIFIC OCCUPANCY TYPE
	//if (!(theOccupancyClass == RFemaNibsBuildingRepairCostModel::RES1 || theOccupancyClass == RFemaNibsBuildingRepairCostModel::RES2)) {
	//	cost = 0;
	//}
	
	double finalLoss = damageFactor * cost * theFootprintArea * theNumberOfStoreys;
	//If the repairStatus is zero, i.e., the repair opeation has not been done yet, the reapir cost is zero. Once the reapir operation is done the cost is added. 
	double repairStatus = theRepairStatus.data()->getCurrentValue(threadID);
	finalLoss = finalLoss * repairStatus;

	// Updating the response current value
	theDamageFactor->setCurrentValue(finalLoss, threadID);

	return 0;
}

bool RFemaNibsBuildingRepairCostModel::canBeRunParallelly(){
	return true;
}



