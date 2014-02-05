#ifndef EXECUTABLETESTER_H
#define EXECUTABLETESTER_H

#include "executablebase.h"
#include "testitem.h"

class ExecutableTester : public ExecutableBase
{
    Q_OBJECT
public:
    explicit ExecutableTester(TestItem *parentNode, QObject *parent = 0);
    explicit ExecutableTester(TestItem *parentNode, const QFileInfo &file, QObject *parent = 0);

signals:

public slots:

protected:
    TestItem *_parentNode;

    void parseOutput();
};

#endif // EXECUTABLETESTER_H
