#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "tablefilterproxymodel.h"
#include "tablemodel.h"
#include <QComboBox>
#include <QDateEdit>
#include <QDoubleSpinBox>
#include <QFileDialog>
#include <QLineEdit>
#include <QMainWindow>
#include <QMessageBox>
#include <QPushButton>
#include <QStandardItemModel>
#include <QTableView>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
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
    void onDateEndFilterChanged(const QDate &date);
    void onMinPriceFilterChanged(double value);
    void onMaxPriceFilterChanged(double value);
    void onAvailableFilterChanged(int state);
    void onApplyFilterClicked(); // 新增筛选按钮处理函数

    // 表格项双击
    void onTableItemDoubleClicked(const QModelIndex &index);

    // 表格数据改变
    void onTableDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);

    // 重置筛选器
    void onResetFilterClicked();

private:
    void setupUI();
    void setupFilters();

    // 更新类别过滤器
    void updateCategoryFilter();

    // 连接信号槽
    void connectSignals();

    // 创建测试数据
    void createTestData();

private:
    Ui::MainWindow *ui;

    TableModel            *m_tableModel;
    QTableView            *m_tableView;
    TableFilterProxyModel *m_proxyModel; // 表格筛选代理模型

    // 操作按钮
    QPushButton *m_addButton;
    QPushButton *m_removeButton;
    QPushButton *m_saveButton;
    QPushButton *m_loadButton;
    QPushButton *m_resetFilterButton;
    QPushButton *m_applyFilterButton; // 新增筛选按钮

    // 筛选组件
    QLineEdit      *m_nameFilterEdit;
    QComboBox      *m_categoryFilterCombo;
    QDateEdit      *m_dateFilterEdit;
    QDateEdit      *m_dateEndFilterEdit;  // 添加日期范围终点
    QDoubleSpinBox *m_minPriceFilterSpin; // 改为最小价格
    QDoubleSpinBox *m_maxPriceFilterSpin; // 添加最大价格
    QComboBox      *m_availableFilterCombo;

    // 当前加载的文件路径
    QString m_currentFilePath;
};
#endif // MAINWINDOW_H
