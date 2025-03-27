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
    m_proxyModel = new TableFilterProxyModel(this);
    m_proxyModel->setSourceModel(m_tableModel);
    m_proxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    
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
    QGroupBox *filterGroupBox = new QGroupBox(tr("筛选条件"), centralWidget);
    QGridLayout *filterLayout = new QGridLayout(filterGroupBox);
    filterLayout->setContentsMargins(8, 8, 8, 8);
    filterLayout->setSpacing(8);
    
    // 名称筛选
    QLabel *nameLabel = new QLabel(tr("名称:"), filterGroupBox);
    m_nameFilterEdit = new QLineEdit(filterGroupBox);
    m_nameFilterEdit->setClearButtonEnabled(true);
    m_nameFilterEdit->setPlaceholderText(tr("输入关键字筛选"));
    filterLayout->addWidget(nameLabel, 0, 0);
    filterLayout->addWidget(m_nameFilterEdit, 0, 1);
    
    // 类别筛选
    QLabel *categoryLabel = new QLabel(tr("类别:"), filterGroupBox);
    m_categoryFilterCombo = new QComboBox(filterGroupBox);
    m_categoryFilterCombo->setMinimumWidth(120);
    filterLayout->addWidget(categoryLabel, 0, 2);
    filterLayout->addWidget(m_categoryFilterCombo, 0, 3);
    
    // 日期筛选
    QLabel *dateLabel = new QLabel(tr("日期范围:"), filterGroupBox);
    m_dateFilterEdit = new QDateEdit(filterGroupBox);
    m_dateFilterEdit->setCalendarPopup(true);
    m_dateFilterEdit->setDate(QDate::currentDate().addMonths(-1));
    
    m_dateEndFilterEdit = new QDateEdit(filterGroupBox);
    m_dateEndFilterEdit->setCalendarPopup(true);
    m_dateEndFilterEdit->setDate(QDate::currentDate());
    
    QHBoxLayout *dateLayout = new QHBoxLayout();
    dateLayout->addWidget(m_dateFilterEdit);
    dateLayout->addWidget(new QLabel(tr("至"), filterGroupBox));
    dateLayout->addWidget(m_dateEndFilterEdit);
    
    filterLayout->addWidget(dateLabel, 0, 4);
    filterLayout->addLayout(dateLayout, 0, 5);
    
    // 价格筛选
    QLabel *priceLabel = new QLabel(tr("价格范围:"), filterGroupBox);
    m_minPriceFilterSpin = new QDoubleSpinBox(filterGroupBox);
    m_minPriceFilterSpin->setRange(0, 9999.99);
    m_minPriceFilterSpin->setValue(0);
    m_minPriceFilterSpin->setPrefix("¥ ");
    
    m_maxPriceFilterSpin = new QDoubleSpinBox(filterGroupBox);
    m_maxPriceFilterSpin->setRange(0, 9999.99);
    m_maxPriceFilterSpin->setValue(9999.99);
    m_maxPriceFilterSpin->setPrefix("¥ ");
    
    QHBoxLayout *priceLayout = new QHBoxLayout();
    priceLayout->addWidget(m_minPriceFilterSpin);
    priceLayout->addWidget(new QLabel(tr("至"), filterGroupBox));
    priceLayout->addWidget(m_maxPriceFilterSpin);
    
    filterLayout->addWidget(priceLabel, 1, 0);
    filterLayout->addLayout(priceLayout, 1, 1);
    
    // 可用性筛选
    QLabel *availableLabel = new QLabel(tr("可用:"), filterGroupBox);
    m_availableFilterCombo = new QComboBox(filterGroupBox);
    m_availableFilterCombo->addItem(tr("全部"), -1);
    m_availableFilterCombo->addItem(tr("是"), 1);
    m_availableFilterCombo->addItem(tr("否"), 0);
    filterLayout->addWidget(availableLabel, 1, 2);
    filterLayout->addWidget(m_availableFilterCombo, 1, 3);
    
    // 筛选操作按钮
    QHBoxLayout *filterButtonLayout = new QHBoxLayout();
    
    // 添加应用筛选按钮
    m_applyFilterButton = new QPushButton(tr("应用筛选"), filterGroupBox);
    m_applyFilterButton->setIcon(QIcon(":/icons/filter.png"));
    
    // 添加重置筛选按钮
    m_resetFilterButton = new QPushButton(tr("重置筛选"), filterGroupBox);
    
    filterButtonLayout->addWidget(m_applyFilterButton);
    filterButtonLayout->addWidget(m_resetFilterButton);
    filterButtonLayout->addStretch();
    
    filterLayout->addLayout(filterButtonLayout, 1, 4, 1, 2);
    
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
    // 数据修改信号
    connect(m_tableModel, &TableModel::dataChanged, this, &MainWindow::onTableDataChanged);
    
    // 表格项双击信号
    connect(m_tableView, &QTableView::doubleClicked, this, &MainWindow::onTableItemDoubleClicked);
    
    // 按钮点击信号
    connect(m_addButton, &QPushButton::clicked, this, &MainWindow::onAddItemClicked);
    connect(m_removeButton, &QPushButton::clicked, this, &MainWindow::onRemoveItemClicked);
    connect(m_saveButton, &QPushButton::clicked, this, &MainWindow::onSaveClicked);
    connect(m_loadButton, &QPushButton::clicked, this, &MainWindow::onLoadClicked);
    connect(m_resetFilterButton, &QPushButton::clicked, this, &MainWindow::onResetFilterClicked);
    connect(m_applyFilterButton, &QPushButton::clicked, this, &MainWindow::onApplyFilterClicked);
    
    // 筛选控件信号 - 值改变时只保存到对应变量，不直接触发筛选
    // 这些连接保留是为了在用户输入时保存筛选值，但不会立即应用筛选
    connect(m_nameFilterEdit, &QLineEdit::textChanged, 
            [this](const QString &text) { m_proxyModel->storeNameFilter(text); });
    
    connect(m_categoryFilterCombo, &QComboBox::currentTextChanged, 
            [this](const QString &category) { m_proxyModel->storeCategoryFilter(category); });
    
    connect(m_dateFilterEdit, &QDateEdit::dateChanged, 
            [this](const QDate &date) { m_proxyModel->storeDateFrom(date); });
    
    connect(m_dateEndFilterEdit, &QDateEdit::dateChanged, 
            [this](const QDate &date) { m_proxyModel->storeDateTo(date); });
    
    connect(m_minPriceFilterSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), 
            [this](double value) { m_proxyModel->storeMinPrice(value); });
    
    connect(m_maxPriceFilterSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), 
            [this](double value) { m_proxyModel->storeMaxPrice(value); });
    
    connect(m_availableFilterCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), 
        [this](int index) {
            QVariant data = m_availableFilterCombo->itemData(index);
            int state = data.isValid() ? data.toInt() : -1;
            m_proxyModel->storeAvailabilityFilter(state);
        });
}

