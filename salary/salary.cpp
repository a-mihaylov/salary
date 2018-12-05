#include "salary.h"

salary::salary(QWidget *parent)
	: QMainWindow(parent) {
  // Предварительная настройка пользовательского интерфейса
  ui.setupUi(this);
  ui.stackedWidget->setCurrentIndex(1);
  ui.worktop->setCurrentIndex(0);
  history_window.push_back(0);
  ui.mainToolBar->setHidden(true);
  ui.project_edit_mounth->setReadOnly(true);
  user = nullptr;

  // Подключение темной темы 
  QFile f(":qdarkstyle/style.qss");
  f.open(QFile::ReadOnly | QFile::Text);
  QTextStream ts(&f);
  this->style_dark_theme = ts.readAll();
  this->theme_is_light = true;
  ui.graphics_tab_widget->setCurrentIndex(0);
  ui.graphics_tab_widget->setHidden(true);
  this->theme_color_graphics = QChart::ChartThemeLight;

  // Настройка делегатов табличных виджетов
  ui.worker_page_dismissial_date->setReadOnly(true);
  ui.worker_page_recruitment_date->setReadOnly(true);

  ui.worker_uncofirmed_table->setItemDelegateForColumn(0, new NonEditTableColumnDelegate());
  ui.worker_uncofirmed_table->setItemDelegateForColumn(1, new BooleanItemDelegate());
  ui.worker_uncofirmed_table->setItemDelegateForColumn(2, new BooleanItemDelegate());

  for (int i = 0; i <= 4; ++i) {
    ui.accounting_table->setItemDelegateForColumn(i, new NonEditTableColumnDelegate());
  }
  ui.accounting_table->setItemDelegateForColumn(5, new TableDelegateWithValidator());

  // Настройка вывода годов во всех связанных спин-боксах
  for (int i = 2010; i <= QDate::currentDate().year(); ++i) {
    ui.accounting_year->addItem(QString::number(i));
    ui.payroll_year->addItem(QString::number(i));
    ui.graphics_year->addItem(QString::number(i));
  }

  // Проверка возможности доступа к удаленной базе данных
  if (db.openDB()) {
    ui.project_edit_position->addItems(db.getAllPosition());
    updateUsersInfo();
    projects = db.getAllProjects();
    qSort(projects.begin(), projects.end());
  }
  else {
    QMessageBox::critical(this, QString::fromWCharArray(L"Подключение к базе данных"), QString::fromWCharArray(L"Извините, в данный момент база данных недоступна"));
    QCoreApplication::quit();
  }

  // Настройка элементов пользовательского интерфейса, которая не может быть выполнена в QT Designer
  ui.prikaz_search_date_start->setDate(QDate::currentDate().addMonths(-1));
  ui.prikaz_search_date_end->setDate(QDate::currentDate());
  ui.worker_uncofirmed_table->setColumnWidth(1, 55);
  ui.worker_uncofirmed_table->setColumnWidth(2, 55);
  ui.worker_uncofirmed_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
  ui.worker_uncofirmed_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
  ui.worker_uncofirmed_table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Fixed);
  ui.worker_page_table_project->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
  ui.worker_page_table_project->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
  ui.project_edit_table_worker->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
  ui.project_edit_table_worker->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
  ui.accounting_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
  ui.accounting_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
  ui.accounting_table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
  ui.payroll_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
  ui.payroll_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);

  // Подключение сигналов к слотам
  connect(ui.menu_worker, SIGNAL(clicked()), this, SLOT(goToWorkerPage()));
  connect(ui.menu_prikaz, SIGNAL(clicked()), this, SLOT(goToPrikazPage()));
  connect(ui.menu_project, SIGNAL(clicked()), this, SLOT(goToProjectPage()));
  connect(ui.menu_salary, SIGNAL(clicked()), this, SLOT(goToSalaryPage()));
  connect(ui.menu_accounting, SIGNAL(clicked()), this, SLOT(goToAccountingPage()));
  connect(ui.menu_graphics, SIGNAL(clicked()), this, SLOT(goToGraphicPage()));
  connect(ui.menu_color_theme_light, SIGNAL(triggered()), this, SLOT(setColorLight()));
  connect(ui.menu_color_theme_dark, SIGNAL(triggered()), this, SLOT(setColorDark()));

  connect(ui.worker_list_current, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(goToCurrentWorkerPage(QListWidgetItem *)));
  connect(ui.worker_list_dismissial, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(goToCurrentWorkerPage(QListWidgetItem *)));
  connect(ui.worker_search_FIO, SIGNAL(textEdited(const QString &)), this, SLOT(searchWorker(const QString &)));

  connect(ui.worker_page_save, SIGNAL(clicked()), this, SLOT(saveEditWorker()));
  connect(ui.worker_page_change_status, SIGNAL(clicked()), this, SLOT(changeWorkerStatus()));
  connect(ui.worker_page_table_project, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(goToCurrentProjectPage(int, int)));

  connect(ui.project_find_name, SIGNAL(textEdited(const QString &)), this, SLOT(searchProject(const QString &)));
  connect(ui.project_create_new, SIGNAL(clicked()), this, SLOT(createNewProject()));
  connect(ui.project_list, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(goToCurrentProjectPage(QListWidgetItem *)));
  connect(ui.project_edit_add_worker, SIGNAL(clicked()), this, SLOT(addProjectWorker()));
  connect(ui.project_edit_delete_worker, SIGNAL(clicked()), this, SLOT(removeProjectWorker()));
  connect(ui.project_edit_date_begin, SIGNAL(dateChanged(const QDate &)), this, SLOT(rewriteCountDotation()));
  connect(ui.project_edit_date_end, SIGNAL(dateChanged(const QDate &)), this, SLOT(rewriteCountDotation()));
  connect(ui.project_edit_table_worker, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(goToCurrentWorkerPage(int, int)));
  connect(ui.project_edit_save_changes, SIGNAL(clicked()), this, SLOT(saveProject()));

  connect(ui.prikaz_search_FIO, SIGNAL(textEdited(const QString &)), this, SLOT(searchPrikaz(const QString &)));
  connect(ui.prikaz_search_recruitment, SIGNAL(clicked()), this, SLOT(searchPrikaz()));
  connect(ui.prikaz_search_dismissial, SIGNAL(clicked()), this, SLOT(searchPrikaz()));
  connect(ui.prikaz_search_date_search, SIGNAL(clicked()), this, SLOT(searchPrikaz()));
  connect(ui.prikaz_search_date_start, SIGNAL(dateChanged(const QDate &)), this, SLOT(searchPrikaz()));
  connect(ui.prikaz_search_date_end, SIGNAL(dateChanged(const QDate &)), this, SLOT(searchPrikaz()));
  connect(ui.prikaz_search_date_search, SIGNAL(clicked()), this, SLOT(searchPrikazDate()));
  connect(ui.prikaz_to_pdf, SIGNAL(clicked()), this, SLOT(printPrikazToPdf()));

  connect(ui.enter_enter, SIGNAL(clicked()), this, SLOT(authorization()));
  connect(ui.enter_registration, SIGNAL(clicked()), this, SLOT(moveRegistration()));

  connect(ui.registration_back, SIGNAL(clicked()), this, SLOT(moveAuthorization()));
  connect(ui.registration_submit, SIGNAL(clicked()), this, SLOT(registration())); 

  connect(ui.worktop, SIGNAL(currentChanged(int)), this, SLOT(worktopChanged(int)));

  connect(ui.graphics_calculate, SIGNAL(clicked()), this, SLOT(calculateGraphics()));

  connect(ui.accounting_show, SIGNAL(clicked()), this, SLOT(accountingShow()));
  connect(ui.accounting_info, SIGNAL(clicked()), this, SLOT(accountingInfoShow()));
  connect(ui.accounting_filter_FIO, SIGNAL(clicked()), this, SLOT(searchAccounting()));
  connect(ui.accounting_filter_mark, SIGNAL(clicked()), this, SLOT(searchAccounting()));
  connect(ui.accounting_filter_project, SIGNAL(clicked()), this, SLOT(searchAccounting()));
  connect(ui.accounting_search_FIO, SIGNAL(currentIndexChanged(int)), this, SLOT(searchAccounting()));
  connect(ui.accounting_search_mark, SIGNAL(currentIndexChanged(int)), this, SLOT(searchAccounting()));
  connect(ui.accounting_search_project, SIGNAL(currentIndexChanged(int)), this, SLOT(searchAccounting()));

  connect(ui.payroll_calculate, SIGNAL(clicked()), this, SLOT(calculatePayroll()));
}

