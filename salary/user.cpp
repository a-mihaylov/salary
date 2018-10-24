#include "user.h"


User::User(const QSqlQuery & query) {
  this->id = query.value(0).toInt();
  this->authority = query.value(1).toInt();
  this->fio = query.value(2).toString();
}


User::~User() {
}

bool User::canCreateUser() const {
  return (this->authority & this->kCreateuser) == this->kCreateuser;
}

bool User::canReadDatabase() const {
  return (this->authority & this->kReadDatabase) == this->kReadDatabase;
}