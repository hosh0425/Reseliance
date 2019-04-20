#include <QtWidgets>
#include <QtOpenGL>
#include <QOpenGLWidget>

#include <math.h>

#include "RGLWidgetEngine.h"

RGLWidgetEngine::RGLWidgetEngine(QWidget *parent)
: QOpenGLWidget(parent)
{
	object = 0;

	xRotDefault = 16 * 285;
	yRotDefault = 16 * 360;
	zRotDefault = 16 * 345;
	xRot = xRotDefault;
	yRot = yRotDefault;
	zRot = zRotDefault;

	xTrans = 0;
	yTrans = 0;
	zTrans = 0;

	globalScale = 5;
	scale = globalScale;

	double temp = 1e50;
	maxX = -temp;
	minX = temp;
	maxY = -temp;
	minY = temp;
	maxZ = -temp;
	minZ = temp;

	margin = 1.02;

	backgroundColor = QColor::fromCmykF(0.39, 0.39, 0.0, 0.0);
}

RGLWidgetEngine::~RGLWidgetEngine()
{
	//makeCurrent();
	//glDeleteLists(object, 1);
}

QSize RGLWidgetEngine::minimumSizeHint() const
{
	return QSize(50, 50);
}

QSize RGLWidgetEngine::sizeHint() const
{
	return QSize(400, 400);
}

void RGLWidgetEngine::setXRotation(int angle)
{
	//normalizeAngle(&angle);
	//if (angle != xRot) {
	//	xRot = angle;
	//	emit xRotationChanged(angle);
	//	paintGL();
	//}
}

void RGLWidgetEngine::setYRotation(int angle)
{
	//normalizeAngle(&angle);
	//if (angle != yRot) {
	//	yRot = angle;
	//	emit yRotationChanged(angle);
	//	paintGL();
	//}
}

void RGLWidgetEngine::setZRotation(int angle)
{
	//normalizeAngle(&angle);
	//if (angle != zRot) {
	//	zRot = angle;
	//	emit zRotationChanged(angle);
	//	paintGL();
	//}
}

int RGLWidgetEngine::getXRotation()
{
	return xRot;
}

int RGLWidgetEngine::getYRotation()
{
	return yRot;
}

int RGLWidgetEngine::getZRotation()
{
	return zRot;
}

void RGLWidgetEngine::setXTranslation(double offset)
{
	//if (offset != xTrans) {
	//	xTrans = offset;
	//	paintGL();
	//}
}

void RGLWidgetEngine::setYTranslation(double offset)
{
	/*if (offset != yTrans) {
		yTrans = offset;
		paintGL();
	}*/
}

void RGLWidgetEngine::setZTranslation(double offset)
{
	/*if (offset != zTrans) {
		zTrans = offset;
		paintGL();
	}*/
}

void RGLWidgetEngine::initializeGL()
{
	//glClearColor(backgroundColor.light().red(), backgroundColor.light().green(), backgroundColor.light().blue(), backgroundColor.light().alpha());
	//glShadeModel(GL_FLAT);
	//glEnable(GL_DEPTH_TEST);
	////glEnable(GL_CULL_FACE);

	//////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////
	////glShadeModel(GL_SMOOTH);
	////glEnable(GL_LIGHTING);
	////glEnable(GL_LIGHT0);
	////glEnable(GL_MULTISAMPLE);
	//////glEnable(GL_COLOR_MATERIAL);
	//////static GLfloat lightPosition[4] = { 0.5, 5.0, 7.0, 1.0 };
	//////glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	////GLfloat LightAmbient[]  = {1.0f, 1.0f, 1.0f, 1.0f};
	////glLightfv(GL_LIGHT0, GL_AMBIENT, LightAmbient);
	//////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////

	//// Antialiasing
	//glEnable(GL_LINE_SMOOTH);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	//glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);//NEW
	//glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);//NEW
	////glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);//NEW
}

