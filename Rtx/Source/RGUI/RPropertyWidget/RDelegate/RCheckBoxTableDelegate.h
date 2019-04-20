#ifndef RCheckBoxTableDelegate_H
#define RCheckBoxTableDelegate_H

#include <QItemDelegate>
#include <QModelIndex>
#include <QObject>
#include <QSize>
#include <QComboBox>

// This file is written based on the generic delegate example of Qt.

class RCheckBoxTableDelegate : public QItemDelegate
{
	Q_OBJECT

public:
	RCheckBoxTableDelegate(QObject *parent = 0);

	QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;

	void setEditorData(QWidget *editor, const QModelIndex &index) const;
	void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

	void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
	//QStringList theStringList;

};

#endif // RCheckBoxTableDelegate_H