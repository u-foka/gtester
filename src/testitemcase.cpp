#include "testitemcase.h"

#include "testitemexecutable.h"

TestItemCase::TestItemCase(QString name, TestItemExecutable *parent) :
    TestItemBase(parent, parent->getModel()), _name(name)
{
}

TestItemCase::~TestItemCase()
{
}

QVariant TestItemCase::getData(int column) const
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
