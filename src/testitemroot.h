#ifndef TESTITEMROOT_H
#define TESTITEMROOT_H

#include "testitembase.h"

class TestItemRoot : public TestItemBase
{
public:
    TestItemRoot(TestModel *model);
    virtual ~TestItemRoot();

    virtual QVariant getData(int column) const;

    bool getShuffle();
    void setShuffle(bool shuffle);

    virtual void save(FileFormatBase *to);
    virtual void saveChildren(FileFormatBase *to);

    virtual void read(FileFormatBase *from);

private:
    bool _shuffle;

};

#endif // TESTITEMROOT_H
