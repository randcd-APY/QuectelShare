#include "gps.h"
#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setOverrideCursor(Qt::BlankCursor);
    gps w;

    w.show();
    //test_main();

    return a.exec();
}
