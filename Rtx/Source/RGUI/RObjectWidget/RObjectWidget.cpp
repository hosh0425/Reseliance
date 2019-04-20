#include "RObjectWidget.h"

#include "RDomain.h"
#include "RContinuousRandomVariable.h"
#include "RLocation.h"
#include "RMainWindow.h"
#include "RScriptModel.h"
#include "RStModel.h"
#include "RPolygonalAreaSourceModel.h"	
#include "RMapWidget.h"
#include "RRegionalLossModel.h"
#include "RDecisionVariable.h"
#include "RSingleLineSourceModel.h"
#include "RMultipointLineSourceModel.h"
#include "RQuadrilateralAreaSourceModel.h"
#include "RRectangularAreaSourceModel.h"
#include "RCircularAreaSourceModel.h"
#include "RPolygonalAreaSourceModel.h"

#include <QHeaderView>
#include <QMessageBox>
#include <QPointer>
#include <QFileInfo>

QMessageBox *mbox2;
RObjectWidget::RObjectWidget(QWidget *parent)
	: QWidget(parent)
{
	setupUi(this);

	theDomain = 0;

	sameItemClicked = true;

	bool ok = connect(treeWidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showPopup(QPoint)));

	createAction = new QAction(tr("&Create"), this);
	//createAction->setIcon(QIcon(":/RImages/add.png"));
	createAction->setStatusTip(tr("Creates an object."));
	ok = connect(createAction, SIGNAL(triggered()), this, SLOT(addObject()));
	createMenu.addAction(createAction);

	removeAllAction = new QAction(tr("Remove &All"), this);
	//removeAllAction->setIcon(QIcon(":/RImages/add.png"));
	removeAllAction->setStatusTip(tr("Removes all objects of this type."));
	ok = connect(removeAllAction, SIGNAL(triggered()), this, SLOT(removeAllObject()));
	createMenu.addAction(removeAllAction);

	removeAction = new QAction(tr("&Remove"), this);
	//removeAction->setIcon(QIcon(":/RImages/remove.png"));
	removeAction->setStatusTip(tr("Removes the object."));
	ok = connect(removeAction, SIGNAL(triggered()), this, SLOT(removeObject()));
	objectMenu.addAction(removeAction);

	duplicateAction = new QAction(tr("&Duplicate"), this);
	//removeAction->setIcon(QIcon(":/RImages/remove.png"));
	duplicateAction->setStatusTip(tr("Duplicates the object."));
	ok = connect(duplicateAction, SIGNAL(triggered()), this, SLOT(duplicateObject()));
	objectMenu.addAction(duplicateAction);

	helpAction = new QAction(tr("&Help"), this);
	//removeAction->setIcon(QIcon(":/RImages/remove.png"));
	helpAction->setStatusTip(tr("Opens the corresponding documentation page."));
	ok = connect(helpAction, SIGNAL(triggered()), this, SLOT(LoadHelp()));
	objectMenu.addAction(helpAction);

	treeWidget->headerItem()->setText(0, "Name");
	treeWidget->sortItems(0, Qt::AscendingOrder);

	ok = connect(treeWidget, SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)), this, SLOT(changeCurrent(QTreeWidgetItem *, QTreeWidgetItem *)));
	ok = connect(treeWidget, SIGNAL(itemClicked(QTreeWidgetItem *, int)), this, SLOT(currentClicked(QTreeWidgetItem *)));
}

RObjectWidget::~RObjectWidget()
{
}

void RObjectWidget::setDomain(RDomain *passedDomain)
{
	theDomain = passedDomain;
	connect(theDomain, SIGNAL(newAnalysisStartedSignal(void)), this, SLOT(deactiveActionsWhileAnalysis(void)));
	connect(theDomain, SIGNAL(allAnalysisEndedSignal(void)), this, SLOT(reactivateActionsAfterAnalysisEnded(void)));
}

void RObjectWidget::createHeader(QString name, QTreeWidgetItem *parent, int parentingStatus)
{
	QTreeWidgetItem *item;
	QStringList headerList;
	if (parentingStatus == 0) {
		// No item will be instantiated
		headerList = RDomain::getSubClasses(name);
	}
	else {
		if (parentingStatus == 1) {
			item = new QTreeWidgetItem(treeWidget);
		}
		else if (parentingStatus == 2) {
			item = new QTreeWidgetItem(parent);
		}
		item->setText(0, name);
		headerList = RDomain::getSubClasses(getHeaderFullName(item));
	}

	if (headerList.isEmpty()) {
		lastLevelList << item;
	}

	for (int i = 0; i < headerList.count(); i++) {
		if (parentingStatus == 0) {
			createHeader(headerList[i], 0, 1);
		}
		else {
			createHeader(headerList[i], item, 2);
		}
	}
	//fitColumns();
}

