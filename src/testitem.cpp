#include "testitem.h"

#include "testitemcase.h"
#include "testmodel.h"

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

    QModelIndex index = getModel()->index(this, ColumnState);
    QVector<int> roles;
    roles << Qt::DisplayRole;

    getModel()->update(index, roles);
    getModel()->updateParents(index, roles, ColumnState);
}

const QString & TestItem::getName() const
{
    return _name;
}

bool TestItem::getEnabled() const
{
    return _enabled;
}

const QString & TestItem::getOutput() const
{
    return _output;
}

void TestItem::setOutput(const QString &output)
{
    _output = output;
}

int TestItem::prepareTestExecution()
{
    setTestState(StateNone);
    return _enabled ? 1 : 0;
}
