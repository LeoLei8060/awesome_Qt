#include "listmodel.h"
#include <QDebug>

ListModel::ListModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int ListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    
    return m_items.size();
}

QVariant ListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_items.size())
        return QVariant();
    
    const ListItem &item = m_items.at(index.row());
    
    switch (role) {
    case NameRole:
        return item.getName();
    case DateRole:
        return item.getDate();
    case SizeRole:
        return item.getSize();
    case IconPathRole:
        return item.getIconPath();
    case IconRole:
        return QVariant::fromValue(item.getIcon());
    case Qt::ToolTipRole:
        return QString("%1\n日期: %2\n大小: %3 KB")
                .arg(item.getName())
                .arg(item.getDate().toString("yyyy-MM-dd hh:mm:ss"))
                .arg(item.getSize());
    default:
        return QVariant();
    }
}

bool ListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || index.row() >= m_items.size())
        return false;
    
    ListItem &item = m_items[index.row()];
    bool changed = false;
    
    switch (role) {
    case NameRole:
        item.setName(value.toString());
        changed = true;
        break;
    case DateRole:
        item.setDate(value.toDateTime());
        changed = true;
        break;
    case SizeRole:
        item.setSize(value.toInt());
        changed = true;
        break;
    case IconPathRole:
        item.setIconPath(value.toString());
        changed = true;
        break;
    default:
        break;
    }
    
    if (changed) {
        emit dataChanged(index, index, QVector<int>() << role);
        return true;
    }
    
    return false;
}

Qt::ItemFlags ListModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;
    
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

QHash<int, QByteArray> ListModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    roles[DateRole] = "date";
    roles[SizeRole] = "size";
    roles[IconPathRole] = "iconPath";
    roles[IconRole] = "icon";
    return roles;
}

void ListModel::addItem(const ListItem &item)
{
    beginInsertRows(QModelIndex(), m_items.size(), m_items.size());
    m_items.append(item);
    endInsertRows();
}

void ListModel::updateItem(int row, const ListItem &item)
{
    if (row < 0 || row >= m_items.size())
        return;
    
    m_items[row] = item;
    QModelIndex idx = index(row, 0);
    emit dataChanged(idx, idx);
}

void ListModel::removeItem(int row)
{
    if (row < 0 || row >= m_items.size())
        return;
    
    beginRemoveRows(QModelIndex(), row, row);
    m_items.removeAt(row);
    endRemoveRows();
}

ListItem ListModel::getItem(int row) const
{
    if (row < 0 || row >= m_items.size())
        return ListItem();
    
    return m_items.at(row);
}

QVector<ListItem> ListModel::getAllItems() const
{
    return m_items;
}

void ListModel::setItems(const QVector<ListItem> &items)
{
    beginResetModel();
    m_items = items;
    endResetModel();
}

void ListModel::clear()
{
    beginResetModel();
    m_items.clear();
    endResetModel();
}

// ListSortFilterProxyModel实现

ListSortFilterProxyModel::ListSortFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent),
      m_sortRole(ListModel::NameRole)
{
    setDynamicSortFilter(true);
}

void ListSortFilterProxyModel::setSortRole(int role)
{
    m_sortRole = role;
    invalidate();
}

int ListSortFilterProxyModel::sortRole() const
{
    return m_sortRole;
}

void ListSortFilterProxyModel::setFilterPattern(const QString &pattern)
{
    if (m_filterPattern != pattern) {
        m_filterPattern = pattern;
        invalidateFilter();
    }
}

QString ListSortFilterProxyModel::filterPattern() const
{
    return m_filterPattern;
}

bool ListSortFilterProxyModel::lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const
{
    QVariant leftData = sourceModel()->data(source_left, m_sortRole);
    QVariant rightData = sourceModel()->data(source_right, m_sortRole);
    
    switch (m_sortRole) {
    case ListModel::NameRole:
        return leftData.toString().compare(rightData.toString(), Qt::CaseInsensitive) < 0;
    case ListModel::DateRole:
        return leftData.toDateTime() < rightData.toDateTime();
    case ListModel::SizeRole:
        return leftData.toInt() < rightData.toInt();
    default:
        return false;
    }
}

bool ListSortFilterProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    if (m_filterPattern.isEmpty())
        return true;
    
    QModelIndex index = sourceModel()->index(source_row, 0, source_parent);
    QString name = sourceModel()->data(index, ListModel::NameRole).toString();
    
    return name.contains(m_filterPattern, Qt::CaseInsensitive);
}
