#ifndef TESTITEM_H
#define TESTITEM_H

#include "testitembase.h"

class TestItemCase;

class TestItem : public TestItemBase
{
public:
    TestItem(QString name, TestItemCase *parent);
    virtual ~TestItem();

    virtual QVariant getCheckState(int column) const;
    virtual bool setCheckState(int column, QVariant state);

    virtual QVariant getData(int column) const;

    virtual TestStates getTestState() const;
    void setTestState(TestStates state);

    const QString & getName() const;
    bool getEnabled() const;

    const QString & getOutput() const;
    void setOutput(const QString &output);

    virtual int prepareTestExecution();

private:
    QString _name;
    bool _enabled;
    TestStates _state;
    QString _output;
};

#endif // TESTITEM_H
