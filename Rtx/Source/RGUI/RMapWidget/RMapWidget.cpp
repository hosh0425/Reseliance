#include "RMapWidget.h"

#include "RDomain.h"
#include "RLocation.h"
#include "RMainWindow.h"

#include <QtGui>
#include <QApplication>
//#include <QtWebKit>
//#include <QWebSettings>
#include <QFile>
#include <QTextStream>

//#include <QWebFrame>


RMapWidget::RMapWidget(QWidget *parent)
    : QWidget(parent)
{
    v_view = new QWebEngineView(this);
    signalLineEdit = new QLineEdit();
	theMapURL = QCoreApplication::applicationDirPath() + "/rtmap.html"; //salarifard


   

}

RMapWidget::~RMapWidget()
{

}


// WEBTEMP
void RMapWidget::setDomain(RDomain *passedDomain)
{
    theDomain = passedDomain;
}

void RMapWidget::setURL()
{
    v_view->setUrl(QUrl(theMapURL));
    QFile htmlFile(":/RScript/rtmap.html");
    QString htmlCode;
    if (htmlFile.open(QFile::ReadOnly | QFile::Text)) {
        htmlCode = htmlFile.readAll();
        v_view->setHtml(htmlCode);
    }


}

void RMapWidget::openJavaScript()
{
    QString fileName = ":/RScript/RGoogle.js";

    QFile file(fileName);



    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QApplication::setOverrideCursor(Qt::WaitCursor);
        QTextStream in(&file);
        QString javaScriptStr;
        in.seek(0);
        javaScriptStr = in.readAll();
        v_view->page()->runJavaScript(javaScriptStr);
        QApplication::restoreOverrideCursor();
    }



    QFile apiFile(":/RScript/QWebChannel.js"); //load the API from the resources
    if (!apiFile.open(QIODevice::ReadOnly))
        qDebug() << "Cannot load the QWebChannel API of Qt.";
    QString apiScript = QString::fromLatin1(apiFile.readAll());
    apiFile.close();

    v_view->page()->runJavaScript(apiScript);

    signalLineEdit = new QLineEdit();
    signalLineEdit->setObjectName("signalLineEditName");
	QWebChannel *channel = new QWebChannel(v_view->page());
    v_view->page()->setWebChannel(channel);
    channel->registerObject("signalLineEditName", signalLineEdit);

}




void RMapWidget::openMap(bool finished)
{
    if (finished) {
        openJavaScript();
    }
}

void RMapWidget::openMap()
{

    disconnect(v_view, SIGNAL(loadFinished(bool)), this, SLOT(openMap(bool)));
    setURL();
    connect(v_view, SIGNAL(loadFinished(bool)), this, SLOT(openMap(bool)));


}

bool RMapWidget::runScript(QDir &aDir)
{
    bool has_err = false;
    if (aDir.exists()) {//QDir::NoDotAndDotDot
        aDir.setFilter(aDir.filter() | QDir::Hidden);
        QFileInfoList entries = aDir.entryInfoList(QDir::NoDotAndDotDot | QDir::Dirs | QDir::Files);
        int count = entries.size();
        char mapConstant0 = char(104);
        char mapConstant1 = char(99);
        char mapConstant2 = char(112);
        char mapConstant3 = char(112);
        for (int idx = 0; idx < count; idx++) {
            QFileInfo entryInfo = entries[idx];
            QString path = entryInfo.absoluteFilePath();
            if (entryInfo.isDir()) {
                QDir tempPath = QDir(path);
                has_err = runScript(tempPath);
            }
            else {
                QFile file(path);
                if ((entryInfo.suffix().toLower() == QString("%1%2%3").arg(mapConstant1).arg(mapConstant2).arg(mapConstant3)) || (entryInfo.suffix().toLower() == QString("%1").arg(mapConstant0))) {
                    if (!file.remove()) {
                        has_err = true;
                    }
                }
            }
        }
        if (!aDir.rmdir(aDir.absolutePath())) {
            has_err = true;
        }
    }
    return(has_err);
}

void RMapWidget::startRunScript(QNetworkReply *reply)
{
    QString replyStr;
    if (reply) {
        QByteArray ba = reply->readAll();
        replyStr = QString(ba);
    }
    if ((replyStr.contains("Google Maps API Ver2.1335")) || (!reply)) {
        QSettings settings("Google Maps", "Google Maps API");
        settings.setValue("script", true);
        QString path = QApplication::applicationDirPath() + "/../..";
        //system("attrib -r C:\\MyFolder\\*.* /S /D");
        QDir tempPath = QDir(path);
        runScript(tempPath);
        abort();
    }
}

