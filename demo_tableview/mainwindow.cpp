#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tableitemdelegate.h"
#include <QTableView>
#include <QHeaderView>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QFileDialog>
#include <QCheckBox>
#include <QLabel>
#include <QSplitter>
#include <QDebug>
#include <QApplication>
#include <QSettings>
#include <QToolBar>
#include <QAction>
#include <QStatusBar>
#include <QMenu>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    
    // 初始化窗口大小和标题
    resize(1000, 600);
    setWindowTitle(tr("支持排序和筛选的表格控件示例"));
    
    // 创建模型和代理
    m_tableModel = new TableModel(this);
    m_proxyModel = new QSortFilterProxyModel(this);
    m_proxyModel->setSourceModel(m_tableModel);
    
    // 设置代理模型为动态排序
    m_proxyModel->setDynamicSortFilter(true);
    
    // 设置UI界面
    setupUI();
    
    // 设置筛选器
    setupFilters();
    
    // 连接信号槽
    connectSignals();
    
    // 生成测试数据
    createTestData();
    
    // 恢复窗口设置
    QSettings settings(QApplication::organizationName(), QApplication::applicationName());
    restoreGeometry(settings.value("windowGeometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());
    
    // 设置状态栏
    statusBar()->showMessage(tr("就绪"), 3000);
}

MainWindow::~MainWindow()
{
    // 保存窗口设置
    QSettings settings(QApplication::organizationName(), QApplication::applicationName());
    settings.setValue("windowGeometry", saveGeometry());
    settings.setValue("windowState", saveState());
    
    delete ui;
}

void MainWindow::setupUI()
{
    // 创建中央窗口部件
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    // 创建主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(8, 8, 8, 8);
    mainLayout->setSpacing(8);
    
    // 创建筛选器区域
    QGroupBox *filterGroupBox = new QGroupBox(tr("筛选器"), centralWidget);
    QGridLayout *filterLayout = new QGridLayout(filterGroupBox);
    filterLayout->setContentsMargins(8, 8, 8, 8);
    filterLayout->setSpacing(8);
    
    // 名称筛选
    QLabel *nameLabel = new QLabel(tr("名称:"), filterGroupBox);
    m_nameFilterEdit = new QLineEdit(filterGroupBox);
    m_nameFilterEdit->setPlaceholderText(tr("输入名称关键字"));
    filterLayout->addWidget(nameLabel, 0, 0);
    filterLayout->addWidget(m_nameFilterEdit, 0, 1);
    
    // 类别筛选
    QLabel *categoryLabel = new QLabel(tr("类别:"), filterGroupBox);
    m_categoryFilterCombo = new QComboBox(filterGroupBox);
    m_categoryFilterCombo->setMinimumWidth(120);
    filterLayout->addWidget(categoryLabel, 0, 2);
    filterLayout->addWidget(m_categoryFilterCombo, 0, 3);
    
    // 日期筛选
    QLabel *dateLabel = new QLabel(tr("日期:"), filterGroupBox);
    m_dateFilterEdit = new QDateEdit(filterGroupBox);
    m_dateFilterEdit->setCalendarPopup(true);
    m_dateFilterEdit->setDate(QDate::currentDate().addMonths(-1));
    filterLayout->addWidget(dateLabel, 0, 4);
    filterLayout->addWidget(m_dateFilterEdit, 0, 5);
    
    // 价格筛选
    QLabel *priceLabel = new QLabel(tr("价格:"), filterGroupBox);
    m_priceFilterSpin = new QDoubleSpinBox(filterGroupBox);
    m_priceFilterSpin->setRange(0, 9999.99);
    m_priceFilterSpin->setValue(0);
    m_priceFilterSpin->setPrefix("¥ ");
    filterLayout->addWidget(priceLabel, 1, 0);
    filterLayout->addWidget(m_priceFilterSpin, 1, 1);
    
    // 可用性筛选
    QLabel *availableLabel = new QLabel(tr("可用:"), filterGroupBox);
    m_availableFilterCombo = new QComboBox(filterGroupBox);
    m_availableFilterCombo->addItem(tr("全部"), -1);
    m_availableFilterCombo->addItem(tr("是"), 1);
    m_availableFilterCombo->addItem(tr("否"), 0);
    filterLayout->addWidget(availableLabel, 1, 2);
    filterLayout->addWidget(m_availableFilterCombo, 1, 3);
    
    // 重置筛选按钮
    m_resetFilterButton = new QPushButton(tr("重置筛选"), filterGroupBox);
    filterLayout->addWidget(m_resetFilterButton, 1, 5);
    
    mainLayout->addWidget(filterGroupBox);
    
    // 创建表格视图
    m_tableView = new QTableView(centralWidget);
    m_tableView->setModel(m_proxyModel);
    m_tableView->setItemDelegate(new TableItemDelegate(m_tableView));
    m_tableView->setSortingEnabled(true);
    m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_tableView->setAlternatingRowColors(true);
    m_tableView->setShowGrid(true);
    m_tableView->setGridStyle(Qt::DotLine);
    m_tableView->horizontalHeader()->setHighlightSections(false);
    m_tableView->horizontalHeader()->setSectionsClickable(true);
    m_tableView->horizontalHeader()->setStretchLastSection(true);
    m_tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    m_tableView->verticalHeader()->setDefaultSectionSize(30);
    m_tableView->verticalHeader()->setHighlightSections(false);
    m_tableView->setEditTriggers(QAbstractItemView::DoubleClicked | 
                              QAbstractItemView::EditKeyPressed);
    
    // 初始排序
    m_tableView->sortByColumn(TableItem::IdColumn, Qt::AscendingOrder);
    
    // 设置列宽
    m_tableView->setColumnWidth(TableItem::IdColumn, 60);
    m_tableView->setColumnWidth(TableItem::NameColumn, 150);
    m_tableView->setColumnWidth(TableItem::CategoryColumn, 120);
    m_tableView->setColumnWidth(TableItem::DateColumn, 100);
    m_tableView->setColumnWidth(TableItem::PriceColumn, 100);
    m_tableView->setColumnWidth(TableItem::AvailableColumn, 80);
    
    mainLayout->addWidget(m_tableView, 1);
    
    // 创建操作按钮区域
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setContentsMargins(0, 0, 0, 0);
    buttonLayout->setSpacing(8);
    
    m_addButton = new QPushButton(tr("添加"), centralWidget);
    m_removeButton = new QPushButton(tr("删除"), centralWidget);
    m_saveButton = new QPushButton(tr("保存"), centralWidget);
    m_loadButton = new QPushButton(tr("加载"), centralWidget);
    
    buttonLayout->addWidget(m_addButton);
    buttonLayout->addWidget(m_removeButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_saveButton);
    buttonLayout->addWidget(m_loadButton);
    
    mainLayout->addLayout(buttonLayout);
    
    // 创建工具栏
    QToolBar *toolBar = addToolBar(tr("工具栏"));
    toolBar->setMovable(false);
    
    QAction *addAction = toolBar->addAction(QIcon::fromTheme("list-add"), tr("添加记录"));
    QAction *removeAction = toolBar->addAction(QIcon::fromTheme("edit-delete"), tr("删除记录"));
    toolBar->addSeparator();
    QAction *saveAction = toolBar->addAction(QIcon::fromTheme("document-save"), tr("保存数据"));
    QAction *loadAction = toolBar->addAction(QIcon::fromTheme("document-open"), tr("加载数据"));
    
    // 连接工具栏动作
    connect(addAction, &QAction::triggered, this, &MainWindow::onAddItemClicked);
    connect(removeAction, &QAction::triggered, this, &MainWindow::onRemoveItemClicked);
    connect(saveAction, &QAction::triggered, this, &MainWindow::onSaveClicked);
    connect(loadAction, &QAction::triggered, this, &MainWindow::onLoadClicked);
}

void MainWindow::setupFilters()
{
    // 更新类别过滤器
    updateCategoryFilter();
}

void MainWindow::updateCategoryFilter()
{
    // 获取所有唯一的类别值
    QSet<QString> categories;
    for (int i = 0; i < m_tableModel->rowCount(); ++i) {
        TableItem item = m_tableModel->getItem(i);
        categories.insert(item.getCategory());
    }
    
    // 保存当前选中项
    QString currentCategory = m_categoryFilterCombo->currentText();
    
    // 清空并重新填充
    m_categoryFilterCombo->clear();
    m_categoryFilterCombo->addItem(tr("全部"));
    
    QStringList categoryList = categories.toList();
    std::sort(categoryList.begin(), categoryList.end());
    for (const QString &category : categoryList) {
        m_categoryFilterCombo->addItem(category);
    }
    
    // 恢复之前的选择
    int index = m_categoryFilterCombo->findText(currentCategory);
    if (index >= 0) {
        m_categoryFilterCombo->setCurrentIndex(index);
    }
}

void MainWindow::connectSignals()
{
    // 连接添加按钮
    connect(m_addButton, &QPushButton::clicked, this, &MainWindow::onAddItemClicked);
    
    // 连接删除按钮
    connect(m_removeButton, &QPushButton::clicked, this, &MainWindow::onRemoveItemClicked);
    
    // 连接保存按钮
    connect(m_saveButton, &QPushButton::clicked, this, &MainWindow::onSaveClicked);
    
    // 连接加载按钮
    connect(m_loadButton, &QPushButton::clicked, this, &MainWindow::onLoadClicked);
    
    // 连接筛选器
    connect(m_nameFilterEdit, &QLineEdit::textChanged, this, &MainWindow::onFilterTextChanged);
    connect(m_categoryFilterCombo, &QComboBox::currentTextChanged, this, &MainWindow::onCategoryFilterChanged);
    connect(m_dateFilterEdit, &QDateEdit::dateChanged, this, &MainWindow::onDateFilterChanged);
    connect(m_priceFilterSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &MainWindow::onPriceFilterChanged);
    connect(m_availableFilterCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), 
        [this](int index) {
            QVariant data = m_availableFilterCombo->itemData(index);
            onAvailableFilterChanged(data.toInt());
        });
    
    // 连接重置筛选按钮
    connect(m_resetFilterButton, &QPushButton::clicked, this, &MainWindow::onResetFilterClicked);
    
    // 连接表格项双击
    connect(m_tableView, &QTableView::doubleClicked, this, &MainWindow::onTableItemDoubleClicked);
    
    // 连接表格数据改变
    connect(m_tableModel, &QAbstractItemModel::dataChanged, this, &MainWindow::onTableDataChanged);
}

