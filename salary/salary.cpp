#include "salary.h"

salary::salary(QWidget *parent)
	: QMainWindow(parent) {
  ui.setupUi(this);
  ui.stackedWidget->setCurrentIndex(1);
  ui.worktop->setCurrentIndex(0);
  ui.menu->setCurrentIndex(0);

  ui.accounting_table->setItemDelegateForColumn(0, new NonEditTableColumnDelegate());
  ui.accounting_table->setItemDelegateForColumn(1, new NonEditTableColumnDelegate());
  ui.accounting_table->setItemDelegateForColumn(2, new NonEditTableColumnDelegate());

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

  connect(ui.project_edit_add_worker, SIGNAL(clicked()), this, SLOT(addProjectWorker()));
  connect(ui.project_edit_delete_worker, SIGNAL(clicked()), this, SLOT(removeProjectWorker()));

  connect(ui.project_find_name, SIGNAL(textEdited(const QString &)), this, SLOT(searchProject(const QString &)));
  connect(ui.worker_search_FIO, SIGNAL(textEdited(const QString &)), this, SLOT(searchWorker(const QString &)));

  connect(ui.worker_page_table_project, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(goToCurrentProjectPage(int, int)));
  connect(ui.project_edit_table_worker, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(goToCurrentWorkerPage(int, int)));

  connect(ui.project_create_new, SIGNAL(clicked()), this, SLOT(createNewProject()));
  connect(ui.project_edit_save_changes, SIGNAL(clicked()), this, SLOT(saveProject()));
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
      fioToUser[it.getFio()] = it;
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
  fillWorkerPage(id);
}

void salary::goToCurrentWorkerPage(int row, int column) {
  int id = ui.project_edit_table_worker->item(row, 0)->data(Qt::UserRole).value<int>();
  fillWorkerPage(id);
}

void salary::goToCurrentProjectPage(QListWidgetItem * item) {
  int id = item->data(Qt::UserRole).value<int>();
  fillProjectPage(id);
}

void salary::goToCurrentProjectPage(int row, int column) {
  int id = ui.worker_page_table_project->item(row, 0)->data(Qt::UserRole).value<int>();
  fillProjectPage(id);
}

