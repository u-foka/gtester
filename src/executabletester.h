#ifndef EXECUTABLETESTER_H
#define EXECUTABLETESTER_H

#include "executablebase.h"

class TestItemExecutable;

class ExecutableTester : public ExecutableBase
{
    Q_OBJECT
public:
    explicit ExecutableTester(TestItemExecutable *parentNode, QObject *parent = 0);
    virtual ~ExecutableTester();

signals:

public slots:

protected:
    TestItemExecutable *_parentNode;

    void parseOutput();
};

#endif // EXECUTABLETESTER_H
