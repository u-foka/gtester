#include "executabletestrunner.h"

#include "testitemexecutable.h"

ExecutableTestRunner::ExecutableTestRunner(TestItemExecutable *parentNode, QObject *parent) :
    ExecutableBase(parentNode->getFileInfo(), parentNode->getTestArguments(), parent)
{
}

ExecutableTestRunner::~ExecutableTestRunner()
{
}

void ExecutableTestRunner::parseOutput()
{

}