salary::~salary() {

}

void salary::closeEvent(QCloseEvent * e) {
  if (user != nullptr) {
    QMessageBox mbox;
    mbox.setIcon(QMessageBox::Question);
    mbox.setWindowTitle(QString::fromWCharArray(L"Подтверждение действия"));
    mbox.setText(QString::fromWCharArray(L"Вы хотите закрыть программу или зайти под другим именем?"));
    QPushButton * close_program = mbox.addButton(QString::fromWCharArray(L"Закрыть программу"), QMessageBox::ActionRole);
    QPushButton * drop_auth = mbox.addButton(QString::fromWCharArray(L"Зайти под другим именем"), QMessageBox::ActionRole);
    QPushButton * back = mbox.addButton(QString::fromWCharArray(L"Отмена"), QMessageBox::ActionRole);
    mbox.exec();

    if (mbox.clickedButton() == close_program) {
      e->accept();
    }
    else if (mbox.clickedButton() == drop_auth) {
      e->ignore();
      users.clear();
      fioToUser.clear();
      idToUser.clear();
      projects.clear();
      prikazes.clear();
      user = nullptr;
      ui.accounting_table->setRowCount(0);
      ui.payroll_table->setRowCount(0);
      ui.payroll_mounth->setCurrentIndex(0);
      ui.payroll_year->setCurrentIndex(0);
      ui.payroll_mounth->setCurrentIndex(0);
      ui.payroll_year->setCurrentIndex(0);
      ui.accounting_mounth->setCurrentIndex(0);
      ui.accounting_year->setCurrentIndex(0);
      ui.graphics_mounth->setCurrentIndex(0);
      ui.graphics_year->setCurrentIndex(0);
      moveAuthorization();
    }
    else {
      e->ignore();
    }
  }
}

void salary::keyPressEvent(QKeyEvent * e) {
  switch (e->key()) {
    case Qt::Key_Return:
      if (ui.stackedWidget->currentIndex() == 0) {
        if (history_window.isEmpty()) {
          history_window.push_back(0);
        }
        switch (history_window.back()) {
          case 1:
            ui.worker_page_save->click();
            break;
          case 2:
            ui.prikaz_to_pdf->click();
            break;
          case 3:
            ui.payroll_calculate->click();
            break;
          case 4:
            ui.accounting_show->click();
            break;
          case 7:
            ui.graphics_calculate->click();
            break;
          default:
            break;
        }
      }
      else {
        switch (ui.stackedWidget->currentIndex()) {
          case 1:
            ui.enter_enter->click();
            break;
          case 2:
            ui.registration_submit->click();
            break;
          default:
            break;
        }
      }
      break;

    case Qt::Key_Backspace:
      if (history_window.size() > 1) {
        history_window.pop_back();
        if (!history_window.isEmpty()) {
          disconnect(ui.worktop, SIGNAL(currentChanged(int)), this, SLOT(worktopChanged(int)));
          ui.worktop->setCurrentIndex(history_window.back());
          connect(ui.worktop, SIGNAL(currentChanged(int)), this, SLOT(worktopChanged(int)));
        }
      }
      break;

    case Qt::Key_F5:
      if (this->theme_is_light) {
        setColorDark();
      }
      else {
        setColorLight();
      }
      break;

    default:
      break;
  }
}

// Блок слотов перехода по окнам
void salary::goToWorkerPage(){
  ui.worktop->setCurrentIndex(0);
  ui.worker_list->setCurrentIndex(0);
  if (db.openDB()) {
    fioToUser.clear();
    idToUser.clear();
    disconnect(ui.worker_uncofirmed_table, SIGNAL(cellChanged(int, int)), this, SLOT(changeStatusUncorfimedWorker(int, int)));
    ui.worker_list_current->clear();
    ui.worker_list_dismissial->clear();
    while (ui.worker_uncofirmed_table->rowCount()) {
      ui.worker_uncofirmed_table->removeRow(0);
    }
    
    updateUsersInfo();
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
    }
    connect(ui.worker_uncofirmed_table, SIGNAL(cellChanged(int, int)), this, SLOT(changeStatusUncorfimedWorker(int, int)));
  }
  else {
    QMessageBox::critical(this, QString::fromWCharArray(L"Подключение к базе данных"), QString::fromWCharArray(L"Извините, в данный момент база данных недоступна"));
  }
}

 //TODO реализовать автоматическое создание приказов и добавление их в бд
