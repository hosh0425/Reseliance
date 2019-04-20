#ifndef STrussElement_H
#define STrussElement_H

#include "SSection.h"
#include "SElement.h"
#include "SNode.h"
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>

class STrussElement : public SElement
{
        Q_OBJECT;

        Q_PROPERTY(QObject *Section READ getSection WRITE setSection);
        Q_PROPERTY(QObject *Node1 READ getNode1 WRITE setNode1);
        Q_PROPERTY(QObject *Node2 READ getNode2 WRITE setNode2);
        Q_PROPERTY(QString GeometricStiffness READ getGeometricStiffness WRITE setGeometricStiffness);
        Q_PROPERTY(double AxialForce READ getAxialForce);
        Q_PROPERTY(double AxialStress READ getAxialStress);

public:
        STrussElement(QObject *parent, QString &name);
        ~STrussElement();

        QObject *getSection() const;
        void setSection(QObject *value);

        QObject *getNode1() const;
        void setNode1(QObject *value);

        QObject *getNode2() const;
        void setNode2(QObject *value);

        QString getGeometricStiffness() const;
        void setGeometricStiffness(QString value);

        double getAxialForce() const;
        double getAxialStress() const;

        int getNumNodes();
        QObject *getNode(int i) const;
        gsl_matrix * getGlobalStiffnessMatrix();
        gsl_vector * getGlobalLoadVector();
        int updateResponse();
        gsl_vector * getSectionResponseInGlobalXYZ();
		bool canBeRunParallelly();



private:
        int updateStructure();
        int getLocalStiffnessMatrix(gsl_matrix *Klocal);

        SSection *theSection;
        SNode *theNode1;
        SNode *theNode2;
        QString theGeometricStiffness;
        gsl_vector *theSectionResponseInGlobalXYZ;

        gsl_matrix *Tlg;
        double L;
        gsl_matrix *theGlobalStiffnessMatrix;
        gsl_vector *theGlobalLoadVector;

        double theAxialForce;
        double theAxialStress;
        int numDOFsPerNode;

};

#endif
