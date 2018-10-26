#pragma once
#include <qsqldatabase.h>
#include <qsqlquery.h>
#include <qsql.h>
#include <QVariant>
#include <QMessageBox>
#include <QSqlError>
#include <QVector>
#include "user.h"

class SalaryDatabase {
public:
  SalaryDatabase();
  ~SalaryDatabase();

  QSqlDatabase& getDB();
  User * Authorization(const QString & username, const QString & hash_password) const;
  bool Registration(const QString & username, const QString & hash_password, const QString & fio) const;
  QVector<User> getAllUsers();
  bool openDB();

private:
  QSqlDatabase db;
};

