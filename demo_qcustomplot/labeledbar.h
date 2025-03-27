#ifndef LABELEDBAR_H
#define LABELEDBAR_H

#include "qcustomplot.h"

// 继承QCPBars，添加显示数值的功能
class LabeledBars : public QCPBars
{
public:
    // 构造函数
    LabeledBars(QCPAxis *keyAxis, QCPAxis *valueAxis);

    // 设置是否显示标签
    void setShowLabels(bool show);
    bool isShowingLabels() const;

    // 设置标签字体和颜色
    void setLabelFont(const QFont &font);
    void setLabelColor(const QColor &color);

    // 设置标签格式（参考QString::number的格式）
    void setLabelFormat(char format, int precision);

protected:
    // 重写绘制函数，添加数值标签
    virtual void draw(QCPPainter *painter) override;

private:
    bool   m_showLabels;
    QFont  m_labelFont;
    QColor m_labelColor;
    char   m_labelFormat;
    int    m_labelPrecision;
};

#endif // LABELEDBAR_H
