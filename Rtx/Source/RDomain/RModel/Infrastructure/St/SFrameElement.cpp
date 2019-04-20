#include "SFrameElement.h"
#include "SSection.h"
#include "SNode.h"
#include "RDomain.h"
#include <math.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_blas.h>

#include "RMainWindow.h"
#include "RPlotWidget.h"
#include <QInputDialog>
//#include <qwt_plot.h>
//#include <qwt_plot_curve.h>
//#include <qwt_legend.h>



// Definition of the static stiffness matrix (one instance for all elements)
//gsl_matrix * SFrameElement::theGlobalStiffnessMatrix;
//gsl_vector * SFrameElement::theGlobalLoadVector;
//gsl_vector * SFrameElement::theSectionResponseInGlobalXYZ;


SFrameElement::SFrameElement(QObject *parent, QString &name)
                : SElement(parent, name)
{
        numDOFsPerNode = 7;
        theSection = 0;
        theNode1 = 0;
        theNode2 = 0;
        theShearDeformation = "No";
        theGeometricStiffness = "No";
        theVectorInLocalXZPlane = "GlobalZDirection";

        Tlg = 0;
        ul = 0;
        L = 0.0;
        theGlobalStiffnessMatrix = 0;
        theGlobalLoadVector = 0;
        theSectionResponseInGlobalXYZ = 0;

        theAxialForce = 0.0;
        theMy = 0.0;
        theVz = 0.0;
        theMz = 0.0;
        theVy = 0.0;
        theStVenantTorque = 0.0;
        theWarpingTorque = 0.0;
        theMaxBendingMoment = 0.0;
        theMaxShearForce = 0.0;
        theMaxAxialStress = 0.0;
        theMaxShearStress = 0.0;

        theResultSlider = 0.5;

        //$$ theUniformLoadInZDirectionParameter = 0;
        //$$ theUniformLoadInYDirectionParameter = 0;
        theUniformLoadInZDirectionDouble = 0.0;
        theUniformLoadInYDirectionDouble = 0.0;

}

SFrameElement::~SFrameElement()
{
        if (theGlobalStiffnessMatrix != 0) {
                gsl_matrix_free(theGlobalStiffnessMatrix);
                theGlobalStiffnessMatrix = 0;
        }
        if (theGlobalLoadVector != 0) {
                gsl_vector_free(theGlobalLoadVector);
                theGlobalLoadVector = 0;
        }
        if (theSectionResponseInGlobalXYZ != 0) {
                gsl_vector_free(theSectionResponseInGlobalXYZ);
                theSectionResponseInGlobalXYZ = 0;
        }
        if (Tlg != 0) {
                gsl_matrix_free(Tlg);
                Tlg = 0;
        }
        if (ul != 0) {
                gsl_vector_free(ul);
                ul = 0;
        }
}


QObject * SFrameElement::getSection() const
{
        return theSection;
}
void SFrameElement::setSection(QObject *value)
{
        theSection = qobject_cast<SSection *>(value);
}




QObject * SFrameElement::getNode1() const
{
        return theNode1;
}
void SFrameElement::setNode1(QObject *value)
{
        theNode1 = qobject_cast<SNode *>(value);
}




QObject * SFrameElement::getNode2() const
{
        return theNode2;
}
void SFrameElement::setNode2(QObject *value)
{
        theNode2 = qobject_cast<SNode *>(value);
}





double SFrameElement::getResultSlider() const
{
        return theResultSlider;
}


void SFrameElement::setResultSlider(double value)
{
        theResultSlider = value;

        if (value<0) {
                rDebug() << "The value of the Result Slider must be between 0 and 1.";
                theResultSlider = 0;
        }
        else if (value>1) {
                rDebug() << "The value of the Result Slider must be between 0 and 1.";
                theResultSlider = 1;
        }

        if (Tlg != 0) {
                updateResponse();
        }


}







QString SFrameElement::getUniformLoadInZDirection()
{
        return getStringParameter(&theUniformLoadInZDirectionDouble, theUniformLoadInZDirectionParameter.data());
}
void SFrameElement::setUniformLoadInZDirection(QString value)
{
        setStringParameter(value, &theUniformLoadInZDirectionDouble, &theUniformLoadInZDirectionParameter);
}
double SFrameElement::getUniformLoadInZDirectionValue()
{
        return getDoubleParameter(&theUniformLoadInZDirectionDouble, theUniformLoadInZDirectionParameter.data());
}


