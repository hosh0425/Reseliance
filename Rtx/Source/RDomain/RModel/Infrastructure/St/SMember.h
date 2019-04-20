#ifndef SMember_H
#define SMember_H

#include "SNode.h"
#include "RObject.h"
#include "RDomain.h"

#include <QObject>

class SMember : public RObject
{
        Q_OBJECT

public:
        SMember(QObject *parent, QString &name);
        ~SMember();

        virtual int getNumNodes() = 0;
        virtual QObject *getNode(int i) const = 0;
        virtual gsl_vector * getGlobalLoadVector() = 0;
        virtual gsl_matrix * getGlobalStiffnessMatrix() = 0;
        virtual int updateResponse() = 0;

private:

};

#endif
