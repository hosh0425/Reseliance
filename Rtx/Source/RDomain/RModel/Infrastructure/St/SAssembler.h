#ifndef SAssembler_H
#define SAssembler_H

#include "RObject.h"
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>

#include <QList>

class RDomain;

class SAssembler : public RObject
{
        Q_OBJECT;

        Q_PROPERTY(ROutputDisplayLevelType OutputDisplayLevel READ getOutputDisplayLevel WRITE setOutputDisplayLevel);

public:
        SAssembler(QObject *parent, QString &name);
        ~SAssembler();

        virtual gsl_matrix * getStiffnessMatrix() = 0;
        virtual gsl_vector * getLoadVector() = 0;
        virtual int setDisplacements(gsl_vector *x) = 0;

        void setDomain(RDomain *passedDomain);

protected:
        RDomain *theDomain;

private:
};

#endif
