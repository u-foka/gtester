#ifndef PROGRESSMANAGER_H
#define PROGRESSMANAGER_H

#include <QObject>
#include <QString>
#include <QMap>

class MainWindow;

class GlobalProgress : public QObject
{
    Q_OBJECT

public:
    GlobalProgress(QObject *parent = 0);
    virtual ~GlobalProgress();

public slots:
    void SetErrorCount(MainWindow *window, size_t errors);
    void SetProgress(MainWindow *window, int progress);
    void ClearProgress(MainWindow *window);
    void RemoveWindow(MainWindow *window);

private:
    struct WindowProgress {
        WindowProgress() : ErrorCount(0), Progress(-1) {}

        size_t ErrorCount;
        int Progress;
    };
    typedef QMap<MainWindow*, WindowProgress> WindowList;

    WindowList _windows;
    size_t sumErrorCount;
    int sumProgress;
    int numProgress;

    void SetGlobalErrorCount();
    void SetGlobalProgress();

    // Platform dependant
    void ErrorLabel(const QString &errors);
    void Progress(int progress, int max);
    void ShowProgress(bool show);

};

#endif // PROGRESSMANAGER_H
