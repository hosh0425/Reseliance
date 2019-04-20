#include "STransformationMatrixAssembler.h"
#include "SNode.h"
#include "SElement.h"
#include "RDomain.h"
#include "RObject.h"
#include "SNodalLoad.h"
#include "SFixedNode.h"
#include "SRigidLink.h"
#include "SAssembler.h"
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_blas.h>
#include <QCoreApplication>

#include <QDir>
#include <QSettings>
#include <QApplication>	



STransformationMatrixAssembler::STransformationMatrixAssembler(QObject *parent, QString &name)
                : SAssembler(parent, name)
{
        Kfinal = 0;
        Kall = 0;
        theLoadVector = 0;
        Taf = 0;
        Td = 0;

        // Standing assumption
        numDOFsPerNode = 7;
}


STransformationMatrixAssembler::~STransformationMatrixAssembler()
{
        if (Kfinal != 0) {
                gsl_matrix_free(Kfinal);
                Kfinal = 0;
        }
        if (Kall != 0) {
                gsl_matrix_free(Kall);
                Kall = 0;
        }
        if (theLoadVector != 0) {
                gsl_vector_free(theLoadVector);
                theLoadVector = 0;
        }
        if (Taf != 0) {
                gsl_matrix_free(Taf);
                Taf = 0;
        }
        if (Td != 0) {
                gsl_matrix_free(Td);
                Td = 0;
        }
}






gsl_matrix * STransformationMatrixAssembler::getStiffnessMatrix()
{
        // Get list of nodes and elements
        QList<SNode *> theNodeList = theDomain->findChildren<SNode *>();
        QList<SElement *> theElementList = theDomain->findChildren<SElement *>();
        int numNodes = theNodeList.count();
        int numElements = theElementList.count();
        
        
        // Initialize the "Kall" stiffness matrix
        int totalNumDOFs = numDOFsPerNode*numNodes;
        if (Kall == 0) {
                Kall = gsl_matrix_calloc(totalNumDOFs, totalNumDOFs);
        }
        else {
                gsl_matrix_free(Kall);
                Kall = gsl_matrix_calloc(totalNumDOFs, totalNumDOFs);
        }




        
        
        // For each element, establish the "Tga" transformation matrix
        // First, initial declarations
        gsl_matrix *Tga = gsl_matrix_alloc(1, 1);
        gsl_matrix *Ktemp = gsl_matrix_alloc(1, 1);
        gsl_matrix *Kglobal;
        int numNodesInElement;
        int numDOFsInElement;
        int elementDOFcounter;
        int nodeTag, elementNodeTag;
        SNode *theNode;
        SNode *theElementNode;
        SElement *theElement;
        
        // Loop over all elements to get Kall
        for (int i=0; i<numElements; i++) {
                
                // Get element pointer and determine the number of DOFs it affects
                numNodesInElement = theElementList[i]->getNumNodes();
                numDOFsInElement = numDOFsPerNode*numNodesInElement;
                
                // Establish the Tga matrix
                gsl_matrix_free(Tga);
                Tga = gsl_matrix_calloc(numDOFsInElement, totalNumDOFs);
                elementDOFcounter=0;
                
                for (int j=0; j<numNodesInElement; j++) {
                        
                        for (int m=0; m<numNodes; m++) {
                                
                                theElementNode = qobject_cast<SNode *>(theElementList[i]->getNode(j));
                                
                                if (theNodeList[m]->objectName() == theElementNode->objectName()) {
                                        
                                        for (int n=0; n<numDOFsPerNode; n++) {
                                                gsl_matrix_set(Tga, elementDOFcounter, (m*numDOFsPerNode)+n, 1.0 );
                                                elementDOFcounter++;
                                        }
                                }
                        }
                }
                
                // Multiply Kall = Tga^T * Kglobal * Tga
                Kglobal = theElementList[i]->getGlobalStiffnessMatrix();

                if (Ktemp) {
                    gsl_matrix_free(Ktemp);
                    Ktemp = 0;
                }
                Ktemp = gsl_matrix_calloc(totalNumDOFs, numDOFsInElement); // Memory leak?
                gsl_blas_dgemm(CblasTrans, CblasNoTrans, 1.0, Tga, Kglobal, 0.0, Ktemp);
                gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, Ktemp, Tga, 1.0, Kall);
                
        }
        gsl_matrix_free(Tga);


        // Set the Taf transformation matrix, which is a data member (It sets Td as well!)
        setTaf();





        // Introduce DOF dependencies
        gsl_matrix *Ktemp3 = gsl_matrix_calloc(totalNumDOFs, totalNumDOFs);
        gsl_blas_dgemm(CblasTrans, CblasNoTrans, 1.0, Td, Kall, 0.0, Ktemp3);
        gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, Ktemp3, Td, 0.0, Kall);






        // Multiply Kfinal = Taf^T * Kall * Taf
        int numFreeDOFs = Taf->size2;
        if (Kfinal == 0) {
                Kfinal = gsl_matrix_calloc(numFreeDOFs, numFreeDOFs);
        }
        else {
                gsl_matrix_free(Kfinal);
                Kfinal = gsl_matrix_calloc(numFreeDOFs, numFreeDOFs);
        }
        gsl_matrix *Ktemp2 = gsl_matrix_calloc(numFreeDOFs, totalNumDOFs);
        gsl_blas_dgemm(CblasTrans, CblasNoTrans, 1.0, Taf, Kall, 0.0, Ktemp2);
        gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, Ktemp2, Taf, 0.0, Kfinal);
        
        





        
        gsl_matrix_free(Ktemp);
        gsl_matrix_free(Ktemp3);
        gsl_matrix_free(Ktemp2);
        
        return Kfinal;
}











