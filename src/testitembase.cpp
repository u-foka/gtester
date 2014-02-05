#include "testitembase.h"

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

TestItemBase::TestItemBase(QString name, bool enabled, TestItemBase *parent) :
    _name(name), _enabled(enabled), _state(StateNone), _parentItem(parent), _model(parent->model())
{
    _parentItem->appendChild(this);

    _model->beginInsertRows(_model->index(_parentItem), row(), row());
    _model->endInsertRows();
}

TestItemBase::TestItemBase(TestModel *model) :
    _name(), _enabled(true), _state(StateNone), _parentItem(0), _model(model)
{
}

TestItemBase::~TestItemBase()
{
    if (_parentItem != 0)
        _parentItem->removeChild(this);

    // Need to delete children in reverse order because they will
    // remove themselfs from this list in their destructor
    QList<TestItemBase*>::Iterator it = _childItems.end();
    while (it != _childItems.begin())
        delete *--it;
}

void TestItemBase::appendChild(TestItemBase *item)
{
    if (_childItems.indexOf(item) >= 0) {
        throw std::runtime_error("Cannot append duplicate child item");
    }

    _childItems.append(item);
}

void TestItemBase::removeChild(TestItemBase *child)
{
    _childItems.removeAll(child);
}

TestItemBase *TestItemBase::child(int row)
{
    return _childItems.value(row);
}

int TestItemBase::childCount() const
{
    return _childItems.count();
}

int TestItemBase::columnCount() const
{
    return ColumnLast;
}

bool TestItemBase::isCheckbox(int column) const
{
    if (_parentItem == 0)
        return false;

    return column == ColumnName;
}

QVariant TestItemBase::bgColor(int column) const
{
    switch (column) {
    case ColumnState:
        return TestStateColors[state()];
    default:
        return QVariant();
    }
}

QVariant TestItemBase::checkState(int column) const
{
    switch (column) {
    case ColumnName:
        return enabled();
    default:
        return QVariant();
    }
}

QVariant TestItemBase::data(int column) const
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

int TestItemBase::row() const
{
    if (_parentItem)
        return _parentItem->_childItems.indexOf(const_cast<TestItemBase*>(this));

    return 0;
}

TestItemBase *TestItemBase::parent() const
{
    return _parentItem;
}

TestModel *TestItemBase::model() const
{
    return _model;
}

Qt::CheckState TestItemBase::enabled() const
{
    if (_childItems.count() == 0)
        return _enabled ? Qt::Checked : Qt::Unchecked;

    Qt::CheckState enabled;
    for (QList<TestItemBase*>::ConstIterator it = _childItems.begin(); it != _childItems.end(); it++) {
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

void TestItemBase::setEnabled(bool enabled)
{
    if (_childItems.count() == 0) {
        _enabled = enabled;
    }

    for (QList<TestItemBase*>::ConstIterator it = _childItems.begin(); it != _childItems.end(); it++)
        (*it)->setEnabled(enabled);
}

TestItemBase::TestStates TestItemBase::state() const
{
    if (_childItems.count() == 0)
        return _state;

    TestStates state;
    for (QList<TestItemBase*>::ConstIterator it = _childItems.begin(); it != _childItems.end(); it++) {
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
