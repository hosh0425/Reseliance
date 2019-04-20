#ifndef SMaterial_H
#define SMaterial_H

#include "RParameter.h"
#include <gsl/gsl_matrix.h>

class SMaterial : public RObject
{
        Q_OBJECT;

        Q_PROPERTY(QString YoungsModulus READ getYoungsModulus WRITE setYoungsModulus);
        Q_PROPERTY(QString PoissonsRatio READ getPoissonsRatio WRITE setPoissonsRatio);
        Q_PROPERTY(QString WeightDensity READ getWeightDensity WRITE setWeightDensity);

public:
        SMaterial(QObject *parent, QString &name);
        ~SMaterial();

        QString getYoungsModulus();
        void setYoungsModulus(QString value);
        double getYoungsModulusValue();

        QString getPoissonsRatio();
        void setPoissonsRatio(QString value);
        double getPoissonsRatioValue();

        QString getWeightDensity();
        void setWeightDensity(QString value);
        double getWeightDensityValue();

        double getShearModulusValue();
        int getPlaneStrainStiffness(gsl_matrix *d);
        int getPlaneStressStiffness(gsl_matrix *d);
        int get3DStiffness(gsl_matrix *d);
		bool canBeRunParallelly();


private:
        RPointer<RParameter> theYoungsModulusParameter;
        double theYoungsModulusDouble;

        RPointer<RParameter> thePoissonsRatioParameter;
        double thePoissonsRatioDouble;

        RPointer<RParameter> theWeightDensityParameter;
        double theWeightDensityDouble;
};
#endif