gsl_vector * STransformationMatrixAssembler::getLoadVector()
{
        // Get list of nodes, elements, etc
        QList<SNode *> theNodeList = theDomain->findChildren<SNode *>();
        QList<SElement *> theElementList = theDomain->findChildren<SElement *>();
        QList<SNodalLoad *> theNodalLoadList = theDomain->findChildren<SNodalLoad *>();
        int numNodes = theNodeList.count();
        int numElements = theElementList.count();
        int numNodalLoads = theNodalLoadList.count();
        
        
        // Create the load vector for the first time (shouldn't do this every time; make data member) /////////////////////////////////////
        int totalNumDOFs = numDOFsPerNode*numNodes;
        gsl_vector * Fall = gsl_vector_calloc(totalNumDOFs);

        
        // Loop over the NODES and loads to form "Fall"
        SNode *theNode;
        gsl_vector *theNodeLoadVector;
        int counter = 0;
        
        double result;
        
        for (int i=0; i<numNodes; i++) {
                
                theNodeLoadVector = 0;
                
                // Loop over nodal loads
                for (int k=0; k<numNodalLoads; k++) {
                        theNode = qobject_cast<SNode *>(theNodalLoadList[k]->getNode());
                        if (theNodeList[i]->objectName()==theNode->objectName()  ) {
                                theNodeLoadVector = theNodalLoadList[k]->getLoadVector();
                        }
                }

                if (theNodeLoadVector == 0) {

                        // No loads specified at this node
                        counter += numDOFsPerNode;
                }
                else {

                        for (int j=0; j<numDOFsPerNode; j++) {

                                result = gsl_vector_get(theNodeLoadVector, j);
                                gsl_vector_set(Fall,counter, gsl_vector_get(theNodeLoadVector, j));
                                counter++;
                        }
                }
        }

        // ELEMENTS
        gsl_matrix *Tga = gsl_matrix_alloc(1, 1);
        gsl_vector *Fglobal;
        int numNodesInElement;
        int numDOFsInElement;
        int elementDOFcounter;
        SNode *theElementNode;

        for (int i=0; i<numElements; i++) {

                // Get element pointer and determine the number of DOFs it affects
                numNodesInElement = theElementList[i]->getNumNodes();
                numDOFsInElement = numDOFsPerNode*numNodesInElement;

                // Establish the Tga matrix
                gsl_matrix_free(Tga);
                Tga = gsl_matrix_calloc(numDOFsInElement, totalNumDOFs);
                elementDOFcounter=0;

                for (int j=0; j<numNodesInElement; j++) {

                        for (int m=0; m<numNodes; m++) {

                                theElementNode = qobject_cast<SNode *>(theElementList[i]->getNode(j));

                                if (theNodeList[m]->objectName() == theElementNode->objectName()) {

                                        for (int n=0; n<numDOFsPerNode; n++) {
                                                gsl_matrix_set(Tga, elementDOFcounter, (m*numDOFsPerNode)+n, 1.0 );
                                                elementDOFcounter++;
                                        }
                                }
                        }
                }

                // Multiply Fall = Tga^T * Fall
                Fglobal = theElementList[i]->getGlobalLoadVector();
                gsl_blas_dgemv(CblasTrans, 1.0, Tga, Fglobal, 1.0, Fall);
        }




        // Introduce DOF depdendencies *** ///////////////////
        gsl_vector *TempFall = gsl_vector_calloc(totalNumDOFs);
        gsl_vector_memcpy(TempFall, Fall);
        gsl_blas_dgemv(CblasTrans, 1.0, Td, TempFall, 0.0, Fall);





        // Create the load vector pointer for the first time (this business should be cleaned up!)
        int numFreeDOFs = Taf->size2;
        if (theLoadVector == 0) {
                theLoadVector = gsl_vector_calloc(numFreeDOFs);
        }
        else {
                gsl_vector_free(theLoadVector);
                theLoadVector = gsl_vector_calloc(numFreeDOFs);
        }


        // Do the multiplication from Fall to Ffinal
        gsl_blas_dgemv(CblasTrans, 1.0, Taf, Fall, 0.0, theLoadVector);


        // Free allocated vectors/matrices
        gsl_matrix_free(Tga);
        gsl_vector_free(Fall);
        gsl_vector_free(TempFall);
        //gsl_vector_free(Fglobal);  Why is it not possible to make this deletion? Change the messy allocation above

        return theLoadVector;
}







