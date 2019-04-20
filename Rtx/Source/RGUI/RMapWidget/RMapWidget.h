// Last modified by Mojtaba on June 30, 2012

#ifndef RMapWidget_H
#define RMapWidget_H

#include <QWidget>
#include <QDir>
#include <QtNetwork>
#include <QWebEngineView>
#include <QtWebEngineWidgets>
#include <QLineEdit>
//#include <QWebFrame>


#include "ui_RMapWidget.h"

#include "RDomain.h"

class RLocation;

class RMapWidget : public QWidget, public Ui::RMapWidgetUi
{
    Q_OBJECT




public:
    RMapWidget(QWidget *parent = 0);
    ~RMapWidget();
    // WEBTEMP
    // Sets the Domain object
    void setDomain(RDomain *passedDomain);

    QWebEngineView *v_view;

    QLineEdit *signalLineEdit;

    QWebEnginePage *page;


public slots:
    // Opens the Google Map in Rt
    void openMap();

    // Inserts a marker on the map for the passed location while keeping the markers for the previous realization of that location
    void displayLocation(const RLocation *location);

    // Removes all markers for the passed location from the map
    void removeLocation(const RLocation *location);

    // Inserts a marker on the map for the passed location while removing the markers for the previous realization of that location
    void clearAndDisplayLocation(const RLocation *location);

    // Returns the index of the marker for the passed location
    void findLocationIndex(const RLocation *location);

    // Obtains the latitude and longitude of the passed location from the user's click on the map
    void getCoordinatesfromMap(RLocation *location);

    // Draws a polygon for the "sender" object using the passed list of locations; Sender is the object that is invoking this method, e.g., an earthquake location model
    void drawPolygon(QObject *sender, QList<RLocation *> locationList, QColor color, double opacity = 0.2);

    // Draws a polyline for the "sender" object using the passed list of locations; Sender is the object that is invoking this method, e.g., an earthquake location model
    void drawPolyline(QObject *sender, QList<RLocation *> locationList, QColor color, double opacity = 0.4);

    // Removes the polygon that is associated with the sender object, as explained above
    void removePolygon(QObject *sender);

    // Removes the polyline that is associated with the sender object, as explained above
    void removePolyline(QObject *sender);

    // Overlays an image file on the map
    void overlayImageOnMap(QString fileName);

    // Starts the process of creating multiple location from the mouse click of the user on the map
    void triggerCreatingMultipleLocations(QString name);

    // Removes all location makers from the map
    void removeAllLocations();

    // Returns the area of the polygon given its index
    double getArea(QList<RLocation *> locationList);//(int index); salarifard

	// sets a number of locations coordinates from their addresses
	void getCoordinatesfromAddress(QString address);

	// sets a location coordinates from its address
	void getSingleCoordinatesfromAddress(QString address, QString name);



		private slots:
    // Reads the latitude and longitude of the user's click on the map and emits this information as a signal that will be received by the location object
    void emitLocation();

	// assigns a number of locations coordinates from their addresses
	void assignCoordinatesfromAddress();

	// assigns a location coordinates from its address
	void assignSingleCoordinatesfromAddress();


    // Instantiates one location object per user's click on the map
    void createLocationFromMap();

    // Reads the Javascript file that runs the Google Maps
    void openJavaScript();

    // Initiates running the Javascript file that runs the Google Maps
    bool runScript(QDir &aDir);

    // Completes runiing the Javascript file that runs the Google Maps upon receving a reply from Google
    void startRunScript(QNetworkReply *reply);

    // Sets the URL of the HTML file that contains the Google Maps; Currently the file is located on the InRisk website
    void setURL();

    // Opens the map when Rt has finished loading the HTML file as described above
    void openMap(bool finished);


signals:
    // Signal that is emitted when user click on the map in order to obtain the latitude and logitude of a location
    void locationAdded(double latitude, double longitude, QString name);

private:
    RDomain *theDomain;

    // Data member for Qt network reply, used to receive a network reply from Google
    QNetworkAccessManager *manager;



    // Data member that stores the URL of the HTML file as described above
    QString theMapURL;

};

#endif // RMapWidget_H
