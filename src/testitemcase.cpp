#include "testitemcase.h"

#include <stdexcept>

#include "testitemexecutable.h"

#include "fileformatbase.h"

TestItemCase::TestItemCase(TestItemBase *parent)
    : TestItemBase(parent, parent->getModel()), _name()
{
    if (dynamic_cast<TestItemExecutable*>(getParent()) == 0)
        throw std::runtime_error("Invalid parent");
}

TestItemCase::TestItemCase(QString name, TestItemExecutable *parent)
    : TestItemBase(parent, parent->getModel()), _name(name)
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

const QString & TestItemCase::getName() const
{
    return _name;
}

void TestItemCase::setName(const QString &name)
{
    _name = name;
}

void TestItemCase::save(FileFormatBase *to)
{
    to->SaveItem(this);
}

void TestItemCase::saveChildren(FileFormatBase *to)
{
    for (QList<TestItemBase*>::Iterator i = _childItems.begin(); i != _childItems.end(); i++)
        (*i)->save(to);
}

void TestItemCase::read(FileFormatBase *from)
{
    from->ReadItem(this);
}
