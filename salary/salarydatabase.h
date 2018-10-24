#pragma once
#include <qsqldatabase.h>
#include <qsqlquery.h>
#include <qsql.h>
#include <QVariant>
#include "user.h"

class SalaryDatabase {
public:
  SalaryDatabase();
  ~SalaryDatabase();

  QSqlDatabase& getDB();
  User * Authorization(const QString & username, const QString & hash_password) const;
  bool openDB();

private:
  QSqlDatabase db;
};