void salary::goToPrikazPage(){
  int rowCount = 0;
  while (ui.prikaz_table->rowCount()) {
    ui.prikaz_table->removeRow(0);
  }

  ui.worktop->setCurrentIndex(2);
  ui.prikaz_search_FIO->clear();
  ui.prikaz_search_date_search->setChecked(false);
  searchPrikazDate();
  ui.prikaz_search_dismissial->setChecked(false);
  ui.prikaz_search_recruitment->setChecked(false);

  if (db.openDB()) {
    prikazes = db.getAllPrikazes();
    for (auto it : prikazes) {

      QString tmpType;
      if (it.getTypeOfPrikaz()) {
        tmpType = QString::fromWCharArray(L"Приём");
      }
      else {
        tmpType = QString::fromWCharArray(L"Увольнение");
      }

      ui.prikaz_table->insertRow(rowCount);
      ui.prikaz_table->setItem(rowCount, 0, new QTableWidgetItem(tmpType));
      ui.prikaz_table->setItem(rowCount, 1, new QTableWidgetItem(it.getDate()));
      ui.prikaz_table->setItem(rowCount, 2, new QTableWidgetItem(it.getFIO()));
      rowCount++;
    }
  }
  else {
    QMessageBox::critical(this, QString::fromWCharArray(L"Подключение к базе данных"), QString::fromWCharArray(L"Извините, в данный момент база данных недоступна"));
  }
}

