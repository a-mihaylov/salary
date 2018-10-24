#ifndef SALARY_H
#define SALARY_H

#include <QtWidgets/QMainWindow>
#include <QCryptographicHash>
#include <QMessageBox>
#include "ui_salary.h"
#include "salarydatabase.h"

class salary : public QMainWindow {
  Q_OBJECT

public:
  salary(QWidget *parent = 0);
  ~salary();

  public slots:

  void goToWorkerPage();
  void goToPrikazPage();
  void goToProjectPage();
  void goToSalaryPage();
  void goToAccountingPage();

  //Слот тестирования различного функционала
  void test_slot();


private:
  Ui::salaryClass ui;
  SalaryDatabase db;

  User * user;

  private slots:
    void authorization();
    void moveRegistration();
    void moveAuthorization();
};

#endif // SALARY_H