QString RObjectWidget::getHeaderFullName(QTreeWidgetItem *item)
{
	QTreeWidgetItem *currentItem = item;

	QString result = item->text(0);
	while (item->parent()) {
		result = item->parent()->text(0) + ' ' + result;
		item = item->parent();
	}
	return result;
}

void RObjectWidget::updateView()
{
	QTreeWidgetItem *currentItem = treeWidget->currentItem();
	QString itemText;
	if (currentItem) {
		itemText = getHeaderFullName(treeWidget->currentItem());
	}
	deleteItems();
	for (int i = 0; i < lastLevelList.count(); i++) {
		populateBranch(lastLevelList[i]);
	}
	if (!itemText.isEmpty()) {
		selectItem(itemText);
	}

	//fitColumns();
}

void RObjectWidget::populateBranch(QTreeWidgetItem *passedItem)
{
	QString type = getHeaderFullName(passedItem);
	QObjectList objectList = theDomain->getObjectList(type);
	for (int j = 0; j < objectList.count(); j++) {
		createItem(passedItem, objectList[j]->objectName());
	}
}

void RObjectWidget::createItem(QTreeWidgetItem *parent, QString text)
{
	QTreeWidgetItem *item = new QTreeWidgetItem(parent);
	item->setFlags(item->flags() | Qt::ItemIsEditable | Qt::ItemIsUserCheckable);
	//item->setCheckState(0, Qt::Checked);
	item->setText(0, text);
}

void RObjectWidget::deleteItems()
{
	for (int i = 0; i < lastLevelList.count(); i++) {
		QList<QTreeWidgetItem *> itemList = lastLevelList[i]->takeChildren();
		for (int j = 0; j < itemList.count(); j++) {
			delete itemList[j];
		}
	}
}

//void RObjectWidget::fitColumns()
//{
//	for (int i = 0; i < treeWidget->columnCount(); i++) { 
//		// Fitting the column width to the data
//		treeWidget->resizeColumnToContents(i);		
//	}
//	// Increasing the width of the first column by 10%
//	treeWidget->header()->resizeSection(0, treeWidget->header()->sectionSize(0)*1.1);
//}


void RObjectWidget::addObject()
{
	QTreeWidgetItem *currentItem = treeWidget->currentItem();
	QString type;
	if (currentItem) {
		type = getHeaderFullName(currentItem);
	}

	// ATTENTION: THE FOLLOWING LINES MAY MAKE THE PROCESS OF ADDING A NEW OBJECT VERY LONG IF THERE ARE A LOT OF OBJECTS IN THE DOMAIN
	QRegExp regExp("(Untitled)(\\d)+");
	QObjectList objectList = theDomain->findChildren<QObject *>(regExp);
	int i = 1;
	if (objectList.count() > 0) {
		QStringList stringList;
		for (int j = 0; j < objectList.count(); j++) {
			stringList << objectList[j]->objectName();
		}
		bool continueFlag = true;
		while (continueFlag) {
			QString string = QString("Untitled%1").arg(i);
			int index = stringList.indexOf(string);
			if (index > -1) {
				stringList.removeAt(index);
				i++;
			}
			else {
				continueFlag = false;
			}
		}
	}

	/*int i = 1;
	while (theDomain->findChild<QObject *>(QString("Untitled%1").arg(i))) {
	i++;
	}*/

	QString name = QString("Untitled%1").arg(i);

	int result = theDomain->createObject(name, type, false);
	QObject *addedObject = theDomain->getLastAddedObject();
	if (result < 0) {
		return;
	}
	else if (result > 0) {
		if (addedObject) {
			name = addedObject->objectName();
		}
	}

	updateView();

	this->selectItem(name);
}

void RObjectWidget::removeAllObject()
{
	QTreeWidgetItem *currentItem = treeWidget->currentItem();

	if (!currentItem) {
		return;
	}
	if (currentItem->childCount() == 0) {
		return;
	}

	int response = QMessageBox::question(this, tr("Confirmation"), QString(tr("Are you sure you want to remove all objects of the type \"%1\"?")).arg(currentItem->text(0)), QMessageBox::Yes | QMessageBox::Default, QMessageBox::No);
	if (response == QMessageBox::Yes) {
		for (int i = 0; i < currentItem->childCount(); i++) {
			QTreeWidgetItem *childItem = currentItem->child(i);
			QString name = childItem->text(0);
			int result = theDomain->deleteObject(name);
			if (result == 0) {
				QString emptyString = "";
				emit currentObjectChanged(emptyString);
			}
		}

		// Warning: updating view after deleting objects can take a long time if there are many objects in the domian. The alternative is to delete currentItem, but the disadvantage is that the child objects are not removed from the tree.
		updateView();
	}
}

