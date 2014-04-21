#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>
#include <QVector>

#define AppInstance (static_cast< ::Application *>(::QCoreApplication::instance()))


class MainWindow;
class DefaultMenu;

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
    void WindowClosed(MainWindow *window);
    void ShowAboutBox();

    void CloseAndQuit();

    void ReportABug();

private:
    QVector<MainWindow*> _windows;
    DefaultMenu *_defaultMenu;

};

#endif // APPLICATION_H
