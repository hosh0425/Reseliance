// Last modified by Mojtaba on March 19, 2014

#include "RMainWindow.h"

#include <QtWidgets>
#include "RMapWidget.h"
#include "RPropertyWidget.h"
#include "RObjectWidget.h"
#include "RAggregationObjectWidget.h"
#include "RAboutDialog.h"
#include "RAnalysisDialog.h"
#include "RDomain.h"
#include "RParameter.h"
#include "RVariableInferenceDialog.h"
#include "RModelInferenceDialog.h"
#include "RParametricAnalysisDialog.h"
#include "RAnalyzer.h"

#include "RParallelComputationManager.h"

//salarifard
#include <qtabbar.h>

bool RMainWindow::noWarning = false;
QMessageBox *mbox;

RMainWindow::RMainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	setAcceptDrops(true);

	createCentralWidget();
	createActions();
	createMenus();
	createToolBars();
	toggleToolBars();
	createStatusBar();
	createDockWindows();
	createConnections();
	readSettings(false);
	propertiesDock->raise();

	setWindowIcon(QIcon(":/RImages/icon.png"));
	setWindowTitle(QApplication::applicationName());

	theDomain = 0;
	parametricAnalysisDialog = 0;
	//projectNumber = 0;

	closeWithoutWarning = false;

}

RMainWindow::~RMainWindow()
{
}

void RMainWindow::createConnections()
{
	QList<RObjectWidget *> objectWidgetList = findChildren<RObjectWidget *>();
	for (int i = 0; i < objectWidgetList.count(); i++) {
		connect(objectWidgetList[i], SIGNAL(currentObjectChanged(QString &)), propertyList, SLOT(viewProperties(QString &)));
		connect(objectWidgetList[i], SIGNAL(currentObjectChanged(QString &)), this, SLOT(raisePropertieDock()));
	}
}

void RMainWindow::createCentralWidget()
{
	mdiArea = new QMdiArea(this);
	mdiArea->setViewMode(QMdiArea::TabbedView);
	//mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	//mdiArea->setTabShape(QTabWidget::Triangular);
	mdiArea->setDocumentMode(true);
	setCentralWidget(mdiArea);
	//centralWidget()->setMinimumSize(400, 300);

	QTabBar *tabBar = this->findChild<QTabBar *>();
	//tabBar->setMovable(true); 
	tabBar->setTabsClosable(true);

	// The following line is not needed, because by pressing the close button, the widget is automatically closed and deleted
	//bool ok = connect(tabBar, SIGNAL(tabCloseRequested(int)), this, SLOT(closeActiveSubWindow())); // 4.5 

	RMapWidget *mapwidget = new RMapWidget();
	//viewMap();
}