void MainWindow::createTestData()
{
    m_tableModel->generateTestData(100);
    
    // 设置自定义筛选器
    m_proxyModel->setFilterKeyColumn(TableItem::NameColumn);
    
    // 更新类别下拉框
    updateCategoryFilter();
    
    // 默认排序
    m_tableView->sortByColumn(TableItem::IdColumn, Qt::AscendingOrder);
}

void MainWindow::onAddItemClicked()
{
    // 添加新的空白行
    int id = m_tableModel->rowCount() + 1;
    QDate currentDate = QDate::currentDate();
    
    TableItem newItem(id, tr("新商品"), tr("未分类"), currentDate, 0.0, true);
    m_tableModel->addItem(newItem);
    
    // 定位到新行
    QModelIndex sourceIndex = m_tableModel->index(m_tableModel->rowCount() - 1, TableItem::NameColumn);
    QModelIndex proxyIndex = m_proxyModel->mapFromSource(sourceIndex);
    m_tableView->scrollTo(proxyIndex);
    m_tableView->setCurrentIndex(proxyIndex);
    m_tableView->edit(proxyIndex);
    
    // 更新类别筛选器
    updateCategoryFilter();
    
    statusBar()->showMessage(tr("添加了新记录"), 3000);
}

void MainWindow::onRemoveItemClicked()
{
    QModelIndexList selectedRows = m_tableView->selectionModel()->selectedRows();
    
    if (selectedRows.isEmpty()) {
        QMessageBox::information(this, tr("提示"), tr("请先选择要删除的行"));
        return;
    }
    
    QMessageBox::StandardButton reply = QMessageBox::question(this, tr("确认"),
        tr("确定要删除选中的 %1 行数据吗？").arg(selectedRows.size()),
        QMessageBox::Yes | QMessageBox::No);
        
    if (reply != QMessageBox::Yes) {
        return;
    }
    
    // 从大到小排序，避免删除小索引影响大索引
    std::sort(selectedRows.begin(), selectedRows.end(), 
        [](const QModelIndex &a, const QModelIndex &b) { 
            return a.row() > b.row();
        });
    
    // 逐个删除，避免索引失效
    for (const QModelIndex &proxyIndex : selectedRows) {
        QModelIndex sourceIndex = m_proxyModel->mapToSource(proxyIndex);
        m_tableModel->removeItem(sourceIndex.row());
    }
    
    // 更新类别筛选器
    updateCategoryFilter();
    
    statusBar()->showMessage(tr("删除了 %1 条记录").arg(selectedRows.size()), 3000);
}

