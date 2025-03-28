#include "mainwidget.h"
#include "ui_mainwidget.h"
#include <QDebug>
#include <QMessageBox>

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWidget)
    , m_refreshRunning(false)
{
    // 设置UI
    ui->setupUi(this);

    // 设置窗口标题和大小
    setWindowTitle("自绘列表示例");
    resize(800, 600);

    // 初始化选择模式下拉框
    ui->selectionModeCombo->addItem("单选", CustomListWidget::SingleSelection);
    ui->selectionModeCombo->addItem("多选", CustomListWidget::MultiSelection);
    ui->selectionModeCombo->addItem("无选择", CustomListWidget::NoSelection);

    // 创建刷新定时器
    m_refreshTimer = new QTimer(this);

    // 初始化随机文本列表
    m_randomTexts << "苹果" << "香蕉" << "橙子" << "葡萄" << "草莓"
                  << "菠萝" << "西瓜" << "樱桃" << "芒果" << "柠檬"
                  << "石榴" << "猕猴桃" << "蓝莓" << "柚子" << "火龙果"
                  << "荔枝" << "桂圆" << "榴莲" << "山竹" << "杨梅";

    // 连接信号与槽
    connect(ui->addButton, &QPushButton::clicked, this, &MainWidget::onAddButtonClicked);
    connect(ui->removeButton, &QPushButton::clicked, this, &MainWidget::onRemoveButtonClicked);
    connect(ui->clearButton, &QPushButton::clicked, this, &MainWidget::onClearButtonClicked);
    connect(ui->selectionModeCombo,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            this,
            &MainWidget::onSelectionModeChanged);

    connect(ui->startStopRefreshButton,
            &QPushButton::clicked,
            this,
            &MainWidget::onStartStopRefreshClicked);
    connect(m_refreshTimer, &QTimer::timeout, this, &MainWidget::onRefreshTimerTimeout);

    connect(ui->listWidget, &CustomListWidget::itemClicked, this, &MainWidget::onItemClicked);
    connect(ui->listWidget,
            &CustomListWidget::itemDoubleClicked,
            this,
            &MainWidget::onItemDoubleClicked);
    connect(ui->listWidget,
            &CustomListWidget::currentItemChanged,
            this,
            &MainWidget::onCurrentItemChanged);
    connect(ui->listWidget,
            &CustomListWidget::itemSelectionChanged,
            this,
            &MainWidget::onItemSelectionChanged);

    // 初始化控件
    updateStatusLabel();

    // 添加一些初始项目
    for (int i = 0; i < 20; i++) {
        onAddButtonClicked();
    }
}

MainWidget::~MainWidget()
{
    // 停止定时器
    if (m_refreshTimer->isActive()) {
        m_refreshTimer->stop();
    }

    delete ui;
}

void MainWidget::onAddButtonClicked()
{
    // 添加一个随机项目
    QString text = randomText();
    QColor  textColor = Qt::black;
    QColor  bgColor = QColor(240, 240, 240);
    QFont   font;

    // 随机设置字体大小
    font.setPointSize(QRandomGenerator::global()->bounded(10, 14));

    ui->listWidget->addItem(text, textColor, bgColor, font);
    updateStatusLabel();
}

void MainWidget::onRemoveButtonClicked()
{
    int currentIndex = ui->listWidget->currentIndex();
    if (currentIndex != -1) {
        ui->listWidget->removeItem(currentIndex);
        updateStatusLabel();
    } else {
        QMessageBox::information(this, "提示", "请先选择一个项目");
    }
}

void MainWidget::onClearButtonClicked()
{
    if (ui->listWidget->count() > 0) {
        QMessageBox::StandardButton reply = QMessageBox::question(this,
                                                                  "确认",
                                                                  "确定要清空所有项目吗？",
                                                                  QMessageBox::Yes
                                                                      | QMessageBox::No);

        if (reply == QMessageBox::Yes) {
            ui->listWidget->clear();
            updateStatusLabel();
        }
    } else {
        QMessageBox::information(this, "提示", "列表已经是空的");
    }
}

void MainWidget::onItemClicked(int index)
{
    qDebug() << "项目点击: " << index;
    updateStatusLabel();
}

void MainWidget::onItemDoubleClicked(int index)
{
    ListItem *item = ui->listWidget->item(index);
    if (item) {
        QMessageBox::information(this, "项目双击", QString("你双击了项目: %1").arg(item->text()));
    }
}

void MainWidget::onCurrentItemChanged(int current, int previous)
{
    qDebug() << "当前项目改变: 从" << previous << "到" << current;
    updateStatusLabel();
}

void MainWidget::onItemSelectionChanged()
{
    qDebug() << "选择变更";
    updateStatusLabel();
}

void MainWidget::onStartStopRefreshClicked()
{
    if (!m_refreshRunning) {
        // 开始刷新
        m_refreshTimer->start(ui->refreshIntervalSpin->value());
        ui->startStopRefreshButton->setText("停止刷新");
        m_refreshRunning = true;
    } else {
        // 停止刷新
        m_refreshTimer->stop();
        ui->startStopRefreshButton->setText("开始刷新");
        m_refreshRunning = false;
    }
}

void MainWidget::onRefreshTimerTimeout()
{
    // 随机更新一个项目
    if (ui->listWidget->count() > 0) {
        int       index = QRandomGenerator::global()->bounded(ui->listWidget->count());
        ListItem *item = ui->listWidget->item(index);
        if (item) {
            if (ui->randomRefreshCheck->isChecked()) {
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
            ui->listWidget->updateItem(index);
        }
    }
}

void MainWidget::onSelectionModeChanged(int index)
{
    CustomListWidget::SelectionMode mode = static_cast<CustomListWidget::SelectionMode>(
        ui->selectionModeCombo->itemData(index).toInt());
    ui->listWidget->setSelectionMode(mode);
}

void MainWidget::updateStatusLabel()
{
    // 更新状态标签
    ui->statusLabel->setText(QString("项目数: %1").arg(ui->listWidget->count()));

    // 更新当前项目标签
    int currentIndex = ui->listWidget->currentIndex();
    if (currentIndex != -1) {
        ListItem *item = ui->listWidget->item(currentIndex);
        if (item) {
            ui->currentItemLabel->setText(
                QString("当前选中: %1 (索引: %2)").arg(item->text()).arg(currentIndex));
        }
    } else {
        ui->currentItemLabel->setText("当前选中: 无");
    }
}

QColor MainWidget::randomColor()
{
    // 生成随机颜色
    return QColor(QRandomGenerator::global()->bounded(50, 200),
                  QRandomGenerator::global()->bounded(50, 200),
                  QRandomGenerator::global()->bounded(50, 200));
}

QString MainWidget::randomText()
{
    // 从随机文本列表中选择一个或多个
    int         count = QRandomGenerator::global()->bounded(1, 4);
    QStringList selected;

    for (int i = 0; i < count; i++) {
        int index = QRandomGenerator::global()->bounded(m_randomTexts.size());
        selected << m_randomTexts[index];
    }

    return selected.join("-") + QString("-%1").arg(QRandomGenerator::global()->bounded(1000));
}
