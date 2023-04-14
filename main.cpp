#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.addLibraryPath(a.applicationDirPath()+"/plugins");
    MainWindow w;
    w.show();

    return a.exec();
}
