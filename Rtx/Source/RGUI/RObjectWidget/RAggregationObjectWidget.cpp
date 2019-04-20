#include "RAggregationObjectWidget.h"

#include "RDomain.h"
#include "RModel.h"

#include "RResponse.h"

#include <QHeaderView>
#include <QMessageBox>

RAggregationObjectWidget::RAggregationObjectWidget(QWidget *parent)
	: QWidget(parent)
{
	setupUi(this);

	theDomain = 0;

	treeWidget->headerItem()->setText(0, "Name");
	treeWidget->sortItems(0, Qt::AscendingOrder);

	bool ok = connect(treeWidget, SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)), this, SLOT(changeCurrent(QTreeWidgetItem *, QTreeWidgetItem *)));
	ok = connect(treeWidget, SIGNAL(itemClicked(QTreeWidgetItem *, int)), this, SLOT(currentClicked(QTreeWidgetItem *)));
}

RAggregationObjectWidget::~RAggregationObjectWidget()
{

}

void RAggregationObjectWidget::setDomain(RDomain *passedDomain)
{
	theDomain = passedDomain;
}

void RAggregationObjectWidget::createHeader(QString name, QTreeWidgetItem *parent)
{
	QTreeWidgetItem *item;

	if (parent == 0) {
		item = new QTreeWidgetItem(treeWidget);
	} else {
		item = new QTreeWidgetItem(parent);
	}
	item->setText(0, name);

	QObject *object = theDomain->findChild<QObject *>(name);
	QObjectList tempObjectList = object->findChildren<QObject *>();

	QStringList objectNameList;
	for (int i = 0; i < tempObjectList.count(); i++) {
		RResponse *response = qobject_cast<RResponse *>(tempObjectList[i]);
		if ((!response) && (tempObjectList[i]->parent() == object)) {
			objectNameList << tempObjectList[i]->objectName();
		}
	}
	
	for (int i = 0; i < objectNameList.count(); i++) {
		createHeader(objectNameList[i], item);
	}
	//fitColumns();
}

void RAggregationObjectWidget::updateView()
{
	//////Temporary
	//QList<RModel *> modelList = theDomain->findChildren<RModel *>();
	//for (int i = 0; i < modelList.count(); i++) {
	//	createHeader(modelList[i]->objectName());
	//}
}

void RAggregationObjectWidget::deleteItems()
{
	treeWidget->clear();
}

//void RAggregationObjectWidget::fitColumns()
//{
//	for (int i = 0; i < treeWidget->columnCount(); i++) { 
//		// Fitting the column width to the data
//		treeWidget->resizeColumnToContents(i);		
//	}
//	// Increasing the width of the first column by 10%
//	treeWidget->header()->resizeSection(0, treeWidget->header()->sectionSize(0)*1.1);
//}

void RAggregationObjectWidget::selectItem(QString itemName)
{
	treeWidget->setFocus();
	treeWidget->clearSelection();
	QList<QTreeWidgetItem *> itemList = treeWidget->findItems(itemName, Qt::MatchExactly, 0);
	if (!itemList.isEmpty()) {
		itemList[0]->setSelected(true);
	}
}

void RAggregationObjectWidget::changeCurrent(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
	if (current) {
		QString objectName = current->text(0);
		emit currentObjectChanged(objectName);
	}	
}

void RAggregationObjectWidget::currentClicked(QTreeWidgetItem *item)
{
	changeCurrent(item, 0);
}

void RAggregationObjectWidget::expandAll()
{
	treeWidget->expandAll();
	//fitColumns();
}

void RAggregationObjectWidget::collapseAll()
{
	treeWidget->collapseAll();
	//fitColumns();
}
