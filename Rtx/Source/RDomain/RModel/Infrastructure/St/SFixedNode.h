#ifndef SFixedNode_H
#define SFixedNode_H

#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include "SNode.h"
#include "RObject.h"
#include "RParameter.h"

class SFixedNode : public RObject
{
        Q_OBJECT;

        Q_PROPERTY(QObject *Node READ getNode WRITE setNode);

        Q_PROPERTY(QString XDisplacement READ getXDisplacement WRITE setXDisplacement);
        Q_PROPERTY(QString YDisplacement READ getYDisplacement WRITE setYDisplacement);
        Q_PROPERTY(QString ZDisplacement READ getZDisplacement WRITE setZDisplacement);
        Q_PROPERTY(QString XRotation READ getXRotation WRITE setXRotation);
        Q_PROPERTY(QString YRotation READ getYRotation WRITE setYRotation);
        Q_PROPERTY(QString ZRotation READ getZRotation WRITE setZRotation);
        Q_PROPERTY(QString Warping READ getWarping WRITE setWarping);
        Q_PROPERTY(double XForce READ getXForce);
        Q_PROPERTY(double YForce READ getYForce);
        Q_PROPERTY(double ZForce READ getZForce);
        Q_PROPERTY(double XMoment READ getXMoment);
        Q_PROPERTY(double YMoment READ getYMoment);
        Q_PROPERTY(double ZMoment READ getZMoment);
        Q_PROPERTY(double WarpingTorque READ getWarpingTorque);

public:
        SFixedNode(QObject *parent, QString &name);
        ~SFixedNode();

        QObject *getNode() const;
        void setNode(QObject *value);

        QString getXDisplacement() const;
        void setXDisplacement(QString value);

        QString getYDisplacement() const;
        void setYDisplacement(QString value);

        QString getZDisplacement() const;
        void setZDisplacement(QString value);

        QString getXRotation() const;
        void setXRotation(QString value);

        QString getYRotation() const;
        void setYRotation(QString value);

        QString getZRotation() const;
        void setZRotation(QString value);

        QString getWarping() const;
        void setWarping(QString value);

        double getXForce() const;
        double getYForce() const;
        double getZForce() const;
        double getXMoment() const;
        double getYMoment() const;
        double getZMoment() const;
        double getWarpingTorque() const;

        gsl_vector * getBoundaryConditionVector();
        int setReactionForceVector(int dof, double value);
		bool canBeRunParallelly();




private:
        QString theXDisplacement;
        QString theYDisplacement;
        QString theZDisplacement;
        QString theXRotation;
        QString theYRotation;
        QString theZRotation;
        QString theWarping;

        SNode *theNode;
        gsl_vector *theBoundaryConditionVector;
        int numDOFsPerNode;
        gsl_vector *theReactionForceVector;
};

#endif