void RObjectWidget::selectItem(QString itemName)
{
	//QList<QTreeWidgetItem *> itemList = treeWidget->findItems(itemName, Qt::MatchExactly, 0);
	//if (!itemList.isEmpty()) {
	//	itemList[0]->setSelected(true);
	//}

	treeWidget->setFocus();
	treeWidget->clearSelection();
	for (int i = 0; i < lastLevelList.count(); i++) {
		QTreeWidgetItem *item = lastLevelList[i];
		for (int j = 0; j < item->childCount(); j++) {
			QTreeWidgetItem *childItem = item->child(j);
			if (childItem->text(0) == itemName) {
				treeWidget->setCurrentItem(childItem);
				//childItem->setSelected(true);
				//changeCurrent(childItem,0);
				treeWidget->expandItem(item);
				return;
			}
		}
	}
}

void RObjectWidget::deactiveActionsWhileAnalysis() {

	removeAction->setDisabled(true);
	duplicateAction->setDisabled(true);
}

void RObjectWidget::reactivateActionsAfterAnalysisEnded() {

	removeAction->setEnabled(true);
	duplicateAction->setEnabled(true);

}

void RObjectWidget::changeCurrent(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
	if (current) {
		if ((current->childCount() == 0) && (!(lastLevelList.contains(current)))) {
			QString objectName = current->text(0);
			emit currentObjectChanged(objectName);
			sameItemClicked = false;
		}
	}
}

void RObjectWidget::currentClicked(QTreeWidgetItem *item)
{
	if (sameItemClicked) {
		changeCurrent(item, 0);
	}
	sameItemClicked = true;
}

void RObjectWidget::removeObject()
{
	QTreeWidgetItem *currentItem = treeWidget->currentItem();
	if (currentItem) {
		QString name = currentItem->text(0);

		int response = QMessageBox::question(this, tr("Confirmation"), QString(tr("Are you sure you want to remove the object \"%1\"?")).arg(name), QMessageBox::Yes | QMessageBox::Default, QMessageBox::No);
		if (response == QMessageBox::Yes) {
			int result = theDomain->deleteObject(name);
			if (result == 0) {
				QString emptyString = "";
				emit currentObjectChanged(emptyString);

				// Warning: updating view after deleting objects can take a long time if there are many objects in the domian. The alternative is to delete currentItem, but the disadvantage is that the child objects are not removed from the tree.
				updateView();
				//delete currentItem;
			}
		}
	}
}

