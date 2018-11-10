#pragma once
#include <qsqldatabase.h>
#include <qsqlquery.h>
#include <qsql.h>
#include <QVariant>
#include <QMessageBox>
#include <QSqlError>
#include <QVector>
#include "user.h"
#include "project.h"
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
  bool addWorkerInProject(int id_worker, int id_project, const QString & position, int coef);
  bool removeWorkerInProject(int id_worker, int id_project, const QString & position);
  bool openDB();

private:
  QSqlDatabase db;
};

