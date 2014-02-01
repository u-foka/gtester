#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("gtester");
    a.setApplicationDisplayName("GTester");
    a.setApplicationVersion("1.0");
    a.setOrganizationDomain("iwstudio.hu");
    a.setOrganizationName("IWStudio");

    MainWindow w;
    w.show();

    return a.exec();
}
