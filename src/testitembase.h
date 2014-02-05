#ifndef TESTITEM_H
#define TESTITEM_H

#include <QList>
#include <QVariant>

class TestModel;

class TestItemBase
{
public:
    enum TestStates {
        StateNone = 0,
        StatePass,
        StateFail,
        StatePartialPass
    };

    TestItemBase(QString name, bool enabled, TestItemBase *parent);
    TestItemBase(TestModel *model);
    ~TestItemBase();

    void appendChild(TestItemBase *child);
    void removeChild(TestItemBase *child);

    TestItemBase *child(int row);
    int childCount() const;
    int columnCount() const;
    bool isCheckbox(int column) const;
    QVariant bgColor(int column) const;
    QVariant checkState(int column) const;
    QVariant data(int column) const;
    int row() const;
    TestItemBase *parent() const;
    TestModel *model() const;

    Qt::CheckState enabled() const;
    void setEnabled(bool enabled);
    TestStates state() const;

private:
    QList<TestItemBase*> _childItems;

    QString _name;
    bool _enabled;
    TestStates _state;

    TestItemBase *_parentItem;
    TestModel *_model;
};

#endif // TESTITEM_H
