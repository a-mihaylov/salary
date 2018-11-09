#include "salary.h"

salary::salary(QWidget *parent)
	: QMainWindow(parent) {
  ui.setupUi(this);
  ui.stackedWidget->setCurrentIndex(1);
  ui.worktop->setCurrentIndex(0);
  ui.menu->setCurrentIndex(0);

  if (db.openDB()) {
    ui.project_edit_position->addItems(db.getAllPosition());
  }
  else {
    QMessageBox::critical(this, QString::fromWCharArray(L"Подключение к базе данных"), QString::fromWCharArray(L"Извините, в данный момент база данных недоступна"));
    QCoreApplication::quit();
  }

  // Настройка элементов пользовательского интерфейса, которая не может быть выполнена в QT Designer
  ui.worker_page_table_project->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
  ui.worker_page_table_project->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
  ui.project_edit_table_worker->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
  ui.project_edit_table_worker->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
  ui.accounting_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
  ui.accounting_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
  ui.accounting_table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
  ui.payroll_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
  ui.payroll_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);

  //Подключение сигналов к слотам
  connect(ui.menu_worker, SIGNAL(clicked()), this, SLOT(goToWorkerPage()));
  connect(ui.menu_prikaz, SIGNAL(clicked()), this, SLOT(goToPrikazPage()));
  connect(ui.menu_project, SIGNAL(clicked()), this, SLOT(goToProjectPage()));
  connect(ui.menu_salary, SIGNAL(clicked()), this, SLOT(goToSalaryPage()));
  connect(ui.menu_accounting, SIGNAL(clicked()), this, SLOT(goToAccountingPage()));

  connect(ui.worker_list_current, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(goToCurrentWorkerPage(QListWidgetItem *)));
  connect(ui.worker_list_dismissial, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(goToCurrentWorkerPage(QListWidgetItem *)));

  connect(ui.worker_page_return, SIGNAL(clicked()), this, SLOT(goToWorkerPage()));
  connect(ui.worker_page_save, SIGNAL(clicked()), this, SLOT(saveEditWorker()));
  connect(ui.worker_page_change_status, SIGNAL(clicked()), this, SLOT(changeWorkerStatus()));

  connect(ui.project_list, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(goToCurrentProjectPage(QListWidgetItem *)));

  connect(ui.enter_enter, SIGNAL(clicked()), this, SLOT(authorization()));
  connect(ui.enter_registration, SIGNAL(clicked()), this, SLOT(moveRegistration()));

  connect(ui.registration_back, SIGNAL(clicked()), this, SLOT(moveAuthorization()));
  connect(ui.registration_submit, SIGNAL(clicked()), this, SLOT(registration()));

}

salary::~salary() {

}

// Блок слотов перехода по окнам
void salary::goToWorkerPage(){
  ui.worktop->setCurrentIndex(0);
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
  ui.worktop->setCurrentIndex(2);
}

void salary::goToProjectPage(){
  ui.worktop->setCurrentIndex(5);
  if (db.openDB()) {
    ui.project_list->clear();
    projects = db.getAllProjects();
    for (auto it : projects) {
      QListWidgetItem * item = new QListWidgetItem(it.getProjectName());
      item->setData(Qt::UserRole, QVariant(it.getID()));
      ui.project_list->addItem(item);
    }
  }
  else {
    QMessageBox::critical(this, QString::fromWCharArray(L"Подключение к базе данных"), QString::fromWCharArray(L"Извините, в данный момент база данных недоступна"));
  }
}

void salary::goToSalaryPage(){
  ui.worktop->setCurrentIndex(3);
}

void salary::goToAccountingPage(){
  ui.worktop->setCurrentIndex(4);
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

      if (concrete_user->isDeleted()) {
        ui.worker_page_change_status->setText(QString::fromWCharArray(L"Принять на работу"));
      }
      else {
        ui.worker_page_change_status->setText(QString::fromWCharArray(L"Уволить"));
      }

      ui.worktop->setCurrentIndex(1);
    }
    else {
      QMessageBox::critical(this, QString::fromWCharArray(L"Подключение к базе данных"), QString::fromWCharArray(L"Извините, не удалось получить информацию данного пользователя"));
    }
  }
  else {
    QMessageBox::critical(this, QString::fromWCharArray(L"Подключение к базе данных"), QString::fromWCharArray(L"Извините, в данный момент база данных недоступна"));
  }
}

void salary::goToCurrentProjectPage(QListWidgetItem * item) {
  int id = item->data(Qt::UserRole).value<int>();
  if (db.openDB()) {
    QVector<User> workerInProject = db.getConcreteProject(id);
    Project concrete_project;
    for (auto it : projects) {
      if (it.getID() == id) {
        concrete_project = it;
        break;
      }
    }

    ui.project_edit_name->setText(concrete_project.getProjectName());
    ui.project_edit_budget->setValue(concrete_project.getBudget());
    ui.project_edit_mounth->setValue(concrete_project.getCountDotation());
    ui.project_edit_date_begin->setDate(QDate::fromString(concrete_project.getDateStart(), QString("yyyy-MM-dd")));
    ui.project_edit_date_end->setDate(QDate::fromString(concrete_project.getDateEnd(), QString("yyyy-MM-dd")));

    while (ui.project_edit_table_worker->rowCount()) {
      ui.project_edit_table_worker->removeRow(0);
    }

    ui.project_edit_table_worker->setRowCount(workerInProject.size());
    int idx = 0;
    for (auto it : workerInProject) {
      ui.project_edit_table_worker->setItem(idx, 0, new QTableWidgetItem(it.getFio()));
      ui.project_edit_table_worker->setItem(idx, 1, new QTableWidgetItem(it.getPosition()));
      ui.project_edit_table_worker->setItem(idx, 2, new QTableWidgetItem(QString::number(it.getMultiply())));
      ++idx;
    }

    ui.worktop->setCurrentIndex(6);
  }
  else {
    QMessageBox::critical(this, QString::fromWCharArray(L"Подключение к базе данных"), QString::fromWCharArray(L"Извините, в данный момент база данных недоступна"));
  }
}

// Блок слотов регистрации и входа пользователя
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

// Блок слотов работы с программой
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
      ui.worker_page_username->setText(update_user.getFio());
      if (update_user.getID() == this->user->getID()) {
        ui.menu_username_leader->setText(update_user.getFio());
      }
    }
    else {
      QMessageBox::critical(this, QString::fromWCharArray(L"Подключение к базе данных"), QString::fromWCharArray(L"Извините, не удалось обновить информацию данного пользователя"));
    }
  }
}

void salary::changeWorkerStatus() {
  int id = ui.worker_page_username->property("ID").value<int>();
  if (db.openDB()) {
    User update_user;
    for (auto & it : users) {
      if (it.getID() == id) {
        update_user = it;
        break;
      }
    }
    if (update_user.isDeleted()) {
      update_user.setDeleted(false);
    }
    else {
      update_user.setDeleted(true);
    }

    if (db.updateUser(update_user)) {
      QMessageBox::information(this, QString::fromWCharArray(L"Обновление информации"), QString::fromWCharArray(L"Информация успешно сохранена"));
      if (update_user.isDeleted()) {
        ui.worker_page_change_status->setText(QString::fromWCharArray(L"Принять на работу"));
      }
      else {
        ui.worker_page_change_status->setText(QString::fromWCharArray(L"Уволить"));
      }
    }
    else {
      QMessageBox::critical(this, QString::fromWCharArray(L"Подключение к базе данных"), QString::fromWCharArray(L"Извините, не удалось обновить информацию данного пользователя"));
    }
  }
}