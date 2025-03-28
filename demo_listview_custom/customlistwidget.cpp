#include "customlistwidget.h"
#include <QDebug>
#include <QPainter>

// ListItem实现
ListItem::ListItem(const QString &text,
                   const QColor  &textColor,
                   const QColor  &bgColor,
                   const QFont   &font)
    : m_text(text)
    , m_textColor(textColor)
    , m_bgColor(bgColor)
    , m_font(font)
    , m_selected(false)
{}

ListItem::~ListItem() {}

// CustomListWidget实现
CustomListWidget::CustomListWidget(QWidget *parent)
    : QWidget(parent)
    , m_itemHeight(40)
    , m_spacing(2)
    , m_currentIndex(-1)
    , m_scrollOffset(0)
    , m_selectionMode(SingleSelection)
    , m_selectedBgColor(QColor(51, 153, 255))
    , m_selectedTextColor(Qt::white)
    , m_hoverBgColor(QColor(230, 230, 230))
    , m_hoverIndex(-1)
{
    // 设置属性
    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_OpaquePaintEvent);
    setMouseTracking(true);

    // 创建滚动条
    m_scrollBar = new QScrollBar(Qt::Vertical, this);
    m_scrollBar->setRange(0, 0);
    m_scrollBar->setPageStep(5);
    m_scrollBar->setSingleStep(1);

    // 连接滚动条信号
    connect(m_scrollBar, &QScrollBar::valueChanged, this, [this](int value) {
        m_scrollOffset = value;
        update();
    });

    // 调整滚动条位置
    updateScrollBar();
}

CustomListWidget::~CustomListWidget()
{
    clear();
}

void CustomListWidget::addItem(const QString &text,
                               const QColor  &textColor,
                               const QColor  &bgColor,
                               const QFont   &font)
{
    ListItem *item = new ListItem(text, textColor, bgColor, font);
    m_items.append(item);

    // 如果是第一个项且当前没有选中项，则选中它
    if (m_items.count() == 1 && m_currentIndex == -1) {
        setCurrentIndex(0);
    }

    // 更新滚动条
    updateScrollBar();
    update();
}

void CustomListWidget::removeItem(int index)
{
    if (index < 0 || index >= m_items.count())
        return;

    // 删除项
    delete m_items.takeAt(index);

    // 更新当前索引
    if (m_currentIndex == index) {
        if (m_items.isEmpty()) {
            m_currentIndex = -1;
        } else if (m_currentIndex >= m_items.count()) {
            m_currentIndex = m_items.count() - 1;
        }
        emit currentItemChanged(m_currentIndex, index);
    } else if (m_currentIndex > index) {
        m_currentIndex--;
    }

    // 更新多选模式下的选中项
    for (int i = 0; i < m_selectedItems.count(); i++) {
        if (m_selectedItems[i] == index) {
            m_selectedItems.removeAt(i);
            i--;
        } else if (m_selectedItems[i] > index) {
            m_selectedItems[i]--;
        }
    }

    // 更新滚动条
    updateScrollBar();
    update();
}

void CustomListWidget::clear()
{
    // 清空所有项
    for (ListItem *item : m_items) {
        delete item;
    }
    m_items.clear();
    m_selectedItems.clear();

    // 重置状态
    m_currentIndex = -1;
    m_scrollOffset = 0;
    m_hoverIndex = -1;

    // 更新滚动条
    updateScrollBar();
    update();
}

ListItem *CustomListWidget::item(int index) const
{
    if (index < 0 || index >= m_items.count())
        return nullptr;

    return m_items.at(index);
}

void CustomListWidget::setItemHeight(int height)
{
    if (height < 1)
        return;

    m_itemHeight = height;
    updateScrollBar();
    update();
}

void CustomListWidget::setSpacing(int spacing)
{
    if (spacing < 0)
        return;

    m_spacing = spacing;
    updateScrollBar();
    update();
}

void CustomListWidget::setSelectionMode(SelectionMode mode)
{
    m_selectionMode = mode;

    if (m_selectionMode == SingleSelection) {
        m_selectedItems.clear();
        if (m_currentIndex >= 0 && m_currentIndex < m_items.count()) {
            m_selectedItems.append(m_currentIndex);
        }
    } else if (m_selectionMode == NoSelection) {
        m_selectedItems.clear();
        m_currentIndex = -1;
    }

    update();
}