void salary::createNewProject() {
  ui.project_edit_gb_workers->setEnabled(false);
  ui.project_edit_name->clear();
  ui.project_edit_name->setProperty("ID", QVariant(0));
  ui.project_edit_budget->setValue(1);
  ui.project_edit_mounth->setValue(1);
  ui.project_edit_date_begin->setDate(QDate::fromString(QString("2000-01-01"), QString("yyyy-MM-dd")));
  ui.project_edit_date_end->setDate(QDate::fromString(QString("2000-01-01"), QString("yyyy-MM-dd")));
  while (ui.project_edit_table_worker->rowCount()) {
    ui.project_edit_table_worker->removeRow(0);
  }

  ui.worktop->setCurrentIndex(6);
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
    goToWorkerPage();
    for (auto it : users) {
      if (!it.isDeleted()) {
        ui.project_edit_list_worker->addItem(it.getFio());
        ui.payroll_FIO->addItem(it.getFio());
      }
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

  QString error_msg;
  QRegExp login_and_passw("[A-Za-z0-9]{6,20}");
  QRegExp fio(QString::fromWCharArray(L"(([А-ЯЁ][а-я-ё]{0,})([А-ЯЁ][а-я-ё]{0,})? ([А-ЯЁ][а-я-ё]{1,})([А-ЯЁ][а-я-ё]{1,})? ?([А-ЯЁ][а-я-ё]{1,})?([А-ЯЁ][а-я-ё]{1,})?)"));
  if (!login_and_passw.exactMatch(ui.registration_login->text())) {
    error_msg.append(QString::fromWCharArray(L"Логин не подходит под требования (латинские буквы и цифры, от 6 до 20 знаков)\n"));
  }
  if (!login_and_passw.exactMatch(ui.registration_password->text())) {
    error_msg.append(QString::fromWCharArray(L"Пароль не подходит под требования (латинские буквы и цифры, от 6 до 20 знаков)\n"));
  }
  if (!fio.exactMatch(ui.registration_fio->text())) {
    error_msg.append(QString::fromWCharArray(L"ФИО не подходит под требования (русские буквы, отступ 1 пробел, каждое слово с большой буквы)\n"));
  }
  if (!error_msg.isEmpty()) {
    error_msg.resize(error_msg.size() - 1);
    QMessageBox::warning(this, QString::fromWCharArray(L"Регистрация провалена"), error_msg);
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

void salary::addProjectWorker() {
  int id = ui.project_edit_name->property("ID").value<int>();

  int rowCount = ui.project_edit_table_worker->rowCount();
  for (int i = 0; i < rowCount; ++i) {
    if (ui.project_edit_table_worker->item(i, 0)->text() == ui.project_edit_list_worker->currentText() && ui.project_edit_table_worker->item(i, 1)->text() == ui.project_edit_position->currentText()) {
      QMessageBox::warning(this, QString::fromWCharArray(L"Предупреждение"), QString::fromWCharArray(L"Вы пытаетесь добавить дубликат работника с должностью"));
      return;
    }
  }
  if (db.addWorkerInProject(fioToUser[ui.project_edit_list_worker->currentText()].getID(), id, ui.project_edit_position->currentText(), ui.project_edit_coef->value())) {
    ui.project_edit_table_worker->setRowCount(rowCount + 1);
    ui.project_edit_table_worker->setItem(rowCount, 0, new QTableWidgetItem(ui.project_edit_list_worker->currentText()));
    ui.project_edit_table_worker->setItem(rowCount, 1, new QTableWidgetItem(ui.project_edit_position->currentText()));
    ui.project_edit_table_worker->setItem(rowCount, 2, new QTableWidgetItem(QString::number(ui.project_edit_coef->value())));
  }
  else {
    QMessageBox::critical(this, QString::fromWCharArray(L"Подключение к базе данных"), QString::fromWCharArray(L"Извините, в данный момент база данных недоступна"));
  }
}

void salary::removeProjectWorker() {
  int id = ui.project_edit_name->property("ID").value<int>();

  if (ui.project_edit_table_worker->currentRow() == -1) {
    QMessageBox::warning(this, QString::fromWCharArray(L"Предупреждение"), QString::fromWCharArray(L"Вы не выбрали сотрудника для удаления"));
  }
  else {
    int curRow = ui.project_edit_table_worker->currentRow();
    if (db.removeWorkerInProject(fioToUser[ui.project_edit_table_worker->item(curRow, 0)->text()].getID(), id, ui.project_edit_table_worker->item(curRow, 1)->text())) {
      ui.project_edit_table_worker->removeRow(ui.project_edit_table_worker->currentRow());
    }
    else {
      QMessageBox::critical(this, QString::fromWCharArray(L"Подключение к базе данных"), QString::fromWCharArray(L"Извините, в данный момент база данных недоступна"));
    }
  }
}

void salary::searchProject(const QString & projectPattern) {
  ui.project_list->clear();
  for (auto it : projects) {
    if (it.getProjectName().toLower().contains(projectPattern.toLower())) {
      QListWidgetItem * item = new QListWidgetItem(it.getProjectName());
      item->setData(Qt::UserRole, QVariant(it.getID()));
      ui.project_list->addItem(item);
    }
  }
}

void salary::searchWorker(const QString & workerPattern) {
  ui.worker_list_current->clear();
  ui.worker_list_dismissial->clear();
  for (auto it : users) {
    if (it.getFio().toLower().contains(workerPattern.toLower())) {
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
}

void salary::saveProject() {
  int id = ui.project_edit_name->property("ID").value<int>();
  if (db.openDB()) {
    Project prj;
    if (id != 0) {
      for (auto & it : projects) {
        if (it.getID() == id) {
          prj = it;
          break;
        }
      }
    }
    prj.setProjectName(ui.project_edit_name->text());
    prj.setBudget(ui.project_edit_budget->value());
    prj.setCountDotation(ui.project_edit_mounth->value());
    prj.setDateStart(ui.project_edit_date_begin->text());
    prj.setDateEnd(ui.project_edit_date_end->text());

    if (id == 0) {
      if (db.createProject(prj)) {
        QMessageBox::information(this, QString::fromWCharArray(L"Создание проекта"), QString::fromWCharArray(L"Проект успешно сохранен"));
        ui.project_edit_gb_workers->setEnabled(true);
      }
      else {
        QMessageBox::critical(this, QString::fromWCharArray(L"Подключение к базе данных"), QString::fromWCharArray(L"Извините, не удалось обновить информацию данного пользователя"));
      }
    }
    else {
      if (db.updateProject(prj)) {
        QMessageBox::information(this, QString::fromWCharArray(L"Обновление информации"), QString::fromWCharArray(L"Информация успешно сохранена"));
      }
      else {
        QMessageBox::critical(this, QString::fromWCharArray(L"Подключение к базе данных"), QString::fromWCharArray(L"Извините, не удалось обновить информацию данного пользователя"));
      }
    }
  }
}

// Блок вспомогательных функций
void salary::fillProjectPage(int id) {
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
    ui.project_edit_name->setProperty("ID", id);
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
      QTableWidgetItem * worker_fio = new QTableWidgetItem(it.getFio());
      worker_fio->setData(Qt::UserRole, QVariant(it.getID()));
      ui.project_edit_table_worker->setItem(idx, 0, worker_fio);
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

void salary::fillWorkerPage(int id) {
  if (db.openDB()) {
    AllInfoForWorker * concrete_user = db.getConcreteUser(id);
    if (concrete_user != nullptr) {
      ui.worker_page_username->setText(concrete_user->user.getFio());
      ui.worker_page_username->setProperty("ID", id);
      ui.worker_page_FIO->setText(concrete_user->user.getFio());
      ui.worker_page_recruitment_date->setDate(QDate::fromString(concrete_user->user.getDateReceipt(), QString("yyyy-MM-dd")));
      ui.worker_page_dismissial_date->setDate(QDate::fromString(concrete_user->user.getDateDismissial(), QString("yyyy-MM-dd")));
      ui.worker_page_b_day->setDate(QDate::fromString(concrete_user->user.getDateBirth(), QString("yyyy-MM-dd")));

      if (concrete_user->user.isDeleted()) {
        ui.worker_page_change_status->setText(QString::fromWCharArray(L"Принять на работу"));
      }
      else {
        ui.worker_page_change_status->setText(QString::fromWCharArray(L"Уволить"));
      }

      while (ui.worker_page_table_project->rowCount()) {
        ui.worker_page_table_project->removeRow(0);
      }
      ui.worker_page_table_project->setRowCount(concrete_user->projects.size());
      int idx = 0;
      for (auto it : concrete_user->projects) {
        QTableWidgetItem * project_name = new QTableWidgetItem(it.getProjectName());
        project_name->setData(Qt::UserRole, QVariant(it.getID()));
        ui.worker_page_table_project->setItem(idx, 0, project_name);
        ui.worker_page_table_project->setItem(idx, 1, new QTableWidgetItem(concrete_user->helpInfo[it.getID()].position));
        ui.worker_page_table_project->setItem(idx, 2, new QTableWidgetItem(QString::number(concrete_user->helpInfo[it.getID()].mark)));
        ++idx;
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