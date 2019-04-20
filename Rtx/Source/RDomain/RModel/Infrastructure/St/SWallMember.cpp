#include "SWallMember.h"
#include "SSection.h"
#include "SNode.h"
#include <math.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_blas.h>

// Definition of the static stiffness matrix (one instance for all elements)
//gsl_matrix * SWallMember::theGlobalStiffnessMatrix;
//gsl_vector * SWallMember::theGlobalLoadVector;
//gsl_vector * SWallMember::theSectionResponseInGlobalXYZ;


SWallMember::SWallMember(QObject *parent, QString &name)
                : SMember(parent, name)
{
        theSection = 0;
        theNode1 = 0;
        theNode2 = 0;

        Tlg = 0;
        L = 0.0;
        theGlobalStiffnessMatrix = 0;
        theGlobalLoadVector = 0;
        theGeometricStiffness = "No";
        theSectionResponseInGlobalXYZ = 0;

        theAxialForce = 0.0;
        theAxialStress = 0.0;

        // Standing assumption
        numDOFsPerNode = 7;
}

SWallMember::~SWallMember()
{
        if (theGlobalStiffnessMatrix != 0) {
                gsl_matrix_free(theGlobalStiffnessMatrix);
                theGlobalStiffnessMatrix = 0;
        }
        if (theGlobalLoadVector != 0) {
                gsl_vector_free(theGlobalLoadVector);
                theGlobalLoadVector = 0;
        }
        if (Tlg != 0) {
                gsl_matrix_free(Tlg);
                Tlg = 0;
        }
        if (theSectionResponseInGlobalXYZ != 0) {
                gsl_vector_free(theSectionResponseInGlobalXYZ);
                theSectionResponseInGlobalXYZ = 0;
        }
}


QObject * SWallMember::getSection() const
{
        return theSection;
}
void SWallMember::setSection(QObject *value)
{
        theSection = qobject_cast<SSection *>(value);
}




QObject * SWallMember::getNode1() const
{
        return theNode1;
}
void SWallMember::setNode1(QObject *value)
{
        theNode1 = qobject_cast<SNode *>(value);
        updateStructure();
}




QObject * SWallMember::getNode2() const
{
        return theNode2;
}
void SWallMember::setNode2(QObject *value)
{
        theNode2 = qobject_cast<SNode *>(value);
        updateStructure();
}



double SWallMember::getAxialForce() const
{
        return theAxialForce;
}



double SWallMember::getAxialStress() const
{
        return theAxialStress;
}





int SWallMember::getNumNodes()
{
        return 2;
}

QObject * SWallMember::getNode(int i) const
{
        if (i==0) {
                return theNode1;
        }
        else if (i==1) {
                return theNode2;
        }
        else {
                rCritical() << "The truss element was asked for a non-existing node.";
        }
}








QString SWallMember::getGeometricStiffness() const
{
        return theGeometricStiffness;
}

void SWallMember::setGeometricStiffness(QString value)
{
        theGeometricStiffness = value;
}








gsl_matrix * SWallMember::getGlobalStiffnessMatrix()
{
        // Initialize the static matrix
        if (theGlobalStiffnessMatrix == 0) {
                theGlobalStiffnessMatrix = gsl_matrix_calloc(2*numDOFsPerNode, 2*numDOFsPerNode);
        }
        else {
                gsl_matrix_set_zero(theGlobalStiffnessMatrix);
        }

        // Initialize the local stiffness matrix (scope is limited to this method)
        gsl_matrix *Klocal = gsl_matrix_calloc(6,6);

        // Set the value of the local stiffness matrix
        getLocalStiffnessMatrix(Klocal);





        // Do the matrix multiplication Kg = Tlg^T * Kl * Tlg
        gsl_matrix_set_zero(theGlobalStiffnessMatrix);
        gsl_matrix *Ktemp = gsl_matrix_calloc(2*numDOFsPerNode, 6);
        gsl_blas_dgemm(CblasTrans, CblasNoTrans, 1.0, Tlg, Klocal, 0.0, Ktemp);
        gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, Ktemp, Tlg, 0.0, theGlobalStiffnessMatrix);

        // Free vectors and matrices
        gsl_matrix_free(Klocal);
        gsl_matrix_free(Ktemp);

        return theGlobalStiffnessMatrix;
}


gsl_vector * SWallMember::getGlobalLoadVector()
{
        // Initialize the static vector
        if (theGlobalLoadVector == 0) {
                theGlobalLoadVector = gsl_vector_calloc(2*numDOFsPerNode);
        }
        else {
                gsl_vector_set_zero(theGlobalLoadVector);
        }


        /*
        // Get the per-unit-length weight of the cross-section
        double sectionWeight = theSection->getSectionWeight();

        // Compute total weight of the element
        double elementWeight = L*sectionWeight;

        // Place half of the weight onto each node
        gsl_vector_set(theGlobalLoadVector, 2, -0.5*elementWeight);
        gsl_vector_set(theGlobalLoadVector, numDOFsPerNode+2, -0.5*elementWeight);
*/

        return theGlobalLoadVector;
}


