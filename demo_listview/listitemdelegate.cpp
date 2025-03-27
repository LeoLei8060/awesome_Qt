#include "listitemdelegate.h"
#include "listmodel.h"
#include <QApplication>
#include <QDateTimeEdit>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QListView>
#include <QPainter>
#include <QPushButton>
#include <QSpinBox>

ListItemDelegate::ListItemDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{}

void ListItemDelegate::paint(QPainter                   *painter,
                             const QStyleOptionViewItem &option,
                             const QModelIndex          &index) const
{
    if (!index.isValid())
        return;

    // 获取数据
    QString   name = index.data(ListModel::NameRole).toString();
    QDateTime date = index.data(ListModel::DateRole).toDateTime();
    int       size = index.data(ListModel::SizeRole).toInt();
    QIcon     icon = qvariant_cast<QIcon>(index.data(ListModel::IconRole));

    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    // 判断是否为图标模式
    bool             isIconMode = false;
    const QListView *listView = qobject_cast<const QListView *>(opt.widget);
    if (listView) {
        isIconMode = (listView->viewMode() == QListView::IconMode);
    }

    // 保存绘制状态
    painter->save();

    if (isIconMode) {
        // 图标模式下的绘制

        // 获取整个项目矩形区域
        QRect itemRect = opt.rect;

        // 绘制选中状态或悬停状态的背景
        if (opt.state & QStyle::State_Selected) {
            // 选中状态
            painter->fillRect(itemRect, opt.palette.highlight());
            painter->setPen(opt.palette.highlightedText().color());
        } else if (opt.state & QStyle::State_MouseOver) {
            // 悬停状态
            painter->fillRect(itemRect, opt.palette.highlight().color().lighter(160));
        }

        // 计算图标的大小和位置（占整个区域的上部分，约60%的高度）
        const int iconHeight = itemRect.height() * 0.6;
        const int iconWidth = iconHeight; // 保持正方形

        // 确保图标不会过大
        const int maxIconSize = 64;
        const int iconSize = qMin(iconWidth, maxIconSize);

        // 计算图标位置（水平居中）
        QRect iconRect = QRect(itemRect.left() + (itemRect.width() - iconSize) / 2,
                               itemRect.top() + itemRect.height() * 0.05, // 顶部留5%的空间
                               iconSize,
                               iconSize);

        // 计算文本区域（占整个区域的下部分，约35%的高度）
        // 文本区域从图标底部开始，到项目底部结束，保留5%的底部空间
        QRect textRect = QRect(itemRect.left() + itemRect.width() * 0.05, // 左边留5%空间
                               iconRect.bottom()
                                   + itemRect.height() * 0.05, // 图标和文本之间留5%的空间
                               itemRect.width() * 0.9,         // 宽度为90%
                               itemRect.height() - iconRect.height()
                                   - itemRect.height() * 0.15 // 减去图标和顶部、中间间隔、底部空间
        );

        // 绘制图标
        if (!icon.isNull()) {
            icon.paint(painter, iconRect);
        }

        // 设置字体
        QFont nameFont = painter->font();
        nameFont.setBold(true);

        QFont detailFont = painter->font();
        detailFont.setPointSize(detailFont.pointSize() - 1);

        // 计算文本区域的三等分（名称、日期、大小）
        const int textPartHeight = textRect.height() / 3;

        // 名称区域（第一部分）
        QRect nameRect = QRect(textRect.left(), textRect.top(), textRect.width(), textPartHeight);

        // 日期区域（第二部分）
        QRect dateRect = QRect(textRect.left(), nameRect.bottom(), textRect.width(), textPartHeight);

        // 大小区域（第三部分）
        QRect sizeRect = QRect(textRect.left(), dateRect.bottom(), textRect.width(), textPartHeight);

        // 绘制名称（居中，带省略号）
        painter->setFont(nameFont);
        QString elidedName = painter->fontMetrics().elidedText(name,
                                                               Qt::ElideMiddle,
                                                               nameRect.width());
        painter->drawText(nameRect, Qt::AlignHCenter | Qt::AlignVCenter, elidedName);

        // 绘制详细信息（居中，分两行，带省略号）
        painter->setFont(detailFont);
        if (!(opt.state & QStyle::State_Selected)) {
            painter->setPen(Qt::gray);
        }

        // 日期和大小信息
        QString dateStr = date.toString("yyyy-MM-dd");
        QString sizeStr = QString("%1 KB").arg(size);

        // 使用elidedText确保文本不会超出范围
        QString elidedDateStr = painter->fontMetrics().elidedText(dateStr,
                                                                  Qt::ElideRight,
                                                                  dateRect.width());
        QString elidedSizeStr = painter->fontMetrics().elidedText(sizeStr,
                                                                  Qt::ElideRight,
                                                                  sizeRect.width());

        painter->drawText(dateRect, Qt::AlignHCenter | Qt::AlignVCenter, elidedDateStr);
        painter->drawText(sizeRect, Qt::AlignHCenter | Qt::AlignVCenter, elidedSizeStr);
    } else {
        // 列表模式下的绘制

        // 绘制背景（包括选中状态）
        QStyle *style = opt.widget ? opt.widget->style() : QApplication::style();
        style->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, painter, opt.widget);

        // 计算各区域
        QRect iconRect = QRect(opt.rect.left() + 5, opt.rect.top() + 5, 40, 40);
        QRect textRect = opt.rect.adjusted(iconRect.width() + 10, 5, -5, -5);

        // 绘制图标
        if (!icon.isNull()) {
            icon.paint(painter, iconRect);
        }

        // 设置字体
        QFont nameFont = painter->font();
        nameFont.setBold(true);
        nameFont.setPointSize(nameFont.pointSize() + 1);

        QFont detailFont = painter->font();
        detailFont.setPointSize(detailFont.pointSize() - 1);

        // 绘制名称
        painter->setFont(nameFont);
        painter->drawText(textRect, Qt::AlignLeft | Qt::AlignTop, name);

        // 绘制详细信息
        painter->setFont(detailFont);
        if (!(opt.state & QStyle::State_Selected)) {
            painter->setPen(Qt::gray);
        }

        QString dateStr = date.toString("yyyy-MM-dd hh:mm:ss");
        QString sizeStr = QString("%1 KB").arg(size);
        QString detailText = QString("日期: %1    大小: %2").arg(dateStr, sizeStr);

        QRect detailRect = textRect;
        detailRect.setTop(textRect.top() + 25);

        painter->drawText(detailRect, Qt::AlignLeft | Qt::AlignTop, detailText);
    }

    // 恢复绘制状态
    painter->restore();
}

