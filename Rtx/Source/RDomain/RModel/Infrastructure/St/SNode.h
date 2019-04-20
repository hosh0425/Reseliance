#ifndef SNode_H
#define SNode_H

#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include "RObject.h"
#include "RParameter.h"

class SNode : public RObject
{
        Q_OBJECT;

        Q_PROPERTY(QString XCoordinate READ getXCoordinate WRITE setXCoordinate);
        Q_PROPERTY(QString YCoordinate READ getYCoordinate WRITE setYCoordinate);
        Q_PROPERTY(QString ZCoordinate READ getZCoordinate WRITE setZCoordinate);
        Q_PROPERTY(double XDisplacement READ getXDisplacement);
        Q_PROPERTY(double YDisplacement READ getYDisplacement);
        Q_PROPERTY(double ZDisplacement READ getZDisplacement);
        Q_PROPERTY(double XRotation READ getXRotation);
        Q_PROPERTY(double YRotation READ getYRotation);
        Q_PROPERTY(double ZRotation READ getZRotation);
        Q_PROPERTY(double Warping READ getWarping);

public:
        SNode(QObject *parent, QString &name);
        ~SNode();

        QString getXCoordinate();
        void setXCoordinate(QString value);
        double getXCoordinateValue();

        QString getYCoordinate() ;
        void setYCoordinate(QString value);
        double getYCoordinateValue();

        QString getZCoordinate();
        void setZCoordinate(QString value);
        double getZCoordinateValue();

        double getXDisplacement() const;
        double getYDisplacement() const;
        double getZDisplacement() const;
        double getXRotation() const;
        double getYRotation() const;
        double getZRotation() const;
        double getWarping() const;

        int setDisplacementVector(int dof, double value);
        gsl_vector * getDisplacementVector();
		bool canBeRunParallelly();


private:
        RPointer<RParameter> theXCoordinateParameter;
        double theXCoordinateDouble;

        RPointer<RParameter> theYCoordinateParameter;
        double theYCoordinateDouble;

        RPointer<RParameter> theZCoordinateParameter;
        double theZCoordinateDouble;

        gsl_vector *theDisplacementVector;

        int numDOFsPerNode;
};

#endif

