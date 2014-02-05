#include "executabletester.h"

#include <stdexcept>
#include <QRegExp>

ExecutableTester::ExecutableTester(TestItem *parentNode, QObject *parent) :
    ExecutableBase(parent), _parentNode(parentNode)
{
}

ExecutableTester::ExecutableTester(TestItem *parentNode, const QFileInfo &file, QObject *parent) :
    ExecutableBase(file, parent), _parentNode(parentNode)
{
}

void ExecutableTester::parseOutput()
{
    const QRegExp testCaseMatch("^([\\w_]+)\\.$");
    const QRegExp testNameMatch("^\\s+([\\w_]+)$");

    TestItem *currentCase = 0;

    QString line;
    while ((line = _stdinStream.readLine()) != 0) {
        if (testCaseMatch.exactMatch(line)) {
            currentCase = new TestItem(testCaseMatch.capturedTexts().at(1), true, _parentNode);
        } else if (testNameMatch.exactMatch(line)) {
            if (currentCase == 0)
                throw std::runtime_error("Found test before test case");
            new TestItem(testNameMatch.capturedTexts().at(1), true, currentCase);
        }
    }
}
