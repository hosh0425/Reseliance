// Last modified by Mojtaba on March 19, 2014

#include "RMainWindow.h"
#include "RDomain.h"

#include <QtGui>
#include <QApplication>
#include <QCoreApplication>
#include <QGuiApplication>
#include <QPointer>
#include <QtGui>
#include <QDir>
#include <QGuiApplication>
#include <QCoreApplication>
#include <QDesktopWidget>
#include <QtCore>
#include <QDebug>
#include <QTextStream>
#include <QSplashScreen>
#include <QScreen>
#include <gsl/gsl_errno.h>
#include "qplaintextedit.h"
#include "RParallelComputationManager.h"
#include "ROutputManager.h"

#ifdef Q_OS_WIN32
#include "Windows.h" 
#endif // Q_OS_WIN32

QPointer<QPlainTextEdit> console;
QByteArray *outputByteArray = 0;

// Custom message handler, which redirects standard output and standard error channels to the output pane
void myGUIMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
	if (!console) {
		return;
	}
	QString msgStr; 
	switch (type) {
	case QtDebugMsg:
		console->appendPlainText(msg);
		break;
	case QtWarningMsg:
		// Warnings are turned off
		/* msgStr = QString("<font color=%1>").arg("blue") + QString(msg) + QString("</font>") + QString("<font color=%1>").arg("black") + QString("&nbsp;") + QString("</font>");
		if (!RMainWindow::noWarning) {
		console->appendHtml(msgStr);
		}*/
		return;
		break;
	case QtCriticalMsg:
		msgStr = QString("<font color=%1>").arg("red") + QString(msg) + QString("</font>") + QString("<font color=%1>").arg("black") + QString("&nbsp;") + QString("</font>");
		console->appendHtml(msgStr);

		break;
	case QtFatalMsg:
		msgStr = QString("<font color=%1>").arg("green") + QString(msg) + QString("</font>") + QString("<font color=%1>").arg("black") + QString("&nbsp;") + QString("</font>");
		console->appendHtml(msgStr);
		abort();
	}
	console->ensureCursorVisible();
	//console->activateWindow();
}

// Custom message handler, which stores standard output and standard error channels in the outputByteArray
void myConsoleMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
	QString msgStr; 
	switch (type) {
		case QtDebugMsg:
			outputByteArray->append(msg);
			fprintf(stdout, "%s\n", msg.toLatin1().data()); //salarifard: right coding on console
			break;
		case QtWarningMsg:
			// Warnings are turned off
			/*outputByteArray->append(msg);
			fprintf(stdout, "%s\n", msg);*/
			break;
		case QtCriticalMsg:
			outputByteArray->append(msg);
			fprintf(stdout, "%s\n", msg.toLatin1().data());//salarifard: right coding on console
			break;
		case QtFatalMsg:
			outputByteArray->append(msg);
			fprintf(stdout, "%s\n", msg.toLatin1().data());//salarifard: right coding on console
			abort();
	}
	outputByteArray->append("\n");
}

// searches argv for ThreadCount=Number, sets the number of maximum threads and removes the argument from argv
int setMaxThreadCount(int *argc , char ** argv[]){

	// a flag which is set to true when argument of threadCount=0 is met
	bool hasThreadCountArg = false;

	for(int i = 0 ; i < *argc ; i++) {

		// checking if the number of threads is given : syntax : ThreadCount=NUM
		if (QString((*argv)[i]).simplified().replace(" " , "").toLower().contains("threadcount")) {

			hasThreadCountArg = true;

			QStringList tokens = QString((*argv)[i]).split("=");

			// checking length to be 2
			if (tokens.size() != 2){
				qDebug() << "Valid syntax for setting the number of threads is : ThreadCount=NUMBER.";
				return -1;
			}

			// converting the given number to int
			bool ok = true;
			int threadCount = tokens[1].toInt(&ok);
			if (ok)
				RParallelComputationManager::getInstance()->setMaxThreadCount(threadCount);
			else{
				qDebug() << "ThreadCount can only be an integer number.";
				return -1;
			}

			break;
		}
	}

	// if has threadCount argument, removing it from argv
	if (hasThreadCountArg){

		char ** argv2 = new char * [*argc - 1];
		int index = 0;

		for (int i = 0 ; i < *argc ; i++)
			if (!QString((*argv)[i]).toLower().contains("threadcount"))
				argv2[index++] = (*argv)[i];

		*argv = argv2;
		*argc = *argc - 1;
	}

	return 0;
}

