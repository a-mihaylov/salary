#pragma once
#include <QString>
#include <QSqlQuery>
#include <QVariant>

class Project {
public:
  Project();
  explicit Project(const QSqlQuery & query, int offset = 0);
  ~Project();

  int getID() const;
  QString getProjectName() const;
  QString getDateStart() const;
  QString getDateEnd() const;
  int getBudget() const;
  int getCountDotation() const;

  void setProjectName(const QString & other);
  void setDateStart(const QString & other);
  void setDateEnd(const QString & other);
  void setBudget(int other);
  void setCountDotation(int other);

  bool operator<(const Project & other) const;

private:
  int id;
  QString name;
  QString date_start;
  QString date_end;
  int budget;
  int count_dotation;
};

