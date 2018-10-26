#pragma once
#include <QString>
#include <QSqlQuery>
#include <QVariant>

//Класс сотрудника
class User
{
public:
  User();
  explicit User(const QSqlQuery & query);
  ~User();

  const static int kReadDatabase = 1 << 0;
  const static int kCreateuser = 1 << 1;

  bool canSomeAction(int action) const;
  int getID() const;
  int getAuthority() const;
  bool isDeleted() const;
  QString getFio() const;
  QString getDateReceive() const;

private:
  int id;
  int authority;
  bool isDelete;
  QString fio;
  QString date_receive;
};

