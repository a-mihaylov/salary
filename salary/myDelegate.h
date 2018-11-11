#pragma once
#include <QItemDelegate>
#include <QStyledItemDelegate>
#include <QValidator>
#include <QLineEdit>
#include <QCheckBox>
#include <QEvent>

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

class BooleanItemDelegate : public QItemDelegate {
public:
  BooleanItemDelegate(QObject *parent = 0);
  QWidget * createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const; 
  void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
  bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);
};