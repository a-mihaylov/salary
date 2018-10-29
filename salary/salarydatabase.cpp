﻿#include "salarydatabase.h"

SalaryDatabase::SalaryDatabase() {
  db = QSqlDatabase::addDatabase("QMYSQL");
  db.setHostName("80.211.88.182");
  db.setPort(3306);
  db.setUserName("root");
  db.setPassword("#EDC3edc");
  db.setDatabaseName("salary");
}

SalaryDatabase::~SalaryDatabase() {
}

QSqlDatabase& SalaryDatabase::getDB() {
  return this->db;
}

User * SalaryDatabase::Authorization(const QString & username, const QString & password_hash) const {
  QSqlQuery query(db);
  query.prepare("SELECT * FROM users WHERE username=? and password_hash=? and isDeleted=0");
  query.addBindValue(username);
  query.addBindValue(password_hash);
  query.exec();

  User * user = nullptr;
  if (query.next()) {
    user = new User(query);
  }

  return user;
}

bool SalaryDatabase::Registration(const QString & username, const QString & password_hash, const QString & fio) const {
  QSqlQuery query(db);
  query.prepare("SELECT id FROM users WHERE username=?");
  query.addBindValue(username);
  query.exec();
  if (query.next()) {
    return false;
  }

  query.prepare("INSERT INTO users(username, password_hash, isDeleted, authority, fio, date_receipt, date_dismissial, date_birth) VALUES(?, ?, ?, ?, ?, CURDATE(), NULL, NULL)");
  query.addBindValue(username);
  query.addBindValue(password_hash);
  query.addBindValue(0);
  query.addBindValue(1);
  query.addBindValue(fio);
  return query.exec();
}

bool SalaryDatabase::openDB() {
  return db.open();
}

QVector<User> SalaryDatabase::getAllUsers() {
  QSqlQuery query(db);
  query.prepare("SELECT * FROM users");
  query.exec();

  QVector<User> users;
  while (query.next()) {
    users.push_back(User(query));
  }

  return users;
}

User * SalaryDatabase::getConcreteUser(int id) {
  QSqlQuery query(db);
  query.prepare("SELECT * FROM users where id=?");
  query.addBindValue(id);
  query.exec();

  User * user = nullptr;
  if (query.next()) {
    user = new User(query);
  }
  return user;
}

bool SalaryDatabase::updateUser(const User & user) {
  QSqlQuery query(db);
  query.prepare("UPDATE users SET isDeleted=?, fio=?, date_receipt=?, date_dismissial=?, date_birth=? WHERE id=?");
  query.addBindValue(int(user.isDeleted()));
  query.addBindValue(user.getFio());
  query.addBindValue(user.getDateReceipt());
  query.addBindValue(user.getDateDismissial());
  query.addBindValue(user.getDateBirth());
  query.addBindValue(user.getID());
  return query.exec();
}

QVector<Project> SalaryDatabase::getAllProjects() {
  QVector<Project> projects;
  QSqlQuery query(db);
  query.prepare("SELECT * FROM project");
  query.exec();
  
  while (query.next()) {
    projects.push_back(Project(query));
  }

  return projects;
}

QVector<User> SalaryDatabase::getConcreteProject(int id) {
  QVector<User> users;
  QSqlQuery query(db);
  query.prepare("SELECT users.id, users.username, users.password_hash, users.isDeleted, users.authority, users.fio, users.date_receipt, \
    users.date_dismissial, users.date_birth from list_users inner join users on users.id = list_users.id_user where id_project=?");
  query.addBindValue(id);
  bool a = query.exec();

  while (query.next()) {
    User user(query);
    // TODO расскоментить после обновления таблицы list_users
    // user.setPosition(query.value(2).toString());
    // user.setMultiply(query.value(3).toInt());
    users.push_back(user);
  }
  return users;
}

bool SalaryDatabase::updateProject(const Project & project) {
  QSqlQuery query(db);
  query.prepare("UPDATE project SET name=?, date_start=?, date_end=?, budget=?, count_dotation=? WHERE id=?");
  query.addBindValue(project.getProjectName());
  query.addBindValue(project.getDateStart());
  query.addBindValue(project.getDateEnd());
  query.addBindValue(project.getBudget());
  query.addBindValue(project.getCountDotation());
  query.addBindValue(project.getID());
  return query.exec();
}