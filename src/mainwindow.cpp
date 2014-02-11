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
    _settings()
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

    _model.addExecutable(fileName);
}

void MainWindow::on_actionRefresh_tests_triggered()
{
    QModelIndexList sel = _ui->testsTree->selectionModel()->selectedIndexes();
    if (sel.count() > 0) {
        _model.refresh(sel.first());
    }
}

void MainWindow::on_actionRemove_test_executable_triggered()
{
    QModelIndexList sel = _ui->testsTree->selectionModel()->selectedIndexes();
    if (sel.count() > 0) {
        _model.removeExecutable(sel.first());
    }
}

void MainWindow::on_testsTree_selectionChanged(QItemSelection newSel, QItemSelection /*oldSel*/)
{
    bool hasSel = newSel.count();

    _ui->actionRemove_test_executable->setEnabled(hasSel);
    _ui->actionRefresh_tests->setEnabled(hasSel);
}
