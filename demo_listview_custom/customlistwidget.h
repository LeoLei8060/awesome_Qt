#ifndef CUSTOMLISTWIDGET_H
#define CUSTOMLISTWIDGET_H

#include <QColor>
#include <QFont>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QResizeEvent>
#include <QScrollBar>
#include <QTimer>
#include <QVector>
#include <QWheelEvent>
#include <QWidget>

// 列表项数据结构
class ListItem
{
public:
    ListItem(const QString &text = "",
             const QColor  &textColor = Qt::black,
             const QColor  &bgColor = Qt::white,
             const QFont   &font = QFont());
    ~ListItem();

    QString text() const { return m_text; }
    QColor  textColor() const { return m_textColor; }
    QColor  backgroundColor() const { return m_bgColor; }
    QFont   font() const { return m_font; }
    bool    isSelected() const { return m_selected; }

    void setText(const QString &text) { m_text = text; }
    void setTextColor(const QColor &color) { m_textColor = color; }
    void setBackgroundColor(const QColor &color) { m_bgColor = color; }
    void setFont(const QFont &font) { m_font = font; }
    void setSelected(bool selected) { m_selected = selected; }

private:
    QString m_text;
    QColor  m_textColor;
    QColor  m_bgColor;
    QFont   m_font;
    bool    m_selected;
};

// 自定义列表控件
class CustomListWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CustomListWidget(QWidget *parent = nullptr);
    ~CustomListWidget() override;

    // 列表项操作
    void addItem(const QString &text,
                 const QColor  &textColor = Qt::black,
                 const QColor  &bgColor = Qt::white,
                 const QFont   &font = QFont());
    void removeItem(int index);
    void clear();

    // 获取列表项
    int       count() const { return m_items.count(); }
    ListItem *item(int index) const;
    int       currentIndex() const { return m_currentIndex; }

    // 设置/获取项高度
    void setItemHeight(int height);
    int  itemHeight() const { return m_itemHeight; }

    // 设置/获取间距
    void setSpacing(int spacing);
    int  spacing() const { return m_spacing; }

    // 设置选择模式
    enum SelectionMode { SingleSelection, MultiSelection, NoSelection };
    void          setSelectionMode(SelectionMode mode);
    SelectionMode selectionMode() const { return m_selectionMode; }

    // 设置/获取当前项
    void setCurrentIndex(int index);

    // 更新指定项
    void updateItem(int index);

    // 获取可见项范围
    QVector<int> visibleItemRange() const;

public slots:
    // 滚动到指定项
    void scrollToItem(int index);

    // 滚动控制
    void scrollUp();
    void scrollDown();
    void scrollToTop();
    void scrollToBottom();

signals:
    void itemClicked(int index);
    void itemDoubleClicked(int index);
    void currentItemChanged(int current, int previous);
    void itemSelectionChanged();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    // 计算点击的项索引
    int itemAt(const QPoint &point) const;

    // 更新滚动条
    void updateScrollBar();

    // 绘制列表项
    void drawItem(QPainter *painter, const QRect &rect, const ListItem *item, bool isSelected);

private:
    QVector<ListItem *> m_items;             // 列表项
    int                 m_itemHeight;        // 项高度
    int                 m_spacing;           // 项间距
    int                 m_currentIndex;      // 当前选中项
    int                 m_scrollOffset;      // 滚动偏移
    QScrollBar         *m_scrollBar;         // 滚动条
    SelectionMode       m_selectionMode;     // 选择模式
    QColor              m_selectedBgColor;   // 选中项背景色
    QColor              m_selectedTextColor; // 选中项文本色
    QColor              m_hoverBgColor;      // 悬停项背景色
    int                 m_hoverIndex;        // 悬停项索引
    QVector<int>        m_selectedItems;     // 多选模式下选中的项
};

#endif // CUSTOMLISTWIDGET_H
