#include "RPropertyWidget.h"

#include <QCompleter>
#include <QLineEdit>
#include <QMetaProperty>
#include <QHeaderView>
#include <QScrollBar>
#include <QResizeEvent>
#include <QHeaderView>
#include <QWebEngineView>
#include <QFileInfo>
#include <QTimer>

#include "RMainWindow.h"
#include "RObject.h"
#include "RDomain.h"
#include "RComboBoxTableDelegate.h"
#include "RCheckBoxTableDelegate.h"
#include "RDateTimeDelegate.h"
#include "RColorComboBoxTableDelegate.h"

QMessageBox *mbox1;

RPropertyWidget::RPropertyWidget(QWidget *parent)
	: QWidget(parent)
{
	setupUi(this);

	currentObject = 0;
	senderObject = 0;
	theDomain = 0;

	currentObjectChanged = false;

	connect(tableWidget, SIGNAL(cellChanged(int, int)), this, SLOT(assignProperties(int, int)));


	bool ok = connect(tableWidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(customMenuRequested(QPoint)));

	helpAction = new QAction(tr("&Help"), this);
	//createAction->setIcon(QIcon(":/RImages/add.png"));
	helpAction->setStatusTip(tr("Opens the Help for this property"));
	ok = connect(helpAction, SIGNAL(triggered()), this, SLOT(loadHelp()));
	createMenu.addAction(helpAction);

}

RPropertyWidget::~RPropertyWidget()
{
	//deleteItems(); // Items are automatically deleted upon destruction of the widget.
}

void RPropertyWidget::viewProperties(QString &passedName)
{
	QObject *oldObject = currentObject;
	currentObject = theDomain->findChild<QObject *>(passedName);
	senderObject = sender();

	if (!(oldObject == currentObject)) {
		currentObjectChanged = true;

		for (int i = 0; i < tableWidget->rowCount(); i++) {
			QAbstractItemDelegate *rowDelegate = tableWidget->itemDelegateForRow(i);
			if (rowDelegate) {
				tableWidget->setItemDelegateForRow(i, 0);
			}
		}

		QObjectList objectList = findChildren<QObject *>("RDelegate");
		int i = 0;
		while (i < objectList.count()) {
			delete objectList.takeAt(0);
		}
	} else {
		currentObjectChanged = false;
	}

	updateView();
}

QString RPropertyWidget::propertyToName(QString name)
{
	QString str(name);
	str.replace(0, 1, str.at(0).toUpper());
	//QRegExp regExp("[A-Z0-9]");

	QRegExp regExp1("(([A-Z]+[a-z]*)|([0-9]+[a-z]*))");
	QString result;
	int pos = 0;

	while ((pos = regExp1.indexIn(str, pos)) != -1) {
		result += regExp1.cap(1) + ' ';
		pos += regExp1.matchedLength();
	}

	str = result;

	QRegExp regExp2("([A-Z]{2,}[a-z]*)");
	QRegExp regExp3("([A-Z][a-z]+)");
	pos = 0;

	while ((pos = regExp2.indexIn(result, pos)) != -1) {
		int index = regExp2.cap(1).lastIndexOf(regExp3);
		result.insert(pos + index, " ");
		pos += regExp2.matchedLength();
	}

	result.remove(result.length() - 1, 1);

	return result;

	/*QString space = " ";
	int no = str.count(regExp);
	int startIndex = -1;
	int endIndex;
	QString tempStr;
	for (int i = 0; i < no; i++) {
		startIndex = str.indexOf(regExp, startIndex+1);
		endIndex = str.indexOf(regExp, startIndex+1) - 1;
		if (i == no - 1) {
			endIndex = str.count()-1;
			space = "";
		}
		tempStr += str.mid(startIndex, endIndex - startIndex + 1);
		tempStr += space;
	}
	return tempStr;*/
}

QString RPropertyWidget::nameToProperty(QString name)
{
	QString str(name);
	if (str == QString("Object Name")) {
		str.replace(0, 1, str.at(0).toLower());
	}
	str.remove(' ');
	return str;
}

void RPropertyWidget::deleteItems()
{
	for (int i = 0; i < tableWidget->rowCount(); i++) {		
		for (int j = 0; j < tableWidget->columnCount(); j++) {
			delete tableWidget->takeItem(0, 0);
		}		
	}	
}

void RPropertyWidget::removeAll()
{
	while (tableWidget->rowCount() > 0) {
		tableWidget->removeRow(0);
	}
}

