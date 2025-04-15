#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QDir>
#include <QDebug>
#include <QFontDatabase>
#include "OffscreenRenderer.h"

// 加载字体函数
void loadFont() {
    QString path = ":/assets_0/Component/DriodSansFallback.ttf";
    int id = QFontDatabase::addApplicationFont(path);
    if (id == -1) {
        qWarning() << "Error loading the font:" << path;
        return;
    }
    QStringList families = QFontDatabase::applicationFontFamilies(id);
    if (!families.isEmpty()) {
        QGuiApplication::setFont(QFont(families.at(0)));
    }
}

int main(int argc, char *argv[])
{
    // 启用高DPI支持
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
    
    QGuiApplication app(argc, argv);
    
    // 加载字体
    loadFont();
    
    // 创建离屏渲染器
    OffscreenRenderer renderer;
    
    return app.exec();
}
