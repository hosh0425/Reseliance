// Last modified by Mojtaba on March 19, 2014

#ifndef RMainWindow_H
#define RMainWindow_H

#include <QMainWindow>
#include <QMessageBox>
#include <QPointer>

class QAction;
class QListWidget;
class QTreeWidget;
class QWidget;
class QToolBox;
class QPlainTextEdit;
class QTabBar;
class QMdiArea;
class QMdiSubWindow;
class QProgressBar;
class RMapWidget;
class RPropertyWidget;
class RObjectWidget;
class RAggregationObjectWidget;
class RParametricAnalysisDialog;
class RDomain;

class RMainWindow : public QMainWindow
{
	Q_OBJECT

public:
	RMainWindow(QWidget *parent = 0);
	~RMainWindow();

	// Static global data member, which prevents showing the Qt warnings in the output pane
	static bool noWarning;

	public slots:

	// Brings a certain pane (dock) to front
	void raiseDock(QString dockName);

	// Updates the passed object tree
	void updateTree(QString treeName);

	// Collapses the passed object tree
	void collapseTree(QString treeName);

	// Expands the passed object tree
	void expandTree(QString treeName);

	// Updates the properties pane
	void updatePropertyList();

	// Resets the domain object and deletes all objects
	void clearAllObjects(bool showDialogBox = true);

	// Opens an input file
	int openFile(QString fileName);

	// Adds a sub-window to the mdiArea
	void addSubWindow(QWidget *widget, QString windowTitle, bool tiled = false);

	// Returns a pointer to the sub-window that contains the passed widget
	QMdiSubWindow *getSubWindow(QWidget *widget);

	// Brings the sub-window of the passed widget to front
	void setActiveSubWindow(QWidget *widget);

	// Opens the map widget
	void viewMap();


	// salarifard
	void zoomIn();

	// salarifard
	void zoomOut();

	// Returns the content of the output pane (console)
	QByteArray getConsoleText();

	// Deactives actions that are not allowed to be done while there is ongoing analysis
	void deactiveActionsWhileAnalysis();

	// Reactivates actions that are not allowed to be done while there is ongoing analysis, after analysis ends
	void reactivateActionsAfterAnalysisEnded();

	// Loads the corresponding Documentation page
	void helpLoader(QString classname);

	//Hides QMessageBox when help page loads
	void hideMessageBox(); //salarifard

signals:
	// Signal emitted when a new sub-window is added to the mdiArea
	void subWindowAdded(QWidget *subWindow);

	private slots:
	// Confirms with the user before quitting Rt
	void closeEvent(QCloseEvent *event);

	// Brings the properties pane (dock) to front
	void raisePropertieDock();

	// Shows the about box
	void about();

	// Toggles the visibility of all panes
	void togglePanes();

	// Toggles the visibility of all toolbars
	void toggleToolBars();

	// Stores the size and position of all panes and toolbars in the system registry
	void writeSettings(bool defaultFlag);

	// Reads and applies the size and position of all panes and toolbars in the system registry
	void readSettings(bool defaultFlag);

	// Sets the current size and position of all panes and toolbars as default
	void setDefaultView();

	// Resets the size and position of the panes to the default setting
	void resetView();

	// Invokes the method in the RMapWidget to overlay an image file on the map
	void overlayImageOnMap();

	// Starts the operation of automatically creating location objects from user clicks on the map
	void createLocationsFromMap();

	// Opens an Rt input file
	void open();

	// Saves an Rt input file
	void save();

	// Opens a recently used Rt input file
	void openRecentFile();

	// Cuts the selected text from the output pane
	void cutText();

	// Copies the selected text from the output pane in the clipboard
	void copyText();

	// Pastes the text from the clipboard to the output pane
	void pasteText();

	// Clears the output pane
	void deleteAllText();

	// Invokes the dialog box to run an analysis
	void runAnalysis();

	// Invokes the dialog box to run a parametric analysis
	void runParametricAnalysis();

	// Invokes the dialog box to run carry out variable inference
	void variableInference();

	// Invokes the dialog box to run carry out model inference
	void modelInference();

