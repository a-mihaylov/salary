#include <QItemDelegate>

class NonEditTableColumnDelegate : public QItemDelegate
{
  Q_OBJECT
public:
  NonEditTableColumnDelegate(QObject * parent = 0) : QItemDelegate(parent) {}
  virtual QWidget * createEditor(QWidget *, const QStyleOptionViewItem &,
    const QModelIndex &) const {
    return 0;
  }
};
