#ifndef TESTITEMEXECUTABLE_H
#define TESTITEMEXECUTABLE_H

#include "testitembase.h"

#include <QFileInfo>

class TestItemRoot;
class TestItem;

class TestItemExecutable : public TestItemBase
{
public:
    TestItemExecutable(QFileInfo file, TestItemRoot *parent);
    virtual ~TestItemExecutable();

    virtual QVariant getData(int column) const;

    const QFileInfo & getFileInfo() const;
    const QStringList & getTestArguments() const;

    TestItem *getTestItem(const QString &name) const;
    TestItem *getTestItem(const QString &caseName, const QString &testName) const;

private:
    QFileInfo _file;
    mutable QStringList _args;

};

#endif // TESTITEMEXECUTABLE_H