void RGLWidgetEngine::paintGL()
{
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glLoadIdentity();
	//glTranslated(xTrans, yTrans, zTrans);
	//glRotated(xRot / 16.0, 1.0, 0.0, 0.0);
	//glRotated(yRot / 16.0, 0.0, 1.0, 0.0);
	//glRotated(zRot / 16.0, 0.0, 0.0, 1.0);
	//glScaled(scale, scale, scale);

	//resizeGL(width(), height());

	//glCallList(object);

	//QFont font = QFont("Times", 12);
	//glDisable(GL_DEPTH_TEST);

	//for (int i = 0; i < theTextList.count(); i++) {
	//	QVector<double> coordinates = theTextCoordinateList[i];

	//	double x  = coordinates[0];
	//	double y  = coordinates[1];
	//	double z  = coordinates[2];

	//	QString text = theTextList[i];
	//	// GLTEMP
	//	//glColor4xOES(theTextColorList[i].red(), theTextColorList[i].green(), theTextColorList[i].blue(), theTextColorList[i].alpha());
	//	//qglcolor(theTextColorList[i]);
	//	//renderText(x, y, z, text, font);
	//	//this->render()
	//}

	//glEnable(GL_DEPTH_TEST);
}

void RGLWidgetEngine::resizeGL(int width, int height)
{
	//int side = qMin(width, height);
	////glViewport((width - side) / 2, (height - side) / 2, side, side);
	//glViewport(0, 0, width, height);

	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();

	//calculateMax(width, height);

	//glOrtho(-maxH * globalScale, maxH * globalScale, -maxV * globalScale, maxV * globalScale, -max * scale * globalScale, max * scale * globalScale);
	////gluPerspective(45.0, 1.0*width/height, 1.0, 50.0);
	//glMatrixMode(GL_MODELVIEW);
}

void RGLWidgetEngine::mousePressEvent(QMouseEvent *event)
{
	/*lastPos = event->pos();*/
}

void RGLWidgetEngine::mouseMoveEvent(QMouseEvent *event)
{
	//int dx = event->x() - lastPos.x();
	//int dy = event->y() - lastPos.y();

	//int rotationFactor = 3;
	//if (event->buttons() & Qt::LeftButton) {
	//	double xTranslationFactor = (maxH * 2.0 * globalScale) / width();
	//	double yTranslationFactor = (maxV * 2.0 * globalScale) / height();
	//	setXTranslation(xTrans + xTranslationFactor * dx);
	//	setYTranslation(yTrans - yTranslationFactor * dy);
	//} else if (event->buttons() & Qt::RightButton) {
	//	setXRotation(xRot + rotationFactor * dy);
	//	setZRotation(zRot + rotationFactor * dx);
	//} else if (event->buttons() & Qt::MidButton) {
	//	setXRotation(xRot + rotationFactor * dy);
	//	setYRotation(yRot + rotationFactor * dx);
	//}
	//lastPos = event->pos();
}

void RGLWidgetEngine::mouseDoubleClickEvent(QMouseEvent *event)
{
	/*xTrans = -(maxX + minX) * globalScale / 2.0;
	yTrans = -(maxZ + minZ) * globalScale / 2.0;
	scale = globalScale;
	xRot = xRotDefault;
	yRot = yRotDefault;
	zRot = zRotDefault;
	paintGL();*/
}

void RGLWidgetEngine::wheelEvent(QWheelEvent *event)
{
	/*double deltaScale;
	event->delta() > 0 ? deltaScale = scale*0.1 : deltaScale = -scale*0.1;
	double oldScale = scale;
	scale += deltaScale;
	double x = event->x();
	int width = this->width();
	double y = event->y();
	int height = this->height();
	setXTranslation(xTrans - (scale/oldScale - 1.0) * ((x / width) * (maxH * 2.0 * globalScale) - (xTrans + maxH * globalScale)));
	setYTranslation(yTrans - (scale/oldScale - 1.0) * (-(y / height) * (maxV * 2.0 * globalScale) - (yTrans - maxV * globalScale)));
	paintGL();*/
}

