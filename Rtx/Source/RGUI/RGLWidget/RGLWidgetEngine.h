// Last modified by Mojtaba on June 30, 2012

#ifndef RGLWidgetEngine_H
#define RGLWidgetEngine_H

#include <QOpenGLWidget>

class RGLWidgetEngine : public QOpenGLWidget
{
	Q_OBJECT

public:
	RGLWidgetEngine(QWidget *parent = 0);
	~RGLWidgetEngine();

	// Returns the minimum size of the OpenGL drawing
	QSize minimumSizeHint() const;

	// Returns the size of the OpenGL drawing
	QSize sizeHint() const;

	// Adds a line to the OpenGL drawing given its start and end coordiantes and color
	void addLine(double x1, double y1, double z1, double x2, double y2, double z2, QColor color);

	// Adds a quadrilateral surface to the OpenGL drawing given its corner points coordiantes and color
	void addSurface(double x1, double y1, double z1, double x2, double y2, double z2, double x3, double y3, double z3, double x4, double y4, double z4, QColor surfaceColor, QColor borderColor);

	// Adds text to the OpenGL drawing given its coordiantes and color
	void addText(double x, double y, double z, QString text, QColor color);

	// Creates/updates the OpenGL drawing
	void update3D();

	// Clears the OpenGL drawing
	void clearAll();

	// Returns the maximum of the coordnates in any dimension
	double getCharacteristicDimension();

	// Returns the rotation around the x-axis
	int getXRotation();

	// Returns the rotation around the y-axis
	int getYRotation();

	// Returns the rotation around the z-axis
	int getZRotation();

public slots:
	// Rotates the OpenGL drawing around the x-axis
	void setXRotation(int angle);

	// Rotates the OpenGL drawing around the y-axis
	void setYRotation(int angle);

	// Rotates the OpenGL drawing around the z-axis
	void setZRotation(int angle);

	// Translates the OpenGL drawing around the x-axis
	void setXTranslation(double offset);

	// Translates the OpenGL drawing around the y-axis
	void setYTranslation(double offset);

	// Translates the OpenGL drawing around the z-axis
	void setZTranslation(double offset);

signals:
	// Signal emmited when the OpenGL is rotated around the x-axis
	void xRotationChanged(int angle);

	// Signal emmited when the OpenGL is rotated around the y-axis
	void yRotationChanged(int angle);

	// Signal emmited when the OpenGL is rotated around the z-axis
	void zRotationChanged(int angle);

protected:
	// Sets the initial OpenGL setting
	void initializeGL();

	// Method needed by Qt, which "paints" the OpenGL drawing
	void paintGL();

	// Resizes the OpenGL drawing
	void resizeGL(int width, int height);

	// Stores the current position of the mouse upon click
	void mousePressEvent(QMouseEvent *event);

	// Moves the OpenGL drawing by mouse drag
	void mouseMoveEvent(QMouseEvent *event);

	// Resizes the OpenGL drawing by mouse wheel
	void wheelEvent(QWheelEvent *event);

	// Resets the zoom of the OpenGL drawing when double-clicked
	void mouseDoubleClickEvent(QMouseEvent *event);

private:
	// Auxiliary method that contains the OpenGL code and draws all lines and surfaces
	GLuint makeObject();

	// Auxiliary method that transforms the angle to a number between 0 and 360 degrees
	void normalizeAngle(int *angle);

	// Auxiliary method that computes the maximum width and height coordinates of the OpenGL widget
	void calculateMax(int width, int height);

	// Data members of the OpenGL drawing
	GLuint object;
	int xRot;
	int yRot;
	int zRot;
	int xRotDefault;
	int yRotDefault;
	int zRotDefault;
	double xTrans;
	double yTrans;
	double zTrans;
	double scale;
	int globalScale;
	QPoint lastPos;
	QColor backgroundColor;
	QList<QVector<double> > theLineList;
	QList<QVector<double> > theQuadList;
	QList<QVector<double> > theTextCoordinateList;
	QList<QColor> theLineColorList;
	QList<QColor> theQuadColorList;
	QList<QColor> theQuadBorderColorList;
	QList<QColor> theTextColorList;
	QStringList theTextList;
	double maxX;
	double minX;
	double maxY;
	double minY;
	double maxZ;
	double minZ;
	double maxH;
	double maxV;
	double max;
	double margin;
};

#endif // RGLWidgetEngine_H
