#include "salary.h"
#include <QtWidgets/QApplication>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
  a.setWindowIcon(QIcon("images/icon.png"));
  QApplication::setStyle(QStyleFactory::create("Fusion"));
	salary w;
	w.show();
	return a.exec();
}
