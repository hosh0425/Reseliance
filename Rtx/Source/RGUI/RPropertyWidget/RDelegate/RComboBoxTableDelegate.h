#ifndef RComboBoxTableDelegate_H
#define RComboBoxTableDelegate_H

#include <QItemDelegate>
#include <QModelIndex>
#include <QObject>
#include <QSize>
#include <QComboBox>

// This file is written based on the generic delegate example of Qt.

class RComboBoxTableDelegate : public QItemDelegate
{
	Q_OBJECT

public:
	RComboBoxTableDelegate(QObject *parent = 0, const QStringList &passedStringList = QStringList() << "", bool passedEditable = true);	
	~RComboBoxTableDelegate();

	QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;

	void setEditorData(QWidget *editor, const QModelIndex &index) const;
	void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

	void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
	QStringList theStringList;
	bool editable;

};

#endif // RComboBoxTableDelegate_H