void RObjectWidget::duplicateObject()
{

	// Get the Object's name and class hierarchy
	QTreeWidgetItem *currentItem = treeWidget->currentItem()->parent();
	QString type, itemName;
	if (currentItem) {
		type = getHeaderFullName(currentItem);
		itemName = treeWidget->currentItem()->text(0);
	}

	// ATTENTION: THE FOLLOWING LINES MAY MAKE THE PROCESS OF ADDING A NEW OBJECT VERY LONG IF THERE ARE A LOT OF OBJECTS IN THE DOMAIN
	QRegExp regExp("("+itemName+")(\\d)+");
	QObjectList objectList = theDomain->findChildren<QObject *>(regExp);

	int i = 1;
	if (objectList.count() > 0) {
		QStringList stringList;
		for (int j = 0; j < objectList.count(); j++) {
			stringList << objectList[j]->objectName();
		}
		bool continueFlag = true;
		while (continueFlag) {
			QString string = itemName + QString("%1").arg(i);
			int index = stringList.indexOf(string);
			if (index > -1) {
				stringList.removeAt(index);
				i++;
			}
			else {
				continueFlag = false;
			}
		}
	}

	/*int i = 1;
	while (theDomain->findChild<QObject *>(QString("Untitled%1").arg(i))) {
	i++;
	}*/

	QString name = itemName + QString("%1").arg(i);

	int result = theDomain->createObject(name, type, false);
	QObject *addedObject = theDomain->getLastAddedObject();
	if (result < 0) {
		return;
	}
	else if (result > 0) {
		if (addedObject) {
			name = addedObject->objectName();
		}
	}

	// Copy the Properties of the object, except its name(i!=0)
	QObject* currentObject = theDomain->findChildren<QObject *>(itemName)[0];
	const QMetaObject *metaobject = currentObject->metaObject();
	int count = metaobject->propertyCount();
	for (int i = 1; i < count; ++i) {
		QMetaProperty metaproperty = metaobject->property(i);
		const char *name = metaproperty.name();
		QVariant value = currentObject->property(name);
		addedObject->setProperty(name, value);

	}

	updateView();

	this->selectItem(name);
}
void RObjectWidget::LoadHelp()
{
	RMainWindow *mainWindow = qobject_cast<RMainWindow *>(theDomain->parent());
	QTreeWidgetItem *currentItem = treeWidget->currentItem()->parent();
	QTreeWidgetItem *currentItemHeader = treeWidget->currentItem()->parent()->parent();
	QString type, type2;
	if (currentItem) {
		type = getHeaderFullName(currentItem);
		if (currentItemHeader)
		{
			type2 = getHeaderFullName(currentItemHeader);
		}
		else
		{
			type2 = "";
		}
	}
	QString className = type.remove(' ');
	//ClassName classEnum = classNamesHash.value(className, NoValue_CN);
	QHash<QString, ClassName> classNamesHash = theDomain->classNamesHash;

	ClassName classEnum = classNamesHash.value(className);
	//QString classtype = classNamesHash.find(classEnum);
	QStringList i = classNamesHash.keys(classEnum);
	for (int Index = 0; Index < i.size(); Index++)
	{
		if (i[Index][0] == 'r')
		{
			type = i[Index];
		}
	}


	QWebEngineView *v_view = new QWebEngineView(this);
	QWebEnginePage *page = new QWebEnginePage();

	// Urls 
	QString documentationUrl = QCoreApplication::applicationDirPath() + "/../../../Help/" + type + ".html";
	QString documentationUrl_final = QCoreApplication::applicationDirPath() + "/Help/" + type + ".html";
	QString documentationUrl_notFound = QCoreApplication::applicationDirPath() + "/../../../Help/404.html";
	QString documentationUrl_final_notFound = QCoreApplication::applicationDirPath() + "/Help/404.html";

/*
	// Checks to see if the Documentation File is Available, If Not, the 404 Page is Loaded
	QFileInfo check_file(documentationUrl);
	if (!check_file.exists())
	{
		documentationUrl = QCoreApplication::applicationDirPath() + "/../../../Help/404.html";
	}
	// Release Build
#ifndef _DEBUG
	documentationUrl = QCoreApplication::applicationDirPath() + "/Help/" + type + ".html";

	// Checks to see if the Documentation File is Available, If Not, the 404 Page is Loaded
	QFileInfo check_filer(documentationUrl);
	if (!check_filer.exists())
	{
		documentationUrl = QCoreApplication::applicationDirPath() + "/Help/404.html";
	}

#endif
*/

	if (QFileInfo(documentationUrl).exists())
		page->load(QUrl((documentationUrl)));
	else if (QFileInfo(documentationUrl_final).exists())
		page->load(QUrl((documentationUrl_final)));
	else if (QFileInfo(documentationUrl_notFound).exists())
		page->load(QUrl((documentationUrl_notFound)));
	else
		page->load(QUrl((documentationUrl_final_notFound)));
	

	mbox2 = new QMessageBox;
	mbox2->setWindowTitle(tr("Information"));
	mbox2->setText("The help page is loading, please wait.");
	mbox2->setStandardButtons(0);
	mbox2->show();
	connect(page, &QWebEnginePage::loadFinished, this, &RObjectWidget::hideMessageBox);
	v_view->setPage(page);
	mainWindow->addSubWindow(v_view, type + " Help Browser");
}

