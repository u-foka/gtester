#include "testitemexecutable.h"

#include "testitemroot.h"

TestItemExecutable::TestItemExecutable(QFileInfo file, TestItemRoot *parent) :
    TestItemBase(parent, parent->getModel()), _file(file)
{
}

TestItemExecutable::~TestItemExecutable()
{
}

QVariant TestItemExecutable::getData(int column) const
{
    switch (column) {
    case ColumnName:
        return _file.fileName();
    case ColumnState:
        return TestStateNames[getTestState()];
    default:
        return QVariant();
    }
}

const QFileInfo & TestItemExecutable::getFileInfo() const
{
    return _file;
}
