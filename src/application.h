#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>
#include <QVector>
#include <QLocalServer>
#include <QPointer>

#ifdef Q_OS_MAC
#include "cocoainitializer.h"
#endif

#define AppInstance (static_cast< ::Application *>(::QCoreApplication::instance()))

class MainWindow;
class DefaultMenu;
class CocoaInitializer;

class Application : public QApplication
{
    Q_OBJECT
public:
    explicit Application(int &argc, char **argv);
    ~Application();

    static int exec();

signals:

public slots:
#ifdef Q_OS_MAC
    void DockClick();
#endif
    void OpenNewWindow(const QString &fileName = QString());
    void SelectOpenFile();
    void OpenFile(const QString &file);
    void WindowClosed(MainWindow *window);
    void WindowFocused(MainWindow *window);
    void ShowAboutBox();

    void CloseAndQuit();

    void ReportABug();

    void InstanceSocketConnection();

protected:
    bool event(QEvent *event);

private:
    QVector<MainWindow*> _windows;
    MainWindow *_lastActiveWindow;
    QPointer<DefaultMenu> _defaultMenu;

    QLocalServer _instanceSocket;

#ifdef Q_OS_MAC
    CocoaInitializer _cocoaInitializer;
#endif

};

#endif // APPLICATION_H