QString SFrameElement::getUniformLoadInYDirection()
{
        return getStringParameter(&theUniformLoadInYDirectionDouble, theUniformLoadInYDirectionParameter.data());
}
void SFrameElement::setUniformLoadInYDirection(QString value)
{
        setStringParameter(value, &theUniformLoadInYDirectionDouble, &theUniformLoadInYDirectionParameter);
}
double SFrameElement::getUniformLoadInYDirectionValue()
{
        return getDoubleParameter(&theUniformLoadInYDirectionDouble, theUniformLoadInYDirectionParameter.data());
}








double SFrameElement::getAxialForce() const
{
        return theAxialForce;
}

double SFrameElement::getMy() const
{
        return theMy;
}

double SFrameElement::getVz() const
{
        return theVz;
}

double SFrameElement::getMz() const
{
        return theMz;
}

double SFrameElement::getVy() const
{
        return theVy;
}

double SFrameElement::getStVenantTorque() const
{
        return theStVenantTorque;
}

double SFrameElement::getWarpingTorque() const
{
        return theWarpingTorque;
}

double SFrameElement::getMaxBendingMoment() const
{
        return theMaxBendingMoment;
}

double SFrameElement::getMaxShearForce() const
{
        return theMaxShearForce;
}

double SFrameElement::getMaxAxialStress() const
{
        return theMaxAxialStress;
}

double SFrameElement::getMaxShearStress() const
{
        return theMaxShearStress;
}









int SFrameElement::getNumNodes()
{
        return 2;
}

QObject * SFrameElement::getNode(int i) const
{
        if (i==0) {
                return theNode1;
        }
        else if (i==1) {
                return theNode2;
        }
        else {
                rCritical() << "The frame element was asked for a non-existing node.";
        }
}


QString SFrameElement::getVectorInLocalXZPlane() const
{
        return theVectorInLocalXZPlane;
}

void SFrameElement::setVectorInLocalXZPlane(QString value)
{
        theVectorInLocalXZPlane = value;

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

                // Cannot let the v-vector go in the same direction as this auxiliary vector
                if ( (theVectorInLocalXZPlane == QString("GlobalXDirection")) && (dy==0.0) && (dz==0.0) ) {
                        rCritical() << "A frame element is oriented in the same direction as the user-defined Vector-In-Local-XZ-Plane";
                }
                if ( (theVectorInLocalXZPlane == QString("GlobalYDirection")) && (dx==0.0) && (dz==0.0) ) {
                        rCritical() << "A frame element is oriented in the same direction as the user-defined Vector-In-Local-XZ-Plane";
                }
                if ( (theVectorInLocalXZPlane == QString("GlobalZDirection")) && (dy==0.0) && (dx==0.0) ) {
                        rCritical() << "A frame element is oriented in the same direction as the user-defined Vector-In-Local-XZ-Plane";
                }
        }

}






QString SFrameElement::getShearDeformation() const
{
        return theShearDeformation;
}

void SFrameElement::setShearDeformation(QString value)
{
        theShearDeformation = value;
}






QString SFrameElement::getGeometricStiffness() const
{
        return theGeometricStiffness;
}

void SFrameElement::setGeometricStiffness(QString value)
{
        theGeometricStiffness = value;
}





void SFrameElement::plotMy()
{
        plot("My");
}


void SFrameElement::plotVz()
{
        plot("Vz");
}


void SFrameElement::plotMz()
{
        plot("Mz");
}


void SFrameElement::plotVy()
{
        plot("Vy");
}


void SFrameElement::plotdwdx()
{
        plot("dwdx");
}


void SFrameElement::plotdvdx()
{
        plot("dvdx");
}


void SFrameElement::plotw()
{
        plot("w");
}


void SFrameElement::plotv()
{
        plot("v");
}


void SFrameElement::plotN()
{
        plot("N");
}








QStringList SFrameElement::getActionList()
{
        QStringList actionList;
        actionList << "Plot w" << "Plot My" << "Plot Vz" << "Plot dwdx" <<  "Plot v" << "Plot Mz" << "Plot Vy" << "Plot dvdx" << "Plot N";
        return actionList;
}























