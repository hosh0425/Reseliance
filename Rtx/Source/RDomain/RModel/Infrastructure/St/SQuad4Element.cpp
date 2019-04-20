#include "SQuad4Element.h"
#include "SMaterial.h"
#include "SNode.h"
#include <math.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_blas.h>

// Definition of the static stiffness matrix (one instance for all elements)
//gsl_matrix * SQuad4Element::theGlobalStiffnessMatrix;
//gsl_vector * SQuad4Element::theGlobalLoadVector;


SQuad4Element::SQuad4Element(QObject *parent, QString &name)
                : SElement(parent, name)
{
        numDOFsPerNode = 7;
        theMaterial = 0;
        theNode1 = 0;
        theNode2 = 0;
        theNode3 = 0;
        theNode4 = 0;
        
        //$$ theThicknessParameter = 0;
        theThicknessDouble = 0.0;

        theAxialStressX = 0.0;
        theAxialStressY = 0.0;
        theShearStressResult = 0.0;

        theXiResultSlider = 0.0;
        theEtaResultSlider = 0.0;

        theQuadrature = "Two by Two Gauss";
        
        Tlg = 0;
        theGlobalStiffnessMatrix = 0;
        theGlobalLoadVector = 0;
}

SQuad4Element::~SQuad4Element()
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
}




QObject * SQuad4Element::getMaterial() const
{
        return theMaterial;
}
void SQuad4Element::setMaterial(QObject *value)
{
        theMaterial = qobject_cast<SMaterial *>(value);
}







QObject * SQuad4Element::getNode1() const
{
        return theNode1;
}
void SQuad4Element::setNode1(QObject *value)
{
        theNode1 = qobject_cast<SNode *>(value);
        updateStructure();
}




QObject * SQuad4Element::getNode2() const
{
        return theNode2;
}
void SQuad4Element::setNode2(QObject *value)
{
        theNode2 = qobject_cast<SNode *>(value);
        updateStructure();
}






QObject * SQuad4Element::getNode3() const
{
        return theNode3;
}
void SQuad4Element::setNode3(QObject *value)
{
        theNode3 = qobject_cast<SNode *>(value);
        updateStructure();
}






QObject * SQuad4Element::getNode4() const
{
        return theNode4;
}
void SQuad4Element::setNode4(QObject *value)
{
        theNode4 = qobject_cast<SNode *>(value);
        updateStructure();
}







QString SQuad4Element::getThickness()
{
        return getStringParameter(&theThicknessDouble, theThicknessParameter.data());
}
void SQuad4Element::setThickness(QString value)
{
        setStringParameter(value, &theThicknessDouble, &theThicknessParameter);
}
double SQuad4Element::getThicknessValue()
{
        return getDoubleParameter(&theThicknessDouble, theThicknessParameter.data());
}








QString SQuad4Element::getQuadrature() const
{
        return theQuadrature;
}

void SQuad4Element::setQuadrature(QString value)
{
        theQuadrature = value;
}








double SQuad4Element::getXiResultSlider() const
{
        return theXiResultSlider;
}

void SQuad4Element::setXiResultSlider(double value)
{
        theXiResultSlider = value;

        if (value<-1) {
                rDebug() << "The value of the Result Slider must be between -1 and 1";
                theXiResultSlider = -1;
        }
        else if (value>1) {
                rDebug() << "The value of the Result Slider must be between -1 and 1";
                theXiResultSlider = 1;
        }
         updateResponse();
}

double SQuad4Element::getEtaResultSlider() const
{
        return theEtaResultSlider;
}

void SQuad4Element::setEtaResultSlider(double value)
{
        theEtaResultSlider = value;

        if (value<0) {
                rDebug() << "The value of the Result Slider must be between -1 and 1";
                theEtaResultSlider = -1;
        }
        else if (value>1) {
                rDebug() << "The value of the Result Slider must be between -1 and 1";
                theEtaResultSlider = 1;
        }
         updateResponse();
}



















double SQuad4Element::getAxialStressX() const
{
        return theAxialStressX;
}


double SQuad4Element::getAxialStressY() const
{
        return theAxialStressY;
}


double SQuad4Element::getShearStressResult() const
{
        return theShearStressResult;
}













int SQuad4Element::getNumNodes()
{
        return 4;
}

