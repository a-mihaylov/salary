#include "salary.h"

salary::salary(QWidget *parent)
	: QMainWindow(parent) {
  ui.setupUi(this);
  ui.stackedWidget->setCurrentIndex(1);
  ui.menu->setCurrentIndex(0);

  // Настройка элементов пользовательского интерфейса, которая не может быть выполнена в QT Designer
  ui.worker_page_table_project->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
  ui.worker_page_table_project->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);


  //Подключение сигналов к слотам
  connect(ui.menu_worker, SIGNAL(clicked()), this, SLOT(goToWorkerPage()));
  connect(ui.menu_prikaz, SIGNAL(clicked()), this, SLOT(goToPrikazPage()));
  connect(ui.menu_project, SIGNAL(clicked()), this, SLOT(goToProjectPage()));
  connect(ui.menu_salary, SIGNAL(clicked()), this, SLOT(goToSalaryPage()));
  connect(ui.menu_accounting, SIGNAL(clicked()), this, SLOT(goToAccountingPage()));
  connect(ui.worker_list_current, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(goToCurrentWorkerPage(QListWidgetItem *)));
  connect(ui.worker_list_dismissial, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(goToCurrentWorkerPage(QListWidgetItem *)));
  connect(ui.worker_page_save, SIGNAL(clicked()), this, SLOT(saveEditWorker()));


  //Подключение тест сигнала к тест слоту
  connect(ui.menu_salary, SIGNAL(clicked()), this, SLOT(test_slot()));

  connect(ui.enter_enter, SIGNAL(clicked()), this, SLOT(authorization()));
  connect(ui.enter_registration, SIGNAL(clicked()), this, SLOT(moveRegistration()));
  connect(ui.registration_back, SIGNAL(clicked()), this, SLOT(moveAuthorization()));
  connect(ui.registration_submit, SIGNAL(clicked()), this, SLOT(registration()));
}

salary::~salary() {

}

void salary::goToWorkerPage(){
  ui.stackedWidget->setCurrentIndex(2);
  if (db.openDB()) {
    ui.worker_list_current->clear();
    ui.worker_list_dismissial->clear();
    users = db.getAllUsers();
    for (auto it : users) {
      QListWidgetItem * item = new QListWidgetItem(it.getFio());
      item->setData(Qt::UserRole, QVariant(it.getID()));
      if (it.isDeleted()) {
        ui.worker_list_dismissial->addItem(item);
      }
      else {
        ui.worker_list_current->addItem(item);
      }
    }
  }
  else {
    QMessageBox::critical(this, QString::fromWCharArray(L"Подключение к базе данных"), QString::fromWCharArray(L"Извините, в данный момент база данных недоступна"));
  }
}

void salary::goToPrikazPage(){
  ui.worktop->setCurrentIndex(4);
}

void salary::goToProjectPage(){

}

void salary::goToSalaryPage(){

}

void salary::goToAccountingPage(){

}

void salary::authorization() {
  if (db.openDB()) {
    user = db.Authorization(ui.enter_login->text(), QCryptographicHash::hash(ui.enter_password->text().toUtf8(), QCryptographicHash::Sha3_512).toHex());
    if (user != nullptr) {
      QMessageBox::information(this, QString::fromWCharArray(L"Авторизация успешна"), QString::fromWCharArray(L"Вы авторизовались"));
      ui.menu_username_leader->setText(user->getFio());
      ui.menu->setCurrentIndex(0);
      ui.stackedWidget->setCurrentIndex(0);
    }
    else {
      QMessageBox::warning(this, QString::fromWCharArray(L"Авторизация провалена"), QString::fromWCharArray(L"Вы не авторизовались"));
    }
  }
  else {
    QMessageBox::critical(this, QString::fromWCharArray(L"Подключение к базе данных"), QString::fromWCharArray(L"Извините, в данный момент база данных недоступна"));
  }
}

void salary::registration() {

  if (ui.registration_password->text() != ui.registration_password_repeat->text()) {
    QMessageBox::warning(this, QString::fromWCharArray(L"Регистрация провалена"), QString::fromWCharArray(L"Пароли не совпадают, повторите ввод паролей"));
    ui.registration_password->clear();
    ui.registration_password_repeat->clear();
    return;
  }

  if (db.openDB()) {
    bool is_ok = db.Registration(ui.registration_login->text(), QCryptographicHash::hash(ui.registration_password->text().toUtf8(), QCryptographicHash::Sha3_512).toHex(), ui.registration_fio->text());
    if (is_ok) {
      QMessageBox::information(this, QString::fromWCharArray(L"Регистрация успешна"), QString::fromWCharArray(L"Вы зарегистрировались"));
      ui.stackedWidget->setCurrentIndex(1);
    }
    else {
      QMessageBox::warning(this, QString::fromWCharArray(L"Регистрация провалена"), QString::fromWCharArray(L"Пользователь с данным логином уже существует"));
    }
  }
  else {
    QMessageBox::critical(this, QString::fromWCharArray(L"Подключение к базе данных"), QString::fromWCharArray(L"Извините, в данный момент база данных недоступна"));
  }
}

void salary::moveRegistration() {
  ui.stackedWidget->setCurrentIndex(2);
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

void salary::goToCurrentWorkerPage(QListWidgetItem * item) {
  int id = item->data(Qt::UserRole).value<int>();
  if (db.openDB()) {
    User * concrete_user = db.getConcreteUser(id);
    if (concrete_user != nullptr) {
      ui.worker_page_username->setText(concrete_user->getFio());
      ui.worker_page_username->setProperty("ID", id);
      ui.worker_page_FIO->setText(concrete_user->getFio());
      ui.worker_page_recruitment_date->setDate(QDate::fromString(concrete_user->getDateReceipt(), QString("yyyy-MM-dd")));
      ui.worker_page_dismissial_date->setDate(QDate::fromString(concrete_user->getDateDismissial(), QString("yyyy-MM-dd")));
      ui.worker_page_b_day->setDate(QDate::fromString(concrete_user->getDateBirth(), QString("yyyy-MM-dd")));
      ui.stackedWidget->setCurrentIndex(3);
    }
    else {
      QMessageBox::critical(this, QString::fromWCharArray(L"Подключение к базе данных"), QString::fromWCharArray(L"Извините, не удалось получить информацию данного пользователя"));
    }
  }
  else {
    QMessageBox::critical(this, QString::fromWCharArray(L"Подключение к базе данных"), QString::fromWCharArray(L"Извините, в данный момент база данных недоступна"));
  }
}

void salary::saveEditWorker() {
  int id = ui.worker_page_username->property("ID").value<int>();
  if (db.openDB()) {
    User update_user;
    for (auto & it : users) {
      if (it.getID() == id) {
        update_user = it;
        break;
      }
    }
    update_user.setDateBirth(ui.worker_page_b_day->text());
    update_user.setDateDismissial(ui.worker_page_dismissial_date->text());
    update_user.setDateReceipt(ui.worker_page_recruitment_date->text());
    update_user.setFio(ui.worker_page_FIO->text());

    if (db.updateUser(update_user)) {
      QMessageBox::information(this, QString::fromWCharArray(L"Обновление информации"), QString::fromWCharArray(L"Информация успешно сохранена"));
    }
    else {
      QMessageBox::critical(this, QString::fromWCharArray(L"Подключение к базе данных"), QString::fromWCharArray(L"Извините, не удалось обновить информацию данного пользователя"));
    }
  }
}