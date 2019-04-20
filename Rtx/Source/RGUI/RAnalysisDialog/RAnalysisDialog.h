// Last modified by Mojtaba on June 30, 2012

#ifndef RAnalysisDialog_H
#define RAnalysisDialog_H

#include <QDialog>
#include "ui_RAnalysisDialog.h"

class RDomain;

using namespace Ui;

class RAnalysisDialog : public QDialog, public RAnalysisDialogUi
{
	Q_OBJECT

public:
	RAnalysisDialog(QWidget *parent = 0, RDomain *passedDomain = 0);
	~RAnalysisDialog();

	// Sets the RDomain object
	void setDomain(RDomain *passedDomain);

private slots:
	// Method tat is invoked when the OK button is clicked on
	void onOkButtonClicked();	

private:
	RDomain *theDomain;

};

#endif // RAnalysisDialog_H