void RObjectWidget::showPopup(const QPoint &position)
{
	QTreeWidgetItem *item = 0;
	item = treeWidget->itemAt(position);
	if (!item) {
		return;
	}
	if (lastLevelList.contains(item)) {
		QString itemFullName = getHeaderFullName(item);
		if (itemFullName == QString("Parameter Random Variable Continuous")) {
			QAction *separator = createMenu.addSeparator();

			QAction rankByNameAction(tr("Rank by &Name"), this);
			rankByNameAction.setStatusTip(tr("Sorts the random variable list alphabetically."));
			bool ok = connect(&rankByNameAction, SIGNAL(triggered()), this, SLOT(rankRandomVariablesByName()));
			createMenu.addAction(&rankByNameAction);

			QAction rankByAlphaImportanceAction(tr("Rank by &Alpha Importance Measure"), this);
			rankByAlphaImportanceAction.setStatusTip(tr("Sorts the random variable list according to rank."));
			ok = connect(&rankByAlphaImportanceAction, SIGNAL(triggered()), this, SLOT(rankRandomVariablesByAlphaImportance()));
			createMenu.addAction(&rankByAlphaImportanceAction);

			QAction rankByGammaImportanceAction(tr("Rank by &Gamma Importance Measure"), this);
			rankByGammaImportanceAction.setStatusTip(tr("Sorts the random variable list according to rank."));
			ok = connect(&rankByGammaImportanceAction, SIGNAL(triggered()), this, SLOT(rankRandomVariablesByGammaImportance()));
			createMenu.addAction(&rankByGammaImportanceAction);

			createMenu.exec(treeWidget->mapToGlobal(position));

			ok = disconnect(&rankByNameAction, SIGNAL(triggered()), this, SLOT(rankRandomVariablesByName()));
			ok = disconnect(&rankByAlphaImportanceAction, SIGNAL(triggered()), this, SLOT(rankRandomVariablesByAlphaImportance()));
			ok = disconnect(&rankByGammaImportanceAction, SIGNAL(triggered()), this, SLOT(rankRandomVariablesByGammaImportance()));

			createMenu.removeAction(&rankByNameAction);
			createMenu.removeAction(&rankByAlphaImportanceAction);
			createMenu.removeAction(&rankByGammaImportanceAction);

			createMenu.removeAction(separator);
			if (separator) {
				delete separator;
			}

			return;
		}
		if (itemFullName == QString("Parameter Decision Variable Continuous")) {
			QAction *separator = createMenu.addSeparator();

			QAction rankByNameAction(tr("Rank by &Name"), this);
			rankByNameAction.setStatusTip(tr("Sorts the decision variable list alphabetically."));
			bool ok = connect(&rankByNameAction, SIGNAL(triggered()), this, SLOT(rankDecisionVariablesByName()));
			createMenu.addAction(&rankByNameAction);

			QAction rankBySensitivityAction(tr("Rank by &Sensitivity"), this);
			rankBySensitivityAction.setStatusTip(tr("Sorts the decision variable list according to sensitivity."));
			ok = connect(&rankBySensitivityAction, SIGNAL(triggered()), this, SLOT(rankDecisionVariablesBySensitivity()));
			createMenu.addAction(&rankBySensitivityAction);

			createMenu.exec(treeWidget->mapToGlobal(position));

			ok = disconnect(&rankByNameAction, SIGNAL(triggered()), this, SLOT(rankDecisionVariablesByName()));
			ok = disconnect(&rankBySensitivityAction, SIGNAL(triggered()), this, SLOT(rankDecisionVariablesBySensitivity()));

			createMenu.removeAction(&rankByNameAction);
			createMenu.removeAction(&rankBySensitivityAction);

			createMenu.removeAction(separator);
			if (separator) {
				delete separator;
			}

			return;
		}
		if (itemFullName == QString("Parameter Location")) {
			QAction *separator = createMenu.addSeparator();

			QAction viewAllLocationsAction(tr("&View All Locations on Map"), this);
			viewAllLocationsAction.setStatusTip(tr("Views all the location objects on the map."));
			bool ok = connect(&viewAllLocationsAction, SIGNAL(triggered()), this, SLOT(viewAllLocationsOnMap()));
			createMenu.addAction(&viewAllLocationsAction);

			QAction removeAllLocationsAction(tr("&Remove All Locations from the Map"), this);
			removeAllLocationsAction.setStatusTip(tr("Removes all the location objects from the map."));
			ok = connect(&removeAllLocationsAction, SIGNAL(triggered()), this, SLOT(removeAllLocationsFromMap()));
			createMenu.addAction(&removeAllLocationsAction);

			QAction getCoordinatesfromAddressAction(tr("&Get All Coordinates from Address"), this);
			getCoordinatesfromAddressAction.setStatusTip(tr("Gets all coordinates from the address of the location."));
			ok = connect(&getCoordinatesfromAddressAction, SIGNAL(triggered()), this, SLOT(getCoordinatesfromAddress()));
			createMenu.addAction(&getCoordinatesfromAddressAction);

			createMenu.exec(treeWidget->mapToGlobal(position));

			ok = disconnect(&viewAllLocationsAction, SIGNAL(triggered()), this, SLOT(viewAllLocationsOnMap()));
			ok = disconnect(&removeAllLocationsAction, SIGNAL(triggered()), this, SLOT(removeAllLocationsFromMap()));
			ok = disconnect(&getCoordinatesfromAddressAction, SIGNAL(triggered()), this, SLOT(getCoordinatesfromAddress()));

			createMenu.removeAction(&viewAllLocationsAction);
			createMenu.removeAction(&removeAllLocationsAction);
			createMenu.removeAction(&getCoordinatesfromAddressAction);

			createMenu.removeAction(separator);
			if (separator) {
				delete separator;
			}

			return;
		}
		if (itemFullName == QString("Model Consequence Economic Regional Loss")) {
			QAction *separator = createMenu.addSeparator();

			QAction viewAllSectorsAction(tr("&View All Models on Map"), this);
			viewAllSectorsAction.setStatusTip(tr("Views all the regional models on the map."));
			bool ok = connect(&viewAllSectorsAction, SIGNAL(triggered()), this, SLOT(viewAllSectorsOnMap()));
			createMenu.addAction(&viewAllSectorsAction);


			createMenu.exec(treeWidget->mapToGlobal(position));

			ok = disconnect(&viewAllSectorsAction, SIGNAL(triggered()), this, SLOT(viewAllSectorsOnMap()));

			createMenu.removeAction(&viewAllSectorsAction);

			createMenu.removeAction(separator);
			if (separator) {
				delete separator;
			}

			return;
		}
		if (itemFullName == QString("Model Hazard Earthquake Location")) {
			QAction *separator = createMenu.addSeparator();

			QAction viewAllModelsAction(tr("&View All Models on Map"), this);
			viewAllModelsAction.setStatusTip(tr("Views all the location models on the map."));
			bool ok = connect(&viewAllModelsAction, SIGNAL(triggered()), this, SLOT(viewAllLocationModelsOnMap()));
			createMenu.addAction(&viewAllModelsAction);

			createMenu.exec(treeWidget->mapToGlobal(position));

			ok = disconnect(&viewAllModelsAction, SIGNAL(triggered()), this, SLOT(viewAllLocationModelsOnMap()));

			createMenu.removeAction(&viewAllModelsAction);

			createMenu.removeAction(separator);
			if (separator) {
				delete separator;
			}

			return;
		}

		createMenu.exec(treeWidget->mapToGlobal(position));

	}
	else if (!(lastLevelList.contains(item)) && (item->childCount() == 0)) {

		QPointer<RObject> object = theDomain->findChild<RObject *>(item->text(0));
		QStringList popupList = object->getActionList();
		if (object) {
			for (int i = 0; i < actionList.count(); i++) {
				objectMenu.removeAction(actionList[i]);
				delete actionList[i];
			}

		}
		if (!popupList.isEmpty()) {
			actionList << objectMenu.addSeparator();

			for (int i = 0; i < popupList.count(); i++) {
				if (popupList[i].toLower() == QString("Separator").toLower()) {
					QAction *separator = objectMenu.addSeparator();
					//separator->setParent(object);
				}
				else {
					QAction *action = new QAction(popupList[i], object);
					action->setObjectName(popupList[i]);
					actionList << action;
					//action->setStatusTip(tr(""));
					bool ok = connect(action, SIGNAL(triggered()), this, SLOT(runAction()));
					objectMenu.addAction(action);
				}
			}
		}

		objectMenu.exec(treeWidget->mapToGlobal(position));

		bool ok = disconnect(0, 0, this, SLOT(runAction()));
	

		return;
	}
	else {
		QString itemFullName = getHeaderFullName(item);
		if (itemFullName == QString("Model Hazard Earthquake Location")) {
			QAction viewAllModelsAction(tr("&View All Models on Map"), this);
			viewAllModelsAction.setStatusTip(tr("Views all the location models on the map."));
			bool ok = connect(&viewAllModelsAction, SIGNAL(triggered()), this, SLOT(viewAllLocationModelsOnMap()));
			otherMenu.addAction(&viewAllModelsAction);

			otherMenu.exec(treeWidget->mapToGlobal(position));

			ok = disconnect(&viewAllModelsAction, SIGNAL(triggered()), this, SLOT(viewAllLocationModelsOnMap()));

			otherMenu.removeAction(&viewAllModelsAction);

			return;
		}

		createMenu.exec(treeWidget->mapToGlobal(position));

	}
}