void RMainWindow::createActions()
{
	clearAllObjectAction = new QAction(tr("&Clear All"), this);
	//clearAllObjectAction->setIcon(QIcon(":/RImages/new.png"));
	clearAllObjectAction->setShortcut(tr("Ctrl+N"));
	clearAllObjectAction->setStatusTip(tr("Cleares all objects."));
	connect(clearAllObjectAction, SIGNAL(triggered()), this, SLOT(clearAllObjects()));

	openAction = new QAction(tr("&Open..."), this);
	//openAction->setIcon(QIcon(":/RImages/open.png"));
	openAction->setShortcut(tr("Ctrl+O"));
	openAction->setStatusTip(tr("Opens an Rt input file."));
	connect(openAction, SIGNAL(triggered()), this, SLOT(open()));

	saveAction = new QAction(tr("&Save..."), this);
	//saveAction->setIcon(QIcon(":/RImages/save.png"));
	saveAction->setShortcut(tr("Ctrl+S"));
	saveAction->setStatusTip(tr("Saves an Rt input file."));
	connect(saveAction, SIGNAL(triggered()), this, SLOT(save()));

	for (int i = 0; i < MaxRecentFiles; ++i) {
		recentFileActions[i] = new QAction(this);
		recentFileActions[i]->setVisible(false);
		connect(recentFileActions[i], SIGNAL(triggered()), this, SLOT(openRecentFile()));
	}

	exitAction = new QAction(tr("E&xit"), this);
	//exitAction->setIcon(QIcon(":/RImages/exit.png"));
	exitAction->setShortcut(tr("Ctrl+Q"));
	exitAction->setStatusTip(tr("Exits the application."));
	connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

	cutAction = new QAction(tr("&Cut Output"), this);
	//cutAction->setIcon(QIcon(":/RImages/cut.png"));
	cutAction->setShortcut(tr("Ctrl+X"));
	cutAction->setStatusTip(tr("Cuts the selected text in the output pane."));
	connect(cutAction, SIGNAL(triggered()), this, SLOT(cutText()));

	copyAction = new QAction(tr("C&opy Output"), this);
	//copyAction->setIcon(QIcon(":/RImages/copy.png"));
	copyAction->setShortcut(tr("Ctrl+C"));
	copyAction->setStatusTip(tr("Copies the selected text in the output pane."));
	connect(copyAction, SIGNAL(triggered()), this, SLOT(copyText()));

	pasteAction = new QAction(tr("&Paste in Output"), this);
	//pasteAction->setIcon(QIcon(":/RImages/paste.png"));
	pasteAction->setShortcut(tr("Ctrl+V"));
	pasteAction->setStatusTip(tr("Pastes the clipboard text in the output pane."));
	connect(pasteAction, SIGNAL(triggered()), this, SLOT(pasteText()));

	deleteAllAction = new QAction(tr("&Delete Output"), this);
	//deleteAllAction->setIcon(QIcon(":/RImages/remove.png"));
	deleteAllAction->setStatusTip(tr("Deletes the entire text in the output pane."));
	connect(deleteAllAction, SIGNAL(triggered()), this, SLOT(deleteAllText()));

	viewMapAction = new QAction(tr("View &Map"), this);
	//viewMapAction->setIcon(QIcon(":/RImages/map.png"));
	viewMapAction->setStatusTip(tr("Opens the Google Maps."));
	connect(viewMapAction, SIGNAL(triggered()), this, SLOT(viewMap()));

	//salarifard

	zoomInAction = new QAction(tr("Zoom In"), this);
	zoomInAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Plus));
	connect(zoomInAction, SIGNAL(triggered()), this, SLOT(zoomIn()));
	


	//salarifard

	zoomOutAction = new QAction(tr("Zoom Out"), this);
	zoomOutAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Minus));
	connect(zoomOutAction, SIGNAL(triggered()), this, SLOT(zoomOut()));


	overlayImageOnMapAction = new QAction(tr("&Overlay Image on Map"), this);
	overlayImageOnMapAction->setStatusTip(tr("Overlays an image on the Google Maps."));
	connect(overlayImageOnMapAction, SIGNAL(triggered()), this, SLOT(overlayImageOnMap()));

	createLocationsFromMapAction = new QAction(tr("&Create Locations from Map"), this);
	createLocationsFromMapAction->setStatusTip(tr("Creates multiple locations by clicking on the Google Maps."));
	connect(createLocationsFromMapAction, SIGNAL(triggered()), this, SLOT(createLocationsFromMap()));

	void createLocationsFromMap();
	toggleAllPanesAction = new QAction(tr("Toggle All"), this);
	//toggleAllPanesAction->setIcon(QIcon(":/RImages/showhideallpanes.png"));
	toggleAllPanesAction->setStatusTip(tr("Shows/hides all the panes."));
	connect(toggleAllPanesAction, SIGNAL(triggered()), this, SLOT(togglePanes()));

	toggleAllToolBarsAction = new QAction(tr("Toggle All"), this);
	//toggleAllToolBarsAction->setIcon(QIcon(":/RImages/showhidealltoolbars.png"));
	toggleAllToolBarsAction->setStatusTip(tr("Shows/hides all the panes."));
	connect(toggleAllToolBarsAction, SIGNAL(triggered()), this, SLOT(toggleToolBars()));

	resetViewAction = new QAction(tr("&Reset View"), this);
	//resetViewAction->setIcon(QIcon(":/RImages/resetview.png"));
	resetViewAction->setStatusTip(tr("Resets the position of menus."));
	connect(resetViewAction, SIGNAL(triggered()), this, SLOT(resetView()));

	setDefaultViewAction = new QAction(tr("&Set View As Default"), this);
	//setDefaultViewAction->setIcon(QIcon(":/RImages/setdefaultview.png"));
	setDefaultViewAction->setStatusTip(tr("Sets the current positions of menus as default."));
	connect(setDefaultViewAction, SIGNAL(triggered()), this, SLOT(setDefaultView()));

	runAnalysisAction = new QAction(tr("&Run Analysis..."), this);
	runAnalysisAction->setStatusTip(tr("Starts an analysis."));
	connect(runAnalysisAction, SIGNAL(triggered()), this, SLOT(runAnalysis()));

	runParametricAnalysisAction = new QAction(tr("Run &Parametric Analysis..."), this);
	runParametricAnalysisAction->setStatusTip(tr("Evaluates functions at a grid of parameter values."));
	connect(runParametricAnalysisAction, SIGNAL(triggered()), this, SLOT(runParametricAnalysis()));

	variableInferenceAction = new QAction(tr("&Variable Inference..."), this);
	variableInferenceAction->setStatusTip(tr("Runs a variable inference analysis."));
	connect(variableInferenceAction, SIGNAL(triggered()), this, SLOT(variableInference()));

	modelInferenceAction = new QAction(tr("&Model Inference..."), this);
	modelInferenceAction->setStatusTip(tr("Runs a model inference analysis."));
	connect(modelInferenceAction, SIGNAL(triggered()), this, SLOT(modelInference()));

	setThreadCountAction = new QAction(tr("&Set Number of Threads..."), this);
	setThreadCountAction->setStatusTip(tr("Sets the maximum number of threads that can be used in parallel analysis."));
	connect(setThreadCountAction, SIGNAL(triggered()), this, SLOT(setThreadCount()));

	tileWindowsAction = new QAction(tr("&Tile Windows"), this);
	//tileWindowsAction->setIcon(QIcon(":/RImages/tilewindows.png"));
	tileWindowsAction->setStatusTip(tr("Tiles windows."));
	connect(tileWindowsAction, SIGNAL(triggered()), this, SLOT(tileWindows()));

	cascadeWindowsAction = new QAction(tr("&Cascade Windows"), this);
	//cascadeWindowsAction->setIcon(QIcon(":/RImages/cascadewindows.png"));
	cascadeWindowsAction->setStatusTip(tr("Cascades windows."));
	connect(cascadeWindowsAction, SIGNAL(triggered()), this, SLOT(cascadeWindows()));

	closeAllAction = new QAction(tr("&Close Windows"), this);
	//closeAllAction->setIcon(QIcon(":/RImages/close.png"));
	closeAllAction->setStatusTip(tr("Closes all windows."));
	connect(closeAllAction, SIGNAL(triggered()), this, SLOT(closeAll()));

	helpAction = new QAction(tr("&Help"), this);
	//helpAction->setIcon(QIcon(":/RImages/help.png"));
	helpAction->setShortcut(tr("F1"));
	helpAction->setStatusTip(tr("Opens the Documentation for help."));
	connect(helpAction, SIGNAL(triggered()), this, SLOT(help()));

	aboutAction = new QAction(tr("&About ") + QApplication::applicationName() + "...", this);
	//aboutAction->setIcon(QIcon(":/RImages/icon.png"));
	aboutAction->setStatusTip(tr("Shows the application about box."));
	connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));
}

