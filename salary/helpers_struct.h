#pragma once
#include "user.h"
#include "project.h"
#include <QHash>
#include <QPair>
typedef long double LD;

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
  QString date_start;
  QString date_end;

  InfoForAccounting() {

  }
};

struct ProjectWithDateWorkerForPayroll {
  QVector<Project> projects;
  QHash< int, QHash<QString, LD> > helpInfo;                            // id_project <position, days> | for concrete worker
  QHash< int, QHash<int, QHash<QString, LD> > > helpInfoOtherWorker;    // id_user id_project <position, days> | for other worker
  QHash<int, LD> convertCoef;

  ProjectWithDateWorkerForPayroll() {
    LD coef = 1;
    for (int i = 10; i >= 1; --i) {
      convertCoef[i] = coef;
      coef -= 0.05;
    }
  }
};