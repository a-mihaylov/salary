#pragma once
#include <QItemDelegate>
#include <QStyledItemDelegate>
#include <QValidator>
#include <QLineEdit>

class NonEditTableColumnDelegate : public QItemDelegate
{
  Q_OBJECT
public:
  NonEditTableColumnDelegate(QObject * parent = 0);
  QWidget * createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

class TableDelegateWithValidator : public QStyledItemDelegate
{
  Q_OBJECT

public:
  TableDelegateWithValidator(QObject* parent = 0);
  QWidget * createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
  void setEditorData(QWidget *editor, const QModelIndex &index) const;
  void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
  void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};