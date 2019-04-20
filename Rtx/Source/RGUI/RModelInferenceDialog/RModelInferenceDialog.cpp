// Modified by Terje on November 25, 2011 (add R and fix sigma_sigma)


#include "RModelInferenceDialog.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QTime>
#include <QtDebug>
#include <QString>
#include <QStringList>

#include "RMainWindow.h"
#include "RDomain.h"
#include "RContinuousRandomVariable.h"
#include "RConstant.h"
#include "RPlotWidget.h"
#include "RAlgebraicExpressionModel.h"
#include "RRandomVariableWithRandomParametersModel.h"



#include <gsl/gsl_blas.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_sort_vector.h>
#include <gsl/gsl_cdf.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_math.h>

#include <math.h>

RModelInferenceDialog::RModelInferenceDialog(QWidget *parent, RDomain *passedDomain)
				: QDialog(parent)
{
		setupUi(this);

		theDomain = passedDomain;
		y = 0;
		X = 0;

		power = 3.0;//Mojtaba

		connect(okButton, SIGNAL(clicked()), this, SLOT(onOkButtonClicked()));
		connect(browseButton, SIGNAL(clicked()), this, SLOT(browse()));
		connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
}

RModelInferenceDialog::~RModelInferenceDialog()
{
		deleteVectors();
}

void RModelInferenceDialog::browse()
{
		// Get the file name from the user using QFileDialog
		QString fileName = QFileDialog::getOpenFileName(this, tr("Open Input file"), QDir::currentPath(), "All Files (*.*)");
		if (fileName.isEmpty()) {
				return;
		}
		lineEdit->setText(fileName);
}

void RModelInferenceDialog::setDomain(RDomain *passedDomain)
{
		theDomain = passedDomain;
}

void RModelInferenceDialog::onOkButtonClicked()
{	
		QString filePath = lineEdit->text();
		filePath.remove('"');

		if (filePath.isEmpty()) {
				QMessageBox::critical(this, tr("Error"), tr("Please choose enter a file name and try again."), QMessageBox::Ok | QMessageBox::Default);
				return;
		}
		RMainWindow *mainWindow = qobject_cast<RMainWindow *>(this->parent());

		accept();

		mainWindow->raiseDock("Output");


		// Read the file here
		QFile file(filePath);
		if (!file.open(QFile::ReadOnly)) {
				// Error
				RObject::rCritical() << "Error: Could not open the file" << filePath << ".";
				return;
		}

		// Read the file content into a QString
		QTextStream stream(&file);
		stream.seek(0);
		//QString fileContentString = in.readAll();

		// Tokenize the QString of file contents by "\n" and create a string list of input file lines
		QString verticalRegExpString = QString("[n\n\v\r%1%2]").arg(QChar(QChar::ParagraphSeparator)).arg(QChar(QChar::LineSeparator));
		QRegExp verticalRegExp = QRegExp(verticalRegExpString);
		//QStringList fileContentStringList = fileContentString.split(verticalRegExp, QString::SkipEmptyParts);

		QStringList fileContentStringList;
		QString line;
		QStringList lineList;
		stream.setAutoDetectUnicode(true);
		do {
				line = stream.readLine();
				if (!line.isEmpty()) {
						lineList = line.split(verticalRegExp, QString::SkipEmptyParts);
						fileContentStringList.append(lineList);
				}
				//QCoreApplication::processEvents();
		} while (!line.isNull());

		int size = fileContentStringList.count();

		QRegExp regExp = QRegExp("[ ,;\t]");
		int columnCount = 0;

		for (int i = 0; i < fileContentStringList.count(); i++) {
				QStringList valueList = fileContentStringList[i].split(regExp, QString::SkipEmptyParts);

				if (i == 0) {
						columnCount = valueList.count();
						if (columnCount < 2) {
								RObject::rCritical() << "Error: The number of columns should be more than 1 in the database file.";
								deleteVectors();
								return;
						}

						X = gsl_matrix_calloc(size, columnCount - 1);
						y = gsl_vector_calloc(size);
				} else {
						if (columnCount != valueList.count()) {
								RObject::rCritical() << "Error: The number of data at the line" << i + 1 << "is different than the line 1 in the database file.";
								deleteVectors();
								return;
						}
				}

				for (int j = 0; j < valueList.count(); j++) {
						QString tempStr = valueList[j].trimmed();
						bool ok;
						double tempValue = tempStr.toDouble(&ok);
						if (!ok) {
								RObject::rCritical() << "Error: Could not read the value number" << j + 1 << "at the line" << i + 1 << "in the database file.";
								deleteVectors();
								return;
						}

						// The first value in the line is the output

						if (j == 0) {
								gsl_vector_set(y, i, tempValue);

								// The from 1 to "valueList.count()" values in the line
						} else if (j > 0) {
								gsl_matrix_set(X, i, j - 1, tempValue);
						}
				}
		}
		file.close();



		/*RObject::rDebug() << (X->size1) << "data points were read to obtain the following y-vector and X-matrix:";
		theDomain->print(y);
		RObject::rDebug() << "";
		theDomain->print(X);
		RObject::rDebug() << "";
		*/


		// Carry out the inference analysis
		int result = this->analyze();
		if (result < 0) {
				RObject::rCritical() << "Error: The variable inference analysis did not converge because of errors.";
				return;
		}
		mainWindow->updateTree("Domain");
}