void RMainWindow::createMenus()
{
	fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(clearAllObjectAction);
	fileMenu->addAction(openAction);
	fileMenu->addSeparator();
	fileMenu->addAction(saveAction);
	fileMenu->addSeparator();
	recentFilesMenu = fileMenu->addMenu(tr("&Recent Files"));
	//recentFilesMenu->setIcon(QIcon(":/RImages/recentfiles.png"));
	for (int i = 0; i < MaxRecentFiles; ++i) {
		recentFilesMenu->addAction(recentFileActions[i]);
	}
	updateRecentFileActions();
	fileMenu->addSeparator();
	fileMenu->addAction(exitAction);

	editMenu = menuBar()->addMenu(tr("&Edit"));
	editMenu->addAction(cutAction);
	editMenu->addAction(copyAction);
	editMenu->addAction(pasteAction);
	editMenu->addSeparator();
	editMenu->addAction(deleteAllAction);

	viewMenu = menuBar()->addMenu(tr("&View"));
	viewMenu->addAction(viewMapAction);
	viewMenu->addAction(overlayImageOnMapAction);
	viewMenu->addAction(createLocationsFromMapAction);
	viewMenu->addSeparator();
	viewMenu->addAction(zoomInAction); //salarifard
	viewMenu->addAction(zoomOutAction); //salarifard
	

	panesMenu = viewMenu->addMenu(tr("Toggle &Panes"));
	//panesMenu->setIcon(QIcon(":/RImages/panes.png"));
	panesMenu->addAction(toggleAllPanesAction);
	panesMenu->addSeparator();
	toolBarsMenu = viewMenu->addMenu(tr("&Toggle Toolbars"));
	//toolBarsMenu->setIcon(QIcon(":/RImages/toolbars.png"));
	toolBarsMenu->addAction(toggleAllToolBarsAction);
	toolBarsMenu->addSeparator();
	viewMenu->addSeparator();
	viewMenu->addAction(resetViewAction);
	viewMenu->addAction(setDefaultViewAction);

	analysisMenu = menuBar()->addMenu(tr("&Analysis"));
	analysisMenu->addAction(runAnalysisAction);
	analysisMenu->addAction(runParametricAnalysisAction);
	analysisMenu->addSeparator();
	analysisMenu->addAction(variableInferenceAction);
	analysisMenu->addAction(modelInferenceAction);
	analysisMenu->addSeparator();
	analysisMenu->addAction(setThreadCountAction);

	windowMenu = menuBar()->addMenu(tr("&Window"));
	windowMenu->addAction(tileWindowsAction);
	windowMenu->addAction(cascadeWindowsAction);
	windowMenu->addSeparator();
	windowMenu->addAction(closeAllAction);

	menuBar()->addSeparator();

	helpMenu = menuBar()->addMenu(tr("&Help"));
	helpMenu->addAction(helpAction);
	helpMenu->addAction(aboutAction);
}

void RMainWindow::createToolBars()
{
	fileToolBar = addToolBar(tr("&File"));
	fileToolBar->setObjectName("File");
	fileToolBar->addAction(clearAllObjectAction);
	fileToolBar->addAction(openAction);
	fileToolBar->addSeparator();
	fileToolBar->addAction(saveAction);
	fileToolBar->addSeparator();
	fileToolBar->addAction(recentFilesMenu->menuAction());

	editToolBar = addToolBar(tr("&Edit"));
	editToolBar->setObjectName("Edit");
	//editToolBar->addAction(cutAction);
	//editToolBar->addAction(copyAction);
	//editToolBar->addAction(pasteAction);
	//editToolBar->addSeparator();
	editToolBar->addAction(deleteAllAction);

	viewToolBar = addToolBar(tr("&View"));
	viewToolBar->setObjectName("View");
	viewToolBar->addAction(viewMapAction);
	viewToolBar->addSeparator();
	viewToolBar->addAction(panesMenu->menuAction());
	viewToolBar->addAction(toolBarsMenu->menuAction());
	viewToolBar->addSeparator();
	viewToolBar->addAction(resetViewAction);
	viewToolBar->addAction(setDefaultViewAction);

	//salarifard
	viewToolBar->addAction(zoomInAction);

	analysisToolBar = addToolBar(tr("&Analysis"));
	analysisToolBar->setObjectName("Analysis");
	analysisToolBar->addAction(runAnalysisAction);
	analysisToolBar->addAction(runParametricAnalysisAction);
	analysisToolBar->addSeparator();
	analysisToolBar->addAction(variableInferenceAction);
	analysisToolBar->addAction(modelInferenceAction);
	analysisToolBar->addSeparator();
	analysisToolBar->addAction(setThreadCountAction);

	windowToolBar = addToolBar(tr("&Window"));
	windowToolBar->setObjectName("Window");
	windowToolBar->addAction(tileWindowsAction);
	//windowToolBar->addAction(cascadeWindowsAction);
	windowToolBar->addSeparator();
	windowToolBar->addAction(closeAllAction);

	helpToolBar = addToolBar(tr("&Help"));
	helpToolBar->setObjectName("Help");
	helpToolBar->addAction(helpAction);
	helpToolBar->addAction(aboutAction);

	QList<QToolBar *> toolBarList = findChildren<QToolBar *>();
	for (int i = 0; i < toolBarList.count(); i++) {
		toolBarsMenu->addAction(toolBarList[i]->toggleViewAction());
	}
}

void RMainWindow::closeEvent(QCloseEvent *event)
{
	if (!closeWithoutWarning) {
		int response = QMessageBox::question(this, tr("Confirmation"), tr("Are you sure you want to exit the program?"), QMessageBox::Yes | QMessageBox::Default, QMessageBox::No);
		if (response == QMessageBox::Yes) {
			writeSettings(false);
			event->accept();
		}
		else {
			event->ignore();
		}
	}
	else {
		event->accept();
	}
}

void RMainWindow::createStatusBar()
{
	statusBar()->showMessage(tr("Ready"));
	progressBar = new QProgressBar(this);
	progressBar->setMinimum(0);
	progressBar->setMaximum(100);
	statusBar()->addPermanentWidget(progressBar);
	//progressBar->resize(progressBar->width(), progressBar->height() * 3 / 4.0);
	statusBar()->removeWidget(progressBar);
}

