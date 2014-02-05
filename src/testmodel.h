#ifndef TESTMODEL_H
#define TESTMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>

class TestItemBase;

class TestModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    TestModel(QObject *parent = 0);
    ~TestModel();

    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const;
    QModelIndex index(TestItemBase *item) const;
    QModelIndex parent(const QModelIndex &index) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    TestItemBase * rootItem();

private:
    TestItemBase *_rootItem;

    friend class TestItemBase;

    void updateParents(const QModelIndex &index, QVector<int> roles = QVector<int>());
    void updateChildren(const QModelIndex &index, QVector<int> roles = QVector<int>());
};

#endif // TESTMODEL_H