gsl_matrix * SFrameElement::getGlobalStiffnessMatrix()
{
        // Set the Tlg matrix
        updateStructure();

        // Initialize the static matrix
        if (theGlobalStiffnessMatrix == 0) {
                theGlobalStiffnessMatrix = gsl_matrix_calloc(2*numDOFsPerNode, 2*numDOFsPerNode);
        }
        else {
                gsl_matrix_set_zero(theGlobalStiffnessMatrix);
        }

        // Initialize and compute the local stiffness matrix (scope is limited to this method)
        gsl_matrix *Klocal = gsl_matrix_calloc(2*numDOFsPerNode, 2*numDOFsPerNode);
        getLocalStiffnessMatrix(Klocal);
        
        // Do the matrix multiplication Kg = Tlg^T * Kl * Tlg
        gsl_matrix_set_zero(theGlobalStiffnessMatrix);
        gsl_matrix *Ktemp = gsl_matrix_calloc(2*numDOFsPerNode, 2*numDOFsPerNode);
        gsl_blas_dgemm(CblasTrans, CblasNoTrans, 1.0, Tlg, Klocal, 0.0, Ktemp);
        gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, Ktemp, Tlg, 0.0, theGlobalStiffnessMatrix);

        gsl_matrix_free(Ktemp);
        gsl_matrix_free(Klocal);

        return theGlobalStiffnessMatrix;
}

