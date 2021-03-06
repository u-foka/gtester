#include "testmodel.h"

#include <stdexcept>

#include <QtGui>
#include <QMessageBox>

#include "testitemroot.h"
#include "testitemexecutable.h"
#include "executablebase.h"
#include "executabletester.h"
#include "executabletestrunner.h"

TestModel::TestModel(QObject *parent)
    : QAbstractItemModel(parent), _rootItem(new TestItemRoot(this)), _clean(true),
      _changed(false), _running(false), _pendingJobs(), _runningTestCount(0), _completedTestCount(0)
{
}

TestModel::~TestModel()
{
    delete _rootItem;
}

int TestModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<TestItemBase*>(parent.internalPointer())->columnCount();
    else
        return _rootItem->columnCount();
}

QVariant TestModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    TestItemBase *item = static_cast<TestItemBase*>(index.internalPointer());

    if (role == Qt::ForegroundRole)
        return QColor(Qt::black);

    if (role == Qt::BackgroundColorRole)
        return item->getBackgroundColor(index.column());

    if (role == Qt::CheckStateRole && item->hasCheckbox(index.column()))
        return item->getCheckState(index.column());

    if (role != Qt::DisplayRole)
        return QVariant();

    return item->getData(index.column());
}

bool TestModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    TestItemBase *item = static_cast<TestItemBase*>(index.internalPointer());

    if (role == Qt::CheckStateRole) {
        item->setCheckState(index.column(), value);
        QVector<int> roles;
        roles << role;
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
        emit dataChanged(index, index, roles);
#else
        emit dataChanged(index, index);
#endif
        updateParents(index, roles);
        updateChildren(index, roles);
        change();
        return true;
    }

    return false;
}

Qt::ItemFlags TestModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    TestItemBase *item = static_cast<TestItemBase*>(index.internalPointer());

    Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

    if (item->hasCheckbox(index.column()))
        flags |= Qt::ItemIsUserCheckable;

    return flags;
}

QVariant TestModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return _rootItem->getData(section);

    return QVariant();
}

QModelIndex TestModel::index(int row, int column, const QModelIndex &parent)
const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    TestItemBase *parentItem;

    if (!parent.isValid())
        parentItem = _rootItem;
    else
        parentItem = static_cast<TestItemBase*>(parent.internalPointer());

    TestItemBase *childItem = parentItem->getChild(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex TestModel::index(TestItemBase *item, int column) const
{
    if (item == _rootItem)
        return QModelIndex();

    return createIndex(item->getRow(), column, item);
}

QModelIndex TestModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    TestItemBase *childItem = static_cast<TestItemBase*>(index.internalPointer());
    TestItemBase *parentItem = childItem->getParent();

    if (parentItem == _rootItem)
        return QModelIndex();

    return createIndex(parentItem->getRow(), 0, parentItem);
}

int TestModel::rowCount(const QModelIndex &parent) const
{
    TestItemBase *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = _rootItem;
    else
        parentItem = static_cast<TestItemBase*>(parent.internalPointer());

    return parentItem->childCount();
}

bool TestModel::isClean()
{
    return _clean;
}

bool TestModel::isChanged()
{
    return _changed;
}

bool TestModel::isRunning()
{
    return _running;
}

TestItemRoot * TestModel::rootItem()
{
    return _rootItem;
}

void TestModel::save(FileFormatBase *to)
{
    _rootItem->save(to);
    change(false);
}

void TestModel::read(FileFormatBase *from)
{
    beginResetModel();
    _rootItem->deleteChildren();
    _rootItem->read(from);
    endResetModel();
    change(false);
}

void TestModel::execute()
{
    _runningTestCount = _rootItem->prepareTestExecution();
    _completedTestCount = 0;

    for (int i = 0; i < _rootItem->childCount(); i++) {
        TestItemExecutable * exec = dynamic_cast<TestItemExecutable*>(_rootItem->getChild(i));
        if (exec == 0)
            continue;

        if (exec->getCheckState(0) == Qt::Unchecked)
            continue;

        queueJob(new ExecutableTestRunner(exec, _rootItem->getShuffle(), this));
    }
}

void TestModel::terminate()
{
    if (_pendingJobs.count() > 0) {
        _pendingJobs.at(0)->terminate();

        for (int i = _pendingJobs.count() -1; i > 0; i--) // Don't delete the first item here
        {
            delete _pendingJobs.at(i);
            _pendingJobs.removeAt(i);
        }
    }
}

void TestModel::addExecutable(QString fileName)
{
    refresh(index( new TestItemExecutable(QFileInfo(fileName), rootItem()) ));
}

void TestModel::removeExecutable(const QModelIndex index)
{
    removeExecutable( static_cast<TestItemBase*>(index.internalPointer()) );
}

void TestModel::removeExecutable(TestItemBase *item)
{
    change();

    // Check if the given item is the root item
    TestItemRoot *root = dynamic_cast<TestItemRoot*>(item);
    if (root != 0) {
        // If we got the root item, refresh each executable
        for (int i = 0; i < root->childCount(); i++) {
            TestItemExecutable *exec = dynamic_cast<TestItemExecutable*>(root->getChild(i));
            if (exec == 0) continue;

            removeExecutable(exec);
        }

        return;
    }

    // If we got anything else, let's go trough the items up to the executable
    TestItemExecutable *exec = 0;
    while (exec == 0 && item != 0) {
        exec = dynamic_cast<TestItemExecutable*>(item);
        if (exec != 0) {
            removeExecutable(exec);
            return;
        }

        item = item->getParent();
    }
}