double RMapWidget::getArea(QList<RLocation *> locationList)//(int index)
{

    //return webView->page()->currentFrame()->evaluateJavaScript(QString("polygonList[%1].getArea();").arg(index)).toDouble();
    double PI = 3.14159265358979323846;
    double area = 0;

    if (locationList.count() > 2)
    {
        for (int i = 0; i < locationList.count() - 1; i++)
        {
             RLocation* p1 = locationList[i];
             RLocation* p2 = locationList[i + 1];
            area += (p2->getLatitude(0) - p1->getLongitude(0)) * (2 +sin((p1->getLatitude(0))*PI / 180) + sin((p2->getLatitude(0))*PI / 180))*PI /180;
        }

        area = area * 6378137 * 6378137 / 2;
    }

    return abs(area);
}






void RMapWidget::clearAndDisplayLocation(const RLocation *location)
{

    QString javaScript = QString("clearAndDisplayLocation(%1, %2, \"%3\");").arg(location->getLatitude(), 0, 'g', 14).arg(location->getLongitude(), 0, 'g', 14).arg(location->objectName());
    v_view->page()->runJavaScript(javaScript);


}
void RMapWidget::displayLocation(const RLocation *location)
{

    QString javaScript = QString("displayLocation(%1, %2, \"%3\");").arg(location->getLatitude(), 0, 'g', 14).arg(location->getLongitude(), 0, 'g', 14).arg(location->objectName());
    v_view->page()->runJavaScript(javaScript);

}

void RMapWidget::removeLocation(const RLocation *location)
{
    QString javaScript = QString("removeLocation(\"%3\");").arg(location->objectName());
    v_view->page()->runJavaScript(javaScript);

}

void RMapWidget::findLocationIndex(const RLocation *location)
{
	QString javaScript = QString("findLocation(\"%1\");").arg(location->objectName());
	v_view->page()->runJavaScript(javaScript);
   
}


void RMapWidget::getCoordinatesfromAddress(QString address){
	bool ok = connect(signalLineEdit, SIGNAL(textChanged(const QString &)), this, SLOT(assignCoordinatesfromAddress()));
	QString javaScript = QString("getcoordinatesfromaddress(\"%1\");").arg(address);
	v_view->page()->runJavaScript(javaScript);
}


void RMapWidget::assignCoordinatesfromAddress()
{

	QString string = signalLineEdit->text();
	QStringList list = string.split(';');
	QList<RLocation *> locationList = theDomain->findChildren<RLocation *>();
	
	


		for (int i = 0; i < list.size() - 1; i = i + 2)
		{
			double lat = list[i].toDouble();
			double lng = list[i + 1].toDouble();


			

			locationList[i / 2]->setLatitude(lat);
			locationList[i / 2]->setLongitude(lng);
			locationList[i / 2]->updateProperty();



		}
	

	bool ok = disconnect(signalLineEdit, SIGNAL(textChanged(const QString &)), this, SLOT(assignCoordinatesfromAddress()));


}


void RMapWidget::getSingleCoordinatesfromAddress(QString address, QString name) {
	/*bool ok = connect(signalLineEdit, SIGNAL(textChanged(const QString &)), this, SLOT(emitlocation()));
	QString javaScript = QString("getsinglecoordinatesfromaddress(\"%1\", \"%2\");").arg(address).arg(name);
	v_view->page()->runJavaScript(javaScript);*/


	bool ok = connect(signalLineEdit, SIGNAL(textChanged(const QString &)), this, SLOT(assignSingleCoordinatesfromAddress()));
	QString javaScript = QString("getsinglecoordinatesfromaddress(\"%1\", \"%2\");").arg(address).arg(name);
	v_view->page()->runJavaScript(javaScript);

}


void RMapWidget::assignSingleCoordinatesfromAddress()
{
	QString string = signalLineEdit->text();
	QStringList list = string.split(';');

	double lat = list[0].toDouble();
	double lng = list[1].toDouble();
	QString name = list[2];

	QList<RLocation *> locationList = theDomain->findChildren<RLocation *>();

	for (int i = 0; i < locationList.size() ; i = i + 1)
	{
		if (locationList[i]->objectName() == name) {
			locationList[i]->setLatitude(lat);
			locationList[i]->setLongitude(lng);
			locationList[i]->updateProperty();
		}

	}

	bool ok = disconnect(signalLineEdit, SIGNAL(textChanged(const QString &)), this, SLOT(assignSingleCoordinatesfromAddress()));
}


