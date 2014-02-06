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

protected:
    virtual TestStates getTestState() const;

private:
    QString _name;
    bool _enabled;
    TestStates _state;
};

#endif // TESTITEM_H