int SWallMember::updateResponse()
{
        // Get the global displacement vector from the nodes
        gsl_vector *ug = gsl_vector_calloc(2*numDOFsPerNode);
        gsl_vector *ug1 = theNode1->getDisplacementVector();
        for (int i=0; i<numDOFsPerNode; i++) {
                gsl_vector_set(ug, i, gsl_vector_get(ug1, i));
        }
        gsl_vector *ug2 = theNode2->getDisplacementVector();
        for (int i=0; i<numDOFsPerNode; i++) {
                gsl_vector_set(ug, (i+numDOFsPerNode), gsl_vector_get(ug2, i));
        }

        // Transform to local system
        gsl_vector *ul = gsl_vector_calloc(6);
        gsl_blas_dgemv(CblasNoTrans, 1.0, Tlg, ug, 0.0, ul);


        // Set up the local stiffness matrix
        gsl_matrix *Klocal = gsl_matrix_calloc(6,6);
        getLocalStiffnessMatrix(Klocal);


        // Multiply Fl=Kl*ul to obtain local forces
        gsl_vector *Fl = gsl_vector_calloc(6);
        gsl_blas_dgemv(CblasNoTrans, 1.0, Klocal, ul, 0.0, Fl);


        // Extract the axial force (tension positive)
        theAxialForce = -gsl_vector_get(Fl,0); // Terje check this sign issue

        // Divide by the area to get stress
        double A = theSection->getAValue();
        theAxialStress = theAxialForce/A;


        // Deallocate vectors and matrices
        gsl_matrix_free(Klocal);
        gsl_vector_free(ug);
        gsl_vector_free(ul);
        gsl_vector_free(Fl);

        return 0;
}



int SWallMember::updateStructure()
{
        // Allocate memory for the transformation matrix
        if (Tlg == 0) {
                Tlg = gsl_matrix_calloc(6, 2*numDOFsPerNode);
        }
        else {
                gsl_matrix_set_zero(Tlg);
        }

        // If the nodes are available, compute directional cosines, etc.
        if (theNode1!=0 && theNode2!=0) {

                // Get nodal coordinates
                double xcoord1, xcoord2, ycoord1, ycoord2, zcoord1, zcoord2;
                xcoord1 = theNode1->getXCoordinateValue();
                xcoord2 = theNode2->getXCoordinateValue();
                ycoord1 = theNode1->getYCoordinateValue();
                ycoord2 = theNode2->getYCoordinateValue();
                zcoord1 = theNode1->getZCoordinateValue();
                zcoord2 = theNode2->getZCoordinateValue();

                // Determine directional cosines and element length
                double dx = xcoord2-xcoord1;
                double dy = ycoord2-ycoord1;
                double dz = zcoord2-zcoord1;
                L = sqrt(dx*dx + dy*dy  + dz*dz );

                // Transform to "global" configuration
                double cx = dx/L;
                double cy = dy/L;
                double cz = dz/L;

                // Determine the "v-vector" (let it go in the global z direction, unless the elemnt is vertical)
                double v0, v1, v2;
                if ( (dx==0.0) && (dy==0.0) ) {
                        // The element is vertical, let the v-vector go in the x-direction
                        v0 = 1.0;
                        v1 = 0.0;
                        v2 = 0.0;
                }
                else {
                        v0 = 0.0;
                        v1 = 0.0;
                        v2 = 1.0;
                }

                // Set the x-vector, which forms the first column of the rotation matrix, R
                double x0 = dx/L;
                double x1 = dy/L;
                double x2 = dz/L;

                // Obtain the y-vector, which forms the second column of R, by cross product of x and the user-given vector in the xz-plane
                double y0 = v1*x2 - v2*x1;
                double y1 = v2*x0 - v0*x2;
                double y2 = v0*x1 - v1*x0;

                // Normalize it
                double norm = sqrt(y0*y0+y1*y1+y2*y2);
                y0 /= norm;
                y1 /= norm;
                y2 /= norm;

                // Finally, get the z-vector as the cross product of x and y. This is the third column in the R-matrix
                double z0 = x1*y2 - x2*y1;
                double z1 = x2*y0 - x0*y2;
                double z2 = x0*y1 - x1*y0;

                // Normalize it (is that necessary?)
                norm = sqrt(z0*z0+z1*z1+z2*z2);
                z0 /= norm;
                z1 /= norm;
                z2 /= norm;


                // Transpose the rotation matrix to make it amenable to the contragradient transformation K = T^T * K * T (and not opposite transposes)
                double backupx1 = x1;
                double backupx2 = x2;
                double backupy2 = y2;
                x1 = y0;
                x2 = z0;
                y2 = z1;
                y0 = backupx1;
                z0 = backupx2;
                z1 = backupy2;


                // Set the transformation matrix
                gsl_matrix_set(Tlg, 0, 0, x0);  gsl_matrix_set(Tlg, 0, 1, y0);  gsl_matrix_set(Tlg, 0, 2, z0);
                gsl_matrix_set(Tlg, 1, 0, x1);  gsl_matrix_set(Tlg, 1, 1, y1);  gsl_matrix_set(Tlg, 1, 2, z1);
                gsl_matrix_set(Tlg, 2, 0, x2);  gsl_matrix_set(Tlg, 2, 1, y2);  gsl_matrix_set(Tlg, 2, 2, z2);

                gsl_matrix_set(Tlg, 3, 7, x0);  gsl_matrix_set(Tlg, 3, 8, y0);  gsl_matrix_set(Tlg, 3, 9, z0);
                gsl_matrix_set(Tlg, 4, 7, x1);  gsl_matrix_set(Tlg, 4, 8, y1);  gsl_matrix_set(Tlg, 4, 9, z1);
                gsl_matrix_set(Tlg, 5, 7, x2);  gsl_matrix_set(Tlg, 5, 8, y2);  gsl_matrix_set(Tlg, 5, 9, z2);

        }
        return 0;
}


