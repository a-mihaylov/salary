#include "project.h"


Project::Project(const QSqlQuery & query, int offset) {
  this->id = query.value(0 + offset).toInt();
  this->name = query.value(1 + offset).toString();
  this->date_start = query.value(2 + offset).toString();
  this->date_end = query.value(3 + offset).toString();
  this->budget = query.value(4 + offset).toInt();
  this->count_dotation = query.value(5 + offset).toInt();
}

Project::Project() {

}

Project::~Project() {
}

int Project::getID() const {
  return this->id;
}

QString Project::getProjectName() const {
  return this->name;
}

QString Project::getDateStart() const {
  return this->date_start;
}

QString Project::getDateEnd() const {
  return this->date_end;
}

int Project::getBudget() const {
  return this->budget;
}

int Project::getCountDotation() const {
  return this->count_dotation;
}

void Project::setProjectName(const QString & other) {
  this->name = other;
}

void Project::setDateStart(const QString & other) {
  this->date_start = other;
}

void Project::setDateEnd(const QString & other) {
  this->date_end = other;
}

void Project::setBudget(int other) {
  this->budget = other;
}

void Project::setCountDotation(int other) {
  this->count_dotation = other;
}

bool Project::operator<(const Project & other) const {
  return this->name < other.getProjectName();
}
