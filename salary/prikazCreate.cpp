#include "prikazCreate.h"


PrikazCreate::PrikazCreate(QWidget *parent)
    : QDialog(parent) {
  prikazui.setupUi(this);
  createButtonClicked = false;
  prikazui.prikaz_create_date->setDate(QDate::currentDate());
  bool a = connect(prikazui.prikaz_create_create, SIGNAL(clicked()), this, SLOT(createPrikazResult()));
  bool c = connect(prikazui.prikaz_create_cancel, SIGNAL(clicked()), this, SLOT(canselPrikazCreate()));
}

PrikazCreate::~PrikazCreate() {

}

void PrikazCreate::createPrikazResult() {
  createButtonClicked = true;
  hide();
}

void PrikazCreate::canselPrikazCreate() {
  close();
}

QDate PrikazCreate::date() {
  return prikazui.prikaz_create_date->date();
}

bool PrikazCreate::isCreateButtonClicked() {
  return createButtonClicked;
}

void PrikazCreate::setMinimumDate(const QString & date) {
  prikazui.prikaz_create_date->setMinimumDate(QDate::fromString(date, "yyyy-MM-dd"));
}

void PrikazCreate::setMaximumDate(const QString & date) {
  prikazui.prikaz_create_date->setMaximumDate(QDate::fromString(date, "yyyy-MM-dd"));
}

void PrikazCreate::setLabelText(const QString & text) {
  prikazui.label->setText(text);
}

void PrikazCreate::setButtonText(const QString & text) {
  prikazui.prikaz_create_create->setText(text);
}