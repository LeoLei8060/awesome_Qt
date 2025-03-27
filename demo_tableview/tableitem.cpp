#include "tableitem.h"

TableItem::TableItem()
    : m_id(0), m_name(""), m_category(""), m_date(QDate::currentDate()), 
      m_price(0.0), m_available(false)
{
}

TableItem::TableItem(int id, const QString &name, const QString &category, 
                     const QDate &date, double price, bool available)
    : m_id(id), m_name(name), m_category(category), m_date(date), 
      m_price(price), m_available(available)
{
}

TableItem::~TableItem()
{
}

int TableItem::getId() const
{
    return m_id;
}

QString TableItem::getName() const
{
    return m_name;
}

QString TableItem::getCategory() const
{
    return m_category;
}

QDate TableItem::getDate() const
{
    return m_date;
}

double TableItem::getPrice() const
{
    return m_price;
}

bool TableItem::isAvailable() const
{
    return m_available;
}

void TableItem::setId(int id)
{
    m_id = id;
}

void TableItem::setName(const QString &name)
{
    m_name = name;
}

void TableItem::setCategory(const QString &category)
{
    m_category = category;
}

void TableItem::setDate(const QDate &date)
{
    m_date = date;
}

void TableItem::setPrice(double price)
{
    m_price = price;
}

void TableItem::setAvailable(bool available)
{
    m_available = available;
}

QVariant TableItem::data(int column) const
{
    switch (column) {
    case IdColumn:
        return m_id;
    case NameColumn:
        return m_name;
    case CategoryColumn:
        return m_category;
    case DateColumn:
        return m_date;
    case PriceColumn:
        return m_price;
    case AvailableColumn:
        return m_available;
    default:
        return QVariant();
    }
}

bool TableItem::setData(int column, const QVariant &value)
{
    switch (column) {
    case IdColumn:
        if (value.canConvert<int>()) {
            m_id = value.toInt();
            return true;
        }
        break;
    case NameColumn:
        if (value.canConvert<QString>()) {
            m_name = value.toString();
            return true;
        }
        break;
    case CategoryColumn:
        if (value.canConvert<QString>()) {
            m_category = value.toString();
            return true;
        }
        break;
    case DateColumn:
        if (value.canConvert<QDate>()) {
            m_date = value.toDate();
            return true;
        }
        break;
    case PriceColumn:
        if (value.canConvert<double>()) {
            m_price = value.toDouble();
            return true;
        }
        break;
    case AvailableColumn:
        if (value.canConvert<bool>()) {
            m_available = value.toBool();
            return true;
        }
        break;
    }
    return false;
}

QString TableItem::columnName(int column)
{
    switch (column) {
    case IdColumn:
        return QStringLiteral("ID");
    case NameColumn:
        return QStringLiteral("名称");
    case CategoryColumn:
        return QStringLiteral("类别");
    case DateColumn:
        return QStringLiteral("日期");
    case PriceColumn:
        return QStringLiteral("价格");
    case AvailableColumn:
        return QStringLiteral("可用");
    default:
        return QString();
    }
}