void RMainWindow::createDockWindows()
{
	setDockOptions(AnimatedDocks | AllowTabbedDocks /*| VerticalTabs*/ | AllowNestedDocks);
	setDockNestingEnabled(true);

	setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
	setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);
	setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
	setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);

	char *domainStr = "Domain";
	domainDock = new QDockWidget(tr("Models"), this);  // Instead of the name "Domain", we use "Modelling"
	domainDock->setAllowedAreas(Qt::AllDockWidgetAreas);
	domainDock->setObjectName(domainStr);
	domainTree = new RObjectWidget(domainDock);
	domainDock->setWidget(domainTree);
	domainTree->createHeader("Domain");
	addDockWidget(Qt::LeftDockWidgetArea, domainDock);
	panesMenu->addAction(domainDock->toggleViewAction());

	char *analysisStr = "Analysis";
	analysisDock = new QDockWidget(tr("Methods"), this);
	analysisDock->setAllowedAreas(Qt::AllDockWidgetAreas);
	analysisDock->setObjectName(analysisStr);
	analysisTree = new RObjectWidget(analysisDock);
	analysisDock->setWidget(analysisTree);
	analysisTree->createHeader(analysisStr);
	addDockWidget(Qt::LeftDockWidgetArea, analysisDock);
	panesMenu->addAction(analysisDock->toggleViewAction());

	char *BIMStr = "BIM";
	//BIMDock = new QDockWidget(tr(BIMStr), this);
	//BIMDock->setAllowedAreas(Qt::AllDockWidgetAreas);
	//BIMDock->setObjectName(BIMStr);
	//BIMTree = new RAggregationObjectWidget(BIMDock);	
	//BIMDock->setWidget(BIMTree);
	////BIMTree->createHeader(BIMStr);
	//addDockWidget(Qt::LeftDockWidgetArea, BIMDock);
	//panesMenu->addAction(BIMDock->toggleViewAction());

	this->tabifyDockWidget(domainDock, analysisDock);
	//this->tabifyDockWidget(analysisDock, BIMDock);
	domainDock->raise();

	char *propertiesStr = "Properties";
	propertiesDock = new QDockWidget(tr(propertiesStr), this);
	propertiesDock->setObjectName(propertiesStr);
	propertiesDock->setAllowedAreas(Qt::AllDockWidgetAreas);
	propertyList = new RPropertyWidget(propertiesDock);
	propertiesDock->setWidget(propertyList);
	addDockWidget(Qt::BottomDockWidgetArea, propertiesDock);
	panesMenu->addAction(propertiesDock->toggleViewAction());

	char *outputStr = "Output";
	consoleDock = new QDockWidget(tr(outputStr), this);
	consoleDock->setObjectName(outputStr);
	consoleDock->setAllowedAreas(Qt::AllDockWidgetAreas);
	console = new QPlainTextEdit(consoleDock);
	console->setReadOnly(false);
	console->setLineWrapMode(QPlainTextEdit::NoWrap);
	consoleDock->setWidget(console);
	QFont consoleFont = console->font();
	//consoleFont.setFamily("Courier New");
	consoleFont.setFamily("Lucida Console");
	console->setFont(consoleFont);
	addDockWidget(Qt::BottomDockWidgetArea, consoleDock);
	panesMenu->addAction(consoleDock->toggleViewAction());

	//this->tabifyDockWidget(propertiesDock, consoleDock);
	propertiesDock->raise();
}

void RMainWindow::dragEnterEvent(QDragEnterEvent *event)
{
	if (event->mimeData()->hasUrls()) {
		event->acceptProposedAction();
	}
}

void RMainWindow::dropEvent(QDropEvent *event)
{
	if (event->mimeData()->hasUrls()) {
		QList<QUrl> urlList = event->mimeData()->urls();
		for (int i = 0; i < urlList.count(); i++) {
			openFile(urlList[i].toLocalFile());
		}
	}
}

void RMainWindow::addSubWindow(QWidget *widget, QString windowTitle, bool tiled)
{
	QMdiSubWindow *subWindow = getSubWindow(widget);
	if (!subWindow) {
		subWindow = mdiArea->addSubWindow(widget);

		//// Set the parent of the widget ot the subWindow, so that you can access the subWindow from within the widget using parentWidget() method to, e.g., resize the subwindow from within the plot widget
		//widget->setParent(subWindow);

		double width = centralWidget()->width();
		double height = centralWidget()->height();
		double factor = 0.8;
		double x = (1 - factor) * width / 2;
		double y = (1 - factor) * height / 2;
		subWindow->setGeometry(x, y, factor * width, factor * height);
		//subWindow->setMinimumSize(1, 1);
		subWindow->setAttribute(Qt::WA_DeleteOnClose);
		widget->setWindowTitle(windowTitle);
		//subWindow->setWindowTitle(windowTitle);
		if (widget->inherits("RMapWidget")) {
			subWindow->setWindowIcon(QIcon(":/RImages/map.png"));
		}
		else if (widget->inherits("RPlotWidget")) {
			subWindow->setWindowIcon(QIcon(":/RImages/chart.png"));
		}
		else if (widget->inherits("QWebView")) {
			subWindow->setWindowIcon(QIcon(":/RImages/help.png"));
		}
		else if (widget->inherits("RMainWindow"))
		{
		}
		emit subWindowAdded(widget);
	}
	subWindow->show();
	mdiArea->setActiveSubWindow(subWindow);
	QList<QMdiSubWindow *> subWindowList = mdiArea->subWindowList();
	if ((tiled) && (subWindowList.count() > 1)) {
		mdiArea->tileSubWindows();
	}
	else {
		subWindow->showMaximized();
	}
}

