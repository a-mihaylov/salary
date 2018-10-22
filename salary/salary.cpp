#include "salary.h"

salary::salary(QWidget *parent)
	: QMainWindow(parent) {
  ui.setupUi(this);

  // Настройка элементов пользовательского интерфейса, которая не может быть выполнена в QT Designer
  ui.worker_page_table_project->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
  ui.worker_page_table_project->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);


  //Подключение сигналов к слотам
  connect(ui.menu_worker, SIGNAL(clicked()), this, SLOT(goToWorkerPage));
  connect(ui.menu_prikaz, SIGNAL(clicked()), this, SLOT(goToPrikazPage));
  connect(ui.menu_project, SIGNAL(clicked()), this, SLOT(goToProjectPage));
  connect(ui.menu_salary, SIGNAL(clicked()), this, SLOT(goToSalaryPage));
  //connect(ui.menu_accounting, SIGNAL(clicked()), this, SLOT(goToAccountingPage));

  //Подключение тест сигнала к тест слоту
  connect(ui.menu_salary, SIGNAL(clicked()), this, SLOT(test_slot()));
}

salary::~salary() {

}

void salary::goToWorkerPage(){

}

void salary::goToPrikazPage(){

}

void salary::goToProjectPage(){

}

void salary::goToSalaryPage(){

}

void salary::goToAccountingPage(){

}


// Слот тестирования каких-либо фич, отображения, связи форм и так далее
// TODO(mihaylov.maz@gmail.com): удалить этот слот к релизу программы    
void salary::test_slot() {
  QString aa = QString::fromWCharArray(L"Привет");
}