#include "prikaz.h"

Prikaz::Prikaz(const QSqlQuery & query) {
  this->id = query.value(0).toInt();
  this->id_creator = query.value(1).toInt();
  this->id_user = query.value(2).toInt();
  this->FIO = query.value(3).toString();
  this->type_of_prikaz = query.value(4).toBool();
  this->date = query.value(5).toString();
}

Prikaz::Prikaz() {
}

Prikaz::~Prikaz() {
}

int Prikaz::getId() const {
  return this->id;
}

QString Prikaz::getFIO() const {
  return this->FIO;
}

int Prikaz::getIdUser() const {
  return this->id_user;
}

int Prikaz::getIdCreator() const {
  return this->id_creator;
}

bool Prikaz::getTypeOfPrikaz() const {
  return this->type_of_prikaz;
}

QString Prikaz::getDate() const {
  return this->date;
}


void Prikaz::printPrikaz() const {
  QString filename;
  filename += QString::number(this->getId() + 1) + "_prikaz.pdf";
  QPdfWriter writer(filename);
  writer.setPageSize(QPagedPaintDevice::A4);
  writer.setPageMargins(QMargins(30, 30, 30, 30));
  QPainter painter(&writer);

  painter.setPen(Qt::black);
  painter.setFont(QFont("Times New Roman", 10));


  QRect r = painter.viewport();

  QString text;
  if (this->getTypeOfPrikaz()) {
    text = QString::fromWCharArray(L"Унифицированная форма № Т-1\nУтверждена постановлением Госкомстата\nРоссии\nот 05.01.2004 № 1");
    painter.drawText(40, 8500, QString::fromWCharArray(L"без тарифной ставки, с получением оклада согласно проделанной работе"));
  }
  else {
    text = QString::fromWCharArray(L"Унифицированная форма № Т-8\nУтверждена постановлением Госкомстата\nРоссии\nот 05.01.2004 № 1");
  }
  painter.setPen(Qt::black);
  painter.setFont(QFont("Times New Roman", 8));
  painter.drawText(r, Qt::AlignRight, text);
  painter.drawText(3000, 2200, QString::fromWCharArray(L"(наименование организации)"));
  painter.drawText(3000, 5700, QString::fromWCharArray(L"(фамилия,имя,отчество)"));
  painter.drawText(3000, 6500, QString::fromWCharArray(L"(структурное подразделение)"));
  painter.drawText(1500, 7000, QString::fromWCharArray(L"(должность (специальность, профессия), разряд, класс (категория) квалификации)"));
  painter.drawText(2500, 8000, QString::fromWCharArray(L"(условия приема на работу, характер работы)"));
  painter.drawText(2800, 10600, QString::fromWCharArray(L"  (должность)                              (личная подпись)                 (расшировка подписи)"));
  painter.drawText(4800, 11200, QString::fromWCharArray(L"  (подпись)                              (дата)"));

  painter.setFont(QFont("Times New Roman", 10));
  painter.drawText(40, 9500, QString::fromWCharArray(L"Основание:"));
  painter.drawText(4650, 2600, QString::fromWCharArray(L"Номер             Дата"));
  painter.drawText(8300, 3850, QString::fromWCharArray(L"Дата"));
  painter.drawText(7050, 5150, QString::fromWCharArray(L"Табельный номер"));
  painter.drawText(7500, 5450, QString::number(this->getIdUser()));
  painter.drawText(7500, 4150, QString::fromWCharArray(L"c"));
  painter.drawText(7500, 4450, QString::fromWCharArray(L"по"));

  painter.drawText(40, 9800, QString::fromWCharArray(L"   Трудовой договор от “_____ ”___________ 20_____ г. №_________"));

  painter.drawText(40, 10400, QString::fromWCharArray(L"Руководитель организации"));
  painter.drawText(1800, 10400, QString::fromWCharArray(L"             _______________________    ________________  _______________________"));
  painter.drawText(40, 11000, QString::fromWCharArray(L"С приказом (распоряжением) работник ознакомлен"));
  painter.drawText(3500, 11060, QString::fromWCharArray(L"            _____________________ “_____ ” __________ 20____г."));



  painter.drawText(100, 6300, QString::fromWCharArray(L"в"));

  painter.setFont(QFont("Times New Roman", 12, 100, false));
  painter.drawText(3200, 2800, (QString::fromWCharArray(L"ПРИКАЗ              ") + QString::number(this->getId() + 1)));
  painter.drawText(5450, 2800, this->getDate());
  painter.drawText(3200, 3100, QString::fromWCharArray(L"(распоряжение)"));
  if (this->getTypeOfPrikaz()) {
    painter.drawText(2700, 3300, QString::fromWCharArray(L"о приеме работника на работу"));
    painter.drawText(3200, 4000, QString::fromWCharArray(L"Принять на работу"));
  }
  else {
    painter.drawText(2500, 3300, QString::fromWCharArray(L"о прекращении трудового договора"));
    painter.drawText(3200, 4000, QString::fromWCharArray(L"Уволить с работы"));
  }


  painter.setFont(QFont("Times New Roman", 12));
  painter.drawText(6200, 1700, QString::fromWCharArray(L"Форма по ОКУД          0301001"));
  painter.drawText(6700, 2000, QString::fromWCharArray(L"по ОКПО"));
  painter.drawText(2500, 5400, this->getFIO());
  painter.drawText(8300, 1400, QString::fromWCharArray(L"Код"));


  QPen pen(Qt::black, 5, Qt::SolidLine);
  painter.setPen(pen);
  painter.drawLine(40, 2000, 6500, 2000);
  painter.drawLine(40, 5500, 8500, 5500); 
  painter.drawLine(200, 6300, 8500, 6300);
  painter.drawLine(40, 6800, 8500, 6800);
  painter.drawLine(40, 7300, 8500, 7300);
  painter.drawLine(40, 7800, 8500, 7800);
  QRectF r1(8000, 1200, 1000, 300);
  QRectF r2(8000, 1500, 1000, 300);
  QRectF r3(8000, 1800, 1000, 300);
  QRectF r4(8000, 3600, 1000, 300);
  QRectF r5(8000, 3900, 1000, 300);
  QRectF r6(8000, 4200, 1000, 300);
  QRectF r7(7000, 3900, 1000, 300);
  QRectF r8(7000, 4200, 1000, 300);
  QRectF r9(7000, 4900, 1500, 300);
  QRectF r10(7000, 5200, 1500, 300);
  QRectF r11(5400, 2425, 1000, 200);
  QRectF r12(4400, 2425, 1000, 200);
  QRectF r13(5400, 2625, 1000, 200);
  QRectF r14(4400, 2625, 1000, 200);
  painter.drawRect(r1);
  painter.drawRect(r2);
  painter.drawRect(r3);
  painter.drawRect(r4);
  painter.drawRect(r5);
  painter.drawRect(r6);
  painter.drawRect(r7);
  painter.drawRect(r8);
  painter.drawRect(r9);
  painter.drawRect(r10);
  painter.drawRect(r11);
  painter.drawRect(r12);
  painter.drawRect(r13);
  painter.drawRect(r14);

  painter.end();
}