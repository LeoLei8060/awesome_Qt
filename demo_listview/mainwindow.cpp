#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QInputDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>

// 配置文件名
static const QString s_configFileName = "listview_data.json";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_model(nullptr)
    , m_proxyModel(nullptr)
    , m_delegate(nullptr)
{
    ui->setupUi(this);

    // 设置窗口标题
    setWindowTitle(tr("支持排序和筛选的列表控件示例"));

    // 初始化模型和视图
    setupModel();
    setupView();

    // 加载初始数据
    loadData();

    // 连接信号槽
    connect(ui->listView, &QListView::doubleClicked, this, &MainWindow::on_listView_doubleClicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupModel()
{
    // 创建数据模型
    m_model = new ListModel(this);

    // 创建排序筛选代理模型
    m_proxyModel = new ListSortFilterProxyModel(this);
    m_proxyModel->setSourceModel(m_model);

    // 创建项委托
    m_delegate = new ListItemDelegate(this);
}

void MainWindow::setupView()
{
    // 设置视图模型和委托
    ui->listView->setModel(m_proxyModel);
    ui->listView->setItemDelegate(m_delegate);

    // 设置选择模式
    ui->listView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->listView->setSelectionBehavior(QAbstractItemView::SelectRows);

    // 启用交替行颜色
    ui->listView->setAlternatingRowColors(true);

    // 设置列表视图模式（默认为列表模式）
    ui->listView->setViewMode(QListView::ListMode);

    // 设置图标大小
    ui->listView->setIconSize(QSize(40, 40));

    // 设置项间距
    ui->listView->setSpacing(2);

    // 配置编辑功能
    ui->listView->setEditTriggers(QAbstractItemView::DoubleClicked
                                  | QAbstractItemView::EditKeyPressed);
}

void MainWindow::loadInitialData()
{
    // 添加一些示例数据
    m_model->addItem(ListItem("示例项目1", QDateTime::currentDateTime(), 120));
    m_model->addItem(ListItem("示例项目2", QDateTime::currentDateTime().addDays(-1), 240));
    m_model->addItem(ListItem("示例项目3", QDateTime::currentDateTime().addDays(-2), 330));
    m_model->addItem(ListItem("示例项目4", QDateTime::currentDateTime().addDays(-3), 450));
    m_model->addItem(ListItem("示例项目5", QDateTime::currentDateTime().addDays(-4), 550));
}

void MainWindow::on_filterLineEdit_textChanged(const QString &text)
{
    // 设置筛选模式
    m_proxyModel->setFilterPattern(text);
}

void MainWindow::on_sortComboBox_currentIndexChanged(int index)
{
    // 设置排序角色
    switch (index) {
    case 0: // 不排序
        m_proxyModel->sort(-1);
        break;
    case 1: // 按名称排序
        m_proxyModel->setSortRole(ListModel::NameRole);
        m_proxyModel->sort(0,
                           ui->descendingCheckBox->isChecked() ? Qt::DescendingOrder
                                                               : Qt::AscendingOrder);
        break;
    case 2: // 按日期排序
        m_proxyModel->setSortRole(ListModel::DateRole);
        m_proxyModel->sort(0,
                           ui->descendingCheckBox->isChecked() ? Qt::DescendingOrder
                                                               : Qt::AscendingOrder);
        break;
    case 3: // 按大小排序
        m_proxyModel->setSortRole(ListModel::SizeRole);
        m_proxyModel->sort(0,
                           ui->descendingCheckBox->isChecked() ? Qt::DescendingOrder
                                                               : Qt::AscendingOrder);
        break;
    }
}

void MainWindow::on_descendingCheckBox_stateChanged(int state)
{
    // 如果当前没有选择排序方式，则忽略
    if (ui->sortComboBox->currentIndex() == 0)
        return;

    // 重新排序
    m_proxyModel->sort(0, state == Qt::Checked ? Qt::DescendingOrder : Qt::AscendingOrder);
}

void MainWindow::on_viewModeComboBox_currentIndexChanged(int index)
{
    // 设置视图模式
    switch (index) {
    case 0: // 列表模式
        ui->listView->setViewMode(QListView::ListMode);
        ui->listView->setIconSize(QSize(40, 40));
        ui->listView->setGridSize(QSize());            // 重置网格大小
        ui->listView->setResizeMode(QListView::Fixed); // 重置调整模式
        ui->listView->setWrapping(false);              // 禁用自动换行
        ui->listView->setSpacing(2);                   // 重置间距
        break;
    case 1: // 图标模式
        ui->listView->setViewMode(QListView::IconMode);
        ui->listView->setIconSize(QSize(64, 64));
        ui->listView->setGridSize(QSize(125, 125)); // 设置网格大小
        ui->listView->setResizeMode(QListView::Adjust);
        ui->listView->setWrapping(true);
        ui->listView->setSpacing(20);                 // 增加间距
        ui->listView->setMovement(QListView::Static); // 防止拖动
        ui->listView->setUniformItemSizes(true);      // 统一项目大小
        break;
    }
}

void MainWindow::on_addButton_clicked()
{
    // 获取名称
    QString name = QInputDialog::getText(this, tr("添加项目"), tr("项目名称:"), QLineEdit::Normal);
    if (name.isEmpty())
        return;

    // 大小
    int size = QInputDialog::getInt(this, tr("添加项目"), tr("项目大小 (KB):"), 100, 0, 999999, 1);

    // 创建新项目
    ListItem newItem(name, QDateTime::currentDateTime(), size);

    // 添加到模型
    m_model->addItem(newItem);

    // 自动保存
    saveData();
}

void MainWindow::on_editButton_clicked()
{
    // 获取选中的索引
    QModelIndex proxyIndex = ui->listView->currentIndex();
    if (!proxyIndex.isValid())
        return;

    // 转换为源模型索引
    QModelIndex sourceIndex = m_proxyModel->mapToSource(proxyIndex);

    // 编辑项目
    editItem(proxyIndex);
}

void MainWindow::on_deleteButton_clicked()
{
    // 获取选中的索引
    QModelIndex proxyIndex = ui->listView->currentIndex();
    if (!proxyIndex.isValid())
        return;

    // 转换为源模型索引
    QModelIndex sourceIndex = m_proxyModel->mapToSource(proxyIndex);

    // 确认删除
    QMessageBox::StandardButton reply = QMessageBox::question(this,
                                                              tr("确认删除"),
                                                              tr("确定要删除选中的项目吗?"),
                                                              QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        // 删除项目
        m_model->removeItem(sourceIndex.row());

        // 自动保存
        saveData();
    }
}

void MainWindow::on_actionSave_triggered()
{
    saveData();
    QMessageBox::information(this, tr("保存"), tr("数据已保存!"));
}

void MainWindow::on_actionExit_triggered()
{
    close();
}

void MainWindow::on_listView_doubleClicked(const QModelIndex &index)
{
    // 不再调用editItem，让委托自动处理编辑
    // 不做任何操作，由代理直接处理编辑功能
}

void MainWindow::editItem(const QModelIndex &proxyIndex)
{
    if (!proxyIndex.isValid())
        return;

    // 转换为源模型索引
    QModelIndex sourceIndex = m_proxyModel->mapToSource(proxyIndex);

    // 获取当前项目
    ListItem item = m_model->getItem(sourceIndex.row());

    // 获取新名称
    QString name = QInputDialog::getText(this,
                                         tr("编辑项目"),
                                         tr("项目名称:"),
                                         QLineEdit::Normal,
                                         item.getName());
    if (name.isEmpty())
        return;

    // 获取新大小
    int size = QInputDialog::getInt(this,
                                    tr("编辑项目"),
                                    tr("项目大小 (KB):"),
                                    item.getSize(),
                                    0,
                                    999999,
                                    1);

    // 更新项目
    item.setName(name);
    item.setSize(size);

    // 更新模型
    m_model->updateItem(sourceIndex.row(), item);

    // 自动保存
    saveData();
}

void MainWindow::saveData()
{
    // 获取所有项目
    QVector<ListItem> items = m_model->getAllItems();

    // 创建JSON数组
    QJsonArray jsonArray;

    // 遍历所有项目
    for (const ListItem &item : items) {
        QJsonObject jsonObj;
        jsonObj["name"] = item.getName();
        jsonObj["date"] = item.getDate().toString(Qt::ISODate);
        jsonObj["size"] = item.getSize();
        jsonObj["iconPath"] = item.getIconPath();

        jsonArray.append(jsonObj);
    }

    // 创建JSON文档
    QJsonDocument jsonDoc(jsonArray);

    // 保存到文件
    QFile file(s_configFileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        file.write(jsonDoc.toJson());
        file.close();
    } else {
        QMessageBox::warning(this,
                             tr("保存失败"),
                             tr("无法保存数据到文件: %1").arg(s_configFileName));
    }
}

void MainWindow::loadData()
{
    // 尝试从文件加载数据
    QFile file(s_configFileName);
    if (file.exists() && file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        // 读取JSON文档
        QJsonDocument jsonDoc = QJsonDocument::fromJson(file.readAll());
        file.close();

        // 解析JSON数组
        if (jsonDoc.isArray()) {
            QJsonArray jsonArray = jsonDoc.array();

            // 清空模型
            m_model->clear();

            // 遍历JSON数组
            for (const QJsonValue &value : jsonArray) {
                if (value.isObject()) {
                    QJsonObject obj = value.toObject();

                    QString   name = obj["name"].toString();
                    QDateTime date = QDateTime::fromString(obj["date"].toString(), Qt::ISODate);
                    int       size = obj["size"].toInt();
                    QString   iconPath = obj["iconPath"].toString();

                    // 添加项目
                    m_model->addItem(ListItem(name, date, size, iconPath));
                }
            }
        }
    } else {
        // 如果文件不存在，加载初始数据
        loadInitialData();
    }
}
