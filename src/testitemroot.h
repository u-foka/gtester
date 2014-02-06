#ifndef TESTITEMROOT_H
#define TESTITEMROOT_H

#include "testitembase.h"

class TestItemRoot : public TestItemBase
{
public:
    TestItemRoot(TestModel *model);
    virtual ~TestItemRoot();

    virtual QVariant getData(int column) const;

};

#endif // TESTITEMROOT_H