	// Invokes the dialog box to set the max thread count that can be used in parallel analysis
	void setThreadCount();

	// Tiles the windows in the mdiArea
	void tileWindows();

	// "Cascades" the windows in the mdiArea (similar to cascading in Windows)
	void cascadeWindows();

	// Closes all windows int he mdiArea
	void closeAll();

	// Opens the Documentation inside Rt
	void help();

	// Closes the currently active sub-window
	void closeActiveSubWindow();

	// Opens an Rt input file when dragged by mouse into Rt
	void dragEnterEvent(QDragEnterEvent *event);
	void dropEvent(QDropEvent *event);

private:
	// Creates the central pane
	void createCentralWidget();

	// Creates the menubar and toolbar actions
	void createActions();

	// Creates the menubar
	void createMenus();

	// Creates the toolbars
	void createToolBars();

	// Cerates the statubar
	void createStatusBar();

	// Creates the panes (docks)
	void createDockWindows();

	// Establishes the signal-slot connection
	void createConnections();

	// Deletes the Domian object, then instantiates it again
	void resetDomain();

	// Displays a message when a map service is requested, but the map widget is not open, then open the map
	void onMapIsNotOpenError();

	// Updates the title of the main window
	void updateApplicationTitle(QString currentTitle);

	// Updates the list of the recent files
	void updateRecentFileList(QString newFileName);

	// Updates the recet files menu actions
	void updateRecentFileActions();

	// Data members to the menubar
	QMenu *fileMenu;
	QMenu *editMenu;
	QMenu *viewMenu;
	QMenu *analysisMenu;
	QMenu *windowMenu;
	QMenu *helpMenu;
	QMenu *panesMenu;
	QMenu *toolBarsMenu;
	QMenu *recentFilesMenu;

	// Datamembers for toolbars
	QToolBar *fileToolBar;
	QToolBar *editToolBar;
	QToolBar *viewToolBar;
	QToolBar *analysisToolBar;
	QToolBar *windowToolBar;
	QToolBar *helpToolBar;

	// Data members for actions
	QAction *clearAllObjectAction;
	QAction *openAction;
	QAction *saveAction;
	QAction *exitAction;
	QAction *cutAction;
	QAction *copyAction;
	QAction *pasteAction;
	QAction *deleteAllAction;
	QAction *toggleAllPanesAction;
	QAction *toggleAllToolBarsAction;
	QAction *resetViewAction;
	QAction *setDefaultViewAction;
	QAction *viewMapAction;
	QAction *overlayImageOnMapAction;
	QAction *createLocationsFromMapAction;
	QAction *runAnalysisAction;
	QAction *runParametricAnalysisAction;
	QAction *variableInferenceAction;
	QAction *modelInferenceAction;
	QAction *tileWindowsAction;
	QAction *cascadeWindowsAction;
	QAction *closeAllAction;
	QAction *helpAction;
	QAction *documentationAction;
	QAction *aboutAction;
	QAction *setThreadCountAction;
	//salarifard
	QAction *zoomInAction;

	//salarifard
	QAction *zoomOutAction;

	enum { MaxRecentFiles = 10 };
	QAction *recentFileActions[MaxRecentFiles];

	// Data members for panes (docks)
	QDockWidget *propertiesDock;
	QDockWidget *consoleDock;
	QDockWidget *domainDock;
	QDockWidget *analysisDock;
	//QDockWidget *BIMDock;
	RPropertyWidget *propertyList;
	RObjectWidget *domainTree;
	RObjectWidget *analysisTree;
	QPlainTextEdit *console;
	//RAggregationObjectWidget *BIMTree;

	// Data member for the parametric analysis dialog box
	RParametricAnalysisDialog *parametricAnalysisDialog;

	// Data member for the map widget
	QPointer<RMapWidget> theMapWidget;

	// Data member for the central pane (mdiArea)
	QMdiArea *mdiArea;

	// Data member for the progress-bar that is shown when an input file is being opened
	QProgressBar *progressBar;

	// Data member for the Domain object
	RDomain *theDomain;

	// Flag that indicates if the confirmation to quit Rt should be shown
	bool closeWithoutWarning;

};

#endif // RMainWindow_H
