// Last modified by Mojtaba on March 19, 2014

#include "RStepperNonlinSingleConstrSolver.h"

#include <QCoreApplication>
#include <QApplication>

#include "RDomain.h"
#include "RModel.h"
#include "RFunction.h"
#include "RStepSizeSearcher.h"
#include "RStepDirectionSearcher.h"
#include "RTransformer.h"
#include "RConvergenceChecker.h"
#include "RRandomVariable.h"
#include "RContinuousRandomVariable.h"
#include "RResponse.h"
#include "RMainWindow.h"
#include "RPlotWidget.h"
#include "RNoCorrelationTransformer.h"
#include "RArmijoStepSizeSearcher.h"

//#include <qwt_plot.h>
//#include <qwt_plot_curve.h> 

#include <gsl/gsl_blas.h>
#include <gsl/gsl_cdf.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_errno.h>

RStepperNonlinSingleConstrSolver::RStepperNonlinSingleConstrSolver(QObject *parent, QString name)
	: RNonlinSingleConstrSolver(parent, name)
{
	/* //$$ 
	theStepSizeSearcher = 0;
	theStepDirectionSearcher = 0;
	theTransformer = 0;
	theConvergenceChecker = 0;
	*/
	theMaximumIterations = 100;

	x = 0;
	u = 0;
	iterationIndex = 0;
	functionValueFirst = 0;
	functionValueLast = 0;
	alpha = 0;
	gamma = 0;
	delta = 0;
	eta = 0;
	uSecondLast = 0;
	alphaSecondLast = 0;
	stepDirection = 0;
	gradientInStandardNormalSpace = 0;
	theDistance = 0;
	theFailureProbability = 0;
	theFirstPrincipalCurvature = 0;
}

RStepperNonlinSingleConstrSolver::~RStepperNonlinSingleConstrSolver()
{
	deleteVectorsMatrices();
}

void RStepperNonlinSingleConstrSolver::deleteVectorsMatrices()
{
	if (x) {
		gsl_vector_free(x);
		x = 0;
	}
	if (u) {
		gsl_vector_free(u);
		u = 0;
	}
	if (gradientInStandardNormalSpace) {
		gsl_vector_free(gradientInStandardNormalSpace);
		gradientInStandardNormalSpace = 0;
	}
	if (alpha) {
		gsl_vector_free(alpha);
		alpha = 0;
	}
	if (gamma) {
		gsl_vector_free(gamma);
		gamma = 0;
	}
	if (delta) {
		gsl_vector_free(delta);
		delta = 0;
	}
	if (eta) {
		gsl_vector_free(eta);
		eta = 0;
	}
	if (alphaSecondLast) {
		gsl_vector_free(alphaSecondLast);
		alphaSecondLast = 0;
	}
	if (uSecondLast) {
		gsl_vector_free(uSecondLast);
		uSecondLast = 0;
	}
	stepDirection = 0;
	iterationIndex = 0;
	functionValueFirst = 0;
	functionValueLast = 0;
	theDistance = 0;
}

QObject * RStepperNonlinSingleConstrSolver::getStepSizeSearcher() const
{
	return theStepSizeSearcher.data();
}

void RStepperNonlinSingleConstrSolver::setStepSizeSearcher(QObject *value)
{
	theStepSizeSearcher = qobject_cast<RStepSizeSearcher *>(value);
}

QObject * RStepperNonlinSingleConstrSolver::getStepDirectionSearcher() const
{
	return theStepDirectionSearcher.data();
}

void RStepperNonlinSingleConstrSolver::setStepDirectionSearcher(QObject *value)
{
	theStepDirectionSearcher = qobject_cast<RStepDirectionSearcher *>(value);
}

QObject * RStepperNonlinSingleConstrSolver::getTransformer() const
{
	return theTransformer.data();
}

void RStepperNonlinSingleConstrSolver::setTransformer(QObject *value)
{
	theTransformer = qobject_cast<RTransformer *>(value);
}

QObject * RStepperNonlinSingleConstrSolver::getConvergenceChecker() const
{
	return theConvergenceChecker.data();
}

void RStepperNonlinSingleConstrSolver::setConvergenceChecker(QObject *value)
{
	theConvergenceChecker = qobject_cast<RConvergenceChecker *>(value);
}

int RStepperNonlinSingleConstrSolver::getMaximumIterations() const
{
	return theMaximumIterations;
}

void RStepperNonlinSingleConstrSolver::setMaximumIterations(int value)
{
	theMaximumIterations = value;
	if (theMaximumIterations < 1) {
		theMaximumIterations = 1;
	}
}

