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

class SalaryDatabase {
public:
  SalaryDatabase();
  ~SalaryDatabase();

  QSqlDatabase& getDB();
  User * Authorization(const QString & username, const QString & hash_password) const;
  bool Registration(const QString & username, const QString & hash_password, const QString & fio) const;
  QVector<User> getAllUsers();
  User * getConcreteUser(int id);
  bool updateUser(const User & user);
  QVector<Project> getAllProjects();
  bool openDB();

private:
  QSqlDatabase db;
};