void CustomListWidget::setCurrentIndex(int index)
{
    if (index < -1 || index >= m_items.count() || m_currentIndex == index)
        return;

    // 保存先前索引
    int previousIndex = m_currentIndex;
    m_currentIndex = index;

    // 更新选中状态
    if (m_selectionMode == SingleSelection) {
        m_selectedItems.clear();
        if (index != -1) {
            m_selectedItems.append(index);
        }
    }

    // 自动滚动到当前项
    if (index != -1) {
        scrollToItem(index);
    }

    // 发送信号
    emit currentItemChanged(m_currentIndex, previousIndex);
    emit itemSelectionChanged();

    update();
}

void CustomListWidget::updateItem(int index)
{
    if (index < 0 || index >= m_items.count())
        return;

    // 判断项是否可见
    QVector<int> visibleRange = visibleItemRange();
    if (index >= visibleRange[0] && index <= visibleRange[1]) {
        // 计算项的区域
        int   y = (index - m_scrollOffset) * (m_itemHeight + m_spacing);
        QRect itemRect(0, y, width() - m_scrollBar->width(), m_itemHeight);
        update(itemRect);
    }
}

QVector<int> CustomListWidget::visibleItemRange() const
{
    QVector<int> range;

    // 计算可见的第一个和最后一个项的索引
    int firstVisible = m_scrollOffset;
    int lastVisible = m_scrollOffset + (height() / (m_itemHeight + m_spacing));

    // 确保范围有效
    firstVisible = qMax(0, firstVisible);
    lastVisible = qMin(m_items.count() - 1, lastVisible);

    range.append(firstVisible);
    range.append(lastVisible);

    return range;
}

void CustomListWidget::scrollToItem(int index)
{
    if (index < 0 || index >= m_items.count())
        return;

    // 计算可见项范围
    QVector<int> visibleRange = visibleItemRange();
    int          firstVisible = visibleRange[0];
    int          lastVisible = visibleRange[1];

    // 如果项在可见范围内，无需滚动
    if (index >= firstVisible && index <= lastVisible)
        return;

    // 滚动到项
    if (index < firstVisible) {
        m_scrollBar->setValue(index);
    } else {
        int visibleItems = (height() / (m_itemHeight + m_spacing));
        m_scrollBar->setValue(index - visibleItems + 1);
    }
}

void CustomListWidget::scrollUp()
{
    m_scrollBar->setValue(m_scrollBar->value() - m_scrollBar->singleStep());
}

void CustomListWidget::scrollDown()
{
    m_scrollBar->setValue(m_scrollBar->value() + m_scrollBar->singleStep());
}

void CustomListWidget::scrollToTop()
{
    m_scrollBar->setValue(m_scrollBar->minimum());
}

void CustomListWidget::scrollToBottom()
{
    m_scrollBar->setValue(m_scrollBar->maximum());
}

void CustomListWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 绘制背景
    painter.fillRect(rect(), Qt::white);

    // 计算可见的项范围
    QVector<int> visibleRange = visibleItemRange();
    int          firstVisible = visibleRange[0];
    int          lastVisible = visibleRange[1];

    // 绘制可见的项
    for (int i = firstVisible; i <= lastVisible && i < m_items.count(); i++) {
        // 计算项的位置
        int   y = (i - m_scrollOffset) * (m_itemHeight + m_spacing);
        QRect itemRect(0, y, width() - m_scrollBar->width(), m_itemHeight);

        // 判断项是否选中
        bool isSelected = m_selectedItems.contains(i);

        // 悬停效果
        bool isHovered = (i == m_hoverIndex);

        // 绘制项
        drawItem(&painter, itemRect, m_items[i], isSelected);

        // 悬停效果
        if (isHovered && !isSelected) {
            painter.fillRect(itemRect,
                             QColor(m_hoverBgColor.red(),
                                    m_hoverBgColor.green(),
                                    m_hoverBgColor.blue(),
                                    128));
        }
    }
}

void CustomListWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        int index = itemAt(event->pos());
        if (index != -1) {
            if (m_selectionMode == MultiSelection) {
                // 多选模式
                if (event->modifiers() & Qt::ControlModifier) {
                    // Ctrl键按下，切换选中状态
                    if (m_selectedItems.contains(index)) {
                        m_selectedItems.removeOne(index);
                    } else {
                        m_selectedItems.append(index);
                    }
                } else if (event->modifiers() & Qt::ShiftModifier) {
                    // Shift键按下，范围选择
                    if (m_currentIndex != -1) {
                        int start = qMin(m_currentIndex, index);
                        int end = qMax(m_currentIndex, index);
                        for (int i = start; i <= end; i++) {
                            if (!m_selectedItems.contains(i)) {
                                m_selectedItems.append(i);
                            }
                        }
                    } else {
                        m_selectedItems.append(index);
                    }
                } else {
                    // 普通点击，清除之前的选择并选中当前项
                    m_selectedItems.clear();
                    m_selectedItems.append(index);
                }

                // 设置当前索引
                setCurrentIndex(index);
            } else if (m_selectionMode == SingleSelection) {
                // 单选模式
                setCurrentIndex(index);
            }

            // 发送点击信号
            emit itemClicked(index);

            // 如果是多选模式，还需要发送选择变更信号
            if (m_selectionMode == MultiSelection) {
                emit itemSelectionChanged();
            }

            update();
        }
    }

    QWidget::mousePressEvent(event);
}

void CustomListWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        int index = itemAt(event->pos());
        if (index != -1) {
            emit itemDoubleClicked(index);
        }
    }

    QWidget::mouseDoubleClickEvent(event);
}

void CustomListWidget::mouseMoveEvent(QMouseEvent *event)
{
    // 更新悬停索引
    int index = itemAt(event->pos());
    if (index != m_hoverIndex) {
        m_hoverIndex = index;
        update();
    }

    QWidget::mouseMoveEvent(event);
}

void CustomListWidget::wheelEvent(QWheelEvent *event)
{
    // 处理滚轮事件
    QPoint angleDelta = event->angleDelta();
    if (!angleDelta.isNull()) {
        if (angleDelta.y() > 0) {
            scrollUp();
        } else {
            scrollDown();
        }
    }

    event->accept();
}

void CustomListWidget::resizeEvent(QResizeEvent *event)
{
    // 调整滚动条位置
    updateScrollBar();

    QWidget::resizeEvent(event);
}

int CustomListWidget::itemAt(const QPoint &point) const
{
    // 如果点击在滚动条区域，返回-1
    if (point.x() >= width() - m_scrollBar->width()) {
        return -1;
    }

    // 计算索引
    int index = m_scrollOffset + (point.y() / (m_itemHeight + m_spacing));

    // 检查索引是否有效
    if (index < 0 || index >= m_items.count()) {
        return -1;
    }

    return index;
}

void CustomListWidget::updateScrollBar()
{
    // 计算总高度
    int totalHeight = m_items.count() * (m_itemHeight + m_spacing);

    // 设置滚动条范围
    int maxScroll = qMax(0, (totalHeight - height()) / (m_itemHeight + m_spacing) + 1);
    m_scrollBar->setRange(0, maxScroll);

    // 设置页面步长
    int visibleItems = height() / (m_itemHeight + m_spacing);
    m_scrollBar->setPageStep(visibleItems);

    // 调整滚动条位置和大小
    m_scrollBar->setGeometry(width() - m_scrollBar->sizeHint().width(),
                             0,
                             m_scrollBar->sizeHint().width(),
                             height());

    // 显示或隐藏滚动条
    m_scrollBar->setVisible(m_items.count() > visibleItems);
}

void CustomListWidget::drawItem(QPainter       *painter,
                                const QRect    &rect,
                                const ListItem *item,
                                bool            isSelected)
{
    // 绘制背景
    QColor bgColor = isSelected ? m_selectedBgColor : item->backgroundColor();
    painter->fillRect(rect, bgColor);

    // 绘制文本
    QColor textColor = isSelected ? m_selectedTextColor : item->textColor();
    painter->setPen(textColor);
    painter->setFont(item->font());

    // 文本绘制区域（留出边距）
    QRect textRect = rect.adjusted(10, 0, -10, 0);

    // 绘制文本
    painter->drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, item->text());
}