void MainWindow::createTestData()
{
    m_tableModel->generateTestData(100);
    
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
    m_proxyModel->setNameFilter(text);
}

void MainWindow::onCategoryFilterChanged(const QString &category)
{
    m_proxyModel->setCategoryFilter(category);
}

void MainWindow::onDateFilterChanged(const QDate &date)
{
    // 更新日期范围的开始日期
    m_proxyModel->setDateRangeFilter(date, m_dateEndFilterEdit->date());
}

void MainWindow::onDateEndFilterChanged(const QDate &date)
{
    // 更新日期范围的结束日期
    m_proxyModel->setDateRangeFilter(m_dateFilterEdit->date(), date);
}

void MainWindow::onMinPriceFilterChanged(double value)
{
    // 更新价格范围的最小值
    m_proxyModel->setPriceRangeFilter(value, m_maxPriceFilterSpin->value());
}

void MainWindow::onMaxPriceFilterChanged(double value)
{
    // 更新价格范围的最大值
    m_proxyModel->setPriceRangeFilter(m_minPriceFilterSpin->value(), value);
}

void MainWindow::onAvailableFilterChanged(int state)
{
    m_proxyModel->setAvailabilityFilter(state);
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
    // 重置所有筛选器UI控件
    m_nameFilterEdit->clear();
    m_categoryFilterCombo->setCurrentIndex(0);
    m_dateFilterEdit->setDate(QDate::currentDate().addMonths(-1));
    m_dateEndFilterEdit->setDate(QDate::currentDate());
    m_minPriceFilterSpin->setValue(0);
    m_maxPriceFilterSpin->setValue(9999.99);
    m_availableFilterCombo->setCurrentIndex(0);
    
    // 重置代理模型筛选器
    m_proxyModel->resetFilters();
    
    statusBar()->showMessage(tr("筛选器已重置"), 3000);
}

void MainWindow::onApplyFilterClicked()
{
    // 应用筛选
    m_proxyModel->applyFilters();
    
    statusBar()->showMessage(tr("筛选器已应用"), 3000);
}
