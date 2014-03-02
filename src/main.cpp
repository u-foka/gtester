#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("gtester");
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    a.setApplicationDisplayName("GTester");
#endif
    a.setApplicationVersion(GTESTER_VERSION);
    a.setOrganizationDomain("iwstudio.hu");
    a.setOrganizationName("IWStudio");

    MainWindow w;
    w.show();

    return a.exec();
}