void RPropertyWidget::updateView()
{
	int currentRow = tableWidget->currentRow();
	
	disconnect(tableWidget, SIGNAL(cellChanged(int, int)), this, SLOT(assignProperties(int, int)));
	deleteItems();
	removeAll();
	if (!currentObject) {
		return;
	}
	
	// Show writable static properties
	for (int i = 0; i < currentObject->metaObject()->propertyCount(); i++) {
		QMetaProperty metaProperty = currentObject->metaObject()->property(i);
		if (metaProperty.isWritable()) {
			QString propertyName = metaProperty.name();

			QString propertyValue = theDomain->getProperty(currentObject, propertyName);
			QVariant::Type type = metaProperty.type();
			
			insertRow(propertyName, propertyValue, type, true, (!metaProperty.isUser() && theDomain->thereIsOngoingAnalysis()));
		}
	}
	
	// Show dynamic properties
	QList<QByteArray> dynamicPropertyList = currentObject->dynamicPropertyNames();
	for (int i = 0; i < dynamicPropertyList.count(); i++) {
		int index = tableWidget->rowCount();
		QString propertyName = QString(dynamicPropertyList[i]);
		QString propertyValue = theDomain->getProperty(currentObject, propertyName);
		QVariant::Type type = currentObject->property(dynamicPropertyList[i]).type();

		insertRow(propertyName, propertyValue, type, true);
	}

	// Show un-writable static properties
	for (int i = 0; i < currentObject->metaObject()->propertyCount(); i++) {
		QMetaProperty metaProperty = currentObject->metaObject()->property(i);
		if (!metaProperty.isWritable()) {
			QString propertyName = metaProperty.name();

			QString propertyValue = theDomain->getProperty(currentObject, propertyName);
			QVariant::Type type = metaProperty.type();
			
			insertRow(propertyName, propertyValue, type, false);
		}
	}

	resizeEvent(0);
	connect(tableWidget, SIGNAL(cellChanged(int, int)), this, SLOT(assignProperties(int, int)));
	
	if ((currentRow > -1) && (currentRow < tableWidget->rowCount())) {
		tableWidget->selectRow(currentRow);
	}
}

void RPropertyWidget::insertRow(QString propertyName, QString propertyValue, QVariant::Type type, bool isWritable,  bool isTemporarilyUnwritable)
{
	int index = tableWidget->rowCount();
	tableWidget->insertRow(index);
	QTableWidgetItem *nameTableItem = new QTableWidgetItem(propertyToName(propertyName));
	nameTableItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
	tableWidget->setItem(index, 0, nameTableItem);

	if (type == QMetaType::Bool) {	
		/*RCheckBoxTableDelegate *tableDelegate = new RCheckBoxTableDelegate(this);
		tableWidget->setItemDelegateForRow(index, tableDelegate);*/
		// Checkbox does not work properly on Mac, so use a combobox with true and false entries
		QStringList trueFlaseStringList;
		trueFlaseStringList << "false" << "true";
		RComboBoxTableDelegate *tableDelegate = new RComboBoxTableDelegate(this, trueFlaseStringList, false);
		tableWidget->setItemDelegateForRow(index, tableDelegate);
	} else if ((type == QMetaType::QDateTime) || ((propertyName == QString("CurrentTime")) && (currentObject->inherits("RTime")))) {	
		RDateTimeDelegate *tableDelegate = new RDateTimeDelegate(this);
		tableWidget->setItemDelegateForRow(index, tableDelegate);
	} else if (type == QMetaType::QColor) {	
		RColorComboBoxTableDelegate *tableDelegate = new RColorComboBoxTableDelegate(this);
		tableWidget->setItemDelegateForRow(index, tableDelegate);
	}

	if (currentObjectChanged) {
		// ATTENTION: THE FOLLOWING LINE MAKES THE PROCESS OF SHOWING THE PROPERTIES VERY LONG IF THERE ARE A LOT OF OBJECTS IN THE DOMAIN
		QStringList objectsStringList = theDomain->getPropertyStringList(propertyName);

		bool editable = true;

		QByteArray byteArrayName = propertyName.toLatin1();
		const char *charName = byteArrayName.data();
		int index = currentObject->metaObject()->indexOfProperty(charName);
		QMetaProperty metaProperty = currentObject->metaObject()->property(index);
		if (metaProperty.isEnumType()) {
			objectsStringList.clear();
			QMetaEnum metaEnum = metaProperty.enumerator();
			for (int i = 0; i < metaEnum.keyCount(); i++) {
				objectsStringList << metaEnum.key(i);
			}
			editable = false;
		}

		if (!objectsStringList.isEmpty()) {
			RComboBoxTableDelegate *tableDelegate = new RComboBoxTableDelegate(this, objectsStringList, editable);
			tableWidget->setItemDelegateForRow(index, tableDelegate);
		}
	}


	QTableWidgetItem *valueTableItem = new QTableWidgetItem(propertyValue);
	if (!isWritable) {
		valueTableItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		valueTableItem->setBackgroundColor(Qt::lightGray);
		nameTableItem->setBackgroundColor(Qt::lightGray);
	}

	if (isTemporarilyUnwritable) {
		valueTableItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		valueTableItem->setBackgroundColor(QColor(225, 225, 225));
		nameTableItem->setBackgroundColor(QColor(225, 225, 225));
	}

	tableWidget->setItem(index, 1, valueTableItem);
}

