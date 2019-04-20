#include <QtGui>
#include <QDesktopWidget>
#include <QApplication>
#include <QLabel>

#include "RAboutDialog.h"


RAboutDialog::RAboutDialog(QWidget *parent)
	: QDialog(parent)
{
	setWindowTitle("About " + QApplication::applicationName());
	int width = 750;
	int height = 500;
	this->setFixedSize(width, height);
	setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
	//setWindowFlags(Qt::WindowSystemMenuHint);
	QPixmap pixmap = QPixmap(":/RImages/splash2.png");
	//QDesktopWidget *desktop = QApplication::desktop();
	//qreal scaleRatio = (qreal)desktop->logicalDpiX() / 96.0;
	//qreal screenWidth = (qreal)desktop->availableGeometry(desktop->primaryScreen()).height();
	pixmap.setDevicePixelRatio(2);
	QLabel *label = new QLabel(this);
	label->setPixmap(pixmap);
	label->setFixedSize(width, height);
}

RAboutDialog::~RAboutDialog()
{
}

void RAboutDialog::mousePressEvent(QMouseEvent *event)
{
	close();
}
