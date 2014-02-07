#ifndef TESTITEMEXECUTABLE_H
#define TESTITEMEXECUTABLE_H

#include "testitembase.h"

#include <QFileInfo>

class TestItemRoot;

class TestItemExecutable : public TestItemBase
{
public:
    TestItemExecutable(QFileInfo file, TestItemRoot *parent);
    virtual ~TestItemExecutable();

    virtual QVariant getData(int column) const;

    const QFileInfo & getFileInfo() const;

private:
    QFileInfo _file;

};

#endif // TESTITEMEXECUTABLE_H
