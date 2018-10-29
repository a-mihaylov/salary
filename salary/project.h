#pragma once
#include <QString>
#include <QSqlQuery>
#include <QVariant>

class Project {
public:
  explicit Project(const QSqlQuery & query);
  ~Project();

  int getID();
  QString getProjectName();
  QString getDateStart();
  QString getDateEnd();
  int getBudget();
  int getCountDotation();

private:
  int id;
  QString name;
  QString date_start;
  QString date_end;
  int budget;
  int count_dotation;
};