GLuint RGLWidgetEngine::makeObject()
{
	GLuint list = 0;// = glGenLists(1);
	//glNewList(list, GL_COMPILE);

	/////glBegin(GL_LINES); ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//for (int i = 0; i < theLineList.count(); i++) {
	//	QVector<double> coordinates = theLineList[i];
	//	double x1  = coordinates[0];
	//	double y1  = coordinates[1];
	//	double z1  = coordinates[2];
	//	double x2  = coordinates[3];
	//	double y2  = coordinates[4];
	//	double z2  = coordinates[5];

	//	maxX = qMax(x1, maxX);
	//	maxX = qMax(x2, maxX);
	//	maxY = qMax(y1, maxY);
	//	maxY = qMax(y2, maxY);
	//	maxZ = qMax(z1, maxZ);
	//	maxZ = qMax(z2, maxZ);

	//	minX = qMin(x1, minX);
	//	minX = qMin(x2, minX);
	//	minY = qMin(y1, minY);
	//	minY = qMin(y2, minY);
	//	minZ = qMin(z1, minZ);
	//	minZ = qMin(z2, minZ);
	//	
	//	// GLTEMP
	//	//qglColor(theLineColorList[i]);
	//	glVertex3d(x1, y1, z1); // origin of the line
	//	glVertex3d(x2, y2, z2); // ending point of the line

	//}

	//glEnd(); ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	/////glBegin(GL_QUADS); ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//for (int i = 0; i < theQuadList.count(); i++) {
	//	QVector<double> coordinates = theQuadList[i];
	//	double x1  = coordinates[0];
	//	double y1  = coordinates[1];
	//	double z1  = coordinates[2];
	//	double x2  = coordinates[3];
	//	double y2  = coordinates[4];
	//	double z2  = coordinates[5];
	//	double x3  = coordinates[6];
	//	double y3  = coordinates[7];
	//	double z3  = coordinates[8];
	//	double x4  = coordinates[9];
	//	double y4  = coordinates[10];
	//	double z4  = coordinates[11];

	//	maxX = qMax(x1, maxX);
	//	maxX = qMax(x2, maxX);
	//	maxX = qMax(x3, maxX);
	//	maxX = qMax(x4, maxX);
	//	maxY = qMax(y1, maxY);
	//	maxY = qMax(y2, maxY);
	//	maxY = qMax(y3, maxY);
	//	maxY = qMax(y4, maxY);
	//	maxZ = qMax(z1, maxZ);
	//	maxZ = qMax(z2, maxZ);
	//	maxZ = qMax(z3, maxZ);
	//	maxZ = qMax(z4, maxZ);

	//	minX = qMin(x1, minX);
	//	minX = qMin(x2, minX);
	//	minX = qMin(x3, minX);
	//	minX = qMin(x4, minX);
	//	minY = qMin(y1, minY);
	//	minY = qMin(y2, minY);
	//	minY = qMin(y3, minY);
	//	minY = qMin(y4, minY);
	//	minZ = qMin(z1, minZ);
	//	minZ = qMin(z2, minZ);
	//	minZ = qMin(z3, minZ);
	//	minZ = qMin(z4, minZ);

	//	// GLTEMP
	//	//qglColor(theQuadColorList[i]);
	//	glVertex3d(x1, y1, z1);
	//	glVertex3d(x2, y2, z2);
	//	glVertex3d(x3, y3, z3);
	//	glVertex3d(x4, y4, z4);


	//}

	//glEnd(); ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



	/////glBegin(GL_LINES); ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//for (int i = 0; i < theQuadList.count(); i++) {

	//	QVector<double> coordinates = theQuadList[i];
	//	double x1  = coordinates[0];
	//	double y1  = coordinates[1];
	//	double z1  = coordinates[2];
	//	double x2  = coordinates[3];
	//	double y2  = coordinates[4];
	//	double z2  = coordinates[5];
	//	double x3  = coordinates[6];
	//	double y3  = coordinates[7];
	//	double z3  = coordinates[8];
	//	double x4  = coordinates[9];
	//	double y4  = coordinates[10];
	//	double z4  = coordinates[11];

	//	// GLTEMP
	//	//qglColor(theQuadBorderColorList[i]);
	//	glVertex3d(x1, y1, z1);
	//	glVertex3d(x2, y2, z2);

	//	glVertex3d(x2, y2, z2);
	//	glVertex3d(x3, y3, z3);

	//	glVertex3d(x3, y3, z3);
	//	glVertex3d(x4, y4, z4);

	//	glVertex3d(x4, y4, z4);
	//	glVertex3d(x1, y1, z1);

	//}

	//glEnd(); ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	//max = qMax(qAbs(minX), qAbs(maxX));
	//max = qMax(max, qAbs(minY));
	//max = qMax(max, qAbs(maxY));
	//max = qMax(max, qAbs(minZ));
	//max = qMax(max, qAbs(maxZ));
	//max *= margin;

	//calculateMax(width(), height());

	////double scaleH = maxH * 2.0 / (maxX - minX) / margin;
	////double scaleV = maxV * 2.0 / (maxZ - minZ) / margin;
	///*double scaleH1 = maxH / qAbs(maxX) / margin;
	//double scaleH2 = maxH / qAbs(minX) / margin;
	//double scaleH = qMin(scaleH1, scaleH2);
	//double scaleV1 = maxV / qAbs(maxZ) / margin;
	//double scaleV2 = maxV / qAbs(minZ) / margin;
	//double scaleV = qMin(scaleV1, scaleV2);*/
	////scale *= qMin(scaleH, scaleV);

	//xTrans = -(maxX + minX) * globalScale / 2.0;
	//yTrans = -(maxZ + minZ) * globalScale / 2.0;

	//glEndList();
	return list;
}

