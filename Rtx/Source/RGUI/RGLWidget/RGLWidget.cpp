#include <QtWidgets>

#include "RGLWidgetEngine.h"
#include "RGLWidget.h"

RGLWidget::RGLWidget(QWidget *parent)
{
	theGLWidget = new RGLWidgetEngine;

	xSlider = createSlider();
	ySlider = createSlider();
	zSlider = createSlider();
	zSlider->setOrientation(Qt::Horizontal);

	QHBoxLayout *horizontalLayout1 = new QHBoxLayout;
	horizontalLayout1->addWidget(theGLWidget);
	horizontalLayout1->addWidget(xSlider);

	QHBoxLayout *horizontalLayout2 = new QHBoxLayout;
	horizontalLayout2->addWidget(zSlider);
	horizontalLayout2->addSpacing(xSlider->sizeHint().width() + 6);

	QVBoxLayout *verticalLayout = new QVBoxLayout;
	verticalLayout->addLayout(horizontalLayout1);
	verticalLayout->addLayout(horizontalLayout2);

	setLayout(verticalLayout);


	xSlider->setValue(theGLWidget->getXRotation());
	ySlider->setValue(theGLWidget->getYRotation());
	zSlider->setValue(theGLWidget->getZRotation());

	connect(xSlider, SIGNAL(valueChanged(int)), theGLWidget, SLOT(setXRotation(int)));
	connect(theGLWidget, SIGNAL(xRotationChanged(int)), xSlider, SLOT(setValue(int)));
	connect(ySlider, SIGNAL(valueChanged(int)), theGLWidget, SLOT(setYRotation(int)));
	connect(theGLWidget, SIGNAL(yRotationChanged(int)), ySlider, SLOT(setValue(int)));
	connect(zSlider, SIGNAL(valueChanged(int)), theGLWidget, SLOT(setZRotation(int)));
	connect(theGLWidget, SIGNAL(zRotationChanged(int)), zSlider, SLOT(setValue(int)));
}

QSlider * RGLWidget::createSlider()
{
	QSlider *slider = new QSlider(Qt::Vertical);
	slider->setRange(0, 360 * 16);
	slider->setSingleStep(16);
	slider->setPageStep(15 * 16);
	slider->setTickInterval(15 * 16);
	slider->setTickPosition(QSlider::TicksRight);
	return slider;
}

RGLWidgetEngine * RGLWidget::getEngine()
{
	return theGLWidget;
}
