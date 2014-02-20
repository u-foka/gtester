#ifndef TESTITEMEXECUTABLE_H
#define TESTITEMEXECUTABLE_H

#include "testitembase.h"

#include <QFileInfo>
#include <QStringList>

class TestItemRoot;
class TestItem;

class TestItemExecutable : public TestItemBase
{
public:
    explicit TestItemExecutable(TestItemBase *parent);
    TestItemExecutable(QFileInfo file, TestItemRoot *parent);
    virtual ~TestItemExecutable();

    virtual QVariant getData(int column) const;

    const QFileInfo & getFileInfo() const;
    void setFileInfo(const QFileInfo &file);
    const QStringList & getTestArguments() const;

    TestItem *getTestItem(const QString &name) const;
    TestItem *getTestItem(const QString &caseName, const QString &testName) const;

    virtual void save(FileFormatBase *to);
    virtual void saveChildren(FileFormatBase *to);

    virtual void read(FileFormatBase *from);

private:
    QFileInfo _file;
    mutable QStringList _args;

};

#endif // TESTITEMEXECUTABLE_H
