#include "RAnalysisDialog.h"

#include <QMessageBox>
#include <QDateTime>

#include "RMainWindow.h"
#include "RDomain.h"
#include "RAnalyzer.h"


RAnalysisDialog::RAnalysisDialog(QWidget *parent, RDomain *passedDomain)
	: QDialog(parent)
{
	setupUi(this);

	theDomain = passedDomain;

	QList<RAnalyzer *> analyzerList = theDomain->findChildren<RAnalyzer *>();
	QStringList analyzerNameList;
	for (int i = 0; i < analyzerList.count(); i++) {
		analyzerNameList << analyzerList[i]->objectName();
	}
	analyzerComboBox->addItems(analyzerNameList);

	connect(okButton, SIGNAL(clicked()), this, SLOT(onOkButtonClicked()));
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
}

RAnalysisDialog::~RAnalysisDialog()
{	
	
}

void RAnalysisDialog::setDomain(RDomain *passedDomain)
{
	theDomain = passedDomain;
}

void RAnalysisDialog::onOkButtonClicked()
{	
	RAnalyzer *analyzer;
	RMainWindow *mainWindow = qobject_cast<RMainWindow *>(this->parent());

	if (analyzerComboBox->currentIndex() > -1) {
		analyzer = theDomain->findChild<RAnalyzer *>(analyzerComboBox->currentText());
	} else {
		QMessageBox::critical(this, tr("Error"), tr("Please choose an analyzer and try again."), QMessageBox::Ok | QMessageBox::Default);
		return;
	}

	accept();
	
	mainWindow->raiseDock("Output");

	int result = analyzer->startAnalyzing();
	if (result < 0) {
		RObject::rCritical() << "Error: The analysis did not run because of errors.";
		return;
	}
}
