// Last modified by Mojtaba on June 30, 2012

#ifndef RModelInferenceDialog_H
#define RModelInferenceDialog_H

#include <QDialog>
#include "ui_RModelInferenceDialog.h"
#include <QPointer>

#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>

class RDomain;

using namespace Ui;

class RModelInferenceDialog : public QDialog, public RModelInferenceDialogUi
{
		Q_OBJECT

public:
		RModelInferenceDialog(QWidget *parent = 0, RDomain *passedDomain = 0);
		~RModelInferenceDialog();

		void setDomain(RDomain *passedDomain);

		double getInput();

private slots:
		void onOkButtonClicked();
		void browse();

private:
		int analyze();
		void deleteVectors();
		void playAroundWithNewData();
		int carryOutInference(gsl_vector *mean_theta, gsl_vector *cov_theta, gsl_matrix *correlationMatrix, double &dmean_sigma, double &cov_sigma, double &R);
		void createResidualPlot(gsl_vector *mean_theta);
		void createModelPredictionPlot(gsl_vector *mean_theta, QString plotTitle);
		void createModelObsRatioPlot(gsl_vector *mean_theta);
		void createNormalityPlot(gsl_vector *mean_theta, double mean_sigma);
		void createRandomVariablesAndModel(gsl_vector *mean_theta, gsl_vector *cov_theta, double mean_sigma, double cov_sigma);
		void searchForExplanatoryFunctions(gsl_vector *mean_theta, gsl_vector *cov_theta);
		int incrementCombinationVector(gsl_vector *cValues);

		RDomain *theDomain;
		gsl_vector *y;
		gsl_matrix *X;
		int n, k;
		QStringList stringList;

		double power;

};

#endif // RModelInferenceDialog_H
