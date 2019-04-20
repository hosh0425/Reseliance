// Last modified by Mojtaba on June 30, 2012

#ifndef RAboutDialog_H
#define RAboutDialog_H

#include <QDialog>

class RAboutDialog : public QDialog
{
	Q_OBJECT
	
public:
	RAboutDialog(QWidget *parent = 0);
	~RAboutDialog();

protected:
	// Closes the about box when clicked on
	void mousePressEvent(QMouseEvent *event);

};

#endif //RAboutDialog_H
