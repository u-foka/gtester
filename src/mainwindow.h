#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QItemSelection>

#include "testmodel.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    virtual ~MainWindow();

    void Clean();
    void OpenFile(const QString &fileName);

    bool isClean();
    bool isChanged();

signals:
    void Closed(MainWindow *window);
    void Focused(MainWindow *window);
    void SetErrorCount(MainWindow *window, size_t errors);
    void SetProgress(MainWindow *window, int percent);
    void ClearProgress(MainWindow *window);

private slots:
    void on_actionOpen_triggered();
    void on_actionSave_triggered();
    void on_actionSave_As_triggered();

    void on_actionRun_selected_triggered();
    void on_actionAdd_test_executable_triggered();
    void on_actionRefresh_tests_triggered();
    void on_actionRemove_test_executable_triggered();

    void testsTree_selectionChanged(QItemSelection newSel, QItemSelection oldSel);

    void model_executionStateChanged(bool getRunning);
    void model_progressUpdated(int percent);
    void model_testStarted(const QString &status);

    void updateOutput(TestItemBase *base);

    void on_actionNew_triggered();

    void on_actionAbout_Qt_triggered();
    void on_actionAbout_GTester_triggered();

    void updateTitle();

protected:
    void closeEvent(QCloseEvent *event);
    void changeEvent(QEvent *event);

private:
    Ui::MainWindow *_ui;
    QSettings _settings;
    TestModel _model;

    QString _actualFile;

};

#endif // MAINWINDOW_H
