#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QColor>
#include <QFont>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QComboBox>
#include <QRandomGenerator>
#include "customlistwidget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onAddButtonClicked();
    void onRemoveButtonClicked();
    void onClearButtonClicked();
    void onItemClicked(int index);
    void onItemDoubleClicked(int index);
    void onCurrentItemChanged(int current, int previous);
    void onItemSelectionChanged();
    void onStartStopRefreshClicked();
    void onRefreshTimerTimeout();
    void onSelectionModeChanged(int index);

private:
    // 更新状态标签
    void updateStatusLabel();
    
    // 随机颜色生成
    QColor randomColor();
    
    // 随机文本生成
    QString randomText();

private:
    CustomListWidget *m_listWidget;
    
    // 控制面板
    QWidget *m_controlPanel;
    QPushButton *m_addButton;
    QPushButton *m_removeButton;
    QPushButton *m_clearButton;
    QComboBox *m_selectionModeCombo;
    
    // 刷新控制
    QCheckBox *m_randomRefreshCheck;
    QPushButton *m_startStopRefreshButton;
    QSpinBox *m_refreshIntervalSpin;
    QTimer *m_refreshTimer;
    bool m_refreshRunning;
    
    // 显示信息
    QLabel *m_statusLabel;
    QLabel *m_currentItemLabel;
    
    // 主布局
    QVBoxLayout *m_mainLayout;
    
    // 随机字符串列表，用于生成随机文本
    QStringList m_randomTexts;
};

#endif // MAINWINDOW_H
