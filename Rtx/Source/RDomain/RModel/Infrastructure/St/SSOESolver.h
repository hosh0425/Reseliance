#ifndef SSOESolver_H
#define SSOESolver_H

#include "RObject.h"
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>

class RDomain;

class SSOESolver : public RObject
{
        Q_OBJECT;

        Q_PROPERTY(ROutputDisplayLevelType OutputDisplayLevel READ getOutputDisplayLevel WRITE setOutputDisplayLevel);

public:
        SSOESolver(QObject *parent, QString &name);
        ~SSOESolver();

        virtual int solveSOE(gsl_matrix *K, gsl_vector *F, gsl_vector *u) = 0;

protected:

private:
	
};

#endif
