#ifndef LISTITEM_H
#define LISTITEM_H

#include <QDateTime>
#include <QIcon>
#include <QString>

class ListItem
{
public:
    ListItem();
    ListItem(const QString   &name,
             const QDateTime &date,
             int              size,
             const QString   &iconPath = QString("://res/1.png"));

    // 获取和设置名称
    QString getName() const;
    void    setName(const QString &name);

    // 获取和设置日期
    QDateTime getDate() const;
    void      setDate(const QDateTime &date);

    // 获取和设置大小
    int  getSize() const;
    void setSize(int size);

    // 获取和设置图标路径
    QString getIconPath() const;
    void    setIconPath(const QString &iconPath);

    // 获取图标对象
    QIcon getIcon() const;

private:
    QString   m_name;
    QDateTime m_date;
    int       m_size;
    QString   m_iconPath;
};

#endif // LISTITEM_H
