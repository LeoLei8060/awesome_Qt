#include "mainwindow.h"
#include <QApplication>
#include <QDate>
#include <QDebug>
#include <QMetaEnum>
#include <QPainter>
#include <QScreen>
#include <QTime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Qt属性编辑器示例");
    resize(900, 600);

    // 创建中央窗口
    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);

    // 创建主布局
    QHBoxLayout *mainLayout = new QHBoxLayout(m_centralWidget);

    // 设置属性编辑器
    setupPropertyBrowser();

    // 设置右侧预览区域
    setupObjectPreview();

    // 添加左右两个区域到主布局
    mainLayout->addWidget(m_propertyBrowser, 2);

    QVBoxLayout *rightLayout = new QVBoxLayout();
    rightLayout->addWidget(m_previewBox);

    // 添加重置按钮
    QPushButton *resetButton = new QPushButton("重置属性", this);
    connect(resetButton, &QPushButton::clicked, this, &MainWindow::resetProperties);
    rightLayout->addWidget(resetButton);

    mainLayout->addLayout(rightLayout, 1);

    // 添加属性
    addProperties();

    // 连接属性值变化的信号
    connect(m_variantManager,
            &QtVariantPropertyManager::valueChanged,
            this,
            &MainWindow::valueChanged);
}

MainWindow::~MainWindow()
{
    delete m_variantManager;
    delete m_variantFactory;
}

void MainWindow::setupPropertyBrowser()
{
    // 创建属性管理器
    m_variantManager = new QtVariantPropertyManager(this);

    // 创建编辑器工厂
    m_variantFactory = new QtVariantEditorFactory(this);

    // 创建树形属性编辑器
    m_propertyBrowser = new QtTreePropertyBrowser(this);
    m_propertyBrowser->setFactoryForManager(m_variantManager, m_variantFactory);
    m_propertyBrowser->setPropertiesWithoutValueMarked(true);
    m_propertyBrowser->setRootIsDecorated(true);
}

void MainWindow::setupObjectPreview()
{
    // 创建预览区域
    m_previewBox = new QGroupBox("对象预览", this);
    QVBoxLayout *previewLayout = new QVBoxLayout(m_previewBox);

    // 创建预览标签
    m_previewLabel = new QLabel(this);
    m_previewLabel->setAlignment(Qt::AlignCenter);
    m_previewLabel->setMinimumSize(200, 200);
    m_previewLabel->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);

    // 创建描述文本框
    m_previewDescription = new QTextEdit(this);
    m_previewDescription->setReadOnly(true);
    m_previewDescription->setMaximumHeight(100);

    previewLayout->addWidget(m_previewLabel);
    previewLayout->addWidget(m_previewDescription);
}

