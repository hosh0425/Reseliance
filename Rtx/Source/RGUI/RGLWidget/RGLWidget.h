// Last modified by Mojtaba on June 30, 2012

#ifndef RGLWidget_H
#define RGLWidget_H

#include <QWidget>

class QSlider;
class RGLWidgetEngine;

class RGLWidget : public QWidget
{
	Q_OBJECT

public:
	RGLWidget(QWidget *parent = 0);

	// Returns a pointer to the RGLWidgetEngine object, which is where the OpenGL code is written
	RGLWidgetEngine *getEngine();

private:
	// Creates the sliders that rotate the OpenGL drawing 
	QSlider *createSlider();

	// Data members
	RGLWidgetEngine *theGLWidget;
	QSlider *xSlider;
	QSlider *ySlider;
	QSlider *zSlider;
};

#endif // RGLWidget_H
