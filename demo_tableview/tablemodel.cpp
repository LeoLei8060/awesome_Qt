#include "tablemodel.h"
#include <QColor>
#include <QDataStream>
#include <QDebug>
#include <QFile>
#include <QLocale>
#include <QRandomGenerator>

TableModel::TableModel(QObject *parent)
    : QAbstractTableModel(parent)
{}

TableModel::~TableModel() {}

int TableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_items.count();
}

int TableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return TableItem::ColumnCount;
}

QVariant TableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_items.count())
        return QVariant();

    const TableItem &item = m_items.at(index.row());

    switch (role) {
    case Qt::DisplayRole:
        switch (index.column()) {
        case TableItem::IdColumn:
            return item.getId();
        case TableItem::NameColumn:
            return item.getName();
        case TableItem::CategoryColumn:
            return item.getCategory();
        case TableItem::DateColumn:
            return formatDate(item.getDate());
        case TableItem::PriceColumn:
            return formatPrice(item.getPrice());
        case TableItem::AvailableColumn:
            return item.isAvailable() ? QStringLiteral("是") : QStringLiteral("否");
        default:
            return QVariant();
        }

    case Qt::EditRole:
        return item.data(index.column());

    case Qt::TextAlignmentRole:
        switch (index.column()) {
        case TableItem::IdColumn:
        case TableItem::PriceColumn:
            return int(Qt::AlignRight | Qt::AlignVCenter);
        case TableItem::AvailableColumn:
            return int(Qt::AlignCenter);
        default:
            return int(Qt::AlignLeft | Qt::AlignVCenter);
        }

    case Qt::BackgroundRole:
        return getCellBackgroundColor(index.row(), index.column());

    case Qt::ForegroundRole:
        // 如果不可用，使用灰色文本
        if (index.column() == TableItem::AvailableColumn && !item.isAvailable()) {
            return QColor(Qt::darkGray);
        }
        // 价格低于50时显示红色
        if (index.column() == TableItem::PriceColumn && item.getPrice() < 50.0) {
            return QColor(Qt::red);
        }
        // 价格高于500时显示蓝色
        if (index.column() == TableItem::PriceColumn && item.getPrice() > 500.0) {
            return QColor(Qt::blue);
        }
        return QVariant();

    case Qt::CheckStateRole:
        if (index.column() == TableItem::AvailableColumn) {
            return item.isAvailable() ? Qt::Checked : Qt::Unchecked;
        }
        return QVariant();
    }

    return QVariant();
}

QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal) {
        if (role == Qt::DisplayRole) {
            return TableItem::columnName(section);
        }
    } else if (orientation == Qt::Vertical) {
        if (role == Qt::DisplayRole) {
            return section + 1; // 行号从1开始
        }
    }
    return QVariant();
}

bool TableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || index.row() >= m_items.count())
        return false;

    if (role == Qt::EditRole) {
        TableItem &item = m_items[index.row()];
        if (item.setData(index.column(), value)) {
            emit dataChanged(index, index);
            return true;
        }
    } else if (role == Qt::CheckStateRole && index.column() == TableItem::AvailableColumn) {
        TableItem &item = m_items[index.row()];
        item.setAvailable(value.toInt() == Qt::Checked);
        emit dataChanged(index, index);
        return true;
    }

    return false;
}

Qt::ItemFlags TableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    Qt::ItemFlags flags = QAbstractTableModel::flags(index);

    // 所有列都可编辑，除了ID列
    if (index.column() != TableItem::IdColumn) {
        flags |= Qt::ItemIsEditable;
    }

    // 可用列可以用复选框编辑
    if (index.column() == TableItem::AvailableColumn) {
        flags |= Qt::ItemIsUserCheckable;
    }

    return flags;
}

void TableModel::addItem(const TableItem &item)
{
    beginInsertRows(QModelIndex(), m_items.count(), m_items.count());
    m_items.append(item);
    endInsertRows();
}

void TableModel::addItems(const QList<TableItem> &items)
{
    if (items.isEmpty())
        return;

    beginInsertRows(QModelIndex(), m_items.count(), m_items.count() + items.count() - 1);
    m_items.append(items);
    endInsertRows();
}

void TableModel::removeItem(int row)
{
    if (row < 0 || row >= m_items.count())
        return;

    beginRemoveRows(QModelIndex(), row, row);
    m_items.removeAt(row);
    endRemoveRows();
}

