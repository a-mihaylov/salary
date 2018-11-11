#ifndef SALARY_H
#define SALARY_H

#include <QtWidgets/QMainWindow>
#include <QCryptographicHash>
#include <QMessageBox>
#include <QHash>
#include <QRegExp>
#include <QValidator>
#include "ui_salary.h"
#include "salarydatabase.h"
#include "myDelegate.h"

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
  void goToCurrentWorkerPage(QListWidgetItem * item);
  void goToCurrentWorkerPage(int row, int column);
  void goToCurrentProjectPage(QListWidgetItem * item);
  void goToCurrentProjectPage(int row, int column);
  void saveEditWorker();
  void changeWorkerStatus();
  void addProjectWorker();
  void removeProjectWorker();
  void searchProject(const QString & projectPattern);
  void searchWorker(const QString & workerPattern);
  void createNewProject();
  void saveProject();
  void accountingShow();
  void saveMarkForUser(int row, int column);
  void changeStatusUncorfimedWorker(int row, int column);
  void searchPrikaz();
  void reserSearchPrikaz();
  void searchPrikazDate();
  void printPrikazToPdf();
  void calculatePayroll();


private:
  Ui::salaryClass ui;
  SalaryDatabase db;

  QVector<User> users;
  QHash<QString, User> fioToUser;
  QVector<Project> projects;
  User * user;

  void fillProjectPage(int id);
  void fillWorkerPage(int id);
  int monthBetweenToDate(const QString & start, const QString & end);
  
  private slots:
    void authorization();
    void moveRegistration();
    void moveAuthorization();
    void registration();
};

#endif // SALARY_H
