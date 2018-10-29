#include "project.h"


Project::Project(const QSqlQuery & query) {
  this->id = query.value(0).toInt();
  this->name = query.value(1).toString();
  this->date_start = query.value(2).toString();
  this->date_end = query.value(3).toString();
  this->budget = query.value(4).toInt();
  this->count_dotation = query.value(5).toInt();
}


Project::~Project() {
}

int Project::getID() {
  return this->id;
}

QString Project::getProjectName() {
  return this->name;
}

QString Project::getDateStart() {
  return this->date_start;
}

QString Project::getDateEnd() {
  return this->date_end;
}

int Project::getBudget() {
  return this->budget;
}

int Project::getCountDotation() {
  return this->count_dotation;
}