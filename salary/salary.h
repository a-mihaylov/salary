#ifndef SALARY_H
#define SALARY_H

#include <QtWidgets/QMainWindow>
#include "ui_salary.h"

class salary : public QMainWindow {
  Q_OBJECT

public:
  salary(QWidget *parent = 0);
  ~salary();

  public slots:


  //Слот тестирования различного функционала
  void test_slot();

private:
  Ui::salaryClass ui;
};

#endif // SALARY_H
