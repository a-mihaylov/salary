#include "salarydatabase.h"

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
    users.date_dismissial, users.date_birth, list_users.position, list_users.factor from list_users inner join users on users.id = list_users.id_user where id_project=?");
  query.addBindValue(id);
  query.exec();

  while (query.next()) {
    User user(query);
    user.setPosition(query.value(9).toString());
    user.setMultiply(query.value(10).toDouble());
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

QStringList SalaryDatabase::getAllPosition() {
  QStringList result;
  QSqlQuery query("SELECT position FROM list_position");
  query.exec();
  while (query.next()) {
    result.push_back(query.value(0).toString());
  }
  return result;
}

bool SalaryDatabase::addWorkerInProject(int id_worker, int id_project, const QString & position, int coef) {
  QSqlQuery query("INSERT INTO list_users(id_user, id_project, position, factor, mark, date) VALUES(?, ?, ?, ?, NULL, CURDATE())");
  query.addBindValue(id_worker);
  query.addBindValue(id_project);
  query.addBindValue(position);
  query.addBindValue(coef);
  return query.exec();
}

bool SalaryDatabase::removeWorkerInProject(int id_worker, int id_project, const QString & position) {
  QSqlQuery query("DELETE FROM list_users WHERE id_user=? AND id_project=? AND position=?");
  query.addBindValue(id_worker);
  query.addBindValue(id_project);
  query.addBindValue(position);
  return query.exec();
}