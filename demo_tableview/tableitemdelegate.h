#ifndef TABLEITEMDELEGATE_H
#define TABLEITEMDELEGATE_H

#include <QStyledItemDelegate>
#include "tableitem.h"

/**
 * @brief 表格项代理类，用于自定义单元格的显示和编辑方式
 */
class TableItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit TableItemDelegate(QObject *parent = nullptr);

    // 重载显示和编辑方法
    void paint(QPainter *painter, const QStyleOptionViewItem &option, 
               const QModelIndex &index) const override;
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, 
                        const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, 
                     const QModelIndex &index) const override;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, 
                            const QModelIndex &index) const override;
    QString displayText(const QVariant &value, const QLocale &locale) const override;

private:
    // 绘制自定义单元格
    void paintPriceCell(QPainter *painter, const QStyleOptionViewItem &option, 
                      const QModelIndex &index) const;
    void paintDateCell(QPainter *painter, const QStyleOptionViewItem &option, 
                     const QModelIndex &index) const;
    void paintAvailableCell(QPainter *painter, const QStyleOptionViewItem &option, 
                          const QModelIndex &index) const;
};

#endif // TABLEITEMDELEGATE_H
