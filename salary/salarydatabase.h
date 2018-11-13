#pragma once
#include <qsqldatabase.h>
#include <qsqlquery.h>
#include <qsql.h>
#include <QVariant>
#include <QMessageBox>
#include <QSqlError>
#include <QVector>
#include <QDate>
#include "user.h"
#include "project.h"
#include "prikaz.h"
#include "helpers_struct.h"

class SalaryDatabase {
public:
  SalaryDatabase();
  ~SalaryDatabase();

  QSqlDatabase& getDB();
  User * Authorization(const QString & username, const QString & hash_password) const;
  bool Registration(const QString & username, const QString & hash_password, const QString & fio) const;
  QVector<User> getAllUsers();
  AllInfoForWorker * getConcreteUser(int id);
  bool updateUser(const User & user);
  QVector<Project> getAllProjects();
  QVector<User> getConcreteProject(int id);
  bool updateProject(const Project & project);
  bool createProject(const Project & project);
  QStringList getAllPosition();
  bool addWorkerInProject(int id_worker, int id_project, const QString & position, int coef, const QString & project_end);
  bool removeWorkerInProject(int id_worker, int id_project, const QString & position);
  QVector<InfoForAccounting> getForAccounting(int mounth, int year, bool searchOnlyNull = false);
  bool updateWorkerInAccounting(const InfoForAccounting & info);
  bool removeUncorfimedWorker(int id);
  bool confirmedWorker(int id);
  QVector<Prikaz> getAllPrikazes();
  bool createPrikaz(bool typeOfPrikaz, int idUser, int idCreator, QString date);
  ProjectWithDateWorkerForPayroll * getProjectForWorkerOnDate(int id, int mounth, int year);
  bool openDB();

private:
  QSqlDatabase db;
};