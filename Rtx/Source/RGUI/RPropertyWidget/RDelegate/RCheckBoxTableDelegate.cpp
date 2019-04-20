#include <QtGui>
#include <QCheckBox>

#include "RCheckBoxTableDelegate.h"

RCheckBoxTableDelegate::RCheckBoxTableDelegate(QObject *parent)
	: QItemDelegate(parent)
{

}

QWidget *RCheckBoxTableDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/* option */, const QModelIndex &/* index */) const
{
	QCheckBox *editor = new QCheckBox(parent);
	editor->setObjectName("RDelegate1");
	editor->installEventFilter(const_cast<RCheckBoxTableDelegate *>(this));
	return editor;
}

void RCheckBoxTableDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
	bool value = index.model()->data(index, Qt::EditRole).toBool();

	QCheckBox *checkBox = static_cast<QCheckBox *>(editor);
	checkBox->setChecked(value);
}

void RCheckBoxTableDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
	QCheckBox *checkBox = static_cast<QCheckBox *>(editor);
	//checkBox->interpretText();
	bool value = checkBox->isChecked();

	model->setData(index, value, Qt::EditRole);
}

void RCheckBoxTableDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
	editor->setGeometry(option.rect);
}

void RCheckBoxTableDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	//paint(painter, option, index);
	QItemDelegate::paint(painter, option, index);
}
