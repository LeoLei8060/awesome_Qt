#ifndef WIDGET_H
#define WIDGET_H

#include <QVector>
#include <QWidget>

namespace Ui {
class Widget;
}

class QCustomPlot;
class QCPGraph;
class LabeledBars;

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

private:
    void setupTab1();
    void setupTab2();
    void setupTabWidget();

private:
    Ui::Widget *ui;

    QVector<QCPGraph *> m_graphs;
    QVector<LabeledBars *>  m_bars;
    QVector<double>     m_t1Data;
    QVector<double>     m_t2Data;
    QVector<double>     m_t3Data;
    QStringList         m_tips;
};

#endif // WIDGET_H
