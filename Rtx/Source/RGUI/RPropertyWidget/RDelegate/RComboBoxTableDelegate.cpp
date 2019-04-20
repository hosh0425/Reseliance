#include <QtGui>
#include <QCompleter>

#include "RComboBoxTableDelegate.h"

RComboBoxTableDelegate::RComboBoxTableDelegate(QObject *parent, const QStringList &passedStringList, bool passedEditable)
	: QItemDelegate(parent)
{
	theStringList = passedStringList;
	setObjectName("RDelegate0");
	editable = passedEditable;
}

RComboBoxTableDelegate::~RComboBoxTableDelegate()
{
	
}

QWidget * RComboBoxTableDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/* option */, const QModelIndex &/* index */) const
{
	QComboBox *editor = new QComboBox(parent);
	editor->addItems(theStringList);
	editor->setEditable(editable);
	editor->setObjectName("RDelegate1");
	//editor->lineEdit()->selectAll();

	QCompleter *completer = new QCompleter(theStringList, editor);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    editor->setCompleter(completer);

	// Temporaty lines to remember how to implement path delegates
	//QCompleter *completer = new QCompleter(editor);
	//QDirModel *dirModel = new QDirModel(completer);
	//completer->setModel(dirModel);

	// ISNTALL OR REMOVE?
	editor->installEventFilter(const_cast<RComboBoxTableDelegate *>(this));
	return editor;
}

void RComboBoxTableDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
	QString value = index.model()->data(index, Qt::EditRole).toString();

	QComboBox *comboBox = static_cast<QComboBox *>(editor);
	if (editable) {
		comboBox->setEditText(value);
	} else {
		comboBox->setCurrentIndex(comboBox->findText(value));
	}
}

void RComboBoxTableDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
	QComboBox *comboBox = static_cast<QComboBox *>(editor);
	//comboBox->interpretText();
	QString value = comboBox->currentText();

	model->setData(index, value, Qt::EditRole);
}

void RComboBoxTableDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
	editor->setGeometry(option.rect);
}

void RComboBoxTableDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	//paint(painter, option, index);
	QItemDelegate::paint(painter, option, index);
}
