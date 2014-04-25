#include "defaultmenu.h"

#include <QMessageBox>

#include "application.h"

DefaultMenu::DefaultMenu(QWidget *parent) :
    QMenuBar(parent)
{
    // Default main menu
    QMenu *fileMenu = addMenu(tr("&File"));
    fileMenu->addAction(tr("About GTester"), AppInstance, SLOT(ShowAboutBox()));
    fileMenu->addAction(tr("About Qt"), this, SLOT(ShowAboutQtBox()));
    fileMenu->addAction(tr("&Quit GTester"), AppInstance, SLOT(CloseAndQuit()));
    QAction *fileNew = fileMenu->addAction(tr("&New"), AppInstance, SLOT(OpenNewWindow()));
    fileNew->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_N));
    fileNew->setIcon(QIcon(":/icons/document-new.png"));
    QAction *fileOpen = fileMenu->addAction(tr("&Open"), AppInstance, SLOT(SelectOpenFile()));
    fileOpen->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_O));
    fileOpen->setIcon(QIcon(":/icons/document-open.png"));

    QMenu *helpMenu = addMenu(tr("&Help"));
    helpMenu->addAction(tr("&Report a bug"), AppInstance, SLOT(ReportABug()));

    // Dock icon menu
    QMenu *dockMenu = new QMenu(this);
    dockMenu->addAction(tr("New File"), AppInstance, SLOT(OpenNewWindow()));

#ifdef Q_OS_MAC
    extern void qt_mac_set_dock_menu(QMenu *);
    qt_mac_set_dock_menu(dockMenu);
#endif
}

void DefaultMenu::ShowAboutQtBox()
{
    QMessageBox::aboutQt(this);
}
