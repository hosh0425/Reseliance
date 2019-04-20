#include <QtGui>

#include "RColorComboBoxTableDelegate.h"

RColorComboBoxTableDelegate::RColorComboBoxTableDelegate(QObject *parent)
	: QItemDelegate(parent)
{

}

RColorComboBoxTableDelegate::~RColorComboBoxTableDelegate()
{
	
}

QWidget * RColorComboBoxTableDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/* option */, const QModelIndex &/* index */) const
{
	QtColorComboBox *editor = new QtColorComboBox(parent);
	editor->setObjectName("RDelegate1");
	editor->setStandardColors();
	editor->setColorDialogEnabled(false);

	editor->installEventFilter(const_cast<RColorComboBoxTableDelegate *>(this));
	return editor;
}

void RColorComboBoxTableDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
	QVariant valueVariant = index.model()->data(index, Qt::EditRole);
	QColor value = valueVariant.value<QColor>();

	QtColorComboBox *comboBox = static_cast<QtColorComboBox *>(editor);
	comboBox->setCurrentColor(value);
}

void RColorComboBoxTableDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
	QtColorComboBox *comboBox = static_cast<QtColorComboBox *>(editor);
	QColor value = comboBox->currentColor();

	model->setData(index, value, Qt::EditRole);
}

void RColorComboBoxTableDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
	editor->setGeometry(option.rect);
}

void RColorComboBoxTableDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	//paint(painter, option, index);
	QItemDelegate::paint(painter, option, index);
}