void RPropertyWidget::resizeEvent(QResizeEvent *event)
{
	int verticalScrollbarWidth = 0;
	if (tableWidget->verticalScrollBar()->isVisible()) {
		verticalScrollbarWidth = tableWidget->verticalScrollBar()->width();
	}

	int verticalHeaderWidth = 0;
	if (tableWidget->verticalHeader()->isVisible()) {
		verticalHeaderWidth = tableWidget->verticalHeader()->sizeHint().width();
	}

	int width = tableWidget->width() - verticalHeaderWidth - verticalScrollbarWidth - 2;
	int count = tableWidget->columnCount();
	for (int i = 0; i < count; i++) {
		tableWidget->setColumnWidth(i, (int)width/count);
	}
}

void RPropertyWidget::assignProperties(int row, int column)
{
	QString propertyName = nameToProperty(tableWidget->item(row, 0)->text());
	QString propertyValue = tableWidget->item(row, 1)->text();

	int result = theDomain->setProperty(currentObject, propertyName, propertyValue);

	// If the passed property is objectName and renaming is successful, then 1 is returned by theDomain, so that the property widget sends a command to the tree widget to refresh the tree
	if (result == 1) {
		bool ok = QMetaObject::invokeMethod(senderObject, "updateView"); 
		ok = QMetaObject::invokeMethod(senderObject, "selectItem", Q_ARG(QString, propertyValue));
	}

	currentObjectChanged = false;

	updateView();
}

void RPropertyWidget::setDomain(RDomain *passedDomain)
{
	theDomain = passedDomain;
}

void RPropertyWidget::loadHelp()
{

	int currentRow = tableWidget->currentRow();
	QMetaProperty metaProperty = currentObject->metaObject()->property(currentRow);
	QString propertyName = metaProperty.name();
	QString classname = currentObject->metaObject()->className();



	RMainWindow *mainWindow = qobject_cast<RMainWindow *>(theDomain->parent());

	QWebEngineView *v_view = new QWebEngineView(this);
	QWebEnginePage *page = new QWebEnginePage();

	// Urls
	QString documentationUrl = QCoreApplication::applicationDirPath() + "/../../../Help/" + classname + ".html";
	QString documentationUrl_final = QCoreApplication::applicationDirPath() + "/Help/" + classname + ".html";
	QString documentationUrl_notFound = QCoreApplication::applicationDirPath() + "/../../../Help/404.html";
	QString documentationUrl_final_notFound = QCoreApplication::applicationDirPath() + "/Help/404.html";


	// Checks to see if the Documentation File is Available, If Not, the 404 Page is Loaded
	/*QFileInfo check_file(documentationUrl);
	if (!check_file.exists())
	{
		documentationUrl = QCoreApplication::applicationDirPath() + "/../../../Help/404.html";
	}

	// Release Build
#ifndef _DEBUG
	documentationUrl = QCoreApplication::applicationDirPath() + "/Help/" + classname + ".html";

	// Checks to see if the Documentation File is Available, If Not, the 404 Page is Loaded
	QFileInfo check_filer(documentationUrl);
	if (!check_filer.exists())
	{
		documentationUrl = QCoreApplication::applicationDirPath() + "/Help/404.html";
	}

#endif
	*/

	if (QFileInfo(documentationUrl).exists())
		page->load(QUrl((documentationUrl + "#" + propertyName)));
	else if(QFileInfo(documentationUrl_final).exists())
		page->load(QUrl((documentationUrl_final + "#" + propertyName)));
	else if (QFileInfo(documentationUrl_notFound).exists())
		page->load(QUrl((documentationUrl_notFound)));
	else
		page->load(QUrl((documentationUrl_final_notFound)));

	

	mbox1 = new QMessageBox;
	mbox1->setWindowTitle(tr("Information"));
	mbox1->setText("The help page is loading, please wait.");
	mbox1->setStandardButtons(0);
	mbox1->show();
	connect(page, &QWebEnginePage::loadFinished, this, &RPropertyWidget::hideMessageBox);
	v_view->setPage(page);
	mainWindow->addSubWindow(v_view, classname + " Help Browser");

}

void RPropertyWidget::customMenuRequested(const QPoint &pos) {

	createMenu.exec(this->mapToGlobal(pos));
}

void RPropertyWidget::hideMessageBox() {

	QTimer::singleShot(1, mbox1, SLOT(hide()));
	
}