QObject * SQuad4Element::getNode(int i) const
{
        if (i==0) {
                return theNode1;
        }
        else if (i==1) {
                return theNode2;
        }
        else if (i==2) {
                return theNode3;
        }
        else if (i==3) {
                return theNode4;
        }
        else {
                rCritical() << "The four-node quadrilaterial element was asked for a node that does not exist.";
        }
}




gsl_matrix * SQuad4Element::getGlobalStiffnessMatrix()
{
        // Initialize the static matrix
        if (theGlobalStiffnessMatrix == 0) {
                theGlobalStiffnessMatrix = gsl_matrix_calloc(4*numDOFsPerNode, 4*numDOFsPerNode);
        }
        else {
                gsl_matrix_set_zero(theGlobalStiffnessMatrix);
        }
        
        // Initialize and compute the local stiffness matrix (scope is limited to this method)
        gsl_matrix *Klocal = gsl_matrix_calloc(4*numDOFsPerNode, 4*numDOFsPerNode);
        getLocalStiffnessMatrix(Klocal);
        
        // Do the matrix multiplication Kg = Tlg^T * Kl * Tlg
        gsl_matrix_set_zero(theGlobalStiffnessMatrix);
        gsl_matrix *Ktemp = gsl_matrix_calloc(4*numDOFsPerNode, 4*numDOFsPerNode);
        gsl_blas_dgemm(CblasTrans, CblasNoTrans, 1.0, Tlg, Klocal, 0.0, Ktemp);
        gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, Ktemp, Tlg, 0.0, theGlobalStiffnessMatrix);


        // Free vectors and matrices
        gsl_matrix_free(Ktemp);
        gsl_matrix_free(Klocal);

        return theGlobalStiffnessMatrix;
}







int SQuad4Element::getLocalStiffnessMatrix(gsl_matrix *Klocal)
{
        // Initial declaration of variables
        double jacDet;
        double xi;
        double eta;
        double dvol;
        
        // Initial declaration of vectors
        gsl_matrix *temp = gsl_matrix_calloc(8, 3);

        // Initial declaration of matrices
        gsl_matrix *B = gsl_matrix_calloc(3,8);
        gsl_matrix *Kbasic = gsl_matrix_calloc(8,8);

        // Prepare to loop over integration points
        gsl_vector *point;
        gsl_vector *weight;
        int numPoints;
        if (theQuadrature == QString("One Point Gauss")) {
                numPoints = 1;
                point = gsl_vector_calloc(1);
                gsl_vector_set(point,0,0.0);
                weight = gsl_vector_calloc(1);
                gsl_vector_set(weight,0,2.0);
        }
        else if (theQuadrature == QString("Two by Two Gauss")) {
                numPoints = 2;
                point = gsl_vector_calloc(2);
                gsl_vector_set(point,0,-0.577350269189626);
                gsl_vector_set(point,1, 0.577350269189626);
                weight = gsl_vector_calloc(2);
                gsl_vector_set(weight,0, 1.0);
                gsl_vector_set(weight,1, 1.0);
        }
        else if (theQuadrature == QString("Three by Three Gauss")) {
                numPoints = 3;
                rCritical() << "Sorry, the Three by Three integration rule is not implemented yet.";
        }
        else {
                rCritical() << "Invalid number of integration points set in a four-node quadrilateral element";
                numPoints = 0;
        }
        
        
        // Get material stiffness
        gsl_matrix *D = gsl_matrix_calloc(3,3);
        theMaterial->getPlaneStressStiffness(D);
        
        
        // Loop over quadrature points
        for (int i=0; i<numPoints; i++) {
                for (int j=0; j<numPoints; j++) {
                        
                        // Get shape functions, derivative and Jacobian at this quadrature point
                        xi = gsl_vector_get(point,i);
                        eta = gsl_vector_get(point,j);
                        
                        // Compute the B matrix and the determinant of the Jacobian matrix
                        getBmatrixAndJacobianDeterminant(xi, eta, B, jacDet);

                        // Pre-compute Jacobian times quadrature weight (volume change)
                        dvol = jacDet * (gsl_vector_get(weight,i)*gsl_vector_get(weight,j)) * theThicknessDouble;
                        
                        // Compute 8x8 stiffness matrix
                        gsl_blas_dgemm(CblasTrans, CblasNoTrans, dvol, B, D, 0.0, temp);
                        gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, temp, B, 1.0, Kbasic);
                }
        }

        
        // Expand to 28x28 stiffness matrix
        gsl_matrix *T = gsl_matrix_calloc(8, 28);
        gsl_matrix_set(T, 0, 0, 1.0);
        gsl_matrix_set(T, 1, 2, 1.0);
        gsl_matrix_set(T, 2, 7, 1.0);
        gsl_matrix_set(T, 3, 9, 1.0);
        gsl_matrix_set(T, 4, 14, 1.0);
        gsl_matrix_set(T, 5, 16, 1.0);
        gsl_matrix_set(T, 6, 21, 1.0);
        gsl_matrix_set(T, 7, 23, 1.0);
        gsl_matrix *temp2 = gsl_matrix_calloc(28,8);
        gsl_blas_dgemm(CblasTrans, CblasNoTrans, 1.0, T, Kbasic, 0.0, temp2);
        gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, temp2, T, 0.0, Klocal);
        
        
        // Free memory of vectors and matrices declared in this method
        gsl_vector_free(point);
        gsl_vector_free(weight);
        gsl_matrix_free(B);
        gsl_matrix_free(D);
        gsl_matrix_free(temp);
        gsl_matrix_free(Kbasic);
        gsl_matrix_free(T);
        gsl_matrix_free(temp2);

        return 0;
}