int RStepperNonlinSingleConstrSolver::findDesignPoint(RFunction *passedFunction, bool populateListsInFunction)
{
	// Checking if all pointer properties are filled out
	int status = checkProperties();
	if (status < 0) {
		rCritical() << "Error: Could find one or more necessary tools in the object" << objectName() << ".";
		return -1;
	}
	
	status = theTransformer.data()->checkProperties();
	if (status < 0) {
		rCritical() << "Error: Could find one or more necessary tools in the object" << theTransformer.data()->objectName() << ".";
		return -1;
	}
	
	
	status = theConvergenceChecker.data()->checkProperties();
	if (status < 0) {
		rCritical() << "Error: Could find one or more necessary tools in the object" << theConvergenceChecker.data()->objectName() << ".";
		return -1;
	}
	
	status = theStepSizeSearcher.data()->checkProperties();
	if (status < 0) {
		rCritical() << "Error: Could find one or more necessary tools in the object" << theStepSizeSearcher.data()->objectName() << ".";
		return -1;
	}
	RArmijoStepSizeSearcher *stepSearcher = qobject_cast<RArmijoStepSizeSearcher *>(theStepSizeSearcher.data());
	if (stepSearcher) {
		QObject *stepSearcherTransformer = stepSearcher->getTransformer();
		if (stepSearcherTransformer != theTransformer.data()) {
			rCritical() << "Error: The transfomer given to the step size searcher" << theStepSizeSearcher.data()->objectName() << "must be he same as the one given to the solver" << this->objectName() << ".";
			return -1;
		}
	}
	
	status = theStepDirectionSearcher.data()->checkProperties();
	if (status < 0) {
		rCritical() << "Error: Could find one or more necessary tools in the object" << theStepDirectionSearcher.data()->objectName() << ".";
		return -1;
	}
	
	int result = 0;

	// Reset the vectors and matrices
	deleteVectorsMatrices();

	// Parse the passed function
	RFunction *theFunction = passedFunction;

	if (populateListsInFunction) {
		result = theFunction->populateLists();
		if (result < 0) {
			rCritical() << "Error: Could not parse the function" << theFunction->objectName() << ".";
			return -1;
		}
	}
	
	//Create the starting point vector by taking the current value of the used random variables in the function
	QList<RRandomVariable *> *randomVariableList = theFunction->getRandomVariableList();
	//QList<RModel *> *modelList = theFunction->getModelList();
	int randomVariablesCount = randomVariableList->count();
	if (randomVariablesCount == 0) {
		rCritical() << "Error: The function" << theFunction->objectName() << "is not a function of any random variables.";
		return -1;
	}

	x = gsl_vector_calloc(randomVariablesCount);
	for (int i = 0; i < randomVariablesCount; i++) {
		double xi;
		if (theStartPointType == RObject::CurrentValue) {
			xi = randomVariableList->value(i)->getCurrentValue();
		} else if (theStartPointType == RObject::Mean) {
			xi = randomVariableList->value(i)->getMeanValue(0);
		} else if (theStartPointType == RObject::Origin) {
			// Do nothing. Will deal with this later in the code.
		}

		gsl_vector_set(x, i, xi);
	}
	u = gsl_vector_calloc(randomVariablesCount);

	// Set the random variable list in theTransformer, which also intilizes the transfomer for a new analysis
	result = theTransformer.data()->setRandomVariableList(randomVariableList);
	if (result < 0) {
		rCritical() << "Error: Could not set the list of random variables to the transformer.";
		return -1;
	}

	double functionValueOld = 1.0;
	double functionValue = 1.0;
	gsl_vector *gradientInStandardNormalSpaceOld = gsl_vector_calloc(randomVariablesCount);
	gradientInStandardNormalSpace = gsl_vector_calloc(randomVariablesCount);
	alpha = gsl_vector_calloc(randomVariablesCount);
	gamma = gsl_vector_calloc(randomVariablesCount);
	delta = gsl_vector_calloc(randomVariablesCount);
	eta = gsl_vector_calloc(randomVariablesCount);
	alphaSecondLast = gsl_vector_calloc(randomVariablesCount);
	uSecondLast = gsl_vector_calloc(randomVariablesCount);
	gsl_vector *uOld = gsl_vector_calloc(randomVariablesCount);
	gsl_vector *functionGradient;
	double gradientInStandardNormalSpaceNorm = 0;
	double stepSize;
	int evaluationInStepSizeSearcher = 0;


#ifndef NO_QCP

	RMainWindow *mainWindow = qobject_cast<RMainWindow *>(parent()->parent());
	QPointer<QCustomPlot> thePlot; //salarifard 
	if (qobject_cast<QApplication*>(QCoreApplication::instance()) != 0) { //salarifard : in GUI mode
		thePlot = new RPlotWidget(mainWindow);
		mainWindow->addSubWindow(thePlot, objectName() + " Plot", true);

		thePlot->setInteraction(QCP::iRangeDrag, true);
		thePlot->setInteraction(QCP::iRangeZoom);
		//thePlot->setInteraction(QCPAxisRect::rangeZoomAxes(Qt::Horizontal|Qt::Vertical), true);
		QList<QCPAxis*> axisrect;
		axisrect << thePlot->xAxis << thePlot->yAxis;
		thePlot->axisRect()->setRangeDragAxes(axisrect);
		thePlot->axisRect()->setRangeZoomAxes(axisrect);
		thePlot->xAxis->setLabel("Step Number");
		thePlot->yAxis->setLabel("Distance");
		//thePlot->yAxis2->setLabel("Distance from origin");
		thePlot->yAxis2->setVisible(true);

		thePlot->addGraph(thePlot->xAxis, thePlot->yAxis);
		thePlot->graph(0)->setAntialiased(true);
		thePlot->graph(0)->setPen(QPen(Qt::red));
		thePlot->graph(0)->setName("Distance from origin");

		thePlot->addGraph(thePlot->xAxis, thePlot->yAxis);
		thePlot->graph(1)->setAntialiased(true);
		thePlot->graph(1)->setPen(QPen(Qt::blue));
		thePlot->graph(1)->setName("Distance from previous point");
	}

#endif // NO_QCP


	QVector<double> theStepNumberVector;
	theStepNumberVector.append(0.0);
	QVector<double> theuVector;
	theuVector.append(0.0);
	QVector<double> theuOldVector;
	theuOldVector.append(0.0);
	
	// Transform starting point into standard normal space
	if (theStartPointType != RObject::Origin) {
		result = theTransformer.data()->transformPointToStandardNormalSpace(x, u,0);
		if (result < 0) {
			rCritical() << "Error: Could not transform from original space to standard-normal space.";
			return -1;
		}
	}

	
	//Loop to find the design point
	iterationIndex = 1;

	while (iterationIndex <= theMaximumIterations)
	{		
		// A line to keep the GUI responsive
		QCoreApplication::processEvents();

		if (iterationIndex == 1) {
			if (getOutputDisplayLevel() >= RObject::Minimum) {
				rDebug() << "Step 0 :";
			}
		}

		if ((theStartPointType == RObject::Origin) && (iterationIndex == 1))  {
			gsl_vector_set_zero(u);
		}

		if ((iterationIndex > 1) || ((iterationIndex == 1) && (theStartPointType == RObject::Origin))) {
			// Transform from u to x space
			result = theTransformer.data()->transformPointToOriginalSpace(u, x,0);
			if (result < 0) {
				rCritical() << "Error: Could not transform from standard-normal space to original space.";
				return -1;
			}
		} else {
			// Print the x point to console if it is not printed in theTransformer
			if ((theTransformer.data()->getOutputDisplayLevel() >= RObject::Moderate) && (getOutputDisplayLevel() >= RObject::Moderate)) {
				rDebug() << "Current point in original space, x:";
				theDomain->print(x, 0);
				rDebug() << "";
			}
		}


		// UPDATING THE CURRENT VALUE OF RANDOM VARIABLES TO THE VALUE OF NEW REALIZATIONS
		for (int i = 0; i < randomVariablesCount; i++) {
			RRandomVariable *randomVariable = randomVariableList->value(i);
			randomVariable->setCurrentValue(gsl_vector_get(x, i));
		}

		
		// Print the x point to console if it is not printed in theTransformer
		if ((theTransformer.data()->getOutputDisplayLevel() < RObject::Moderate) && (getOutputDisplayLevel() >= RObject::Moderate)) {
			rDebug() << "Current point in original space, x:";
			theDomain->print(x, 0);
			rDebug() << "";
		}
		// Print the u point to console
		if ((getOutputDisplayLevel() >= RObject::Moderate)) {
			rDebug() << "Current point in standard-normal space, u:";
			theDomain->print(u, 0);
			rDebug() << "";
		}

		// Evaluate limit-state function unless it has been done in a trial step by the "theStepSizeSearcher"
		if (evaluationInStepSizeSearcher == 0) {
			// Calling the method to update the isAnalyzed flags of RResponses due to the new realization of random variables
			theFunction->updateAnalysisStatus(0);

			result = theFunction->evaluateFunction(true, RObject::RandomVariable,0);
			if (result < 0) {
				rCritical() << "Error: Could not evaluate the function" << theFunction->objectName() << ".";
				return -1;
			}
			functionValueOld = functionValue;
			functionValue = theFunction->getFunctionValue();
		}

		// Set scale parameter
		if (iterationIndex == 1)	{
			functionValueFirst = functionValue;
			if (getOutputDisplayLevel() >= RObject::Minimum) {
				rDebug() << "Limit-state function value at start point =" << functionValue;
				rDebug() << "";
			}
			if (functionValue == 0) {
				rCritical() << "Warning: The limit-state function value at start point is Zero. This will cuase problems in checking the convergence of the analysis.";
			}
			theConvergenceChecker.data()->setScaleValue(functionValue);
		}

		// Gradient in original space
		result = theFunction->evaluateGradient(RObject::RandomVariable);
		// Attention: "functionGradient" should already be allocated in "theFunction", here just the memory address is obtained.
		functionGradient = theFunction->getGradient();
		if (result < 0) {
			rCritical() << "Error: Could not evaluate the gradient of the function.";
			return -1;
		}
		if (getOutputDisplayLevel() >= RObject::Maximum) {
			rDebug() << "Gradient in original space:";
			theDomain->print(functionGradient, 0);
			rDebug() << "";
		}

		// Check if all components of the vector is zero
		double functionGradientNorm = gsl_blas_dnrm2(functionGradient);
		if (functionGradientNorm == 0) {
			rCritical() << "Error: All components of the gradient vector are zero.";
			return -1;
		}
	

		gsl_vector_memcpy(gradientInStandardNormalSpaceOld, gradientInStandardNormalSpace);
		// Gradient in standard normal space
		result = theTransformer.data()->transformGradientToStandardNormalSpace(x, u, functionGradient, gradientInStandardNormalSpace, 0);
		if (result < 0) {
			rCritical() << "Error: Could not transform the gradient to standard-normal space.";
			return -1;
		}


		if (getOutputDisplayLevel() >= RObject::Maximum) {
			rDebug() << "Gradient in standard-normal space:";
			theDomain->print(gradientInStandardNormalSpace, 0);
			rDebug() << "";
		}


		// Compute the norm of the gradient in standard normal space
		gradientInStandardNormalSpaceNorm = gsl_blas_dnrm2(gradientInStandardNormalSpace);

		// Check that the norm is not zero
		if (gradientInStandardNormalSpaceNorm == 0.0) {
			rCritical() << "Error: The norm of the gradient in standard-normal space is zero.";
			return -1;
		}
		
		// Compute alpha-vector using BLAS: alpha = gradientInStandardNormalSpace * (-1 / gradientInStandardNormalSpaceNorm);
		gsl_vector_memcpy(alpha, gradientInStandardNormalSpace);
		gsl_vector_scale(alpha, -1.0/gradientInStandardNormalSpaceNorm);








		// Check convergence
		result = theConvergenceChecker.data()->check(u, functionValue, gradientInStandardNormalSpace);
		if (result > 0)  {
			// Design point found
			rDebug() << "";
			rDebug() << "Search for the design point has been completed successfully.";

			RObject::ROutputDisplayLevelType transformerDisplayLevel = theTransformer.data()->getOutputDisplayLevel();
			theTransformer.data()->setOutputDisplayLevel(RObject::None);
			result = theTransformer.data()->transformPointToOriginalSpace(u, x,0);
			if (result < 0) {
				rCritical() << "Error: Could not transform from standard-normal space to original space.";
				return -1;
			}			
			theTransformer.data()->setOutputDisplayLevel(transformerDisplayLevel);

			// Print the design point to console
			if (getOutputDisplayLevel() >= RObject::Minimum) {
				rDebug() << "Design point in original space, x:";
				theDomain->print(x, 0);
				rDebug() << "";
				rDebug() << "Design point in standard-normal space, u:";
				theDomain->print(u, 0);
				rDebug() << "";
			}

			
			RNoCorrelationTransformer *noCorrelationTrans = qobject_cast<RNoCorrelationTransformer *>(theTransformer.data());
			if (noCorrelationTrans) {			// gamma = alpha
				gsl_vector_memcpy(gamma, alpha);
			} 
			else {								// gamma should be evaluated
				// Allocate jacobian matrices
				gsl_matrix *jacobian_x_u = gsl_matrix_calloc(randomVariablesCount, randomVariablesCount);
				gsl_matrix *jacobian_u_x = gsl_matrix_calloc(randomVariablesCount, randomVariablesCount);

				// Calculate the jacobians
				result = theTransformer.data()->computeJacobian(x, u, jacobian_u_x, RTransformer::u_x, 0);
				if (result < 0) {
					rCritical() << "Error: Could not compute the Jacobian matrix du/dx.";
					return -1;
				}
				result = theTransformer.data()->computeJacobian(x, u, jacobian_x_u, RTransformer::x_u, 0);
				if (result < 0) {
					rCritical() << "Error: Could not compute the Jacobian matrix dx/du.";
					return -1;
				}

				// Compute the gamma vector
				gsl_matrix *jacobianProduct = gsl_matrix_calloc(randomVariablesCount, randomVariablesCount);
				// Compute using BLAS: jacobianProduct = jacobian_x_u * transpose(jacobian_x_u)
				int gsl_status = gsl_blas_dgemm(CblasNoTrans, CblasTrans, 1.0, jacobian_x_u, jacobian_x_u, 0.0, jacobianProduct);
				if (gsl_status) {  
					rCritical() << "Error in GSL:" << gsl_strerror(gsl_status);
				}
				// Allocate and compute D_prime
				gsl_matrix *D_prime = gsl_matrix_calloc(randomVariablesCount, randomVariablesCount);
				for (int i = 0; i < randomVariablesCount; i++) {
					gsl_matrix_set(D_prime, i, i, sqrt(gsl_matrix_get(jacobianProduct, i, i)));
				}

				// Compute using BLAS:  gamma = D_prime * jacobian_u_x * alpha
				gsl_vector *tempProduct = gsl_vector_calloc(randomVariablesCount);
				gsl_blas_dgemv(CblasTrans, 1.0, jacobian_u_x, alpha, 0.0, tempProduct);
				gsl_blas_dgemv(CblasNoTrans, 1.0, D_prime, tempProduct, 0.0, gamma);
				
				// Normalize Gamma: WHY???				
				double gammaNorm = gsl_blas_dnrm2(gamma);
				gsl_vector_scale(gamma, 1.0 / gammaNorm);

				// Free temporary matrices and vectors
				if (jacobian_x_u) {
					gsl_matrix_free(jacobian_x_u);
					jacobian_x_u = 0;
				}
				if (jacobian_u_x) {
					gsl_matrix_free(jacobian_u_x);
					jacobian_u_x = 0;
				}
				if (jacobianProduct) {
					gsl_matrix_free(jacobianProduct);
					jacobianProduct = 0;
				}
				if (D_prime) {
					gsl_matrix_free(D_prime);
					D_prime = 0;
				}
				if (tempProduct) {
					gsl_vector_free(tempProduct);
					tempProduct = 0;
				}
			}
			

			functionValueLast = functionValue;

			// theDistance (Beta) using BLAS:  Beta = alpha * u
			gsl_blas_ddot(alpha, u, &theDistance);

			// TO DO: Impelement the caluclation of other importance measures, such as delta and eta

			// Assigning the importance vector (alpha) elements to random variables
			for (int i = 0; i < randomVariablesCount; i++) {
				RContinuousRandomVariable *cRV = qobject_cast<RContinuousRandomVariable *>(randomVariableList->value(i));	
				cRV->setAlphaImportanceMeasure(gsl_vector_get(alpha, i));
				cRV->setGammaImportanceMeasure(gsl_vector_get(gamma, i));
			}

			// Print the importance measures to console
			if (getOutputDisplayLevel() >= RObject::Moderate) {
				rDebug() << "Alpha importance measure:";
				theDomain->print(alpha, 0);
				rDebug() << "";
				rDebug() << "Gamma importance measure:";
				theDomain->print(gamma, 0);
				rDebug() << "";
			}



			/*rDebug() << "u";
			theDomain->print(u);
			rDebug() << "uSecondLast";
			theDomain->print(uSecondLast);
			rDebug() << "alpha";
			theDomain->print(alpha);
			rDebug() << "alphaSecondLast";
			theDomain->print(alphaSecondLast);*/



			theFailureProbability = 1 - gsl_cdf_gaussian_P(theDistance - 0, 1);


			return 0;
		}
		
		// Store 'u' and 'alpha' at the second last iteration point
		gsl_vector_memcpy(uSecondLast, u);
		gsl_vector_memcpy(alphaSecondLast, alpha);

		// Print the step number to console
		if (getOutputDisplayLevel() >= RObject::Minimum) {
			rDebug() << "";
			rDebug() << "Step" << iterationIndex <<":";
		}

		// What is this??????????????
		/*// Update Hessian approximation, if any
		if (  (theHessianApproximation!=0) && (i!=1)  ) {
			theHessianApproximation->updateHessianApproximation(u_old, gFunctionValue_old, gradientInStandardNormalSpace_old, stepSize, searchDirection, gFunctionValue, gradientInStandardNormalSpace);
		}*/


		// Determine the step direction
		result = theStepDirectionSearcher.data()->computeStepDirection(iterationIndex, u, functionValue, gradientInStandardNormalSpace);			
		if (result < 0) {
			rCritical() << "Error: Could not compute the step direction.";
			return -1;
		}
		// Attention: "stepDirection" should already be allocated in "theStepDirectionSearcher", here just the memory address is obtained.
		stepDirection = theStepDirectionSearcher.data()->getSearchDirection();
		
		// Determine the step size
		result = theStepSizeSearcher.data()->computeStepSize(iterationIndex, u, theFunction, functionValue, gradientInStandardNormalSpace, stepDirection, randomVariableList);
		if (result < 0) { // (something went wrong)
			rCritical() << "Error: Could not compute step size.";
			return -1;
		}
		else if (result == 0) {  // (nothing was evaluated in step size)
			evaluationInStepSizeSearcher = 0;
		} else if (result == 1) {  // (the function was evaluated)
			evaluationInStepSizeSearcher = 1;
			functionValueOld = functionValue;
			functionValue = theStepSizeSearcher.data()->getFunctionValue();
		}
		stepSize = theStepSizeSearcher.data()->getStepSize();

		// Determine new iteration point and take the step
		gsl_vector_memcpy(uOld, u);
		//uOld = u;
		// Compute using BLAS: u = u_old + searchDirection * stepSize
		gsl_vector_memcpy(u, stepDirection);
		gsl_vector_scale(u, stepSize);

		double uMinus_uOldNorm = gsl_blas_dnrm2(u);

		gsl_vector_add(u, uOld);
		double uNorm = gsl_blas_dnrm2(u);

#ifndef NO_QCP

		// Plotting the distance to origin

		if (qobject_cast<QApplication*>(QCoreApplication::instance()) != 0) {
			// Instatiating thePlot
			if (!thePlot) {
			}
			// Updating the plotting value vectors
			theStepNumberVector.append(iterationIndex);
			theuVector.append(uNorm);
			theuOldVector.append(uMinus_uOldNorm);


			if (qobject_cast<QApplication*>(QCoreApplication::instance()) != 0) { //salarifard: in GUI mode
			// Doing the plot
				thePlot->graph(0)->setData(theStepNumberVector, theuVector);
				thePlot->graph(0)->setPen(QPen(Qt::red));
				thePlot->graph(1)->setData(theStepNumberVector, theuOldVector);
				thePlot->graph(1)->setPen(QPen(Qt::blue));

				// Rescaling the axes
				thePlot->yAxis->setRangeLower(0.0);
				thePlot->xAxis->setRangeLower(0.0);
				thePlot->yAxis->setRangeUpper(theuVector.last());
				thePlot->xAxis->setRangeUpper(theStepNumberVector.last());
				thePlot->graph(0)->rescaleAxes(true);
				thePlot->graph(1)->rescaleAxes(true);

				thePlot->replot();
			}
		}
#endif // NO_QCP


		// Increment the iteration index
		iterationIndex++;
	}

	if (gradientInStandardNormalSpaceOld) {
		gsl_vector_free(gradientInStandardNormalSpaceOld);
		gradientInStandardNormalSpaceOld = 0;
	}
	if (uOld) {
		gsl_vector_free(uOld);
		uOld = 0;
	}
	
	// Print an error message if max number of iterations was reached (note: in this case the last trial point was never transformed/printed)
	rCritical() << "Error: Maximum number of iterations was reached. Analysis terminated without convergence.";
	return -1;
}

