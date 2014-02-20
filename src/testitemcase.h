#ifndef TESTITEMCASE_H
#define TESTITEMCASE_H

#include "testitembase.h"

class TestItemExecutable;

class TestItemCase : public TestItemBase
{
public:
    explicit TestItemCase(TestItemBase *parent);
    TestItemCase(QString name, TestItemExecutable *parent);
    virtual ~TestItemCase();

    virtual QVariant getData(int column) const;

    const QString & getName() const;
    void setName(const QString &name);

    virtual void save(FileFormatBase *to);
    virtual void saveChildren(FileFormatBase *to);

    virtual void read(FileFormatBase *from);

private:
    QString _name;
};

#endif // TESTITEMCASE_H