void salary::goToProjectPage(){
  ui.worktop->setCurrentIndex(5);
  setFioForComboBox(ui.project_edit_list_worker);

  if (db.openDB()) {
    ui.project_list->clear();
    projects = db.getAllProjects();
    qSort(projects.begin(), projects.end());
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
  setFioForComboBox(ui.payroll_FIO);
}

void salary::goToAccountingPage(){
  ui.worktop->setCurrentIndex(4);
  setFioForComboBox(ui.accounting_search_FIO);
  setProjectsForComboBox(ui.accounting_search_project);
  ui.accounting_search_FIO->setEnabled(false);
  ui.accounting_search_project->setEnabled(false);
}

void salary::goToGraphicPage() {
  ui.worktop->setCurrentIndex(7);
  ui.graphics_tab_widget->setCurrentIndex(0);
  setFioForComboBox(ui.graphics_FIO);
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

// Блок сигналов для работника
void salary::goToLKWorker() {
  ui.worktop->setCurrentIndex(1);
  fillWorkerPage(user->getID());
}

// Блок слотов регистрации и входа пользователя
void salary::authorization() {
  if (db.openDB()) {
    user = db.Authorization(ui.enter_login->text(), QCryptographicHash::hash(ui.enter_password->text().toUtf8(), QCryptographicHash::Sha3_512).toHex());
    if (user != nullptr) {
      QMessageBox::information(this, QString::fromWCharArray(L"Авторизация успешна"), QString::fromWCharArray(L"Вы авторизовались"));
      ui.menu_username_leader->setText(user->getFio());
      prepareGuiOnRole();
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
  ui.registration_password->clear();
  ui.registration_fio->clear();
  ui.registration_login->clear();
  ui.registration_password->clear();
  ui.registration_password_repeat->clear();
}

void salary::moveAuthorization() {
  ui.stackedWidget->setCurrentIndex(1);
  ui.enter_password->clear();
  ui.registration_password->clear();
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
    QString tmp_recruitment_date, tmp_dismissial_date;
    update_user.setFio(ui.worker_page_FIO->text());
    if (update_user.getDateReceipt() != ui.worker_page_recruitment_date->text()) {
      tmp_recruitment_date = update_user.getDateReceipt();
      update_user.setDateReceipt(ui.worker_page_recruitment_date->text());
      db.updatePrikaz(true, update_user.getID(), update_user.getDateReceipt(), tmp_recruitment_date);
    }
    if (update_user.isDeleted() && update_user.getDateDismissial() != ui.worker_page_dismissial_date->text()) {
      tmp_dismissial_date = update_user.getDateDismissial();
      update_user.setDateDismissial(ui.worker_page_dismissial_date->text());
      db.updatePrikaz(false, update_user.getID(), update_user.getDateDismissial(), tmp_dismissial_date);
    }

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
    PrikazCreate * w = new PrikazCreate();
    w->setMinimumDate(update_user.getDateReceipt());
    w->exec();
    if (w->isCreateButtonClicked()) {
      if (update_user.isDeleted()) {
        update_user.setDeleted(false);
        update_user.setDateDismissial("2000-01-01");
        ui.worker_page_dismissial_date->setVisible(false);
        ui.label_dismissial->setVisible(false);
        update_user.setDateReceipt(w->date().toString("yyyy-MM-dd"));
        ui.worker_page_recruitment_date->setDate(w->date());
        ui.worker_page_recruitment_date->setDate(QDate::fromString("2000-01-01", "yyyy-DD-mm"));
      }
      else {
        update_user.setDeleted(true);
        update_user.setDateDismissial(w->date().toString("yyyy-MM-dd"));
        ui.worker_page_dismissial_date->setDate(w->date());
        ui.worker_page_dismissial_date->setVisible(true);
        ui.label_dismissial->setVisible(true);
        db.removeWorkerInProject(update_user.getID(), 0, w->date().toString("yyyy-MM-dd"), "", true);
      }
      if (db.updateUser(update_user) && db.createPrikaz(!update_user.isDeleted(), update_user.getID(), this->user->getID(), w->date().toString("yyyy-MM-dd"))) {
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
      w->close();
      updateUsersInfo();
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
  Project prj;
  for (auto it : projects) {
    if (it.getID() == id) {
      prj = it;
    }
  }

  if (fioToUser[ui.project_edit_list_worker->currentText()].getDateReceipt() > QDate::currentDate().toString("yyyy-MM-dd")) {
    QMessageBox::critical(this, QString::fromWCharArray(L"Ошибка назначения сотрудника"), QString::fromWCharArray(L"Вы пытаетесь добавить сотрудника, который еще официально не работает"));
  }
  else {
    PrikazCreate * w = new PrikazCreate();
    w->setLabelText(QString::fromWCharArray(L"Выберите дату назначения сотрудника"));
    w->setMinimumDate(qMax(prj.getDateStart(), fioToUser[ui.project_edit_list_worker->currentText()].getDateReceipt()));
    w->setMaximumDate(QDate::fromString(prj.getDateEnd(), "yyyy-MM-dd").addDays(-1).toString("yyyy-MM-dd"));
    w->setButtonText(QString::fromWCharArray(L"Принять"));
    w->exec();
    if (db.addWorkerInProject(fioToUser[ui.project_edit_list_worker->currentText()].getID(), id, ui.project_edit_position->currentText(), ui.project_edit_coef->value(), prj.getDateEnd(), w->date().toString("yyyy-MM-dd"))) {
      ui.project_edit_table_worker->setRowCount(rowCount + 1);
      QTableWidgetItem * fio = new QTableWidgetItem(ui.project_edit_list_worker->currentText());
      fio->setData(Qt::UserRole, fioToUser[ui.project_edit_list_worker->currentText()].getID());
      ui.project_edit_table_worker->setItem(rowCount, 0, fio);
      ui.project_edit_table_worker->setItem(rowCount, 1, new QTableWidgetItem(ui.project_edit_position->currentText()));
      ui.project_edit_table_worker->setItem(rowCount, 2, new QTableWidgetItem(QString::number(ui.project_edit_coef->value())));
    }
    else {
      QMessageBox::critical(this, QString::fromWCharArray(L"Подключение к базе данных"), QString::fromWCharArray(L"Извините, в данный момент база данных недоступна"));
    }
    w->close();
  }
}

void salary::removeProjectWorker() {
  int id = ui.project_edit_name->property("ID").value<int>();
  Project prj;
  for (auto it : projects) {
    if (it.getID() == id) {
      prj = it;
    }
  }

  if (ui.project_edit_table_worker->currentRow() == -1) {
    QMessageBox::warning(this, QString::fromWCharArray(L"Предупреждение"), QString::fromWCharArray(L"Вы не выбрали сотрудника для удаления"));
  }
  else {
    PrikazCreate * w = new PrikazCreate();
    w->setLabelText(QString::fromWCharArray(L"Выберите дату снятия сотрудника"));
    w->setMinimumDate(QDate::currentDate().toString("yyyy-MM-dd"));
    w->setMaximumDate(QDate::fromString(prj.getDateEnd(), "yyyy-MM-dd").addDays(-1).toString("yyyy-MM-dd"));
    w->setButtonText(QString::fromWCharArray(L"Снять"));
    w->exec();
    int curRow = ui.project_edit_table_worker->currentRow();
    if (db.removeWorkerInProject(fioToUser[ui.project_edit_table_worker->item(curRow, 0)->text()].getID(), id, ui.project_edit_table_worker->item(curRow, 1)->text(), w->date().toString("yyyy-MM-dd"))) {
      if (w->date() == QDate::currentDate()) {
        ui.project_edit_table_worker->removeRow(ui.project_edit_table_worker->currentRow());
      }
    }
    else {
      QMessageBox::critical(this, QString::fromWCharArray(L"Подключение к базе данных"), QString::fromWCharArray(L"Извините, в данный момент база данных недоступна"));
    }
    w->close();
  }
}

void salary::searchProject(const QString & projectPattern) {
  QString project_name;
  for (int i = 0; i < ui.project_list->count(); ++i) {
    project_name = ui.project_list->item(i)->text();
    if (project_name.toLower().contains(projectPattern.toLower())) {
      ui.project_list->item(i)->setHidden(false);
    }
    else {
      ui.project_list->item(i)->setHidden(true);
    }
  }
}

void salary::searchWorker(const QString & workerPattern) {
  QString worker_name;
  for (int i = 0; i < ui.worker_list_current->count(); ++i) {
    worker_name = ui.worker_list_current->item(i)->text();
    if (worker_name.toLower().contains(workerPattern.toLower())) {
      ui.worker_list_current->item(i)->setHidden(false);
    }
    else {
      ui.worker_list_current->item(i)->setHidden(true);
    }
  }
  for (int i = 0; i < ui.worker_list_dismissial->count(); ++i) {
    worker_name = ui.worker_list_dismissial->item(i)->text();
    if (worker_name.toLower().contains(workerPattern.toLower())) {
      ui.worker_list_dismissial->item(i)->setHidden(false);
    }
    else {
      ui.worker_list_dismissial->item(i)->setHidden(true);
    }
  }
  for (int i = 0; i < ui.worker_uncofirmed_table->rowCount(); ++i) {
    worker_name = ui.worker_uncofirmed_table->item(i, 0)->text();
    if (worker_name.toLower().contains(workerPattern.toLower())) {
      ui.worker_uncofirmed_table->setRowHidden(i, false);
    }
    else {
      ui.worker_uncofirmed_table->setRowHidden(i, true);
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
    if (id > 0) {
      if (db.workerExistInProject(id)) {
        QMessageBox::critical(this, QString::fromWCharArray(L"Ошибка обновления"), QString::fromWCharArray(L"На данном проекте есть работник, поэтому\
                                                                                                            вы не можете изменить дату начала проекта"));
        return;
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
        projects = db.getAllProjects();
        qSort(projects.begin(), projects.end());
      }
      else {
        QMessageBox::critical(this, QString::fromWCharArray(L"Подключение к базе данных"), QString::fromWCharArray(L"Извините, не удалось обновить информацию данного пользователя"));
      }
    }
    else {
      if (db.updateProject(prj)) {
        QMessageBox::information(this, QString::fromWCharArray(L"Обновление информации"), QString::fromWCharArray(L"Информация успешно сохранена"));
        projects = db.getAllProjects();
        qSort(projects.begin(), projects.end());
      }
      else {
        QMessageBox::critical(this, QString::fromWCharArray(L"Подключение к базе данных"), QString::fromWCharArray(L"Извините, не удалось обновить информацию данного пользователя"));
      }
    }
  }
}

void salary::accountingShow(const QVector<InfoForAccounting> & other) {
  while (ui.accounting_table->rowCount()) {
    ui.accounting_table->removeRow(0);
  }

  if (user->compareRole(User::kWorker)) {
    QDate date;
    date.setDate(ui.accounting_year->currentText().toInt(), ui.accounting_mounth->currentIndex() + 1, 1);
    if (date > QDate::currentDate()) {
      return;
    }
  }

  if (db.openDB()) {
    disconnect(ui.accounting_table, SIGNAL(cellChanged(int, int)), this, SLOT(saveMarkForUser(int, int)));
    QVector<InfoForAccounting> tmp;
    if (other.isEmpty()) {
      tmp = db.getForAccounting(ui.accounting_mounth->currentIndex() + 1, ui.accounting_year->currentText().toInt());
    }
    else {
      tmp = other;
    }
    ui.accounting_table->setRowCount(tmp.size());
    int rowCount = 0;
    int idx = 0;
    for (auto it : tmp) {
      if (user->compareRole(User::kWorker) && it.fio != user->getFio()) {
        continue;
      }
      ++rowCount;

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
    ui.accounting_table->setRowCount(rowCount);
    connect(ui.accounting_table, SIGNAL(cellChanged(int, int)), this, SLOT(saveMarkForUser(int, int)));
  }
  searchAccounting();
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
  searchAccounting();
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
  int month = ui.payroll_mounth->currentIndex() + 1;
  int year = ui.payroll_year->currentText().toInt();
  QString fio = ui.payroll_FIO->currentText();
  int id_user = fioToUser[fio].getID();

  if (user->compareRole(User::kWorker)) {
    id_user = user->getID();
  }

  if (db.openDB()) {
    QVector<InfoForAccounting> markWithNull = db.getForAccounting(month, year, true);
    if (!markWithNull.isEmpty()) {
      if (user->compareRole(User::kWorker)) {}
      else if (user->compareRole(User::kAdmin)) {
        QMessageBox::warning(this, QString::fromWCharArray(L"Предупреждение"), QString::fromWCharArray(L"Оценки за данный месяц выставлены не у всех работников.\nПожалуйста, поставьте оценки."));
        goToAccountingPage();
        accountingShow(markWithNull);
      }
      return;
    }

    ProjectWithDateWorkerForPayroll * list_project = db.getProjectForWorkerOnDate(id_user, month, year);
    QHash<int, int> projectToMonth;
    QHash<int, LD> projectPayrollOneMonth;
    QHash<int, LD> summaryCoefForProject;
    int rowCount = preparePayroll(list_project, projectToMonth, projectPayrollOneMonth, summaryCoefForProject);
    
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

void salary::calculateGraphics() {
  int month = ui.graphics_mounth->currentIndex() + 1;
  int year = ui.graphics_year->currentText().toInt();
  QString fio = ui.graphics_FIO->currentText();
  int id_user = fioToUser[fio].getID();

  if (user->compareRole(User::kWorker)) {
    id_user = user->getID();
  }

  if (db.openDB()) {
    QVector<InfoForAccounting> markWithNull = db.getForAccounting(month, year, true);
    if (!markWithNull.isEmpty()) {
      if (user->compareRole(User::kWorker)) {}
      else if (user->compareRole(User::kAdmin)) {
        QMessageBox::warning(this, QString::fromWCharArray(L"Предупреждение"), QString::fromWCharArray(L"Оценки за данный месяц выставлены не у всех работников.\nПожалуйста, поставьте оценки."));
        goToAccountingPage();
        accountingShow(markWithNull);
      }
      return;
    }

    ui.graphics_tab_widget->setHidden(false);

    ProjectWithDateWorkerForPayroll * list_project = db.getProjectForWorkerOnDate(id_user, month, year, true);
    QHash<int, int> projectToMonth;
    QHash<int, LD> projectPayrollOneMonth;
    QHash<int, LD> summaryCoefForProject;
    preparePayroll(list_project, projectToMonth, projectPayrollOneMonth, summaryCoefForProject, true);
    drawPieChartForWorker(list_project, projectToMonth, projectPayrollOneMonth, summaryCoefForProject);
    if (user->compareRole(User::kAdmin)) {
      drawPieChartProjectByProject(list_project, projectToMonth, projectPayrollOneMonth, summaryCoefForProject);
      drawPieChartProjectByWorker(list_project, projectToMonth, projectPayrollOneMonth, summaryCoefForProject);
    }
    updateThemeGraphics();
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

    disconnect(ui.project_edit_date_begin, SIGNAL(dateChanged(const QDate &)), this, SLOT(rewriteCountDotation()));
    disconnect(ui.project_edit_date_end, SIGNAL(dateChanged(const QDate &)), this, SLOT(rewriteCountDotation()));
    ui.project_edit_name->setText(concrete_project.getProjectName());
    ui.project_edit_name->setProperty("ID", id);
    ui.project_edit_budget->setValue(concrete_project.getBudget());
    ui.project_edit_date_begin->setDate(QDate::fromString(concrete_project.getDateStart(), QString("yyyy-MM-dd")));
    ui.project_edit_date_end->setDate(QDate::fromString(concrete_project.getDateEnd(), QString("yyyy-MM-dd")));
    ui.project_edit_mounth->setValue(monthBetweenToDate(concrete_project.getDateStart(), concrete_project.getDateEnd()));
    connect(ui.project_edit_date_begin, SIGNAL(dateChanged(const QDate &)), this, SLOT(rewriteCountDotation()));
    connect(ui.project_edit_date_end, SIGNAL(dateChanged(const QDate &)), this, SLOT(rewriteCountDotation()));

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
      ui.worker_page_dismissial_date->setVisible(concrete_user->user.isDeleted());
      ui.label_dismissial->setVisible(concrete_user->user.isDeleted());
    }
    else {
      QMessageBox::critical(this, QString::fromWCharArray(L"Подключение к базе данных"), QString::fromWCharArray(L"Извините, не удалось получить информацию данного пользователя"));
    }
  }
  else {
    QMessageBox::critical(this, QString::fromWCharArray(L"Подключение к базе данных"), QString::fromWCharArray(L"Извините, в данный момент база данных недоступна"));
  }
}

void salary::searchPrikaz(const QString &FIO) {
  QString fio = FIO;
  if (fio.isEmpty()) {
    fio = ui.prikaz_search_FIO->text();
  }
  for (int idx = 0; idx < ui.prikaz_table->rowCount(); ++idx) {
    bool visible = true;
    if (!ui.prikaz_table->item(idx, 2)->text().toLower().contains(fio.toLower())) {
      ui.prikaz_table->setRowHidden(idx, true);
      visible = false;
    }
    if ((ui.prikaz_search_recruitment->isChecked() != ui.prikaz_search_dismissial->isChecked()) &&
      ((ui.prikaz_search_recruitment->isChecked() && ui.prikaz_table->item(idx, 0)->text() == QString::fromWCharArray(L"Увольнение")) ||
      (ui.prikaz_search_dismissial->isChecked() && ui.prikaz_table->item(idx, 0)->text() == QString::fromWCharArray(L"Приём")))) {
      ui.prikaz_table->setRowHidden(idx, true);
      visible = false;
    }

    if (ui.prikaz_search_date_search->isChecked() && 
      (ui.prikaz_table->item(idx, 1)->text() < ui.prikaz_search_date_start->text() ||
      ui.prikaz_table->item(idx, 1)->text() > ui.prikaz_search_date_end->text())) {
      ui.prikaz_table->setRowHidden(idx, true);
      visible = false;
    }
    if (visible) {
      ui.prikaz_table->setRowHidden(idx, false);
    }
  }
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
  if (ui.prikaz_table->currentRow() == -1) {
    QMessageBox::warning(this, QString::fromWCharArray(L"Внимание"), QString::fromWCharArray(L"Выберите приказ прежде чем вывести его на печать"));
  }
  else {
    prikazes[ui.prikaz_table->currentRow()].printPrikaz();
  }
}

int salary::monthBetweenToDate(const QString & start, const QString & end) {
  QDate cpy_start = QDate::fromString(start, QString("yyyy-MM-dd"));
  QDate cpy_end = QDate::fromString(end, QString("yyyy-MM-dd"));
  cpy_start.setDate(cpy_start.year(), cpy_start.month(), 1);
  cpy_end.setDate(cpy_end.year(), cpy_end.month(), 1);
  int result = 0;
  while (cpy_start <= cpy_end) {
    ++result;
    cpy_start = cpy_start.addMonths(1);
  }
  return result;
}

void salary::setFioForComboBox(QComboBox * box) {
  box->clear();
  if (user->compareRole(User::kWorker)) {
    box->addItem(user->getFio());
    box->setEnabled(false);
  }
  else if (user->compareRole(User::kAdmin)) {
    for (auto it : users) {
      if (!it.isDeleted() && it.isConfirmed()) {
        box->addItem(it.getFio());
      }
    }
    box->setEnabled(true);
  }
}

void salary::setProjectsForComboBox(QComboBox * box) {
  box->clear();
  projects = db.getAllProjects();
  if (user->compareRole(User::kAdmin)) {
    for (auto it : projects) {
      box->addItem(it.getProjectName());
    }
    box->setEnabled(true);
  }
}

int salary::preparePayroll(const ProjectWithDateWorkerForPayroll * list_project, QHash<int, int> & projectToMonth, QHash<int, LD> & projectPayrollOneMonth, QHash<int, LD> & summaryCoefForProject, bool change_mode) {
  int rowCount = 0;
  if (change_mode) {
    for (auto it : list_project->all_projects) {
      projectToMonth[it.getID()] = monthBetweenToDate(it.getDateStart(), it.getDateEnd());
      projectPayrollOneMonth[it.getID()] = LD(it.getBudget()) / projectToMonth[it.getID()];
    }
  }
  else {
    for (auto it : list_project->projects) {
      projectToMonth[it.getID()] = monthBetweenToDate(it.getDateStart(), it.getDateEnd());
      projectPayrollOneMonth[it.getID()] = LD(it.getBudget()) / projectToMonth[it.getID()];
    }
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
  return rowCount;
}

void salary::drawPieChartForWorker(const ProjectWithDateWorkerForPayroll * list_project, const QHash<int, int> & projectToMonth, const QHash<int, LD> & projectPayrollOneMonth, const QHash<int, LD> & summaryCoefForProject) {
  DrilldownChart *chart = new DrilldownChart();
  chart->setAnimationOptions(QChart::AllAnimations);
  chart->legend()->setVisible(true);
  chart->legend()->setAlignment(Qt::AlignRight);

  QPieSeries *salarySeries = new QPieSeries();

  int id_user = fioToUser[ui.graphics_FIO->currentText()].getID();
  if (user->compareRole(User::kWorker)) {
    id_user = user->getID();
  }

  for (auto prj : list_project->idToProject.values()) {
    LD salary_value = 0;
    for (auto pos : list_project->helpInfoOtherWorker[id_user][prj.getID()].keys()) {
      salary_value += projectPayrollOneMonth[prj.getID()] / summaryCoefForProject[prj.getID()] * list_project->helpInfoOtherWorker[id_user][prj.getID()][pos];
    }
    if (salary_value > 0) {
      *salarySeries << new DrilldownSlice(salary_value, prj.getProjectName(), salarySeries);
    }
  }
  chart->changeSeries(salarySeries);
  ui.graphics_salary_for_worker->setChart(chart);
  ui.graphics_salary_for_worker->setRenderHint(QPainter::Antialiasing);
  ui.graphics_salary_for_worker->chart()->setTitle(QString::fromWCharArray(L"Заработная плата работника на проектах"));
  ui.graphics_salary_by_worker->chart()->setTheme(this->theme_color_graphics);
  ui.graphics_salary_for_worker->chart()->legend()->setFont(QFont("Arial", 10));
}

void salary::drawPieChartProjectByProject(const ProjectWithDateWorkerForPayroll * list_project, const QHash<int, int> & projectToMonth, const QHash<int, LD> & projectPayrollOneMonth, const QHash<int, LD> & summaryCoefForProject) {
  DrilldownChart *chart = new DrilldownChart();
  chart->setAnimationOptions(QChart::AllAnimations);
  chart->legend()->setVisible(true);
  chart->legend()->setAlignment(Qt::AlignRight);

  QPieSeries *salarySeries = new QPieSeries();
  QHash<QString, LD> payrollByProject;
  LD salary_value = 0;
  for (auto prj : list_project->idToProject.values()) {
    salary_value = 0;
    if (!payrollByProject.contains(prj.getProjectName())) {
      payrollByProject[prj.getProjectName()] = 0;
    }
    for (auto pos : list_project->helpInfo[prj.getID()].keys()) {
      salary_value += projectPayrollOneMonth[prj.getID()] / summaryCoefForProject[prj.getID()] * list_project->helpInfo[prj.getID()][pos];
    }
    payrollByProject[prj.getProjectName()] += salary_value;
  }
  for (auto id_other_worker : list_project->helpInfoOtherWorker.keys()) {
    for (auto id_project : list_project->helpInfoOtherWorker[id_other_worker].keys()) {
      salary_value = 0;
      if (!payrollByProject.contains(list_project->idToProject[id_project].getProjectName())) {
        payrollByProject[list_project->idToProject[id_project].getProjectName()] = 0;
      }
      for (auto pos : list_project->helpInfoOtherWorker[id_other_worker][id_project].keys()) {
        salary_value += projectPayrollOneMonth[id_project] / summaryCoefForProject[id_project] * list_project->helpInfoOtherWorker[id_other_worker][id_project][pos];
      }
      payrollByProject[list_project->idToProject[id_project].getProjectName()] += salary_value;
    }
  }

  for (auto it : payrollByProject.keys()) {
    *salarySeries << new DrilldownSlice(payrollByProject[it], it, salarySeries);
  }

  chart->changeSeries(salarySeries);
  ui.graphics_salary_by_project->setChart(chart);
  ui.graphics_salary_by_project->setRenderHint(QPainter::Antialiasing);
  ui.graphics_salary_by_project->chart()->setTitle(QString::fromWCharArray(L"Заработная плата по проектам"));
  ui.graphics_salary_by_worker->chart()->setTheme(this->theme_color_graphics);
  ui.graphics_salary_by_project->chart()->legend()->setFont(QFont("Arial", 10));
}

void salary::drawPieChartProjectByWorker(const ProjectWithDateWorkerForPayroll * list_project, const QHash<int, int> & projectToMonth, const QHash<int, LD> & projectPayrollOneMonth, const QHash<int, LD> & summaryCoefForProject) {
  DrilldownChart *chart = new DrilldownChart();
  chart->setAnimationOptions(QChart::AllAnimations);
  chart->legend()->setVisible(true);
  chart->legend()->setAlignment(Qt::AlignRight);

  QPieSeries *salarySeries = new QPieSeries();
  QHash<QString, LD> payrollByWorker;
  payrollByWorker[fioToUser[ui.graphics_FIO->currentText()].getFio()] = 0;
  
  LD salary_value = 0;
  for (auto prj : list_project->idToProject.values()) {
    salary_value = 0;
    for (auto pos : list_project->helpInfo[prj.getID()].keys()) {
      salary_value += projectPayrollOneMonth[prj.getID()] / summaryCoefForProject[prj.getID()] * list_project->helpInfo[prj.getID()][pos];
    }
    payrollByWorker[fioToUser[ui.graphics_FIO->currentText()].getFio()] += salary_value;
  }
  for (auto id_other_worker : list_project->helpInfoOtherWorker.keys()) {
    payrollByWorker[idToUser[id_other_worker].getFio()] = 0;
    for (auto id_project : list_project->helpInfoOtherWorker[id_other_worker].keys()) {
      salary_value = 0;
      for (auto pos : list_project->helpInfoOtherWorker[id_other_worker][id_project].keys()) {
        salary_value += projectPayrollOneMonth[id_project] / summaryCoefForProject[id_project] * list_project->helpInfoOtherWorker[id_other_worker][id_project][pos];
      }
      payrollByWorker[idToUser[id_other_worker].getFio()] += salary_value;
    }
  }

  for (auto it : payrollByWorker.keys()) {
    *salarySeries << new DrilldownSlice(payrollByWorker[it], it, salarySeries);
  }

  chart->changeSeries(salarySeries);
  ui.graphics_salary_by_worker->setChart(chart);
  ui.graphics_salary_by_worker->setRenderHint(QPainter::Antialiasing);
  ui.graphics_salary_by_worker->chart()->setTitle(QString::fromWCharArray(L"Заработная плата по работникам"));
  ui.graphics_salary_by_worker->chart()->setTheme(this->theme_color_graphics);
  ui.graphics_salary_by_worker->chart()->legend()->setFont(QFont("Arial", 10));
}

void salary::worktopChanged(int index) {
  history_window.push_back(index);
}

void salary::setColorLight() {
  qApp->setStyleSheet("");
  this->theme_color_graphics = QChart::ChartThemeLight;
  this->theme_is_light = true;
  updateThemeGraphics();
}

void salary::setColorDark() {
  qApp->setStyleSheet(this->style_dark_theme);
  this->theme_color_graphics = QChart::ChartThemeDark;
  this->theme_is_light = false;
  updateThemeGraphics();
}

void salary::updateThemeGraphics() {
  ui.graphics_salary_for_worker->chart()->setTheme(this->theme_color_graphics);
  ui.graphics_salary_for_worker->setRenderHint(QPainter::Antialiasing);
  ui.graphics_salary_for_worker->chart()->legend()->setFont(QFont("Arial", 10));
  
  if (user->compareRole(User::kAdmin)) {
    ui.graphics_salary_by_worker->chart()->setTheme(this->theme_color_graphics);
    ui.graphics_salary_by_worker->setRenderHint(QPainter::Antialiasing);
    ui.graphics_salary_by_worker->chart()->legend()->setFont(QFont("Arial", 10));
    ui.graphics_salary_by_project->chart()->setTheme(this->theme_color_graphics);
    ui.graphics_salary_by_project->setRenderHint(QPainter::Antialiasing);
    ui.graphics_salary_by_project->chart()->legend()->setFont(QFont("Arial", 10));
  }
}

void salary::rewriteCountDotation() {
  ui.project_edit_mounth->setValue(monthBetweenToDate(ui.project_edit_date_begin->text(), ui.project_edit_date_end->text()));
}

void salary::updateUsersInfo() {
  users = db.getAllUsers();
  qSort(users.begin(), users.end());
  fioToUser.clear();
  idToUser.clear();
  for (auto it : users) {
    fioToUser[it.getFio()] = it;
    idToUser[it.getID()] = it;
  }
}

void salary::prepareGuiOnRole() {
  ui.stackedWidget->setCurrentIndex(0);
  ui.graphics_tab_widget->setHidden(true);
  if (user->compareRole(User::kWorker)) {
    goToLKWorker();

    disconnect(ui.menu_worker, SIGNAL(clicked()), this, SLOT(goToWorkerPage()));
    ui.menu_worker->setText(QString::fromWCharArray(L"Личный кабинет"));
    connect(ui.menu_worker, SIGNAL(clicked()), this, SLOT(goToLKWorker()));

    ui.worker_page_FIO->setReadOnly(true);
    ui.worker_page_b_day->setReadOnly(true);
    ui.worker_page_recruitment_date->setReadOnly(true);
    ui.worker_page_dismissial_date->setReadOnly(true);
    ui.worker_page_change_status->setVisible(false);
    ui.worker_page_save->setVisible(false);
    disconnect(ui.worker_page_table_project, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(goToCurrentProjectPage(int, int)));

    ui.menu_prikaz->setVisible(false);
    ui.menu_project->setVisible(false);

    ui.menu_accounting->setText(QString::fromWCharArray(L"Просмотр KPI"));
    ui.groupBox_6->setTitle(QString::fromWCharArray(L"Выберите период, за который хотите посмотреть оценки"));
    ui.accounting_table->setItemDelegateForColumn(5, new NonEditTableColumnDelegate());

    ui.groupBox_8->setTitle(QString::fromWCharArray(L"Выберите период, за который хотите посчитать зарплату"));

    ui.graphics_tab_widget->setCurrentIndex(1);
    hiddenChart.push_back(static_cast<QChartView *>(ui.graphics_tab_widget->currentWidget()));
    ui.graphics_tab_widget->removeTab(1);
    ui.graphics_tab_widget->setCurrentIndex(1);
    hiddenChart.push_back(static_cast<QChartView *>(ui.graphics_tab_widget->currentWidget()));
    ui.graphics_tab_widget->removeTab(1);
  }
  else if (user->compareRole(User::kAdmin)) {
    goToWorkerPage();

    disconnect(ui.menu_worker, SIGNAL(clicked()), this, SLOT(goToLKWorker()));
    ui.menu_worker->setText(QString::fromWCharArray(L"Мои работники"));
    connect(ui.menu_worker, SIGNAL(clicked()), this, SLOT(goToWorkerPage()));

    ui.worker_page_FIO->setReadOnly(false);
    ui.worker_page_b_day->setReadOnly(false);
    ui.worker_page_recruitment_date->setReadOnly(false);
    ui.worker_page_dismissial_date->setReadOnly(false);
    ui.worker_page_change_status->setVisible(true);
    ui.worker_page_save->setVisible(true);
    connect(ui.worker_page_table_project, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(goToCurrentProjectPage(int, int)));

    ui.menu_prikaz->setVisible(true);
    ui.menu_project->setVisible(true);

    ui.menu_accounting->setText(QString::fromWCharArray(L"Учет результатов"));
    ui.groupBox_6->setTitle(QString::fromWCharArray(L"Выберите период, за который будут проставляться оценки для каждого работника"));
    ui.accounting_table->setItemDelegateForColumn(5, new TableDelegateWithValidator());

    ui.groupBox_8->setTitle(QString::fromWCharArray(L"Выберите сотрудника, для которого требуется рассчитать зарплату за конкретный период"));

    if (!hiddenChart.isEmpty()) {
      ui.graphics_tab_widget->insertTab(1, hiddenChart.front(), QString::fromWCharArray(L"ЗП по проектам"));
      hiddenChart.pop_front();
      ui.graphics_tab_widget->insertTab(2, hiddenChart.front(), QString::fromWCharArray(L"ЗП по сотрудникам"));
      hiddenChart.pop_front();
    }
  }
}

void salary::accountingInfoShow() {
  QMessageBox::information(this, QString::fromWCharArray(L"Справка по распределению оценочных баллов"),
    QString::fromWCharArray(L"    Выставляя работникам оценки за каждый конкретный проект, учитывайте\
                             только результаты работы пользователей, а не количество отработанных\
                             ими дней.\n\n    Количество отработанных на проекте дней учитывайтся автоматически\
                             \n\n    Проставляйте оценки от одного до десяти для оценки участия\
                             конкретного работника в конкретном проекте за выбранный месяц. \n\n    Критерии оценки:\
                             \n     1 - работник не справился с поставленными задачами, работал очень плохо.\
                             \n     10 - работник отлично справился со всеми поставленными ему задачами.\
                             \n\n    Все промежуточные оценки условны и выставляются на усмотрение руководителя.\
                             \n\n    Поскольку программа учитывает оценки пользователей, коэффициент, а\
                             также количество отработанных на проекте дней, некоторые работники\
                             могут быть премированы средствами, которые не получили работники, \
                             плохо справившиеся с поставленными задачами."));
}

void salary::searchAccounting() {
  ui.accounting_search_FIO->setEnabled(ui.accounting_filter_FIO->isChecked());
  ui.accounting_search_project->setEnabled(ui.accounting_filter_project->isChecked());
  ui.accounting_search_mark->setEnabled(ui.accounting_filter_mark->isChecked());
  for (int idx = 0; idx < ui.accounting_table->rowCount(); ++idx) {
    ui.accounting_table->setRowHidden(idx, false);
  }

  for (int idx = 0; idx < ui.accounting_table->rowCount(); ++idx) {
    if (ui.accounting_filter_FIO->isChecked() &&
      ui.accounting_search_FIO->currentText() != ui.accounting_table->item(idx, 1)->text()) {
      ui.accounting_table->setRowHidden(idx, true);
    }
    if (ui.accounting_filter_project->isChecked() &&
      ui.accounting_search_project->currentText() != ui.accounting_table->item(idx, 0)->text()) {
      ui.accounting_table->setRowHidden(idx, true);
    }
    if (ui.accounting_filter_mark->isChecked() &&
      ((ui.accounting_search_mark->currentIndex() == 0 && ui.accounting_table->item(idx, 5)->text() == "0") ||
      (ui.accounting_search_mark->currentIndex() == 1 && ui.accounting_table->item(idx, 5)->text() != "0"))) {
      ui.accounting_table->setRowHidden(idx, true);
    }
  }
}