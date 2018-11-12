#include "salary.h"

salary::salary(QWidget *parent)
	: QMainWindow(parent) {
  ui.setupUi(this);
  ui.stackedWidget->setCurrentIndex(1);
  ui.worktop->setCurrentIndex(0);
  ui.menu->setCurrentIndex(0);

  ui.worker_uncofirmed_table->setItemDelegateForColumn(0, new NonEditTableColumnDelegate());
  ui.worker_uncofirmed_table->setItemDelegateForColumn(1, new BooleanItemDelegate());
  ui.worker_uncofirmed_table->setItemDelegateForColumn(2, new BooleanItemDelegate());
  for (int i = 0; i <= 4; ++i) {
    ui.accounting_table->setItemDelegateForColumn(i, new NonEditTableColumnDelegate());
  }
  ui.accounting_table->setItemDelegateForColumn(5, new TableDelegateWithValidator());

  for (int i = 2010; i <= QDate::currentDate().year(); ++i) {
    ui.accounting_year->addItem(QString::number(i));
    ui.payroll_year->addItem(QString::number(i));
  }

  if (db.openDB()) {
    ui.project_edit_position->addItems(db.getAllPosition());
    users = db.getAllUsers();
    projects = db.getAllProjects();
  }
  else {
    QMessageBox::critical(this, QString::fromWCharArray(L"Подключение к базе данных"), QString::fromWCharArray(L"Извините, в данный момент база данных недоступна"));
    QCoreApplication::quit();
  }

  // Настройка элементов пользовательского интерфейса, которая не может быть выполнена в QT Designer
  ui.worker_page_table_project->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
  ui.worker_page_table_project->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
  ui.worker_uncofirmed_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
  ui.worker_uncofirmed_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
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
  
  connect(ui.accounting_show, SIGNAL(clicked()), this, SLOT(accountingShow()));

  connect(ui.prikaz_search, SIGNAL(clicked()), this, SLOT(searchPrikaz()));
  connect(ui.prikaz_search_reset, SIGNAL(clicked()), this, SLOT(reserSearchPrikaz()));
  connect(ui.prikaz_search_date_search, SIGNAL(clicked()), this, SLOT(searchPrikazDate()));
  connect(ui.prikaz_to_pdf, SIGNAL(clicked()), this, SLOT(printPrikazToPdf()));

  connect(ui.payroll_calculate, SIGNAL(clicked()), this, SLOT(calculatePayroll()));
}

salary::~salary() {

}

// Блок слотов перехода по окнам
void salary::goToWorkerPage(){
  ui.worktop->setCurrentIndex(0);
  ui.worker_list->setCurrentIndex(0);
  if (db.openDB()) {
    disconnect(ui.worker_uncofirmed_table, SIGNAL(cellChanged(int, int)), this, SLOT(changeStatusUncorfimedWorker(int, int)));
    ui.worker_list_current->clear();
    ui.worker_list_dismissial->clear();
    while (ui.worker_uncofirmed_table->rowCount()) {
      ui.worker_uncofirmed_table->removeRow(0);
    }
    
    users = db.getAllUsers();
    for (auto it : users) {
      if (it.isDeleted() && it.isConfirmed()) {
        QListWidgetItem * item = new QListWidgetItem(it.getFio());
        item->setData(Qt::UserRole, QVariant(it.getID()));  
        ui.worker_list_dismissial->addItem(item);
      }
      else if (it.isConfirmed()) {
        QListWidgetItem * item = new QListWidgetItem(it.getFio());
        item->setData(Qt::UserRole, QVariant(it.getID())); 
        ui.worker_list_current->addItem(item);
      }
      else {
        int row_count = ui.worker_uncofirmed_table->rowCount();
        ui.worker_uncofirmed_table->setRowCount(row_count + 1);
        QTableWidgetItem * item = new QTableWidgetItem(it.getFio());
        item->setData(Qt::UserRole, QVariant(it.getID()));
        ui.worker_uncofirmed_table->setItem(row_count, 0, item);
      }
      fioToUser[it.getFio()] = it;
    }
    connect(ui.worker_uncofirmed_table, SIGNAL(cellChanged(int, int)), this, SLOT(changeStatusUncorfimedWorker(int, int)));
  }
  else {
    QMessageBox::critical(this, QString::fromWCharArray(L"Подключение к базе данных"), QString::fromWCharArray(L"Извините, в данный момент база данных недоступна"));
  }
}

 //TODO реализовать автоматическое создание приказов и добавление их в бд