int RStepperNonlinSingleConstrSolver::correctWithFirstPrincipalCurvature()
{
	// SORM with first pricipal curvature

	// Compute curvature according to Der Kiureghian & De Stefano (1992), Eq.26:
	int randomVariablesCount = x->size;
	gsl_vector *du = gsl_vector_calloc(randomVariablesCount);
	gsl_vector_memcpy(du, u);
	gsl_vector_sub(du, uSecondLast);

	// Compute using BLAS: alphaProduct = alphaSecondLast . alpha
	double alphaProduct;
	gsl_blas_ddot(alpha, alphaSecondLast, &alphaProduct);

	// Compute using BLAS: alpha_du = alpha . du
	double alpha_du;
	gsl_blas_ddot(alpha, du, &alpha_du);

	double duNorm = gsl_blas_dnrm2(du);

	// Check sign and compute curvature
	theFirstPrincipalCurvature = GSL_SIGN(alpha_du) * acos(alphaProduct) / sqrt(gsl_pow_2(duNorm) - gsl_pow_2(alpha_du));

	if (theFirstPrincipalCurvature * theDistance < -1) {
		rCritical() << "Warning: The first principal curvature is negative and smaller than the negative inverse of the reliability index. Hence, the failture probability from FORM cannot be corrected with this curvature.";
		return -1;
	}

	QVector<double> curvatureVector;
	curvatureVector << theFirstPrincipalCurvature;
	//rDebug() << "First principal curvature =" << theFirstPrincipalCurvature;



	// Correct the pf of FORM using the fisrtCurvature
	double denominator = gsl_cdf_gaussian_P(-theDistance, 1.0);
	if (denominator == 0.0) {
		rCritical() << "Error: SORM analysis did not converge due to too a large reliability index value.";
		return -1;
	}
	double psi_beta = gsl_ran_gaussian_pdf(theDistance, 1.0) / denominator;

	double product = 1.0;
	for (int i = 0; i < curvatureVector.count(); i++) {
		product /= sqrt(1.0 + psi_beta * curvatureVector[i]);
	}
	double pfSORM = theFailureProbability * product;


	// Assign the corrected values
	theFailureProbability = pfSORM;

	//rDebug() << "pf =" << pfSORM;
	//rDebug() << "SORM Beta =" << betaSORM;
	//rDebug() << " ";

	//rDebug() << "Alpha Vector:";
	//theDomain->print(alpha);
	//rDebug() << " ";
	//rDebug() << "Alpha second last Vector:";
	//theDomain->print(alphaSecondLast);
	//rDebug() << " ";
	//rDebug() << "u Vector:";
	//theDomain->print(u);
	//rDebug() << " ";
	//rDebug() << "u second last Vector:";
	//theDomain->print(uSecondLast);*

	return 0;
}