int STransformationMatrixAssembler::setDisplacements(gsl_vector *ufinal)
{
        // Get list of nodes and elemenents
        QList<SElement *> theElementList = theDomain->findChildren<SElement *>();
        QList<SNode *> theNodeList = theDomain->findChildren<SNode *>();
        QList<SFixedNode *> theFixedNodeList = theDomain->findChildren<SFixedNode *>();
        int numNodes = theNodeList.count();
        int numElements = theElementList.count();

        
        // Transform "ufinal" into "uall"
        int totalNumDOFs = numDOFsPerNode*numNodes;
        gsl_vector *uall = gsl_vector_calloc(totalNumDOFs);
        gsl_blas_dgemv(CblasNoTrans, 1.0, Taf, ufinal, 0.0, uall);



        
        // Apply the Td matrix for DOF dependencies
        gsl_vector * uallTemp = gsl_vector_calloc(totalNumDOFs);
        gsl_vector_memcpy(uallTemp, uall);
        gsl_blas_dgemv(CblasNoTrans, 1.0, Td, uallTemp, 0.0, uall);


        // Compute the load vector, for the purpose of determining support reactions
        gsl_vector * Fall = gsl_vector_calloc(totalNumDOFs);
        gsl_blas_dgemv(CblasNoTrans, 1.0, Kall, uall, 0.0, Fall);


        // Add the "particular solution" to F=Ku, i.e., add the effect of element loads .................................................................................... terje now
        gsl_matrix *Tga = gsl_matrix_alloc(1, 1);
        int numNodesInElement;
        int numDOFsInElement;
        int elementDOFcounter;
        SNode *theElementNode;
        gsl_vector *Fglobal;
        for (int i=0; i<numElements; i++) {

                // Get element pointer and determine the number of DOFs it affects
                numNodesInElement = theElementList[i]->getNumNodes();
                numDOFsInElement = numDOFsPerNode*numNodesInElement;

                // Establish the Tga matrix
                gsl_matrix_free(Tga);
                Tga = gsl_matrix_calloc(numDOFsInElement, totalNumDOFs);
                elementDOFcounter=0;

                for (int j=0; j<numNodesInElement; j++) {

                        for (int m=0; m<numNodes; m++) {

                                theElementNode = qobject_cast<SNode *>(theElementList[i]->getNode(j));

                                if (theNodeList[m]->objectName() == theElementNode->objectName()) {

                                        for (int n=0; n<numDOFsPerNode; n++) {
                                                gsl_matrix_set(Tga, elementDOFcounter, (m*numDOFsPerNode)+n, 1.0 );
                                                elementDOFcounter++;
                                        }
                                }
                        }
                }

                // Multiply Fall = Tga^T * Fall
                Fglobal = theElementList[i]->getGlobalLoadVector();
                gsl_blas_dgemv(CblasTrans, -1.0, Tga, Fglobal, 1.0, Fall);
        }
        gsl_matrix_free(Tga);
        //gsl_vector_free(Fglobal);  Why is it not possible to make this deletion? Change the messy allocation above























        
        // Loop over the nodes and set the displacements
        int uAllCounter = 0;
        int FallCounter = 0;
        SNode *theNode;
        for (int i=0; i<numNodes; i++) {
                

                // Set displacements at nodes
                for (int j=0; j<numDOFsPerNode; j++) {
                        
                        theNodeList[i]->setDisplacementVector(j, gsl_vector_get(uall, uAllCounter));
                        uAllCounter++;
                }


                // Set forces (reactions) at supports
                for (int j=0; j<theFixedNodeList.count(); j++) {

                        theNode = qobject_cast<SNode *>(theFixedNodeList[j]->getNode());

                        if ( theNodeList[i]->objectName() == theNode->objectName() ) {

                                for (int jj=0; jj<numDOFsPerNode; jj++) {

                                        theFixedNodeList[j]->setReactionForceVector(jj, gsl_vector_get(Fall, FallCounter+jj));
                                }

                        }


                }
                FallCounter += 7;


        }


        // Loop over the elements and set their forces now that the nodes have new displacements
        for (int i=0; i<theElementList.count(); i++) {
                theElementList[i]->updateResponse();
        }
        
        
        // Free vectors and matrices
        gsl_vector_free(Fall);
        gsl_vector_free(uall);
        gsl_vector_free(uallTemp);

        return 0;
}