gsl_vector * SQuad4Element::getGlobalLoadVector()
{
        // Initialize the static vector
        if (theGlobalLoadVector == 0) {
                theGlobalLoadVector = gsl_vector_calloc(4*numDOFsPerNode);
        }
        else {
                gsl_vector_set_zero(theGlobalLoadVector);
        }
        
        return theGlobalLoadVector;
}







int SQuad4Element::updateResponse()
{
        // Initialize the stress and strain vectors
        gsl_vector *strain = gsl_vector_calloc(3);
        gsl_vector *stress = gsl_vector_calloc(3);

        // Other initial declarations
        gsl_matrix *B = gsl_matrix_calloc(3,8);
        gsl_vector *point;
        int numPoints;
        double jacDet = 0.0;


        // Get the global displacement vector from the nodes
        gsl_vector *ug = gsl_vector_calloc(4*numDOFsPerNode);

        gsl_vector *ug1 = theNode1->getDisplacementVector();
        for (int i=0; i<numDOFsPerNode; i++) {
                gsl_vector_set(ug, i, gsl_vector_get(ug1, i));
        }

        gsl_vector *ug2 = theNode2->getDisplacementVector();
        for (int i=0; i<numDOFsPerNode; i++) {
                gsl_vector_set(ug, (i+numDOFsPerNode), gsl_vector_get(ug2, i));
        }

        gsl_vector *ug3 = theNode3->getDisplacementVector();
        for (int i=0; i<numDOFsPerNode; i++) {
                gsl_vector_set(ug, (i+2*numDOFsPerNode), gsl_vector_get(ug3, i));
        }

        gsl_vector *ug4 = theNode4->getDisplacementVector();
        for (int i=0; i<numDOFsPerNode; i++) {
                gsl_vector_set(ug, (i+3*numDOFsPerNode), gsl_vector_get(ug4, i));
        }

        // Transform to local system
        gsl_vector *ul = gsl_vector_calloc(4*numDOFsPerNode);
        gsl_blas_dgemv(CblasNoTrans, 1.0, Tlg, ug, 0.0, ul);

        // Transform to basic system (contract to 8-dim vector)
        gsl_vector *ub = gsl_vector_calloc(8);
        gsl_matrix *T = gsl_matrix_calloc(8, 28);
        gsl_matrix_set(T, 0, 0, 1.0);
        gsl_matrix_set(T, 1, 2, 1.0);
        gsl_matrix_set(T, 2, 7, 1.0);
        gsl_matrix_set(T, 3, 9, 1.0);
        gsl_matrix_set(T, 4, 14, 1.0);
        gsl_matrix_set(T, 5, 16, 1.0);
        gsl_matrix_set(T, 6, 21, 1.0);
        gsl_matrix_set(T, 7, 23, 1.0);
        gsl_blas_dgemv(CblasNoTrans, 1.0, T, ul, 0.0, ub);



        if (theQuadrature == QString("One Point")) {
                numPoints = 1;
                point = gsl_vector_calloc(1);
                gsl_vector_set(point,0,0.0);
        }
        else if (theQuadrature == QString("Two by Two Gauss")) {
                numPoints = 2;
                point = gsl_vector_calloc(2);
                gsl_vector_set(point,0,-0.577350269189626);
                gsl_vector_set(point,1, 0.577350269189626);
        }
        else if (theQuadrature == QString("Three by Three")) {
                numPoints = 3;
                rCritical() << "Sorry, the Three by Three integration rule is not implemented yet.";
        }
        else {
                rCritical() << "Invalid number of integration points set in a four-node quadrilateral element";
                numPoints = 0;
        }


        // Get material stiffness
        gsl_matrix *D = gsl_matrix_calloc(3,3);
        theMaterial->getPlaneStressStiffness(D);


        theAxialStressX = 0.0;
        theAxialStressY = 0.0;
        theShearStressResult = 0.0;

        // Compute the B matrix and the determinant of the Jacobian matrix
        getBmatrixAndJacobianDeterminant(theXiResultSlider, theEtaResultSlider, B, jacDet);

        // Compute the strain vector
        gsl_blas_dgemv(CblasNoTrans, 1.0, B, ub, 0.0, strain);

        // Compute the stress vector
        gsl_blas_dgemv(CblasNoTrans, 1.0, D, strain, 0.0, stress);

        // Pick up stress components
        theAxialStressX = gsl_vector_get(stress,0);
        theAxialStressY = gsl_vector_get(stress,1);
        theShearStressResult = gsl_vector_get(stress,2);


        // Free vectors and matrices
        gsl_vector_free(strain);
        gsl_vector_free(stress);
        gsl_matrix_free(B);
        gsl_matrix_free(D);
        gsl_matrix_free(T);
        gsl_vector_free(ub);
        gsl_vector_free(ug);
        gsl_vector_free(ul);
        gsl_vector_free(point);

        return 0;
}


