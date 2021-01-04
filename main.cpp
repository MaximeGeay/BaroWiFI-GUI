#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("Barographe");
    QCoreApplication::setApplicationName("BaroConfig");

    MainWindow w;
    w.show();

    return a.exec();
}
