#include "tableitemdelegate.h"
#include "tableitem.h"
#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QDateEdit>
#include <QDateTime>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QLocale>
#include <QPainter>

TableItemDelegate::TableItemDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{}

void TableItemDelegate::paint(QPainter                   *painter,
                              const QStyleOptionViewItem &option,
                              const QModelIndex          &index) const
{
    int column = index.column();

    switch (column) {
    case TableItem::PriceColumn:
        paintPriceCell(painter, option, index);
        break;
    case TableItem::DateColumn:
        paintDateCell(painter, option, index);
        break;
    case TableItem::AvailableColumn:
        paintAvailableCell(painter, option, index);
        break;
    default:
        QStyledItemDelegate::paint(painter, option, index);
        break;
    }
}

QWidget *TableItemDelegate::createEditor(QWidget                    *parent,
                                         const QStyleOptionViewItem &option,
                                         const QModelIndex          &index) const
{
    int column = index.column();

    switch (column) {
    case TableItem::NameColumn:
    case TableItem::CategoryColumn: {
        QLineEdit *editor = new QLineEdit(parent);
        // 如果是类别列，设置最大长度
        if (column == TableItem::CategoryColumn) {
            editor->setMaxLength(20);
        }
        return editor;
    }
    case TableItem::DateColumn: {
        QDateEdit *editor = new QDateEdit(parent);
        editor->setCalendarPopup(true);
        editor->setDisplayFormat("yyyy-MM-dd");
        editor->setDate(QDate::currentDate());
        editor->setMinimumDate(QDate(2000, 1, 1));
        editor->setMaximumDate(QDate::currentDate().addYears(1));
        return editor;
    }
    case TableItem::PriceColumn: {
        QDoubleSpinBox *editor = new QDoubleSpinBox(parent);
        editor->setRange(0.0, 10000);
        editor->setDecimals(2);
        editor->setSingleStep(1.0);
        editor->setPrefix("¥ ");
        return editor;
    }
    case TableItem::AvailableColumn: {
        QCheckBox *editor = new QCheckBox(parent);
        editor->setStyleSheet("QCheckBox { margin: 0px; padding: 0px; }");
        return editor;
    }
    default:
        return QStyledItemDelegate::createEditor(parent, option, index);
    }
}

void TableItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    int      column = index.column();
    QVariant value = index.model()->data(index, Qt::EditRole);

    switch (column) {
    case TableItem::NameColumn:
    case TableItem::CategoryColumn: {
        QLineEdit *lineEdit = qobject_cast<QLineEdit *>(editor);
        if (lineEdit) {
            lineEdit->setText(value.toString());
        }
        break;
    }
    case TableItem::DateColumn: {
        QDateEdit *dateEdit = qobject_cast<QDateEdit *>(editor);
        if (dateEdit) {
            dateEdit->setDate(value.toDate());
        }
        break;
    }
    case TableItem::PriceColumn: {
        QDoubleSpinBox *spinBox = qobject_cast<QDoubleSpinBox *>(editor);
        if (spinBox) {
            spinBox->setValue(value.toDouble());
        }
        break;
    }
    case TableItem::AvailableColumn: {
        QCheckBox *checkBox = qobject_cast<QCheckBox *>(editor);
        if (checkBox) {
            checkBox->setChecked(value.toBool());
        }
        break;
    }
    default:
        QStyledItemDelegate::setEditorData(editor, index);
        break;
    }
}

void TableItemDelegate::setModelData(QWidget            *editor,
                                     QAbstractItemModel *model,
                                     const QModelIndex  &index) const
{
    int column = index.column();

    switch (column) {
    case TableItem::NameColumn:
    case TableItem::CategoryColumn: {
        QLineEdit *lineEdit = qobject_cast<QLineEdit *>(editor);
        if (lineEdit) {
            model->setData(index, lineEdit->text(), Qt::EditRole);
        }
        break;
    }
    case TableItem::DateColumn: {
        QDateEdit *dateEdit = qobject_cast<QDateEdit *>(editor);
        if (dateEdit) {
            model->setData(index, dateEdit->date(), Qt::EditRole);
        }
        break;
    }
    case TableItem::PriceColumn: {
        QDoubleSpinBox *spinBox = qobject_cast<QDoubleSpinBox *>(editor);
        if (spinBox) {
            model->setData(index, spinBox->value(), Qt::EditRole);
        }
        break;
    }
    case TableItem::AvailableColumn: {
        QCheckBox *checkBox = qobject_cast<QCheckBox *>(editor);
        if (checkBox) {
            model->setData(index, checkBox->isChecked(), Qt::EditRole);
        }
        break;
    }
    default:
        QStyledItemDelegate::setModelData(editor, model, index);
        break;
    }
}

