#include "SLUSolver.h"

#include <QCoreApplication>

#include "RDomain.h"
#include "RMainWindow.h"
#include <gsl/gsl_blas.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_linalg.h>

SLUSolver::SLUSolver(QObject *parent, QString &name)
                : SSOESolver(parent, name)
{
}

SLUSolver::~SLUSolver()
{
}


int SLUSolver::solveSOE(gsl_matrix *K, gsl_vector *F, gsl_vector *u)
{
        // Get number of free degrees of freedom
        int numDOFs = F->size;

        // Check the determinant to see if the SOE is solvable
        int signum = 0;
        gsl_permutation *p = gsl_permutation_alloc(numDOFs);
        gsl_linalg_LU_decomp(K, p, &signum);
        double determinant = gsl_linalg_LU_det(K,signum);

        // Give user some information, if wanted by output display level
        //if (theOutputDisplayLevel >= RObject::Moderate) {
        //        rDebug() << "The determinant of the stiffness matrix is";
        //        rDebug() << "";
        //}


        // Solve the SOE if it is solvable
        if (determinant <= 0.0) {
                rCritical() << "Error: The stiffness matrix is singular! Try restraining some DOFs.";
                return -1;
        }
        else {
                gsl_linalg_LU_solve (K, p, F, u);
        }


        // An alternative approach for solving the system of equations:
        //gsl_linalg_HH_solve(K,F,u);

        gsl_permutation_free(p);

        return 0;
}

bool SLUSolver::canBeRunParallelly() {
	return false;
}