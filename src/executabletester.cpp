#include "executabletester.h"

#include <stdexcept>
#include <QRegExp>

#include "testitemexecutable.h"
#include "testitemcase.h"
#include "testitem.h"

ExecutableTester::ExecutableTester(TestItemExecutable *parentNode, QObject *parent) :
    ExecutableBase(parentNode->getFileInfo(), QStringList() << "--gtest_list_tests", parent), _parentNode(parentNode)
{
}

ExecutableTester::~ExecutableTester()
{
}

void ExecutableTester::parseOutput()
{
    const QRegExp testCaseMatch("^([\\w_]+)\\.$");
    const QRegExp testNameMatch("^\\s+([\\w_]+)$");

    TestItemCase *currentCase = 0;

    QString line;
    while ((line = _stdinStream.readLine()) != 0) {
        if (testCaseMatch.exactMatch(line)) {
            currentCase = new TestItemCase(testCaseMatch.capturedTexts().at(1), _parentNode);
        } else if (testNameMatch.exactMatch(line)) {
            if (currentCase == 0)
                throw std::runtime_error("Found test before test case");
            new TestItem(testNameMatch.capturedTexts().at(1), currentCase);
        }
    }
}
