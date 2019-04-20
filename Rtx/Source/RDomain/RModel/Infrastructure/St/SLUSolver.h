#ifndef SLUSolver_H
#define SLUSolver_H

#include "SSOESolver.h"
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>

class SLUSolver : public SSOESolver
{
        Q_OBJECT

public:
        SLUSolver(QObject *parent, QString &name);
        ~SLUSolver();

        int solveSOE(gsl_matrix *K, gsl_vector *F, gsl_vector *u);
		bool canBeRunParallelly();


private:

};

#endif
