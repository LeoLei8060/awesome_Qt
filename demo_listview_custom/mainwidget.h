#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include "customlistwidget.h"
#include <QColor>
#include <QFont>
#include <QRandomGenerator>
#include <QTimer>
#include <QWidget>

namespace Ui {
class MainWidget;
}

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = nullptr);
    ~MainWidget();

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
    Ui::MainWidget *ui;

    // 刷新控制
    QTimer *m_refreshTimer;
    bool    m_refreshRunning;

    // 随机字符串列表，用于生成随机文本
    QStringList m_randomTexts;
};

#endif // MAINWIDGET_H
