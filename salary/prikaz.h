#pragma once
#include "salary.h"

class Prikaz
{
public:
  Prikaz();
  explicit Prikaz(const QSqlQuery & query);
  ~Prikaz();

  int getId() const;
  int getIdUser() const;
  int getIdCreator() const;
  bool getTypeOfPrikaz() const;
  QString getDate() const;

private:
  int id;
  int idUser;
  int idCreator;
  bool typeOfPrikaz;
  QString date;

};

