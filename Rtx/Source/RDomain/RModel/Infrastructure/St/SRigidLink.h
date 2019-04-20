#ifndef SRigidLink_H
#define SRigidLink_H

#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include "SNode.h"
#include "RObject.h"
#include "RParameter.h"

class SRigidLink : public RObject
{
        Q_OBJECT;

        Q_PROPERTY(QObject *IndependentNode READ getIndependentNode WRITE setIndependentNode);
        Q_PROPERTY(QObject *DependentNode READ getDependentNode WRITE setDependentNode);
        Q_PROPERTY(QString LinkXDisplacement READ getLinkXDisplacement WRITE setLinkXDisplacement);
        Q_PROPERTY(QString LinkYDisplacement READ getLinkYDisplacement WRITE setLinkYDisplacement);
        Q_PROPERTY(QString LinkZDisplacement READ getLinkZDisplacement WRITE setLinkZDisplacement);
        Q_PROPERTY(QString LinkXRotation READ getLinkXRotation WRITE setLinkXRotation);
        Q_PROPERTY(QString LinkYRotation READ getLinkYRotation WRITE setLinkYRotation);
        Q_PROPERTY(QString LinkZRotation READ getLinkZRotation WRITE setLinkZRotation);
        Q_PROPERTY(QString LinkWarping READ getLinkWarping WRITE setLinkWarping);

public:
        SRigidLink(QObject *parent, QString &name);
        ~SRigidLink();

        QObject *getIndependentNode() const;
        void setIndependentNode(QObject *value);
        QObject *getDependentNode() const;
        void setDependentNode(QObject *value);

        QString getLinkXDisplacement() const;
        void setLinkXDisplacement(QString value);

        QString getLinkYDisplacement() const;
        void setLinkYDisplacement(QString value);

        QString getLinkZDisplacement() const;
        void setLinkZDisplacement(QString value);

        QString getLinkXRotation() const;
        void setLinkXRotation(QString value);

        QString getLinkYRotation() const;
        void setLinkYRotation(QString value);

        QString getLinkZRotation() const;
        void setLinkZRotation(QString value);

        QString getLinkWarping() const;
        void setLinkWarping(QString value);

        gsl_vector * getBoundaryConditionVector();
		bool canBeRunParallelly();



private:
        QString theLinkXDisplacement;
        QString theLinkYDisplacement;
        QString theLinkZDisplacement;
        QString theLinkXRotation;
        QString theLinkYRotation;
        QString theLinkZRotation;
        QString theLinkWarping;

        SNode *theIndependentNode;
        SNode *theDependentNode;

        gsl_vector *theLinkVector;
        int numDOFsPerNode;
};

#endif

