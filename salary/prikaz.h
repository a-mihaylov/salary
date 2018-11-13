#pragma once
#include <QString>
#include <QSqlQuery>
#include <QVariant>

class Prikaz
{
public:
  Prikaz();
  explicit Prikaz(const QSqlQuery & query);
  ~Prikaz();

  int getId() const;
  QString getFIO() const;
  int getIdUser() const;
  int getIdCreator() const;
  bool getTypeOfPrikaz() const;
  QString getDate() const;
  QString getPrikazString(Prikaz &prikaz);

private:
  int id;
  QString FIO;
  int id_creator;
  int id_user;
  bool type_of_prikaz;
  QString date;

};

