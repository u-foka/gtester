#include "application.h"

#include <QMessageBox>
#include <QPointer>
#include <QFileDialog>
#include <QDir>
#include <QDesktopServices>

#include "mainwindow.h"
#include "defaultmenu.h"

#ifdef Q_OS_MAC

#include <objc/objc.h>
#include <objc/message.h>

bool dockClickHandler(id self,SEL _cmd,...)
{
    Q_UNUSED(self)
    Q_UNUSED(_cmd)
    ((Application*)qApp)->onDockClick();
    return true;
}

#endif

Application::Application(int &argc, char **argv) :
    QApplication(argc, argv), _windows(), _defaultMenu(new DefaultMenu())
{
    setApplicationName("gtester");
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    setApplicationDisplayName("GTester");
#endif
    setApplicationVersion(GTESTER_VERSION);
    setOrganizationDomain("iwstudio.hu");
    setOrganizationName("IWStudio");

#ifdef Q_OS_MAC
    setQuitOnLastWindowClosed(false);

    objc_object* cls = objc_getClass("NSApplication");
    SEL sharedApplication = sel_registerName("sharedApplication");
    objc_object* appInst = objc_msgSend(cls, sharedApplication);

    if(appInst != NULL)
    {
        objc_object* delegate = objc_msgSend(appInst, sel_registerName("delegate"));
        objc_object* delClass = objc_msgSend(delegate, sel_registerName("class"));
        const char* tst = class_getName(delClass->isa);
        bool test = class_addMethod((objc_class*)delClass,
            sel_registerName("applicationShouldHandleReopen:hasVisibleWindows:"), (IMP)dockClickHandler, "B@:");

        if (!test)
        {
            // failed to register handler...
        }
    }

#endif
}

Application::~Application()
{
}

int Application::exec()
{
#ifdef Q_OS_LINUX
    setWindowIcon(QIcon(":/icons/gtester128.png"));
#endif

    ((Application*)qApp)->openNewWindow();

    return QApplication::exec();
}

#ifdef Q_OS_MAC
void Application::onDockClick()
{
    if (_windows.empty()) {
        openNewWindow();
    }
}
#endif

void Application::openNewWindow(const QString &fileName)
{
    QPoint windowPos;
    bool useWindowPos = false;

    if (activeWindow()) {
        windowPos = activeWindow()->pos();
        useWindowPos = true;
    }

    MainWindow *w = new MainWindow();
    _windows.push_back(w);

    connect(w, SIGNAL(Closed(MainWindow*)), this, SLOT(onWindowClosed(MainWindow*)));
    if (useWindowPos) {
        w->setGeometry(windowPos.x() + 20, windowPos.y() + 20, w->width(), w->height());
    }

    w->show();
    if (! fileName.isEmpty()) {
        w->OpenFile(fileName);
    }
}

void Application::selectOpenFile()
{
    QString newFile = QFileDialog::getOpenFileName(0, tr("Open GTester file"), QDir::homePath(), tr("GTester files (*.gtester);;All files (*)"));

    if (newFile.isEmpty())
        return;

    MainWindow *window = dynamic_cast<MainWindow*>(activeWindow());
    if (window != 0 && window->isClean()) {
        window->OpenFile(newFile);
    } else {
        openNewWindow(newFile);
    }
}

void Application::onWindowClosed(MainWindow *window)
{
    _windows.removeAt(_windows.indexOf(window));
    delete window;
}

void Application::ShowAboutBox()
{
    QString aboutText = tr(
            "<h3>About GTester</h3>"
            "<p>Version: %1</p>"
            "<p>GTester is a simple gui to run google test executables.</p>"
            "<p>Copyright (c) 2014 Tamás Eisenberger<br/>"
            "Released under <a href=\"http://www.gnu.org/licenses/gpl-3.0.html\">GPLv3</a>.</p>"
            "<p>The source is available at: <a href=\"https://github.com/u-foka/gtester\">https://github.com/u-foka/gtester</a><br/>"
            "Please report bugs using the issue tracker on github: <a href=\"https://github.com/u-foka/gtester/issues\">https://github.com/u-foka/gtester/issues</a></p>"
        ).arg(QString(GTESTER_VERSION).replace('_', ' '));

    QPointer<QMessageBox> msg = new QMessageBox();
    msg->setIconPixmap(QPixmap(":/icons/gtester128.png"));
    msg->setText(aboutText);
    msg->addButton(QMessageBox::Close);
    msg->exec();
}

void Application::CloseAndQuit()
{
    closeAllWindows();
    if (_windows.count() == 0) {
        quit();
    }
}

void Application::ReportABug()
{
    QDesktopServices::openUrl(QUrl("https://github.com/u-foka/gtester/issues"));
}
