#include "globalprogress.h"

GlobalProgress::GlobalProgress(QObject *parent)
    : QObject(parent), _windows(), sumErrorCount(0), sumProgress(0), numProgress(0)
{
}

GlobalProgress::~GlobalProgress()
{
}

void GlobalProgress::SetErrorCount(MainWindow *window, size_t errors)
{
    WindowProgress &p = _windows[window];

    sumErrorCount += errors - p.ErrorCount;
    p.ErrorCount = errors;

    SetGlobalErrorCount();
}

void GlobalProgress::SetProgress(MainWindow *window, int progress)
{
    WindowProgress &p = _windows[window];

    if (p.Progress == -1) {
        ++numProgress;
        p.Progress = 0;
    }

    sumProgress += progress - p.Progress;
    p.Progress = progress;

    SetGlobalProgress();
}

void GlobalProgress::ClearProgress(MainWindow *window)
{
    WindowProgress &p = _windows[window];
    if (p.Progress >= 0) {
        --numProgress;
        sumProgress -= p.Progress;
        p.Progress = -1;

        SetGlobalProgress();
    }
}

void GlobalProgress::RemoveWindow(MainWindow *window)
{
    ClearProgress(window);

    WindowProgress &p = _windows[window];
    sumErrorCount -= p.ErrorCount;
    SetGlobalErrorCount();

    _windows.remove(window);
}

void GlobalProgress::SetGlobalErrorCount()
{
    if (sumErrorCount == 0) {
        ErrorLabel("");
    } else {
        ErrorLabel(QString::number(sumErrorCount));
    }
}

void GlobalProgress::SetGlobalProgress()
{
    if (numProgress < 1) {
        ShowProgress(false);
    } else {
        Progress(sumProgress, numProgress * 100);
        ShowProgress(true);
    }
}
