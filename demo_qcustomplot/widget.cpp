#include "widget.h"
#include "qcustomplot.h"
#include "ui_widget.h"
#include "labeledbar.h"

#include <random>

#define TIPS_TAB1 "基础的折线图，显示曲线图例，支持鼠标滚轮缩放X轴，支持鼠标左键拖动X轴"
#define TIPS_TAB2 \
    "柱状图，显示三组数据，支持数值的显隐和分组模式的切换（即三组数据分三条显示还是堆叠在一条显示" \
    "）"

double random_double(double min, double max)
{
    // 使用thread_local确保每个线程有独立的随机数生成器
    thread_local std::mt19937 engine(std::random_device{}());
    // 定义均匀分布，生成min到max之间的double值
    std::uniform_real_distribution<double> dist(min, max);
    // 返回生成的随机数
    return dist(engine);
}

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    setupTabWidget();
    ui->label->setText(m_tips[0]);

    connect(ui->tabWidget, &QTabWidget::currentChanged, this, [this](int index) {
        if (index < m_tips.count())
            ui->label->setText(m_tips[index]);
        else
            ui->label->setText("error");
    });
}

Widget::~Widget()
{
    delete ui;
}

void Widget::setupTab1()
{
    const int       x_min = 0, y_min = 0, x_max = 1000, y_max = 100;
    QVector<double> keys, vals;
    for (int i = x_min; i < x_max; ++i) {
        keys << i;
        vals << random_double(y_min, y_max);
    }
    // 设置Tab页标题
    ui->tabWidget->setTabText(0, "折线图");

    // 设置坐标轴
    ui->tab->xAxis->setLabel("Key");
    ui->tab->xAxis->setRange(x_min, x_max);
    ui->tab->yAxis->setLabel("Value");
    ui->tab->yAxis->setRange(y_min, y_max);

    // 显示图例
    ui->tab->legend->setVisible(true);

    ui->tab->setMouseTracking(true);
    // 添加图表对象
    auto graph = ui->tab->addGraph();
    m_graphs.append(graph);

    // 设置线类型
    graph->setLineStyle(QCPGraph::LineStyle::lsLine);
    // 设置数据
    graph->setData(keys, vals);

    // 添加鼠标滚轮缩放功能
    ui->tab->setInteraction(QCP::iRangeDrag, true); // 允许拖动
    ui->tab->setInteraction(QCP::iRangeZoom, true); // 允许缩放

    // 设置只允许X轴拖动
    ui->tab->axisRect()->setRangeDrag(Qt::Horizontal);

    // 设置只允许X轴缩放
    ui->tab->axisRect()->setRangeZoom(Qt::Horizontal);

    // 设置鼠标左键拖动（禁用选择矩形）
    ui->tab->setSelectionRectMode(QCP::SelectionRectMode::srmNone);

    // 连接双击恢复原始视图的信号槽
    connect(ui->tab, &QCustomPlot::mouseDoubleClick, this, [=]() {
        ui->tab->xAxis->setRange(x_min, x_max);
        ui->tab->yAxis->setRange(y_min, y_max);
        ui->tab->replot();
    });
}

