#include <QtGui>

#include "RDateTimeDelegate.h"

#include "RTime.h"

RDateTimeDelegate::RDateTimeDelegate(QObject *parent)
	: QItemDelegate(parent)
{
	format = RTime::getTimeFormat();
	setObjectName("RDelegate2");
}

QWidget *RDateTimeDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/* option */, const QModelIndex &/* index */) const
{
	QDateTimeEdit *editor = new QDateTimeEdit(parent);
	editor->setObjectName("RDelegate1");
	editor->setDisplayFormat(format);
	editor->setCalendarPopup(true);
	editor->installEventFilter(const_cast<RDateTimeDelegate *>(this));
	return editor;
}

void RDateTimeDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
	QString value = index.model()->data(index, Qt::EditRole).toString();

	QDateTimeEdit *dateTimeEditor = static_cast<QDateTimeEdit *>(editor);
	dateTimeEditor->setDateTime(QDateTime().fromString(value, format));
}

void RDateTimeDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
	QDateTimeEdit *dateTimeEditor = static_cast<QDateTimeEdit *>(editor);

	QDateTime time = dateTimeEditor->dateTime();
	QString value = time.toString(format);

	model->setData(index, value, Qt::EditRole);
}

void RDateTimeDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
	editor->setGeometry(option.rect);
}

void RDateTimeDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	//paint(painter, option, index);
	QItemDelegate::paint(painter, option, index);
}
