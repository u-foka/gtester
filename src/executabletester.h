#ifndef EXECUTABLETESTER_H
#define EXECUTABLETESTER_H

#include "executablebase.h"
#include "testitembase.h"

class ExecutableTester : public ExecutableBase
{
    Q_OBJECT
public:
    explicit ExecutableTester(TestItemBase *parentNode, QObject *parent = 0);
    explicit ExecutableTester(TestItemBase *parentNode, const QFileInfo &file, QObject *parent = 0);

signals:

public slots:

protected:
    TestItemBase *_parentNode;

    void parseOutput();
};

#endif // EXECUTABLETESTER_H
