#include "testitemroot.h"

#include "fileformatbase.h"

TestItemRoot::TestItemRoot(TestModel *model) :
    TestItemBase(0, model), _shuffle(false)
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

bool TestItemRoot::getShuffle()
{
    return _shuffle;
}

void TestItemRoot::setShuffle(bool shuffle)
{
    _shuffle = shuffle;
}

void TestItemRoot::save(FileFormatBase *to)
{
    to->SaveItem(this);
}

void TestItemRoot::saveChildren(FileFormatBase *to)
{
    for (QList<TestItemBase*>::Iterator i = _childItems.begin(); i != _childItems.end(); i++)
        (*i)->save(to);
}

void TestItemRoot::read(FileFormatBase *from)
{
    from->ReadItem(this);
}
