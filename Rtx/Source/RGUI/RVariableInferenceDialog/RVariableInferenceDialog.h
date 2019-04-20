// Last modified by Mojtaba on June 30, 2012

#ifndef RVariableInferenceDialog_H
#define RVariableInferenceDialog_H

#include <QDialog>
#include "ui_RVariableInferenceDialog.h"

#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>

class RDomain;

using namespace Ui;

class RVariableInferenceDialog : public QDialog, public RVariableInferenceDialogUi
{
	Q_OBJECT

public:
	RVariableInferenceDialog(QWidget *parent = 0, RDomain *passedDomain = 0);
	~RVariableInferenceDialog();

	void setDomain(RDomain *passedDomain);

	double getInput();

private slots:
	void onOkButtonClicked();	
	void browse();

private:
	int analyze();
	void deleteVectors();

	RDomain *theDomain;
	gsl_matrix *X;

};

#endif // RVariableInferenceDialog_H