double RStepperNonlinSingleConstrSolver::getFirstPrincipalCurvature()
{
	return theFirstPrincipalCurvature;
}

int RStepperNonlinSingleConstrSolver::computeRandomVariableSensitivities(gsl_vector *betaSensitivityWrtMeanVector, gsl_vector *betaSensitivityWrtStdvVector, gsl_vector *kappaSensitivityWrtMeanVector, gsl_vector *kappaSensitivityWrtStdvVector)
{
	if (getOutputDisplayLevel() >= RObject::Moderate ) {
		rDebug() << "Computing the random variable sensitivities...";
	}
	
	if (!betaSensitivityWrtMeanVector) {
		return -1;
	}
	if (!betaSensitivityWrtStdvVector) {
		return -1;
	}
	
	int randomVariablesCount = betaSensitivityWrtMeanVector->size;
	gsl_matrix *Jacobian_u_Mean = gsl_matrix_calloc(randomVariablesCount, randomVariablesCount);
	gsl_matrix *Jacobian_u_Stdv = gsl_matrix_calloc(randomVariablesCount, randomVariablesCount);
	theTransformer.data()->computeYSensitivityToDistributionParameters(x, Jacobian_u_Mean, Jacobian_u_Stdv, 0);

	// Using GSL, carry out the betaSensitivityWrtMeanVector = Jacobian_u_Mean * alpha
	int status = gsl_blas_dgemv(CblasNoTrans, 1.0, Jacobian_u_Mean, alpha, 0.0, betaSensitivityWrtMeanVector);
	if (status) { 
		rCritical() << "Error in GSL:" << gsl_strerror(status);
		return -1;
	} 

	// Using GSL, carry out the betaSensitivityWrtStdvVector = Jacobian_u_Stdv * alpha
	status = gsl_blas_dgemv(CblasNoTrans, 1.0, Jacobian_u_Stdv, alpha, 0.0, betaSensitivityWrtStdvVector);
	if (status) { 
		rCritical() << "Error in GSL:" << gsl_strerror(status);
		return -1;
	}

	if (kappaSensitivityWrtMeanVector && kappaSensitivityWrtStdvVector) {
		gsl_vector *du = gsl_vector_calloc(randomVariablesCount);
		gsl_vector_memcpy(du, u);
		gsl_vector_sub(du, uSecondLast);

		// Compute using BLAS: alphaProduct = alphaSecondLast . alpha
		double alphaProduct;
		gsl_blas_ddot(alpha, alphaSecondLast, &alphaProduct);

		// Compute using BLAS: alpha_du = alpha . du
		double alpha_du;
		gsl_blas_ddot(alpha, du, &alpha_du);

		double duNorm = gsl_blas_dnrm2(du);
		double kappa = theFirstPrincipalCurvature;
		
		
		
		// New difefrentiation scheme
		double uNorm = gsl_blas_dnrm2(u);
		gsl_vector *uNext = gsl_vector_calloc(randomVariablesCount);
		gsl_vector_memcpy(uNext, alpha);
		gsl_vector_scale(uNext, theDistance);

		double uNextNorm = gsl_blas_dnrm2(uNext);
		double kappaDenominator = sqrt((gsl_pow_2(duNorm) - gsl_pow_2(alpha_du)));
		double sqrtOneMinusAlphaProductSquare = sqrt(1 - gsl_pow_2(alphaProduct));

		gsl_vector *du_Minus_Alpha_du_Times_Alpha = gsl_vector_calloc(randomVariablesCount);
		gsl_vector_memcpy(du_Minus_Alpha_du_Times_Alpha, alpha);
		gsl_vector_scale(du_Minus_Alpha_du_Times_Alpha, -alpha_du);
		gsl_vector_add(du_Minus_Alpha_du_Times_Alpha, du);

		gsl_vector *tempVector = gsl_vector_calloc(randomVariablesCount);



		
		gsl_vector *dKappadu = gsl_vector_calloc(randomVariablesCount);
		gsl_vector_memcpy(dKappadu, alphaSecondLast);
		gsl_vector_scale(dKappadu, -alphaProduct);
		gsl_vector_add(dKappadu, alpha);
		gsl_vector_scale(dKappadu, -1.0 / (uNorm * sqrtOneMinusAlphaProductSquare * kappaDenominator));
		gsl_vector_memcpy(tempVector, du_Minus_Alpha_du_Times_Alpha);
		gsl_vector_scale(tempVector, kappa / gsl_pow_2(kappaDenominator));
		gsl_vector_sub(dKappadu, tempVector);


		gsl_vector *dKappaduSecondLast = gsl_vector_calloc(randomVariablesCount);
		gsl_vector_memcpy(dKappaduSecondLast, tempVector);

		gsl_vector *dKappaduNext = gsl_vector_calloc(randomVariablesCount);
		gsl_vector_memcpy(dKappaduNext, alpha);
		gsl_vector_scale(dKappaduNext, -alphaProduct);
		gsl_vector_add(dKappaduNext, alphaSecondLast);
		gsl_vector_scale(dKappaduNext, -1.0 / (uNextNorm * sqrtOneMinusAlphaProductSquare * kappaDenominator));
		gsl_vector_scale(tempVector, alpha_du / uNextNorm);
		gsl_vector_add(dKappaduNext, tempVector);



		gsl_matrix *Jacobian_uSecondLast_Mean = gsl_matrix_calloc(randomVariablesCount, randomVariablesCount);
		gsl_matrix *Jacobian_uSecondLast_Stdv = gsl_matrix_calloc(randomVariablesCount, randomVariablesCount);
		gsl_vector *xSecondLast = gsl_vector_calloc(randomVariablesCount);
		theTransformer.data()->transformPointToOriginalSpace(uSecondLast, xSecondLast,0);
		theTransformer.data()->computeYSensitivityToDistributionParameters(xSecondLast, Jacobian_uSecondLast_Mean, Jacobian_uSecondLast_Stdv, 0);

		gsl_matrix *Jacobian_uNext_Mean = gsl_matrix_calloc(randomVariablesCount, randomVariablesCount);
		gsl_matrix *Jacobian_uNext_Stdv = gsl_matrix_calloc(randomVariablesCount, randomVariablesCount);
		gsl_vector *xNext = gsl_vector_calloc(randomVariablesCount);
		theTransformer.data()->transformPointToOriginalSpace(uNext, xNext,0);
		theTransformer.data()->computeYSensitivityToDistributionParameters(xNext, Jacobian_uNext_Mean, Jacobian_uNext_Stdv, 0);


		status = gsl_blas_dgemv(CblasNoTrans, 1.0, Jacobian_u_Mean, dKappadu, 0.0, kappaSensitivityWrtMeanVector);
		if (status) { 
			rCritical() << "Error in GSL:" << gsl_strerror(status);
			return -1;
		} 

		status = gsl_blas_dgemv(CblasNoTrans, 1.0, Jacobian_u_Stdv, dKappadu, 0.0, kappaSensitivityWrtStdvVector);
		if (status) { 
			rCritical() << "Error in GSL:" << gsl_strerror(status);
			return -1;
		}

		status = gsl_blas_dgemv(CblasNoTrans, 1.0, Jacobian_uSecondLast_Mean, dKappaduSecondLast, 1.0, kappaSensitivityWrtMeanVector);
		if (status) { 
			rCritical() << "Error in GSL:" << gsl_strerror(status);
			return -1;
		} 

		status = gsl_blas_dgemv(CblasNoTrans, 1.0, Jacobian_uSecondLast_Stdv, dKappaduSecondLast, 1.0, kappaSensitivityWrtStdvVector);
		if (status) { 
			rCritical() << "Error in GSL:" << gsl_strerror(status);
			return -1;
		}

		status = gsl_blas_dgemv(CblasNoTrans, 1.0, Jacobian_uNext_Mean, dKappaduNext, 1.0, kappaSensitivityWrtMeanVector);
		if (status) { 
			rCritical() << "Error in GSL:" << gsl_strerror(status);
			return -1;
		} 

		status = gsl_blas_dgemv(CblasNoTrans, 1.0, Jacobian_uNext_Stdv, dKappaduNext, 1.0, kappaSensitivityWrtStdvVector);
		if (status) { 
			rCritical() << "Error in GSL:" << gsl_strerror(status);
			return -1;
		}




		/* //Implementation assuming that kappa only depends on u through u*
		gsl_vector *du = gsl_vector_calloc(randomVariablesCount);
		gsl_vector_memcpy(du, u);
		gsl_vector_sub(du, uSecondLast);

		// Compute using BLAS: alphaProduct = alphaSecondLast . alpha
		double alphaProduct;
		gsl_blas_ddot(alpha, alphaSecondLast, &alphaProduct);

		// Compute using BLAS: alpha_du = alpha . du
		double alpha_du;
		gsl_blas_ddot(alpha, du, &alpha_du);

		double duNorm = gsl_blas_dnrm2(du);
		double kappa = theFirstPrincipalCurvature;

		// Compute dKappa_du = -kappa / (|du|^2 - |alpha_du|^2) * (du - alpha_du * alpha)
		gsl_vector *dKappa_du = gsl_vector_calloc(randomVariablesCount);
		gsl_vector_memcpy(dKappa_du, alpha);
		gsl_vector_scale(dKappa_du, -alpha_du);
		gsl_vector_add(dKappa_du, du);
		gsl_vector_scale(dKappa_du, -kappa / (gsl_pow_2(duNorm) - gsl_pow_2(alpha_du)));

		gsl_matrix *Jacobian_uSecondLast_Mean = gsl_matrix_calloc(randomVariablesCount, randomVariablesCount);
		gsl_matrix *Jacobian_uSecondLast_Stdv = gsl_matrix_calloc(randomVariablesCount, randomVariablesCount);
		gsl_vector *xSecondLast = gsl_vector_calloc(randomVariablesCount);
		theTransformer->transformPointToOriginalSpace(uSecondLast, xSecondLast);
		theTransformer->computeYSensitivityToDistributionParameters(xSecondLast, Jacobian_uSecondLast_Mean, Jacobian_uSecondLast_Stdv);

		// Compute Jacobian_u_Mean = Jacobian_u_Mean - Jacobian_uSecondLast_Mean   and    Jacobian_u_Stdv = Jacobian_u_Stdv - Jacobian_uSecondLast_Stdv
		gsl_matrix_sub(Jacobian_u_Mean, Jacobian_uSecondLast_Mean);
		gsl_matrix_sub(Jacobian_u_Stdv, Jacobian_uSecondLast_Stdv);

		// Using GSL, carry out the kappaSensitivityWrtMeanVector = Jacobian_u_Mean * dKappa_du
		status = gsl_blas_dgemv(CblasNoTrans, 1.0, Jacobian_u_Mean, dKappa_du, 0.0, kappaSensitivityWrtMeanVector);
		if (status) { 
			rCritical() << "Error in GSL:" << gsl_strerror(status);
			return -1;
		} 

		// Using GSL, carry out the kappaSensitivityWrtStdvVector = Jacobian_u_Stdv * dKappa_du
		status = gsl_blas_dgemv(CblasNoTrans, 1.0, Jacobian_u_Stdv, dKappa_du, 0.0, kappaSensitivityWrtStdvVector);
		if (status) { 
			rCritical() << "Error in GSL:" << gsl_strerror(status);
			return -1;
		}
		*/





		if (du) {
			gsl_vector_free(du);
			du = 0;
		}
		if (dKappadu) {
			gsl_vector_free(dKappadu);
			dKappadu = 0;
		}
		if (dKappaduSecondLast) {
			gsl_vector_free(dKappaduSecondLast);
			dKappaduSecondLast = 0;
		}
		if (dKappaduNext) {
			gsl_vector_free(dKappaduNext);
			dKappaduNext = 0;
		}
		if (xSecondLast) {
			gsl_vector_free(xSecondLast);
			xSecondLast = 0;
		}
		if (Jacobian_uSecondLast_Mean) {
			gsl_matrix_free(Jacobian_uSecondLast_Mean);
			Jacobian_uSecondLast_Mean = 0;
		}
		if (Jacobian_uSecondLast_Stdv) {
			gsl_matrix_free(Jacobian_uSecondLast_Stdv);
			Jacobian_uSecondLast_Stdv = 0;
		}
		if (uNext) {
			gsl_vector_free(uNext);
			uNext = 0;
		}
		if (xNext) {
			gsl_vector_free(xNext);
			xNext = 0;
		}
		if (Jacobian_uNext_Mean) {
			gsl_matrix_free(Jacobian_uNext_Mean);
			Jacobian_uNext_Mean = 0;
		}
		if (Jacobian_uNext_Stdv) {
			gsl_matrix_free(Jacobian_uNext_Stdv);
			Jacobian_uNext_Stdv = 0;
		}
		if (tempVector) {
			gsl_vector_free(tempVector);
			tempVector = 0;
		}
		if (du_Minus_Alpha_du_Times_Alpha) {
			gsl_vector_free(du_Minus_Alpha_du_Times_Alpha);
			du_Minus_Alpha_du_Times_Alpha = 0;
		}
	}

	if (getOutputDisplayLevel() >= RObject::Moderate ) {
		rDebug() << "Random variable sensitivities are computed successfully.";
	}


	if (Jacobian_u_Mean) {
		gsl_matrix_free(Jacobian_u_Mean);
		Jacobian_u_Mean = 0;
	}
	if (Jacobian_u_Stdv) {
		gsl_matrix_free(Jacobian_u_Stdv);
		Jacobian_u_Stdv = 0;
	}

	return 0;
}

