#ifndef EXECUTABLETESTRUNNER_H
#define EXECUTABLETESTRUNNER_H

#include "executablebase.h"

class TestItemExecutable;

class ExecutableTestRunner : public ExecutableBase
{
    Q_OBJECT
public:
    explicit ExecutableTestRunner(TestItemExecutable *parentNode, bool shuffle, QObject *parent = 0);
    virtual ~ExecutableTestRunner();

signals:

public slots:

protected:
    TestItemExecutable *_parentNode;

    virtual void parseOutput();

};

#endif // EXECUTABLETESTRUNNER_H