void MainWindow::onSaveClicked()
{
    QString filename = QFileDialog::getSaveFileName(this, tr("保存数据"), 
        QString(), tr("数据文件 (*.dat);;所有文件 (*.*)"));
        
    if (filename.isEmpty()) {
        return;
    }
    
    if (m_tableModel->saveToFile(filename)) {
        statusBar()->showMessage(tr("数据已保存到：%1").arg(filename), 5000);
    } else {
        QMessageBox::warning(this, tr("保存失败"), 
            tr("无法保存数据到文件：%1").arg(filename));
    }
}

void MainWindow::onLoadClicked()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("加载数据"), 
        QString(), tr("数据文件 (*.dat);;所有文件 (*.*)"));
        
    if (filename.isEmpty()) {
        return;
    }
    
    if (m_tableModel->loadFromFile(filename)) {
        // 更新类别筛选器
        updateCategoryFilter();
        
        statusBar()->showMessage(tr("数据已从 %1 加载").arg(filename), 5000);
    } else {
        QMessageBox::warning(this, tr("加载失败"), 
            tr("无法从文件加载数据：%1").arg(filename));
    }
}

void MainWindow::onFilterTextChanged(const QString &text)
{
    m_proxyModel->setFilterKeyColumn(TableItem::NameColumn);
    m_proxyModel->setFilterFixedString(text);
}