int SFrameElement::getLocalStiffnessMatrix(gsl_matrix *Klocal)
{
        // Get section stiffness
        double EA = theSection->getEA();
        double EIzz = theSection->getEIz();
        double EIyy = theSection->getEIy();
        double GJ = theSection->getGJ();
        double ECw = theSection->getECw();
        double GAvz = theSection->getGAvz();
        double GAvy = theSection->getGAvy();

        double alpha_z = 0.0;
        double alpha_y = 0.0;
        if (theShearDeformation == QString("Yes") && (GAvz!=0.0)) {
                alpha_z = 12.0*EIyy/(GAvz*L*L);
        }
        if (theShearDeformation == QString("Yes") && (GAvy!=0.0)) {
                alpha_y = 12.0*EIzz/(GAvy*L*L);
        }

        double P = 0.0;
        if (theGeometricStiffness == QString("Yes")) {
                P = theAxialForce;
        }


        // Start with the stiffness matrix in the "local" configuration
        double L2 = L*L;
        double L3 = L*L*L;
        double EAL = EA/L;
        gsl_matrix_set(Klocal, 0, 0,  EAL);  gsl_matrix_set(Klocal, 0, 7, -EAL);
        gsl_matrix_set(Klocal, 7, 0,  -EAL); gsl_matrix_set(Klocal, 7, 7, EAL);


        // Bending in the x-y-plane
        double TwelveEIzzL3 = 12.0*EIzz/(L3*(1.0+alpha_y));
        double SixEIzzL2 = 6.0*EIzz/(L2*(1.0+alpha_y));
        double FourEIzzL = (4.0+alpha_y)*EIzz/(L*(1.0+alpha_y));
        double TwoEIzzL = (2.0-alpha_y)*EIzz/(L*(1.0+alpha_y));
        gsl_matrix_set(Klocal, 1, 1,  TwelveEIzzL3-6.0*P/(5.0*L));  gsl_matrix_set(Klocal, 1, 5, SixEIzzL2-P/10.0);        gsl_matrix_set(Klocal, 1, 8,  -TwelveEIzzL3+6.0*P/(5.0*L)); gsl_matrix_set(Klocal, 1, 12, SixEIzzL2-P/10.0);
        gsl_matrix_set(Klocal, 5, 1,  SixEIzzL2-P/10.0);            gsl_matrix_set(Klocal, 5, 5, FourEIzzL-2.0*P*L/15.0);  gsl_matrix_set(Klocal, 5, 8,  -SixEIzzL2+P/10.0);           gsl_matrix_set(Klocal, 5, 12, TwoEIzzL+P*L/30.0);
        gsl_matrix_set(Klocal, 8, 1,  -TwelveEIzzL3+6.0*P/(5.0*L)); gsl_matrix_set(Klocal, 8, 5, -SixEIzzL2+P/10.0);       gsl_matrix_set(Klocal, 8, 8,  TwelveEIzzL3-6.0*P/(5.0*L));  gsl_matrix_set(Klocal, 8, 12, -SixEIzzL2+P/10.0);
        gsl_matrix_set(Klocal, 12, 1,  SixEIzzL2-P/10.0);           gsl_matrix_set(Klocal, 12, 5, TwoEIzzL+P*L/30.0);      gsl_matrix_set(Klocal, 12, 8,  -SixEIzzL2+P/10.0);          gsl_matrix_set(Klocal, 12, 12, FourEIzzL-2.0*P*L/15.0);


        // Bending in the x-z-plane
        double TwelveEIyyL3 = 12.0*EIyy/(L3*(1.0+alpha_z));
        double SixEIyyL2 = 6.0*EIyy/(L2*(1.0+alpha_z));
        double FourEIyyL = (4.0+alpha_z)*EIyy/(L*(1.0+alpha_z));
        double TwoEIyyL = (2.0-alpha_z)*EIyy/(L*(1.0+alpha_z));
        gsl_matrix_set(Klocal, 2, 2,  TwelveEIyyL3-6.0*P/(5.0*L));  gsl_matrix_set(Klocal, 2, 4, -SixEIyyL2+P/10.0);       gsl_matrix_set(Klocal, 2, 9,  -TwelveEIyyL3+6.0*P/(5.0*L)); gsl_matrix_set(Klocal, 2, 11, -SixEIyyL2+P/10.0);
        gsl_matrix_set(Klocal, 4, 2,  -SixEIyyL2+P/10.0);           gsl_matrix_set(Klocal, 4, 4, FourEIyyL-2.0*P*L/15.0);  gsl_matrix_set(Klocal, 4, 9,  SixEIyyL2-P/10.0);            gsl_matrix_set(Klocal, 4, 11, TwoEIyyL+P*L/30.0);
        gsl_matrix_set(Klocal, 9, 2,  -TwelveEIyyL3+6.0*P/(5.0*L)); gsl_matrix_set(Klocal, 9, 4, SixEIyyL2-P/10.0);        gsl_matrix_set(Klocal, 9, 9,  TwelveEIyyL3-6.0*P/(5.0*L));  gsl_matrix_set(Klocal, 9, 11, SixEIyyL2-P/10.0);
        gsl_matrix_set(Klocal, 11, 2,  -SixEIyyL2+P/10.0);          gsl_matrix_set(Klocal, 11, 4, TwoEIyyL+P*L/30.0);      gsl_matrix_set(Klocal, 11, 9,  SixEIyyL2-P/10.0);           gsl_matrix_set(Klocal, 11, 11, FourEIyyL-2.0*P*L/15.0);


        // Torsion
        double TwelveECwL3 = 12.0*ECw/L3;
        double SixECwL2 = 6.0*ECw/L2;
        double FourECwL = 4.0*ECw/L;
        double TwoECwL = 2.0*ECw/L;
        double SixGJ5L = 6.0*GJ/(5.0*L);
        double GJ10 = GJ/10.0;
        double TwoLGJ15 = 2.0*L*GJ/15.0;
        double LGJ30 = L*GJ/30.0;
        gsl_matrix_set(Klocal, 3, 3,  TwelveECwL3+SixGJ5L);   gsl_matrix_set(Klocal, 3, 6, SixECwL2+GJ10);     gsl_matrix_set(Klocal, 3, 10,  -TwelveECwL3-SixGJ5L); gsl_matrix_set(Klocal, 3, 13, SixECwL2+GJ10);
        gsl_matrix_set(Klocal, 6, 3,  SixECwL2+GJ10);         gsl_matrix_set(Klocal, 6, 6, FourECwL+TwoLGJ15); gsl_matrix_set(Klocal, 6, 10,  -SixECwL2-GJ10);       gsl_matrix_set(Klocal, 6, 13, TwoECwL-LGJ30);
        gsl_matrix_set(Klocal, 10, 3,  -TwelveECwL3-SixGJ5L); gsl_matrix_set(Klocal, 10, 6, -SixECwL2-GJ10);   gsl_matrix_set(Klocal, 10, 10,  TwelveECwL3+SixGJ5L); gsl_matrix_set(Klocal, 10, 13, -SixECwL2-GJ10);
        gsl_matrix_set(Klocal, 13, 3,  SixECwL2+GJ10);        gsl_matrix_set(Klocal, 13, 6, TwoECwL-LGJ30);    gsl_matrix_set(Klocal, 13, 10,  -SixECwL2-GJ10);      gsl_matrix_set(Klocal, 13, 13, FourECwL+TwoLGJ15);

        return 0;
}