int STransformationMatrixAssembler::setTaf()
{
        // This method sets Td too...


        // Initial declarations
        QList<SNode *> theNodeList = theDomain->findChildren<SNode *>();
        QList<SFixedNode *> theFixedNodeList = theDomain->findChildren<SFixedNode *>();
        QList<SRigidLink *> theLinkList = theDomain->findChildren<SRigidLink *>();
        int numNodes = theNodeList.count();
        int numFixedNodes = theFixedNodeList.count();
        int numLinks = theLinkList.count();
        int allDOFcounter;
        int totalNumDOFs = numDOFsPerNode*numNodes;
        gsl_vector *theGlobalBCs = gsl_vector_calloc(totalNumDOFs);
        
        
        gsl_vector *theBCs;
        allDOFcounter=0;
        SNode *theNode;
        SNode *theDependentNode;
        SNode *theIndependentNode;

        for (int ii=0; ii<numNodes; ii++) {

                theBCs = 0;

                // Loop over fixed nodes ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
                for (int k=0; k<numFixedNodes; k++) {
                        theNode = qobject_cast<SNode *>(theFixedNodeList[k]->getNode());
                        if (theNodeList[ii]->objectName()==theNode->objectName()) {
                                theBCs = theFixedNodeList[k]->getBoundaryConditionVector();
                        }
                }

                // Loop over dependent DOFs and lock them ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
                for (int k=0; k<numLinks; k++) {
                        theDependentNode = qobject_cast<SNode *>(theLinkList[k]->getDependentNode());
                        if (theNodeList[ii]->objectName()==theDependentNode->objectName()) {
                                if (theBCs != 0) {
                                        rCritical() << "A node cannot have both fixed and dependent DOFs.";
                                }
                                theBCs = theLinkList[k]->getBoundaryConditionVector();
                        }
                }

                // Remove DOFs because they have no stiffness
                if (theBCs==0) {
                        for (int i=0; i<numDOFsPerNode; i++) {
                                if (gsl_matrix_get(Kall,allDOFcounter,allDOFcounter) == 0.0) { // Zero stiffness on the diagonal of the stiffness matrix
                                        if (getOutputDisplayLevel() >= RObject::Minimum) {
                                                rDebug() << "DOF number" << (i+1) << "of" << theNodeList[ii]->objectName() << "was free with zero stiffness; it is now restrained";
                                        }
                                        gsl_vector_set(theGlobalBCs,allDOFcounter,1.0);
                                }
                                allDOFcounter++;
                        }
                }
                // Remove DOFs either because the user fix them or because they have no stiffness
                else {
                        for (int i=0; i<numDOFsPerNode; i++) {
                                if (gsl_vector_get(theBCs,i) == 1.0) {
                                        gsl_vector_set(theGlobalBCs,allDOFcounter,1.0);
                                }
                                else if (gsl_matrix_get(Kall,allDOFcounter,allDOFcounter) == 0.0) { // Zero stiffness on the diagonal of the stiffness matrix
                                        if (getOutputDisplayLevel() >= RObject::Minimum) {
                                                rDebug() << "DOF number" << (i+1) << "of" << theNodeList[ii]->objectName() << "was free with zero stiffness; it is now restrained";
                                        }
                                        
                                        gsl_vector_set(theGlobalBCs,allDOFcounter,1.0);
                                }
                                allDOFcounter++;
                        }
                }
        }
        








        // Initialize the Taf matrix
        int numFreeDOFs = totalNumDOFs-gsl_blas_dasum(theGlobalBCs);
        if (Taf == 0) {
                Taf = gsl_matrix_calloc(totalNumDOFs, numFreeDOFs);
        }
        else {
                gsl_matrix_free(Taf);
                Taf = gsl_matrix_calloc(totalNumDOFs, numFreeDOFs);
        }


        // Establish the Taf matrix
        int rowCounter=0;
        int colCounter=0;
        for (int i=0; i<totalNumDOFs; i++) {

                if (gsl_vector_get(theGlobalBCs,i) == 1.0) {
                        rowCounter++;
                }
                else {
                        gsl_matrix_set(Taf, rowCounter, colCounter, 1.0 );
                        rowCounter++;
                        colCounter++;
                }
        }









        // Initialize the Td matrix *** ////////////
        if (Td == 0) {
                Td = gsl_matrix_calloc(totalNumDOFs, totalNumDOFs);
        }
        else {
                gsl_matrix_free(Td);
                Td = gsl_matrix_calloc(totalNumDOFs, totalNumDOFs);
        }
        gsl_matrix_set_identity(Td);


        // Establish the Td matrix
        int indexOfDependentNode;
        int indexOfIndependentNode;
        for (int k=0; k<numLinks; k++) {
                theDependentNode = qobject_cast<SNode *>(theLinkList[k]->getDependentNode());
                theIndependentNode = qobject_cast<SNode *>(theLinkList[k]->getIndependentNode());

                indexOfDependentNode = theNodeList.indexOf(theDependentNode);
                indexOfIndependentNode = theNodeList.indexOf(theIndependentNode);

                theBCs = theLinkList[k]->getBoundaryConditionVector();

                for (int i=0; i<numDOFsPerNode; i++) {

                        if (gsl_vector_get(theBCs,i) != 0.0) {


                                // Set zero on the diagonal
                                gsl_matrix_set(Td, indexOfDependentNode*numDOFsPerNode+i, indexOfDependentNode*numDOFsPerNode+i, 0.0 );

                                // Set the dependency factor further over in row
                                gsl_matrix_set(Td, indexOfDependentNode*numDOFsPerNode+i, indexOfIndependentNode*numDOFsPerNode+i, gsl_vector_get(theBCs,i) );
                        }

                }
        }



        
        // Free vectors and matrices
        gsl_vector_free(theGlobalBCs);
        
        return 0;
}

bool STransformationMatrixAssembler::canBeRunParallelly() {	
	return false;
}





