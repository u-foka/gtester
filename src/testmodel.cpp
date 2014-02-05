#include "testmodel.h"

#include <QtGui>

#include "testitem.h"

TestModel::TestModel(QObject *parent)
    : QAbstractItemModel(parent), _rootItem(0)
{
    _rootItem = new TestItem(this);
}

TestModel::~TestModel()
{
    delete _rootItem;
}

int TestModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<TestItem*>(parent.internalPointer())->columnCount();
    else
        return _rootItem->columnCount();
}

QVariant TestModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    TestItem *item = static_cast<TestItem*>(index.internalPointer());

    if (role == Qt::ForegroundRole)
        return QColor(Qt::black);

    if (role == Qt::BackgroundColorRole)
        return item->bgColor(index.column());

    if (role == Qt::CheckStateRole && item->isCheckbox(index.column()))
        return item->checkState(index.column());

    if (role != Qt::DisplayRole)
        return QVariant();

    return item->data(index.column());
}

bool TestModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    TestItem *item = static_cast<TestItem*>(index.internalPointer());

    if (role == Qt::CheckStateRole) {
        item->setEnabled(value.toInt() == Qt::Checked ? true : false);
        QVector<int> roles;
        roles << role;
        emit dataChanged(index, index, roles);
        updateParents(index, roles);
        updateChildren(index, roles);
        return true;
    }

    return false;
}

Qt::ItemFlags TestModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    TestItem *item = static_cast<TestItem*>(index.internalPointer());

    Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

    if (item->isCheckbox(index.column()))
        flags |= Qt::ItemIsUserCheckable;

    return flags;
}

QVariant TestModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return _rootItem->data(section);

    return QVariant();
}

QModelIndex TestModel::index(int row, int column, const QModelIndex &parent)
const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    TestItem *parentItem;

    if (!parent.isValid())
        parentItem = _rootItem;
    else
        parentItem = static_cast<TestItem*>(parent.internalPointer());

    TestItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex TestModel::index(TestItem *item) const
{
    if (item == _rootItem)
        return QModelIndex();

    return createIndex(item->row(), 0, item);
}

QModelIndex TestModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    TestItem *childItem = static_cast<TestItem*>(index.internalPointer());
    TestItem *parentItem = childItem->parent();

    if (parentItem == _rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int TestModel::rowCount(const QModelIndex &parent) const
{
    TestItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = _rootItem;
    else
        parentItem = static_cast<TestItem*>(parent.internalPointer());

    return parentItem->childCount();
}

TestItem * TestModel::rootItem()
{
    return _rootItem;
}

void TestModel::updateParents(const QModelIndex &index, QVector<int> roles)
{
    for (QModelIndex parent = index.parent(); parent.isValid(); parent = parent.parent())
        emit dataChanged(parent, parent, roles);
}

void TestModel::updateChildren(const QModelIndex &index, QVector<int> roles)
{
    TestItem *item = static_cast<TestItem*>(index.internalPointer());

    for (int i = 0; i < item->childCount(); i++) {
        QModelIndex child = index.child(i, index.column());
        emit dataChanged(child, child, roles);
        updateChildren(child, roles);
    }
}
