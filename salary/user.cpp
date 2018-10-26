#include "user.h"


User::User(const QSqlQuery & query) {
  this->id = query.value(0).toInt();
  this->authority = query.value(1).toInt();
  this->fio = query.value(2).toString();
  this->isDelete = query.value(3).toInt();
}

User::User() {

}

User::~User() {
}

bool User::canSomeAction(int action) const {
  return (this->authority & action) == action;
}

int User::getID() const {
  return this->id;
}

int User::getAuthority() const {
  return this->authority;
}

bool User::isDeleted() const {
  return this->isDelete;
}

QString User::getFio() const {
  return this->fio;
}