int RModelInferenceDialog::analyze()
{
		// Inform the user that the sampling analysis has started
		RObject::rDebug() << "##### STARTING VARIABLE INFERENCE ANALYSIS ...";
		RObject::rDebug() << "";

		QTime time;
		time.start();


		// PLAY AROUND WITH NEW DATA
		//playAroundWithNewData();

		if (!X || !y) {
			qCritical() << "Error: The matrix of regressors and/or the vector of regressand have not been read from the text file successfully. Please revise the file and try again.";
			return -1;
		}


		// SET CLASS VARIABLES
		n = X->size1;
		k = X->size2;


		// CARRY OUT THE INFERENCE ANALYSIS
		gsl_vector *mean_theta = gsl_vector_calloc(k);
		gsl_vector *cov_theta = gsl_vector_calloc(k);
		gsl_matrix *correlationMatrix = gsl_matrix_calloc(k,k);
		double mean_sigma = 0.0;
		double cov_sigma = 0.0;
		double R = 0.0;
		carryOutInference(mean_theta, cov_theta, correlationMatrix, mean_sigma, cov_sigma, R);


		// PRINT RESULTS TO THE OUTPUT PANE
		RObject::rDebug() << "Mean of the model parameters:";
		theDomain->print(mean_theta, 0);
		RObject::rDebug() << "";
		RObject::rDebug() << "Coefficient of variation (in percent) of the model parameters:";
		theDomain->print(cov_theta, 0);
		RObject::rDebug() << "";
		RObject::rDebug() << "Correlation matrix of the model parameters:";
		theDomain->print(correlationMatrix, 0);
		RObject::rDebug() << "";
		RObject::rDebug() << "R-factor:" << R;
		RObject::rDebug() << "Mean of sigma:" << mean_sigma;
		RObject::rDebug() << "Coefficient of variation (in percent) of sigma:" << cov_sigma; //Mojtaba
		RObject::rDebug() << "";


		// PLOT RESIDUALS VERSUS REGRESSORS
		if (ResiudalPlotCheckBox->isChecked()) {
				createResidualPlot(mean_theta);
		}

		QCoreApplication::processEvents(); //Mojtaba


		// PLOT MODEL PREDICTIONS VERSUS THE OBSERVED DATA
		if (ModelPredictionPlotCheckBox->isChecked()) {
				QString plotTitle = "Model Predictions Vs. Observations";
				createModelPredictionPlot(mean_theta, plotTitle);
		}

		QCoreApplication::processEvents(); //Mojtaba


		// PLOT THE RATIO OF THE MODEL PREDICTION AND OBSERVATIONS, AS A FUNCTION OF THE OBSERVED VALUE
		if (ModelObsRatioPlotCheckBox->isChecked()) {
				createModelObsRatioPlot(mean_theta);
		}

		QCoreApplication::processEvents(); //Mojtaba


		// DISPLAY A NORMALITY PLOT
		if (NormalityPlotCheckBox->isChecked()) {
				createNormalityPlot(mean_theta, mean_sigma);

		}

		QCoreApplication::processEvents(); //Mojtaba


		// SEARCH FOR EXPLANATORY FUNCTIONS (Perhaps best do this OUTSIDE Rt?)
		if (SearchCheckBox->isChecked()) {
				searchForExplanatoryFunctions(mean_theta, cov_theta);

		}

		QCoreApplication::processEvents(); //Mojtaba


		// CREATING RANDOM VARIABLES AND THE MODEL
		if (CreateRVCheckBox->isChecked()) {
				createRandomVariablesAndModel(mean_theta, cov_theta, mean_sigma, cov_sigma);
		}


		RObject::rDebug() << "##### MODEL INFERENCE ANALYSIS DONE IN" << (double)(1000.0*time.elapsed())/1000000.0 << "SECONDS.";
		RObject::rDebug() << "";
		RObject::rDebug() << "";

		return 0;
}

void RModelInferenceDialog::deleteVectors()
{
		if (y) {
				gsl_vector_free(y);
				y = 0;
		}
		if (X) {
				gsl_matrix_free(X);
				X = 0;
		}
}

void RModelInferenceDialog::playAroundWithNewData()
{
		// Erase the read data, just to play around with data generated here...
		gsl_vector_free(y);
		gsl_matrix_free(X);


		// Create a random number generator
		const gsl_rng_type * T;
		gsl_rng * r;
		gsl_rng_env_setup();
		T = gsl_rng_default;
		r = gsl_rng_alloc(T);


		// Generate the outcome of some regressors, P, L, E, I (Units!  N, mm)
		int n=30;
		int k=4;
		y = gsl_vector_calloc(n);
		X = gsl_matrix_calloc(n,k);
		double x2, x3, x4, Response;
		for (int i=0; i<n; i++) {

				gsl_matrix_set(X,i,0,1.0);

				x2 = gsl_ran_flat (r, 100.0,     1000.0);
				gsl_matrix_set(X,i,1,x2);

				x3 = gsl_ran_flat (r, 1000.0,     3000.0);
				gsl_matrix_set(X,i,2,x3);

				x4 = gsl_ran_flat (r, 2.0,   10.0);
				gsl_matrix_set(X,i,3,x4);

				Response = x3/x4/1000.0 + gsl_pow_2(x2/x4/200.0);
				gsl_vector_set(y,i,Response);
		}




		/*
		// Generate the outcome of some regressors, P, L, E, I (Units!  N, mm)
		int n=30;
		int k=5;
		y = gsl_vector_calloc(n);
		X = gsl_matrix_calloc(n,k);
		double P, L, E, I, Drift;
		for (int i=0; i<n; i++) {

		gsl_matrix_set(X,i,0,1.0);

		P = gsl_ran_flat (r, 5000.0,     100000.0);
		gsl_matrix_set(X,i,1,P);

		L = gsl_ran_flat (r, 1000.0,     10000.0);
		gsl_matrix_set(X,i,2,L);

		E = gsl_ran_flat (r, 150000.0,   300000.0);
		gsl_matrix_set(X,i,3,E);

		I = gsl_ran_flat (r, 10000000.0, 500000000.0);
		gsl_matrix_set(X,i,4,I);

		Drift = P*L*L/(3.0*E*I);
		gsl_vector_set(y,i,Drift);
		}
		*/

		/*
		// Generate the outcome of some regressors, P, L, E, I (Units!  N, mm)
		int n=50;
		int k=12;
		y = gsl_vector_calloc(n);
		X = gsl_matrix_calloc(n,k);
		double P, L, E, I, x2, x3, x4, x5, x6, x7, x8, Response;
		for (int i=0; i<n; i++) {

		gsl_matrix_set(X,i,0,1.0);

		P = gsl_ran_flat (r, 5000.0,     100000.0);
		gsl_matrix_set(X,i,1,P);

		L = gsl_ran_flat (r, 1000.0,     10000.0);
		gsl_matrix_set(X,i,2,L);

		E = gsl_ran_flat (r, 150000.0,   300000.0);
		gsl_matrix_set(X,i,3,E);

		I = gsl_ran_flat (r, 10000000.0, 500000000.0);
		gsl_matrix_set(X,i,4,I);

		x2 = 1.0/E;
		gsl_matrix_set(X,i,5,x2);

		x3 = 1.0/I;
		gsl_matrix_set(X,i,6,x3);

		x4 = L*L*L;
		gsl_matrix_set(X,i,7,x4);

		x5 = P/E;
		gsl_matrix_set(X,i,8,x5);

		x6 = L*L*L/I;
		gsl_matrix_set(X,i,9,x6);

		x7 = L*L*L/(E*I);
		gsl_matrix_set(X,i,10,x7);

		x8 = P*L*L*L/(E*I);
		gsl_matrix_set(X,i,11,x8);

		Response = P*L*L*L/(3.0*E*I);
		gsl_vector_set(y,i,Response);
		}

		*/

		/*
		// Generate the outcome of some regressors and some model parameters (this is the CIVL 518 assigmnent)
		int n=50;
		int k=5;
		y = gsl_vector_calloc(n);
		X = gsl_matrix_calloc(n,k);
		double x2, x3, x4, x5, theta1, theta2, theta3, theta4, theta5, mu, sigma, Response;
		for (int i=0; i<n; i++) {

		gsl_matrix_set(X,i,0,1.0);

		x2 = gsl_ran_flat (r, 0.0,  4.0);
		gsl_matrix_set(X,i,1,x2);

		x3 = gsl_ran_flat (r, 0.0,  5.0);
		gsl_matrix_set(X,i,2,x3);

		x4 = gsl_ran_flat (r, 0.0,   2.0);
		gsl_matrix_set(X,i,3,x4);

		x5 = gsl_ran_flat (r, 0.0,   5.0);
		gsl_matrix_set(X,i,4,x5);

		mu = 2.0;
		sigma = 0.05*mu;
		theta1 = mu + gsl_ran_gaussian(r,sigma);

		mu = 1.0;
		sigma = 0.05*mu;
		theta2 = mu + gsl_ran_gaussian(r,sigma);

		mu = 0.5;
		sigma = 0.05*mu;
		theta3 = mu + gsl_ran_gaussian(r,sigma);

		mu = 2.0;
		sigma = 0.05*mu;
		theta4 = mu + gsl_ran_gaussian(r,sigma);

		mu = 0.3;
		sigma = 0.05*mu;
		theta5 = mu + gsl_ran_gaussian(r,sigma);

		Response = theta1 + theta2*x2 + theta3*x3 + theta4*x4 - theta5*x5;
		gsl_vector_set(y,i,Response);
		}
		*/


		/*
		// Generate the outcome of some misbehaved data
		int n=50;
		int k=5;
		y = gsl_vector_calloc(n);
		X = gsl_matrix_calloc(n,k);
		double x2, x3, x4, x5, theta1, theta2, theta3, theta4, theta5, mu, sigma, Response;
		for (int i=0; i<n; i++) {

		gsl_matrix_set(X,i,0,1.0);

		x2 = gsl_ran_flat (r, 0.0,  4.0);
		gsl_matrix_set(X,i,1,x2);

		x3 = gsl_ran_flat (r, 0.0,  5.0);
		gsl_matrix_set(X,i,2,x3);

		x4 = gsl_ran_flat (r, 0.0,   2.0);
		gsl_matrix_set(X,i,3,x4);

		x5 = gsl_ran_flat (r, 0.0,   5.0);
		gsl_matrix_set(X,i,4,x5);

		mu = 2.0;
		sigma = 0.05*mu;
		theta1 = mu + gsl_ran_gaussian(r,sigma);

		mu = 1.0;
		sigma = 0.05*mu;
		theta2 = mu + gsl_ran_gaussian(r,sigma);

		mu = 0.5;
		sigma = 0.05*mu;
		theta3 = mu + gsl_ran_gaussian(r,sigma);

		mu = 2.0;
		sigma = 0.05*mu;
		theta4 = mu + gsl_ran_gaussian(r,sigma);

		theta5 = mu + gsl_ran_flat(r,0.5, 5.0);

		Response = theta1 + theta2*x2 + theta3*x3 + theta4*x4 - theta5*x5;
		gsl_vector_set(y,i,Response);
		}
		*/





		// Print the new data to screen
		gsl_matrix *tempYX = gsl_matrix_calloc(n, k+1);
		for (int i=0; i<n; i++) {
				gsl_matrix_set(tempYX, i, 0, gsl_vector_get(y,i));
				for (int j=0; j<k; j++) {
						gsl_matrix_set(tempYX, i, j+1, gsl_matrix_get(X,i,j));
				}
		}
		RObject::rDebug() << "The data that is being played around with...:";
		theDomain->print(tempYX, 0);
		RObject::rDebug() << "";
		gsl_matrix_free(tempYX);



}

