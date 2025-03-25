#include "labeledbar.h"

LabeledBars::LabeledBars(QCPAxis *keyAxis, QCPAxis *valueAxis)
    : QCPBars(keyAxis, valueAxis)
    , m_showLabels(false)
    , m_labelFont(QFont("Arial", 8))
    , m_labelColor(Qt::black)
    , m_labelFormat('f')
    , m_labelPrecision(1)
{
}

void LabeledBars::setShowLabels(bool show)
{
    m_showLabels = show;
}

bool LabeledBars::isShowingLabels() const
{
    return m_showLabels;
}

void LabeledBars::setLabelFont(const QFont &font)
{
    m_labelFont = font;
}

void LabeledBars::setLabelColor(const QColor &color)
{
    m_labelColor = color;
}

void LabeledBars::setLabelFormat(char format, int precision)
{
    m_labelFormat = format;
    m_labelPrecision = precision;
}

void LabeledBars::draw(QCPPainter *painter)
{
    // 先调用基类的绘制方法，绘制柱子
    QCPBars::draw(painter);
    
    // 如果不显示标签，则不绘制
    if (!m_showLabels || mDataContainer->isEmpty())
        return;
    
    // 设置画笔、画刷和字体
    painter->setPen(QPen(m_labelColor));
    painter->setBrush(Qt::NoBrush);
    painter->setFont(m_labelFont);
    
    // 获取坐标轴的范围
    double lowerKey = mKeyAxis->range().lower;
    double upperKey = mKeyAxis->range().upper;
    
    // 遍历每个数据点
    for (int i = 0; i < mDataContainer->size(); ++i)
    {
        // 获取数据点
        QCPBarsDataContainer::const_iterator it = mDataContainer->constBegin() + i;
        double key = it->key;
        double value = it->value;
        
        // 检查是否在可见范围内
        if (key < lowerKey || key > upperKey)
            continue;
        
        // 计算柱子的位置和宽度
        double width = mWidth;
        double keyPixel = mKeyAxis->coordToPixel(key);
        double valuePixel = mValueAxis->coordToPixel(value);
        double baseValuePixel = mValueAxis->coordToPixel(qMin(mBaseValue, (double)0));
        
        // 文本内容 - 根据格式和精度进行格式化
        QString text = QString::number(value, m_labelFormat, m_labelPrecision);
        
        // 计算文本的宽度和高度
        QFontMetrics fontMetrics(m_labelFont);
        QSize textSize = fontMetrics.size(Qt::TextSingleLine, text);
        
        // 计算文本的位置 - 在柱子顶部居中显示
        double textX = keyPixel - textSize.width() / 2.0;
        double textY;
        
        // 根据柱子方向确定文本位置
        if (value >= 0)
            textY = valuePixel - textSize.height() - 2; // 正值，在柱子上方显示
        else
            textY = valuePixel + 2; // 负值，在柱子下方显示
        
        // 绘制文本
        painter->drawText(textX, textY, textSize.width(), textSize.height(), Qt::AlignCenter, text);
    }
}
