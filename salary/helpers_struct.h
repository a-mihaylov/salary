#pragma once
#include "user.h"
#include "project.h"
#include <QHash>
#include <QPair>

struct valueForHashInWorker {
  int mark;
  QString position;
  valueForHashInWorker() {

  }
};

struct AllInfoForWorker {
  User user;
  QHash< int, valueForHashInWorker > helpInfo;
  QVector<Project> projects;

  AllInfoForWorker() {

  }
};

struct InfoForAccounting {
  int id_user;
  QString fio;
  QString position;
  int mark;
  int id_project;
  QString project_name;

  InfoForAccounting() {

  }
};