int RModelInferenceDialog::carryOutInference(gsl_vector *mean_theta, gsl_vector *cov_theta, gsl_matrix *correlationMatrix, double &mean_sigma, double &cov_sigma, double &R)
{
		// Get number of regressors and observations
		int nu = n-k;

		// Give warning if there is little data
		if (nu<=4) {
				RObject::rCritical() << "You've got too few observations (data) compared with the number of regressors!";
		}
		else if (n<2*k) {
				RObject::rDebug() << "You've got quite few observations (data) compared with the number of regressors.";
		}

		// Compute the Ordinary Least Squares estimate; the mean of the thetas
		gsl_matrix *XX = gsl_matrix_calloc(k,k);
		gsl_blas_dgemm(CblasTrans, CblasNoTrans, 1.0, X, X, 0.0, XX);

		gsl_matrix *XXcopy = gsl_matrix_calloc(k, k);
		gsl_matrix_memcpy(XXcopy, XX);
		gsl_matrix *XXinv = gsl_matrix_calloc(k, k);
		int signum = 0;
		gsl_permutation *p = gsl_permutation_alloc(k);
		gsl_linalg_LU_decomp(XXcopy, p, &signum);

		double determinant = gsl_linalg_LU_det(XXcopy,signum);
		if (determinant==0.0) {
				RObject::rCritical() << "Error: Could not invert X*X^T in model inference analysis.";
		}

		gsl_linalg_LU_invert(XXcopy, p, XXinv);
		gsl_permutation_free(p);


		// Warn about collinearity
		for (int i=0; i<(XXinv->size1); i++) {
				if (  gsl_matrix_get(XXinv,i,i)<0.0  ) {
						RObject::rCritical() << "You've got problems with collinearity in your data!";
				}
		}


		gsl_matrix *XXinvX = gsl_matrix_calloc(k,n);
		gsl_blas_dgemm(CblasNoTrans, CblasTrans, 1.0, XXinv, X, 0.0, XXinvX);

		gsl_blas_dgemv(CblasNoTrans, 1.0, XXinvX, y, 0.0, mean_theta);


		// Compute the R-factor, namely the correlation between observed and predicted results
		double modelPrediction, observation;
		double sumPred = 0.0;
		double sumPredSquared = 0.0;
		double sumObs = 0.0;
		double sumObsSquared = 0.0;
		double sumObsPred = 0.0;

		for (int i=0; i<n; i++) {

				observation = gsl_vector_get(y,i);

				modelPrediction = 0.0;

				for (int j=0; j<k; j++) {
						modelPrediction += gsl_vector_get(mean_theta,j)*gsl_matrix_get(X,i,j);
				}

				sumPred += modelPrediction;
				sumPredSquared += modelPrediction*modelPrediction;

				sumObs += observation;
				sumObsSquared += observation*observation;

				sumObsPred += modelPrediction*observation;
		}

		double predictionMean = 1.0/((double)n) * sumPred;
		double predictionVariance = 1.0/((double)(n-1)) * (sumPredSquared - n*predictionMean*predictionMean);
		double observationMean = 1.0/((double)n) * sumObs;
		double observationVariance = 1.0/((double)(n-1)) * (sumObsSquared - n*observationMean*observationMean);
		R = 1.0/((double)(n-1)) * (sumObsPred - n*predictionMean*observationMean) / (sqrt(predictionVariance) * sqrt(observationVariance));


		// Compute X*theta
		gsl_vector *Xtheta = gsl_vector_calloc(n);
		gsl_blas_dgemv(CblasNoTrans, 1.0, X, mean_theta, 0.0, Xtheta);


		// Compute the Ordinary Least Squares error variance
		gsl_vector *y_minus_Xtheta = gsl_vector_calloc(n);
		gsl_vector_memcpy(y_minus_Xtheta, y);
		gsl_vector_sub(y_minus_Xtheta, Xtheta);
		double dot_product_y_minus_Xtheta;
		gsl_blas_ddot (y_minus_Xtheta, y_minus_Xtheta, &dot_product_y_minus_Xtheta);
		double s_squared = dot_product_y_minus_Xtheta/nu;


		// Compute the covariance matrix for the model parameters
		gsl_matrix *covarianceMatrix = gsl_matrix_calloc(k,k);
		gsl_matrix_memcpy(covarianceMatrix, XXinv);
		double multiplicationFactor;
		if (NormalDistributionRadioButton->isChecked()) {
			multiplicationFactor = s_squared;
		}
		else if (TDistributionRadioButton->isChecked()) {
			multiplicationFactor = nu*s_squared / (nu - 2.0);
		}
		gsl_matrix_scale(covarianceMatrix, multiplicationFactor);
		//for (int i=0; i<k; i++) {
		//	for (int j=0; j<k; j++) {
		//		gsl_matrix_set(covarianceMatrix,i,j,   (multiplicationFactor*gsl_matrix_get(covarianceMatrix,i,j)));
		//	}
		//}



		// Collect the standard deviations from the covariance matrix
		gsl_vector *stdv_theta = gsl_vector_calloc(k);
		for (int i=0; i<k; i++) {
				gsl_vector_set(stdv_theta, i, sqrt(gsl_matrix_get(covarianceMatrix,i,i)) );
		}


		// Collect the coefficient of variation in a vector (in percent!)
		for (int i=0; i<k; i++) {
				gsl_vector_set(cov_theta, i, qAbs(100.0*gsl_vector_get(stdv_theta,i)/gsl_vector_get(mean_theta,i)) );
		}


		// Place 1/stdvs on the diagonal of the D^-1 matrix and extract the correlation matrix
		gsl_matrix *Dinv = gsl_matrix_calloc(k,k);
		for (int i=0; i<k; i++) {
				gsl_matrix_set(Dinv, i, i, 1.0/gsl_vector_get(stdv_theta,i));
		}
		gsl_matrix *DinvSigma = gsl_matrix_calloc(k,k);
		gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, Dinv, covarianceMatrix, 0.0, DinvSigma);
		gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, DinvSigma, Dinv, 0.0, correlationMatrix);


		// Compute statistics for the epsilon variable
		double mean_sigma_squared;
		double variance_sigma;
		if (NormalDistributionRadioButton->isChecked()) {
			mean_sigma_squared = s_squared;
			variance_sigma = s_squared / (2.0*(nu - 4.0));
		}
		else if (TDistributionRadioButton->isChecked()) {
			mean_sigma_squared = s_squared * nu / (nu - 2);
			double variance_sigma_squared = nu*nu*s_squared*s_squared*(2.0 / ((nu - 2.0)*(nu - 2.0)*(nu - 4.0)));
			variance_sigma = variance_sigma_squared / (4.0*mean_sigma_squared);
		}

		


		double stdv_sigma = sqrt(variance_sigma);
		mean_sigma = sqrt(mean_sigma_squared);
		cov_sigma = qAbs(stdv_sigma / mean_sigma * 100.0);


		// Free vectors and matrices
		gsl_matrix_free(XX);
		gsl_matrix_free(XXcopy);
		gsl_matrix_free(XXinv);
		gsl_matrix_free(XXinvX);

		gsl_vector_free(Xtheta);
		gsl_vector_free(y_minus_Xtheta);
		gsl_vector_free(stdv_theta);

		return 0;
}

