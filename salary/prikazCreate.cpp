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