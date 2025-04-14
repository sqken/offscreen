#include <QGuiApplication>
#include <QQmlApplicationEngine>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv); // 创建 Qt 应用程序对象

    QQmlApplicationEngine engine; // 创建 QML 引擎
    const QUrl url(QStringLiteral("qrc:/main.qml")); // 指定要加载的 QML 文件
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url); // 加载 QML 文件

    return app.exec(); // 启动 Qt 事件循环
}
