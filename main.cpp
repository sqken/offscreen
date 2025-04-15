#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlComponent>
#include <QQuickItem>
#include <QQuickWindow>
#include <QImage>
#include <QDir>
#include <QTimer>
#include <QDebug>
#include <QQmlContext>
#include <QFile>
#include <QQuickRenderControl>
#include <QOffscreenSurface>
#include <QDateTime>
#include <QFontDatabase>

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
        qDebug() << "set default font" << path;
        QGuiApplication::setFont(QFont(families.at(0)));
    }
}

class OffscreenRenderer : public QObject
{
    Q_OBJECT

public:
    OffscreenRenderer(QObject *parent = nullptr)
        : QObject(parent)
    {
        // 创建QML引擎
        m_engine = new QQmlEngine(this);
        
        // 检查资源文件
        QFile imageFile(":/assets_0/Component/safe_belt.png");
        if (imageFile.exists()) {
            qDebug() << "图片文件存在:" << imageFile.fileName();
        } else {
            qWarning() << "图片文件不存在:" << imageFile.fileName();
        }
        
        // 加载QML
        QQmlComponent component(m_engine, QUrl(QStringLiteral("qrc:/main.qml")));
        if (component.isError()) {
            for (const QQmlError &error : component.errors()) {
                qWarning() << "QML错误:" << error.toString();
            }
            return;
        }
        
        // 创建顶级窗口
        m_window = new QQuickWindow();
        m_window->setWidth(800);
        m_window->setHeight(600);
        m_window->setColor(Qt::white);
        
        // 创建根项目
        m_rootItem = qobject_cast<QQuickItem*>(component.create());
        if (!m_rootItem) {
            qWarning() << "创建根项目失败";
            if (component.isError()) {
                for (const QQmlError &error : component.errors()) {
                    qWarning() << "QML创建错误:" << error.toString();
                }
            }
            return;
        }
        
        qDebug() << "QML根项目创建成功";
        
        // 设置尺寸和父项
        m_rootItem->setParentItem(m_window->contentItem());
        m_rootItem->setWidth(m_window->width());
        m_rootItem->setHeight(m_window->height());
        
        // 使窗口可见但不显示
        m_window->setVisibility(QWindow::Hidden);
        
        // 创建定时器，每秒触发一次渲染
        m_timer = new QTimer(this);
        connect(m_timer, &QTimer::timeout, this, &OffscreenRenderer::render);
        m_timer->start(1000); // 每秒更新一次
        
        // 立即执行一次渲染
        QTimer::singleShot(100, this, &OffscreenRenderer::render);
    }
    
    ~OffscreenRenderer() {
        delete m_rootItem;
        delete m_window;
    }

public slots:
    void render() {
        if (!m_rootItem || !m_window) {
            qWarning() << "渲染失败：根项目或窗口为空";
            return;
        }
        
        qDebug() << "开始渲染...";
        
        // 确保窗口更新
        m_window->update();
        
        // 让QML引擎处理待处理的操作
        QCoreApplication::processEvents();
        
        // 以下是关键操作，使用QQuickWindow内置的抓取功能
        QImage image = m_window->grabWindow();
        
        // 确保图像不为空
        if (image.isNull()) {
            qWarning() << "生成的图像为空!";
            return;
        }
        
        qDebug() << "图像获取成功，大小:" << image.size() << "格式:" << image.format();
        
        // 获取当前时间作为文件名的一部分
        QString timeStr = QDateTime::currentDateTime().toString("HHmmss");
        
        // 保存为BMP文件
        QString savePath = QDir::currentPath() + "/offscreen_output.bmp";
        if (image.save(savePath, "BMP")) {
            qDebug() << "图像保存成功:" << savePath;
        } else {
            qWarning() << "保存图像失败:" << savePath;
        }
        
        // 同时保存PNG版本
        QString pngPath = QDir::currentPath() + "/offscreen_output.png";
        if (image.save(pngPath, "PNG")) {
            qDebug() << "PNG图像保存成功:" << pngPath;
        } else {
            qWarning() << "保存PNG图像失败:" << pngPath;
        }
    }
    
private:
    QQmlEngine *m_engine;
    QQuickWindow *m_window;
    QQuickItem *m_rootItem;
    QTimer *m_timer;
};

int main(int argc, char *argv[])
{
    // 启用高DPI支持
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
    
    QGuiApplication app(argc, argv);
    
    // 加载字体
    loadFont();
    
    // 获取当前目录
    qDebug() << "当前工作目录:" << QDir::currentPath();
    
    // 检查资源文件
    QDir resourceDir(":/");
    qDebug() << "资源目录内容:";
    for (const QString &entry : resourceDir.entryList()) {
        qDebug() << "  " << entry;
    }
    
    QDir assetsDir(":/assets_0");
    if (assetsDir.exists()) {
        qDebug() << "资源assets_0目录内容:";
        for (const QString &entry : assetsDir.entryList()) {
            qDebug() << "  " << entry;
        }
    } else {
        qWarning() << "资源assets_0目录不存在!";
    }
    
    // 创建离屏渲染器
    OffscreenRenderer renderer;
    
    return app.exec();
}

#include "main.moc"
