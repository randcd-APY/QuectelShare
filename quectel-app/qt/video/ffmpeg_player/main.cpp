//#include "gallery.h"
#include "filelist.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    fileList w;
//    Gallery w;
    w.show();

    return a.exec();
}