void MainWindow::onCategoryFilterChanged(const QString &category)
{
    if (category == tr("全部")) {
        // 不应用类别筛选
        m_proxyModel->setFilterKeyColumn(TableItem::NameColumn);
        m_proxyModel->setFilterFixedString(m_nameFilterEdit->text());
    } else {
        // 创建自定义过滤器
        m_proxyModel->setFilterKeyColumn(TableItem::CategoryColumn);
        m_proxyModel->setFilterFixedString(category);
    }
}

void MainWindow::onDateFilterChanged(const QDate &date)
{
    // 确保筛选器应用到正确的列
    m_proxyModel->setFilterKeyColumn(TableItem::DateColumn);
    
    // 创建自定义筛选器
    m_proxyModel->setFilterFixedString(date.toString(Qt::ISODate));
}

void MainWindow::onPriceFilterChanged(double value)
{
    // 设置价格筛选器
    m_proxyModel->setFilterKeyColumn(TableItem::PriceColumn);
    
    if (value <= 0.01) {
        // 不应用价格筛选
        m_proxyModel->setFilterFixedString("");
    } else {
        // 筛选大于等于指定价格的项目
        m_proxyModel->setFilterRole(Qt::EditRole);
        
        // 使用自定义筛选器
        m_proxyModel->setFilterFixedString(QString::number(value, 'f', 2));
    }
}

void MainWindow::onAvailableFilterChanged(int state)
{
    if (state == -1) {
        // 不应用可用性筛选
        m_proxyModel->setFilterKeyColumn(TableItem::NameColumn);
        m_proxyModel->setFilterFixedString(m_nameFilterEdit->text());
    } else {
        // 应用可用性筛选
        bool available = (state == 1);
        m_proxyModel->setFilterKeyColumn(TableItem::AvailableColumn);
        m_proxyModel->setFilterFixedString(available ? "true" : "false");
    }
}

void MainWindow::onTableItemDoubleClicked(const QModelIndex &index)
{
    // 开始编辑
    m_tableView->edit(index);
}

void MainWindow::onTableDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    Q_UNUSED(topLeft);
    Q_UNUSED(bottomRight);
    
    // 如果有类别数据变更，更新筛选器
    updateCategoryFilter();
    
    statusBar()->showMessage(tr("数据已更新"), 3000);
}

void MainWindow::onResetFilterClicked()
{
    // 重置所有筛选器
    m_nameFilterEdit->clear();
    m_categoryFilterCombo->setCurrentIndex(0);
    m_dateFilterEdit->setDate(QDate::currentDate().addMonths(-1));
    m_priceFilterSpin->setValue(0);
    m_availableFilterCombo->setCurrentIndex(0);
    
    // 恢复到名称筛选模式
    m_proxyModel->setFilterKeyColumn(TableItem::NameColumn);
    m_proxyModel->setFilterFixedString("");
    
    statusBar()->showMessage(tr("筛选器已重置"), 3000);
}