gsl_vector * SFrameElement::getGlobalLoadVector()
{
        // Assume that Tlg is alread set when the stiffness matrix was established


        // Initialize the static vector
        if (theGlobalLoadVector == 0) {
                theGlobalLoadVector = gsl_vector_calloc(2*numDOFsPerNode);
        }
        else {
                gsl_vector_set_zero(theGlobalLoadVector);
        }

        // Get the per-unit-length weight of the cross-section
        double sectionWeight = theSection->getSectionWeight();

        // Compute total weight of the element
        double elementWeight = L*sectionWeight;



        // Now deal with the element loads
        double qz = getUniformLoadInZDirectionValue();
        double qy = getUniformLoadInYDirectionValue();

        gsl_vector *Fl = gsl_vector_calloc(2*numDOFsPerNode);

        gsl_vector_set(Fl, (2-1), 0.5*qy*L); // Fy1
        gsl_vector_set(Fl, (3-1), 0.5*qz*L); // Fz1

        gsl_vector_set(Fl, (5-1), -qz*L*L/12.0); // My1
        gsl_vector_set(Fl, (6-1), qy*L*L/12.0); // Mz1

        gsl_vector_set(Fl, (9-1), 0.5*qy*L); // Fy2
        gsl_vector_set(Fl, (10-1), 0.5*qz*L); // Fz2

        gsl_vector_set(Fl, (12-1), qz*L*L/12.0); // My2
        gsl_vector_set(Fl, (13-1), -qy*L*L/12.0); // Mz2


        // Do the matrix-vector multiplication Fg = Tlg^T * Fl
        gsl_blas_dgemv(CblasTrans, 1.0, Tlg, Fl, 0.0, theGlobalLoadVector);


        gsl_vector_free(Fl);



        // Add self weight
        //gsl_vector_set(theGlobalLoadVector, (3-1), (gsl_vector_set(theGlobalLoadVector, (3-1)))-0.5*(elementWeight)*L); // Fz1
        //gsl_vector_set(theGlobalLoadVector, (5-1), gsl_vector_set(theGlobalLoadVector, (5-1))()+(elementWeight)*L*L/12.0); // My1
        //gsl_vector_set(theGlobalLoadVector, (10-1), (gsl_vector_set(theGlobalLoadVector, (10-1)))-0.5*(elementWeight)*L); // Fz2
        //gsl_vector_set(theGlobalLoadVector, (12-1), (gsl_vector_set(theGlobalLoadVector, (12-1)))-(elementWeight)*L*L/12.0); // My2


        return theGlobalLoadVector;
}






int SFrameElement::updateResponse()
{
        // Allocate memory for the local displacement repsonse vector
        if (ul == 0) {
                ul = gsl_vector_calloc(2*numDOFsPerNode);
        }
        else {
                gsl_vector_set_zero(ul);
        }


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
        gsl_blas_dgemv(CblasNoTrans, 1.0, Tlg, ug, 0.0, ul);

        // Set up the local stiffness matrix
        gsl_matrix *Klocal = gsl_matrix_calloc(2*numDOFsPerNode,2*numDOFsPerNode);
        getLocalStiffnessMatrix(Klocal);

        // Multiply Fl=Kl*ul to obtain local forces
        gsl_vector *Fl = gsl_vector_calloc(2*numDOFsPerNode);
        gsl_blas_dgemv(CblasNoTrans, 1.0, Klocal, ul, 0.0, Fl);

        // Extract the axial force (tension positive)
        theAxialForce = -gsl_vector_get(Fl,0);

        // Compute other section responses at Xi
        theMy = getSectionResponse("My",theResultSlider);
        theVz = getSectionResponse("Vz",theResultSlider);
        theMz = getSectionResponse("Mz",theResultSlider);
        theVy = getSectionResponse("Vy",theResultSlider);
        theMaxBendingMoment = qMax( (getSectionResponse("MyMax",0.0)) , (getSectionResponse("MzMax",0.0)) );
        theMaxShearForce = qMax( (getSectionResponse("VzMax",0.0)) , (getSectionResponse("VyMax",0.0)) );

        //theStVenantTorque = ;
        //theWarpingTorque = ;
        //theMaxAxialStress = ;
        //theMaxShearStress = ;

        // Deallocate vectors and matrices
        gsl_matrix_free(Klocal);
        gsl_vector_free(ug);
        gsl_vector_free(Fl);

        return 0;
}






