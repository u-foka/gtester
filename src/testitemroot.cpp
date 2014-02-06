#include "testitemroot.h"

TestItemRoot::TestItemRoot(TestModel *model) :
    TestItemBase(0, model)
{
}

TestItemRoot::~TestItemRoot()
{
}

QVariant TestItemRoot::getData(int column) const
{
    switch (column) {
    case ColumnName:
        return "Name";
    case ColumnState:
        return "Status";
    default:
        return QVariant();
    }
}
