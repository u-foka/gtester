#include "testitem.h"

#include "testitemcase.h"

TestItem::TestItem(QString name, TestItemCase *parent) :
    TestItemBase(parent, parent->getModel()), _name(name), _enabled(true), _state(StateNone)
{
    const QString disabled("DISABLED_");

    if (_name.startsWith(disabled))
        _enabled = false;
}

TestItem::~TestItem()
{
}

QVariant TestItem::getCheckState(int column) const
{
    if (column != ColumnName)
        return QVariant();

    return _enabled ? Qt::Checked : Qt::Unchecked;
}

bool TestItem::setCheckState(int column, QVariant state)
{
    if (column != ColumnName)
        return false;

    _enabled = state.toInt() != Qt::Unchecked;

    return true;
}

QVariant TestItem::getData(int column) const
{
    switch (column) {
    case ColumnName:
        return _name;
    case ColumnState:
        return TestStateNames[getTestState()];
    default:
        return QVariant();
    }
}

TestItem::TestStates TestItem::getTestState() const
{
    return _state;
}

void TestItem::setTestState(TestStates state)
{
    _state = state;
}

const QString & TestItem::getName() const
{
    return _name;
}

bool TestItem::getEnabled() const
{
    return _enabled;
}
