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
  query.prepare("SELECT * FROM users WHERE username=? and password_hash=? and isDeleted=0 and isConfirmed=1");
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

  query.prepare("INSERT INTO users(username, password_hash, isDeleted, authority, fio, date_receipt, date_dismissial, date_birth, isConfirmed) VALUES(?, ?, ?, ?, ?, CURDATE(), NULL, NULL, 0)");
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

AllInfoForWorker * SalaryDatabase::getConcreteUser(int id) {
  AllInfoForWorker * forInfo = nullptr;

  QSqlQuery query(db);
  query.prepare("SELECT * FROM users WHERE id=?");
  query.addBindValue(id);
  query.exec();
  if (query.next()) {
    forInfo = new AllInfoForWorker();
    forInfo->user = User(query);

    query.prepare("SELECT * FROM project INNER JOIN list_users on project.id = list_users.id_project WHERE list_users.id_user=?\
                                    GROUP BY project.name, list_users.position");
    query.addBindValue(id);
    query.exec();
    while (query.next()) {
      forInfo->projects.push_back(Project(query));
      forInfo->helpInfo[forInfo->projects.back().getID()].position = query.value(9).toString();
      forInfo->helpInfo[forInfo->projects.back().getID()].mark = query.value(11).toInt();
    }
  }

  return forInfo;
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
                users.date_dismissial, users.date_birth, users.isConfirmed list_users.position, list_users.factor from list_users inner join users on \
                users.id = list_users.id_user where id_project=? and date_end>CURDATE() group by users.fio, list_users.position");
  query.addBindValue(id);
  bool a = query.exec();
  QString error = query.lastError().text();

  while (query.next()) {
    User user(query);
    user.setPosition(query.value(10).toString());
    user.setMultiply(query.value(11).toDouble());
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

bool SalaryDatabase::createProject(const Project & project) {
  QSqlQuery query("INSERT INTO project(name, date_start, date_end, budget, count_dotation) VALUES(?, ?, ?, ?, ?)");
  query.addBindValue(project.getProjectName());
  query.addBindValue(project.getDateStart());
  query.addBindValue(project.getDateEnd());
  query.addBindValue(project.getBudget());
  query.addBindValue(project.getCountDotation());
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

bool SalaryDatabase::addWorkerInProject(int id_worker, int id_project, const QString & position, int coef, const QString & project_end) {
  QString req = "INSERT INTO list_users(id_user, id_project, position, factor, mark, date_start, date_end) VALUES(?, ?, ?, ?, NULL, CURDATE(), \"#date_end#\")";
  QDate curdate = QDate::currentDate();
  QDate tmp = curdate;
  QDate project_end_date = QDate::fromString(project_end, QString("yyyy-MM-dd"));
  int cnt = 1;
  tmp.setDate(tmp.year(), tmp.month(), tmp.daysInMonth());
  if (tmp > project_end_date) {
    req.replace(QRegExp("#date_end#"), project_end);
  }
  else {
    req.replace(QRegExp("#date_end#"), tmp.toString("yyyy-MM-dd"));
    tmp = curdate;
    tmp = tmp.addMonths(1);
    QString pattern(",(?, ?, ?, ?, NULL, \"#date_start#\", \"#date_end#\")");
    while (tmp.year() != project_end_date.year() || tmp.month() != project_end_date.month()) {
      QString query = pattern;
      tmp.setDate(tmp.year(), tmp.month(), 1);
      query.replace(QRegExp("#date_start#"), tmp.toString("yyyy-MM-dd"));
      tmp.setDate(tmp.year(), tmp.month(), tmp.daysInMonth());
      query.replace(QRegExp("#date_end#"), tmp.toString("yyyy-MM-dd"));
      tmp.setDate(tmp.year(), tmp.month(), 1);
      tmp = tmp.addMonths(1);
      req.append(query);
      ++cnt;
    }
    pattern.replace(QRegExp("#date_start#"), tmp.toString("yyyy-MM-dd"));
    pattern.replace(QRegExp("#date_end#"), project_end);
    req.append(pattern);
    ++cnt;
  }

  QSqlQuery query(req);
  while (cnt--) {
    query.addBindValue(id_worker);
    query.addBindValue(id_project);
    query.addBindValue(position);
    query.addBindValue(coef);
  }
  return query.exec();
}

bool SalaryDatabase::removeWorkerInProject(int id_worker, int id_project, const QString & position) {
  QSqlQuery query("DELETE FROM list_users WHERE id_user=? AND id_project=? AND position=? AND date_start>=CURDATE()");
  query.addBindValue(id_worker);
  query.addBindValue(id_project);
  query.addBindValue(position);
  bool isOk = query.exec();
  query.prepare("UPDATE list_users SET date_end=CURDATE() WHERE id_user=? AND id_project=? AND position=? ORDER BY date_end DESC LIMIT 1");
  query.addBindValue(id_worker);
  query.addBindValue(id_project);
  query.addBindValue(position);
  return isOk && query.exec();
}

QVector<InfoForAccounting> SalaryDatabase::getForAccounting(int mounth, int year) {
  QSqlQuery query("SELECT users.id, users.fio, list_users.position, list_users.mark, project.id, project.name, list_users.date_start, list_users.date_end from list_users \
                  inner join users on users.id = list_users.id_user inner join project on project.id = list_users.id_project where list_users.date_start>=? and list_users.date_end<=?");
  QDate date;
  date.setDate(year, mounth, 1);
  query.addBindValue(date.toString("yyyy-MM-dd"));
  date.setDate(year, mounth, date.daysInMonth());
  query.addBindValue(date.toString("yyyy-MM-dd"));
  query.exec();

  QVector<InfoForAccounting> result;
  while (query.next()) {
    InfoForAccounting tmp;
    tmp.id_user = query.value(0).toInt();
    tmp.fio = query.value(1).toString();
    tmp.position = query.value(2).toString();
    tmp.mark = query.value(3).toInt();
    tmp.id_project = query.value(4).toInt();
    tmp.project_name = query.value(5).toString();
    tmp.date_start = query.value(6).toString();
    tmp.date_end = query.value(7).toString();
    result.push_back(tmp);
  }
  return result;
}

bool SalaryDatabase::updateWorkerInAccounting(const InfoForAccounting & info) {
  QSqlQuery query("UPDATE list_users SET mark=? WHERE id_user=? AND id_project=? AND position=? AND date_start=? AND date_end=?");
  query.addBindValue(info.mark);
  query.addBindValue(info.id_user);
  query.addBindValue(info.id_project);
  query.addBindValue(info.position);
  query.addBindValue(info.date_start);
  query.addBindValue(info.date_end);
  return query.exec();
}
