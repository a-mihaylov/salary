#include "salary.h"

salary::salary(QWidget *parent)
	: QMainWindow(parent) {
  ui.setupUi(this);
  ui.stackedWidget->setCurrentIndex(1);
  ui.menu->setCurrentIndex(0);

  // ��������� ��������� ����������������� ����������, ������� �� ����� ���� ��������� � QT Designer
  ui.worker_page_table_project->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
  ui.worker_page_table_project->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);


  //����������� �������� � ������
  connect(ui.menu_worker, SIGNAL(clicked()), this, SLOT(goToWorkerPage));
  connect(ui.menu_prikaz, SIGNAL(clicked()), this, SLOT(goToPrikazPage));
  connect(ui.menu_project, SIGNAL(clicked()), this, SLOT(goToProjectPage));
  connect(ui.menu_salary, SIGNAL(clicked()), this, SLOT(goToSalaryPage));
  connect(ui.menu_accounting, SIGNAL(clicked()), this, SLOT(goToAccountingPage));

  //����������� ���� ������� � ���� �����
  connect(ui.menu_salary, SIGNAL(clicked()), this, SLOT(test_slot()));

  connect(ui.enter_enter, SIGNAL(clicked()), this, SLOT(authorization()));
  connect(ui.enter_registration, SIGNAL(clicked()), this, SLOT(moveRegistration()));
  connect(ui.registration_back, SIGNAL(clicked()), this, SLOT(moveAuthorization()));
  connect(ui.registration_submit, SIGNAL(clicked()), this, SLOT(registration()));
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

void salary::authorization() {
  if (db.openDB()) {
    user = db.Authorization(ui.enter_login->text(), QCryptographicHash::hash(ui.enter_password->text().toUtf8(), QCryptographicHash::Sha3_512).toHex());
    if (user != nullptr) {
      QMessageBox::information(this, QString::fromWCharArray(L"����������� �������"), QString::fromWCharArray(L"�� ��������������"));
      ui.menu_username_leader->setText(user->fio);
      ui.menu->setCurrentIndex(1);
    }
    else {
      QMessageBox::warning(this, QString::fromWCharArray(L"����������� ���������"), QString::fromWCharArray(L"�� �� ��������������"));
    }
  }
  else {
    QMessageBox::critical(this, QString::fromWCharArray(L"����������� � ���� ������"), QString::fromWCharArray(L"��������, � ������ ������ ���� ������ ����������"));
  }
}

void salary::registration() {

  if (ui.registration_password != ui.registration_password_repeat) {
    QMessageBox::warning(this, QString::fromWCharArray(L"����������� ���������"), QString::fromWCharArray(L"������ �� ���������, ��������� ���� �������"));
    ui.registration_password->clear();
    ui.registration_password_repeat->clear();
    return;
  }

  if (db.openDB()) {
    bool is_ok = db.Registration(ui.registration_login->text(), QCryptographicHash::hash(ui.registration_password->text().toUtf8(), QCryptographicHash::Sha3_512).toHex(), ui.registration_fio->text());
    if (is_ok) {
      QMessageBox::information(this, QString::fromWCharArray(L"����������� �������"), QString::fromWCharArray(L"�� ������������������"));
      ui.stackedWidget->setCurrentIndex(1);
    }
    else {
      QMessageBox::warning(this, QString::fromWCharArray(L"����������� ���������"), QString::fromWCharArray(L"������������ � ������ ������� ��� ����������"));
    }
  }
  else {
    QMessageBox::critical(this, QString::fromWCharArray(L"����������� � ���� ������"), QString::fromWCharArray(L"��������, � ������ ������ ���� ������ ����������"));
  }
}


void salary::moveRegistration() {
  ui.stackedWidget->setCurrentIndex(0);
  ui.enter_login->clear();
  ui.enter_password->clear();
}

void salary::moveAuthorization() {
  ui.stackedWidget->setCurrentIndex(1);

  ui.registration_fio->clear();
  ui.registration_login->clear();
  ui.registration_password->clear();
  ui.registration_password_repeat->clear();
}