void RModelInferenceDialog::createResidualPlot(gsl_vector *mean_theta)
{
	#ifndef NO_QCP

	RMainWindow *mainWindow = qobject_cast<RMainWindow *>(parent());

	double epsilon, regressorValue, xMin, xMax, epsMax;
	QVector<double> regressorV, epsilonV;
	for (int i = 0; i < k+1; i++) {

		QCoreApplication::processEvents(); //Mojtaba

		QCustomPlot *thePlot = new RPlotWidget(mainWindow);

		QString str;
		if (i < k) {
		 str = QString("x%1").arg(i + 1);
		}
		else {
			str = QString("y");
		}
		mainWindow->addSubWindow(thePlot, " Residual versus regressor " + str, true);

		QList<QCPAxis*> axisrect;
		axisrect << thePlot->xAxis << thePlot->yAxis;

		thePlot->axisRect()->setRangeZoomAxes(axisrect);
		thePlot->axisRect()->setRangeDragAxes(axisrect);
		thePlot->setInteraction(QCP::iRangeDrag, true); // Enable dragging
		thePlot->setInteraction(QCP::iRangeZoom, true); // Enable zoom by scrolling

		thePlot->xAxis->setLabel(str);
		thePlot->yAxis->setLabel("Residual");

		// Get the value of X*theta, namely the model prediction
		gsl_vector *Xtheta = gsl_vector_calloc(n);
		if (i < k) {
			
			gsl_blas_dgemv(CblasNoTrans, 1.0, X, mean_theta, 0.0, Xtheta);

			// Plot the points

			for (int j = 0; j < n; j++) {
				epsilon = gsl_vector_get(y, j) - gsl_vector_get(Xtheta, j);
				regressorValue = gsl_matrix_get(X, j, i);

				if (j == 0) {
					xMin = regressorValue;
					xMax = regressorValue;
					epsMax = epsilon;
				}
				else {
					if (qAbs(epsilon) > epsMax) {
						epsMax = qAbs(epsilon);
					}
					if (regressorValue > xMax) {
						xMax = regressorValue;
					}
					if (regressorValue < xMin) {
						xMin = regressorValue;
					}
				}

				regressorV << regressorValue;
				epsilonV << epsilon;
			}

		}
		else {
			
			gsl_blas_dgemv(CblasNoTrans, 1.0, X, mean_theta, 0.0, Xtheta);


			// Plot the points

			for (int j = 0; j < n; j++) {
				epsilon = gsl_vector_get(y, j) - gsl_vector_get(Xtheta, j);
				regressorValue = gsl_vector_get(y, j);

				if (j == 0) {
					xMin = regressorValue;
					xMax = regressorValue;
					epsMax = epsilon;
				}
				else {
					if (qAbs(epsilon) > epsMax) {
						epsMax = qAbs(epsilon);
					}
					if (regressorValue > xMax) {
						xMax = regressorValue;
					}
					if (regressorValue < xMin) {
						xMin = regressorValue;
					}
				}

				regressorV << regressorValue;
				epsilonV << epsilon;
			}
		}



		

		thePlot->addGraph();
		thePlot->graph(0)->setLineStyle(QCPGraph::lsNone);
		thePlot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 4));
		thePlot->graph(0)->setData(regressorV, epsilonV);

		if (xMin == xMax) {
			xMin -= 1.0;
			xMax += 1.0;
		}
		thePlot->xAxis->setRange(xMin - 0.1*(xMax - xMin), xMax + 0.1*(xMax - xMin));
		thePlot->yAxis->setRange(-epsMax - 0.1*epsMax, epsMax + 0.1*epsMax);

		// Take away the legend
		thePlot->legend->setVisible(false);

		thePlot->replot();
	}
	#endif // NO_QCP

}

