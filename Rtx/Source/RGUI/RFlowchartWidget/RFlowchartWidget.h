// Last modified by Mojtaba on June 30, 2012

#ifndef RFLOWCHARTWIDGET_H
#define RFLOWCHARTWIDGET_H

#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMenu>
#include <QPointer>

#include "ui_RFlowchartWidget.h"

using namespace Ui;

class RFlowchartWidget : public QGraphicsView, public RFlowchartWidgetUi
{
	Q_OBJECT

public:
	RFlowchartWidget(QWidget *parent = 0);
	~RFlowchartWidget();

	// Sets an object list that contains all models and the function together with a QMap that contains their relationship
	void setObjectList(QList<QObjectList> objectListList, QMap<QPointer<QObject>, QObjectList> objectMap);

protected:
    //void mousePressEvent(QMouseEvent *event);
    //void mouseMoveEvent(QMouseEvent *event);
	// Zooms in/out on the flowchart according to the mouse-wheel
    void wheelEvent(QWheelEvent *event);

	// Resets the zoom level of the flowchart when double-clicked on
	void mouseDoubleClickEvent(QMouseEvent *event);

private slots:
	// Shows the right-click menu on the flowchart
	void showPopup(const QPoint &position);

	// Saves the flowchart to vector graphics formats including PDF and PS
	void saveDiagramToVectorFile();

	// Saves the flowchart to the SVG vector graphics format
	void saveDiagramToSvgFile();

	// Saves the flowchart to pixel graphics formats
	void saveDiagramToPixelFile();

	// Sends the diagram to the printer
	void printDiagram();

	// Changes the arrows from direct style to elbow style
	void setElbow(bool checked);

	// Zooms in on the flowchart
	void zoomIn();

	// Zooms out on the flowchart
	void zoomOut();

	// Resets the flowchart zoom level
	void resetZoom();

private:
	// Auxiliart method that creates the right-click menu
	void createMenu();

	// Auxiliary method that does the actual drawing of the flowchart
	void draw();

	// Data members for the right-click menu
	QMenu *menu;
	QAction *saveDiagramVectorAction;
	QAction *saveDiagramSvgAction;
	QAction *saveDiagramPixelAction;
	QAction *printDiagramAction;
	QAction *elbowAction;
	QAction *zoomInAction;
	QAction *zoomOutAction;
	QAction *resetZoomAction;

	// Data members for the model and function obejcts and their relationship
	QList<QObjectList> theObjectListList;
	QMap<QPointer<QObject>, QObjectList> theObjectMap;

	// Data member for Qt 2D graphics
	QGraphicsScene *theScene;

	// Data members that indicates if the arrows should be elbows or straight lines
	bool elbowFlag;

	// The current zoom factor
	double theScaleFactor;

	//QPoint lastPos;

};

#endif // RFLOWCHARTWIDGET_H