QSize ListItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index);

    // 判断是否为图标模式
    bool             isIconMode = false;
    const QListView *listView = qobject_cast<const QListView *>(option.widget);
    if (listView) {
        isIconMode = (listView->viewMode() == QListView::IconMode);
    }

    if (isIconMode) {
        // 对于图标模式，返回视图的网格大小
        if (listView && !listView->gridSize().isEmpty()) {
            return listView->gridSize();
        }
        return QSize(125, 125); // 默认大小
    } else {
        return QSize(option.rect.width(), 50);
    }
}

QWidget *ListItemDelegate::createEditor(QWidget                    *parent,
                                        const QStyleOptionViewItem &option,
                                        const QModelIndex          &index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);

    // 根据需求，双击编辑只处理name属性，所以直接返回QLineEdit
    return new QLineEdit(parent);
}

void ListItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    // 设置编辑器数据为当前item的name
    QLineEdit *lineEdit = qobject_cast<QLineEdit *>(editor);
    if (lineEdit) {
        QString name = index.data(ListModel::NameRole).toString();
        lineEdit->setText(name);
    }
}

void ListItemDelegate::setModelData(QWidget            *editor,
                                    QAbstractItemModel *model,
                                    const QModelIndex  &index) const
{
    // 由于只需要处理name属性的编辑，简化逻辑直接处理QLineEdit
    QLineEdit *lineEdit = qobject_cast<QLineEdit *>(editor);
    if (lineEdit) {
        // 先使用EditRole设置数据
        model->setData(index, lineEdit->text(), Qt::EditRole);
        // 再使用NameRole设置数据，确保模型能正确处理
        model->setData(index, lineEdit->text(), ListModel::NameRole);
    } else {
        // 如果不是QLineEdit，则调用父类方法
        QStyledItemDelegate::setModelData(editor, model, index);
    }
}

void ListItemDelegate::updateEditorGeometry(QWidget                    *editor,
                                            const QStyleOptionViewItem &option,
                                            const QModelIndex          &index) const
{
    Q_UNUSED(index);
    editor->setGeometry(option.rect);
}
