#ifndef PRIKAZCREATE_H
#define PRIKAZCREATE_H

#include <QDialog>
#include "ui_prikazCreate.h"

class PrikazCreate : public QDialog
{
  Q_OBJECT

public:
  PrikazCreate(QWidget *parent = 0);
  ~PrikazCreate();
  QDate date();
  bool isCreateButtonClicked();
  void setMinimumDate(const QString & date);

public slots:
  void createPrikazResult();
  void canselPrikazCreate();

private:
  Ui::PrikazCreate prikazui;
  bool createButtonClicked;
};

#endif // PRIKAZCREATE_H
