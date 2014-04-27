#ifndef TESTITEM_H
#define TESTITEM_H

#include "testitembase.h"

class TestItemCase;

class TestItem : public TestItemBase
{
public:
    explicit TestItem(TestItemBase *parent);
    TestItem(QString name, TestItemCase *parent);
    virtual ~TestItem();

    virtual QVariant getCheckState(int column) const;
    virtual bool setCheckState(int column, QVariant state);

    virtual QVariant getData(int column) const;

    virtual TestStates getTestState() const;
    void setTestState(TestStates state);

    virtual size_t getErrorCount() const;

    virtual void selectAll();
    virtual void deselectAll();
    virtual void invertSelection();

    const QString & getName() const;
    void setName(const QString &name);
    bool getEnabled() const;
    void setEnabled(bool enabled);

    const QString & getOutput() const;
    void setOutput(const QString &output);

    virtual int prepareTestExecution();

    virtual void save(FileFormatBase *to);
    virtual void saveChildren(FileFormatBase *to);

    virtual void read(FileFormatBase *from);

private:
    QString _name;
    bool _enabled;
    TestStates _state;
    QString _output;
};

#endif // TESTITEM_H
