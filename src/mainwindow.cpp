#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <stdexcept>

#include <QCloseEvent>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QPointer>

#include "application.h"
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

#ifdef Q_OS_MAC
    _ui->mainToolBar->setMovable(false);
    _ui->mainToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    _ui->statusBar->setSizeGripEnabled(false);
#endif

    _ui->testsTree->setModel(&_model);
    _ui->testsTree->setSelectionMode(QTreeView::SingleSelection);
    _ui->testsTree->setSelectionBehavior(QTreeView::SelectRows);
    _ui->testsTree->header()->resizeSection(0, 250);
    _ui->testsTree->header()->setStretchLastSection(false);
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    _ui->testsTree->header()->setSectionResizeMode(0, QHeaderView::Stretch);
#else
    _ui->testsTree->header()->setResizeMode(0, QHeaderView::Stretch);
#endif

    // Setup splitter
    _ui->splitter->setStretchFactor(0, 1);
    _ui->splitter->setStretchFactor(1, 0);

    // Connect Widget & Action signals
    connect(_ui->testsTree->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this, SLOT(testsTree_selectionChanged(QItemSelection,QItemSelection)));

    connect(_ui->actionQuit, SIGNAL(triggered()), AppInstance, SLOT(CloseAndQuit()));
    connect(_ui->actionReport_a_bug, SIGNAL(triggered()), AppInstance, SLOT(ReportABug()));

    connect(_ui->actionRun_selected, SIGNAL(triggered()), &_model, SLOT(execute()));
    connect(_ui->actionTerminate, SIGNAL(triggered()), &_model, SLOT(terminate()));
    connect(_ui->actionShuffle_tests, SIGNAL(toggled(bool)), &_model, SLOT(setShuffle(bool)));
    connect(_ui->actionSelect_All, SIGNAL(triggered()), &_model, SLOT(selectAll()));
    connect(_ui->actionDeselect_All, SIGNAL(triggered()), &_model, SLOT(deselectAll()));
    connect(_ui->actionInvert_selection, SIGNAL(triggered()), &_model, SLOT(invertSelection()));

    // Connect Model signals
    connect(&_model, SIGNAL(changed()), this, SLOT(updateTitle()));
    connect(&_model, SIGNAL(executionStateChanged(bool)), this, SLOT(model_executionStateChanged(bool)));
    connect(&_model, SIGNAL(progressUpdated(int)), this, SLOT(model_progressUpdated(int)));
    connect(&_model, SIGNAL(testStarted(QString)), this, SLOT(model_testStarted(QString)));

    // Restore geometry
    restoreGeometry(_settings.value(SETTINGS_WINDOW_GEOMETRY).toByteArray());
    restoreState(_settings.value(SETTINGS_WINDOW_STATE).toByteArray());

    _ui->splitter->restoreState(_settings.value(SETTINGS_SPLITTER_STATE).toByteArray());

    _ui->testsTree->header()->restoreState(_settings.value(SETTINGS_TREEHEADER_STATE).toByteArray());

    Clean();
}

MainWindow::~MainWindow()
{
    delete _ui;
}

void MainWindow::Clean()
{
    if (_model.isRunning())
        return;

    _actualFile.clear();
    _model.clear();
    updateTitle();
}

void MainWindow::OpenFile(const QString &fileName)
{
    _actualFile = fileName;
    updateTitle();

    try {
        QFile file(_actualFile);
        file.open(QIODevice::ReadOnly);

        FileFormatV10 from(file);

        _model.read(&from);

        file.close();
    } catch (std::runtime_error &e) {
        Clean();

        QMessageBox::critical(this, tr("Failed to open file"),
                              tr("Failed to open file.\nTechnical details: %1").arg(e.what()),
                              QMessageBox::Ok);
    }
}

bool MainWindow::isClean()
{
    return _model.isClean();
}

bool MainWindow::isChanged()
{
    return _model.isChanged();
}

void MainWindow::model_executionStateChanged(bool running)
{
    bool hasSelection = _ui->testsTree->selectionModel()->selectedIndexes().count() > 0;

    _ui->actionNew->setEnabled(!running);
    _ui->actionOpen->setEnabled(!running);
    _ui->actionSave->setEnabled(!running);
    _ui->actionSave_As->setEnabled(!running);

    _ui->actionAdd_test_executable->setEnabled(!running);
    _ui->actionRemove_test_executable->setEnabled(!running && hasSelection);
    _ui->actionRun_selected->setEnabled(!running);
    _ui->actionRefresh_tests->setEnabled(!running && hasSelection);
    _ui->actionTerminate->setEnabled(running);

    if (!running) {
        _ui->progressBar->setValue(0);
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

    if (! _model.isClean() && _model.isChanged()) {
        QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Question);
            msgBox.setText(tr("The document has been modified."));
            msgBox.setInformativeText(tr("Do you want to save your changes?"));
            msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
            msgBox.setDefaultButton(QMessageBox::Save);

            switch (msgBox.exec()) {
            case QMessageBox::Save:
                on_actionSave_triggered();
                if (_model.isChanged()) {
                    event->ignore();
                    return;
                }

                break;

            case QMessageBox::Discard:
                // Do nothing
                break;

            case QMessageBox::Cancel:
                event->ignore();
                return;

            default:
                throw std::runtime_error("Invalid ansver");
            }
    }

    // Save geometry
    _settings.setValue(SETTINGS_WINDOW_GEOMETRY, saveGeometry());
    _settings.setValue(SETTINGS_WINDOW_STATE, saveState());

    _settings.setValue(SETTINGS_SPLITTER_STATE, _ui->splitter->saveState());

    _settings.setValue(SETTINGS_TREEHEADER_STATE, _ui->testsTree->header()->saveState());

    // Notify application
    emit Closed(this);

    QMainWindow::closeEvent(event);
}

void MainWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::ActivationChange && isActiveWindow()) {
        emit Focused(this);
    }

    QMainWindow::changeEvent(event);
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

#ifdef Q_OS_WIN
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open test executable"), QDir::homePath(), tr("Executable files (*.exe);;All files (*)"));
#else
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open test executable"), QDir::homePath(), tr("All files (*)"));
#endif
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

    QFileInfo info(_actualFile);
    if (info.exists()) {
        QFile::remove(_actualFile);
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

    QString newFile = QFileDialog::getSaveFileName(this, tr("Save GTester file"), QDir::homePath(), tr("GTester Files (*.gtester);;All Files (*)"));

    if (! newFile.isEmpty()) {
        // Append extension if necessary
        if (! newFile.endsWith(".gtester")) {
            newFile.append(".gtester");
        }

        _actualFile = newFile;
        on_actionSave_triggered();
        updateTitle();
    }
}

void MainWindow::on_actionOpen_triggered()
{
    AppInstance->SelectOpenFile();
}

void MainWindow::on_actionNew_triggered()
{
    AppInstance->OpenNewWindow();
}

void MainWindow::updateTitle()
{
    setWindowFilePath(_actualFile);

    QString title;
    if (_actualFile.isEmpty()) {
        title = tr("New file");
    } else {
        title = QFileInfo(_actualFile).fileName();
    }
    title.append("[*]");
    setWindowTitle(title);

    setWindowModified(_model.isChanged());
}

void MainWindow::on_actionAbout_Qt_triggered()
{
    QMessageBox::aboutQt(this);
}

void MainWindow::on_actionAbout_GTester_triggered()
{
    AppInstance->ShowAboutBox();
}
