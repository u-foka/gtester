#include "executabletestrunner.h"

#include <QDebug>

#include "testitemexecutable.h"
#include "testitem.h"

ExecutableTestRunner::ExecutableTestRunner(TestItemExecutable *parentNode, bool shuffle, QObject *parent) :
    ExecutableBase(parentNode->getFileInfo(), parentNode->getTestArguments(), parent), _parentNode(parentNode)
{
    _args.append("--gtest_color=no");
    if (shuffle)
        _args.append("--gtest_shuffle");
}

ExecutableTestRunner::~ExecutableTestRunner()
{
}

void ExecutableTestRunner::parseOutput()
{
    const QRegExp testRun("^\\[\\s*RUN\\s*\\]\\s+(.+)$");
    const QRegExp testPass("^\\[\\s*OK\\s*\\]\\s+(.+) \\((\\d+) ms\\)$");
    const QRegExp testFail("^\\[\\s*FAILED\\s*\\]\\s+(.+) \\((\\d+) ms\\)$");

    qDebug() << "Executable output:";
    qDebug() << QString(_stdin);

    QString line;
    QString test;
    TestItem *item;
    QStringList testOutput;
    while ((line = _stdinStream.readLine()).isNull() == false) {

        if (testRun.exactMatch(line)) {

            testOutput.clear();
            test = testRun.capturedTexts().at(1);
            item = _parentNode->getTestItem(test);
            if (item == 0) {
                _status = UnknownError;
                return;
            }

        } else if (testPass.exactMatch(line)) {

            if (test != testPass.capturedTexts().at(1)) {
                _status = UnknownError;
                return;
            }

            item->setTestState(TestItemBase::StatePass);
            item->setOutput(testOutput.join('\n'));

        } else if (testFail.exactMatch(line)) {

            if (test != testFail.capturedTexts().at(1)) {
                _status = UnknownError;
                return;
            }

            item->setTestState(TestItemBase::StateFail);
            item->setOutput(testOutput.join('\n'));

        } else {

            testOutput.push_back(line);

        }

    }
}
