#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("gtester");
    a.setApplicationDisplayName("GTester");
    a.setApplicationVersion(GTESTER_VERSION);
    a.setOrganizationDomain("iwstudio.hu");
    a.setOrganizationName("IWStudio");

    MainWindow w;
    w.show();

    return a.exec();
}