QMdiSubWindow * RMainWindow::getSubWindow(QWidget *widget)
{
	QList<QMdiSubWindow *> subWindowList = mdiArea->subWindowList();
	for (int i = 0; i < subWindowList.count(); i++) {
		if (subWindowList[i]->widget() == widget) {
			return subWindowList[i];
		}
	}
	return 0;
}

void RMainWindow::setActiveSubWindow(QWidget *widget)
{
	QMdiSubWindow *subWindow = getSubWindow(widget);
	if (subWindow) {
		mdiArea->setActiveSubWindow(subWindow);
	}
}

void RMainWindow::closeActiveSubWindow()
{
	QMdiSubWindow *subWindow = mdiArea->activeSubWindow();
	mdiArea->closeActiveSubWindow();
	if (subWindow) {
		delete subWindow;
	}
}

void RMainWindow::closeAll()
{
	QList<QMdiSubWindow *> subWindowList = mdiArea->subWindowList();
	if (subWindowList.count() == 0) {
		return;
	}
	int response = QMessageBox::Yes;//QMessageBox::question(this, tr("Confirmation"), tr("Are you sure you want to close all windows?"), QMessageBox::Yes | QMessageBox::Default, QMessageBox::No);
	if (response == QMessageBox::Yes) {
		while (subWindowList.count() > 0) {
			subWindowList[0]->close();
			delete subWindowList.takeAt(0);
		}
	}
}

void RMainWindow::readSettings(bool defaultFlag)
{
	QSettings settings(QApplication::organizationName(), QApplication::applicationName());
	int desktopWidth = QApplication::desktop()->width();     // returns desktop width
	int desktopHeight = QApplication::desktop()->height();    // returns desktop height
	setMinimumSize(320, 320);
	double ratio = 0.7;
	QString pathStr;
	if (defaultFlag) {
		pathStr = "view/default/";
	}
	else {
		pathStr = "view/";
	}
	//if (!(settings.contains("default/pos"))) {QTimer::singleShot(1000, this, SLOT(writeSettings(true)));}
	QPoint pos = settings.value(pathStr + "pos", QPoint(0.5*desktopWidth*(1 - ratio), 0.5*desktopHeight*(1 - ratio))).toPoint();
	QSize size = settings.value(pathStr + "size", QSize(ratio*desktopWidth, ratio*desktopHeight)).toSize();
	bool firstTime = !(settings.contains(pathStr + "maximized"));
	if (!firstTime) {
		bool maximized = settings.value(pathStr + "maximized", false).toBool();
		if (maximized == true) {
			showMaximized();
		}
		else {
			showNormal();
			move(pos);
			resize(size);
		}
	}
	else {
		showMaximized();
	}
	QByteArray state = settings.value(pathStr + "toolbars").toByteArray();
	restoreState(state, 0);
	//centralWidget()->restoreGeometry(settings.value(pathStr+"geometry").toByteArray());
}

void RMainWindow::writeSettings(bool defaultFlag)
{
	QSettings settings(QApplication::organizationName(), QApplication::applicationName());
	QString pathStr;
	if (defaultFlag) {
		pathStr = "view/default/";
	}
	else {
		pathStr = "view/";
	}
	settings.setValue(pathStr + "pos", pos());
	settings.setValue(pathStr + "size", size());
	settings.setValue(pathStr + "maximized", isMaximized());
	settings.setValue(pathStr + "toolbars", saveState(0));
	//settings.setValue(pathStr+"geometry", centralWidget()->saveGeometry());
}

void RMainWindow::resetView()
{
	readSettings(true);
}

void RMainWindow::clearAllObjects(bool showDialogBox)
{
	int response = QMessageBox::Yes;
	if (showDialogBox) {
		response = QMessageBox::question(this, tr("Confirmation"), tr("Are you sure you want to remove all objects?"), QMessageBox::Yes | QMessageBox::Default, QMessageBox::No);
	}
	if (response == QMessageBox::Yes) {
		resetDomain();
		//projectNumber++;
		//QString currentTitle = QString("Project%1").arg(projectNumber);
		//updateApplicationTitle(currentTitle);
		theDomain->setCurrentFilePath(QApplication::applicationDirPath());

		if (parametricAnalysisDialog) {
			delete parametricAnalysisDialog;
			parametricAnalysisDialog = 0;
		}
	}
}

void RMainWindow::open()
{
	// Get the file name from the user using QFileDialog
	QStringList fileNameList = QFileDialog::getOpenFileNames(this, tr("Open Rt Input Files"), QDir::currentPath(), "Text Files (*.txt);;All Files (*.*)");
	if (fileNameList.isEmpty()) {
		return;
	}

	// Open files
	for (int i = 0; i < fileNameList.count(); i++) {
		openFile(fileNameList[i]);
	}
}

int RMainWindow::openFile(QString fileName)
{
	statusBar()->addPermanentWidget(progressBar);
	progressBar->show();
	//progressBar->resize(progressBar->width(), progressBar->height() * 3 / 4.0);
	connect(theDomain, SIGNAL(fileOpenProgressed(int)), progressBar, SLOT(setValue(int)));

	// Set cursor status to wait
	QApplication::setOverrideCursor(Qt::WaitCursor);

	RMainWindow::noWarning = true;

	QObjectList objectList;
	QString errorMessage;
	int result = theDomain->openFile(fileName, &objectList, &errorMessage);
	if (result < 0) {
		QApplication::restoreOverrideCursor();
		QMessageBox::critical(this, tr("Error"), errorMessage, QMessageBox::Ok | QMessageBox::Default, QMessageBox::Ok);
		statusBar()->removeWidget(progressBar);
		statusBar()->showMessage("File open operation failed.", 2000);
		RMainWindow::noWarning = false;
	}

	RMainWindow::noWarning = false;

	disconnect(theDomain, SIGNAL(fileOpenProgressed(int)), progressBar, SLOT(setValue(int)));
	statusBar()->removeWidget(progressBar);
	statusBar()->showMessage("File opened.", 2000);

	updateRecentFileList(fileName);

	// Refresh the object trees and expand them
	domainTree->updateView();
	analysisTree->updateView();
	domainTree->expandPopulatedBranches();
	analysisTree->expandPopulatedBranches();

	// Restore the cursor to
	QApplication::restoreOverrideCursor();

	return 0;
}

