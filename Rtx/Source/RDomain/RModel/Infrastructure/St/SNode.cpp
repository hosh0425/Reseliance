#include "SNode.h"
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>

#include "RObject.h"
#include "RParameter.h"


SNode::SNode(QObject *parent, QString &name)
                : RObject(parent, name)
{
        // Standing assumption
        numDOFsPerNode = 7;

		/* //$$ 
        theXCoordinateParameter = 0;
        theYCoordinateParameter = 0;
        theZCoordinateParameter = 0;
		*/

        theXCoordinateDouble = 0.0;
        theYCoordinateDouble = 0.0;
        theZCoordinateDouble = 0.0;

        theDisplacementVector = gsl_vector_calloc(numDOFsPerNode);
}

SNode::~SNode()
{       
        gsl_vector_free(theDisplacementVector);
}






QString SNode::getXCoordinate()
{
        return getStringParameter(&theXCoordinateDouble, theXCoordinateParameter.data());
}
void SNode::setXCoordinate(QString value)
{
        setStringParameter(value, &theXCoordinateDouble, &theXCoordinateParameter);
}
double SNode::getXCoordinateValue()
{
        return getDoubleParameter(&theXCoordinateDouble, theXCoordinateParameter.data());
}


QString SNode::getYCoordinate()
{
        return getStringParameter(&theYCoordinateDouble, theYCoordinateParameter.data());
}
void SNode::setYCoordinate(QString value)
{
        setStringParameter(value, &theYCoordinateDouble, &theYCoordinateParameter);
}
double SNode::getYCoordinateValue()
{
        return getDoubleParameter(&theYCoordinateDouble, theYCoordinateParameter.data());
}


QString SNode::getZCoordinate()
{
        return getStringParameter(&theZCoordinateDouble, theZCoordinateParameter.data());
}
void SNode::setZCoordinate(QString value)
{
        setStringParameter(value, &theZCoordinateDouble, &theZCoordinateParameter);
}
double SNode::getZCoordinateValue()
{
        return getDoubleParameter(&theZCoordinateDouble, theZCoordinateParameter.data());
}

















double SNode::getXDisplacement() const
{
        return gsl_vector_get(theDisplacementVector,0);
}

double SNode::getYDisplacement() const
{
        return gsl_vector_get(theDisplacementVector,1);
}

double SNode::getZDisplacement() const
{
        return gsl_vector_get(theDisplacementVector,2);
}

double SNode::getXRotation() const
{
        return gsl_vector_get(theDisplacementVector,3);
}

double SNode::getYRotation() const
{
        return gsl_vector_get(theDisplacementVector,4);
}

double SNode::getZRotation() const
{
        return gsl_vector_get(theDisplacementVector,5);
}

double SNode::getWarping() const
{
        return gsl_vector_get(theDisplacementVector,6);
}






int SNode::setDisplacementVector(int dof, double value)
{
        gsl_vector_set(theDisplacementVector, dof, value);
        return 0;
}

gsl_vector * SNode::getDisplacementVector()
{
        return theDisplacementVector;
}

bool SNode::canBeRunParallelly() {
	return false;
}