int SQuad4Element::updateStructure()
{
        // Allocate memory for the transformation matrix
        if (Tlg == 0) {
                Tlg = gsl_matrix_calloc(4*numDOFsPerNode, 4*numDOFsPerNode);
        }
        else {
                gsl_matrix_set_zero(Tlg);
        }
        
        // If the nodes are available, compute directional cosines, etc.
        if (theNode1!=0 && theNode2!=0 && theNode3!=0 && theNode4!=0) {
                
                
                // Get nodal coordinates
                double xcoord1 = theNode1->getXCoordinateValue();
                double xcoord2 = theNode2->getXCoordinateValue();
                double xcoord3 = theNode3->getXCoordinateValue();
                double xcoord4 = theNode4->getXCoordinateValue();
                
                double ycoord1 = theNode1->getYCoordinateValue();
                double ycoord2 = theNode2->getYCoordinateValue();
                double ycoord3 = theNode3->getYCoordinateValue();
                double ycoord4 = theNode4->getYCoordinateValue();
                
                double zcoord1 = theNode1->getZCoordinateValue();
                double zcoord2 = theNode2->getZCoordinateValue();
                double zcoord3 = theNode3->getZCoordinateValue();
                double zcoord4 = theNode4->getZCoordinateValue();
                
                // Determine directional cosines
                double dx12 = xcoord2-xcoord1;
                double dy12 = ycoord2-ycoord1;
                double dz12 = zcoord2-zcoord1;
                
                double dx13 = xcoord3-xcoord1;
                double dy13 = ycoord3-ycoord1;
                double dz13 = zcoord3-zcoord1;
                
                double dx14 = xcoord4-xcoord1;
                double dy14 = ycoord4-ycoord1;
                double dz14 = zcoord4-zcoord1;
                
                gsl_vector *vector12 = gsl_vector_calloc(3);
                gsl_vector *vector13 = gsl_vector_calloc(3);
                gsl_vector *vector14 = gsl_vector_calloc(3);
                gsl_vector *normal = gsl_vector_calloc(3);
                
                gsl_vector_set(vector12, 0, dx12);
                gsl_vector_set(vector12, 1, dy12);
                gsl_vector_set(vector12, 2, dz12);
                
                gsl_vector_set(vector13, 0, dx13);
                gsl_vector_set(vector13, 1, dy13);
                gsl_vector_set(vector13, 2, dz13);
                
                gsl_vector_set(vector14, 0, dx14);
                gsl_vector_set(vector14, 1, dy14);
                gsl_vector_set(vector14, 2, dz14);
                
                gsl_vector_set(normal, 0, dy12*dz13 - dz12*dy13);
                gsl_vector_set(normal, 1, dz12*dx13 - dx12*dz13);
                gsl_vector_set(normal, 2, dx12*dy13 - dy12*dx13);

                double eps = 0.0000000001;
                double result;

                gsl_blas_ddot(vector12, normal, &result);
                if (result > -eps && result < eps) {
                        // This is ok
                }
                else {
                        rCritical() << "One of the four-node quadrilateral elements is not plane!";
                }

                gsl_blas_ddot(vector13, normal, &result);
                if (result > -eps && result < eps) {
                        // This is ok
                }
                else {
                        rCritical() << "One of the four-node quadrilateral elements is not plane!";
                }

                gsl_blas_ddot(vector14, normal, &result);
                if (result > -eps && result < eps) {
                        // This is ok
                }
                else {
                        rCritical() << "One of the four-node quadrilateral elements is not plane!";
                }

                // Determine the "v-vector," namely a vector that lies in the local xz-plane, which coincidentally is selected as the element plane
                double v0, v1, v2;
                v0 = -(dy12*gsl_vector_get(normal,2) - dz12*gsl_vector_get(normal,1));
                v1 = -(dz12*gsl_vector_get(normal,0) - dx12*gsl_vector_get(normal,2));
                v2 = -(dx12*gsl_vector_get(normal,1) - dy12*gsl_vector_get(normal,0));
                double length = sqrt(v0*v0 + v1*v1 + v2*v2);
                v0 = v0/length;
                v1 = v1/length;
                v2 = v2/length;


                // Set the x-vector, which forms the first column of the rotation matrix, R
                length = gsl_blas_dnrm2(vector12);
                double x0 = gsl_vector_get(vector12,0)/length;
                double x1 = gsl_vector_get(vector12,1)/length;
                double x2 = gsl_vector_get(vector12,2)/length;


                // Normalize it (is that necessary?)
                double norm = sqrt(x0*x0+x1*x1+x2*x2);
                x0 /= norm;
                x1 /= norm;
                x2 /= norm;


                // Obtain the y-vector, which forms the second column of R, by cross product of x and the user-given vector in the xz-plane
                double y0 = -(v1*x2 - v2*x1);
                double y1 = -(v2*x0 - v0*x2);
                double y2 = -(v0*x1 - v1*x0);


                // Normalize it (is that necessary?)
                norm = sqrt(y0*y0+y1*y1+y2*y2);
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
                int add = 0;
                gsl_matrix_set(Tlg, 0+add, 0+add, x0);  gsl_matrix_set(Tlg, 0+add, 1+add, y0);  gsl_matrix_set(Tlg, 0+add, 2+add, z0);
                gsl_matrix_set(Tlg, 1+add, 0+add, x1);  gsl_matrix_set(Tlg, 1+add, 1+add, y1);  gsl_matrix_set(Tlg, 1+add, 2+add, z1);
                gsl_matrix_set(Tlg, 2+add, 0+add, x2);  gsl_matrix_set(Tlg, 2+add, 1+add, y2);  gsl_matrix_set(Tlg, 2+add, 2+add, z2);
                
                add += 3;
                gsl_matrix_set(Tlg, 0+add, 0+add, x0);  gsl_matrix_set(Tlg, 0+add, 1+add, y0);  gsl_matrix_set(Tlg, 0+add, 2+add, z0);
                gsl_matrix_set(Tlg, 1+add, 0+add, x1);  gsl_matrix_set(Tlg, 1+add, 1+add, y1);  gsl_matrix_set(Tlg, 1+add, 2+add, z1);
                gsl_matrix_set(Tlg, 2+add, 0+add, x2);  gsl_matrix_set(Tlg, 2+add, 1+add, y2);  gsl_matrix_set(Tlg, 2+add, 2+add, z2);

                add += 4;
                gsl_matrix_set(Tlg, 0+add, 0+add, x0);  gsl_matrix_set(Tlg, 0+add, 1+add, y0);  gsl_matrix_set(Tlg, 0+add, 2+add, z0);
                gsl_matrix_set(Tlg, 1+add, 0+add, x1);  gsl_matrix_set(Tlg, 1+add, 1+add, y1);  gsl_matrix_set(Tlg, 1+add, 2+add, z1);
                gsl_matrix_set(Tlg, 2+add, 0+add, x2);  gsl_matrix_set(Tlg, 2+add, 1+add, y2);  gsl_matrix_set(Tlg, 2+add, 2+add, z2);

                add += 3;
                gsl_matrix_set(Tlg, 0+add, 0+add, x0);  gsl_matrix_set(Tlg, 0+add, 1+add, y0);  gsl_matrix_set(Tlg, 0+add, 2+add, z0);
                gsl_matrix_set(Tlg, 1+add, 0+add, x1);  gsl_matrix_set(Tlg, 1+add, 1+add, y1);  gsl_matrix_set(Tlg, 1+add, 2+add, z1);
                gsl_matrix_set(Tlg, 2+add, 0+add, x2);  gsl_matrix_set(Tlg, 2+add, 1+add, y2);  gsl_matrix_set(Tlg, 2+add, 2+add, z2);

                add += 4;
                gsl_matrix_set(Tlg, 0+add, 0+add, x0);  gsl_matrix_set(Tlg, 0+add, 1+add, y0);  gsl_matrix_set(Tlg, 0+add, 2+add, z0);
                gsl_matrix_set(Tlg, 1+add, 0+add, x1);  gsl_matrix_set(Tlg, 1+add, 1+add, y1);  gsl_matrix_set(Tlg, 1+add, 2+add, z1);
                gsl_matrix_set(Tlg, 2+add, 0+add, x2);  gsl_matrix_set(Tlg, 2+add, 1+add, y2);  gsl_matrix_set(Tlg, 2+add, 2+add, z2);

                add += 3;
                gsl_matrix_set(Tlg, 0+add, 0+add, x0);  gsl_matrix_set(Tlg, 0+add, 1+add, y0);  gsl_matrix_set(Tlg, 0+add, 2+add, z0);
                gsl_matrix_set(Tlg, 1+add, 0+add, x1);  gsl_matrix_set(Tlg, 1+add, 1+add, y1);  gsl_matrix_set(Tlg, 1+add, 2+add, z1);
                gsl_matrix_set(Tlg, 2+add, 0+add, x2);  gsl_matrix_set(Tlg, 2+add, 1+add, y2);  gsl_matrix_set(Tlg, 2+add, 2+add, z2);

                add += 4;
                gsl_matrix_set(Tlg, 0+add, 0+add, x0);  gsl_matrix_set(Tlg, 0+add, 1+add, y0);  gsl_matrix_set(Tlg, 0+add, 2+add, z0);
                gsl_matrix_set(Tlg, 1+add, 0+add, x1);  gsl_matrix_set(Tlg, 1+add, 1+add, y1);  gsl_matrix_set(Tlg, 1+add, 2+add, z1);
                gsl_matrix_set(Tlg, 2+add, 0+add, x2);  gsl_matrix_set(Tlg, 2+add, 1+add, y2);  gsl_matrix_set(Tlg, 2+add, 2+add, z2);

                add += 3;
                gsl_matrix_set(Tlg, 0+add, 0+add, x0);  gsl_matrix_set(Tlg, 0+add, 1+add, y0);  gsl_matrix_set(Tlg, 0+add, 2+add, z0);
                gsl_matrix_set(Tlg, 1+add, 0+add, x1);  gsl_matrix_set(Tlg, 1+add, 1+add, y1);  gsl_matrix_set(Tlg, 1+add, 2+add, z1);
                gsl_matrix_set(Tlg, 2+add, 0+add, x2);  gsl_matrix_set(Tlg, 2+add, 1+add, y2);  gsl_matrix_set(Tlg, 2+add, 2+add, z2);

                gsl_vector_free(vector12);
                gsl_vector_free(vector13);
                gsl_vector_free(vector14);
                gsl_vector_free(normal);

                
        }
        
        return 0;
}






