#ifndef TESTITEMBASE_H
#define TESTITEMBASE_H

#include <QList>
#include <QVariant>

class TestModel;

class TestItemBase
{
public:
    TestItemBase(TestItemBase *parent, TestModel *model);
    virtual ~TestItemBase();

    TestItemBase *getChild(int getRow);
    int childCount() const;
    int columnCount() const;
    int getRow() const;

    virtual bool hasCheckbox(int column) const;
    virtual QVariant getCheckState(int column) const;
    virtual bool setCheckState(int column, QVariant state);

    virtual QVariant getBackgroundColor(int column) const;
    virtual QVariant getData(int column) const =0;

    TestItemBase *getParent() const;
    TestModel *getModel() const;

protected:
    enum ColumnNames {
        ColumnName = 0,
        ColumnState,
        ColumnLast
    };

    enum TestStates {
        StateNone = 0,
        StatePass,
        StateFail,
        StatePartialPass
    };

    static const QList<QString> TestStateNames;
    static const QList<QVariant> TestStateColors;

    virtual TestStates getTestState() const;

private:
    QList<TestItemBase*> _childItems;

    TestItemBase *_parentItem;
    TestModel *_model;

    void appendChild(TestItemBase *getChild);
    void removeChild(TestItemBase *getChild);
};

#endif // TESTITEMBASE_H
