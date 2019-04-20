#include "SMaterial.h"
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>


SMaterial::SMaterial(QObject *parent, QString &name)
                : RObject(parent, name)
{
	/* //$$ 
        theYoungsModulusParameter = 0;
        thePoissonsRatioParameter = 0;
        theWeightDensityParameter = 0;
	*/

        theYoungsModulusDouble = 0.0;
        thePoissonsRatioDouble = 0.0;
        theWeightDensityDouble = 0.0;
}

SMaterial::~SMaterial()
{
}




QString SMaterial::getYoungsModulus()
{
        return getStringParameter(&theYoungsModulusDouble, theYoungsModulusParameter.data());
}
void SMaterial::setYoungsModulus(QString value)
{
        setStringParameter(value, &theYoungsModulusDouble, &theYoungsModulusParameter);
}
double SMaterial::getYoungsModulusValue()
{
        return getDoubleParameter(&theYoungsModulusDouble, theYoungsModulusParameter.data());
}


QString SMaterial::getPoissonsRatio()
{
        return getStringParameter(&thePoissonsRatioDouble, thePoissonsRatioParameter.data());
}
void SMaterial::setPoissonsRatio(QString value)
{
        setStringParameter(value, &thePoissonsRatioDouble, &thePoissonsRatioParameter);
}
double SMaterial::getPoissonsRatioValue()
{
        return getDoubleParameter(&thePoissonsRatioDouble, thePoissonsRatioParameter.data());
}




QString SMaterial::getWeightDensity()
{
        return getStringParameter(&theWeightDensityDouble, theWeightDensityParameter.data());
}
void SMaterial::setWeightDensity(QString value)
{
        setStringParameter(value, &theWeightDensityDouble, &theWeightDensityParameter);
}
double SMaterial::getWeightDensityValue()
{
        return getDoubleParameter(&theWeightDensityDouble, theWeightDensityParameter.data());
}






double SMaterial::getShearModulusValue()
{
        double E = getDoubleParameter(&theYoungsModulusDouble,theYoungsModulusParameter.data());
        double nu = getDoubleParameter(&thePoissonsRatioDouble,thePoissonsRatioParameter.data());
        return E/(2.0*(1.0+nu));
}


int SMaterial::getPlaneStressStiffness(gsl_matrix *d)
{
        double E = getDoubleParameter(&theYoungsModulusDouble,theYoungsModulusParameter.data());
        double nu = getDoubleParameter(&thePoissonsRatioDouble,thePoissonsRatioParameter.data());

        double factor = E/(1.0-nu*nu);

        gsl_matrix_set(d, 0, 0, factor);     gsl_matrix_set(d, 0, 1, nu*factor);  gsl_matrix_set(d, 0, 2, 0.0);
        gsl_matrix_set(d, 1, 0, nu*factor);  gsl_matrix_set(d, 1, 1, factor);     gsl_matrix_set(d, 1, 2, 0.0);
        gsl_matrix_set(d, 2, 0, 0.0);        gsl_matrix_set(d, 2, 1, 0.0);        gsl_matrix_set(d, 2, 2, 0.5*factor*(1.0-nu));

        return 0;
}




int SMaterial::getPlaneStrainStiffness(gsl_matrix *d)
{
        double E = getDoubleParameter(&theYoungsModulusDouble,theYoungsModulusParameter.data());
        double nu = getDoubleParameter(&thePoissonsRatioDouble,thePoissonsRatioParameter.data());

        double factor = E/((1.0+nu)*(1.0-2.0*nu));

        gsl_matrix_set(d, 0, 0, factor*(1.0-nu));     gsl_matrix_set(d, 0, 1, nu*factor);  gsl_matrix_set(d, 0, 2, 0.0);
        gsl_matrix_set(d, 1, 0, nu*factor);  gsl_matrix_set(d, 1, 1, factor*(1.0-nu));     gsl_matrix_set(d, 1, 2, 0.0);
        gsl_matrix_set(d, 2, 0, 0.0);        gsl_matrix_set(d, 2, 1, 0.0);        gsl_matrix_set(d, 2, 2, 0.5*factor*(1.0-2.0*nu));

        return 0;
}




int SMaterial::get3DStiffness(gsl_matrix *d)
{
        return 0;
}


bool SMaterial::canBeRunParallelly() {
	return false;
}






