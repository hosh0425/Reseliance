#ifndef SQuad4Element_H
#define SQuad4Element_H

#include "SMaterial.h"
#include "SElement.h"
#include "SNode.h"
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>

class SQuad4Element : public SElement
{
        Q_OBJECT;

        Q_PROPERTY(QObject *Material READ getMaterial WRITE setMaterial);
        Q_PROPERTY(QObject *Node1 READ getNode1 WRITE setNode1);
        Q_PROPERTY(QObject *Node2 READ getNode2 WRITE setNode2);
        Q_PROPERTY(QObject *Node3 READ getNode3 WRITE setNode3);
        Q_PROPERTY(QObject *Node4 READ getNode4 WRITE setNode4);
        Q_PROPERTY(QString Thickness READ getThickness WRITE setThickness);
        Q_PROPERTY(QString Quadrature READ getQuadrature WRITE setQuadrature);
        Q_PROPERTY(double XiResultSlider READ getXiResultSlider WRITE setXiResultSlider);
        Q_PROPERTY(double EtaResultSlider READ getEtaResultSlider WRITE setEtaResultSlider);
        Q_PROPERTY(double AxialStressX READ getAxialStressX);
        Q_PROPERTY(double AxialStressY READ getAxialStressY);
        Q_PROPERTY(double ShearStress READ getShearStressResult);

public:
        SQuad4Element(QObject *parent, QString &name);
        ~SQuad4Element();

        QObject *getMaterial() const;
        void setMaterial(QObject *value);

        QObject *getNode1() const;
        void setNode1(QObject *value);

        QObject *getNode2() const;
        void setNode2(QObject *value);

        QObject *getNode3() const;
        void setNode3(QObject *value);

        QObject *getNode4() const;
        void setNode4(QObject *value);

        double getXiResultSlider() const;
        void setXiResultSlider(double value);
        double getEtaResultSlider() const;
        void setEtaResultSlider(double value);

        QString getThickness();
        void setThickness(QString value);
        double getThicknessValue();

        QString getQuadrature() const;
        void setQuadrature(QString value);

        double getAxialStressX() const;
        double getAxialStressY() const;
        double getShearStressResult() const;

        int getNumNodes();
        QObject *getNode(int i) const;
        gsl_matrix * getGlobalStiffnessMatrix();
        gsl_vector * getGlobalLoadVector();
        int updateResponse();
		bool canBeRunParallelly();

        
private:
        int updateStructure();
        int getLocalStiffnessMatrix(gsl_matrix *Klocal);
        int getBmatrixAndJacobianDeterminant(double xi, double eta, gsl_matrix *B, double &jacDet);

        SMaterial *theMaterial;
        SNode *theNode1;
        SNode *theNode2;
        SNode *theNode3;
        SNode *theNode4;
        RPointer<RParameter> theThicknessParameter;
        double theThicknessDouble;
        QString theQuadrature;

        double theAxialStressX;
        double theAxialStressY;
        double theShearStressResult;

        double theXiResultSlider;
        double theEtaResultSlider;

        gsl_matrix *Tlg;
        gsl_matrix *theGlobalStiffnessMatrix;
        gsl_vector *theGlobalLoadVector;
        int numDOFsPerNode;
};

#endif
