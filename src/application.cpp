#include "application.h"

#include <QMessageBox>
#include <QPointer>
#include <QFileDialog>
#include <QDir>
#include <QDesktopServices>
#include <QLocalSocket>

#include "mainwindow.h"
#include "defaultmenu.h"
#include "setfocus.h"

#ifdef Q_OS_MAC

#include <objc/objc.h>
#include <objc/message.h>

bool dockClickHandler(id self,SEL _cmd,...)
{
    Q_UNUSED(self)
    Q_UNUSED(_cmd)
    AppInstance->DockClick();
    return true;
}

#endif

const QString InstanceSocketName = "hu.iwstudio.gtester.instance.socket";

Application::Application(int &argc, char **argv) :
    QApplication(argc, argv), _windows(), _defaultMenu(new DefaultMenu()), _instanceSocket()
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

    connect(&_instanceSocket, SIGNAL(newConnection()), this, SLOT(InstanceSocketConnection()));
}

Application::~Application()
{
}

int Application::exec()
{
    QLocalSocket instanceClient;
    instanceClient.connectToServer(InstanceSocketName);
    if (instanceClient.waitForConnected(500)) {
        // Notify the existing instance

        return 0;
    }

    QLocalServer::removeServer(InstanceSocketName);
    AppInstance->_instanceSocket.listen(InstanceSocketName);

#ifdef Q_OS_LINUX
    setWindowIcon(QIcon(":/icons/gtester128.png"));
#endif

    AppInstance->OpenNewWindow();

    return QApplication::exec();
}

#ifdef Q_OS_MAC
void Application::DockClick()
{
    if (_windows.empty()) {
        OpenNewWindow();
    }
}
#endif

void Application::OpenNewWindow(const QString &fileName)
{
    QPoint windowPos;
    bool useWindowPos = false;

    if (activeWindow()) {
        windowPos = activeWindow()->pos();
        useWindowPos = true;
    }

    MainWindow *w = new MainWindow();
    _windows.push_back(w);

    connect(w, SIGNAL(Closed(MainWindow*)), this, SLOT(WindowClosed(MainWindow*)));
    if (useWindowPos) {
        w->setGeometry(windowPos.x() + 20, windowPos.y() + 20, w->width(), w->height());
    }

    w->show();
    SetFocus::To(w);
    if (! fileName.isEmpty()) {
        w->OpenFile(fileName);
    }
}

void Application::SelectOpenFile()
{
    QString newFile = QFileDialog::getOpenFileName(0, tr("Open GTester file"), QDir::homePath(), tr("GTester files (*.gtester);;All files (*)"));

    if (newFile.isEmpty())
        return;

    MainWindow *window = dynamic_cast<MainWindow*>(activeWindow());
    if (window != 0 && window->isClean()) {
        window->OpenFile(newFile);
    } else {
        OpenNewWindow(newFile);
    }
}

void Application::WindowClosed(MainWindow *window)
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

void Application::InstanceSocketConnection()
{
    qDebug() << "InstanceSocketConnection";

    QPointer<QLocalSocket> socket(_instanceSocket.nextPendingConnection());

    qDebug() << "activeWindow() " << activeWindow();
    if (activeWindow() != 0) {
        SetFocus::To(activeWindow());
    } else if (_windows.count() > 0) {
        // Workaround for qt bug on Mac OS
        SetFocus::To(_windows.at(0));
    } else {
        OpenNewWindow();
    }
}