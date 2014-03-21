#include "executabletestrunner.h"

#include <stdexcept>

#include "testitemexecutable.h"
#include "testitem.h"
#include "testmodel.h"

ExecutableTestRunner::ExecutableTestRunner(TestItemExecutable *parentNode, bool shuffle, QObject *parent) :
    ExecutableBase(parentNode->getFileInfo(), parentNode->getTestArguments(), parent), _parentNode(parentNode)
{
    if (parentNode == 0)
        throw std::runtime_error("Cannot create ExecutableTestRunner without a parent node");

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

    QString line;
    while ((line = _stdinStream.readLine()).isNull() == false) {

        if (testRun.exactMatch(line)) {

            _testOutput.clear();
            _currentTest = testRun.capturedTexts().at(1);
            _currentItem = _parentNode->getTestItem(_currentTest);
            if (_currentItem == 0) {
                _status = UnknownError;
                return;
            }

            _parentNode->getModel()->updateRunningTest(_currentTest);

        } else if (testPass.exactMatch(line)) {

            if (_currentTest != testPass.capturedTexts().at(1)) {
                _status = UnknownError;
                return;
            }

            _currentItem->setTestState(TestItemBase::StatePass);
            _currentItem->setOutput(_testOutput.join("\n"));
            _currentItem = 0;

        } else if (testFail.exactMatch(line)) {

            if (_currentTest != testFail.capturedTexts().at(1)) {
                _status = UnknownError;
                return;
            }

            _currentItem->setTestState(TestItemBase::StateFail);
            _currentItem->setOutput(_testOutput.join("\n"));
            _currentItem = 0;

        } else {

            _testOutput.push_back(line);

        }

    }

    if (_status != Running && _currentItem != 0) {
        // Test exited within a test body
        _currentItem->setTestState(TestItemBase::StateTerminated);
        _currentItem = 0;
    }
}
