#include "mainwindow.h"
#include <QDebug>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      m_refreshRunning(false)
{
    // 设置窗口标题和大小
    setWindowTitle("自绘列表示例");
    resize(800, 600);
    
    // 创建中央部件
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    // 创建主布局
    m_mainLayout = new QVBoxLayout(centralWidget);
    
    // 创建自定义列表控件
    m_listWidget = new CustomListWidget(this);
    
    // 创建控制面板
    m_controlPanel = new QWidget(this);
    QHBoxLayout *controlLayout = new QHBoxLayout(m_controlPanel);
    
    // 创建操作按钮
    m_addButton = new QPushButton("添加项", this);
    m_removeButton = new QPushButton("删除选中项", this);
    m_clearButton = new QPushButton("清空", this);
    
    // 创建选择模式下拉框
    QLabel *selectionModeLabel = new QLabel("选择模式:", this);
    m_selectionModeCombo = new QComboBox(this);
    m_selectionModeCombo->addItem("单选", CustomListWidget::SingleSelection);
    m_selectionModeCombo->addItem("多选", CustomListWidget::MultiSelection);
    m_selectionModeCombo->addItem("无选择", CustomListWidget::NoSelection);
    
    // 添加到控制布局
    controlLayout->addWidget(m_addButton);
    controlLayout->addWidget(m_removeButton);
    controlLayout->addWidget(m_clearButton);
    controlLayout->addWidget(selectionModeLabel);
    controlLayout->addWidget(m_selectionModeCombo);
    controlLayout->addStretch();
    
    // 创建刷新控制面板
    QGroupBox *refreshGroupBox = new QGroupBox("实时刷新控制", this);
    QHBoxLayout *refreshLayout = new QHBoxLayout(refreshGroupBox);
    
    // 创建刷新控制组件
    m_randomRefreshCheck = new QCheckBox("随机内容", this);
    m_randomRefreshCheck->setChecked(true);
    
    QLabel *refreshIntervalLabel = new QLabel("刷新间隔(ms):", this);
    m_refreshIntervalSpin = new QSpinBox(this);
    m_refreshIntervalSpin->setRange(10, 5000);
    m_refreshIntervalSpin->setValue(100);
    
    m_startStopRefreshButton = new QPushButton("开始刷新", this);
    
    // 添加到刷新布局
    refreshLayout->addWidget(m_randomRefreshCheck);
    refreshLayout->addWidget(refreshIntervalLabel);
    refreshLayout->addWidget(m_refreshIntervalSpin);
    refreshLayout->addWidget(m_startStopRefreshButton);
    refreshLayout->addStretch();
    
    // 创建状态信息面板
    QGroupBox *statusGroupBox = new QGroupBox("列表状态", this);
    QVBoxLayout *statusLayout = new QVBoxLayout(statusGroupBox);
    
    m_statusLabel = new QLabel("项目数: 0", this);
    m_currentItemLabel = new QLabel("当前选中: 无", this);
    
    statusLayout->addWidget(m_statusLabel);
    statusLayout->addWidget(m_currentItemLabel);
    
    // 添加组件到主布局
    m_mainLayout->addWidget(m_listWidget);
    m_mainLayout->addWidget(m_controlPanel);
    m_mainLayout->addWidget(refreshGroupBox);
    m_mainLayout->addWidget(statusGroupBox);
    
    // 设置布局比例
    m_mainLayout->setStretchFactor(m_listWidget, 4);
    m_mainLayout->setStretchFactor(m_controlPanel, 1);
    m_mainLayout->setStretchFactor(refreshGroupBox, 1);
    m_mainLayout->setStretchFactor(statusGroupBox, 1);
    
    // 创建刷新定时器
    m_refreshTimer = new QTimer(this);
    
    // 初始化随机文本列表
    m_randomTexts << "苹果" << "香蕉" << "橙子" << "葡萄" << "草莓" 
                 << "菠萝" << "西瓜" << "樱桃" << "芒果" << "柠檬"
                 << "石榴" << "猕猴桃" << "蓝莓" << "柚子" << "火龙果"
                 << "荔枝" << "桂圆" << "榴莲" << "山竹" << "杨梅";
                 
    // 连接信号与槽
    connect(m_addButton, &QPushButton::clicked, this, &MainWindow::onAddButtonClicked);
    connect(m_removeButton, &QPushButton::clicked, this, &MainWindow::onRemoveButtonClicked);
    connect(m_clearButton, &QPushButton::clicked, this, &MainWindow::onClearButtonClicked);
    connect(m_selectionModeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            this, &MainWindow::onSelectionModeChanged);
    
    connect(m_startStopRefreshButton, &QPushButton::clicked, 
            this, &MainWindow::onStartStopRefreshClicked);
    connect(m_refreshTimer, &QTimer::timeout, this, &MainWindow::onRefreshTimerTimeout);
    
    connect(m_listWidget, &CustomListWidget::itemClicked, this, &MainWindow::onItemClicked);
    connect(m_listWidget, &CustomListWidget::itemDoubleClicked, this, &MainWindow::onItemDoubleClicked);
    connect(m_listWidget, &CustomListWidget::currentItemChanged, this, &MainWindow::onCurrentItemChanged);
    connect(m_listWidget, &CustomListWidget::itemSelectionChanged, this, &MainWindow::onItemSelectionChanged);
    
    // 初始化控件
    updateStatusLabel();
    
    // 添加一些初始项目
    for (int i = 0; i < 20; i++) {
        onAddButtonClicked();
    }
}

