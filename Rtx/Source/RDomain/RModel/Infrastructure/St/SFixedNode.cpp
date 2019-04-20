#include "SFixedNode.h"
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>

#include "RObject.h"
#include "RParameter.h"
#include "SNode.h"


SFixedNode::SFixedNode(QObject *parent, QString &name)
                : RObject(parent, name)
{
        theXDisplacement = "Fixed";
        theYDisplacement = "Fixed";
        theZDisplacement = "Fixed";
        theXRotation = "Fixed";
        theYRotation = "Fixed";
        theZRotation = "Fixed";
        theWarping = "Fixed";

        numDOFsPerNode = 7;
        theNode = 0;
        theBoundaryConditionVector = gsl_vector_calloc(numDOFsPerNode);
        gsl_vector_set_all(theBoundaryConditionVector, 1.0);
        theReactionForceVector = gsl_vector_calloc(numDOFsPerNode);
}

SFixedNode::~SFixedNode()
{
        if (theBoundaryConditionVector!=0) {
                gsl_vector_free(theBoundaryConditionVector);
        }

        gsl_vector_free(theReactionForceVector);

}





QString SFixedNode::getXDisplacement() const
{
        return theXDisplacement;
}

void SFixedNode::setXDisplacement(QString value)
{
        theXDisplacement = value;
        if (value == QString("Fixed")) {
                gsl_vector_set(theBoundaryConditionVector, 0, 1.0);
        }
        else if (value == QString("Free")) {
                gsl_vector_set(theBoundaryConditionVector, 0, 0.0);
        }
        else  {
                rCritical() << "Error: Illegal boundary condition provided in one of the FixedNode objects.";
        }
}

QString SFixedNode::getYDisplacement() const
{
        return theYDisplacement;
}

void SFixedNode::setYDisplacement(QString value)
{
        theYDisplacement = value;
        if (value == QString("Fixed")) {
                gsl_vector_set(theBoundaryConditionVector, 1, 1.0);
        }
        else if (value == QString("Free")) {
                gsl_vector_set(theBoundaryConditionVector, 1, 0.0);
        }
        else  {
                rCritical() << "Error: Illegal boundary condition provided in one of the FixedNode objects.";
        }
}

QString SFixedNode::getZDisplacement() const
{
        return theZDisplacement;
}

void SFixedNode::setZDisplacement(QString value)
{
        theZDisplacement = value;
        if (value == QString("Fixed")) {
                gsl_vector_set(theBoundaryConditionVector, 2, 1.0);
        }
        else if (value == QString("Free")) {
                gsl_vector_set(theBoundaryConditionVector, 2, 0.0);
        }
        else  {
                rCritical() << "Error: Illegal boundary condition provided in one of the FixedNode objects.";
        }
}

QString SFixedNode::getXRotation() const
{
        return theXRotation;
}

void SFixedNode::setXRotation(QString value)
{
        theXRotation = value;
        if (value == QString("Fixed")) {
                gsl_vector_set(theBoundaryConditionVector, 3, 1.0);
        }
        else if (value == QString("Free")) {
                gsl_vector_set(theBoundaryConditionVector, 3, 0.0);
        }
        else  {
                rCritical() << "Error: Illegal boundary condition provided in one of the FixedNode objects.";
        }
}

QString SFixedNode::getYRotation() const
{
        return theYRotation;
}

void SFixedNode::setYRotation(QString value)
{
        theYRotation = value;
        if (value == QString("Fixed")) {
                gsl_vector_set(theBoundaryConditionVector, 4, 1.0);
        }
        else if (value == QString("Free")) {
                gsl_vector_set(theBoundaryConditionVector, 4, 0.0);
        }
        else  {
                rCritical() << "Error: Illegal boundary condition provided in one of the FixedNode objects.";
        }
}

QString SFixedNode::getZRotation() const
{
        return theZRotation;
}

void SFixedNode::setZRotation(QString value)
{
        theZRotation = value;
        if (value == QString("Fixed")) {
                gsl_vector_set(theBoundaryConditionVector, 5, 1.0);
        }
        else if (value == QString("Free")) {
                gsl_vector_set(theBoundaryConditionVector, 5, 0.0);
        }
        else  {
                rCritical() << "Error: Illegal boundary condition provided in one of the FixedNode objects.";
        }
}

QString SFixedNode::getWarping() const
{
        return theWarping;
}

void SFixedNode::setWarping(QString value)
{
        theWarping = value;
        if (value == QString("Fixed")) {
                gsl_vector_set(theBoundaryConditionVector, 6, 1.0);
        }
        else if (value == QString("Free")) {
                gsl_vector_set(theBoundaryConditionVector, 6, 0.0);
        }
        else  {
                rCritical() << "Error: Illegal boundary condition provided in one of the FixedNode objects.";
        }
}





QObject * SFixedNode::getNode() const
{
        return theNode;
}
void SFixedNode::setNode(QObject *value)
{
        theNode = qobject_cast<SNode *>(value);
}





gsl_vector * SFixedNode::getBoundaryConditionVector()
{
        return theBoundaryConditionVector;
}




double SFixedNode::getXForce() const
{
        return gsl_vector_get(theReactionForceVector,0);
}

double SFixedNode::getYForce() const
{
        return gsl_vector_get(theReactionForceVector,1);
}

double SFixedNode::getZForce() const
{
        return gsl_vector_get(theReactionForceVector,2);
}

double SFixedNode::getXMoment() const
{
        return gsl_vector_get(theReactionForceVector,3);
}

double SFixedNode::getYMoment() const
{
        return gsl_vector_get(theReactionForceVector,4);
}

double SFixedNode::getZMoment() const
{
        return gsl_vector_get(theReactionForceVector,5);
}

double SFixedNode::getWarpingTorque() const
{
        return gsl_vector_get(theReactionForceVector,6);
}





int SFixedNode::setReactionForceVector(int dof, double value)
{
        gsl_vector_set(theReactionForceVector, dof, value);
        return 0;
}

bool SFixedNode::canBeRunParallelly() {
	return false;
}

