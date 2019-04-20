#include "SNodalLoad.h"
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>

#include "RObject.h"
#include "RParameter.h"
#include "SNode.h"


SNodalLoad::SNodalLoad(QObject *parent, QString &name)
                : RObject(parent, name)
{
        theNode = 0;
        theLoadVector = 0;

        //$$ theFxParameter = 0;
        theFxDouble = 0.0;
        //$$ theFyParameter = 0;
        theFyDouble = 0.0;
        //$$ theFzParameter = 0;
        theFzDouble = 0.0;
        //$$ theMxParameter = 0;
        theMxDouble = 0.0;
        //$$ theMyParameter = 0;
        theMyDouble = 0.0;
        //$$ theMzParameter = 0;
        theMzDouble = 0.0;
}

SNodalLoad::~SNodalLoad()
{
        if (theLoadVector!=0) {
                gsl_vector_free(theLoadVector);
        }
}



QString SNodalLoad::getFx()
{
        return getStringParameter(&theFxDouble, theFxParameter.data());
}

void SNodalLoad::setFx(QString value)
{
        setStringParameter(value, &theFxDouble, &theFxParameter);
}

double SNodalLoad::getFxValue()
{
        return getDoubleParameter(&theFxDouble, theFxParameter.data());
}

QString SNodalLoad::getFy()
{
        return getStringParameter(&theFyDouble, theFyParameter.data());
}

void SNodalLoad::setFy(QString value)
{
        setStringParameter(value, &theFyDouble, &theFyParameter);
}

double SNodalLoad::getFyValue()
{
        return getDoubleParameter(&theFyDouble, theFyParameter.data());
}

QString SNodalLoad::getFz()
{
        return getStringParameter(&theFzDouble, theFzParameter.data());
}

void SNodalLoad::setFz(QString value)
{
        setStringParameter(value, &theFzDouble, &theFzParameter);
}

double SNodalLoad::getFzValue()
{
        return getDoubleParameter(&theFzDouble, theFzParameter.data());
}

QString SNodalLoad::getMx()
{
        return getStringParameter(&theMxDouble, theMxParameter.data());
}

void SNodalLoad::setMx(QString value)
{
        setStringParameter(value, &theMxDouble, &theMxParameter);
}

double SNodalLoad::getMxValue()
{
        return getDoubleParameter(&theMxDouble, theMxParameter.data());
}

QString SNodalLoad::getMy()
{
        return getStringParameter(&theMyDouble, theMyParameter.data());
}

void SNodalLoad::setMy(QString value)
{
        setStringParameter(value, &theMyDouble, &theMyParameter);
}

double SNodalLoad::getMyValue()
{
        return getDoubleParameter(&theMyDouble, theMyParameter.data());
}

QString SNodalLoad::getMz()
{
        return getStringParameter(&theMzDouble, theMzParameter.data());
}

void SNodalLoad::setMz(QString value)
{
        setStringParameter(value, &theMzDouble, &theMzParameter);
}

double SNodalLoad::getMzValue()
{
        return getDoubleParameter(&theMzDouble, theMzParameter.data());
}






QObject * SNodalLoad::getNode() const
{
        return theNode;
}
void SNodalLoad::setNode(QObject *value)
{
        theNode = qobject_cast<SNode *>(value);
}




gsl_vector * SNodalLoad::getLoadVector()
{
        int numDOFsPerNode = 7;

        double theLoadXValue = getFxValue();
        double theLoadYValue = getFyValue();
        double theLoadZValue = getFzValue();
        double theLoadXXValue = getMxValue();
        double theLoadYYValue = getMyValue();
        double theLoadZZValue = getMzValue();

        if (theLoadVector == 0) {
                theLoadVector = gsl_vector_calloc(numDOFsPerNode);
        }

        gsl_vector_set(theLoadVector, 0, theLoadXValue);
        gsl_vector_set(theLoadVector, 1, theLoadYValue);
        gsl_vector_set(theLoadVector, 2, theLoadZValue);
        gsl_vector_set(theLoadVector, 3, theLoadXXValue);
        gsl_vector_set(theLoadVector, 4, theLoadYYValue);
        gsl_vector_set(theLoadVector, 5, theLoadZZValue);

        return theLoadVector;
}

bool SNodalLoad::canBeRunParallelly() {
	return false;
}