void RMainWindow::updateApplicationTitle(QString currentFileName)
{
	QString windowTitle = currentFileName + " - " + QApplication::applicationName();
	setWindowTitle(windowTitle);
}

void RMainWindow::resetDomain()
{
	//Things that should be done in case of "New" or "Open" file
	if (theDomain) {
		delete theDomain;
		theDomain = 0;
	}
	QString name = "Domain";
	theDomain = new RDomain(this, name);

	connect(theDomain, SIGNAL(newAnalysisStartedSignal(void)), this, SLOT(deactiveActionsWhileAnalysis(void)));
	connect(theDomain, SIGNAL(allAnalysisEndedSignal(void)), this, SLOT(reactivateActionsAfterAnalysisEnded(void)));

	propertyList->setDomain(theDomain);
	analysisTree->setDomain(theDomain);
	domainTree->setDomain(theDomain);
	if (theMapWidget) {
		theMapWidget->setDomain(theDomain);
	}
	//BIMTree->setDomain(theDomain);

	analysisTree->updateView();
	domainTree->updateView();
	//BIMTree->updateView();
	propertyList->removeAll();
}

void RMainWindow::viewMap()
{
	if (!theMapWidget) {
		theMapWidget = new RMapWidget(this);
		theMapWidget->setDomain(theDomain);
	}

	QWebEngineView *v_view = theMapWidget->v_view;
	QWebEnginePage *page = theMapWidget->page;

	addSubWindow(v_view, "Map");
	theMapWidget->openMap();
}

//salarifard
void RMainWindow::zoomIn()
{
	
	QList<QMdiSubWindow *> subWindowList = mdiArea->subWindowList();
	for (int i = 0; i < subWindowList.count(); i++) {
		
		QString str = subWindowList[i]->widget()->windowTitle();
		if (str.contains("Help")) {

			QWebEngineView *view = (QWebEngineView*)subWindowList[i]->widget();

			view->setZoomFactor(view->zoomFactor() + 0.1);

		}

		
	}

	
}


//salarifard
void RMainWindow::zoomOut()
{

	QList<QMdiSubWindow *> subWindowList = mdiArea->subWindowList();
	for (int i = 0; i < subWindowList.count(); i++) {

		QString str = subWindowList[i]->widget()->windowTitle();
		if (str.contains("Help")) {

			QWebEngineView *view = (QWebEngineView*)subWindowList[i]->widget();

			view->setZoomFactor(view->zoomFactor() - 0.1);

		}


	}


}

void RMainWindow::save()
{
	// Get the file name from the user using QFileDialog
	QString fileName = QFileDialog::getSaveFileName(this, tr("Save an Rt Input File"), QDir::currentPath(), "Text Files (*.txt)");
	if (fileName.isEmpty()) {
		return;
	}

	if (QFileInfo(fileName).suffix().toLower() != QString("txt").toLower()) {
		fileName += ".txt";
	}

	// Open the file
	QFile file(fileName);
	if (!file.open(QFile::WriteOnly | QFile::Text)) {
		// Error
		QMessageBox::critical(this, tr("Error"), QString("Could not create the file %1.").arg(fileName), QMessageBox::Ok | QMessageBox::Default);
		return;
	}
	// Set cursor status to wait
	QApplication::setOverrideCursor(Qt::WaitCursor);

	// Save the file
	QObjectList objectList = theDomain->findChildren<QObject *>();

	int count = objectList.count();
	statusBar()->addPermanentWidget(progressBar);
	progressBar->show();
	//progressBar->resize(progressBar->width(), progressBar->height() * 3 / 4.0);

	for (int i = 0; i < count; i++) {
		progressBar->setValue(i / count * (progressBar->maximum() - progressBar->minimum()));

		// A line to keep the GUI responsive
		QCoreApplication::processEvents();

		QObject *object = objectList[i];

		RObject *rObject = qobject_cast<RObject *>(object);
		if (!rObject) {
			qCritical() << "Error in saving the file: One of the objects could not be saved.";
			continue;
		}
		if (rObject->isAutoGenerated()) {
			continue;
		}

		QString string = object->metaObject()->className();

		for (int i = 0; i < object->metaObject()->propertyCount(); i++) {
			QMetaProperty metaProperty = object->metaObject()->property(i);
			QString propertyName = metaProperty.name();
			QString propertyValue = theDomain->getProperty(object, propertyName);

			if (/*(propertyName == QString("Type")) || */(propertyValue.isEmpty()) || !metaProperty.isWritable()) {
				continue;
			}

			string += " |" + (propertyList->propertyToName(propertyName)).remove(' ') + ": " + propertyValue;
		}

		QList<QByteArray> dynamicPropertyList = object->dynamicPropertyNames();
		for (int i = 0; i < dynamicPropertyList.count(); i++) {
			QString propertyName = QString(dynamicPropertyList[i]);
			QString propertyValue = theDomain->getProperty(object, propertyName);

			string += " |" + (propertyList->propertyToName(propertyName)).remove(' ') + ": " + propertyValue;
		}

		string += "\n";

		QByteArray byteArray;
		byteArray.append(string);

		file.write(byteArray);
		file.flush();
	}

	// Close the file
	file.close();

	updateRecentFileList(fileName);

	// Update the file path of the application
	theDomain->setCurrentFilePath(RDomain::getAbsoluteFolderPath(fileName));

	// Restore the cursor to
	QApplication::restoreOverrideCursor();

	statusBar()->removeWidget(progressBar);
	statusBar()->showMessage("File saved.", 2000);
}

