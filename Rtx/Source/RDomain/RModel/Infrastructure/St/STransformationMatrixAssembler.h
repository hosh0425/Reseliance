#ifndef STransformationMatrixAssembler_H
#define STransformationMatrixAssembler_H

#include "SAssembler.h"
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>

class STransformationMatrixAssembler : public SAssembler
{
        Q_OBJECT

public:
        STransformationMatrixAssembler(QObject *parent, QString &name);
        ~STransformationMatrixAssembler();

	gsl_matrix * getStiffnessMatrix();
	gsl_vector * getLoadVector();
	int setDisplacements(gsl_vector *x);
	bool canBeRunParallelly();

private:
        int setTaf();

        gsl_matrix *Kfinal;
        gsl_matrix *Kall;
        gsl_vector *theLoadVector;
        gsl_matrix *Taf;
        gsl_matrix *Td;
        int numDOFsPerNode;
};

#endif
