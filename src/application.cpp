#include "application.h"

#include <QMessageBox>
#include <QPointer>
#include <QFileDialog>
#include <QDir>
#include <QDesktopServices>
#include <QLocalSocket>
#include <QFileOpenEvent>

#ifdef Q_OS_MAC
#include <objc/objc.h>
#include <objc/message.h>
#endif

#include "mainwindow.h"
#include "defaultmenu.h"
#include "setfocus.h"
#include "ipcmessage.h"

const QString InstanceSocketName = "hu.iwstudio.gtester.instance.socket";
const QString IpcFileNameToken = "FileName";

#ifdef Q_OS_MAC
bool dockClickHandler(id self,SEL _cmd,...)
{
    Q_UNUSED(self)
    Q_UNUSED(_cmd)
    AppInstance->DockClick();
    return true;
}
#endif

Application::Application(int &argc, char **argv) :
    QApplication(argc, argv), _windows(), _lastActiveWindow(0), _defaultMenu(new DefaultMenu()),
    _instanceSocket()
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

    objc_object* cls = (objc_object*) objc_getClass("NSApplication");
    SEL sharedApplication = sel_registerName("sharedApplication");
    objc_object* appInst = objc_msgSend(cls, sharedApplication);

    if(appInst != NULL)
    {
        objc_object* delegate = objc_msgSend(appInst, sel_registerName("delegate"));
        objc_object* delClass = objc_msgSend(delegate, sel_registerName("class"));
        bool res = class_addMethod(reinterpret_cast<Class>(delClass),
            sel_registerName("applicationShouldHandleReopen:hasVisibleWindows:"), (IMP)dockClickHandler, "B@:");

        if (!res)
        {
            qDebug() << "failed to register dockClickHandler...";
        }
    }

#endif

    connect(&_instanceSocket, SIGNAL(newConnection()), this, SLOT(InstanceSocketConnection()));
}

Application::~Application()
{
    delete _defaultMenu;
}

bool Application::checkInstance(int argc, char *argv[])
{
    QLocalSocket instanceClient;
    instanceClient.connectToServer(InstanceSocketName);
    if (instanceClient.waitForConnected(500)) {
        IpcMessage msg;
        if (argc <= 1) {
            msg.Type(IpcMessage::Activate);
        } else {
            msg.Type(IpcMessage::OpenDocument);
            msg.Values().insert(IpcFileNameToken, argv[1]);
        }

        QByteArray data;
        msg.Serialize(data);
        instanceClient.write(data);
        if (! instanceClient.waitForBytesWritten()) {
            qDebug() << "Failed to write message to the other instance...";
        }
        instanceClient.disconnectFromServer();

        return true;
    }

    return false;
}

int Application::exec()
{
    QLocalServer::removeServer(InstanceSocketName);
    AppInstance->_instanceSocket.listen(InstanceSocketName);

#ifdef Q_OS_LINUX
    setWindowIcon(QIcon(":/icons/gtester128.png"));
#endif

    if (arguments().count() <= 1) {
        AppInstance->OpenNewWindow();
    } else {
        AppInstance->OpenNewWindow(arguments().at(1));
    }

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

    if (_lastActiveWindow) {
        windowPos = _lastActiveWindow->pos();
        useWindowPos = true;
    }

    MainWindow *w = new MainWindow();
    _windows.push_back(w);

    connect(w, SIGNAL(Closed(MainWindow*)), this, SLOT(WindowClosed(MainWindow*)));
    connect(w, SIGNAL(Focused(MainWindow*)), this, SLOT(WindowFocused(MainWindow*)));
    connect(w, SIGNAL(SetErrorCount(MainWindow*,size_t)), &_globalProgress, SLOT(SetErrorCount(MainWindow*,size_t)));
    connect(w, SIGNAL(SetProgress(MainWindow*,int)), &_globalProgress, SLOT(SetProgress(MainWindow*,int)));
    connect(w, SIGNAL(ClearProgress(MainWindow*)), &_globalProgress, SLOT(ClearProgress(MainWindow*)));
    connect(w, SIGNAL(Closed(MainWindow*)), &_globalProgress, SLOT(RemoveWindow(MainWindow*)));

    if (useWindowPos) {
        w->setGeometry(windowPos.x() + 20, windowPos.y() + 40, w->width(), w->height());
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

    OpenFile(newFile);
}

void Application::OpenFile(const QString &file)
{
    if (_lastActiveWindow != 0 && _lastActiveWindow->isClean()) {
        _lastActiveWindow->OpenFile(file);
        SetFocus::To(_lastActiveWindow);
    } else {
        OpenNewWindow(file);
    }
}

void Application::WindowClosed(MainWindow *window)
{
    _windows.removeAt(_windows.indexOf(window));
    if (_lastActiveWindow == window) {
        _lastActiveWindow = 0;
    }
    window->deleteLater();
}

void Application::WindowFocused(MainWindow *window)
{
    _lastActiveWindow = window;
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
    QPointer<QLocalSocket> socket(_instanceSocket.nextPendingConnection());
    socket->waitForReadyRead();

    QByteArray data = socket->readAll();
    IpcMessage msg;
    msg.Parse(data);

    switch (msg.Type()) {
    case IpcMessage::OpenDocument: {
        IpcMessage::ValueList::Iterator i = msg.Values().find(IpcFileNameToken);
        if (i != msg.Values().end()) {
            OpenNewWindow(i.value());
        }
        break;
    }
    case IpcMessage::Activate:
        if (_lastActiveWindow != 0) {
            SetFocus::To(_lastActiveWindow);
        } else if (_windows.count() > 0) {
            SetFocus::To(_windows.last());
        } else {
            OpenNewWindow();
        }
        break;

    }

}

bool Application::event(QEvent *event)
{
    switch (event->type()) {
    case QEvent::FileOpen:
        OpenFile(static_cast<QFileOpenEvent*>(event)->file());
        return true;

    default:
        return QApplication::event(event);

    }
}