void RMainWindow::about()
{
	RAboutDialog *dialog = new RAboutDialog(this);
	if (dialog->exec()) {

	}
	delete dialog;
}

void RMainWindow::togglePanes()
{
	bool hideFlag = true;
	QList<QDockWidget *> dockWidgetsList = findChildren<QDockWidget *>();
	for (int i = 0; i < dockWidgetsList.count(); i++) {
		if (dockWidgetsList[i]->isHidden()) {
			hideFlag = false;
		}
	}
	for (int i = 0; i < dockWidgetsList.count(); i++) {
		dockWidgetsList[i]->setHidden(hideFlag);
	}
}

void RMainWindow::toggleToolBars()
{
	bool hideFlag = true;
	QList<QToolBar *> toolBarList = findChildren<QToolBar *>();
	for (int i = 0; i < toolBarList.count(); i++) {
		if (toolBarList[i]->isHidden()) {
			hideFlag = false;
		}
	}
	for (int i = 0; i < toolBarList.count(); i++) {
		toolBarList[i]->setHidden(hideFlag);
	}
}

void RMainWindow::setDefaultView()
{
	writeSettings(true);
}

void RMainWindow::onMapIsNotOpenError()
{
	viewMap();
	QMessageBox::information(0, tr("Information"), tr("The map is not open. Rt will now open the map. Please try again when the loading of the map is complete"), QMessageBox::Ok | QMessageBox::Default);
}

void RMainWindow::overlayImageOnMap()
{
	if (!theMapWidget) {
		onMapIsNotOpenError();
		return;
	}

	// Get the image url from the user using QInputDialog : salarifard

	bool ok;
	QString fileName = QInputDialog::getText(this, tr("Address Box"), tr("Please enter the image url here"), QLineEdit::Normal, "", &ok);

	//QString fileName = QFileDialog::getOpenFileName(this, tr("Open an Image"), QDir::currentPath(), "Images (*.jpg *.png *.gif);;All Files (*.*)");
	if (!ok || fileName.isEmpty()) {
		return;
	}
	theMapWidget->overlayImageOnMap(fileName);
}

void RMainWindow::createLocationsFromMap()
{
	if (!theMapWidget) {
		onMapIsNotOpenError();
		return;
	}

	bool ok;
	QString text = QInputDialog::getText(this, tr("Enter an object name"), tr("Hint: Right click on the map in the end to exit the point creation mode."), QLineEdit::Normal, "Point", &ok);
	if (ok && !text.isEmpty()) {
		if (theDomain->nameIsValid(text, false) == 0) {
			theMapWidget->triggerCreatingMultipleLocations(text);
		}
	}
}

void RMainWindow::raiseDock(QString dockName)
{
	QList<QDockWidget *> dockWidgetlist = this->findChildren<QDockWidget *>();
	for (int i = 0; i < dockWidgetlist.count(); i++) {
		if (dockWidgetlist[i]->objectName() == dockName) {
			dockWidgetlist[i]->raise();
			return;
		}
	}
}

void RMainWindow::raisePropertieDock()
{
	propertiesDock->raise();
}

void RMainWindow::updateTree(QString treeName)
{
	if (treeName == QString("Analysis")) {
		analysisTree->updateView();
	}
	else if (treeName == QString("Domain")) {
		domainTree->updateView();
	}
}

void RMainWindow::collapseTree(QString treeName)
{
	if (treeName == QString("Analysis")) {
		analysisTree->collapseAll();
	}
	else if (treeName == QString("Domain")) {
		domainTree->collapseAll();
	}
}

void RMainWindow::expandTree(QString treeName)
{
	if (treeName == QString("Analysis")) {
		analysisTree->expandAll();
	}
	else if (treeName == QString("Domain")) {
		domainTree->expandAll();
	}
}

void RMainWindow::updatePropertyList()
{
	propertyList->updateView();
}

void RMainWindow::runAnalysis()
{
	RAnalysisDialog *dialog = new RAnalysisDialog(this, theDomain);
	if (dialog->exec()) {

	}
	delete dialog;
}

void RMainWindow::runParametricAnalysis()
{
	if (!parametricAnalysisDialog) {
		parametricAnalysisDialog = new RParametricAnalysisDialog(this, theDomain);
	}
	else {
		parametricAnalysisDialog->setDomain(theDomain);
	}
	if (parametricAnalysisDialog->exec()) {

	}
}

void RMainWindow::variableInference()
{
	RVariableInferenceDialog *dialog = new RVariableInferenceDialog(this, theDomain);
	if (dialog->exec()) {

	}
	delete dialog;
}

void RMainWindow::modelInference()
{
	RModelInferenceDialog *dialog = new RModelInferenceDialog(this, theDomain);
	if (dialog->exec()) {

	}
	delete dialog;
}

void RMainWindow::setThreadCount()
{

	// asking the new thread count
	bool ok; //salarifard
	int threadCount = QInputDialog::getInt(this, tr("Number of Threads"), tr("Enter the number of threads; enter 0 for sequetial analysis"), RParallelComputationManager::getInstance()->getMaxThreadCount(), 0, 2147483647, 1, &ok); //salarifard

	// checking the status of changing threadCount
	bool canChange = RParallelComputationManager::getInstance()->canChangeThreadCount(threadCount);

	if (!canChange ) { //salarifard
		
		QMessageBox::warning(this, tr("Information"), tr("The number of threads can only be set when no pobjects are created. You may clear all object or restart Rt in order to set the number of threads."));
	}
	else {
		int response = QMessageBox::No;
		if (ok)
		response = QMessageBox::question(this, tr("Confirmation"), tr("You are changing the number of threads. Are you sure you want to continue?"), QMessageBox::Yes, QMessageBox::No  | QMessageBox::Default);

		if (response == QMessageBox::Yes){
			RParallelComputationManager::getInstance()->setMaxThreadCount(threadCount);
		}
	}
}