// Application instantiation
QCoreApplication* createApplication(int &argc, char *argv[], bool isConsole)
{
	for (int i = 0; i < argc; ++i) {
		if (!qstrcmp(argv[i], "nogui")) //salarifard typo mistake
		{
			qInstallMessageHandler(myConsoleMessageOutput);
			return new QCoreApplication(argc, argv, !isConsole);

		}
	}
	qInstallMessageHandler(myGUIMessageOutput);
	QApplication *theapp = new QApplication(argc, argv, !isConsole);
	theapp->setQuitOnLastWindowClosed(true);
	return theapp;
}
// main() method
int main(int argc, char *argv[])
{

	// creating output manager and parallel computation manager instances
	RParallelComputationManager::createInstance();
	ROutputManager::createInstance();

	bool isConsole = false;

	// setting the number of threads if it is given in input argument
	if (setMaxThreadCount(&argc,&argv) < 0)
		return -1;

	for(int i=0; i<argc; i++) {
		if (QString(argv[i]).toLower() == QString("nogui")) {
			isConsole = true;
		}
	}

	// Hide the console in case Rt is run in Windows
	#ifdef Q_OS_WIN32
		if (!isConsole) {
			FreeConsole();
		}
	#endif // Q_OS_WIN32

	// Declare the Rt application
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QScopedPointer<QCoreApplication> theApplication(createApplication(argc, argv, isConsole));
	bool hasGuiApp = (qobject_cast<QApplication*>(QCoreApplication::instance()) != 0);
	theApplication->setAttribute(Qt::AA_EnableHighDpiScaling);

	// Set the name and version of the Rt application
	theApplication->setApplicationName("Rtx");
	theApplication->setApplicationVersion("2.0.0");
	theApplication->setOrganizationName("SUT");
	theApplication->setOrganizationDomain("http://rtx.civil.sharif.edu");

	// Turn off the default GSL error handler that aborts the program when an error occurs
	// NOTE: Now that the default error handler is turned off, all return values of GSL should be checked. This is yet to be done. Consult the GSL manual error handling section for more information.
	gsl_set_error_handler_off();

	QString currentPath = QDir::currentPath();

	if (!isConsole) {
		// Screen size
		//int desktopWidth = QApplication::desktop()->width();     // returns desktop width
		//int desktopHeight = QApplication::desktop()->height();    // returns desktop height

		// Add the splash screen
		Qt::Alignment bottomLeft = Qt::AlignBottom | Qt::AlignLeft;
		QSplashScreen theSplash;
		//theSplash.setPixmap(QPixmap(":/RImages/splash.png").scaled(desktopWidth / 2, desktopHeight / 2));
		QPixmap pixmap = QPixmap(":/RImages/splash2.png");
		//qreal scaleRatio = (qreal)QApplication::desktop()->physicalDpiX() / 96.0;
		pixmap.setDevicePixelRatio(2);
		theSplash.setPixmap(pixmap);
		theSplash.setWindowFlags(theSplash.windowFlags() | Qt::WindowStaysOnTopHint);
		theSplash.show();
		theSplash.showMessage(QObject::tr("Setting up the main window..."), bottomLeft, Qt::white);

		// Declare the main window
		RMainWindow theMainWindow;

		// Install the custom message handler that is defined above
		console = theMainWindow.findChild<QPlainTextEdit *>();

		theMainWindow.show();

		// Establish signal-slot connections
		theSplash.showMessage(QObject::tr("Establishing connections..."), bottomLeft, Qt::white);

		theSplash.showMessage(QObject::tr(""), bottomLeft, Qt::white);

		// Close the splash screen
		//QTimer::singleShot(4000, &theSplash, SLOT(close()));	// After 4 seconds
		theSplash.finish(&theMainWindow);						// When the main window is loaded

		// Open the input files that are passed in argv
		if (argc == 1) {
			theMainWindow.clearAllObjects(false);
		} else if (argc >= 2) {
			theMainWindow.clearAllObjects(false);
			for (int i = 1; i < argc; i++) {
				QString fileName = argv[i];
				QFileInfo info(fileName);
				if (info.isRelative()) {
					fileName = currentPath + "/" + fileName;
				}
				theMainWindow.openFile(fileName);
			}
		}
	
		// Run the Rt application
		return theApplication->exec();
	} else {	
		if (argc <= 2) {
			qDebug() << "Error: Please indicate an input file as an argument";
			return 0;
		} else {
			QString name = "Domain";
			RDomain theDomain(0, name);	
			theDomain.setCurrentFilePath(QApplication::applicationDirPath());
			
			// Install the custom message handler that is defined above
			outputByteArray = theDomain.getOutputByteArray();

			for (int i = 1; i < argc; i++) {
				if (QString(argv[i]).toLower() != QString("nogui")) {
					QObjectList objectList;
					QString errorMessage;
					QString fileName = argv[i];
					QFileInfo info(fileName);
					if (info.isRelative()) {
						fileName = currentPath + "/" + fileName;
					}
					qDebug() << "Opening input file" << fileName << "...";
					int result = theDomain.openFile(fileName, &objectList, &errorMessage);
					/*if (result < 0) {
						
					}*/
				}
			}
		}

		// Exit Rt whether or not the user has asked for this at the end of the analysis
		exit(0);

		// Run the Rt application
		return theApplication->exec();
	}
}