double SFrameElement::getSectionResponse(QString type, double xi)
{
        if (ul == 0) {
                return 0.0;
        }
        else {

                // Quick return if plotting the axial force
                if (type == QString("N")) {
                        return theAxialForce;
                }

                // Initial declarations for this method
                double u1, u2, u3, u4, C1, C2, C3, C4, w, v, dwdx, dvdx;

                // Set x-value from xi-value
                double x = xi*L;

                // No element loads yet
                double qz = getUniformLoadInZDirectionValue();
                double qy = getUniformLoadInYDirectionValue();

                // Get section stiffnesses
                double EIz = theSection->getEIz();
                double EIy = theSection->getEIy();

                if ( (EIy != 0.0) && ( (type == QString("w")) || (type == QString("dwdx")) || (type == QString("My")) || (type == QString("MyMax")) || (type == QString("Vz"))   || (type == QString("VzMax"))         ) ) {
                        // FOR THE BEAM ELEMENT IN THE X-Z PLANE (SEE DOF NUMBERING IN THE FRAME ELEMENT DOCUMENT)

                        // Extract DOF displacements
                        u1 = gsl_vector_get(ul,(3-1));
                        u2 = gsl_vector_get(ul,(5-1));
                        u3 = gsl_vector_get(ul,(10-1));
                        u4 = gsl_vector_get(ul,(12-1));

                        // Coefficients in the solution to the differential equation
                        C1 = -qz*L/(12.0*EIy) - 2.0*u3/(L*L*L) - u2/(L*L) + 2.0*u1/(L*L*L) - u4/(L*L);
                        C2 = qz*L*L/(24.0*EIy) + 3.0*u3/(L*L) + 2.0*u2/L - 3.0*u1/(L*L) + u4/L;
                        C3 = -u2;
                        C4 = u1;

                        if (type == QString("w")) {
                                return (qz*(x*x*x*x)/(24.0*EIy) + C1*x*x*x + C2*x*x + C3*x + C4);
                        }
                        else if (type == QString("dwdx")) {
                                return (qz*(x*x*x)/(6.0*EIy) + 3.0*C1*x*x + 2.0*C2*x + C3);
                        }
                        else if (type == QString("My")) {
                                return (qz*(x*x)/2.0 + EIy*6.0*C1*x + 2.0*EIy*C2);
                        }
                        else if (type == QString("MyMax")) {

                                double Mend1 = qAbs(2.0*EIy*C2);
                                double Mend2 = qAbs(qz*(x*x)/2.0 + EIy*6.0*C1*x + 2.0*EIy*C2);
                                if (qz==0.0) {
                                        if (Mend1>Mend2) {
                                                return Mend1;
                                        }
                                        else {
                                                return Mend2;
                                        }
                                }
                                else {
                                        double location = -(EIy*6.0*C1)/qz;
                                        if (location<0.0 || location>L) {
                                                return qMax(Mend1,Mend2);
                                        }
                                        else {
                                                double MatLocation = qAbs(qz*(location*location)/2.0 + EIy*6.0*C1*location + 2.0*EIy*C2);
                                                if (MatLocation>qMax(Mend1,Mend2)) {
                                                        return MatLocation;
                                                }
                                                else {
                                                        return qMax(Mend1, Mend2);
                                                }
                                        }
                                }
                        }
                        else if (type == QString("Vz")) {
                                return (qz*x + EIy*6.0*C1);
                        }
                        else if (type == QString("VzMax")) {
                                double Vend1 = qAbs(EIy*6.0*C1);
                                double Vend2 = qAbs(qz*L + EIy*6.0*C1);
                                return qMax(Vend1, Vend2);
                        }
                }
                else if ( (EIz != 0.0) && ( (type == QString("v")) || (type == QString("dvdx")) || (type == QString("Mz")) || (type == QString("MzMax")) || (type == QString("Vy"))  || (type == QString("VyMax"))    ) ) {
                        // FOR THE BEAM ELEMENT IN THE X-Y PLANE (SEE DOF NUMBERING IN THE FRAME ELEMENT DOCUMENT)

                        // Extract DOF displacements
                        u1 = gsl_vector_get(ul,(2-1));
                        u2 = -gsl_vector_get(ul,(6-1));
                        u3 = gsl_vector_get(ul,(9-1));
                        u4 = -gsl_vector_get(ul,(13-1));

                        // Coefficients in the solution to the differential equation
                        C1 = -qy*L/(12.0*EIz) - 2.0*u3/(L*L*L) - u2/(L*L) + 2.0*u1/(L*L*L) - u4/(L*L);
                        C2 = qy*L*L/(24.0*EIz) + 3.0*u3/(L*L) + 2.0*u2/L - 3.0*u1/(L*L) + u4/L;
                        C3 = -u2;
                        C4 = u1;

                        if (type == QString("v")) {
                                return (qy*(x*x*x*x)/(24.0*EIz) + C1*x*x*x + C2*x*x + C3*x + C4);
                        }
                        else if (type == QString("dvdx")) {
                                return (qy*(x*x*x)/(6.0*EIz) + 3.0*C1*x*x + 2.0*C2*x + C3);
                        }
                        else if (type == QString("Mz")) {
                                return (qy*(x*x)/2.0 + EIz*6.0*C1*x + 2.0*EIz*C2);
                        }
                        else if (type == QString("MzMax")) {
                                double Mend1 = qAbs(2.0*EIz*C2);
                                double Mend2 = qAbs(qy*(x*x)/2.0 + EIz*6.0*C1*x + 2.0*EIz*C2);
                                if (qy==0.0) {
                                        if (Mend1>Mend2) {
                                                return Mend1;
                                        }
                                        else {
                                                return Mend2;
                                        }
                                }
                                else {
                                        double location = -(EIz*6.0*C1)/qy;
                                        rDebug() << "Location of MzMax: " << location;

                                        if (location<0.0 || location>L) {
                                                return qMax(Mend1,Mend2);
                                        }
                                        else {
                                                double MatLocation = qAbs(qy*(location*location)/2.0 + EIz*6.0*C1*location + 2.0*EIz*C2);
                                                if (MatLocation>qMax(Mend1,Mend2)) {
                                                        return MatLocation;
                                                }
                                                else {
                                                        return qMax(Mend1, Mend2);
                                                }
                                        }
                                }
                        }
                        else if (type == QString("Vy")) {
                                return (qy*x + EIz*6.0*C1);
                        }
                        else if (type == QString("VyMax")) {
                                double Vend1 = qAbs(EIz*6.0*C1);
                                double Vend2 = qAbs(qy*L + EIz*6.0*C1);
                                return qMax(Vend1, Vend2);
                        }
                }
                else {
                        return 0.0;
                }

        }
}





