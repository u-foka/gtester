#ifndef TESTITEMBASE_H
#define TESTITEMBASE_H

#include <QList>
#include <QVariant>

class TestModel;
class FileFormatBase;

class TestItemBase
{
public:
    TestItemBase(TestItemBase *parent, TestModel *model);
    virtual ~TestItemBase();

    enum TestStates {
        StateNone = 0,
        StatePass,
        StateFail,
        StateTerminated,
        StatePartialPass
    };

    static const QList<QString> TestStateNames;
    static const QList<QVariant> TestStateColors;

    TestItemBase *getChild(int getRow) const;
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

    virtual TestStates getTestState() const;

    virtual void selectAll();
    virtual void deselectAll();
    virtual void invertSelection();

    void deleteChildren();
    virtual int prepareTestExecution();

    virtual void save(FileFormatBase *to) =0;
    virtual void saveChildren(FileFormatBase *to) =0;

    virtual void read(FileFormatBase *from) =0;

protected:
    QList<TestItemBase*> _childItems;

    enum ColumnNames {
        ColumnName = 0,
        ColumnState,
        ColumnLast
    };

private:
    TestItemBase *_parentItem;
    TestModel *_model;

    void appendChild(TestItemBase *getChild);
    void removeChild(TestItemBase *getChild);
};

#endif // TESTITEMBASE_H