void RObjectWidget::runAction()
{
	QObject *senderObject = sender();
	QString syntax = senderObject->objectName();
	syntax.remove('&');
	QStringList tempList = syntax.split(' ', QString::SkipEmptyParts);
	syntax.clear();
	for (int i = 0; i < tempList.count(); i++) {
		syntax += tempList[i];//.replace(0, 1, tempList[i].at(0).toUpper());
	}
	syntax.replace(0, 1, syntax.at(0).toLower());
	QByteArray byteStr = syntax.toLatin1();
	const char *charStr = byteStr.data();

	bool ok = QMetaObject::invokeMethod(senderObject->parent(), charStr);
}

void RObjectWidget::expandAll()
{
	treeWidget->expandAll();
	//fitColumns();
}

void RObjectWidget::collapseAll()
{
	treeWidget->collapseAll();
	//fitColumns();
}

void RObjectWidget::expandPopulatedBranches()
{
	RMainWindow::noWarning = true;
	for (int i = 0; i < lastLevelList.count(); i++) {

		if (lastLevelList[i]->childCount() > 0) {
			if (!treeWidget->isItemExpanded(lastLevelList[i])) {
				treeWidget->expandItem(lastLevelList[i]);
			}
			QTreeWidgetItem *parent = lastLevelList[i]->parent();
			while (parent) {
				if (!treeWidget->isItemExpanded(parent)) {
					treeWidget->expandItem(parent);
				}
				parent = parent->parent();
			}
		}
	}
	RMainWindow::noWarning = false;
	//fitColumns();
}