void TestModel::removeExecutable(TestItemExecutable *item)
{
    beginRemoveRows(index(item->getParent()), item->getRow(), item->getRow());
    delete item;
    endRemoveRows();
}

void TestModel::refresh(const QModelIndex &index)
{
    refresh( static_cast<TestItemBase*>(index.internalPointer()) );
}

void TestModel::refresh(TestItemBase *item)
{
    change();

    // Check if the given item is the root item
    TestItemRoot *root = dynamic_cast<TestItemRoot*>(item);
    if (root != 0) {
        // If we got the root item, refresh each executable
        for (int i = 0; i < root->childCount(); i++) {
            TestItemExecutable *exec = dynamic_cast<TestItemExecutable*>(root->getChild(i));
            if (exec == 0) continue;

            refresh(exec);
        }

        return;
    }

    // If we got anything else, let's go trough the items up to the executable
    TestItemExecutable *exec = 0;
    while (exec == 0 && item != 0) {
        exec = dynamic_cast<TestItemExecutable*>(item);
        if (exec != 0) {
            refresh(exec);
            return;
        }

        item = item->getParent();
    }
}

void TestModel::refresh(TestItemExecutable *item)
{
    beginRemoveRows(index(item), 0, item->childCount());
    item->deleteChildren();
    endRemoveRows();

    queueJob(new ExecutableTester(item, this));
}

void TestModel::clear()
{
    beginResetModel();
    _rootItem->deleteChildren();
    _rootItem->setShuffle(true);
    endResetModel();

    _clean = true;
    _changed = false;
}

bool TestModel::getShuffle()
{
    return _rootItem->getShuffle();
}

void TestModel::setShuffle(bool shuffle)
{
    _rootItem->setShuffle(shuffle);
    change();
}

void TestModel::selectAll()
{
    _rootItem->selectAll();
}

void TestModel::deselectAll()
{
    _rootItem->deselectAll();
}

void TestModel::invertSelection()
{
    _rootItem->invertSelection();
}

void TestModel::jobExecuted()
{
    if (! _running) {
        _running = true;
        emit started();
        emit executionStateChanged(_running);
    }
}

void TestModel::jobFinished()
{
    ExecutableBase *currentJob = _pendingJobs.first();
    _pendingJobs.removeFirst();
    delete currentJob; // Delete here to prevent memory leak if for example any handler of finished crashes

    if (_pendingJobs.count() > 0) {
        executeNextJob();
    } else {
        _running = false;
        _runningTestCount = 0;
        _completedTestCount = 0;
        emit executionStateChanged(_running);
        emit finished();
    }
}

void TestModel::executeNextJob()
{
    try {
        if (_pendingJobs.count() < 1)
            throw std::runtime_error("No jobs pending");

        ExecutableBase *job = _pendingJobs.first();
        if (job->getStatus() != ExecutableBase::NotRunYet)
            throw std::runtime_error("The job is already running");

        job->execute();
    } catch(std::runtime_error &e) {
        if (_pendingJobs.count() > 0) {
            delete _pendingJobs.takeFirst();
        }

        if (_pendingJobs.count() > 0) { // still more than zero so there are other jobs
            int res =  QMessageBox::question(0, tr("Execution failed"),
                                             tr("%1\nDo you want to continue with the next?").arg(e.what()),
                                             QMessageBox::Yes, QMessageBox::No);

            if (res == QMessageBox::Yes) {
                executeNextJob();
            } else {
                qDeleteAll(_pendingJobs);
                _pendingJobs.clear();
                emit finished();
            }
        } else {
            QMessageBox::warning(0, tr("Execution failed"),
                                 e.what(), QMessageBox::Ok);

            emit finished();
        }

    }
}

void TestModel::change(bool c)
{
    _clean = false;
    _changed = c;
    emit changed();
}

void TestModel::queueJob(ExecutableBase *job)
{
    connect(job, SIGNAL(started()), this, SLOT(jobExecuted()));
    connect(job, SIGNAL(finished()), this, SLOT(jobFinished()));

    _pendingJobs.append(job);

    if (_pendingJobs.count() == 1) {
        // Execute async to let the caller add any more items to the queue before it actually gets executed
        QMetaObject::invokeMethod(this, "executeNextJob", Qt::QueuedConnection);
    }
}

void TestModel::update(const QModelIndex &index, QVector<int> roles)
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    emit dataChanged(index, index, roles);
#else
    Q_UNUSED(roles);
    emit dataChanged(index, index);
#endif
}

void TestModel::updateParents(const QModelIndex &index, QVector<int> roles, int column)
{
    for (QModelIndex parent = index.parent(); parent.isValid(); parent = parent.parent()) {
        QModelIndex colIndex = this->index(parent.row(), column, parent.parent());
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
        emit dataChanged(colIndex, colIndex, roles);
#else
        Q_UNUSED(roles);
        emit dataChanged(colIndex, colIndex);
#endif
    }
}

void TestModel::updateChildren(const QModelIndex &index, QVector<int> roles)
{
    TestItemBase *item = static_cast<TestItemBase*>(index.internalPointer());

    for (int i = 0; i < item->childCount(); i++) {
        QModelIndex child = index.child(i, index.column());
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
        emit dataChanged(child, child, roles);
#else
        emit dataChanged(child, child);
#endif
        updateChildren(child, roles);
    }
}

void TestModel::updateRunningTest(const QString &test)
{
    if (_runningTestCount > 0) {
        emit progressUpdated(static_cast<int>(double(_completedTestCount) / _runningTestCount * 100 + 0.5));
        _completedTestCount++;
    }

    emit testStarted(test);
}