int SWallMember::getLocalStiffnessMatrix(gsl_matrix *Klocal)
{
        double EA = theSection->getEA();
        double value = EA/L;
        gsl_matrix_set_zero(Klocal);

        // Elastic stiffness
        gsl_matrix_set(Klocal, 0, 0,  value);
        gsl_matrix_set(Klocal, 3, 0, -value);
        gsl_matrix_set(Klocal, 0, 3, -value);
        gsl_matrix_set(Klocal, 3, 3,  value);

        // Geometric stiffnes
        if (theGeometricStiffness == QString("Yes")) {

                double PoverL = -theAxialForce/L;

                gsl_matrix_set(Klocal, 1, 1,  PoverL);
                gsl_matrix_set(Klocal, 4, 1, -PoverL);
                gsl_matrix_set(Klocal, 1, 4, -PoverL);
                gsl_matrix_set(Klocal, 4, 4,  PoverL);

                gsl_matrix_set(Klocal, 2, 2,  PoverL);
                gsl_matrix_set(Klocal, 5, 2, -PoverL);
                gsl_matrix_set(Klocal, 2, 5, -PoverL);
                gsl_matrix_set(Klocal, 5, 5,  PoverL);
        }

        return 0;
}








gsl_vector * SWallMember::getSectionResponseInGlobalXYZ()
{

        // Allocate memory for the response vector
        if (theSectionResponseInGlobalXYZ == 0) {
                theSectionResponseInGlobalXYZ = gsl_vector_calloc(3);
        }
        else {
                gsl_vector_set_zero(theSectionResponseInGlobalXYZ);
        }

        // Set the local response vector
        gsl_vector *localResponse = gsl_vector_calloc(3);
        gsl_vector_set(localResponse, 0, 0.0);
        gsl_vector_set(localResponse, 1, 0.0);
        gsl_vector_set(localResponse, 2, theAxialForce); // Select to plot the axial force diagram in the local z-direction

        // Rotate the local vector into the global coordinate system, by using the 3x3 submatrix from Tlg (its inverse equals its transpose)
        gsl_matrix *RotMat = gsl_matrix_calloc(3,3);
        gsl_matrix_set(RotMat, 0, 0, gsl_matrix_get(Tlg,0, 0));
        gsl_matrix_set(RotMat, 1, 0, gsl_matrix_get(Tlg,0, 1));
        gsl_matrix_set(RotMat, 2, 0, gsl_matrix_get(Tlg,0, 2));

        gsl_matrix_set(RotMat, 0, 1, gsl_matrix_get(Tlg,1, 0));
        gsl_matrix_set(RotMat, 1, 1, gsl_matrix_get(Tlg,1, 1));
        gsl_matrix_set(RotMat, 2, 1, gsl_matrix_get(Tlg,1, 2));

        gsl_matrix_set(RotMat, 0, 2, gsl_matrix_get(Tlg,2, 0));
        gsl_matrix_set(RotMat, 1, 2, gsl_matrix_get(Tlg,2, 1));
        gsl_matrix_set(RotMat, 2, 2, gsl_matrix_get(Tlg,2, 2));


        gsl_blas_dgemv(CblasNoTrans, 1.0, RotMat, localResponse, 0.0, theSectionResponseInGlobalXYZ);


        gsl_vector_free(localResponse);
        gsl_matrix_free(RotMat);

        return theSectionResponseInGlobalXYZ;

}

bool SWallMember::canBeRunParallelly() {
	return false;
}