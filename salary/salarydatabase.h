#pragma once
#include <qsqldatabase.h>
#include <qsqlquery.h>
#include <qsql.h>
#include <QVariant>
#include <QMessageBox>
#include <QSqlError>
#include "user.h"

class SalaryDatabase {
public:
  SalaryDatabase();
  ~SalaryDatabase();

  QSqlDatabase& getDB();
  User * Authorization(const QString & username, const QString & hash_password) const;
  bool Registration(const QString & username, const QString & hash_password, const QString & fio) const;
  bool openDB();

private:
  QSqlDatabase db;
};

