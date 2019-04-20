// Last modified by Mojtaba on June 30, 2012

#ifndef RAggregationObjectWidget_H
#define RAggregationObjectWidget_H

#include <QWidget>
#include <QMenu>
#include "ui_RObjectWidget.h"

class RDomain;

class RAggregationObjectWidget : public QWidget, public Ui::RObjectWidgetUi
{
	Q_OBJECT

public:
	RAggregationObjectWidget(QWidget *parent = 0);
	~RAggregationObjectWidget();

	// Sets the Domain object
	void setDomain(RDomain *passedDomain);

	// Creates the object tree by reading the tree headers from RDomain
	void createHeader(QString name, QTreeWidgetItem *parent = 0);

	// Expands all branches of the object tree
	void expandAll();

	// Collapses all branches of the object tree
	void collapseAll();

public slots:
	// Updates the objects tree
	void updateView();

	// Selects an item in the tree that has the passed name
	void selectItem(QString itemName);

private slots:
	// Emits a signal that indicates that the user has changes the current selection in the objects tree
	void changeCurrent(QTreeWidgetItem *current, QTreeWidgetItem *previous);

	// Emits a signal that indicates that the user has clicked on the current selection in the objects tree
	void currentClicked(QTreeWidgetItem *item);

signals:
	// Signal that is emitted when the current object selection changes
	void currentObjectChanged(QString &name);

private:
	// Removes the object tree items for all objects
	void deleteItems();

	// Pointer to the Domain object
	RDomain *theDomain;

};

#endif // RAggregationObjectWidget_H
