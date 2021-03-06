﻿#pragma once
#include <QString>
#include <QSqlQuery>
#include <QVariant>
#include <QVector>
#include "project.h"

//Класс сотрудника
class User {
public:
  User();
  explicit User(const QSqlQuery & query);
  ~User();

  const static int kWorker = 1 << 0;
  const static int kAdmin = 1 << 1;

  bool compareRole(int role) const;

  int getID() const;
  int getAuthority() const;
  bool isDeleted() const;
  bool isConfirmed() const;
  QString getFio() const;
  QString getDateReceipt() const;
  QString getDateDismissial() const;
  QString getDateBirth() const;
  QString getPosition() const;
  double getMultiply() const;

  void setDeleted(bool other);
  void setConfirmed(bool other);
  void setFio(const QString & other);
  void setDateReceipt(const QString & other);
  void setDateDismissial(const QString & other);
  void setDateBirth(const QString & other);
  void setPosition(const QString & other);
  void setMultiply(double other);

  bool operator<(const User & other) const;

private:
  int id;
  int authority;
  bool isDelete;
  bool isConfirm;
  QString fio;
  QString date_receipt;
  QString date_dissmissial;
  QString date_birth;
  QString position;
  double multiply;
};