void MainWindow::addProperties()
{
    // 创建基本属性分组
    m_basicGroup = m_variantManager->addProperty(QtVariantPropertyManager::groupTypeId(),
                                                 "基本属性");

    // 名称属性
    m_nameProperty = m_variantManager->addProperty(QVariant::String, "名称");
    m_nameProperty->setValue("测试对象");
    m_basicGroup->addSubProperty(m_nameProperty);

    // 启用属性
    m_enabledProperty = m_variantManager->addProperty(QVariant::Bool, "启用");
    m_enabledProperty->setValue(true);
    m_basicGroup->addSubProperty(m_enabledProperty);

    // 可见性属性
    m_visibleProperty = m_variantManager->addProperty(QVariant::Bool, "可见");
    m_visibleProperty->setValue(true);
    m_basicGroup->addSubProperty(m_visibleProperty);

    // 类型属性（枚举）
    m_typeProperty = m_variantManager->addProperty(QtVariantPropertyManager::enumTypeId(), "类型");
    QStringList typeEnums;
    typeEnums << "矩形" << "圆形" << "三角形" << "文本";
    m_variantManager->setAttribute(m_typeProperty, "enumNames", typeEnums);
    m_typeProperty->setValue(0);
    m_basicGroup->addSubProperty(m_typeProperty);

    // 整数值属性
    m_intValueProperty = m_variantManager->addProperty(QVariant::Int, "整数值");
    m_intValueProperty->setValue(50);
    m_variantManager->setAttribute(m_intValueProperty, "minimum", 0);
    m_variantManager->setAttribute(m_intValueProperty, "maximum", 100);
    m_variantManager->setAttribute(m_intValueProperty, "singleStep", 5);
    m_basicGroup->addSubProperty(m_intValueProperty);

    // 浮点值属性
    m_doubleValueProperty = m_variantManager->addProperty(QVariant::Double, "浮点值");
    m_doubleValueProperty->setValue(25.5);
    m_variantManager->setAttribute(m_doubleValueProperty, "minimum", 0.0);
    m_variantManager->setAttribute(m_doubleValueProperty, "maximum", 100.0);
    m_variantManager->setAttribute(m_doubleValueProperty, "singleStep", 0.5);
    m_variantManager->setAttribute(m_doubleValueProperty, "decimals", 2);
    m_basicGroup->addSubProperty(m_doubleValueProperty);

    // 创建外观属性分组
    m_appearanceGroup = m_variantManager->addProperty(QtVariantPropertyManager::groupTypeId(),
                                                      "外观属性");

    // 颜色属性
    m_colorProperty = m_variantManager->addProperty(QVariant::Color, "颜色");
    m_colorProperty->setValue(QColor(Qt::blue));
    m_appearanceGroup->addSubProperty(m_colorProperty);

    // 字体属性
    m_fontProperty = m_variantManager->addProperty(QVariant::Font, "字体");
    m_fontProperty->setValue(QFont("Arial", 12));
    m_appearanceGroup->addSubProperty(m_fontProperty);

    // 尺寸属性
    m_sizeProperty = m_variantManager->addProperty(QVariant::Size, "尺寸");
    m_sizeProperty->setValue(QSize(100, 100));
    m_appearanceGroup->addSubProperty(m_sizeProperty);

    // 位置属性
    m_positionProperty = m_variantManager->addProperty(QVariant::Point, "位置");
    m_positionProperty->setValue(QPoint(50, 50));
    m_appearanceGroup->addSubProperty(m_positionProperty);

    // 创建高级属性分组
    m_advancedGroup = m_variantManager->addProperty(QtVariantPropertyManager::groupTypeId(),
                                                    "高级属性");

    // 矩形属性
    m_rectProperty = m_variantManager->addProperty(QVariant::Rect, "矩形");
    m_rectProperty->setValue(QRect(10, 10, 80, 80));
    m_advancedGroup->addSubProperty(m_rectProperty);

    // 枚举多选属性
    m_flagProperty = m_variantManager->addProperty(QtVariantPropertyManager::flagTypeId(), "标志");
    QStringList flagNames;
    flagNames << "标志1" << "标志2" << "标志3" << "标志4";
    m_variantManager->setAttribute(m_flagProperty, "flagNames", flagNames);
    m_flagProperty->setValue(5); // 1 | 4
    m_advancedGroup->addSubProperty(m_flagProperty);

    // 日期属性
    m_dateProperty = m_variantManager->addProperty(QVariant::Date, "日期");
    m_dateProperty->setValue(QDate::currentDate());
    m_advancedGroup->addSubProperty(m_dateProperty);

    // 时间属性
    m_timeProperty = m_variantManager->addProperty(QVariant::Time, "时间");
    m_timeProperty->setValue(QTime::currentTime());
    m_advancedGroup->addSubProperty(m_timeProperty);

    // 日期时间属性
    m_dateTimeProperty = m_variantManager->addProperty(QVariant::DateTime, "日期时间");
    m_dateTimeProperty->setValue(QDateTime::currentDateTime());
    m_advancedGroup->addSubProperty(m_dateTimeProperty);

    // 将所有分组添加到属性浏览器中
    m_propertyBrowser->addProperty(m_basicGroup);
    m_propertyBrowser->addProperty(m_appearanceGroup);
    m_propertyBrowser->addProperty(m_advancedGroup);

    // 更新对象预览
    updateObject();
}

void MainWindow::valueChanged(QtProperty *property, const QVariant &value)
{
    Q_UNUSED(property);
    Q_UNUSED(value);

    // 更新对象预览
    updateObject();
}