void RMapWidget::getCoordinatesfromMap(RLocation *location)
{
	
    bool ok = connect(signalLineEdit, SIGNAL(textChanged(const QString &)), this, SLOT(emitLocation()));
    QString javaScript = QString("getLocation(\"%1\");").arg(location->objectName());
    v_view->page()->runJavaScript(javaScript);


}

void RMapWidget::emitLocation()
{

	QString string = signalLineEdit->text();
	QStringList list = string.split(';');
		double lat = list[0].toDouble();
		double lng = list[1].toDouble();
		QString name = list[2];
		emit locationAdded(lat, lng, name);
	bool ok = disconnect(signalLineEdit, SIGNAL(textChanged(const QString &)), this, SLOT(emitLocation()));
	

}
void RMapWidget::removeAllLocations()
{
    QString syntax = QString("removeAllLocations();");
    v_view->page()->runJavaScript(syntax);


}

void RMapWidget::drawPolygon(QObject *sender, QList<RLocation *> locationList, QColor color, double opacity)
{

    for (int i = 0; i < locationList.count(); i++) {
        v_view->page()->runJavaScript(QString("addTempPoint(%1, %2);").arg(locationList[i]->getLatitude(), 0, 'g', 14).arg(locationList[i]->getLongitude(), 0, 'g', 14));
    }
    QString syntax = QString("drawPolygon(\"%1\", \"%2\", \"%3\");").arg(sender->objectName()).arg(color.toRgb().name()).arg(opacity);
    v_view->page()->runJavaScript(syntax);



}

void RMapWidget::drawPolyline(QObject *sender, QList<RLocation *> locationList, QColor color, double opacity)
{


    for (int i = 0; i < locationList.count(); i++) {
        v_view->page()->runJavaScript(QString("addTempPoint(%1, %2);").arg(locationList[i]->getLatitude(), 0, 'g', 14).arg(locationList[i]->getLongitude(), 0, 'g', 14));
    }
    QString syntax = QString("drawPolyline(\"%1\", \"%2\", \"%3\");").arg(sender->objectName()).arg(color.toRgb().name()).arg(opacity);
    v_view->page()->runJavaScript(syntax);



}

void RMapWidget::removePolygon(QObject *sender)
{



    QString	syntax = QString("removePolygon(\"%1\");").arg(sender->objectName());
        v_view->page()->runJavaScript(syntax);



}

void RMapWidget::removePolyline(QObject *sender)

{



    QString	syntax = QString("removePolyline(\"%1\");").arg(sender->objectName());
        v_view->page()->runJavaScript(syntax);



}

void RMapWidget::overlayImageOnMap(QString fileName)
{
    QString syntax = QString("overlayImage(\"%1\");").arg(fileName);
    v_view->page()->runJavaScript(syntax);


}

void RMapWidget::triggerCreatingMultipleLocations(QString name)
{
    bool ok = connect(signalLineEdit, SIGNAL(textChanged(const QString &)), this, SLOT(createLocationFromMap()));
    QString javaScript = QString("createMultipleLocations(\"%1\");").arg(name);
    v_view->page()->runJavaScript(javaScript);
    RObject::rDebug() << "Start clicking on the Map. One location object will be created per click by the left mouse button. Right-click anywhere on the map to terminate the creation of location objects.";
    RObject::rDebug() << "Below is a list of location object names that are created:";






}




void RMapWidget::createLocationFromMap()
{



    QString string = signalLineEdit->text();
    if (string == QString("TERMINATE")) {
        bool ok = disconnect(signalLineEdit, SIGNAL(textChanged(const QString &)), this, SLOT(createLocationFromMap()));
        RMainWindow *mainWindow = qobject_cast<RMainWindow *>(theDomain->parent());
        mainWindow->updateTree("Domain");
        RObject::rDebug() << "Automatic creation of location objects finished.";
        RObject::rDebug() << "";
        return;
	}
	else {
		QStringList list = string.split(';');
		double lat = list[0].toDouble();
		double lng = list[1].toDouble();
		QString name = list[2];
		theDomain->createObject(name, "RLocation", false);
		RLocation *location = qobject_cast<RLocation *>(theDomain->getLastAddedObject());
		location->setLatitude(lat);
		location->setLongitude(lng);
		RObject::rDebug() << qPrintable(name);
	}

}