void RMainWindow::tileWindows()
{
	mdiArea->tileSubWindows();
}

void RMainWindow::cascadeWindows()
{
	mdiArea->cascadeSubWindows();
}
void RMainWindow::cutText()
{
	console->cut();
}

void RMainWindow::copyText()
{
	console->copy();
}

void RMainWindow::pasteText()
{
	console->paste();
}

void RMainWindow::deleteAllText()
{
	console->clear();
}

void RMainWindow::help()
{

	QWebEngineView *v_view = new QWebEngineView(this);
	QWebEnginePage *page = v_view->page();

	QToolBar *toolBar = addToolBar(tr("Navigation"));
	toolBar->addAction(v_view->pageAction(QWebEnginePage::Back));
	toolBar->addAction(v_view->pageAction(QWebEnginePage::Forward));
	toolBar->addAction(v_view->pageAction(QWebEnginePage::Reload));
	toolBar->addAction(v_view->pageAction(QWebEnginePage::Stop));


	QString documentationUrl = QCoreApplication::applicationDirPath() + "/../../../Help/HomePage.html";

	QString documentationUrl_final = QCoreApplication::applicationDirPath() + "/Help/HomePage.html";
/*#ifndef _DEBUG
	documentationUrl = QCoreApplication::applicationDirPath() + "/Help/HomePage.html";
#endif */

	if (QFileInfo(documentationUrl).exists())
		page->load(QUrl((documentationUrl)));
	else 
		page->load(QUrl((documentationUrl_final)));


	mbox = new QMessageBox;
	mbox->setWindowTitle(tr("Information"));
	mbox->setText("The help page is loading, please wait.");
	mbox->setStandardButtons(0);
	mbox->show();
	connect(page, &QWebEnginePage::loadFinished, this, &RMainWindow::hideMessageBox);
	addSubWindow(v_view, "Help Browser");



#ifndef WEB_PROTOTYPE
	QWebSettings *settings = QWebSettings::globalSettings();
	settings->setAttribute(QWebSettings::PluginsEnabled, true);

	QWebView *helpPage = new QWebView(this);
	helpPage->setUrl(QUrl("http://www.inrisk.ubc.ca/software/rt/"));
	addSubWindow(helpPage, "Help on InRisk Website");
#endif // WEB_PROTOTYPE
}

void RMainWindow::hideMessageBox() {
		
	QTimer::singleShot(1, mbox, SLOT(hide()));	

}

void RMainWindow::helpLoader(QString classname)
{

	QWebEngineView *v_view = new QWebEngineView();
	QWebEnginePage *page = v_view->page();

	//QToolBar *toolBar = addToolBar(tr("Navigation"));
	//toolBar->addAction(v_view->pageAction(QWebEnginePage::Back));
	//toolBar->addAction(v_view->pageAction(QWebEnginePage::Forward));
	//toolBar->addAction(v_view->pageAction(QWebEnginePage::Reload));
	//toolBar->addAction(v_view->pageAction(QWebEnginePage::Stop));

	QString documentationUrl = QCoreApplication::applicationDirPath() + "/../../../Help/" + classname + ".html";
	QString documentationUrl_final = QCoreApplication::applicationDirPath() + "/../../../Help/" + classname + ".html";

/*#ifndef _DEBUG
	documentationUrl = QCoreApplication::applicationDirPath() + "/Help/HomePage.html";
#endif*/

	if(QFileInfo(documentationUrl).exists())
		page->load(QUrl((documentationUrl)));
	else
		page->load(QUrl((documentationUrl_final)));

	addSubWindow(v_view, classname + " Help Browser");

	

}

void RMainWindow::updateRecentFileActions()
{
	QSettings settings(QApplication::organizationName(), QApplication::applicationName());
	QStringList files = settings.value("recentFiles/recentFileList").toStringList();

	int numRecentFiles = qMin(files.size(), (int)MaxRecentFiles);

	for (int i = 0; i < numRecentFiles; ++i) {
		QString text = tr("&%1 %2").arg(i + 1).arg(QFileInfo(files[i]).fileName());
		recentFileActions[i]->setText(text);
		recentFileActions[i]->setData(files[i]);
		recentFileActions[i]->setVisible(true);
	}
	for (int i = numRecentFiles; i < MaxRecentFiles; ++i) {
		recentFileActions[i]->setVisible(false);
	}
}

void RMainWindow::updateRecentFileList(QString newFileName)
{
	QString fileName = RDomain::stringToFilePath(newFileName);

	QSettings settings(QApplication::organizationName(), QApplication::applicationName());
	QString key = "recentFiles/recentFileList";
	QStringList files = settings.value(key).toStringList();
	files.removeAll(newFileName);
	files.prepend(newFileName);
	while (files.size() > MaxRecentFiles) {
		files.removeLast();
	}

	settings.setValue(key, files);
	updateRecentFileActions();
}

void RMainWindow::openRecentFile()
{
	QAction *action = qobject_cast<QAction *>(sender());
	if (action) {
		openFile(action->data().toString());
	}
}

QByteArray RMainWindow::getConsoleText()
{
	return console->toPlainText().toLatin1();
}

void RMainWindow::deactiveActionsWhileAnalysis() {

	clearAllObjectAction->setDisabled(true);
	runAnalysisAction->setDisabled(true);
	runParametricAnalysisAction->setDisabled(true);
	variableInferenceAction->setDisabled(true);
	modelInferenceAction->setDisabled(true);
	setThreadCountAction->setDisabled(true);

	// updating property pane
	updatePropertyList();
}

void RMainWindow::reactivateActionsAfterAnalysisEnded() {

	clearAllObjectAction->setEnabled(true);
	runAnalysisAction->setEnabled(true);
	runParametricAnalysisAction->setEnabled(true);
	variableInferenceAction->setEnabled(true);
	modelInferenceAction->setEnabled(true);
	setThreadCountAction->setEnabled(true);

	// updating property pane
	updatePropertyList();
}