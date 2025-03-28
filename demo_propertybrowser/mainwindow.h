#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QComboBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QSpinBox>
#include <QTextEdit>
#include <QVBoxLayout>

#include "qtpropertybrowser/qteditorfactory.h"
#include "qtpropertybrowser/qtpropertymanager.h"
#include "qtpropertybrowser/qttreepropertybrowser.h"
#include "qtpropertybrowser/qtvariantproperty.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void valueChanged(QtProperty *property, const QVariant &value);
    void resetProperties();
    void updateObject();

private:
    // 创建属性表组件
    void setupPropertyBrowser();
    // 创建测试对象展示区域
    void setupObjectPreview();
    // 创建属性并添加到表中
    void addProperties();

    // UI组件
    QWidget               *m_centralWidget;
    QtTreePropertyBrowser *m_propertyBrowser;
    QGroupBox             *m_previewBox;

    // 属性管理器
    QtVariantPropertyManager *m_variantManager;
    QtVariantEditorFactory   *m_variantFactory;

    // 属性组
    QtProperty *m_basicGroup;
    QtProperty *m_appearanceGroup;
    QtProperty *m_advancedGroup;

    // 基本属性
    QtVariantProperty *m_nameProperty;
    QtVariantProperty *m_enabledProperty;
    QtVariantProperty *m_visibleProperty;
    QtVariantProperty *m_typeProperty;
    QtVariantProperty *m_intValueProperty;
    QtVariantProperty *m_doubleValueProperty;

    // 外观属性
    QtVariantProperty *m_colorProperty;
    QtVariantProperty *m_fontProperty;
    QtVariantProperty *m_sizeProperty;
    QtVariantProperty *m_positionProperty;

    // 高级属性
    QtVariantProperty *m_rectProperty;
    QtVariantProperty *m_enumProperty;
    QtVariantProperty *m_flagProperty;
    QtVariantProperty *m_dateProperty;
    QtVariantProperty *m_timeProperty;
    QtVariantProperty *m_dateTimeProperty;

    // 预览区域组件
    QLabel    *m_previewLabel;
    QTextEdit *m_previewDescription;
};

#endif // MAINWINDOW_H
