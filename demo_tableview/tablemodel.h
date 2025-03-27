#ifndef TABLEMODEL_H
#define TABLEMODEL_H

#include <QAbstractTableModel>
#include <QList>
#include <QColor>
#include <QBrush>
#include "tableitem.h"

/**
 * @brief 表格模型类，用于管理表格数据并提供给QTableView显示
 */
class TableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit TableModel(QObject *parent = nullptr);
    ~TableModel();

    // 重载QAbstractTableModel必要的虚函数
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    // 筛选和排序所需的方法
    void addItem(const TableItem &item);
    void addItems(const QList<TableItem> &items);
    void removeItem(int row);
    void clearItems();
    TableItem getItem(int row) const;
    QList<TableItem> getAllItems() const;
    TableItem *getItemPtr(int row);
    
    // 生成测试数据
    void generateTestData(int count = 50);

    // 数据保存与加载方法
    bool saveToFile(const QString &filename);
    bool loadFromFile(const QString &filename);

private:
    QList<TableItem> m_items;  // 存储表格数据
    
    // 获取单元格背景色
    QBrush getCellBackgroundColor(int row, int column) const;
    
    // 格式化数据显示
    QString formatPrice(double price) const;
    QString formatDate(const QDate &date) const;
};

#endif // TABLEMODEL_H