void RModelInferenceDialog::createModelPredictionPlot(gsl_vector *mean_theta, QString plotTitle)
{

#ifndef NO_QCP


	RMainWindow *mainWindow = qobject_cast<RMainWindow *>(parent());

	// Create a plot
	QPointer<RPlotWidget> thePredictionPlot = new RPlotWidget(mainWindow);
	mainWindow->addSubWindow(thePredictionPlot, plotTitle, true);

	QList<QCPAxis*> axisrect;
	axisrect << thePredictionPlot->xAxis << thePredictionPlot->yAxis;
	thePredictionPlot->axisRect()->setRangeZoomAxes(axisrect);
	thePredictionPlot->axisRect()->setRangeDragAxes(axisrect);
	thePredictionPlot->setInteraction(QCP::iRangeDrag, true); // Enable dragging
	thePredictionPlot->setInteraction(QCP::iRangeZoom, true); // Enable zoom by scrolling

	thePredictionPlot->xAxis->setLabel("Observations");
	thePredictionPlot->yAxis->setLabel("Model Predictions");

	thePredictionPlot->addGraph();
	thePredictionPlot->graph(0)->setLineStyle(QCPGraph::lsNone);
	thePredictionPlot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 4));

	
	QCoreApplication::processEvents(); //Mojtaba


	// Plot the points
	QCPGraph *theMarker = thePredictionPlot->graph(0);
	double modelPrediction, observation, minModel, maxModel;
	for (int i = 0; i < n; i++) {

		observation = gsl_vector_get(y, i);
		modelPrediction = 0.0;
		for (int j = 0; j < k; j++) {
			modelPrediction += gsl_vector_get(mean_theta, j)*gsl_matrix_get(X, i, j);
		}

		if (i == 0) {
			if (observation > modelPrediction) {
				minModel = modelPrediction;
				maxModel = observation;
			}
			else {
				minModel = observation;
				maxModel = modelPrediction;
			}
		}

		if (modelPrediction > maxModel) {
			maxModel = modelPrediction;
		}
		if (observation > maxModel) {
			maxModel = observation;
		}
		if (modelPrediction < minModel) {
			minModel = modelPrediction;
		}
		if (observation < minModel) {
			minModel = observation;
		}
	
	
		theMarker->addData(observation, modelPrediction);
	}

	// Create the zero to one curve
	thePredictionPlot->addGraph();
	QCPGraph *curve = thePredictionPlot->graph(1);
	curve->setPen(QPen(Qt::black));
	
	
	QVector<double> theZeroOneVector;
	theZeroOneVector << minModel - 0.1*(maxModel - minModel) << maxModel + 0.1*(maxModel - minModel);
	curve->setData(theZeroOneVector, theZeroOneVector);


	// Take away the legends
	thePredictionPlot->legend->setVisible(false);

	thePredictionPlot->xAxis->rescale(true);
	thePredictionPlot->yAxis->rescale(true);
	thePredictionPlot->xAxis->scaleRange(1.1);
	thePredictionPlot->yAxis->scaleRange(1.1);
	thePredictionPlot->replot();
#endif // NO_QCP



}

void RModelInferenceDialog::createModelObsRatioPlot(gsl_vector *mean_theta)
{

#ifndef NO_QCP
	RMainWindow *mainWindow = qobject_cast<RMainWindow *>(parent());

	// Create a plot
	QPointer<RPlotWidget> theRatioPlot = new RPlotWidget(mainWindow);
	mainWindow->addSubWindow(theRatioPlot, "Prediction/Observation Ratio", true);
	theRatioPlot->xAxis->setLabel("Predicted Values");
	theRatioPlot->yAxis->setLabel("Prediction / Observation");

	QList<QCPAxis*> axisrect;
	axisrect << theRatioPlot->xAxis << theRatioPlot->yAxis;
	theRatioPlot->axisRect()->setRangeZoomAxes(axisrect);
	theRatioPlot->axisRect()->setRangeDragAxes(axisrect);
	theRatioPlot->setInteraction(QCP::iRangeDrag, true); // Enable dragging
	theRatioPlot->setInteraction(QCP::iRangeZoom, true); // Enable zoom by scrolling

	theRatioPlot->addGraph();
	theRatioPlot->graph(0)->setLineStyle(QCPGraph::lsNone);
	theRatioPlot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 4));

	QCoreApplication::processEvents(); //Mojtaba

	// Plot the points
	QCPGraph *theMarker = theRatioPlot->graph(0);
	double modelPrediction, observation, minModel, maxModel, ratio;
	double maxRatio = 0.0;;

	for (int i = 0; i < n; i++) {

		observation = gsl_vector_get(y, i);
		modelPrediction = 0.0;
		for (int j = 0; j < k; j++) {
			modelPrediction += gsl_vector_get(mean_theta, j)*gsl_matrix_get(X, i, j);
		}

		if (i == 0) {
			if (observation > modelPrediction) {
				minModel = modelPrediction;
				maxModel = observation;
			}
			else {
				minModel = observation;
				maxModel = modelPrediction;
			}
		}

		if (modelPrediction > maxModel) {
			maxModel = modelPrediction;
		}
		if (observation > maxModel) {
			maxModel = observation;
		}
		if (modelPrediction < minModel) {
			minModel = modelPrediction;
		}
		if (observation < minModel) {
			minModel = observation;
		}

		if (modelPrediction != 0.0 && observation != 0.0) {
			ratio = modelPrediction / observation;

			if (ratio > maxRatio) {
				maxRatio = ratio;
			}
			theMarker->addData(modelPrediction, ratio);
		}
	}

	// Create flat curve at 1
	theRatioPlot->addGraph();
	QCPGraph *curve = theRatioPlot->graph(1);
	QPen thePen;
	thePen.setColor(QColor(Qt::black));
	curve->setPen(thePen);
	QVector<double> theFlatVector;
	QVector<double> theOneToOneVector;
	theOneToOneVector << 1.0 << 1.0;
	theFlatVector << minModel - 0.1*(maxModel - minModel) << maxModel + 0.1*(maxModel - minModel);
	curve->setData(theFlatVector, theOneToOneVector);

	theRatioPlot->xAxis->setRange(minModel - 0.1*(maxModel - minModel), maxModel + 0.1*(maxModel - minModel));
	theRatioPlot->yAxis->setRange(0.0, maxRatio + 1.0);

	// Take away the legends
	theRatioPlot->legend->setVisible(false);

	theRatioPlot->replot();
#endif // NO_QCP

}