int SQuad4Element::getBmatrixAndJacobianDeterminant(double xi, double eta, gsl_matrix *B, double &jacDet)
{
        // Initial declaration of vectors
        gsl_vector *N = gsl_vector_calloc(4);
        gsl_vector *dNdxi = gsl_vector_calloc(4);
        gsl_vector *dNdeta = gsl_vector_calloc(4);
        gsl_vector *dNdx = gsl_vector_calloc(4);
        gsl_vector *dNdy = gsl_vector_calloc(4);

        // Initial declaration of matrices
        gsl_matrix *jacobianMatrix = gsl_matrix_calloc(2,2);
        gsl_matrix *iJac = gsl_matrix_calloc(2,2);

        // Get nodal coordinates to work with
        double xcoord1 = theNode1->getXCoordinateValue();
        double xcoord2 = theNode2->getXCoordinateValue();
        double xcoord3 = theNode3->getXCoordinateValue();
        double xcoord4 = theNode4->getXCoordinateValue();

        double ycoord1 = theNode1->getYCoordinateValue();
        double ycoord2 = theNode2->getYCoordinateValue();
        double ycoord3 = theNode3->getYCoordinateValue();
        double ycoord4 = theNode4->getYCoordinateValue();

        double zcoord1 = theNode1->getZCoordinateValue();
        double zcoord2 = theNode2->getZCoordinateValue();
        double zcoord3 = theNode3->getZCoordinateValue();
        double zcoord4 = theNode4->getZCoordinateValue();

        // Determine directional cosines of the element edges
        double dx12 = xcoord2-xcoord1;
        double dy12 = ycoord2-ycoord1;
        double dz12 = zcoord2-zcoord1;

        double dx13 = xcoord3-xcoord1;
        double dy13 = ycoord3-ycoord1;
        double dz13 = zcoord3-zcoord1;

        double dx14 = xcoord4-xcoord1;
        double dy14 = ycoord4-ycoord1;
        double dz14 = zcoord4-zcoord1;

        // Determine vector perpendicular to element plane
        double normal1 = dy12*dz13 - dz12*dy13;
        double normal2 = dz12*dx13 - dx12*dz13;
        double normal3 = dx12*dy13 - dy12*dx13;

        // Get nodal coordinates *in the local coordinate system* ...
        gsl_vector *x = gsl_vector_calloc(4);
        gsl_vector *y = gsl_vector_calloc(4);

        // ... the first node is by definite set to the origin
        gsl_vector_set(x, 0, 0.0);
        gsl_vector_set(y, 0, 0.0);

        // ... the second node is by definition set along the x-axis
        double distance12 = sqrt(dx12*dx12 + dy12*dy12 + dz12*dz12);
        gsl_vector_set(x, 1, distance12);
        gsl_vector_set(y, 1, 0.0);

        // ... next the y-vector is obtained as the cross-product of the x-axis and the normal vector
        double y_vector1 = -(dy12*normal3 - dz12*normal2);
        double y_vector2 = -(dz12*normal1 - dx12*normal3);
        double y_vector3 = -(dx12*normal2 - dy12*normal1);
        double length_y = sqrt(y_vector1*y_vector1 + y_vector2*y_vector2 + y_vector3*y_vector3);

        // ... now, normalize the x and y vectors
        gsl_vector *unit_x = gsl_vector_calloc(3);
        gsl_vector_set(unit_x, 0, dx12/distance12);
        gsl_vector_set(unit_x, 1, dy12/distance12);
        gsl_vector_set(unit_x, 2, dz12/distance12);
        gsl_vector *unit_y = gsl_vector_calloc(3);
        gsl_vector_set(unit_y, 0, y_vector1/length_y);
        gsl_vector_set(unit_y, 1, y_vector2/length_y);
        gsl_vector_set(unit_y, 2, y_vector3/length_y);

        // ... the coordinates of the third node is found by dot product:
        gsl_vector *vector13 = gsl_vector_calloc(3);
        gsl_vector_set(vector13, 0, dx13);
        gsl_vector_set(vector13, 1, dy13);
        gsl_vector_set(vector13, 2, dz13);
        double result;
        gsl_blas_ddot(vector13,unit_x, &result);
        gsl_vector_set(x, 2, result);
        gsl_blas_ddot(vector13,unit_y, &result);
        gsl_vector_set(y, 2, result);

        // ... and the same for the fourth and last node:
        gsl_vector *vector14 = gsl_vector_calloc(3);
        gsl_vector_set(vector14, 0, dx14);
        gsl_vector_set(vector14, 1, dy14);
        gsl_vector_set(vector14, 2, dz14);
        gsl_blas_ddot(vector14,unit_x, &result);
        gsl_vector_set(x, 3, result);
        gsl_blas_ddot(vector14,unit_y, &result);
        gsl_vector_set(y, 3, result);

        gsl_vector_free(unit_x);
        gsl_vector_free(unit_y);
        gsl_vector_free(vector13);
        gsl_vector_free(vector14);
        // End of getting nodal coordinates to work with *************************************************************



        // Get shape functions, derivative and Jacobian at this quadrature point
        double factor = 0.25;

        gsl_vector_set(N,0, (factor * (1.0-xi) * (1.0-eta)));
        gsl_vector_set(N,1, (factor * (1.0+xi) * (1.0-eta)));
        gsl_vector_set(N,2, (factor * (1.0+xi) * (1.0+eta)));
        gsl_vector_set(N,3, (factor * (1.0-xi) * (1.0+eta)));

        gsl_vector_set(dNdxi, 0, (factor * (-1.0) * (1.0-eta)));
        gsl_vector_set(dNdxi, 1, (factor * (1.0) * (1.0-eta)));
        gsl_vector_set(dNdxi, 2, (factor * (1.0) * (1.0+eta)));
        gsl_vector_set(dNdxi, 3, (factor * (-1.0) * (1.0+eta)));

        gsl_vector_set(dNdeta, 0, (factor * (1.0-xi) * (-1.0)));
        gsl_vector_set(dNdeta, 1, (factor * (1.0+xi) * (-1.0)));
        gsl_vector_set(dNdeta, 2, (factor * (1.0+xi) * (1.0)));
        gsl_vector_set(dNdeta, 3, (factor * (1.0-xi) * (1.0)));

        gsl_matrix_set_zero(jacobianMatrix);

        for (int m=0; m<4; m++) {
                gsl_matrix_set(jacobianMatrix, 0,0, (gsl_matrix_get(jacobianMatrix,0,0) + gsl_vector_get(dNdxi,m)*gsl_vector_get(x,m)));
                gsl_matrix_set(jacobianMatrix, 0,1, (gsl_matrix_get(jacobianMatrix,0,1) + gsl_vector_get(dNdxi,m)*gsl_vector_get(y,m)));
                gsl_matrix_set(jacobianMatrix, 1,0, (gsl_matrix_get(jacobianMatrix,1,0) + gsl_vector_get(dNdeta,m)*gsl_vector_get(x,m)));
                gsl_matrix_set(jacobianMatrix, 1,1, (gsl_matrix_get(jacobianMatrix,1,1) + gsl_vector_get(dNdeta,m)*gsl_vector_get(y,m)));
        }

        // Set the Jacobian determinant
        jacDet = (gsl_matrix_get(jacobianMatrix,0,0) * gsl_matrix_get(jacobianMatrix,1,1)) - (gsl_matrix_get(jacobianMatrix,0,1) * gsl_matrix_get(jacobianMatrix,1,0));

        gsl_matrix_set(iJac, 0,0, (1.0/jacDet*gsl_matrix_get(jacobianMatrix,1,1)));
        gsl_matrix_set(iJac, 1,1, (1.0/jacDet*gsl_matrix_get(jacobianMatrix,0,0)));
        gsl_matrix_set(iJac, 0,1, (-1.0/jacDet*gsl_matrix_get(jacobianMatrix,0,1)));
        gsl_matrix_set(iJac, 1,0, (-1.0/jacDet*gsl_matrix_get(jacobianMatrix,1,0)));

        for (int m=0; m<4; m++) {
                gsl_vector_set(dNdx, m, (gsl_vector_get(dNdxi,m) * gsl_matrix_get(iJac,0,0) + gsl_vector_get(dNdeta,m) * gsl_matrix_get(iJac,0,1)));
                gsl_vector_set(dNdy, m, (gsl_vector_get(dNdxi,m) * gsl_matrix_get(iJac,1,0) + gsl_vector_get(dNdeta,m) * gsl_matrix_get(iJac,1,1)));
        }

        // Set the strain-displacement matrix B
        for (int m=0; m<4; m++) {
                gsl_matrix_set(B,0,(2*m), gsl_vector_get(dNdx, m));
                gsl_matrix_set(B,1,(2*m), 0.0);
                gsl_matrix_set(B,2,(2*m), gsl_vector_get(dNdy, m));

                gsl_matrix_set(B,0,(2*m+1), 0.0);
                gsl_matrix_set(B,1,(2*m+1), gsl_vector_get(dNdy, m));
                gsl_matrix_set(B,2,(2*m+1), gsl_vector_get(dNdx, m));
        }


        // Free memory of vectors and matrices declared in this method
        gsl_vector_free(N);
        gsl_vector_free(dNdxi);
        gsl_vector_free(dNdeta);
        gsl_vector_free(dNdx);
        gsl_vector_free(dNdy);
        gsl_vector_free(x);
        gsl_vector_free(y);
        gsl_matrix_free(jacobianMatrix);
        gsl_matrix_free(iJac);

        return 0;
}


bool SQuad4Element::canBeRunParallelly() {
	return false;
}