gsl_vector * SFrameElement::getSectionResponseInGlobalXYZ(QString type, double xi)
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
        if (type == QString("Displacement")) {

                double u1 = gsl_vector_get(ul,(1-1));
                double u8 = gsl_vector_get(ul,(8-1));

                gsl_vector_set(localResponse, 0, u1+(u8-u1)*xi);
                gsl_vector_set(localResponse, 1, getSectionResponse("v", xi));
                gsl_vector_set(localResponse, 2, getSectionResponse("w", xi));
        }
        else if (type == QString("My")) {
                gsl_vector_set(localResponse, 0, 0.0);
                gsl_vector_set(localResponse, 1, 0.0);
                gsl_vector_set(localResponse, 2, getSectionResponse("My", xi));
        }
        else if (type == QString("Mz")) {
                gsl_vector_set(localResponse, 0, 0.0);
                gsl_vector_set(localResponse, 1, getSectionResponse("Mz", xi));
                gsl_vector_set(localResponse, 2, 0.0);
        }
        else if (type == QString("Vz")) {
                gsl_vector_set(localResponse, 0, 0.0);
                gsl_vector_set(localResponse, 1, 0.0);
                gsl_vector_set(localResponse, 2, getSectionResponse("Vz", xi));
        }
        else if (type == QString("Vy")) {
                gsl_vector_set(localResponse, 0, 0.0);
                gsl_vector_set(localResponse, 1, getSectionResponse("Vy", xi));
                gsl_vector_set(localResponse, 2, 0.0);
        }
        else if (type == QString("N")) {
                gsl_vector_set(localResponse, 0, 0.0);
                gsl_vector_set(localResponse, 1, 0.0);
                gsl_vector_set(localResponse, 2, getSectionResponse("N", 0.0)); // Select to plot the axial force diagram in the local z-direction
        }
        else {
                rDebug() << "A frame element was asked to plot something that it does not have.";
        }

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




