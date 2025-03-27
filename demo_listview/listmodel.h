#ifndef LISTMODEL_H
#define LISTMODEL_H

#include "listitem.h"
#include <QAbstractListModel>
#include <QSortFilterProxyModel>
#include <QVector>

class ListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles {
        NameRole = Qt::DisplayRole,
        DateRole = Qt::UserRole + 1,
        SizeRole,
        IconPathRole,
        IconRole
    };

    explicit ListModel(QObject *parent = nullptr);

    // QAbstractItemModel接口实现
    int      rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags          flags(const QModelIndex &index) const override;
    QHash<int, QByteArray> roleNames() const override;

    // 自定义方法
    void              addItem(const ListItem &item);
    void              updateItem(int row, const ListItem &item);
    void              removeItem(int row);
    ListItem          getItem(int row) const;
    QVector<ListItem> getAllItems() const;
    void              setItems(const QVector<ListItem> &items);
    void              clear();

private:
    QVector<ListItem> m_items;
};

// 排序筛选代理模型
class ListSortFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit ListSortFilterProxyModel(QObject *parent = nullptr);

    void setSortRole(int role);
    int  sortRole() const;

    void    setFilterPattern(const QString &pattern);
    QString filterPattern() const;

protected:
    bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const override;
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

private:
    int     m_sortRole;
    QString m_filterPattern;
};

#endif // LISTMODEL_H