void TableItemDelegate::updateEditorGeometry(QWidget                    *editor,
                                             const QStyleOptionViewItem &option,
                                             const QModelIndex          &index) const
{
    Q_UNUSED(index)
    editor->setGeometry(option.rect);
}

QString TableItemDelegate::displayText(const QVariant &value, const QLocale &locale) const
{
    if (value.userType() == QMetaType::Double) {
        return locale.toCurrencyString(value.toDouble());
    } else if (value.userType() == QMetaType::QDate) {
        return locale.toString(value.toDate(), QLocale::ShortFormat);
    }

    return QStyledItemDelegate::displayText(value, locale);
}

void TableItemDelegate::paintPriceCell(QPainter                   *painter,
                                       const QStyleOptionViewItem &option,
                                       const QModelIndex          &index) const
{
    painter->save();

    // 绘制选中背景
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    // 绘制背景
    QStyle *style = option.widget ? option.widget->style() : QApplication::style();
    style->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, painter, option.widget);

    // 获取价格数据
    double  price = index.data(Qt::EditRole).toDouble();
    QString priceText = QLocale::system().toCurrencyString(price);

    // 设置字体和对齐方式
    QFont font = option.font;
    if (price > 500.0) {
        font.setBold(true);
    }
    painter->setFont(font);

    // 设置文本颜色
    if (price < 50.0) {
        painter->setPen(Qt::red);
    } else if (price > 500.0) {
        painter->setPen(Qt::blue);
    } else {
        painter->setPen(option.palette.text().color());
    }

    // 绘制文本
    QRect textRect = option.rect;
    textRect.adjust(5, 0, -5, 0); // 增加些边距
    painter->drawText(textRect, Qt::AlignRight | Qt::AlignVCenter, priceText);

    painter->restore();
}

void TableItemDelegate::paintDateCell(QPainter                   *painter,
                                      const QStyleOptionViewItem &option,
                                      const QModelIndex          &index) const
{
    painter->save();

    // 绘制选中背景
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    // 绘制背景
    QStyle *style = option.widget ? option.widget->style() : QApplication::style();
    style->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, painter, option.widget);

    // 获取日期数据
    QDate   date = index.data(Qt::EditRole).toDate();
    QString dateText = QLocale::system().toString(date, QLocale::ShortFormat);

    // 绘制日期图标
    int   iconSize = option.rect.height() - 4;
    QRect iconRect(option.rect.left() + 4, option.rect.top() + 2, iconSize, iconSize);
    painter->drawPixmap(iconRect, QIcon::fromTheme("office-calendar").pixmap(iconSize, iconSize));

    // 绘制文本
    QRect textRect = option.rect;
    textRect.setLeft(iconRect.right() + 4);
    painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, dateText);

    painter->restore();
}

void TableItemDelegate::paintAvailableCell(QPainter                   *painter,
                                           const QStyleOptionViewItem &option,
                                           const QModelIndex          &index) const
{
    painter->save();

    // 获取数据
    bool available = index.data(Qt::EditRole).toBool();

    // 绘制背景
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);
    QStyle *style = option.widget ? option.widget->style() : QApplication::style();
    style->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, painter, option.widget);

    // 绘制复选框
    QStyleOptionButton checkBoxStyle;
    checkBoxStyle.state = QStyle::State_Enabled;
    checkBoxStyle.state |= available ? QStyle::State_On : QStyle::State_Off;

    QRect checkBoxRect = style->subElementRect(QStyle::SE_CheckBoxIndicator, &checkBoxStyle);
    checkBoxRect.moveCenter(option.rect.center());
    checkBoxStyle.rect = checkBoxRect;

    style->drawPrimitive(QStyle::PE_IndicatorCheckBox, &checkBoxStyle, painter);

    painter->restore();
}