void MainWindow::resetProperties()
{
    // 重置所有属性为默认值
    m_nameProperty->setValue("测试对象");
    m_enabledProperty->setValue(true);
    m_visibleProperty->setValue(true);
    m_typeProperty->setValue(0);
    m_intValueProperty->setValue(50);
    m_doubleValueProperty->setValue(25.5);
    m_colorProperty->setValue(QColor(Qt::blue));
    m_fontProperty->setValue(QFont("Arial", 12));
    m_sizeProperty->setValue(QSize(100, 100));
    m_positionProperty->setValue(QPoint(50, 50));
    m_rectProperty->setValue(QRect(10, 10, 80, 80));
    m_flagProperty->setValue(5);
    m_dateProperty->setValue(QDate::currentDate());
    m_timeProperty->setValue(QTime::currentTime());
    m_dateTimeProperty->setValue(QDateTime::currentDateTime());

    // 更新对象预览
    updateObject();
}

void MainWindow::updateObject()
{
    // 获取当前属性值
    QString name = m_nameProperty->valueText();
    bool    enabled = m_enabledProperty->value().toBool();
    bool    visible = m_visibleProperty->value().toBool();
    int     type = m_typeProperty->value().toInt();
    int     intValue = m_intValueProperty->value().toInt();
    double  doubleValue = m_doubleValueProperty->value().toDouble();
    QColor  color = m_colorProperty->value().value<QColor>();
    QFont   font = m_fontProperty->value().value<QFont>();
    QSize   size = m_sizeProperty->value().value<QSize>();
    QPoint  position = m_positionProperty->value().value<QPoint>();

    // 更新预览标签
    m_previewLabel->setEnabled(enabled);
    m_previewLabel->setVisible(visible);

    // 创建预览图像
    QPixmap pixmap(200, 200);
    pixmap.fill(Qt::white);
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    // 绘制图形
    if (visible) {
        // 设置画笔和画刷
        QPen   pen(Qt::black, 2);
        QBrush brush(color);
        painter.setPen(pen);
        painter.setBrush(brush);

        // 根据类型绘制不同的图形
        QRect rect(position.x(), position.y(), size.width(), size.height());
        switch (type) {
        case 0: // 矩形
            painter.drawRect(rect);
            break;
        case 1: // 圆形
            painter.drawEllipse(rect);
            break;
        case 2: // 三角形
        {
            QPolygon triangle;
            triangle << QPoint(position.x() + size.width() / 2, position.y())
                     << QPoint(position.x(), position.y() + size.height())
                     << QPoint(position.x() + size.width(), position.y() + size.height());
            painter.drawPolygon(triangle);
        } break;
        case 3: // 文本
            painter.setFont(font);
            painter.drawText(rect, Qt::AlignCenter, name);
            break;
        }

        // 显示数值
        painter.setPen(Qt::black);
        // 为数值显示使用用户选择的字体，但大小设置为固定值以确保可读性
        QFont valueFont = font;
        valueFont.setPointSize(10);
        painter.setFont(valueFont);
        painter.drawText(10, 190, QString("整数值: %1").arg(intValue));
        painter.drawText(100, 190, QString("浮点值: %1").arg(doubleValue, 0, 'f', 1));
    }

    m_previewLabel->setPixmap(pixmap);

    // 更新描述文本
    QString description = QString("名称: %1\n").arg(name);
    description += QString("状态: %1\n").arg(enabled ? "启用" : "禁用");
    description += QString("可见性: %1\n").arg(visible ? "可见" : "不可见");
    description += QString("类型: %1\n").arg(m_typeProperty->valueText());
    description += QString("尺寸: %1 x %2\n").arg(size.width()).arg(size.height());
    description += QString("位置: (%1, %2)\n").arg(position.x()).arg(position.y());
    description += QString("字体: %1, %2pt").arg(font.family()).arg(font.pointSize());

    m_previewDescription->setText(description);

    // 应用字体到描述文本框，让用户可以看到字体效果
    QFont descFont = font;
    // 保持合理的字体大小，防止过大或过小
    if (descFont.pointSize() < 8)
        descFont.setPointSize(8);
    if (descFont.pointSize() > 14)
        descFont.setPointSize(14);
    m_previewDescription->setFont(descFont);
}
