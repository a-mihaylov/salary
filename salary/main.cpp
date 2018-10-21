#include "salary.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	salary w;
	w.show();
	return a.exec();
}
