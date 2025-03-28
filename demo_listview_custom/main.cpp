#include "mainwidget.h"
#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 设置应用程序信息
    QApplication::setApplicationName("自绘列表示例");
    QApplication::setOrganizationName("Awesome Qt");

    MainWidget w;
    w.show();

    return a.exec();
}
