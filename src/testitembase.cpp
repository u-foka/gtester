#include "testitembase.h"

#include <stdexcept>
#include <QStringList>
#include <QColor>

#include "testmodel.h"

const QList<QString> TestItemBase::TestStateNames(QList<QString>() <<
        "Not yet run" <<
        "Passed" <<
        "Failed" <<
        "Partially passed"
        );

const QList<QVariant> TestItemBase::TestStateColors(QList<QVariant>() <<
        QColor(Qt::lightGray) <<
        QColor(Qt::green) <<
        QColor(Qt::red) <<
        QColor(Qt::yellow)
        );

TestItemBase::TestItemBase(TestItemBase *parent, TestModel *model) :
    _parentItem(parent), _model(model)
{
    if (_parentItem == 0)
        return;

    _parentItem->appendChild(this);

    _model->beginInsertRows(_model->index(_parentItem), getRow(), getRow());
    _model->endInsertRows();
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

TestItemBase *TestItemBase::getChild(int row)
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

QVariant TestItemBase::getBackgroundColor(int column) const
{
    switch (column) {
    case ColumnState:
        return TestStateColors[getTestState()];
    default:
        return QVariant();
    }
}

int TestItemBase::getRow() const
{
    if (_parentItem)
        return _parentItem->_childItems.indexOf(const_cast<TestItemBase*>(this));

    return 0;
}

bool TestItemBase::hasCheckbox(int column) const
{
    return column == ColumnName;
}

QVariant TestItemBase::getCheckState(int column) const
{
    Qt::CheckState state = Qt::PartiallyChecked;
    for (QList<TestItemBase*>::ConstIterator it = _childItems.begin(); it != _childItems.end(); it++) {
        if (it == _childItems.begin()) {
            // Inicialize state with the first item
            QVariant firstState = (*it)->getCheckState(column);
            if (firstState.isNull()) return firstState;
            state = static_cast<Qt::CheckState>((*it)->getCheckState(column).toInt());
        } else if (state != (*it)->getCheckState(column).toInt()) {
            // Already have both checked and unchecked children, returning immediatelly
            return Qt::PartiallyChecked;
        }
    }

    return state;
}

bool TestItemBase::setCheckState(int column, QVariant state)
{
    for (QList<TestItemBase*>::ConstIterator it = _childItems.begin(); it != _childItems.end(); it++)
        if (! (*it)->setCheckState(column, state))
            return false;

    return true;
}

TestItemBase *TestItemBase::getParent() const
{
    return _parentItem;
}

TestModel *TestItemBase::getModel() const
{
    return _model;
}

TestItemBase::TestStates TestItemBase::getTestState() const
{
    TestStates state = StateNone;
    for (QList<TestItemBase*>::ConstIterator it = _childItems.begin(); it != _childItems.end(); it++) {
        if ((*it)->getTestState() == StateFail) {
            return StateFail;
        } else if (it == _childItems.begin()) {
            // Inicialize enabled with the first item
            state = (*it)->getTestState();
        } else if (state != (*it)->getTestState()) {
            // Already have both StatePass and StateNone children, returning immediatelly
            return StatePartialPass;
        }
    }

    return state;
}
