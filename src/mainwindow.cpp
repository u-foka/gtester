#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QCloseEvent>
#include <QFileDialog>

#include "testitemexecutable.h"

#define SETTINGS_VERSION "Version"
#define SETTINGS_WINDOW_GEOMETRY "MainWindow/Geometry"
#define SETTINGS_WINDOW_STATE "MainWindow/State"
#define SETTINGS_SPLITTER_STATE "MainWindow/Splitter/State"
#define SETTINGS_TREEHEADER_STATE "MainWindow/TestsTree/Header/State"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    _ui(new Ui::MainWindow),
    _settings(),
    _model(), // Don't give it the window as parent since the destructor will destroy it anyway
    _running(false)
{
    _ui->setupUi(this);
    _ui->testsTree->setModel(&_model);
    _ui->testsTree->setSelectionMode(QTreeView::SingleSelection);
    _ui->testsTree->setSelectionBehavior(QTreeView::SelectRows);
    _ui->testsTree->header()->resizeSection(0, 250);

    // Setup splitter
    _ui->splitter->setStretchFactor(0, 1);
    _ui->splitter->setStretchFactor(1, 0);

    connect(_ui->testsTree->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this, SLOT(on_testsTree_selectionChanged(QItemSelection,QItemSelection)));

    connect(&_model, SIGNAL(executionStateChanged(bool)), this, SLOT(setRunning(bool)));
    connect(_ui->actionRun_selected, SIGNAL(triggered()), &_model, SLOT(execute()));
    connect(_ui->actionTerminate, SIGNAL(triggered()), &_model, SLOT(terminate()));
    connect(_ui->actionShuffle_tests, SIGNAL(toggled(bool)), &_model, SLOT(setShuffle(bool)));

    _model.setShuffle(_ui->actionShuffle_tests->isChecked());

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

bool MainWindow::getRunning() const
{
    return _running;
}

void MainWindow::setRunning(bool running)
{
    _running = running;
    bool hasSelection = _ui->testsTree->selectionModel()->selectedIndexes().count() > 0;

    _ui->actionNew->setEnabled(!_running);
    _ui->actionOpen->setEnabled(!_running);
    _ui->actionSave->setEnabled(!_running);
    _ui->actionSave_As->setEnabled(!_running);
    _ui->actionQuit->setEnabled(!_running);

    _ui->actionAdd_test_executable->setEnabled(!_running);
    _ui->actionRemove_test_executable->setEnabled(!_running && hasSelection);
    _ui->actionRun_selected->setEnabled(!_running);
    _ui->actionRefresh_tests->setEnabled(!_running && hasSelection);
    _ui->actionTerminate->setEnabled(_running);
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
    if (_running)
        return;
}

void MainWindow::on_actionAdd_test_executable_triggered()
{
    if (_running)
        return;

    QString fileName = QFileDialog::getOpenFileName(this, "Open test executable");
    if (fileName.isEmpty())
        return;

    _model.addExecutable(fileName);
}

void MainWindow::on_actionRefresh_tests_triggered()
{
    if (_running)
        return;

    QModelIndexList sel = _ui->testsTree->selectionModel()->selectedIndexes();
    if (sel.count() > 0) {
        _model.refresh(sel.first());
    }
}

void MainWindow::on_actionRemove_test_executable_triggered()
{
    if (_running)
        return;

    QModelIndexList sel = _ui->testsTree->selectionModel()->selectedIndexes();
    if (sel.count() > 0) {
        _model.removeExecutable(sel.first());
    }
}

void MainWindow::on_testsTree_selectionChanged(QItemSelection newSel, QItemSelection /*oldSel*/)
{
    bool enabled = newSel.count() > 0 && !_running;

    _ui->actionRemove_test_executable->setEnabled(enabled);
    _ui->actionRefresh_tests->setEnabled(enabled);
}
