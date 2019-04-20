#ifndef SElement_H
#define SElement_H

#include "SNode.h"
#include "RObject.h"
#include "RObject.h"

#include <QObject>

class SElement : public RObject
{
        Q_OBJECT

public:
        SElement(QObject *parent, QString &name);
        ~SElement();

        virtual int getNumNodes() = 0;
        virtual QObject *getNode(int i) const = 0;
        virtual gsl_vector * getGlobalLoadVector() = 0;
        virtual gsl_matrix * getGlobalStiffnessMatrix() = 0;
        virtual int updateResponse() = 0;

private:

};

#endif