void RModelInferenceDialog::createNormalityPlot(gsl_vector *mean_theta, double mean_sigma)
{

#ifndef NO_QCP

	RMainWindow *mainWindow = qobject_cast<RMainWindow *>(parent());

	// Create a plot
	QPointer<RPlotWidget> theNormalityPlot = new RPlotWidget(mainWindow);
	mainWindow->addSubWindow(theNormalityPlot, "Normality Plot", true);
	theNormalityPlot->xAxis->setLabel("Normal theoretical quantiles");
	theNormalityPlot->yAxis->setLabel("Normal data quantiles");

	QList<QCPAxis*> axisrect;
	axisrect << theNormalityPlot->xAxis << theNormalityPlot->yAxis;
	theNormalityPlot->axisRect()->setRangeZoomAxes(axisrect);
	theNormalityPlot->axisRect()->setRangeDragAxes(axisrect);
	theNormalityPlot->setInteraction(QCP::iRangeDrag, true); // Enable dragging
	theNormalityPlot->setInteraction(QCP::iRangeZoom, true); // Enable zoom by scrolling

	QCoreApplication::processEvents(); //Mojtaba

	// Create a random number generator
	const gsl_rng_type * T;
	gsl_rng * r;
	gsl_rng_env_setup();
	T = gsl_rng_default;
	r = gsl_rng_alloc(T);


	// Generate the outcome of a standard normal random variable
	//gsl_vector *normalEpsilons = gsl_vector_calloc(n);
	//for (int i=0; i<n; i++) {
	//	gsl_vector_set(normalEpsilons,i, gsl_ran_gaussian(r, 3.0));
	//}
	//gsl_sort_vector(normalEpsilons);


	// Compute X*theta, namely the model prediction
	gsl_vector *Xtheta = gsl_vector_calloc(n);
	gsl_blas_dgemv(CblasNoTrans, 1.0, X, mean_theta, 0.0, Xtheta);


	// Store the epsilon values in a vector
	gsl_vector *epsilons = gsl_vector_calloc(n);
	for (int i = 0; i < n; i++) {
		gsl_vector_set(epsilons, i, gsl_vector_get(y, i) - gsl_vector_get(Xtheta, i));
	}
	gsl_sort_vector(epsilons);


	// Plot the points
	double normalEpsilon;
	for (int i = 0; i < n; i++) {

		normalEpsilon = gsl_cdf_gaussian_Pinv(((i + 1) / (double)n), mean_sigma);

		theNormalityPlot->addGraph();
		theNormalityPlot->graph(i)->setLineStyle(QCPGraph::lsNone);
		theNormalityPlot->graph(i)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 4));

		QCPGraph *theMarker = theNormalityPlot->graph(i);

		//theMarker->setValue(gsl_vector_get(normalEpsilons,i), gsl_vector_get(epsilons,i));
		theMarker->addData(normalEpsilon, gsl_vector_get(epsilons, i));
	}


	// Create the zero to one curve
	theNormalityPlot->addGraph();

	QCPGraph *curve = theNormalityPlot->graph(n);
	QPen thePen;
	thePen.setColor(QColor(Qt::black));
	curve->setPen(thePen);
	
	QVector<double> theZeroOneVector;
	double epsMin = gsl_vector_min(epsilons);
	double epsMax = gsl_vector_max(epsilons);
	theZeroOneVector << epsMin - 0.2*(epsMax - epsMin) << epsMax + 0.2*(epsMax - epsMin);
	curve->setData(theZeroOneVector, theZeroOneVector);

	// Take away the legends
	theNormalityPlot->legend->setVisible(false);

	//theNormalityPlot->xAxis->rescale(true);
	theNormalityPlot->xAxis->setRange(QCPRange(epsMin, epsMax));
	theNormalityPlot->yAxis->setRange(QCPRange(epsMin, epsMax));
	theNormalityPlot->xAxis->scaleRange(1.1);
	theNormalityPlot->yAxis->scaleRange(1.1);
	theNormalityPlot->replot();

	//gsl_vector_free(epsilons);
	//gsl_vector_free(normalEpsilons);
	// 
#endif // NO_QCP



}

void RModelInferenceDialog::createRandomVariablesAndModel(gsl_vector *mean_theta, gsl_vector *cov_theta, double mean_sigma, double cov_sigma)
{
	// Create the model parameters, theta
	RContinuousRandomVariable *rv;
	for (int i = 0; i<k; i++) {

		QString myString = QString("theta%1").arg(i + 1);
		theDomain->createObject(myString, "RContinuousRandomVariable", false);
		rv = qobject_cast<RContinuousRandomVariable *>(theDomain->getLastAddedObject());

		if (NormalDistributionRadioButton->isChecked()) {
			rv->setDistributionType("Normal (mean, stdv)");
			rv->setMean(QString("%1").arg(gsl_vector_get(mean_theta, i), 0, 'g', 14));
			rv->setStandardDeviation(QString("%1").arg(gsl_vector_get(cov_theta, i) / 100 * qAbs(gsl_vector_get(mean_theta, i)), 0, 'g', 14)); // COV is divided by 100
		}
		else if (TDistributionRadioButton->isChecked()) {
			rv->setDistributionType("t-distribution (nu, xo, sigma)");
			rv->setParameter1(QString("%1").arg(double(n - k), 0, 'g', 14));
			rv->setParameter2(QString("%1").arg(gsl_vector_get(mean_theta, i), 0, 'g', 14));
			double sigma = gsl_vector_get(cov_theta, i) / 100 * qAbs(gsl_vector_get(mean_theta, i)) / sqrt(1.0*(n - k) / (n - k - 2));
			rv->setParameter3(QString("%1").arg(sigma, 0, 'g', 14));
		}

	}
	// Create the model parameter, epsilon, and the random sigma
	theDomain->createObject("Epsilon", "RRandomVariableWithRandomParametersModel", false);
	RRandomVariableWithRandomParametersModel *epsilon = qobject_cast<RRandomVariableWithRandomParametersModel *>(theDomain->getLastAddedObject());

	theDomain->createObject("EpsilonUncertainty", "RContinuousRandomVariable", false);
	RContinuousRandomVariable *epsilonUncertainty = qobject_cast<RContinuousRandomVariable *>(theDomain->getLastAddedObject());
	epsilonUncertainty->setDistributionType("Normal (mean, stdv)");
	epsilonUncertainty->setMean(QString("%1").arg(0.0, 0, 'g', 14));
	epsilonUncertainty->setStandardDeviation(QString("%1").arg(1.0, 0, 'g', 14));

	theDomain->createObject("ZeroConstant", "RConstant", false);
	RConstant *ZeroConstant = qobject_cast<RConstant *>(theDomain->getLastAddedObject());
	ZeroConstant->setCurrentValue(0.0);

	if (NormalDistributionRadioButton->isChecked()) {
		theDomain->createObject("Sigma", "RContinuousRandomVariable", false);
		RContinuousRandomVariable *sigma = qobject_cast<RContinuousRandomVariable *>(theDomain->getLastAddedObject());
		sigma->setDistributionType("Normal (mean, stdv)");
		sigma->setMean(QString("%1").arg(mean_sigma, 0, 'g', 14));
		sigma->setCoefficientOfVariation(QString("%1").arg(cov_sigma, 0, 'g', 14));
		epsilon->setStandardNormalRandomVariable(epsilonUncertainty);
		epsilon->setParameterList("ZeroConstant, Sigma");
	}
	else if (TDistributionRadioButton->isChecked()) {
		theDomain->createObject("SigmaSquared", "RContinuousRandomVariable", false);
		RContinuousRandomVariable *sigma_squared = qobject_cast<RContinuousRandomVariable *>(theDomain->getLastAddedObject());
		sigma_squared->setDistributionType("Scaled Inverse Chi-square (nu, xo, sigma)");
		double mean_sigma_squared = gsl_pow_2(mean_sigma);
		sigma_squared->setMean(QString("%1").arg(mean_sigma_squared, 0, 'g', 14));
		double stdv_sigma = cov_sigma / 100.0 * mean_sigma;
		double variance_sigma = gsl_pow_2(stdv_sigma);
		double vaiance_sigma_squared = variance_sigma * (4.0*mean_sigma_squared);
		double stdv_sigma_squared = sqrt(vaiance_sigma_squared);
		sigma_squared->setStandardDeviation(QString("%1").arg(stdv_sigma_squared, 0, 'g', 14));

		theDomain->createObject("SqrtSigmaSquared", "RAlgebraicExpressionModel", false);
		RAlgebraicExpressionModel *theSigma = qobject_cast<RAlgebraicExpressionModel *>(theDomain->getLastAddedObject());
		theSigma->setExpression("sqrt(SigmaSquared)");
		epsilon->setStandardNormalRandomVariable(epsilonUncertainty);
		epsilon->setParameterList("ZeroConstant, SqrtSigmaSquaredResponse");
	}


		// Create the explanatory variables (they are case specific, so set them as Constants equal to 1.0)
		RConstant *xVariable;
		for (int i=0; i<k; i++) {

				QString myString = QString("x%1").arg(i+1);
				theDomain->createObject(myString, "RConstant", false);
				xVariable = qobject_cast<RConstant *>(theDomain->getLastAddedObject());
				xVariable->setCurrentValue(1.0);
		}


		// Create a generic model (if a search has been carried out, then use the new explanatory functions)
		theDomain->createObject("theProbabilisticModel", "RAlgebraicExpressionModel", false);
		RAlgebraicExpressionModel *theExprModel = qobject_cast<RAlgebraicExpressionModel *>(theDomain->getLastAddedObject());
		QString theExpressionString = "theta1 * (";
		if (SearchCheckBox->isChecked()) {
				theExpressionString.append( stringList[0] + ")");
				for (int i=2; i<=k; i++) {
						theExpressionString.append(QString(QString(" + theta%1 * (").arg(i) + stringList[i-1]) + ")");
				}
		}
		else {
				theExpressionString.clear();
				theExpressionString.append("theta1*x1");
				for (int i=2; i<=k; i++) {
						theExpressionString.append(QString(" + theta%1*x%1").arg(i));
				}
		}
		theExpressionString.append(" + EpsilonResponse");
		theExprModel->setExpression(theExpressionString);

}

