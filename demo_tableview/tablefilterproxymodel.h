#ifndef TABLEFILTERPROXYMODEL_H
#define TABLEFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QDate>
#include "tableitem.h"

/**
 * @brief 自定义过滤代理模型，支持日期范围、价格范围和可用性的筛选
 */
class TableFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit TableFilterProxyModel(QObject *parent = nullptr);

    // 存储筛选条件（不应用）
    void storeNameFilter(const QString &name);
    void storeCategoryFilter(const QString &category);
    void storeDateFrom(const QDate &from);
    void storeDateTo(const QDate &to);
    void storeMinPrice(double minPrice);
    void storeMaxPrice(double maxPrice);
    void storeAvailabilityFilter(int state); // -1: 全部, 0: 否, 1: 是
    
    // 应用存储的筛选条件
    void applyFilters();
    
    // 立即应用的筛选方法（保留供兼容）
    void setNameFilter(const QString &name);
    void setCategoryFilter(const QString &category);
    void setDateRangeFilter(const QDate &from, const QDate &to);
    void setPriceRangeFilter(double minPrice, double maxPrice);
    void setAvailabilityFilter(int state); // -1: 全部, 0: 否, 1: 是

    // 重置所有筛选器
    void resetFilters();

protected:
    // 重写父类的过滤方法
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

private:
    // 存储的筛选条件（未应用）
    QString m_storedNameFilter;
    QString m_storedCategoryFilter;
    QDate m_storedDateFrom;
    QDate m_storedDateTo;
    double m_storedMinPrice;
    double m_storedMaxPrice;
    int m_storedAvailabilityState; // -1: 全部, 0: 否, 1: 是
    
    // 应用的筛选条件
    QString m_nameFilter;
    QString m_categoryFilter;
    QDate m_dateFrom;
    QDate m_dateTo;
    double m_minPrice;
    double m_maxPrice;
    int m_availabilityState; // -1: 全部, 0: 否, 1: 是

    // 各种条件是否启用
    bool m_nameFilterEnabled;
    bool m_categoryFilterEnabled;
    bool m_dateFilterEnabled;
    bool m_priceFilterEnabled;
    bool m_availabilityFilterEnabled;
};

#endif // TABLEFILTERPROXYMODEL_H
