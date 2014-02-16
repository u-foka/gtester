#ifndef EXECUTABLETESTRUNNER_H
#define EXECUTABLETESTRUNNER_H

#include "executablebase.h"

class TestItemExecutable;
class TestItem;

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

    QString _currentTest;
    TestItem *_currentItem;
    QStringList _testOutput;

    virtual void parseOutput();

};

#endif // EXECUTABLETESTRUNNER_H
