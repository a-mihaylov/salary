#pragma once
#include "myDelegate.h"

NonEditTableColumnDelegate::NonEditTableColumnDelegate(QObject * parent) : QItemDelegate(parent) {}

QWidget * NonEditTableColumnDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const {
  return 0;
}


TableDelegateWithValidator::TableDelegateWithValidator(QObject* parent) : QStyledItemDelegate(parent) {}

QWidget * TableDelegateWithValidator::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const {
  QLineEdit* editor = new QLineEdit(parent);
  QIntValidator* val = new QIntValidator(0, 10, editor);
  editor->setValidator(val);
  return editor;
}

void TableDelegateWithValidator::setEditorData(QWidget *editor, const QModelIndex &index) const {
  int value = index.model()->data(index, Qt::EditRole).toInt();
  QLineEdit* line = static_cast<QLineEdit*>(editor);
  line->setText(QString::number(value));
}

void TableDelegateWithValidator::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
  QLineEdit* line = static_cast<QLineEdit*>(editor);
  QString value = line->text();
  model->setData(index, value);
}

void TableDelegateWithValidator::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const {
  editor->setGeometry(option.rect);
}