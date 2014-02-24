#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QCloseEvent>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QPointer>

#include "testitemexecutable.h"
#include "testitem.h"
#include "fileformatv10.h"

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
    _actualFile()
{
    _ui->setupUi(this);
    updateTitle();
#ifdef Q_OS_LINUX
    setWindowIcon(QIcon(":/icons/gtester128.png"));
#endif

    _ui->testsTree->setModel(&_model);
    _ui->testsTree->setSelectionMode(QTreeView::SingleSelection);
    _ui->testsTree->setSelectionBehavior(QTreeView::SelectRows);
    _ui->testsTree->header()->resizeSection(0, 250);
    _ui->testsTree->header()->setStretchLastSection(false);
    _ui->testsTree->header()->setSectionResizeMode(0, QHeaderView::Stretch);

    // Setup splitter
    _ui->splitter->setStretchFactor(0, 1);
    _ui->splitter->setStretchFactor(1, 0);

    connect(_ui->testsTree->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this, SLOT(testsTree_selectionChanged(QItemSelection,QItemSelection)));

    connect(_ui->actionRun_selected, SIGNAL(triggered()), &_model, SLOT(execute()));
    connect(_ui->actionTerminate, SIGNAL(triggered()), &_model, SLOT(terminate()));
    connect(_ui->actionShuffle_tests, SIGNAL(toggled(bool)), &_model, SLOT(setShuffle(bool)));
    connect(_ui->actionSelect_All, SIGNAL(triggered()), &_model, SLOT(selectAll()));
    connect(_ui->actionDeselect_All, SIGNAL(triggered()), &_model, SLOT(deselectAll()));
    connect(_ui->actionInvert_selection, SIGNAL(triggered()), &_model, SLOT(invertSelection()));

    _model.setShuffle(_ui->actionShuffle_tests->isChecked());

    connect(&_model, SIGNAL(executionStateChanged(bool)), this, SLOT(model_executionStateChanged(bool)));
    connect(&_model, SIGNAL(progressUpdated(int)), this, SLOT(model_progressUpdated(int)));
    connect(&_model, SIGNAL(testStarted(QString)), this, SLOT(model_testStarted(QString)));

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

void MainWindow::model_executionStateChanged(bool running)
{
    bool hasSelection = _ui->testsTree->selectionModel()->selectedIndexes().count() > 0;

    _ui->actionNew->setEnabled(!running);
    _ui->actionOpen->setEnabled(!running);
    _ui->actionSave->setEnabled(!running);
    _ui->actionSave_As->setEnabled(!running);
    _ui->actionQuit->setEnabled(!running);

    _ui->actionAdd_test_executable->setEnabled(!running);
    _ui->actionRemove_test_executable->setEnabled(!running && hasSelection);
    _ui->actionRun_selected->setEnabled(!running);
    _ui->actionRefresh_tests->setEnabled(!running && hasSelection);
    _ui->actionTerminate->setEnabled(running);

    if (!running) {
        _ui->statusBar->clearMessage();
        if (hasSelection)
            updateOutput(static_cast<TestItemBase*>(
                             _ui->testsTree->selectionModel()->selectedIndexes().first().internalPointer()
                         ));
    } else {
        _ui->outputEdit->setText(QString());
    }
}

void MainWindow::closeEvent(QCloseEvent *event) {
    if (_model.isRunning()) {
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
    if (_model.isRunning())
        return;
}

void MainWindow::on_actionAdd_test_executable_triggered()
{
    if (_model.isRunning())
        return;

    QString fileName = QFileDialog::getOpenFileName(this, "Open test executable");
    if (fileName.isEmpty())
        return;

    _model.addExecutable(fileName);
}

void MainWindow::on_actionRefresh_tests_triggered()
{
    if (_model.isRunning())
        return;

    QModelIndexList sel = _ui->testsTree->selectionModel()->selectedIndexes();
    if (sel.count() > 0) {
        _model.refresh(sel.first());
    }
}

void MainWindow::on_actionRemove_test_executable_triggered()
{
    if (_model.isRunning())
        return;

    QModelIndexList sel = _ui->testsTree->selectionModel()->selectedIndexes();
    if (sel.count() > 0) {
        _model.removeExecutable(sel.first());
    }
}

void MainWindow::testsTree_selectionChanged(QItemSelection newSel, QItemSelection /*oldSel*/)
{
    bool enabled = newSel.count() > 0 && !_model.isRunning();

    _ui->actionRemove_test_executable->setEnabled(enabled);
    _ui->actionRefresh_tests->setEnabled(enabled);

    if (newSel.count() > 0) {
        updateOutput(static_cast<TestItemBase*>(newSel.first().indexes().first().internalPointer()));
    }
}

void MainWindow::model_progressUpdated(int percent)
{
    _ui->progressBar->setValue(percent);
}

void MainWindow::model_testStarted(const QString &status)
{
    _ui->statusBar->showMessage("Running test: " + status);
}

void MainWindow::updateOutput(TestItemBase *base)
{
    TestItem *item = dynamic_cast<TestItem*>(base);

    if (item == 0)
        _ui->outputEdit->setText(QString());
    else
        _ui->outputEdit->setText(item->getOutput());
}

void MainWindow::on_actionSave_triggered()
{
    if (_model.isRunning())
        return;

    if (_actualFile.isEmpty()) {
        on_actionSave_As_triggered();
        return;
    }

    QFile file(_actualFile);
    file.open(QIODevice::WriteOnly | QIODevice::Truncate);

    FileFormatV10 to(file);

    _model.save(&to);

    file.close();
}

void MainWindow::on_actionSave_As_triggered()
{
    if (_model.isRunning())
        return;

    QString newFile = QFileDialog::getSaveFileName(this);

    if (! newFile.isEmpty()) {
        _actualFile = newFile;
        updateTitle();
        on_actionSave_triggered();
    }
}

void MainWindow::on_actionOpen_triggered()
{
    if (_model.isRunning())
        return;

    QString newFile = QFileDialog::getOpenFileName(this, "Open GTester file");

    if (newFile.isEmpty())
        return;

    _actualFile = newFile;
    updateTitle();

    QFile file(_actualFile);
    file.open(QIODevice::ReadOnly);

    FileFormatV10 from(file);

    _model.read(&from);

    file.close();

    _ui->actionShuffle_tests->setChecked(_model.getShuffle());
}

void MainWindow::on_actionNew_triggered()
{
    if (_model.isRunning())
        return;

    _actualFile.clear();
    updateTitle();
    _model.clear();

    _model.setShuffle(true);
    _ui->actionShuffle_tests->setChecked(true);
}

void MainWindow::updateTitle()
{
    if (_actualFile.isEmpty()) {
        setWindowTitle(tr("New file"));
        return;
    }

    setWindowTitle(QFileInfo(_actualFile).fileName());
}

void MainWindow::on_actionAbout_Qt_triggered()
{
    QMessageBox::aboutQt(this);
}

void MainWindow::on_actionAbout_GTester_triggered()
{
    QString aboutText = tr(
            "<h3>About GTester</h3>"
            "<p>Version: %1</p>"
            "<p>GTester is a simple gui to run google test executables.</p>"
            "<p>Copyright (c) 2014 Tamás Eisenberger<br/>"
            "Released under <a href=\"http://www.gnu.org/licenses/gpl-3.0.html\">GPLv3</a>.</p>"
            "<p>The source is available at: <a href=\"https://github.com/u-foka/gtester\">https://github.com/u-foka/gtester</a><br/>"
            "Please report bugs using the issue tracker on github: <a href=\"https://github.com/u-foka/gtester/issues\">https://github.com/u-foka/gtester/issues</a></p>"
        ).arg(QString(GTESTER_VERSION).replace('_', ' '));

    QPointer<QMessageBox> msg = new QMessageBox(this);
    msg->setIconPixmap(QPixmap(":/icons/gtester128.png"));
    msg->setText(aboutText);
    msg->addButton(QMessageBox::Close);
    msg->exec();
}
