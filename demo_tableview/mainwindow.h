#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSortFilterProxyModel>
#include <QLineEdit>
#include <QComboBox>
#include <QDateEdit>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QModelIndex>
#include <QTableView>
#include <QPushButton>
#include "tablemodel.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

/**
 * @brief 主窗口类，负责UI界面的创建和交互
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // 新增记录
    void onAddItemClicked();
    
    // 删除记录
    void onRemoveItemClicked();
    
    // 保存数据
    void onSaveClicked();
    
    // 加载数据
    void onLoadClicked();
    
    // 筛选器更新
    void onFilterTextChanged(const QString &text);
    void onCategoryFilterChanged(const QString &category);
    void onDateFilterChanged(const QDate &date);
    void onPriceFilterChanged(double value);
    void onAvailableFilterChanged(int state);
    
    // 表格项双击
    void onTableItemDoubleClicked(const QModelIndex &index);
    
    // 表格数据改变
    void onTableDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
    
    // 重置筛选器
    void onResetFilterClicked();

private:
    Ui::MainWindow *ui;
    
    // 表格模型
    TableModel *m_tableModel;
    
    // 排序筛选代理模型
    QSortFilterProxyModel *m_proxyModel;
    
    // 表格视图
    QTableView *m_tableView;
    
    // 操作按钮
    QPushButton *m_addButton;
    QPushButton *m_removeButton;
    QPushButton *m_saveButton;
    QPushButton *m_loadButton;
    QPushButton *m_resetFilterButton;
    
    // 筛选组件
    QLineEdit *m_nameFilterEdit;
    QComboBox *m_categoryFilterCombo;
    QDateEdit *m_dateFilterEdit;
    QDoubleSpinBox *m_priceFilterSpin;
    QComboBox *m_availableFilterCombo;
    
    // 初始化UI
    void setupUI();
    
    // 初始化筛选器
    void setupFilters();
    
    // 更新类别过滤器
    void updateCategoryFilter();
    
    // 连接信号槽
    void connectSignals();
    
    // 创建测试数据
    void createTestData();
};
#endif // MAINWINDOW_H
