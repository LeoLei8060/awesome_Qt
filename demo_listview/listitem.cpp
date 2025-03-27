#include "listitem.h"

ListItem::ListItem()
    : m_name("")
    , m_date(QDateTime::currentDateTime())
    , m_size(0)
    , m_iconPath("")
{}

ListItem::ListItem(const QString &name, const QDateTime &date, int size, const QString &iconPath)
    : m_name(name)
    , m_date(date)
    , m_size(size)
    , m_iconPath(iconPath)
{}

QString ListItem::getName() const
{
    return m_name;
}

void ListItem::setName(const QString &name)
{
    m_name = name;
}

QDateTime ListItem::getDate() const
{
    return m_date;
}

void ListItem::setDate(const QDateTime &date)
{
    m_date = date;
}

int ListItem::getSize() const
{
    return m_size;
}

void ListItem::setSize(int size)
{
    m_size = size;
}

QString ListItem::getIconPath() const
{
    return m_iconPath;
}

void ListItem::setIconPath(const QString &iconPath)
{
    m_iconPath = iconPath;
}

QIcon ListItem::getIcon() const
{
    if (m_iconPath.isEmpty()) {
        return QIcon(); // 返回空图标
    }
    return QIcon(m_iconPath);
}
