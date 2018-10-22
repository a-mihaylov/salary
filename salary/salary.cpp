#include "salary.h"

salary::salary(QWidget *parent)
	: QMainWindow(parent) {
  ui.setupUi(this);

  // ��������� ��������� ����������������� ����������, ������� �� ����� ���� ��������� � QT Designer
  ui.worker_page_table_project->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
  ui.worker_page_table_project->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);


  //����������� �������� � ������
  connect(ui.menu_worker, SIGNAL(clicked()), this, SLOT(goToWorkerPage));
  connect(ui.menu_prikaz, SIGNAL(clicked()), this, SLOT(goToPrikazPage));
  connect(ui.menu_project, SIGNAL(clicked()), this, SLOT(goToProjectPage));
  connect(ui.menu_salary, SIGNAL(clicked()), this, SLOT(goToSalaryPage));
  //connect(ui.menu_accounting, SIGNAL(clicked()), this, SLOT(goToAccountingPage));

  //����������� ���� ������� � ���� �����
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


// ���� ������������ �����-���� ���, �����������, ����� ���� � ��� �����
// TODO(mihaylov.maz@gmail.com): ������� ���� ���� � ������ ���������    
void salary::test_slot() {
  QString aa = QString::fromWCharArray(L"������");
}