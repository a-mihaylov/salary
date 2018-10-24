#pragma once
#include <QString>
#include <QSqlQuery>
#include <QVariant>

//Класс сотрудника
class User
{
public:
  explicit User(const QSqlQuery & query);
  ~User();

  const static int kReadDatabase = 1 << 0;
  const static int kCreateuser = 1 << 1;

  bool canCreateUser() const;

  bool canReadDatabase() const;

  QString fio;

private:
  int id;
  int authority;
};