void RGLWidgetEngine::calculateMax(int width, int height)
{
	//maxH = max;
	//maxV = max;
	//if (width > height) {
	//	maxH *= (double) width / height;
	//} else {
	//	maxV *= (double) height / width;
	//}
}


void RGLWidgetEngine::addLine(double x1, double y1, double z1, double x2, double y2, double z2, QColor color)
{
	QVector<double> lineVector;
	lineVector << x1 << y1 << z1 << x2 << y2 << z2;
	theLineList << lineVector;
	theLineColorList << color;
}

void RGLWidgetEngine::addText(double x, double y, double z, QString text, QColor color)
{
	QVector<double> theCoordinateVector;
	theCoordinateVector << x << y << z;

	theTextCoordinateList << theCoordinateVector;
	theTextColorList << color;
	theTextList << text;
}

void RGLWidgetEngine::addSurface(double x1, double y1, double z1, double x2, double y2, double z2, double x3, double y3, double z3, double x4, double y4, double z4, QColor surfaceColor, QColor borderColor)
{
	QVector<double> surfaceVector;
	surfaceVector << x1 << y1 << z1 << x2 << y2 << z2 << x3 << y3 << z3 << x4 << y4 << z4;
	theQuadList << surfaceVector;
	theQuadColorList << surfaceColor;
	theQuadBorderColorList << borderColor;
}

void RGLWidgetEngine::update3D()
{
	//object = makeObject();
	//paintGL();
}

void RGLWidgetEngine::clearAll()
{
	//theLineList.clear();
	//theQuadList.clear();
	//theTextCoordinateList.clear();
	//theLineColorList.clear();
	//theQuadColorList.clear();
	//theQuadBorderColorList.clear();
	//theTextColorList.clear();
	//theTextList.clear();
	//

	//xRot = xRotDefault;
	//yRot = yRotDefault;
	//zRot = zRotDefault;
	//xTrans = 0;
	//yTrans = 0;
	//zTrans = 0;
	//scale = globalScale;

	//double temp = 1e50;
	//maxX = -temp;
	//minX = temp;
	//maxY = -temp;
	//minY = temp;
	//maxZ = -temp;
	//minZ = temp;

	//makeCurrent();
	//glDeleteLists(object, 1);

	//update3D();
}

void RGLWidgetEngine::normalizeAngle(int *angle)
{
	while (*angle < 0)
		*angle += 360 * 16;
	while (*angle > 360 * 16)
		*angle -= 360 * 16;
}

double RGLWidgetEngine::getCharacteristicDimension()
{
	return max;
}