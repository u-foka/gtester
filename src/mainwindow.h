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

private slots:
    void on_actionRun_selected_triggered();
    void on_actionAdd_test_executable_triggered();
    void on_actionRefresh_tests_triggered();
    void on_actionRemove_test_executable_triggered();

    void on_testsTree_selectionChanged(QItemSelection newSel, QItemSelection oldSel);

protected:
    void closeEvent(QCloseEvent *event);

private:
    Ui::MainWindow *_ui;
    QSettings _settings;
    TestModel _model;

    bool _running;

};

#endif // MAINWINDOW_H
