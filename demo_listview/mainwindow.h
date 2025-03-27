#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "listitemdelegate.h"
#include "listmodel.h"
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_filterLineEdit_textChanged(const QString &text);
    void on_sortComboBox_currentIndexChanged(int index);
    void on_descendingCheckBox_stateChanged(int state);
    void on_viewModeComboBox_currentIndexChanged(int index);
    void on_addButton_clicked();
    void on_editButton_clicked();
    void on_deleteButton_clicked();
    void on_actionSave_triggered();
    void on_actionExit_triggered();
    void on_listView_doubleClicked(const QModelIndex &index);

private:
    Ui::MainWindow           *ui;
    ListModel                *m_model;
    ListSortFilterProxyModel *m_proxyModel;
    ListItemDelegate         *m_delegate;

    void setupModel();
    void setupView();
    void loadInitialData();
    void saveData();
    void loadData();
    void editItem(const QModelIndex &index);
};

#endif // MAINWINDOW_H
