// Last modified by Mojtaba on June 30, 2012

#ifndef RObjectWidget_H
#define RObjectWidget_H

#include <QWidget>
#include <QMenu>
#include <QtWidgets>
#include <QWebEngineView>
#include "ui_RObjectWidget.h"

class RDomain;

class RObjectWidget : public QWidget, public Ui::RObjectWidgetUi
{
	Q_OBJECT

public:
	RObjectWidget(QWidget *parent = 0);
	~RObjectWidget();

	// Sets the Domain object
	void setDomain(RDomain *passedDomain);

	// Expands all branches of the object tree
	void expandAll();

	// Collapses all branches of the object tree
	void collapseAll();

	// Expands only those branches of the object tree that have objects
	void expandPopulatedBranches();

	// Creates the object tree by reading the tree headers from RDomain
	void createHeader(QString name, QTreeWidgetItem *parent = 0, int parentingStatus = 0);

	public slots:
	// Updates the objects tree
	void updateView();

	// Selects an item in the tree that has the passed name
	void selectItem(QString itemName);

	// Deactivates actions that are not allowed to be done while there is ongoing analysis
	void deactiveActionsWhileAnalysis();

	// Reactivates actions that are not allowed to be done while there is ongoing analysis, after analysis ends
	void reactivateActionsAfterAnalysisEnded();

	//Hides QMessageBox when help page loads
	void hideMessageBox();

	private slots:
	// Instantiates an object upon user's click
	void addObject();

	// Removes all objects of the current branch upon user's click
	void removeAllObject();

	// Removes the selected objects upon user's click
	void removeObject();

	// Duplicates the selected objects upon user's click
	void duplicateObject();

	// Loads the corresponding Documentation page
	void LoadHelp();

	// Emits a signal that indicates that the user has changes the current selection in the objects tree
	void changeCurrent(QTreeWidgetItem *current, QTreeWidgetItem *previous);

	// Emits a signal that indicates that the user has clicked on the current selection in the objects tree
	void currentClicked(QTreeWidgetItem *item);

	// Shows the "right-click" menu
	void showPopup(const QPoint &position);

	// Runs the action that the user selects on the "right-click" menu
	void runAction();

	// Ranks random variables alphabetically
	void rankRandomVariablesByName();

	// Ranks random variables according to the Alpha importance measure
	void rankRandomVariablesByAlphaImportance();

	// Ranks random variables according to the Gamma importance measure
	void rankRandomVariablesByGammaImportance();

	// Ranks random variables alphabetically
	void rankDecisionVariablesByName();

	// Ranks random variables according to the decision sensitivity
	void rankDecisionVariablesBySensitivity();

	// Invokes the method in the location object to obtain the latitude and longitude of the location from its address
	void getCoordinatesfromAddress();

	// Displays all location objects on the map
	void viewAllLocationsOnMap();

	// Removes all location objects from the map
	void removeAllLocationsFromMap();

	// Displays all regional models on the map
	void viewAllSectorsOnMap();

	// Displays all earthquake location models on map
	void viewAllLocationModelsOnMap();

signals:
	// Signal that is emitted when the current object selection changes
	void currentObjectChanged(QString &name);

private:
	// Removes the object tree items for all objects
	void deleteItems();

	// Returns the full path of the passed tree item as a string, e.g., "Parameter Response File"
	QString getHeaderFullName(QTreeWidgetItem *item);

	// Populates the passed tree item with the objects names of its type
	void populateBranch(QTreeWidgetItem *passedItem);

	// Creates a tree item given its parent item (parent branch) and its name
	void createItem(QTreeWidgetItem *parent, QString text);

	// Auxiliary method that ranks random variables according to the passed importance measure type, which can be Alpha or Gamma
	void rankRandomVariablesByImportance(QString importanceMeasureType);

	// Pointer to the Domain object
	RDomain *theDomain;

	// Data member for the list of tree items that appear in the last level and include the objects
	QList<QTreeWidgetItem *> lastLevelList;

	// Data members for the "right-click" menu for creating objects
	QMenu createMenu;
	QAction *createAction;
	QAction *removeAllAction;
	QAction *duplicateAction;
	QAction *helpAction;

	// Data members for the "right-click" menu for removing objects and other type of object-specific actions
	QMenu objectMenu;
	QList<QPointer<QAction> > actionList;
	QAction *removeAction;

	// Data members for the "right-click" menu for actions on the branches of the tree above the last branch
	QMenu otherMenu;

	// Flag that indicates if the new user's selection is the same as previous selection
	bool sameItemClicked;

};

#endif // RObjectWidget_H