void TableModel::clearItems()
{
    beginResetModel();
    m_items.clear();
    endResetModel();
}

TableItem TableModel::getItem(int row) const
{
    if (row >= 0 && row < m_items.count())
        return m_items.at(row);
    return TableItem();
}

QList<TableItem> TableModel::getAllItems() const
{
    return m_items;
}

TableItem *TableModel::getItemPtr(int row)
{
    if (row >= 0 && row < m_items.count())
        return &m_items[row];
    return nullptr;
}

void TableModel::generateTestData(int count)
{
    beginResetModel();
    m_items.clear();

    QStringList categories = {QStringLiteral("电子产品"),
                              QStringLiteral("家居用品"),
                              QStringLiteral("办公用品"),
                              QStringLiteral("户外装备"),
                              QStringLiteral("食品饮料"),
                              QStringLiteral("图书音像")};

    QStringList names = {QStringLiteral("笔记本电脑"),
                         QStringLiteral("智能手机"),
                         QStringLiteral("平板电脑"),
                         QStringLiteral("咖啡杯"),
                         QStringLiteral("办公桌"),
                         QStringLiteral("座椅"),
                         QStringLiteral("订书机"),
                         QStringLiteral("文件夹"),
                         QStringLiteral("笔筒"),
                         QStringLiteral("帐篷"),
                         QStringLiteral("睡袋"),
                         QStringLiteral("背包"),
                         QStringLiteral("巧克力"),
                         QStringLiteral("矿泉水"),
                         QStringLiteral("果汁"),
                         QStringLiteral("小说"),
                         QStringLiteral("编程书籍"),
                         QStringLiteral("影片光盘")};

    QDate currentDate = QDate::currentDate();

    for (int i = 0; i < count; ++i) {
        int     id = i + 1;
        QString name = names.at(QRandomGenerator::global()->bounded(names.size()));
        QString category = categories.at(QRandomGenerator::global()->bounded(categories.size()));
        QDate   date = currentDate.addDays(-QRandomGenerator::global()->bounded(365));
        double  price = QRandomGenerator::global()->bounded(1000)
                       + QRandomGenerator::global()->bounded(100) / 100.0;
        bool available = QRandomGenerator::global()->bounded(2) > 0;

        m_items.append(TableItem(id, name, category, date, price, available));
    }

    endResetModel();
}

bool TableModel::saveToFile(const QString &filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "无法打开文件进行写入:" << filename;
        return false;
    }

    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_5_15);

    // 写入项目数量
    int count = m_items.count();
    out << count;

    // 依次写入每个项目
    for (const TableItem &item : m_items) {
        out << item.getId() << item.getName() << item.getCategory() << item.getDate()
            << item.getPrice() << item.isAvailable();
    }

    file.close();
    return true;
}

bool TableModel::loadFromFile(const QString &filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "无法打开文件进行读取:" << filename;
        return false;
    }

    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_5_15);

    // 读取项目数量
    int count;
    in >> count;

    // 清空当前数据
    beginResetModel();
    m_items.clear();

    // 依次读取每个项目
    for (int i = 0; i < count; ++i) {
        int     id;
        QString name, category;
        QDate   date;
        double  price;
        bool    available;

        in >> id >> name >> category >> date >> price >> available;

        m_items.append(TableItem(id, name, category, date, price, available));
    }

    endResetModel();
    file.close();
    return true;
}

QBrush TableModel::getCellBackgroundColor(int row, int column) const
{
    // 交替行背景色
    if (row % 2 == 0) {
        return QBrush(QColor(240, 240, 240));
    }

    // 特定列背景色
    if (column == TableItem::CategoryColumn) {
        const TableItem &item = m_items.at(row);
        QString          category = item.getCategory();

        if (category == QStringLiteral("电子产品")) {
            return QBrush(QColor(230, 255, 230));
        } else if (category == QStringLiteral("家居用品")) {
            return QBrush(QColor(230, 230, 255));
        } else if (category == QStringLiteral("办公用品")) {
            return QBrush(QColor(255, 230, 230));
        }
    }

    return QBrush();
}

QString TableModel::formatPrice(double price) const
{
    return QLocale::system().toCurrencyString(price);
}

QString TableModel::formatDate(const QDate &date) const
{
    return QLocale::system().toString(date, QLocale::ShortFormat);
}
