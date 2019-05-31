#include "qcamdemo.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCamDemo w;
    w.show();

    return a.exec();
}
