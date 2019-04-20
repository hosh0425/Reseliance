// Last modified by Mojtaba on June 30, 2012

#ifndef RPlotWidget_H
#define RPlotWidget_H



#include <QMenu>
#include <QPointer>
#include <QPrintDialog>
#include "RObject.h"
#include "qcustomplot.h"

class RPlotWidget : public QCustomPlot
{
	Q_OBJECT

public:
	RPlotWidget(QWidget *parent = 0);
	~RPlotWidget();

	//QChart *m_chart;
	//QBarSeries *m_series;
	//QChartView *m_chartView;

	QSharedPointer<QCustomPlot> plotShared;

	public slots:
	// Refreshes the plot
	//void replot();

	//void setData(QVector<double> x, QVector<double> y);

	private slots:

	void mouseWheel();

	void mousePress();

	void mouseDoubleClickEvent(QMouseEvent *event);

	void contextMenuRequest(QPoint pos);

	// Turns the legend on/off
	void showLegend(bool checked);

	void resetZoom();

	void moveLegend();

	void saveDiagramToTextFile();

	void saveDiagramToVectorFile();

	void printPreview();

	void renderPlot(QPrinter *printer);

	void saveDiagramToPixelFile();

	// Sets the label of the bottom axis
	void setBottomAxisLabel();
	
	// Sets the minimum value of the bottom axis
	void setBottomAxisMin();
	
	// Sets the maximum value of the bottom axis
	void setBottomAxisMax();
	
	// Sets the scale of the bottom axis to linear or logarithmic
	void setBottomAxisLog(bool checked);
	
	// Sets the label of the top axis
	void setTopAxisLabel();
	
	// Sets the minimum value of the top axis
	void setTopAxisMin();
	
	// Sets the maximum value of the top axis
	void setTopAxisMax();
	
	// Sets the scale of the top axis to linear or logarithmic
	void setTopAxisLog(bool checked);
	
	// Sets the label of the left axis
	void setLeftAxisLabel();
	
	// Sets the minimum value of the left axis
	void setLeftAxisMin();
	
	// Sets the maximum value of the left axis
	void setLeftAxisMax();
	
	// Sets the scale of the left axis to linear or logarithmic
	void setLeftAxisLog(bool checked);
	
	// Sets the label of the right axis
	void setRightAxisLabel();
	
	// Sets the minimum value of the right axis
	void setRightAxisMin();
	
	// Sets the maximum value of the right axis
	void setRightAxisMax();
	
	// Sets the scale of the right axis to linear or logarithmic
	void setRightAxisLog(bool checked);

	void axisLabelDoubleClick(QCPAxis *axis, QCPAxis::SelectablePart part);

	void legendDoubleClick(QCPLegend *legend, QCPAbstractLegendItem *item);

	void titleDoubleClick(QMouseEvent* event);

	void addTitle();


	//void titleDoubleClick(QMouseEvent *event);
	//void axisLabelDoubleClick(QCPAxis* axis, QCPAxis::SelectablePart part);
	//void legendDoubleClick(QCPLegend* legend, QCPAbstractLegendItem* item);
	//void selectionChanged();
	//
	//void addRandomGraph();
	//void removeSelectedGraph();
	//void removeAllGraphs();
	//void contextMenuRequest(QPoint pos);
	//
	//void graphClicked(QCPAbstractPlottable *plottable, int dataIndex);

	
	// Data members for the "right-click" menu
private:
	QMenu *menu;
	QAction *saveDiagramTextAction;
	QAction *saveDiagramVectorAction;
	QAction *saveDiagramSvgAction;
	QAction *saveDiagramPixelAction;
	QAction *printDiagramAction;
	QAction *zoomInAction;
	QAction *zoomOutAction;
	QAction *resetZoomAction;
	QMenu *bottomAxisMenu;
	QMenu *topAxisMenu;
	QMenu *leftAxisMenu;
	QMenu *rightAxisMenu;
	QAction *bottomAxisLabelAction;
	QAction *bottomAxisMinAction;
	QAction *bottomAxisMaxAction;
	QAction *bottomAxisLogAction;
	QAction *topAxisLabelAction;
	QAction *topAxisMinAction;
	QAction *topAxisMaxAction;
	QAction *topAxisLogAction;
	QAction *leftAxisLabelAction;
	QAction *leftAxisMinAction;
	QAction *leftAxisMaxAction;
	QAction *leftAxisLogAction;
	QAction *rightAxisLabelAction;
	QAction *rightAxisMinAction;
	QAction *rightAxisMaxAction;
	QAction *rightAxisLogAction;
	QMenu *sizeMenu;
	QAction *horizontalSizeAction;
	QAction *verticalSizeAction;
	QAction *showLegendAction;

	QCustomPlot *thePlot = new QCustomPlot;

	// Data member used for storing the custom title of the plot
	QCPTextElement *title;

	QCPLayoutGrid *subLayout;
	QCPLayoutElement *dummyElement;
	QCPLayoutElement *dummyElement2;


#ifdef Q_OS_WIN32

	
#endif // Q_OS_WIN32

	// Data member that stores the current zoom factor
	double theScaleFactor;

	// Data member for checkable items of the context menu
	bool Legendbool;
	bool bottomLogaxisbool;
	bool topLogaxisbool;
	bool leftLogaxisbool;
	bool rightLogaxisbool;

};

#endif // RPlotWidget_H