void RModelInferenceDialog::searchForExplanatoryFunctions(gsl_vector *mean_theta, gsl_vector *cov_theta)
{
		QString plotTitle = "Before Search";
		createModelPredictionPlot(mean_theta, plotTitle);

		// Initial declarations
		double trialFunction;
		int indexOfWorstExplanatoryFunction;
		double valueOfWorstExplanatoryFunction;
		gsl_matrix *correlationMatrix = gsl_matrix_calloc(k,k);
		double mean_sigma = 0.0;
		double cov_sigma = 0.0;
		double R = 0.0;


		// Check if the first column is intercept
		bool firstColumnIsIntercept = true;
		for (int i=0; i<n; i++) {
				if (gsl_matrix_get(X,i,0)!=1.0) {
						firstColumnIsIntercept = false;
						break;
				}
		}


		// Establish a new X matrix of explanatory functions
		gsl_matrix *Xnew = gsl_matrix_calloc(n,k);
		gsl_matrix_memcpy(Xnew, X);

		// Also have a trial X matrix to play around with
		gsl_matrix *Xtrial = gsl_matrix_calloc(n,k);

		// Set the combination vector (the vector of c-values)
		gsl_vector *cValues = gsl_vector_calloc(k);
		for (int i=0; i<k; i++) {
				gsl_vector_set(cValues,i,-power);//Mojtaba: -2.0
		}


		// Initialize the list of regressors
		if (!stringList.isEmpty()) {
				stringList.clear();
		}

		QString regressorString = "";
		for (int i=0; i<k; i++) {
				regressorString.clear();
				regressorString.append( QString("x%1").arg(i+1) );
				stringList << regressorString;
		}


		// Allocate vectors and matrices for the quick inference analysis
		gsl_matrix *XX = gsl_matrix_calloc(k,k);
		gsl_matrix *XXcopy = gsl_matrix_calloc(k, k);
		gsl_matrix *XXinv = gsl_matrix_calloc(k, k);
		gsl_matrix *XXinvX = gsl_matrix_calloc(k,n);
		gsl_vector *Xtheta = gsl_vector_calloc(n);
		gsl_vector *y_minus_Xtheta = gsl_vector_calloc(n);
		gsl_matrix *covarianceMatrix = gsl_matrix_calloc(k,k);
		gsl_vector *cov_theta_trial = gsl_vector_calloc(k);
		gsl_permutation *p = gsl_permutation_alloc(k);
		int signum = 0;
		double dot_product_y_minus_Xtheta;
		double s_squared;
		double multiplicationFactor;
		double determinant;
		double Xminimum;
		double Xmaximum;
		double covarianceValue;
		double cSum;
		int c;
		bool negativeCovariance;
		bool oneElementInXisOne;


		// Loop over all combinations
		int numCombinations = (int)gsl_pow_int(2*power+1,k); //Mojtaba: 5.0
		RObject::rDebug() << "*************************";
		RObject::rDebug() << " ";
		RObject::rDebug() << " ";
		RObject::rDebug() << "Number of combinations in search for explanatory functions:" << numCombinations;
		for (int m=0; m<numCombinations; m++) {
				//Mojtaba
				if (m % 50 == 0) {
						QCoreApplication::processEvents();
						//RObject::rDebug() << "Processing number" << m;
				}

				negativeCovariance = 0;
				oneElementInXisOne = 0;

				// Don't do the trial functions that are already in X
				cSum = 0.0;
				for (int i=0; i<k; i++) {
						cSum += gsl_vector_get(cValues, i);
				}
				for (int i=0; i<k; i++) {
						if (gsl_vector_get(cValues,i)==1.0) {
								oneElementInXisOne = 1;
								break;
						}
				}
				if (!(oneElementInXisOne && cSum==1.0)) {

						// Update the trial matrix
						gsl_matrix_memcpy(Xtrial, Xnew);

						// Identify the regressor with highest coefficient of variation
						indexOfWorstExplanatoryFunction = gsl_vector_max_index(cov_theta);
						valueOfWorstExplanatoryFunction = gsl_vector_get(cov_theta, indexOfWorstExplanatoryFunction);

						// Insert the candidate explanatory function into the trial X matrix
						for (int i=0; i<n; i++) {
								trialFunction = 1.0;
								for (int j=0; j<k; j++) {
										trialFunction = trialFunction * pow( gsl_matrix_get(X,i,j) , gsl_vector_get(cValues,j) );
								}
								gsl_matrix_set( Xtrial, i, indexOfWorstExplanatoryFunction, trialFunction);
						}

						Xmaximum = gsl_matrix_max(Xtrial);
						Xminimum = gsl_matrix_min(Xtrial);
						if (Xminimum>1.0e-16 || Xmaximum<1.0e+16) {

								// Do a "quick inference analysis
								int nu = n-k;
								gsl_blas_dgemm(CblasTrans, CblasNoTrans, 1.0, Xtrial, Xtrial, 0.0, XX);
								gsl_matrix_memcpy(XXcopy, XX);
								signum = 0;
								gsl_linalg_LU_decomp(XXcopy, p, &signum);
								determinant = gsl_linalg_LU_det(XXcopy,signum);

								if (determinant != 0.0) {

										gsl_linalg_LU_invert(XXcopy, p, XXinv);
										gsl_blas_dgemm(CblasNoTrans, CblasTrans, 1.0, XXinv, Xtrial, 0.0, XXinvX);
										gsl_blas_dgemv(CblasNoTrans, 1.0, XXinvX, y, 0.0, mean_theta);
										gsl_blas_dgemv(CblasNoTrans, 1.0, Xtrial, mean_theta, 0.0, Xtheta);
										gsl_vector_memcpy(y_minus_Xtheta, y);
										gsl_vector_sub(y_minus_Xtheta, Xtheta);
										gsl_blas_ddot (y_minus_Xtheta, y_minus_Xtheta, &dot_product_y_minus_Xtheta);
										s_squared = dot_product_y_minus_Xtheta/nu;
										gsl_matrix_memcpy(covarianceMatrix, XXinv);
										multiplicationFactor = nu*s_squared/(nu-2.0);
										for (int i=0; i<k; i++) {
												for (int j=0; j<k; j++) {
														gsl_matrix_set(covarianceMatrix,i,j,   (multiplicationFactor*gsl_matrix_get(covarianceMatrix,i,j)));
												}
										}

										for (int i=0; i<k; i++) {
												covarianceValue = gsl_matrix_get(covarianceMatrix,i,i);
												if (covarianceValue<0.0) {
														negativeCovariance = 1;
														break;
												}
												else {
														gsl_vector_set(cov_theta_trial, i, qAbs(100.0*(sqrt(covarianceValue))/gsl_vector_get(mean_theta,i)) );
												}
										}


										if (!negativeCovariance) {

												// Check the COV for the new variable
												if (gsl_vector_get(cov_theta_trial,indexOfWorstExplanatoryFunction)<valueOfWorstExplanatoryFunction) {

														// Update the matrix of regressor values
														gsl_matrix_memcpy(Xnew, Xtrial);
														gsl_vector_memcpy(cov_theta, cov_theta_trial);

														// Update the string of regressors
														stringList[indexOfWorstExplanatoryFunction].clear();
														for (int i=0; i<k; i++) {
																c = (int)gsl_vector_get(cValues,i);
																if ((i==0) && (!firstColumnIsIntercept)) {
																		if (c==0) {
																				stringList[indexOfWorstExplanatoryFunction].append( QString("1").arg(i+1) );
																		}
																		else if (c==1) {
																				stringList[indexOfWorstExplanatoryFunction].append( QString("x%1").arg(i+1) );
																		}
																		else if (c==-1) {
																				stringList[indexOfWorstExplanatoryFunction].append( QString("1/x%1").arg(i+1) );
																		}
																		else if (c<0) {
																				stringList[indexOfWorstExplanatoryFunction].append( QString("1/x%1^%2").arg(i+1).arg(-c) );
																		}
																		else {
																				stringList[indexOfWorstExplanatoryFunction].append( QString("x%1^%2").arg(i+1).arg(c) );
																		}
																}
																else if ((i==0) && (firstColumnIsIntercept)) {
																}
																else if ((i==1) && (firstColumnIsIntercept)) {
																		if (c==0) {
																				stringList[indexOfWorstExplanatoryFunction].append( QString("1").arg(i+1) );
																		}
																		else if (c==1) {
																				stringList[indexOfWorstExplanatoryFunction].append( QString("x%1").arg(i+1) );
																		}
																		else if (c==-1) {
																				stringList[indexOfWorstExplanatoryFunction].append( QString("1 / x%1").arg(i+1) );
																		}
																		else if (c<0) {
																				stringList[indexOfWorstExplanatoryFunction].append( QString("1 / x%1^%2").arg(i+1).arg(-c) );
																		}
																		else {
																				stringList[indexOfWorstExplanatoryFunction].append( QString("x%1^%2").arg(i+1).arg(c) );
																		}
																}
																else {
																		if (c==0) {
																		}
																		else if (c==1) {
																				stringList[indexOfWorstExplanatoryFunction].append( QString(" * x%1").arg(i+1) );
																		}
																		else if (c==-1) {
																				stringList[indexOfWorstExplanatoryFunction].append( QString(" / x%1").arg(i+1) );
																		}
																		else if (c<0) {
																				stringList[indexOfWorstExplanatoryFunction].append( QString(" / x%1^%2").arg(i+1).arg(-c) );
																		}
																		else {
																				stringList[indexOfWorstExplanatoryFunction].append( QString(" * x%1^%2").arg(i+1).arg(c) );
																		}
																}
														}
												}
										}
								}
						}
				}

				// Increment c-values
				incrementCombinationVector(cValues);
		}


		// Now the original X matrix is replaced by the new
		gsl_matrix_memcpy(X, Xnew);

		// Carry out a full inference analysis
		carryOutInference(mean_theta, cov_theta, correlationMatrix, mean_sigma, cov_sigma, R);

		// Print results
		RObject::rDebug() << "";
		RObject::rDebug() << "Regressors after search:";
		for (int i=0; i<k; i++) {
				RObject::rDebug() << stringList[i];
		}
		RObject::rDebug() << "";
		RObject::rDebug() << "Mean of the model parameters (after search):";
		theDomain->print(mean_theta, 0);
		RObject::rDebug() << "";
		RObject::rDebug() << "Coefficient of variation (in percent) of the model parameters (after search):";
		theDomain->print(cov_theta, 0);
		RObject::rDebug() << "";
		RObject::rDebug() << "Correlation matrix of the model parameters (after search):";
		theDomain->print(correlationMatrix, 0);
		RObject::rDebug() << "";
		RObject::rDebug() << "R-factor (after search):" << R;
		RObject::rDebug() << "Mean of sigma (after search):" << mean_sigma;
		RObject::rDebug() << "";



		// Plot the quality of the new model
		plotTitle = "After Search";
		createModelPredictionPlot(mean_theta, plotTitle);


		gsl_permutation_free(p);
		gsl_vector_free(cValues);
		gsl_matrix_free(Xtrial);
		gsl_matrix_free(XX);
		gsl_matrix_free(XXcopy);
		gsl_matrix_free(XXinv);
		gsl_matrix_free(XXinvX);
		gsl_vector_free(Xtheta);
		gsl_vector_free(y_minus_Xtheta);
		gsl_matrix_free(covarianceMatrix);

}

int RModelInferenceDialog::incrementCombinationVector(gsl_vector *cValues)
{
		for (int i=0; i<k; i++) {
				if (gsl_vector_get(cValues,i)==power) { //Mojtaba: 2.0
						gsl_vector_set(cValues, i, -power );//Mojtaba: -2.0
				}
				else {
						gsl_vector_set(cValues, i, (gsl_vector_get(cValues,i)+1.0) );
						return 0;
				}
		}
		return 0;
}
