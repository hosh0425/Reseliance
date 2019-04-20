#ifndef SFrameElement_H
#define SFrameElement_H

#include "SSection.h"
#include "SElement.h"
#include "SNode.h"
#include "RDomain.h"
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>

class SFrameElement : public SElement
{
        Q_OBJECT;

        Q_PROPERTY(QObject *Section READ getSection WRITE setSection);
        Q_PROPERTY(QObject *Node1 READ getNode1 WRITE setNode1);
        Q_PROPERTY(QObject *Node2 READ getNode2 WRITE setNode2);
        Q_PROPERTY(QString UniformLoadInZDirection READ getUniformLoadInZDirection WRITE setUniformLoadInZDirection);
        Q_PROPERTY(QString UniformLoadInYDirection READ getUniformLoadInYDirection WRITE setUniformLoadInYDirection);
        Q_PROPERTY(QString VectorInLocalXZPlane READ getVectorInLocalXZPlane WRITE setVectorInLocalXZPlane);
        Q_PROPERTY(QString ShearDeformation READ getShearDeformation WRITE setShearDeformation);
        Q_PROPERTY(QString GeometricStiffness READ getGeometricStiffness WRITE setGeometricStiffness);
        Q_PROPERTY(double ResultSlider READ getResultSlider WRITE setResultSlider);
        Q_PROPERTY(double AxialForce READ getAxialForce);
        Q_PROPERTY(double My READ getMy);
        Q_PROPERTY(double Vz READ getVz);
        Q_PROPERTY(double Mz READ getMz);
        Q_PROPERTY(double Vy READ getVy);
        Q_PROPERTY(double StVenantTorque READ getStVenantTorque);
        Q_PROPERTY(double WarpingTorque READ getWarpingTorque);
        Q_PROPERTY(double MaxBendingMoment READ getMaxBendingMoment);
        Q_PROPERTY(double MaxShearForce READ getMaxShearForce);
        Q_PROPERTY(double MaxAxialStress READ getMaxAxialStress);
        Q_PROPERTY(double MaxShearStress READ getMaxShearStress);

public:
        SFrameElement(QObject *parent, QString &name);
        ~SFrameElement();

        QObject *getSection() const;
        void setSection(QObject *value);

        QObject *getNode1() const;
        void setNode1(QObject *value);

        QObject *getNode2() const;
        void setNode2(QObject *value);

        QString getShearDeformation() const;
        void setShearDeformation(QString value);

        QString getGeometricStiffness() const;
        void setGeometricStiffness(QString value);

        QString getVectorInLocalXZPlane() const;
        void setVectorInLocalXZPlane(QString value);

        QString getUniformLoadInZDirection();
        void setUniformLoadInZDirection(QString value);
        double getUniformLoadInZDirectionValue();

        QString getUniformLoadInYDirection();
        void setUniformLoadInYDirection(QString value);
        double getUniformLoadInYDirectionValue();

        double getResultSlider() const;
        void setResultSlider(double value);

        double getAxialForce() const;
        double getMy() const;
        double getVz() const;
        double getMz() const;
        double getVy() const;
        double getStVenantTorque() const;
        double getWarpingTorque() const;
        double getMaxBendingMoment() const;
        double getMaxShearForce() const;
        double getMaxAxialStress() const;
        double getMaxShearStress() const;

        int getNumNodes();
        QObject *getNode(int i) const;
        gsl_matrix * getGlobalStiffnessMatrix();
        gsl_vector * getGlobalLoadVector();
        int updateResponse();
        double getSectionResponse(QString type, double xi);
        gsl_vector * getSectionResponseInGlobalXYZ(QString type, double xi);

        QStringList getActionList();
		bool canBeRunParallelly();


private slots:
        void plotMy();
        void plotVz();
        void plotMz();
        void plotVy();
        void plotdwdx();
        void plotdvdx();
        void plotw();
        void plotv();
        void plotN();

private:
        void plot(QString type);

        int updateStructure();
        int getLocalStiffnessMatrix(gsl_matrix *Klocal);

        SSection *theSection;
        SNode *theNode1;
        SNode *theNode2;
        QString theVectorInLocalXZPlane;
        RPointer<RParameter> theUniformLoadInZDirectionParameter;
        double theUniformLoadInZDirectionDouble;
        RPointer<RParameter> theUniformLoadInYDirectionParameter;
        double theUniformLoadInYDirectionDouble;
        QString theShearDeformation;
        QString theGeometricStiffness;

        gsl_matrix *Tlg;
        double L;
        gsl_matrix *theGlobalStiffnessMatrix;
        gsl_vector *theGlobalLoadVector;
        gsl_vector *theSectionResponseInGlobalXYZ;

        gsl_vector *ul;
        double theAxialForce;
        double theMy;
        double theVz;
        double theMz;
        double theVy;
        double theStVenantTorque;
        double theWarpingTorque;
        double theMaxBendingMoment;
        double theMaxShearForce;
        double theMaxAxialStress;
        double theMaxShearStress;

        double theResultSlider;
        int numDOFsPerNode;

};

#endif
