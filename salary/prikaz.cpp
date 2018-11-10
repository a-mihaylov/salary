#include "prikaz.h"

Prikaz::Prikaz(const QSqlQuery & query) {
  this->id = query.value(0).toInt();
  this->typeOfPrikaz = query.value(1).toBool();
  this->idUser = query.value(2).toInt();
  this->idCreator = query.value(3).toInt();
  this->date = query.value(4).toString();
}

Prikaz::Prikaz() {
}

Prikaz::~Prikaz() {
}

int Prikaz::getId() const {
  return this->id;
}

int Prikaz::getIdUser() const {
  return this->idUser;
}

int Prikaz::getIdCreator() const {
  return this->idCreator;
}

bool Prikaz::getTypeOfPrikaz() const {
  return this->typeOfPrikaz;
}

QString Prikaz::getDate() const {
  return this->date;
}
