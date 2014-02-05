#include "testitem.h"

#include <stdexcept>
#include <QStringList>
#include <QColor>

#include "testmodel.h"

enum ColumnNames {
    ColumnName = 0,
    ColumnState,
    ColumnLast
};

const QList<QString> TestStateNames(QList<QString>() <<
        "Not yet run" <<
        "Passed" <<
        "Failed" <<
        "Partially passed"
        );

const QList<QVariant> TestStateColors(QList<QVariant>() <<
        QColor(Qt::lightGray) <<
        QColor(Qt::green) <<
        QColor(Qt::red) <<
        QColor(Qt::yellow)
        );

TestItem::TestItem(QString name, bool enabled, TestItem *parent) :
    _name(name), _enabled(enabled), _state(StateNone), _parentItem(parent), _model(parent->model())
{
    _parentItem->appendChild(this);

    _model->beginInsertRows(_model->index(_parentItem), row(), row());
    _model->endInsertRows();
}

TestItem::TestItem(TestModel *model) :
    _name(), _enabled(true), _state(StateNone), _parentItem(0), _model(model)
{
}

TestItem::~TestItem()
{
    if (_parentItem != 0)
        _parentItem->removeChild(this);

    // Need to delete children in reverse order because they will
    // remove themselfs from this list in their destructor
    QList<TestItem*>::Iterator it = _childItems.end();
    while (it != _childItems.begin())
        delete *--it;
}

void TestItem::appendChild(TestItem *item)
{
    if (_childItems.indexOf(item) >= 0) {
        throw std::runtime_error("Cannot append duplicate child item");
    }

    _childItems.append(item);
}

void TestItem::removeChild(TestItem *child)
{
    _childItems.removeAll(child);
}

TestItem *TestItem::child(int row)
{
    return _childItems.value(row);
}

int TestItem::childCount() const
{
    return _childItems.count();
}

int TestItem::columnCount() const
{
    return ColumnLast;
}

bool TestItem::isCheckbox(int column) const
{
    if (_parentItem == 0)
        return false;

    return column == ColumnName;
}

QVariant TestItem::bgColor(int column) const
{
    switch (column) {
    case ColumnState:
        return TestStateColors[state()];
    default:
        return QVariant();
    }
}

QVariant TestItem::checkState(int column) const
{
    switch (column) {
    case ColumnName:
        return enabled();
    default:
        return QVariant();
    }
}

QVariant TestItem::data(int column) const
{
    switch (column) {
    case ColumnName:
        if (_parentItem == 0) return "Name";
        return _name;
    case ColumnState:
        if (_parentItem == 0) return "Status";
        return TestStateNames[state()];
    default:
        return QVariant();
    }
}

int TestItem::row() const
{
    if (_parentItem)
        return _parentItem->_childItems.indexOf(const_cast<TestItem*>(this));

    return 0;
}

TestItem *TestItem::parent() const
{
    return _parentItem;
}

TestModel *TestItem::model() const
{
    return _model;
}

Qt::CheckState TestItem::enabled() const
{
    if (_childItems.count() == 0)
        return _enabled ? Qt::Checked : Qt::Unchecked;

    Qt::CheckState enabled;
    for (QList<TestItem*>::ConstIterator it = _childItems.begin(); it != _childItems.end(); it++) {
        if (it == _childItems.begin()) {
            // Inicialize enabled with the first item
            enabled = (*it)->enabled();
        } else if (enabled != (*it)->enabled()) {
            // Already have both checked and unchecked children, returning immediatelly
            return Qt::PartiallyChecked;
        }
    }

    return enabled;
}

void TestItem::setEnabled(bool enabled)
{
    if (_childItems.count() == 0) {
        _enabled = enabled;
    }

    for (QList<TestItem*>::ConstIterator it = _childItems.begin(); it != _childItems.end(); it++)
        (*it)->setEnabled(enabled);
}

TestItem::TestStates TestItem::state() const
{
    if (_childItems.count() == 0)
        return _state;

    TestStates state;
    for (QList<TestItem*>::ConstIterator it = _childItems.begin(); it != _childItems.end(); it++) {
        if ((*it)->state() == StateFail) {
            return StateFail;
        } else if (it == _childItems.begin()) {
            // Inicialize enabled with the first item
            state = (*it)->state();
        } else if (state != (*it)->state()) {
            // Already have both StatePass and StateNone children, returning immediatelly
            return StatePartialPass;
        }
    }

    return state;
}
