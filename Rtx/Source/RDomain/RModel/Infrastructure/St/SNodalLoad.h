#ifndef SNodalLoad_H
#define SNodalLoad_H

#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include "RObject.h"
#include "RParameter.h"
#include "SNode.h"

class SNodalLoad : public RObject
{
        Q_OBJECT;
        Q_PROPERTY(QObject *Node READ getNode WRITE setNode);
        Q_PROPERTY(QString Fx READ getFx WRITE setFx);
        Q_PROPERTY(QString Fy READ getFy WRITE setFy);
        Q_PROPERTY(QString Fz READ getFz WRITE setFz);
        Q_PROPERTY(QString Mx READ getMx WRITE setMx);
        Q_PROPERTY(QString My READ getMy WRITE setMy);
        Q_PROPERTY(QString Mz READ getMz WRITE setMz);

public:
        SNodalLoad(QObject *parent, QString &name);
        ~SNodalLoad();

        QObject *getNode() const;
        void setNode(QObject *value);

        QString getFx();
        void setFx(QString value);
        double getFxValue();

        QString getFy() ;
        void setFy(QString value);
        double getFyValue();

        QString getFz();
        void setFz(QString value);
        double getFzValue();

        QString getMx();
        void setMx(QString value);
        double getMxValue();

        QString getMy() ;
        void setMy(QString value);
        double getMyValue();

        QString getMz();
        void setMz(QString value);
        double getMzValue();

        gsl_vector * getLoadVector();
		bool canBeRunParallelly();


private:
        RPointer<RParameter> theFxParameter;
        double theFxDouble;

        RPointer<RParameter> theFyParameter;
        double theFyDouble;

        RPointer<RParameter> theFzParameter;
        double theFzDouble;

        RPointer<RParameter> theMxParameter;
        double theMxDouble;

        RPointer<RParameter> theMyParameter;
        double theMyDouble;

        RPointer<RParameter> theMzParameter;
        double theMzDouble;

        SNode *theNode;
        gsl_vector *theLoadVector;
};

#endif