MainWindow::~MainWindow()
{
    // 停止定时器
    if (m_refreshTimer->isActive()) {
        m_refreshTimer->stop();
    }
}

void MainWindow::onAddButtonClicked()
{
    // 添加一个随机项目
    QString text = randomText();
    QColor textColor = Qt::black;
    QColor bgColor = QColor(240, 240, 240);
    QFont font;
    
    // 随机设置字体大小
    font.setPointSize(QRandomGenerator::global()->bounded(10, 14));
    
    m_listWidget->addItem(text, textColor, bgColor, font);
    updateStatusLabel();
}

void MainWindow::onRemoveButtonClicked()
{
    int currentIndex = m_listWidget->currentIndex();
    if (currentIndex != -1) {
        m_listWidget->removeItem(currentIndex);
        updateStatusLabel();
    } else {
        QMessageBox::information(this, "提示", "请先选择一个项目");
    }
}

void MainWindow::onClearButtonClicked()
{
    if (m_listWidget->count() > 0) {
        QMessageBox::StandardButton reply = QMessageBox::question(this, "确认", 
            "确定要清空所有项目吗？", QMessageBox::Yes | QMessageBox::No);
            
        if (reply == QMessageBox::Yes) {
            m_listWidget->clear();
            updateStatusLabel();
        }
    } else {
        QMessageBox::information(this, "提示", "列表已经是空的");
    }
}

void MainWindow::onItemClicked(int index)
{
    qDebug() << "项目点击: " << index;
    updateStatusLabel();
}

void MainWindow::onItemDoubleClicked(int index)
{
    ListItem *item = m_listWidget->item(index);
    if (item) {
        QMessageBox::information(this, "项目双击", 
            QString("你双击了项目: %1").arg(item->text()));
    }
}

void MainWindow::onCurrentItemChanged(int current, int previous)
{
    qDebug() << "当前项目改变: 从" << previous << "到" << current;
    updateStatusLabel();
}

void MainWindow::onItemSelectionChanged()
{
    qDebug() << "选择变更";
    updateStatusLabel();
}

void MainWindow::onStartStopRefreshClicked()
{
    if (!m_refreshRunning) {
        // 开始刷新
        m_refreshTimer->start(m_refreshIntervalSpin->value());
        m_startStopRefreshButton->setText("停止刷新");
        m_refreshRunning = true;
    } else {
        // 停止刷新
        m_refreshTimer->stop();
        m_startStopRefreshButton->setText("开始刷新");
        m_refreshRunning = false;
    }
}

void MainWindow::onRefreshTimerTimeout()
{
    // 随机更新一个项目
    if (m_listWidget->count() > 0) {
        int index = QRandomGenerator::global()->bounded(m_listWidget->count());
        ListItem *item = m_listWidget->item(index);
        if (item) {
            if (m_randomRefreshCheck->isChecked()) {
                // 随机修改文本
                item->setText(randomText());
            }
            
            // 随机修改背景色
            QColor bgColor;
            bgColor.setRgb(QRandomGenerator::global()->bounded(210, 240),
                         QRandomGenerator::global()->bounded(210, 240),
                         QRandomGenerator::global()->bounded(210, 240));
            item->setBackgroundColor(bgColor);
            
            // 更新项目
            m_listWidget->updateItem(index);
        }
    }
}

void MainWindow::onSelectionModeChanged(int index)
{
    CustomListWidget::SelectionMode mode = 
        static_cast<CustomListWidget::SelectionMode>(m_selectionModeCombo->itemData(index).toInt());
    m_listWidget->setSelectionMode(mode);
}

void MainWindow::updateStatusLabel()
{
    // 更新状态标签
    m_statusLabel->setText(QString("项目数: %1").arg(m_listWidget->count()));
    
    // 更新当前项目标签
    int currentIndex = m_listWidget->currentIndex();
    if (currentIndex != -1) {
        ListItem *item = m_listWidget->item(currentIndex);
        if (item) {
            m_currentItemLabel->setText(QString("当前选中: %1 (索引: %2)")
                .arg(item->text()).arg(currentIndex));
        }
    } else {
        m_currentItemLabel->setText("当前选中: 无");
    }
}

QColor MainWindow::randomColor()
{
    // 生成随机颜色
    return QColor(QRandomGenerator::global()->bounded(50, 200),
                QRandomGenerator::global()->bounded(50, 200),
                QRandomGenerator::global()->bounded(50, 200));
}

QString MainWindow::randomText()
{
    // 从随机文本列表中选择一个或多个
    int count = QRandomGenerator::global()->bounded(1, 4);
    QStringList selected;
    
    for (int i = 0; i < count; i++) {
        int index = QRandomGenerator::global()->bounded(m_randomTexts.size());
        selected << m_randomTexts[index];
    }
    
    return selected.join("-") + QString("-%1").arg(QRandomGenerator::global()->bounded(1000));
}
