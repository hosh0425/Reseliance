#ifndef RColorComboBoxTableDelegate_H
#define RColorComboBoxTableDelegate_H

#include <QItemDelegate>
#include <QModelIndex>
#include <QObject>
#include <QSize>
#include <QtColorComboBox.h>

// This file is written based on the generic delegate example of Qt.

class RColorComboBoxTableDelegate : public QItemDelegate
{
	Q_OBJECT

public:
	RColorComboBoxTableDelegate(QObject *parent = 0);	
	~RColorComboBoxTableDelegate();

	QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;

	void setEditorData(QWidget *editor, const QModelIndex &index) const;
	void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

	void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:

};

#endif // RColorComboBoxTableDelegate_H