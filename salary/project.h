#pragma once
#include <QString>
#include <QSqlQuery>
#include <QVariant>

class Project {
public:
  Project();
  explicit Project(const QSqlQuery & query);
  ~Project();

  int getID() const;
  QString getProjectName() const;
  QString getDateStart() const;
  QString getDateEnd() const;
  int getBudget() const;
  int getCountDotation() const;

private:
  int id;
  QString name;
  QString date_start;
  QString date_end;
  int budget;
  int count_dotation;
};

