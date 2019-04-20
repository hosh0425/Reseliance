#include "SRigidLink.h"
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>

#include "RObject.h"
#include "RParameter.h"
#include "SNode.h"


SRigidLink::SRigidLink(QObject *parent, QString &name)
                : RObject(parent, name)
{
        theLinkXDisplacement = "Yes";
        theLinkYDisplacement = "Yes";
        theLinkZDisplacement = "Yes";
        theLinkXRotation = "No";
        theLinkYRotation = "No";
        theLinkZRotation = "No";
        theLinkWarping = "No";

        numDOFsPerNode = 7;
        theIndependentNode = 0;
        theDependentNode = 0;
        theLinkVector = gsl_vector_calloc(numDOFsPerNode);
        gsl_vector_set(theLinkVector, 0, 1.0);
        gsl_vector_set(theLinkVector, 1, 1.0);
        gsl_vector_set(theLinkVector, 2, 1.0);
        gsl_vector_set(theLinkVector, 3, 0.0);
        gsl_vector_set(theLinkVector, 4, 0.0);
        gsl_vector_set(theLinkVector, 5, 0.0);
        gsl_vector_set(theLinkVector, 6, 0.0);
}

SRigidLink::~SRigidLink()
{
        if (theLinkVector!=0) {
                gsl_vector_free(theLinkVector);
        }
}





QString SRigidLink::getLinkXDisplacement() const
{
        return theLinkXDisplacement;
}

void SRigidLink::setLinkXDisplacement(QString value)
{
        // NOTICE for coding of linear dependency: simply replace the number 1.0 with alpha; the Assembler is already prepared for this


        theLinkXDisplacement = value;
        if (value == QString("Yes")) {
                gsl_vector_set(theLinkVector, 0, 1.0);
        }
        else if (value == QString("No")) {
                gsl_vector_set(theLinkVector, 0, 0.0);
        }
        else  {
                rCritical() << "Error: Illegal boundary condition provided in one of the RigidLink objects.";
        }
}

QString SRigidLink::getLinkYDisplacement() const
{
        return theLinkYDisplacement;
}

void SRigidLink::setLinkYDisplacement(QString value)
{
        theLinkYDisplacement = value;
        if (value == QString("Yes")) {
                gsl_vector_set(theLinkVector, 1, 1.0);
        }
        else if (value == QString("No")) {
                gsl_vector_set(theLinkVector, 1, 0.0);
        }
        else  {
                rCritical() << "Error: Illegal boundary condition provided in one of the RigidLink objects.";
        }
}

QString SRigidLink::getLinkZDisplacement() const
{
        return theLinkZDisplacement;
}

void SRigidLink::setLinkZDisplacement(QString value)
{
        theLinkZDisplacement = value;
        if (value == QString("Yes")) {
                gsl_vector_set(theLinkVector, 2, 1.0);
        }
        else if (value == QString("No")) {
                gsl_vector_set(theLinkVector, 2, 0.0);
        }
        else  {
                rCritical() << "Error: Illegal boundary condition provided in one of the RigidLink objects.";
        }
}

QString SRigidLink::getLinkXRotation() const
{
        return theLinkXRotation;
}

void SRigidLink::setLinkXRotation(QString value)
{
        theLinkXRotation = value;
        if (value == QString("Yes")) {
                gsl_vector_set(theLinkVector, 3, 1.0);
        }
        else if (value == QString("No")) {
                gsl_vector_set(theLinkVector, 3, 0.0);
        }
        else  {
                rCritical() << "Error: Illegal boundary condition provided in one of the RigidLink objects.";
        }
}

QString SRigidLink::getLinkYRotation() const
{
        return theLinkYRotation;
}

void SRigidLink::setLinkYRotation(QString value)
{
        theLinkYRotation = value;
        if (value == QString("Yes")) {
                gsl_vector_set(theLinkVector, 4, 1.0);
        }
        else if (value == QString("No")) {
                gsl_vector_set(theLinkVector, 4, 0.0);
        }
        else  {
                rCritical() << "Error: Illegal boundary condition provided in one of the RigidLink objects.";
        }
}

QString SRigidLink::getLinkZRotation() const
{
        return theLinkZRotation;
}

void SRigidLink::setLinkZRotation(QString value)
{
        theLinkZRotation = value;
        if (value == QString("Yes")) {
                gsl_vector_set(theLinkVector, 5, 1.0);
        }
        else if (value == QString("No")) {
                gsl_vector_set(theLinkVector, 5, 0.0);
        }
        else  {
                rCritical() << "Error: Illegal boundary condition provided in one of the RigidLink objects.";
        }
}

QString SRigidLink::getLinkWarping() const
{
        return theLinkWarping;
}

void SRigidLink::setLinkWarping(QString value)
{
        theLinkWarping = value;
        if (value == QString("Yes")) {
                gsl_vector_set(theLinkVector, 6, 1.0);
        }
        else if (value == QString("No")) {
                gsl_vector_set(theLinkVector, 6, 0.0);
        }
        else  {
                rCritical() << "Error: Illegal boundary condition provided in one of the RigidLink objects.";
        }
}





QObject * SRigidLink::getIndependentNode() const
{
        return theIndependentNode;
}
void SRigidLink::setIndependentNode(QObject *value)
{
        theIndependentNode = qobject_cast<SNode *>(value);
}



QObject * SRigidLink::getDependentNode() const
{
        return theDependentNode;
}
void SRigidLink::setDependentNode(QObject *value)
{
        theDependentNode = qobject_cast<SNode *>(value);
}





gsl_vector * SRigidLink::getBoundaryConditionVector()
{
        return theLinkVector;
}

bool SRigidLink::canBeRunParallelly() {
	return false;
}
