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

private:
    QString _name;
    bool _enabled;
    TestStates _state;
};

#endif // TESTITEM_H
