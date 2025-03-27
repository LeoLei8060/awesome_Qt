#include "tablefilterproxymodel.h"
#include "tablemodel.h"

TableFilterProxyModel::TableFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
    , m_nameFilterEnabled(false)
    , m_categoryFilterEnabled(false)
    , m_dateFilterEnabled(false)
    , m_priceFilterEnabled(false)
    , m_availabilityFilterEnabled(false)
    , m_minPrice(0)
    , m_maxPrice(10000)
    , m_availabilityState(-1)
    , m_storedMinPrice(0)
    , m_storedMaxPrice(10000)
    , m_storedAvailabilityState(-1)
{
    // 设置初始日期
    m_dateFrom = QDate::currentDate().addMonths(-1);
    m_dateTo = QDate::currentDate();
    m_storedDateFrom = m_dateFrom;
    m_storedDateTo = m_dateTo;
}

void TableFilterProxyModel::storeNameFilter(const QString &name)
{
    m_storedNameFilter = name;
}

void TableFilterProxyModel::storeCategoryFilter(const QString &category)
{
    m_storedCategoryFilter = category;
}

void TableFilterProxyModel::storeDateFrom(const QDate &from)
{
    m_storedDateFrom = from;
}

void TableFilterProxyModel::storeDateTo(const QDate &to)
{
    m_storedDateTo = to;
}

void TableFilterProxyModel::storeMinPrice(double minPrice)
{
    m_storedMinPrice = minPrice;
}

void TableFilterProxyModel::storeMaxPrice(double maxPrice)
{
    m_storedMaxPrice = maxPrice;
}

void TableFilterProxyModel::storeAvailabilityFilter(int state)
{
    m_storedAvailabilityState = state;
}

void TableFilterProxyModel::applyFilters()
{
    // 应用名称筛选
    m_nameFilter = m_storedNameFilter;
    m_nameFilterEnabled = !m_nameFilter.isEmpty();

    // 应用类别筛选
    if (m_storedCategoryFilter == tr("全部")) {
        m_categoryFilterEnabled = false;
    } else {
        m_categoryFilter = m_storedCategoryFilter;
        m_categoryFilterEnabled = true;
    }

    // 应用日期范围筛选
    m_dateFrom = m_storedDateFrom;
    m_dateTo = m_storedDateTo;
    m_dateFilterEnabled = true;

    // 应用价格范围筛选
    m_minPrice = m_storedMinPrice;
    m_maxPrice = m_storedMaxPrice;
    m_priceFilterEnabled = (m_minPrice > 0 || m_maxPrice < 10000);

    // 应用可用性筛选
    m_availabilityState = m_storedAvailabilityState;
    m_availabilityFilterEnabled = (m_availabilityState != -1);

    // 触发筛选
    invalidateFilter();
}

void TableFilterProxyModel::setNameFilter(const QString &name)
{
    storeNameFilter(name);
    m_nameFilter = name;
    m_nameFilterEnabled = !name.isEmpty();
    invalidateFilter();
}

void TableFilterProxyModel::setCategoryFilter(const QString &category)
{
    storeCategoryFilter(category);

    if (category == tr("全部")) {
        m_categoryFilterEnabled = false;
    } else {
        m_categoryFilter = category;
        m_categoryFilterEnabled = true;
    }
    invalidateFilter();
}

void TableFilterProxyModel::setDateRangeFilter(const QDate &from, const QDate &to)
{
    storeDateFrom(from);
    storeDateTo(to);

    m_dateFrom = from;
    m_dateTo = to;
    m_dateFilterEnabled = true;
    invalidateFilter();
}

void TableFilterProxyModel::setPriceRangeFilter(double minPrice, double maxPrice)
{
    storeMinPrice(minPrice);
    storeMaxPrice(maxPrice);

    m_minPrice = minPrice;
    m_maxPrice = maxPrice;
    m_priceFilterEnabled = (minPrice > 0 || maxPrice < 10000);
    invalidateFilter();
}

void TableFilterProxyModel::setAvailabilityFilter(int state)
{
    storeAvailabilityFilter(state);

    m_availabilityState = state;
    m_availabilityFilterEnabled = (state != -1);
    invalidateFilter();
}

void TableFilterProxyModel::resetFilters()
{
    // 重置存储的筛选条件
    m_storedNameFilter.clear();
    m_storedCategoryFilter = tr("全部");
    m_storedDateFrom = QDate::currentDate().addMonths(-1);
    m_storedDateTo = QDate::currentDate();
    m_storedMinPrice = 0;
    m_storedMaxPrice = 10000.00;
    m_storedAvailabilityState = -1;

    // 重置应用的筛选条件
    m_nameFilterEnabled = false;
    m_categoryFilterEnabled = false;
    m_dateFilterEnabled = false;
    m_priceFilterEnabled = false;
    m_availabilityFilterEnabled = false;

    m_nameFilter.clear();
    m_categoryFilter.clear();
    m_dateFrom = QDate::currentDate().addMonths(-1);
    m_dateTo = QDate::currentDate();
    m_minPrice = 0;
    m_maxPrice = 10000.00;
    m_availabilityState = -1;

    invalidateFilter();
}

bool TableFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    // 获取所有需要筛选的字段
    QModelIndex nameIndex = sourceModel()->index(sourceRow, TableItem::NameColumn, sourceParent);
    QModelIndex categoryIndex = sourceModel()->index(sourceRow,
                                                     TableItem::CategoryColumn,
                                                     sourceParent);
    QModelIndex dateIndex = sourceModel()->index(sourceRow, TableItem::DateColumn, sourceParent);
    QModelIndex priceIndex = sourceModel()->index(sourceRow, TableItem::PriceColumn, sourceParent);
    QModelIndex availableIndex = sourceModel()->index(sourceRow,
                                                      TableItem::AvailableColumn,
                                                      sourceParent);

    // 获取字段值
    QString name = sourceModel()->data(nameIndex).toString();
    QString category = sourceModel()->data(categoryIndex).toString();

    QDate date = sourceModel()->data(dateIndex, Qt::EditRole).toDate();

    double price = sourceModel()->data(priceIndex, Qt::EditRole).toDouble();

    bool available = sourceModel()->data(availableIndex, Qt::EditRole).toBool();

    // 应用名称筛选
    if (m_nameFilterEnabled && !name.contains(m_nameFilter, Qt::CaseInsensitive)) {
        return false;
    }

    // 应用类别筛选
    if (m_categoryFilterEnabled && category != m_categoryFilter) {
        return false;
    }

    // 应用日期范围筛选
    if (m_dateFilterEnabled && date.isValid()) {
        if (date < m_dateFrom || date > m_dateTo) {
            return false;
        }
    }

    // 应用价格范围筛选
    if (m_priceFilterEnabled) {
        if (price < m_minPrice || price > m_maxPrice) {
            return false;
        }
    }

    // 应用可用性筛选
    if (m_availabilityFilterEnabled) {
        bool expected = (m_availabilityState == 1);
        if (available != expected) {
            return false;
        }
    }

    // 通过所有筛选条件
    return true;
}
