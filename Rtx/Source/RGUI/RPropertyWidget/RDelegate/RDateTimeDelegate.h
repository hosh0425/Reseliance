#ifndef RDateTimeDelegate_H
#define RDateTimeDelegate_H

#include <QItemDelegate>
#include <QModelIndex>
#include <QObject>
#include <QSize>
#include <QDateTimeEdit>

// This file is written based on the generic delegate example of Qt.

class RDateTimeDelegate : public QItemDelegate
{
	Q_OBJECT

public:
	RDateTimeDelegate(QObject *parent = 0);

	QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;

	void setEditorData(QWidget *editor, const QModelIndex &index) const;
	void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

	void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
	QString format;

};

#endif // RDateTimeDelegate_H