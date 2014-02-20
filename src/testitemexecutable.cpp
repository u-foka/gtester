#include "testitemexecutable.h"

#include <stdexcept>

#include "testitemroot.h"
#include "testitemcase.h"
#include "testitem.h"

#include "fileformatbase.h"

TestItemExecutable::TestItemExecutable(TestItemBase *parent) :
    TestItemBase(parent, parent->getModel()), _file()
{
    if (dynamic_cast<TestItemRoot*>(getParent()) == 0)
        throw std::runtime_error("Invalid parent");
}

TestItemExecutable::TestItemExecutable(QFileInfo file, TestItemRoot *parent) :
    TestItemBase(parent, parent->getModel()), _file(file)
{
}

TestItemExecutable::~TestItemExecutable()
{
}

QVariant TestItemExecutable::getData(int column) const
{
    switch (column) {
    case ColumnName:
        return _file.fileName();
    case ColumnState:
        return TestStateNames[getTestState()];
    default:
        return QVariant();
    }
}

const QFileInfo & TestItemExecutable::getFileInfo() const
{
    return _file;
}

void TestItemExecutable::setFileInfo(const QFileInfo &file)
{
    if (! file.exists() || ! file.isExecutable())
        throw std::runtime_error("The given test executable not found: " + file.filePath().toStdString());

    _file = file;
}

const QStringList & TestItemExecutable::getTestArguments() const
{
    const QString disabled("--gtest_also_run_disabled_tests");

    QString filter("--gtest_filter=");

    QStringList tests;
    for (int testCaseId = 0; testCaseId < childCount(); testCaseId++) {
        TestItemCase *testCase = dynamic_cast<TestItemCase*>(getChild(testCaseId));
        if (testCase == 0)
            continue;

        for (int testId = 0; testId < testCase->childCount(); testId++) {
            TestItem *test = dynamic_cast<TestItem*>(testCase->getChild(testId));
            if (test == 0)
                continue;

            if (! test->getEnabled())
                continue;

            tests.append(testCase->getName() + "." + test->getName());
        }
    }

    filter.append(tests.join(":"));

    _args.clear();
    _args.append(disabled);
    _args.append(filter);

    return _args;
}

TestItem * TestItemExecutable::getTestItem(const QString &name) const
{
    QStringList split = name.split(".");

    if (split.count() != 2)
        return 0;

    return getTestItem(split[0], split[1]);
}

TestItem * TestItemExecutable::getTestItem(const QString &caseName, const QString &testName) const
{
    for (int testCaseId = 0; testCaseId < childCount(); testCaseId++) {
        TestItemCase *testCase = dynamic_cast<TestItemCase*>(getChild(testCaseId));
        if (testCase == 0)
            continue;

        if (testCase->getName() != caseName)
            continue;

        for (int testId = 0; testId < testCase->childCount(); testId++) {
            TestItem *test = dynamic_cast<TestItem*>(testCase->getChild(testId));
            if (test == 0)
                continue;

            if (test->getName() != testName)
                continue;

            return test;
        }
    }

    return 0;
}

void TestItemExecutable::save(FileFormatBase *to)
{
    to->SaveItem(this);
}

void TestItemExecutable::saveChildren(FileFormatBase *to)
{
    for (QList<TestItemBase*>::Iterator i = _childItems.begin(); i != _childItems.end(); i++)
        (*i)->save(to);
}

void TestItemExecutable::read(FileFormatBase *from)
{
    from->ReadItem(this);
}