void salary::goToPrikazPage(){
  ui.worktop->setCurrentIndex(2);
  for (int i = 0; i < ui.prikaz_list->count(); i++) {
    ui.prikaz_list->item(i)->setHidden(false);
  }
  ui.prikaz_search_FIO->clear();
  ui.prikaz_search_date_search->setChecked(false);
  searchPrikazDate();
  ui.prikaz_search_dismissial->setChecked(false);
  ui.prikaz_search_recruitment->setChecked(false);

  if (db.openDB()) {
    prikazes = db.getAllPrikazes();
    for (auto it : prikazes) {
      ui.prikaz_list->addItem(QString::number(ui.prikaz_list->count()) + it.getPrikazString(it));
    }
  }
  else {
    QMessageBox::critical(this, QString::fromWCharArray(L"Подключение к базе данных"), QString::fromWCharArray(L"Извините, в данный момент база данных недоступна"));
  }
}

void salary::goToProjectPage(){
  ui.worktop->setCurrentIndex(5);
  ui.project_edit_list_worker->clear();
  ui.payroll_FIO->clear();
  for (auto it : users) {
    if (!it.isDeleted() && it.isConfirmed()) {
      ui.project_edit_list_worker->addItem(it.getFio());
      ui.payroll_FIO->addItem(it.getFio());
    }
  }

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
  ui.project_edit_list_worker->clear();
  ui.payroll_FIO->clear();
  for (auto it : users) {
    if (!it.isDeleted() && it.isConfirmed()) {
      ui.project_edit_list_worker->addItem(it.getFio());
      ui.payroll_FIO->addItem(it.getFio());
    }
  }
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
      moveAuthorization();
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
  ui.registration_password->clear();
  ui.registration_password_repeat->clear();
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
  QString date_end;
  for (auto it : projects) {
    if (it.getID() == id) {
      date_end = it.getDateEnd();
    }
  }
  if (db.addWorkerInProject(fioToUser[ui.project_edit_list_worker->currentText()].getID(), id, ui.project_edit_position->currentText(), ui.project_edit_coef->value(), date_end)) {
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
  while (ui.worker_uncofirmed_table->rowCount()) {
    ui.worker_uncofirmed_table->removeRow(0);
  }
  disconnect(ui.worker_uncofirmed_table, SIGNAL(cellChanged(int, int)), this, SLOT(changeStatusUncorfimedWorker(int, int)));
  for (auto it : users) {
    if (it.getFio().toLower().contains(workerPattern.toLower())) {
      if (it.isDeleted() && it.isConfirmed()) {
        QListWidgetItem * item = new QListWidgetItem(it.getFio());
        item->setData(Qt::UserRole, QVariant(it.getID()));
        ui.worker_list_dismissial->addItem(item);
      }
      else if (it.isConfirmed()) {
        QListWidgetItem * item = new QListWidgetItem(it.getFio());
        item->setData(Qt::UserRole, QVariant(it.getID()));
        ui.worker_list_current->addItem(item);
      }
      else {
        int row_count = ui.worker_uncofirmed_table->rowCount();
        ui.worker_uncofirmed_table->setRowCount(row_count + 1);
        QTableWidgetItem * item = new QTableWidgetItem(it.getFio());
        item->setData(Qt::UserRole, QVariant(it.getID()));
        ui.worker_uncofirmed_table->setItem(row_count, 0, item);
      }
    }
  }
  connect(ui.worker_uncofirmed_table, SIGNAL(cellChanged(int, int)), this, SLOT(changeStatusUncorfimedWorker(int, int)));
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

void salary::accountingShow() {
  while (ui.accounting_table->rowCount()) {
    ui.accounting_table->removeRow(0);
  }

  if (db.openDB()) {
    disconnect(ui.accounting_table, SIGNAL(cellChanged(int, int)), this, SLOT(saveMarkForUser(int, int)));
    QVector<InfoForAccounting> tmp = db.getForAccounting(ui.accounting_mounth->currentIndex() + 1, ui.accounting_year->currentText().toInt());
    ui.accounting_table->setRowCount(tmp.size());
    int idx = 0;
    for (auto it : tmp) {
      QTableWidgetItem * for_project = new QTableWidgetItem(it.project_name);
      for_project->setData(Qt::UserRole, QVariant(it.id_project));
      ui.accounting_table->setItem(idx, 0, for_project);
      QTableWidgetItem * for_worker = new QTableWidgetItem(it.fio);
      for_worker->setData(Qt::UserRole, QVariant(it.id_user));
      ui.accounting_table->setItem(idx, 1, for_worker);
      ui.accounting_table->setItem(idx, 2, new QTableWidgetItem(it.position));
      ui.accounting_table->setItem(idx, 3, new QTableWidgetItem(it.date_start));
      ui.accounting_table->setItem(idx, 4, new QTableWidgetItem(it.date_end));
      ui.accounting_table->setItem(idx, 5, new QTableWidgetItem(QString::number(it.mark)));
      ++idx;
    }
    connect(ui.accounting_table, SIGNAL(cellChanged(int, int)), this, SLOT(saveMarkForUser(int, int)));
  }
}

void salary::saveMarkForUser(int row, int column) {
  if (db.openDB()) {
    InfoForAccounting tmp;
    tmp.id_project = ui.accounting_table->item(row, 0)->data(Qt::UserRole).value<int>();
    tmp.id_user = ui.accounting_table->item(row, 1)->data(Qt::UserRole).value<int>();
    tmp.position = ui.accounting_table->item(row, 2)->text();
    tmp.date_start = ui.accounting_table->item(row, 3)->text();
    tmp.date_end = ui.accounting_table->item(row, 4)->text();
    tmp.mark = ui.accounting_table->item(row, column)->text().toInt();
    if (!db.updateWorkerInAccounting(tmp)) {
      QMessageBox::critical(this, QString::fromWCharArray(L"Подключение к базе данных"), QString::fromWCharArray(L"Извините, не удалось обновить информацию данного пользователя"));
    }
  }
}

void salary::changeStatusUncorfimedWorker(int row, int column) {
  QMessageBox mbox;
  mbox.setIcon(QMessageBox::Question);
  mbox.setWindowTitle(QString::fromWCharArray(L"Подтверждение действия"));
  if (column == 1) {
    mbox.setText(QString::fromWCharArray(L"Вы действительно хотите подтвердить учетную запись?"));
  }
  else {
    mbox.setText(QString::fromWCharArray(L"Вы действительно хотите удалить учетную запись?"));
  }
  QPushButton * yes = mbox.addButton(QString::fromWCharArray(L"Да"), QMessageBox::ActionRole);
  QPushButton * no = mbox.addButton(QString::fromWCharArray(L"Нет"), QMessageBox::ActionRole);
  mbox.exec();

  if (mbox.clickedButton() == yes) {
    if (db.openDB()) {
      int id = ui.worker_uncofirmed_table->item(row, 0)->data(Qt::UserRole).toInt();
      if (column == 1) {
        if (db.confirmedWorker(id)) {
          ui.worker_uncofirmed_table->removeRow(row);
          for (auto & it : users) {
            if (it.getID() == id) {
              it.setConfirmed(true);
              QListWidgetItem * worker = new QListWidgetItem(it.getFio());
              worker->setData(Qt::UserRole, QVariant(it.getID()));
              ui.worker_list_current->addItem(worker);
              db.createPrikaz(true, it.getID(), this->user->getID(), QDate::currentDate().toString("yyyy-MM-dd"));
              break;
            }
          }
        }
        else {
          QMessageBox::critical(this, QString::fromWCharArray(L"Подключение к базе данных"), QString::fromWCharArray(L"Извините, не удалось подтвердить данного пользователя"));
        }
      }
      else {
        if (db.removeUncorfimedWorker(id)) {
          ui.worker_uncofirmed_table->removeRow(row);
          int idx = 0;
          for (auto & it : users) {
            if (it.getID() == id) {
              break;
            }
            ++idx;
          }
          users.remove(idx);
        }
        else {
          QMessageBox::critical(this, QString::fromWCharArray(L"Подключение к базе данных"), QString::fromWCharArray(L"Извините, не удалось удалить данного пользователя"));
        }
      }
    }
  }
  else {
    disconnect(ui.worker_uncofirmed_table, SIGNAL(cellChanged(int, int)), this, SLOT(changeStatusUncorfimedWorker(int, int)));

    QTableWidgetItem * for_fio = ui.worker_uncofirmed_table->takeItem(row, 0);
    ui.worker_uncofirmed_table->removeRow(row);
    ui.worker_uncofirmed_table->insertRow(row);
    ui.worker_uncofirmed_table->setItem(row, 0, for_fio);
    ui.worker_uncofirmed_table->clearSelection();

    connect(ui.worker_uncofirmed_table, SIGNAL(cellChanged(int, int)), this, SLOT(changeStatusUncorfimedWorker(int, int)));
  }
}

void salary::calculatePayroll() {
  int mounth = ui.payroll_mounth->currentIndex() + 1;
  int year = ui.payroll_year->currentText().toInt();
  QString fio = ui.payroll_FIO->currentText();
  int id_user = fioToUser[fio].getID();

  if (db.openDB()) {
    ProjectWithDateWorkerForPayroll * list_project = db.getProjectForWorkerOnDate(id_user, mounth, year);
    QHash<int, int> projectToMonth;
    QHash<int, LD> projectPayrollOneMonth;
    QHash<int, LD> summaryCoefForProject;
    int rowCount = 0;
    for (auto it : list_project->projects) {
      projectToMonth[it.getID()] = monthBetweenToDate(it.getDateStart(), it.getDateEnd());
      projectPayrollOneMonth[it.getID()] = LD(it.getBudget()) / projectToMonth[it.getID()];
    }

    for (auto id_project : list_project->helpInfo.keys()) {
      summaryCoefForProject[id_project] = 0;
      for (auto pos : list_project->helpInfo[id_project].keys()) {
        summaryCoefForProject[id_project] += list_project->helpInfo[id_project][pos];
        ++rowCount;
      }
    }

    for (auto id_other_worker : list_project->helpInfoOtherWorker.keys()) {
      for (auto id_project : list_project->helpInfoOtherWorker[id_other_worker].keys()) {
        for (auto pos : list_project->helpInfoOtherWorker[id_other_worker][id_project].keys()) {
          summaryCoefForProject[id_project] += list_project->helpInfoOtherWorker[id_other_worker][id_project][pos];
        }
      }
    }

    while (ui.payroll_table->rowCount()) {
      ui.payroll_table->removeRow(0);
    }
    
    ui.payroll_table->setRowCount(rowCount);
    int idx = 0;
    for (auto prj : list_project->projects) {
      for (auto pos : list_project->helpInfo[prj.getID()].keys()) {
        ui.payroll_table->setItem(idx, 0, new QTableWidgetItem(prj.getProjectName()));
        ui.payroll_table->setItem(idx, 1, new QTableWidgetItem(pos));
        ui.payroll_table->setItem(idx, 2, new QTableWidgetItem(QString::number(double(projectPayrollOneMonth[prj.getID()] / summaryCoefForProject[prj.getID()] * list_project->helpInfo[prj.getID()][pos]), 'f', 2)));
        ++idx;
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

void salary::searchPrikaz() {
  reserSearchPrikaz();
    for (int i = 1; i < ui.prikaz_list->count(); i++) {
      if (i%2) {
        ui.prikaz_list->item(i)->setHidden(true);
      }
    }
}

void salary::reserSearchPrikaz() {
  for (int i = 0; i < ui.prikaz_list->count(); i++)
    ui.prikaz_list->item(i)->setHidden(false);
}

void salary::searchPrikazDate(){
  if (ui.prikaz_search_date_search->isChecked()) {
    ui.prikaz_search_date_start->setEnabled(true);
    ui.prikaz_search_date_end->setEnabled(true);
  }
  else {
    ui.prikaz_search_date_start->setEnabled(false);
    ui.prikaz_search_date_end->setEnabled(false);
  }
}

void salary::printPrikazToPdf(){

}

int salary::monthBetweenToDate(const QString & start, const QString & end) {
  QDate cpy_start = QDate::fromString(start, QString("yyyy-MM-dd"));
  QDate cpy_end = QDate::fromString(end, QString("yyyy-MM-dd"));
  cpy_start.setDate(cpy_start.year(), cpy_start.month(), 1);
  int result = 1;
  while (cpy_start.year() != cpy_end.year() || cpy_start.month() != cpy_end.month()) {
    ++result;
    cpy_start = cpy_start.addMonths(1);
  }
  return result;
}