#include "prikaz.h"

Prikaz::Prikaz(const QSqlQuery & query) {
  this->id = query.value(0).toInt();
  this->idCreator = query.value(1).toInt();
  this->idUser = query.value(2).toInt();
  this->FIO = query.value(3).toString();
  this->typeOfPrikaz = query.value(4).toBool();
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

QString Prikaz::getPrikazString(Prikaz &prikaz) {
  if (prikaz.getTypeOfPrikaz()) {
    return (QString("\t") + QString::fromWCharArray(L"Приём	   ") + prikaz.date + QString("\t") + prikaz.FIO);
  }
  else {
    return (QString("\t") + QString::fromWCharArray(L"Увольнение	   ") + prikaz.date + QString("\t") + prikaz.FIO);
  }
}