void Widget::setupTab2()
{
    // 设置Tab页标题
    ui->tabWidget->setTabText(1, "柱状图");

    // 创建数据 - 三组数据（T1、T2、T3），X轴是1~12月
    QVector<double>  ticks;  // X轴刻度
    QVector<QString> labels; // X轴标签

    // 填充X轴数据（1-12月）
    for (int i = 1; i <= 12; ++i) {
        ticks << i;
        labels << QString("%1月").arg(i);
    }

    // 生成随机数据
    QVector<double> t1Data, t2Data, t3Data;
    for (int i = 0; i < 12; ++i) {
        t1Data << random_double(1, 10);
        t2Data << random_double(1, 10);
        t3Data << random_double(1, 10);
    }

    // 设置X轴
    QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
    textTicker->addTicks(ticks, labels);
    ui->tab2->xAxis->setTicker(textTicker);
    ui->tab2->xAxis->setTickLabelRotation(60); // 旋转标签以避免重叠
    ui->tab2->xAxis->setSubTicks(false);
    ui->tab2->xAxis->setTickLength(0, 4);
    ui->tab2->xAxis->setRange(0, 13);
    ui->tab2->xAxis->setBasePen(QPen(Qt::black));
    ui->tab2->xAxis->setTickPen(QPen(Qt::black));
    ui->tab2->xAxis->grid()->setVisible(false);
    ui->tab2->xAxis->setTickLabelColor(Qt::black);
    ui->tab2->xAxis->setLabelColor(Qt::black);

    // 设置Y轴
    ui->tab2->yAxis->setRange(0, 30);
    ui->tab2->yAxis->setBasePen(QPen(Qt::black));
    ui->tab2->yAxis->setTickPen(QPen(Qt::black));
    ui->tab2->yAxis->setSubTickPen(QPen(Qt::black));
    ui->tab2->yAxis->grid()->setPen(QPen(QColor(130, 130, 130), 0, Qt::DotLine));
    ui->tab2->yAxis->setTickLabelColor(Qt::black);
    ui->tab2->yAxis->setLabelColor(Qt::black);
    ui->tab2->yAxis->setLabel("数值");

    // 创建三个柱状图对象
    LabeledBars *t1Bars = new LabeledBars(ui->tab2->xAxis, ui->tab2->yAxis);
    LabeledBars *t2Bars = new LabeledBars(ui->tab2->xAxis, ui->tab2->yAxis);
    LabeledBars *t3Bars = new LabeledBars(ui->tab2->xAxis, ui->tab2->yAxis);

    // 保存柱状图对象的指针，以便后续访问
    m_bars.append(t1Bars);
    m_bars.append(t2Bars);
    m_bars.append(t3Bars);

    // 设置柱状图属性
    t1Bars->setName("T1");
    t1Bars->setPen(QPen(QColor(0, 168, 140).lighter(130)));
    t1Bars->setBrush(QColor(0, 168, 140));

    t2Bars->setName("T2");
    t2Bars->setPen(QPen(QColor(240, 120, 0).lighter(130)));
    t2Bars->setBrush(QColor(240, 120, 0));

    t3Bars->setName("T3");
    t3Bars->setPen(QPen(QColor(100, 100, 200).lighter(130)));
    t3Bars->setBrush(QColor(100, 100, 200));

    // 配置标签
    for (int i = 0; i < m_bars.size(); ++i) {
        m_bars[i]->setLabelFont(QFont("Arial", 8));
        m_bars[i]->setLabelColor(Qt::black);
        m_bars[i]->setLabelFormat('f', 1);
    }

    // 默认为分组模式，设置柱子的宽度和偏移
    double width = 0.3; // 1/组数
    t1Bars->setWidth(width);
    t2Bars->setWidth(width);
    t3Bars->setWidth(width);

    // 设置柱子间距
    t1Bars->setAntialiased(false);
    t2Bars->setAntialiased(false);
    t3Bars->setAntialiased(false);

    // 设置分组模式下的位置
    t2Bars->moveAbove(t1Bars);
    t3Bars->moveAbove(t2Bars);

    // 设置数据
    t1Bars->setData(ticks, t1Data);
    t2Bars->setData(ticks, t2Data);
    t3Bars->setData(ticks, t3Data);

    // 显示图例
    ui->tab2->legend->setVisible(true);
    ui->tab2->legend->setFont(QFont("Arial", 9));
    ui->tab2->legend->setBrush(QBrush(QColor(255, 255, 255, 220)));
    ui->tab2->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop | Qt::AlignRight);

    // 存储原始数据，以便模式切换时使用
    m_t1Data = t1Data;
    m_t2Data = t2Data;
    m_t3Data = t3Data;

    // 交互设置
    ui->tab2->setInteraction(QCP::iRangeDrag, true);
    ui->tab2->setInteraction(QCP::iRangeZoom, true);

    // 连接显示数值按钮的信号
    connect(ui->btn_tab2_showVal, &QPushButton::clicked, this, [this]() {
        // 获取当前按钮状态
        bool isChecked = ui->btn_tab2_showVal->isChecked();

        // 更新所有柱状图是否显示数值
        for (int i = 0; i < m_bars.size(); ++i) {
            // 设置是否显示标签
            m_bars[i]->setShowLabels(isChecked);
        }

        // 更新图表
        ui->tab2->replot();
    });

    // 连接模式切换按钮的信号
    connect(ui->btn_tab2_mode, &QPushButton::clicked, this, [this, ticks, width]() {
        // 获取当前按钮状态
        bool isStacked = ui->btn_tab2_mode->isChecked();

        if (!isStacked) {
            // 堆叠模式
            ui->btn_tab2_mode->setText("堆叠模式");

            // 设置堆叠顺序
            m_bars[1]->moveAbove(m_bars[0]);
            m_bars[2]->moveAbove(m_bars[1]);
            m_bars[1]->setStackingGap(1);
            m_bars[2]->setStackingGap(1);

            // 堆叠模式下重新设置数据
            m_bars[0]->setData(ticks, m_t1Data);
            m_bars[1]->setData(ticks, m_t2Data);
            m_bars[2]->setData(ticks, m_t3Data);
        } else {
            // 分组模式
            ui->btn_tab2_mode->setText("分组模式");

            // 设置分组位置
            m_bars[0]->moveBelow(nullptr); // 清除堆叠关系
            m_bars[1]->moveBelow(nullptr);
            m_bars[2]->moveBelow(nullptr);

            // 偏移柱子位置实现分组效果
            QVector<double> t1pos = ticks;
            QVector<double> t2pos, t3pos;

            for (double pos : ticks) {
                t2pos << pos + width;
                t3pos << pos + width * 2;
            }

            // 重新设置数据的位置
            m_bars[0]->setData(t1pos, m_t1Data);
            m_bars[1]->setData(t2pos, m_t2Data);
            m_bars[2]->setData(t3pos, m_t3Data);
        }

        // 更新图表
        ui->tab2->replot();
    });

    // 设置按钮初始状态
    ui->btn_tab2_showVal->setCheckable(true);
    ui->btn_tab2_showVal->setChecked(false);
    ui->btn_tab2_showVal->setText("显示数值");

    ui->btn_tab2_mode->setCheckable(true);
    ui->btn_tab2_mode->setChecked(false);
    ui->btn_tab2_mode->setText("分组模式");

    // 绘制图表
    ui->tab2->replot();
}

void Widget::setupTabWidget()
{
    setupTab1();
    m_tips.append(TIPS_TAB1);
    setupTab2();
    m_tips.append(TIPS_TAB2);
}