gsl_vector * RStepperNonlinSingleConstrSolver::get_x() const
{
	return x;
}

gsl_vector * RStepperNonlinSingleConstrSolver::get_u() const
{
	return u;
}

gsl_vector * RStepperNonlinSingleConstrSolver::get_alpha() const
{
	return alpha;
}

gsl_vector * RStepperNonlinSingleConstrSolver::get_gamma() const
{
	return gamma;
}

int RStepperNonlinSingleConstrSolver::getStepsCount() const
{
	return iterationIndex-1;
}

gsl_vector * RStepperNonlinSingleConstrSolver::getSecondLast_u() const
{
	return uSecondLast;
}

gsl_vector * RStepperNonlinSingleConstrSolver::getSecondLast_alpha() const
{
	return alphaSecondLast;
}

gsl_vector * RStepperNonlinSingleConstrSolver::getLastStepDirection() const
{
	return stepDirection;
}

double RStepperNonlinSingleConstrSolver::getFirstFunctionValue() const
{
	return functionValueFirst;
}

double RStepperNonlinSingleConstrSolver::getLastFunctionValue() const
{
	return functionValueLast;
}

gsl_vector * RStepperNonlinSingleConstrSolver::getGradientInStandardNormalSpace() const
{
	return gradientInStandardNormalSpace;
}

double RStepperNonlinSingleConstrSolver::getDistance() const
{
	return theDistance;
}

double RStepperNonlinSingleConstrSolver::getFailureProbability()
{
	return theFailureProbability;
}

bool RStepperNonlinSingleConstrSolver::canBeRunParallelly() {	
	return false;
}