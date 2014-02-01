#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QCloseEvent>

#define SETTINGS_WINDOW_GEOMETRY "MainWindow/Geometry"
#define SETTINGS_WINDOW_STATE "MainWindow/State"
#define SETTINGS_SPLITTER_GEOMETRY "MainWindow/Splitter/Geometry"
#define SETTINGS_SPLITTER_STATE "MainWindow/Splitter/State"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    settings(),
    executeThread(0)
{
    ui->setupUi(this);

    // Setup splitter
    ui->splitter->setStretchFactor(0, 1);
    ui->splitter->setStretchFactor(1, 0);

    // Restore geometry
    restoreGeometry(settings.value(SETTINGS_WINDOW_GEOMETRY).toByteArray());
    restoreState(settings.value(SETTINGS_WINDOW_STATE).toByteArray());

    ui->splitter->restoreGeometry(settings.value(SETTINGS_SPLITTER_GEOMETRY).toByteArray());
    ui->splitter->restoreState(settings.value(SETTINGS_SPLITTER_STATE).toByteArray());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event) {
    if (executeThread != 0) {
        event->ignore();
        return;
    }

    // Save geometry
    settings.setValue(SETTINGS_WINDOW_GEOMETRY, saveGeometry());
    settings.setValue(SETTINGS_WINDOW_STATE, saveState());

    settings.setValue(SETTINGS_SPLITTER_GEOMETRY, ui->splitter->saveGeometry());
    settings.setValue(SETTINGS_SPLITTER_STATE, ui->splitter->saveState());
}

void MainWindow::on_actionRun_selected_triggered()
{
    executeThread = (ExecuteThread*)1;
}
