#ifndef TABLEITEM_H
#define TABLEITEM_H

#include <QVariant>
#include <QString>
#include <QDate>

/**
 * @brief 表格项类，用于存储表格中一行的数据
 */
class TableItem
{
public:
    TableItem();
    TableItem(int id, const QString &name, const QString &category, 
              const QDate &date, double price, bool available);
    ~TableItem();

    // 获取数据函数
    int getId() const;
    QString getName() const;
    QString getCategory() const;
    QDate getDate() const;
    double getPrice() const;
    bool isAvailable() const;

    // 设置数据函数
    void setId(int id);
    void setName(const QString &name);
    void setCategory(const QString &category);
    void setDate(const QDate &date);
    void setPrice(double price);
    void setAvailable(bool available);

    // 根据列索引获取数据
    QVariant data(int column) const;
    
    // 根据列索引设置数据
    bool setData(int column, const QVariant &value);

    // 定义列枚举
    enum Column {
        IdColumn = 0,
        NameColumn,
        CategoryColumn,
        DateColumn,
        PriceColumn,
        AvailableColumn,
        ColumnCount  // 总列数
    };

    // 获取列标题
    static QString columnName(int column);

private:
    int m_id;
    QString m_name;
    QString m_category;
    QDate m_date;
    double m_price;
    bool m_available;
};

#endif // TABLEITEM_H
