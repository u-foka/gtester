#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QCloseEvent>
#include <QFileDialog>

#include "executabletester.h"

#define SETTINGS_VERSION "Version"
#define SETTINGS_WINDOW_GEOMETRY "MainWindow/Geometry"
#define SETTINGS_WINDOW_STATE "MainWindow/State"
#define SETTINGS_SPLITTER_STATE "MainWindow/Splitter/State"
#define SETTINGS_TREEHEADER_STATE "MainWindow/TestsTree/Header/State"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    _ui(new Ui::MainWindow),
    _settings()
{
    _ui->setupUi(this);
    _ui->testsTree->setModel(&_model);
    _ui->testsTree->header()->resizeSection(0, 250);

    // Setup splitter
    _ui->splitter->setStretchFactor(0, 1);
    _ui->splitter->setStretchFactor(1, 0);

    // Restore geometry
    restoreGeometry(_settings.value(SETTINGS_WINDOW_GEOMETRY).toByteArray());
    restoreState(_settings.value(SETTINGS_WINDOW_STATE).toByteArray());

    _ui->splitter->restoreState(_settings.value(SETTINGS_SPLITTER_STATE).toByteArray());

    _ui->testsTree->header()->restoreState(_settings.value(SETTINGS_TREEHEADER_STATE).toByteArray());
}

MainWindow::~MainWindow()
{
    delete _ui;
}

void MainWindow::closeEvent(QCloseEvent *event) {
    if (_running) {
        event->ignore();
        return;
    }

    // Save geometry
    _settings.setValue(SETTINGS_WINDOW_GEOMETRY, saveGeometry());
    _settings.setValue(SETTINGS_WINDOW_STATE, saveState());

    _settings.setValue(SETTINGS_SPLITTER_STATE, _ui->splitter->saveState());

    _settings.setValue(SETTINGS_TREEHEADER_STATE, _ui->testsTree->header()->saveState());
}

void MainWindow::on_actionRun_selected_triggered()
{

}

void MainWindow::on_actionAdd_test_executable_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open test executable");
    if (fileName.isEmpty())
        return;

    QFileInfo fileInfo(fileName);

    ExecutableTester *tester = new ExecutableTester(
                new TestItemBase(fileInfo.fileName(), true, _model.rootItem()),
                fileInfo, this);
    tester->execute();


}
