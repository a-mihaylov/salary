#ifndef SALARY_H
#define SALARY_H

#include <QtWidgets/QMainWindow>
#include <QCryptographicHash>
#include <QMessageBox>
#include <QHash>
#include <QRegExp>
#include <QValidator>
#include <QQueue>
#include <QFile>
#include <QTextStream>

#include <QInputDialog>
#include "ui_salary.h"
#include "salarydatabase.h"
#include "myDelegate.h"
#include "prikaz.h"
#include "myCustomGraphics.h"
#include "prikazCreate.h"

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
  void goToGraphicPage();
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
  void accountingShow(const QVector<InfoForAccounting> & other = QVector<InfoForAccounting>());
  void saveMarkForUser(int row, int column);
  void changeStatusUncorfimedWorker(int row, int column);
  void searchPrikaz(const QString &FIO = "");
  void searchPrikazDate();
  void printPrikazToPdf();
  void calculatePayroll();
  void calculateGraphics();
  void closeEvent(QCloseEvent * e);
  void keyPressEvent(QKeyEvent * e);
  void worktopChanged(int index);


private:
  Ui::salaryClass ui;
  SalaryDatabase db;

  QQueue<int> history_window;
  QVector<User> users;
  QHash<QString, User> fioToUser;
  QHash<int, User> idToUser;
  QVector<Project> projects;
  QVector<Prikaz> prikazes;
  User * user;

  void fillProjectPage(int id);
  void fillWorkerPage(int id);
  int monthBetweenToDate(const QString & start, const QString & end);
  void setFioForComboBox(QComboBox * box);
  int preparePayroll(const ProjectWithDateWorkerForPayroll * list_project, QHash<int, int> & projectToMonth, QHash<int, LD> & projectPayrollOneMonth, QHash<int, LD> & summaryCoefForProject, bool change_mode = false);
  void drawPieChartForWorker(const ProjectWithDateWorkerForPayroll * list_project, const QHash<int, int> & projectToMonth, const QHash<int, LD> & projectPayrollOneMonth, const QHash<int, LD> & summaryCoefForProject);
  void drawPieChartProjectByProject(const ProjectWithDateWorkerForPayroll * list_project, const QHash<int, int> & projectToMonth, const QHash<int, LD> & projectPayrollOneMonth, const QHash<int, LD> & summaryCoefForProject);
  void drawPieChartProjectByWorker(const ProjectWithDateWorkerForPayroll * list_project, const QHash<int, int> & projectToMonth, const QHash<int, LD> & projectPayrollOneMonth, const QHash<int, LD> & summaryCoefForProject);

  private slots:
    void authorization();
    void moveRegistration();
    void moveAuthorization();
    void registration();
};

#endif // SALARY_H