int SFrameElement::updateStructure()
{
        // Allocate memory for the transformation matrix
        if (Tlg == 0) {
                Tlg = gsl_matrix_calloc(2*numDOFsPerNode, 2*numDOFsPerNode);
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

                // Determine the "v-vector" from user input
                double v0, v1, v2;
                if (theVectorInLocalXZPlane == QString("GlobalXDirection")) {
                        if ( (dy==0.0) && (dz==0.0) ) {
                                rCritical() << "A frame element is oriented in the same direction as the user-defined Vector-In-Local-XZ-Plane";
                        }
                        v0 = 1.0;
                        v1 = 0.0;
                        v2 = 0.0;
                }
                else if (theVectorInLocalXZPlane == QString("GlobalYDirection")) {
                        if ( (dx==0.0) && (dz==0.0) ) {
                                rCritical() << "A frame element is oriented in the same direction as the user-defined Vector-In-Local-XZ-Plane";
                        }
                        v0 = 0.0;
                        v1 = 1.0;
                        v2 = 0.0;
                }
                else if (theVectorInLocalXZPlane == QString("GlobalZDirection")) {
                        if ( (dy==0.0) && (dx==0.0) ) {
                                rCritical() << "A frame element is oriented in the same direction as the user-defined Vector-In-Local-XZ-Plane";
                        }
                        v0 = 0.0;
                        v1 = 0.0;
                        v2 = 1.0;
                }
                else {
                        rDebug() << "The vector in the local xz-plane of a frame element was incorrect";
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
                int add = 0;
                gsl_matrix_set(Tlg, 0+add, 0+add, x0);  gsl_matrix_set(Tlg, 0+add, 1+add, y0);  gsl_matrix_set(Tlg, 0+add, 2+add, z0);
                gsl_matrix_set(Tlg, 1+add, 0+add, x1);  gsl_matrix_set(Tlg, 1+add, 1+add, y1);  gsl_matrix_set(Tlg, 1+add, 2+add, z1);
                gsl_matrix_set(Tlg, 2+add, 0+add, x2);  gsl_matrix_set(Tlg, 2+add, 1+add, y2);  gsl_matrix_set(Tlg, 2+add, 2+add, z2);

                add = 3;
                gsl_matrix_set(Tlg, 0+add, 0+add, x0);  gsl_matrix_set(Tlg, 0+add, 1+add, y0);  gsl_matrix_set(Tlg, 0+add, 2+add, z0);
                gsl_matrix_set(Tlg, 1+add, 0+add, x1);  gsl_matrix_set(Tlg, 1+add, 1+add, y1);  gsl_matrix_set(Tlg, 1+add, 2+add, z1);
                gsl_matrix_set(Tlg, 2+add, 0+add, x2);  gsl_matrix_set(Tlg, 2+add, 1+add, y2);  gsl_matrix_set(Tlg, 2+add, 2+add, z2);

                add = 7;
                gsl_matrix_set(Tlg, 0+add, 0+add, x0);  gsl_matrix_set(Tlg, 0+add, 1+add, y0);  gsl_matrix_set(Tlg, 0+add, 2+add, z0);
                gsl_matrix_set(Tlg, 1+add, 0+add, x1);  gsl_matrix_set(Tlg, 1+add, 1+add, y1);  gsl_matrix_set(Tlg, 1+add, 2+add, z1);
                gsl_matrix_set(Tlg, 2+add, 0+add, x2);  gsl_matrix_set(Tlg, 2+add, 1+add, y2);  gsl_matrix_set(Tlg, 2+add, 2+add, z2);

                add = 10;
                gsl_matrix_set(Tlg, 0+add, 0+add, x0);  gsl_matrix_set(Tlg, 0+add, 1+add, y0);  gsl_matrix_set(Tlg, 0+add, 2+add, z0);
                gsl_matrix_set(Tlg, 1+add, 0+add, x1);  gsl_matrix_set(Tlg, 1+add, 1+add, y1);  gsl_matrix_set(Tlg, 1+add, 2+add, z1);
                gsl_matrix_set(Tlg, 2+add, 0+add, x2);  gsl_matrix_set(Tlg, 2+add, 1+add, y2);  gsl_matrix_set(Tlg, 2+add, 2+add, z2);

                // The warping DOF is the same in the local and in the global coordinate system
                gsl_matrix_set(Tlg, (7-1), (7-1), 1.0);
                gsl_matrix_set(Tlg, (14-1), (14-1), 1.0);

        }

        return 0;
}








void SFrameElement::plot(QString type)
{

}


bool SFrameElement::canBeRunParallelly() {
	return false;
}