void RObjectWidget::rankRandomVariablesByName()
{
	for (int i = 0; i < lastLevelList.count(); i++) {
		if (getHeaderFullName(lastLevelList[i]) == QString("Parameter Random Variable Continuous")) {
			lastLevelList[i]->takeChildren();
			populateBranch(lastLevelList[i]);
			break;
		}
	}
}

void RObjectWidget::rankRandomVariablesByAlphaImportance()
{
	rankRandomVariablesByImportance("Alpha");
}

void RObjectWidget::rankRandomVariablesByGammaImportance()
{
	rankRandomVariablesByImportance("Gamma");
}

void RObjectWidget::rankRandomVariablesByImportance(QString importanceMeasureType)
{
	QString type = "Parameter Random Variable Continuous";
	for (int i = 0; i < lastLevelList.count(); i++) {
		if (getHeaderFullName(lastLevelList[i]) == type) {
			lastLevelList[i]->takeChildren();

			// Sorting random variables according to ranking
			QObjectList objectList = theDomain->getObjectList(type);
			QList<RRandomVariable *> randomVariableList;
			for (int j = 0; j < objectList.count(); j++) {
				RRandomVariable *object = qobject_cast<RRandomVariable *>(objectList[j]);
				randomVariableList.append(object);
			}
			theDomain->sort(&randomVariableList, importanceMeasureType);

			for (int j = 0; j < randomVariableList.count(); j++) {
				createItem(lastLevelList[i], randomVariableList[j]->objectName());
			}
			break;
		}
	}
}

void RObjectWidget::rankDecisionVariablesByName()
{
	for (int i = 0; i < lastLevelList.count(); i++) {
		if (getHeaderFullName(lastLevelList[i]) == QString("Parameter Decision Variable Continuous")) {
			lastLevelList[i]->takeChildren();
			populateBranch(lastLevelList[i]);
			break;
		}
	}
}

void RObjectWidget::rankDecisionVariablesBySensitivity()
{
	QString type = "Parameter Decision Variable Continuous";
	for (int i = 0; i < lastLevelList.count(); i++) {
		if (getHeaderFullName(lastLevelList[i]) == type) {
			lastLevelList[i]->takeChildren();

			// Sorting decision variables according to ranking
			QObjectList objectList = theDomain->getObjectList(type);
			QList<RDecisionVariable *> decisionVariableList;
			for (int j = 0; j < objectList.count(); j++) {
				RDecisionVariable *object = qobject_cast<RDecisionVariable *>(objectList[j]);
				decisionVariableList.append(object);
			}
			theDomain->sort(&decisionVariableList, "Sensitivity");

			for (int j = 0; j < decisionVariableList.count(); j++) {
				createItem(lastLevelList[i], decisionVariableList[j]->objectName());
			}
			break;
		}
	}
}

void RObjectWidget::getCoordinatesfromAddress()
{
	QString addresses="";
	QDateTime startTime = QDateTime::currentDateTime();

	QList<RLocation *> locationList = theDomain->findChildren<RLocation *>();
	for (int i = 0; i < locationList.count(); i++) {
		QString address = locationList[i]->getAddress();
		if (address.isEmpty()) {
			RObject::rCritical() << "Warning: The location" << locationList[i]->objectName() << "does not include any address.";
		}
		else {

			addresses = addresses + locationList[i]->getAddress() + "_";
		//	locationList[i]->getCoordinatesfromAddress();	

		
		}
		// A line to keep the GUI responsive
		if (i % 10 == 0) {
			QCoreApplication::processEvents();
		}
	}

	locationList[0]->getMultipleCoordinatesfromAddress(addresses);
	QDateTime finishTime = QDateTime::currentDateTime();
	double numDays = startTime.daysTo(finishTime);
	double numMilliSecs = startTime.time().msecsTo(finishTime.time());
	double numSecs = numDays * 86400.0 + numMilliSecs / 1000.0;

	RObject::rDebug() << "##### UPDATING THE COORDINATES OF ALL LOCATIONS BASED ON ADDRESS FINISHED IN" << numSecs << "SECONDS.";
}

