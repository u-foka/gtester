#ifndef TESTITEMCASE_H
#define TESTITEMCASE_H

#include "testitembase.h"

class TestItemExecutable;

class TestItemCase : public TestItemBase
{
public:
    TestItemCase(QString name, TestItemExecutable *parent);
    virtual ~TestItemCase();

    virtual QVariant getData(int column) const;

private:
    QString _name;
};

#endif // TESTITEMCASE_H
