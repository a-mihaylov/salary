#include "prikaz.h"

Prikaz::Prikaz(const QSqlQuery & query) {
  this->id = query.value(0).toInt();
  this->id_creator = query.value(1).toInt();
  this->id_user = query.value(2).toInt();
  this->FIO = query.value(3).toString();
  this->type_of_prikaz = query.value(4).toBool();
  this->date = query.value(5).toString();
}

Prikaz::Prikaz() {
}

Prikaz::~Prikaz() {
}

int Prikaz::getId() const {
  return this->id;
}

QString Prikaz::getFIO() const {
  return this->FIO;
}

int Prikaz::getIdUser() const {
  return this->id_user;
}

int Prikaz::getIdCreator() const {
  return this->id_creator;
}

bool Prikaz::getTypeOfPrikaz() const {
  return this->type_of_prikaz;
}

QString Prikaz::getDate() const {
  return this->date;
}