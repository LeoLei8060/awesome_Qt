#ifndef LISTITEMDELEGATE_H
#define LISTITEMDELEGATE_H

#include <QStyledItemDelegate>

class ListItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit ListItemDelegate(QObject *parent = nullptr);

    // 重写绘制方法
    void  paint(QPainter                   *painter,
                const QStyleOptionViewItem &option,
                const QModelIndex          &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    // 编辑器相关
    QWidget *createEditor(QWidget                    *parent,
                          const QStyleOptionViewItem &option,
                          const QModelIndex          &index) const override;
    void     setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void     setModelData(QWidget            *editor,
                          QAbstractItemModel *model,
                          const QModelIndex  &index) const override;
    void     updateEditorGeometry(QWidget                    *editor,
                                  const QStyleOptionViewItem &option,
                                  const QModelIndex          &index) const override;
};

#endif // LISTITEMDELEGATE_H