void RObjectWidget::viewAllLocationsOnMap()
{
	QDateTime startTime = QDateTime::currentDateTime();

	QList<RLocation *> locationList = theDomain->findChildren<RLocation *>();
	if (locationList.count() > 0) {
		locationList[0]->openMap();
	}
	else {
		return;
	}

	for (int i = 0; i < locationList.count(); i++) {
		if ((locationList[i]->getLatitude() != 0) || (locationList[i]->getLongitude() != 0)) {
			locationList[i]->redisplayonMap();

			// A line to keep the GUI responsive
			if (i % 10 == 0) {
				QCoreApplication::processEvents();
			}
		}
	}

	QDateTime finishTime = QDateTime::currentDateTime();
	double numDays = startTime.daysTo(finishTime);
	double numMilliSecs = startTime.time().msecsTo(finishTime.time());
	double numSecs = numDays * 86400.0 + numMilliSecs / 1000.0;

	RObject::rDebug() << "##### VIEWING ALL LOCATIONS ON MAP FINISHED IN" << numSecs << "SECONDS.";
}

void RObjectWidget::viewAllLocationModelsOnMap()
{
	QDateTime startTime = QDateTime::currentDateTime();
	QList<RSingleLineSourceModel *> modelList1 = theDomain->findChildren<RSingleLineSourceModel *>();
	for (int i = 0; i < modelList1.count(); i++) {
		modelList1[i]->displayonMap();
		if (i % 10 == 0) {
			QCoreApplication::processEvents();
		}
	}
	QList<RMultipointLineSourceModel *> modelList2 = theDomain->findChildren<RMultipointLineSourceModel *>();
	for (int i = 0; i < modelList2.count(); i++) {
		modelList2[i]->displayonMap();
		if (i % 10 == 0) {
			QCoreApplication::processEvents();
		}
	}
	QList<RRectangularAreaSourceModel *> modelList3 = theDomain->findChildren<RRectangularAreaSourceModel *>();
	for (int i = 0; i < modelList3.count(); i++) {
		modelList3[i]->displayonMap();
		if (i % 10 == 0) {
			QCoreApplication::processEvents();
		}
	}
	QList<RCircularAreaSourceModel *> modelList4 = theDomain->findChildren<RCircularAreaSourceModel *>();
	for (int i = 0; i < modelList4.count(); i++) {
		modelList4[i]->displayonMap();
		if (i % 10 == 0) {
			QCoreApplication::processEvents();
		}
	}
	QList<RPolygonalAreaSourceModel *> modelList5 = theDomain->findChildren<RPolygonalAreaSourceModel *>();
	for (int i = 0; i < modelList5.count(); i++) {
		modelList5[i]->displayonMap();
		if (i % 10 == 0) {
			QCoreApplication::processEvents();
		}
	}

	QDateTime finishTime = QDateTime::currentDateTime();
	double numDays = startTime.daysTo(finishTime);
	double numMilliSecs = startTime.time().msecsTo(finishTime.time());
	double numSecs = numDays * 86400.0 + numMilliSecs / 1000.0;

	RObject::rDebug() << "##### VIEWING ALL EARTHQUAKE LOCATION MODELS ON MAP FINISHED IN" << numSecs << "SECONDS.";
}

void RObjectWidget::removeAllLocationsFromMap()
{
	QDateTime startTime = QDateTime::currentDateTime();
	RMainWindow *mainWindow = qobject_cast<RMainWindow *>(theDomain->parent());
	RMapWidget *mapWidget = mainWindow->findChild<RMapWidget *>();
	if (mapWidget) {
		mapWidget->removeAllLocations();
	}

	QDateTime finishTime = QDateTime::currentDateTime();
	double numDays = startTime.daysTo(finishTime);
	double numMilliSecs = startTime.time().msecsTo(finishTime.time());
	double numSecs = numDays * 86400.0 + numMilliSecs / 1000.0;

	RObject::rDebug() << "##### REMOVING ALL LOCATIONS FROM MAP FINISHED IN" << numSecs << "SECONDS.";
}

void RObjectWidget::viewAllSectorsOnMap()
{
	QList<RRegionalLossModel *> modelList = theDomain->findChildren<RRegionalLossModel *>();

	if (modelList.count() > 0) {
		RMainWindow *mainWindow = qobject_cast<RMainWindow *>(theDomain->parent());
		QPointer<RMapWidget> mapWidget = mainWindow->findChild<RMapWidget *>();
		if (!mapWidget) {
			modelList[0]->displayonMap();
			return;
		}
	}
	else {
		return;
	}

	for (int i = 0; i < modelList.count(); i++) {
		modelList[i]->displayonMap();

		// A line to keep the GUI responsive
		if (i % 10 == 0) {
			QCoreApplication::processEvents();
		}
	}
}

void RObjectWidget::hideMessageBox() {

	QTimer::singleShot(1, mbox2, SLOT(hide()));

}