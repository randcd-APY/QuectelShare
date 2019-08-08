#include "widget.h"
#include "ql_sensor.h"
#include <pthread.h>
#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QApplication::setOverrideCursor(Qt::BlankCursor);
	Widget w;
	w.show();

	return a.exec();
}
