#include "user.h"


User::User(const QSqlQuery & query) {
  this->id = query.value(0).toInt();
  this->isDelete = query.value(3).toInt();
  this->authority = query.value(4).toInt();
  this->fio = query.value(5).toString();
  this->date_receipt = query.value(6).toString();
  this->date_dissmissial = query.value(7).toString();
  this->date_birth = query.value(8).toString();
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

QString User::getDateReceipt() const {
  return this->date_receipt;
}

QString User::getDateDismissial() const {
  return this->date_dissmissial;
}

QString User::getDateBirth() const {
  return this->date_birth;
}

QString User::getPosition() const {
  return this->position;
}

int User::getMultiply() const {
  return this->multiply;
}

void User::setDeleted(bool other) {
  this->isDelete = other;
}

void User::setFio(const QString & other) {
  this->fio = other;
}

void User::setDateReceipt(const QString & other) {
  this->date_receipt = other;
}

void User::setDateDismissial(const QString & other) {
  this->date_dissmissial = other;
}

void User::setDateBirth(const QString & other) {
  this->date_birth = other;
}

void User::setPosition(const QString & other) {
  this->position = other;
}
void User::setMultiply(int other) {
  this->multiply = other;
}